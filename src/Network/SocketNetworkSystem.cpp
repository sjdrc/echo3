#include <echo/Network/NetRedefinitions.h>
#include <echo/Types.h>
#include <echo/Network/SocketNetworkSystem.h>
#include <echo/Network/TCPConnection.h>
#ifdef ECHO_SSL_SUPPORT
#include <echo/Network/TLSConnection.h>
#endif
#include <echo/Network/UDPConnection.h>
#include <echo/Util/StringUtils.h>
#include <echo/Kernel/ScopedLock.h>
#include <boost/lexical_cast.hpp>
#ifdef ECHO_PLATFORM_LINUX
#include <echo/Platforms/Linux/SocketNetworkPlatformDetail.h>
#endif
#ifdef ECHO_PLATFORM_NINTENDO_GAMECUBE_OR_WII
#include <echo/Platforms/Wii/SocketNetworkPlatformDetail.h>
#endif
#ifdef ECHO_PLATFORM_WINDOWS
#include <echo/Platforms/Windows/SocketNetworkPlatformDetail.h>
#endif
#include <iostream>
#include <sstream>
#include <errno.h>
#include <unistd.h>
#include <algorithm>

namespace Echo
{
	//////////////////////////////////////////////////////////////////////////
	//SocketNetworkSystem::SocketThreadTask::SocketThreadTask
	//////////////////////////////////////////////////////////////////////////

	SocketNetworkSystem::SocketThreadTask::SocketThreadTask(SocketNetworkSystem& s, Size maximumSockets, int pollTimeInMS) :
		mCount(0),
		mMaximumSockets(maximumSockets),
		mPollTimeInMS(pollTimeInMS),
		mSystem(s)
	{
	}

	SocketNetworkSystem::SocketThreadTask::~SocketThreadTask()
	{
		mCount = 0;
	}

	bool SocketNetworkSystem::SocketThreadTask::AddSocket(Socket s)
	{
		ScopedLock locky(mPendingSocketsMutex);
		if(mCount == mMaximumSockets)
		{
			return false;
		}
		// Only add it if we haven't already
		if( mSocketToPollFDMap.find(s)!=mSocketToPollFDMap.end() ||
			mPendingAddSockets.find(s)!=mPendingAddSockets.end())
		{
			ECHO_LOG_ERROR("Socket already added");
			return false;
		}
		mPendingAddSockets.insert(s);
		// Make sure we don't remove it if we requested to do so before
		mPendingRemovalSockets.erase(s);
		mPollUpdateRequired = true;
		mCount++;
		return true;
	}

	bool SocketNetworkSystem::SocketThreadTask::RemoveSocket(Socket s)
	{
		ScopedLock locky(mPendingSocketsMutex);
		if( mSocketToPollFDMap.find(s)==mSocketToPollFDMap.end() ||
			mPendingRemovalSockets.find(s)!=mPendingRemovalSockets.end())
		{
			ECHO_LOG_INFO("Socket already marked for removal");
			return false;
		}
		mPendingRemovalSockets.insert(s);
		// Make sure we don't add it if we requested to do so before
		mPendingAddSockets.erase(s);
		mCount--;
		mPollUpdateRequired = true;
		return true;
	}

	void SocketNetworkSystem::SocketThreadTask::EnableWriteCheck(Socket s)
	{
		ScopedLock locky(mPendingSocketsMutex);

		if(mSocketToPollFDMap.find(s)==mSocketToPollFDMap.end())
		{
			ECHO_LOG_ERROR("Cannot enable write check on socket - not added to worker");
			return;
		}
		
		if(mPendingRemovalSockets.find(s)!=mPendingRemovalSockets.end())
		{
			ECHO_LOG_ERROR("Cannot enable write check on socket - marked for removal");
			return;
		}
		
		if(mPendingWriteEnableSockets.find(s)==mPendingWriteEnableSockets.end())
		{
			mPendingWriteEnableSockets.insert(s);
			mPollUpdateRequired = true;
		}
	}

	void SocketNetworkSystem::SocketThreadTask::DisableWriteCheck(Socket s)
	{
		ScopedLock locky(mPendingSocketsMutex);
		
		if(mSocketToPollFDMap.find(s)==mSocketToPollFDMap.end())
		{
			ECHO_LOG_ERROR("Cannot disable write check on socket - not added to worker");
			return;
		}

		if(mPendingRemovalSockets.find(s)!=mPendingRemovalSockets.end())
		{
			ECHO_LOG_ERROR("Cannot disable write check on socket - marked for removal");
			return;
		}
		
		//Make sure the same socket isn't added twice.
		if(mPendingWriteDisableSockets.find(s)==mPendingWriteDisableSockets.end())
		{
			mPendingWriteDisableSockets.insert(s);
			mPollUpdateRequired = true;
		}
	}
	
	void SocketNetworkSystem::SocketThreadTask::InternalAddSocket(Socket s)
	{
		auto it = mSocketToPollFDMap.find(s);
		if(it!=mSocketToPollFDMap.end())
		{
			ECHO_LOG_ERROR("Socket book keeping error on add sockets");
			return;
		}
		
		pollfd pfd;
		pfd.fd = s;
		#ifdef ECHO_PLATFORM_WINDOWS
			// Windows doesn't support everything, see https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-wsapoll
			pfd.events = POLLIN | POLLOUT;// | POLLERR | POLLHUP | POLLNVAL;
		#else
			pfd.events = POLLIN | POLLOUT | POLLPRI | POLLERR | POLLHUP | POLLNVAL;
		#endif
		pfd.revents = 0;
		mPollFDs.push_back(pfd);
		mSocketToPollFDMap[s] = mPollFDs.size()-1;
	}
	
	void SocketNetworkSystem::SocketThreadTask::InternalRemoveSocket(Socket s)
	{
		auto it = mSocketToPollFDMap.find(s);
		if(it==mSocketToPollFDMap.end())
		{
			ECHO_LOG_ERROR("Socket book keeping error on removal sockets");
			return;
		}
		
		Size numberOfFDs = mPollFDs.size();
		if(it->second >= numberOfFDs)
		{
			ECHO_LOG_ERROR("Socket book keeping error on removal sockets. Invalid index.");
			return;
		}

		if(numberOfFDs>1)
		{
			// Swap this index with the end
			std::swap(mPollFDs[it->second],mPollFDs[numberOfFDs-1]);
		}

		// Drop the end
		mPollFDs.resize(numberOfFDs-1);

		// We need to rebuild mSocketToPollFDMap because the indexes will have changed
		// This is easier to maintain than tracking the book keeping properly
		mSocketToPollFDMap.clear();
		numberOfFDs = mPollFDs.size();
		for(Size i = 0; i < numberOfFDs; ++i)
		{
			mSocketToPollFDMap[mPollFDs[i].fd] = i;
		}
	}

	void SocketNetworkSystem::SocketThreadTask::InternalWriteDisable(Socket s)
	{
		auto it = mSocketToPollFDMap.find(s);
		if(it==mSocketToPollFDMap.end())
		{
			ECHO_LOG_ERROR("Socket book keeping error on write disable sockets");
			return;
		}
		
		Size numberOfFDs = mPollFDs.size();
		if(it->second >= numberOfFDs)
		{
			ECHO_LOG_ERROR("Socket book keeping error on write disable sockets. Invalid index.");
			return;
		}

		auto& pfd = mPollFDs[it->second];
		pfd.events = pfd.events & ~POLLOUT;
	}
	
	void SocketNetworkSystem::SocketThreadTask::InternalWriteEnable(Socket s)
	{
		auto it = mSocketToPollFDMap.find(s);
		if(it==mSocketToPollFDMap.end())
		{
			ECHO_LOG_ERROR("Socket book keeping error on write enable sockets");
			return;
		}
		
		Size numberOfFDs = mPollFDs.size();
		if(it->second >= numberOfFDs)
		{
			ECHO_LOG_ERROR("Socket book keeping error on write enable sockets. Invalid index.");
			return;
		}
		auto& pfd = mPollFDs[it->second];
		pfd.events = pfd.events | POLLOUT;
	}

	void SocketNetworkSystem::SocketThreadTask::Update(Seconds lastFrameTime)
	{
		if(!mPollFDs.empty())
		{
			int pollReturn = echo_net_poll(mPollFDs.data(), mPollFDs.size(), mPollTimeInMS);
			//ECHO_LOG_INFO("Post poll");

			if(pollReturn > 0)
			{
				for(auto& pfd : mPollFDs)
				{
					if(pfd.revents & POLLNVAL)
					{
						//ECHO_LOG_INFO("ExceptNotify");
						mSystem.ExceptNotify(pfd.fd);
						RemoveSocket(pfd.fd);
					} else
					{
						//We'll process read first as this will allow us to detect bad connections.
						if(pfd.revents & (POLLIN | POLLHUP | POLLERR | POLLPRI))
						{
							// Read handling should detect that the socket is closed
							//ECHO_LOG_INFO("ReadNotify");
							mSystem.ReadNotify(pfd.fd);
						}
						
						if(pfd.revents & POLLOUT)
						{
							//ECHO_LOG_INFO("WriteNotify " << pfd.fd);
							DisableWriteCheck(pfd.fd); //For unix systems that continually report write,
							//it is re-enabled in connection write error
							mSystem.WriteNotify(pfd.fd);
						}
					}
					//Check for any connecting timeouts.
					mSystem.CheckForTimeout(pfd.fd);
				}
			} else
			#ifdef ECHO_PLATFORM_WINDOWS
			{
				HandleError(pollReturn);
			}
			#else
				if(pollReturn < 0)
				{
					switch(errno)
					{
						case EFAULT:
						{
							ECHO_LOG_ERROR("Poll indicated the array given as argument was not contained in the calling program's address space.");
						}break;
						case EINTR:
						{
							ECHO_LOG_ERROR("Poll indicated a signal occurred before any requested event.");
						}break;
						case EINVAL:
						{
							ECHO_LOG_ERROR("Poll indicated the nfds value exceeds the RLIMIT_NOFILE value. Value: " << mPollFDs.size());
						}break;
						case ENOMEM:
						{
							ECHO_LOG_ERROR("Poll indicated there was no space to allocate file descriptor tables.");
						}break;
						default:
							ECHO_LOG_ERROR("Unknown poll error: poll returned " << pollReturn << " errno:" << errno);
							break;
					}
				}
			#endif
		}

		if(mPollUpdateRequired)
		{
			ScopedLock locky(mPendingSocketsMutex);
			mPollUpdateRequired = false;

			//Remove any sockets that are pending removal
			for(auto& s : mPendingRemovalSockets)
			{
				InternalRemoveSocket(s);
			}
			mPendingRemovalSockets.clear();

			//Remove any write sockets that are pending write notification removal
			for(auto& s : mPendingWriteDisableSockets)
			{
				InternalWriteDisable(s);
			}
			mPendingWriteDisableSockets.clear();

			//Add any write sockets that are pending writing notifications
			for(auto& s : mPendingWriteEnableSockets)
			{
				InternalWriteEnable(s);
			}
			mPendingWriteEnableSockets.clear();

			for(auto& s : mPendingAddSockets)
			{
				InternalAddSocket(s);
			}
			mPendingAddSockets.clear();
			
			if(mPollFDs.empty())
			{
				TaskManager* manager = GetSingleTaskManager();
				if(manager)
				{
					manager->RemoveAllTasks();
				}else
				{
					ECHO_LOG_ERROR("Socket Task either doesn't have a TaskManager or more than one TaskManager is managing it. This should not happen");
				}
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	SocketNetworkSystem::SocketNetworkSystem(NetworkManager& networkManager, Size maximumSocketsPerThread, int pollTimeInMS) : NetworkSystem("Socket", networkManager)
	{
		mStarted = false;
		// Use system maximum.
		if(maximumSocketsPerThread==0)
		{
			mMaximumSocketsPerThread = FD_SETSIZE;
		}else
		{
			mMaximumSocketsPerThread = maximumSocketsPerThread;
		}
		mPollTimeInMS = pollTimeInMS;
	}

	SocketNetworkSystem::~SocketNetworkSystem()
	{
		CleanUp();
	}

	bool SocketNetworkSystem::Initialise()
	{
		mPlatformDetail = SocketNetworkPlatformDetail::Create();
		return (mPlatformDetail!=nullptr);
	}

	bool SocketNetworkSystem::Start()
	{
		ScopedLock locky(mSocketThreadTasksMutex);
		//We'll just start threads when we need to
		for(auto& t : mSocketThreadTasks)
		{
			t->mThread->Execute();
		}
		mStarted = true;
		return true;
	}
	
	void SocketNetworkSystem::DisconnectAll()
	{
		// Create a copy of the connections list since a connect request can affect
		// the list and we can't hold the lock while attempting to disconnect. A
		// disconnect will not do anything the connection is already disconnected.
		mConnectionsMutex.Lock();
		std::map< Socket, shared_ptr<Connection> > connections = mConnections;
		mConnectionsMutex.Unlock();
		for(auto it : connections)
		{
			it.second->Disconnect();
		}
	}

	void SocketNetworkSystem::CleanUp()
	{
		DisconnectAll();
		std::list< shared_ptr<ThreadTaskPair> > threads;
		{
			ScopedLock locky(mSocketThreadTasksMutex);
			threads = mSocketThreadTasks;
			mSocketThreadTasks.clear();
		}
		for(auto t : threads)
		{
			t->mThread->SetThreadCompleteCallback(Thread::ThreadCompleteCallback());
			t->mThread->Terminate(true);
		}
		mStarted = false;
		mPlatformDetail.reset();
	}

	void SocketNetworkSystem::GetSupportedConnectionTypes(std::vector< std::string >& outSupportedConnectionTypes)
	{
		outSupportedConnectionTypes.push_back("passive");
		outSupportedConnectionTypes.push_back("direct");
	}

	void SocketNetworkSystem::GetAdditionalConnectionInformationFormat(std::vector< std::string >& outAdditionalInformation)
	{
		outAdditionalInformation.push_back("port,[BROADCAST]");
		outAdditionalInformation.push_back("port,[security=tls,ca=fileName],[privateKey=fileName],[publicCert=fileName]");
	}

	bool SocketNetworkSystem::GetInterfaceInformation(std::vector<SocketNetworkSystem::InterfaceInformation>& outInterfaceInformation)
	{
		InterfaceInformation interfaceInformation;
		char ac[80];
		if(echo_gethostname(ac, sizeof (ac)) == -1)
		{
			return false;
		}
		interfaceInformation.mHostname = ac;

		struct hostent *phe = gethostbyname(ac);
		if(phe == 0)
		{
			return false;
		}

		for(int i = 0; phe->h_addr_list[i] != 0; ++i)
		{
			interfaceInformation.mIPAddress = inet_ntoa(*reinterpret_cast<struct in_addr*>(&phe->h_addr_list[i]));
			//cout << "Address " << i << ": " << inet_ntoa(addr) << endl;
			//Determine the subnet mask

			outInterfaceInformation.push_back(interfaceInformation);
		}

		return 0;
	}

	bool SocketNetworkSystem::AcceptConnection(Socket s)
	{
		IncomingConnectionListener* listener = 0;
		mConnectionsMutex.Lock();
		std::map< Socket, IncomingConnectionListener* >::iterator it = mIncomingConnectionListeners.find(s);

		if(it != mIncomingConnectionListeners.end())
		{
			listener = it->second;
		}

		shared_ptr<Connection> listeningConnection;
		std::map< Socket, shared_ptr<Connection> >::iterator lit = mConnections.find(s);
		if(lit!=mConnections.end())
		{
			listeningConnection = lit->second;
		}
		mConnectionsMutex.Unlock();

		if(!listener || !listeningConnection)
		{
			ECHO_LOG_ERROR("Received incoming connection request but a listener cannot be found for the connection");
			return false;
		}

		// This could be made nicer if we abstracted TCP and TLS connection with a SocketConnection
		Socket newSocket;
		shared_ptr<Connection> connection;
		#ifdef ECHO_SSL_SUPPORT
		shared_ptr<TLSConnection> secureConnection = dynamic_pointer_cast<TLSConnection>(listeningConnection);
		if(secureConnection)
		{
			shared_ptr<TLSConnection> newConnection = secureConnection->Accept();
			if(newConnection)
			{
				newSocket = newConnection->GetSocket();
				connection = newConnection;
			}else
			{
				ECHO_LOG_ERROR("Unhandled TLSConnection accept");
				return false;
			}
		}else
		#endif
		{
			shared_ptr<TCPConnection> tcpConnection = dynamic_pointer_cast<TCPConnection>(listeningConnection);
			if(tcpConnection)
			{
				// Regular connection
				shared_ptr<TCPConnection> newConnection = tcpConnection->Accept();
				if(newConnection)
				{
					newSocket = newConnection->GetSocket();
					connection = newConnection;
				}else
				{
					ECHO_LOG_ERROR("Unhandled TCPConnection accept");
					return false;
				}
			}else
			{
				ECHO_LOG_ERROR("Connection type unknown by SocketNetworkSystem to accept connections on. Please check the implementation.");
				return false;
			}
		}

		// Only if a known connection type establishes.
		if(connection)
		{
			UpdateSocket(newSocket, connection);
			UpdateIncoming(connection, listener);
		}
		return true;
	}

	void SocketNetworkSystem::CheckForTimeout(Socket s)
	{
		mConnectionsMutex.Lock();
		std::map< Socket, shared_ptr<Connection> >::iterator it = mConnections.find(s);
		if(it == mConnections.end())
		{
			//This is not an error because the connection may have disconnected between read or write processing and checking for a timeout.
			mConnectionsMutex.Unlock();
			return;
		}
		shared_ptr<Connection> connection = it->second;
		mConnectionsMutex.Unlock();

		if(connection->GetConnecting())
		{
			connection->_Disconnect();
		}
	}
	
	bool SocketNetworkSystem::IsSocketConnected(Socket s)
	{
		mConnectionsMutex.Lock();
		std::map< Socket, shared_ptr<Connection> >::iterator it = mConnections.find(s);
		if(it != mConnections.end())
		{
			return it->second->IsConnected();
		}
		mConnectionsMutex.Unlock();
		return false;
	}
	
	void SocketNetworkSystem::ReadNotify(Socket s)
	{
		mConnectionsMutex.Lock();
		std::map< Socket, shared_ptr<Connection> >::iterator it = mConnections.find(s);
		if(it == mConnections.end())
		{
			mConnectionsMutex.Unlock();
			return;
		}
		shared_ptr<Connection> connection = it->second;
		mConnectionsMutex.Unlock();

		if(!connection->IsConnected() && !connection->GetConnecting())
		{
			//Attempt to accept a new incoming connection
			if(AcceptConnection(s))
			{
				return;
			}
		}
		UpdateReceive(connection);
	}

	void SocketNetworkSystem::WriteNotify(Socket s)
	{
		mConnectionsMutex.Lock();
		std::map< Socket, shared_ptr<Connection> >::iterator it = mConnections.find(s);
		if(it == mConnections.end())
		{
			//This is not an error because the connection may have disconnected between read processing and write processing.
			mConnectionsMutex.Unlock();
			return;
		}
		shared_ptr<Connection> connection = it->second;
		mConnectionsMutex.Unlock();
		
		if(connection->GetConnecting())
		{
			connection->SetState(Connection::States::CONNECTED);
			UpdateConnect(connection);
		}

		UpdateSend(connection);
	}

	void SocketNetworkSystem::ExceptNotify(Socket s)
	{
		mConnectionsMutex.Lock();
		std::map< Socket, shared_ptr<Connection> >::iterator it = mConnections.find(s);
		if(it == mConnections.end())
		{
			ECHO_LOG_WARNING("SocketNetworkSystem::ExceptNotify(): Socket not associated with a connection.");
			mConnectionsMutex.Unlock();
			return;
		}
		// Exception can occur on TCP streams when out of band data is available for reading. This is
		// apparently very rare and often not used. Based on that, we'll disconnect the connection in
		// case of an attempt to find an exploit.
		it->second->Disconnect();
		UpdateConnect(it->second);
		mConnectionsMutex.Unlock();
	}

	/** Returns true on success, or false if there was an error */
	bool SocketNetworkSystem::SetSocketBlockingEnabled(int fd, bool blocking)
	{
		if(fd < 0)
		{
			return false;
		}

	#ifdef WIN32
		unsigned long mode = blocking ? 0 : 1;
		return (echo_net_ioctl(fd, FIONBIO, &mode) == 0) ? true : false;
	#elif defined (ECHO_PLATFORM_NINTENDO_WII)
		//This is adapted from the net_ioctl() implementation. The network_wii code includes the following define:
		//		#define IOS_O_NONBLOCK 0x04 //(O_NONBLOCK >> 16) - it's in octal representation, so this shift
		//		leads to 0 and hence nonblocking sockets didn't work. changed it to the right value.
		// So I've hard coded this value in here.
		u32 flags = net_fcntl(fd, F_GETFL, 0);
		const u32 IOS_O_NONBLOCK=0x4;
		flags &= ~IOS_O_NONBLOCK;
		if (!blocking)
		{
			flags |= IOS_O_NONBLOCK;
		}
		return (net_fcntl(fd, F_SETFL, flags)==0);
	#else
		int flags = echo_net_fcntl(fd, F_GETFL, 0);
		if(flags < 0)
		{
			return false;
		}
		flags = blocking ? (flags&~O_NONBLOCK) : (flags | O_NONBLOCK);
		return (echo_net_fcntl(fd, F_SETFL, flags) == 0) ? true : false;
	#endif
	}
	
	bool SocketNetworkSystem::HandleError(int code)
	{
	#ifdef ECHO_PLATFORM_WINDOWS
		if(code == SOCKET_ERROR)
		{
			int e = WSAGetLastError();
			switch(e)
			{
			case WSANOTINITIALISED:
				ECHO_LOG_ERROR("WSANOTINITIALISED - application will now close.\n");
				PostQuitMessage(1);
				break;
			case WSAENETDOWN:
				ECHO_LOG_ERROR("WSAENETDOWN - A Problem with the network has been encountered.\n");
				break;
			case WSAEINTR:
				ECHO_LOG_ERROR("WSAEINTR - BLocking function canceled.\n");
				break;
			case WSAEADDRNOTAVAIL:
				ECHO_LOG_ERROR("WSAEADDRNOTAVAIL - Invalid address\n");
				break;
			case WSAECONNREFUSED:
				ECHO_LOG_ERROR("WSAECONNREFUSED - Connection was refused by host\n");
				break;
			case WSAEISCONN:
				ECHO_LOG_ERROR("WSAEISCONN - Socket already connected\n");
				break;
			case WSAENETUNREACH:
				ECHO_LOG_ERROR("WSAENETUNREACH - Network unreachable\n");
				break;
			case WSAEHOSTUNREACH:
				ECHO_LOG_ERROR("WSAEHOSTUNREACH - Host unreachable\n");
				break;
			case WSAETIMEDOUT:
				ECHO_LOG_ERROR("WSAETIMEDOUT - Attempt to connect timed out\n");
				break;
			case WSAEINVAL:
				ECHO_LOG_ERROR("WSAEINVAL - One of the specified parameters was invalid such as the window handle not referring to an existing window, or the specified socket is in an invalid state. ");
				break;
			case WSAEINPROGRESS:
				ECHO_LOG_ERROR("WSAEINPROGRESS - A bLocking Winsock call is in progress, or the service provider is still processing a callback function. ");
				break;
			case WSAENOTSOCK:
				ECHO_LOG_ERROR("WSAENOTSOCK - The descriptor is not a socket. ");
				break;
			case WSAEADDRINUSE:
				ECHO_LOG_ERROR("WSAEADDRINUSE - Address already in use");
				break;
			case WSAENOTCONN:
				ECHO_LOG_ERROR("WSAENOTCONN - Socket not connected");
				break;
			case WSAEAFNOSUPPORT:
				ECHO_LOG_ERROR("WSAEAFNOSUPPORT - Address family not supported by protocol family");
				break;
			case WSAENOBUFS:
				ECHO_LOG_ERROR("WSAENOBUFS - No buffer space available. An operation on a socket could not be performed because the system lacked sufficient buffer space or because a queue was full.");
				break;
			case WSAEWOULDBLOCK:
				//This one is ok :)
				//#ifdef _DEBUG
				//	ECHO_LOG_WARNING("WSAEWOULDBLOCK - Resource temporarily unavailable.");
				//#endif
				return true;
				break;
			default:
				ECHO_LOG_ERROR("Unknown Error: " << e << " : " << std::hex << e << std::dec);
				break;
			}
			return false;
		}
		return true;
	#elif defined (ECHO_PLATFORM_MAC)
		if(code != -1)
			return true;
		switch(errno)
		{
		case EBADF:
			ECHO_LOG_ERROR("The socket argument is not a valid file descriptor.");
			break;
		case EDESTADDRREQ:
			ECHO_LOG_ERROR("The socket is not bound to a local address, and the protocol does not support listening on an unbound socket.");
			break;
		case EINVAL:
			ECHO_LOG_ERROR("The socket is already connected or has been shut down");
			break;
		case ENOTSOCK:
			ECHO_LOG_ERROR("The socket argument does not refer to a socket.");
			break;
		case EOPNOTSUPP:
			ECHO_LOG_ERROR("The socket protocol does not support listen()");
			break;
		case EACCES:
			ECHO_LOG_ERROR("Permission to create a socket of the specified type and/or protocol is denied.");
			break;
		case EAFNOSUPPORT:
			ECHO_LOG_ERROR("The specified address family is not supported.");
			break;
		case EISCONN:
			ECHO_LOG_ERROR("The per-process descriptor table is full.");
			break;
		case EMFILE:
			ECHO_LOG_ERROR("The per-process descriptor table is full.");
			break;
		case ENFILE:
			ECHO_LOG_ERROR("The system file table is full.");
			break;
		case ENOBUFS:
			ECHO_LOG_ERROR("Insufficient buffer space is available.  The socket cannot be created until sufficient resources are freed.");
			break;
		case ENOMEM:
			ECHO_LOG_ERROR("Insufficient memory was available to fulfill the request.");
			break;
		case EPROTONOSUPPORT:
			ECHO_LOG_ERROR("The protocol type or the specified protocol is not supported within this domain.");
			break;
		case EPROTOTYPE:
			ECHO_LOG_ERROR("The socket type is not supported by the protocol.");
			break;
		default:
			return true;
			break;
		}
	#else //ECHO_PLATFORM_LINUX and all others (assuming nix)
		if(code >= 0)
			return true;
		switch(errno)
		{
		case EACCES:
			ECHO_LOG_ERROR("Permission to create a socket of the specified type and/or protocol is denied.");
			break;
		case EAFNOSUPPORT:
			ECHO_LOG_ERROR("The implementation does not support the specified address family.");
			break;
		case EINVAL:
			ECHO_LOG_ERROR("Unknown protocol, or protocol family not available.");
			break;
		case EMFILE:
			ECHO_LOG_ERROR("Process file table overflow.");
			break;
		case ENFILE:
			ECHO_LOG_ERROR("The system limit on the total number of open files has been reached.");
			break;
		case ENOBUFS:
		case ENOMEM:
			ECHO_LOG_ERROR("Insufficient memory is available. The socket cannot be created until sufficient resources are freed.");
			break;
		case EPROTONOSUPPORT:
			ECHO_LOG_ERROR("The protocol type or the specified protocol is not supported within this domain.");
			break;
		default:
			return false;
			break;
		}
	#endif
		return false;
	}

	void SocketNetworkSystem::CleanSocket(Socket s)
	{
		int bOptVal = 1;
		int bOptLen = sizeof (int);
		if(!HandleError(echo_setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char*)&bOptVal, bOptLen)))
		{
			ECHO_LOG_WARNING("SocketNetworkSystem::CleanSocket(): Could not set socket for reuse");
		}
		echo_closesocket(s);
	}

	void SocketNetworkSystem::EnableSocketWriteCheck(Socket s)
	{
		std::map< Socket, shared_ptr<ThreadTaskPair> >::iterator sttit = mSocketThreadTaskConnectionLookup.find(s);
		if(sttit != mSocketThreadTaskConnectionLookup.end())
		{
			sttit->second->mSocketThreadTask->EnableWriteCheck(s);
		}
	}

	void SocketNetworkSystem::DisableSocketWriteCheck(Socket s)
	{
		std::map< Socket, shared_ptr<ThreadTaskPair> >::iterator sttit = mSocketThreadTaskConnectionLookup.find(s);
		if(sttit != mSocketThreadTaskConnectionLookup.end())
		{
			sttit->second->mSocketThreadTask->DisableWriteCheck(s);
		}
	}
	
	void SocketNetworkSystem::OnThreadTaskComplete(shared_ptr<ThreadTaskPair> taskPair)
	{
		ScopedLock locky(mSocketThreadTasksMutex);
		mSocketThreadTasks.remove(taskPair);
		taskPair->mThread->Stop();
		for(auto it=mSocketThreadTaskConnectionLookup.begin();it!=mSocketThreadTaskConnectionLookup.end();)
		{
			if(it->second==taskPair)
			{
				auto thisIt = it;
				it++;
				mSocketThreadTaskConnectionLookup.erase(thisIt);
			}else
			{
				it++;
			}
		}
		mThreadTasksToCleanup.push_back(taskPair);
		ECHO_LOG_INFO("mSocketThreadTasks " << mSocketThreadTasks.size() << " mSocketThreadTaskConnectionLookup " << mSocketThreadTaskConnectionLookup.size() << " taskPair " << taskPair.get() << " taskPair.use_count() " << taskPair.use_count());
	}

	bool SocketNetworkSystem::AssignThreadTask(Socket s)
	{
		if(s==-1)
		{
			return false;
		}
		
		ScopedLock locky(mSocketThreadTasksMutex);

		// Clean up from any previous tasks
		for(auto& taskPair : mThreadTasksToCleanup)
		{
			taskPair->mThread->SetThreadCompleteCallback(Thread::ThreadCompleteCallback());
		}
		for(auto& taskPair : mThreadTasksToCleanup)
		{
			taskPair->mThread->Join();
		}
		mThreadTasksToCleanup.clear();
		
		shared_ptr<ThreadTaskPair> taskPair = FindAvailableSocketThreadTask();

		if(taskPair)
		{
			// Did it lie to us?
			if(taskPair->mSocketThreadTask->AddSocket(s))
			{
				return true;
			}
			ECHO_LOG_WARNING("FindAvailableSocketThreadTask() returned a task that is not available. This shouldn't happen. I will start up a new task.");
		}
		
		taskPair.reset(new ThreadTaskPair());
		taskPair->mSocketThreadTask.reset(new SocketThreadTask(*this, mMaximumSocketsPerThread, mPollTimeInMS));
		taskPair->mThread.reset(new TaskThread("SocketNetworkSystem worker"));
			
		taskPair->mThread->AddTask(taskPair->mSocketThreadTask);
		taskPair->mThread->SetThreadCompleteCallback(std::bind(&SocketNetworkSystem::OnThreadTaskComplete,this,taskPair));
		if(!taskPair->mSocketThreadTask->AddSocket(s))
		{
			ECHO_LOG_ERROR("Unable to assign socket to new worker thread");
			return false;
		}

		mSocketThreadTasks.push_back(taskPair);
		mSocketThreadTaskConnectionLookup[s] = taskPair;
		if(mStarted)
		{
			taskPair->mThread->Execute();
		}
		
		return true;
	}

	bool SocketNetworkSystem::Listen(IncomingConnectionListener* listener, const ConnectionDetails& connectionDetails)
	{
		if(!connectionDetails.HasAddress() || !connectionDetails.HasAdditionalInfo())
		{
			ECHO_LOG_INFO("Socket listening requires an address (can be ANY) and a port to be specified, using port=number or just the port number first");
			return false;
		}
		//Type defaults to Direct
		//Type

		u16 port = connectionDetails.GetAdditionalInfoWithIndexFallback<u16>("port",0,0);
		if(port==0)
		{
			return false;
		}

		std::string connectionType;
		if(connectionDetails.HasType())
		{
			connectionType = connectionDetails.GetType();
		} else
		{
			connectionType = "direct";
		}
		if(connectionType == "passive")
		{
			//Setup the UDP Socket
			shared_ptr<UDPConnection> udpConnection(new UDPConnection(*this));
			udpConnection->SetConnectionDetails(connectionDetails);
			udpConnection->SetPort_(port);
			udpConnection->Connect();
			if(!UpdateSocket(udpConnection->mSocket,udpConnection))
			{
				ECHO_LOG_ERROR("SocketNetworkSystem::Listen(): unable to queue UDP socket for checking.");
				return false;
			}
			return true;
		} else
		if(connectionType == "direct")
		{
			#ifdef ECHO_SSL_SUPPORT
			bool isSecure = false;
			std::string security = connectionDetails.GetAdditionalInfo<std::string>("security","");
			
			if(security=="tls")
			{
				isSecure = true;
			}
			#endif
			Socket s = echo_socket(AF_INET, SOCK_STREAM, 0);
			if(!HandleError(s))
			{
				ECHO_LOG_ERROR("Unable to create socket");
				return false;
			}

			// Enable address reuse
			int on = 1;
			if(!HandleError(echo_setsockopt( s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) )))
			{
				ECHO_LOG_WARNING("Unable to flag the socket for resuse, re-binding later may fail.");
			}

			if(!SetSocketBlockingEnabled(s, false))
			{
				ECHO_LOG_ERROR("setting socket to non-bLocking");
				CleanSocket(s);
				return false;
			}
			
			shared_ptr<Connection> connection;
			SocketAddress* socketAddress;
			#ifdef ECHO_SSL_SUPPORT
			if(isSecure)
			{
				shared_ptr<TLSConnection> tlsConnection(new TLSConnection(*this, connectionDetails, port, s));
				connection = tlsConnection;
				socketAddress = reinterpret_cast<SocketAddress*>(&tlsConnection->GetSocketAddress());
			}else
			#endif
			{
				shared_ptr<TCPConnection> tcpConnection(new TCPConnection(*this, connectionDetails.GetAddress(),port,s));
				connection = tcpConnection;
				socketAddress = reinterpret_cast<SocketAddress*>(&tcpConnection->GetSocketAddress());
			}
			
			if(!HandleError(echo_bind(s, socketAddress, sizeof(SocketAddressIn))))
			{
				ECHO_LOG_ERROR("Unable To Bind Listen Socket on port. " << port);
				echo_closesocket(s);
				return false;
			}

			if(!HandleError(echo_listen(s, 0)))
			{
				ECHO_LOG_ERROR("setting socket for listening.");
				return false;
			}

			ECHO_LOG_INFO("Listening on port: " << port);

			mConnectionsMutex.Lock();
			mIncomingConnectionListeners[s] = listener;
			mConnectionsMutex.Unlock();
			if(!UpdateSocket(s,connection))
			{
				ECHO_LOG_ERROR("SocketNetworkSystem::Listen(): unable to queue socket for checking.");
				return false;
			}
			return true;
		}
		
		ECHO_LOG_ERROR("SocketNetworkSystem::Listen(): Unsupported connection type. '" << connectionDetails.GetType() << "'");
		return false;
	}

	shared_ptr<Connection> SocketNetworkSystem::Connect(const ConnectionDetails& connectionDetails, Connection::ConnectCallback connectionCallback, Connection::DisconnectCallback disconnectCallback)
	{
		if(!connectionDetails.HasAddress() || !connectionDetails.HasAdditionalInfo())
		{
			return shared_ptr<Connection>();
		}
		//Type defaults to Direct
		//Type
		shared_ptr<Connection> connection;

		u16 port = connectionDetails.GetAdditionalInfoWithIndexFallback("port", 0, 0);
		if(port == 0)
		{
			ECHO_LOG_INFO("Socket connections need at minimum a port.");
			return nullptr;
		}

		std::string connectionType;
		if(connectionDetails.HasType())
		{
			connectionType = connectionDetails.GetType();
		} else
		{
			connectionType = "direct";
		}
		if(connectionType == "passive")
		{
			if(port == 0)
			{
				return shared_ptr<Connection>();
			}
			shared_ptr<UDPConnection> udpConnection(new UDPConnection(*this));
			udpConnection->SetConnectionDetails(connectionDetails);
			udpConnection->RegisterConnectCallback("",connectionCallback);
			udpConnection->RegisterDisconnectCallback("",disconnectCallback);
			udpConnection->Connect();
			connection = udpConnection;
		} else
		if(connectionType == "direct")
		{
			#ifdef ECHO_SSL_SUPPORT
			std::string security = connectionDetails.GetAdditionalInfo<std::string>("security", "");
			if(security == "tls")
			{
				shared_ptr<TLSConnection> tlsConnection(new TLSConnection(*this, connectionDetails));
				tlsConnection->RegisterConnectCallback("",connectionCallback);
				tlsConnection->RegisterDisconnectCallback("",disconnectCallback);
				tlsConnection->Connect();
				connection = tlsConnection;
			}else
			#endif
			{			
				shared_ptr<TCPConnection> tcpConnection(new TCPConnection(*this, connectionDetails));
				tcpConnection->RegisterConnectCallback("",connectionCallback);
				tcpConnection->RegisterDisconnectCallback("",disconnectCallback);
				tcpConnection->Connect();
				connection = tcpConnection;
			}
		} else
		{
			ECHO_LOG_ERROR("SocketNetworkSystem::Listen(): Unsupported connection type. '" << connectionDetails.GetType() << "'");
			return shared_ptr<Connection>();
		}
		return connection;
	}
	
	shared_ptr<SocketNetworkSystem::ThreadTaskPair> SocketNetworkSystem::FindAvailableSocketThreadTask()
	{
		for(auto& it : mSocketThreadTasks)
		{
			if(it->mSocketThreadTask->HasCapacity())
			{
				return it;
			}
		}
		return nullptr;
	}

	bool SocketNetworkSystem::UpdateSocket(Socket s, shared_ptr<Connection> connection)
	{
		Socket originalSocket = -1;
		mConnectionsMutex.Lock();
		//Update the connection lookup and find the original socket
		std::map< Socket, shared_ptr<Connection> >::iterator it = mConnections.begin();
		std::map< Socket, shared_ptr<Connection> >::iterator itEnd = mConnections.end();
		while(it != itEnd)
		{
			if(it->second == connection)
			{
				originalSocket = it->first;
				mConnections.erase(it);
				break;
			}
			++it;
		}
		if(s != -1)
		{
			mConnections[s] = connection;
		}
		mConnectionsMutex.Unlock();

		//Find Thread Task that has the connection and update it
		std::map< Socket, shared_ptr<ThreadTaskPair> >::iterator sttit = mSocketThreadTaskConnectionLookup.find(originalSocket);
		if(sttit != mSocketThreadTaskConnectionLookup.end())
		{
			sttit->second->mSocketThreadTask->RemoveSocket(originalSocket);
			if(s != -1)
			{
				mSocketThreadTaskConnectionLookup[s]=sttit->second;
				sttit->second->mSocketThreadTask->AddSocket(s);
			}
			mSocketThreadTaskConnectionLookup.erase(sttit);
		} else
		{
			if(s == -1)
			{
				// This is deliberate
				return true;
			}

			if(!AssignThreadTask(s))
			{
				ECHO_LOG_ERROR("Failed to add the socket to the socket task. This socket will not be readable or writable.");
				return false;
			}
		}
		return true;
	}
	
	bool SocketNetworkSystem::GetHostByName(const std::string& hostnameOrIP, u32& output)
	{
#if defined(ECHO_WINSOCKS_NETWORKING) || defined(ECHO_POSIX_NETWORKING)
		struct addrinfo* dnsResult = nullptr;
		int e = getaddrinfo(hostnameOrIP.c_str(),NULL,NULL,&dnsResult);
		if(e != 0)
		{
			ECHO_LOG_ERROR("getaddrinfo returned: " << e);
		}else
		{
			if(dnsResult)
			{
				//Is it an IPv4?
				if((dnsResult->ai_protocol==IPPROTO_TCP ||
					dnsResult->ai_protocol==IPPROTO_IP ||	// "Dummy protocol for TCP" see /usr/include/netinet/in.h
					dnsResult->ai_protocol==IPPROTO_UDP) &&
						dnsResult->ai_addrlen==sizeof(SocketAddressIn))
				{
					struct sockaddr_in* inSockAddr = reinterpret_cast<struct sockaddr_in*>(dnsResult->ai_addr);
					output = inSockAddr->sin_addr.s_addr;
					freeaddrinfo(dnsResult);
					return true;
				}else
				{
					if(dnsResult->ai_protocol!=IPPROTO_TCP || dnsResult->ai_protocol!=IPPROTO_UDP)
					{
						ECHO_LOG_ERROR("ai_protocol!=IPPROTO_TCP || ai_protocol!=IPPROTO_UDP: " << dnsResult->ai_protocol);
					}
					if(dnsResult->ai_addrlen!=sizeof(SocketAddressIn))
					{
						ECHO_LOG_ERROR("ai_addrlen!=sizeof(SocketAddressIn): " << sizeof(SocketAddressIn));
					}
				}
			}else
			{
				ECHO_LOG_ERROR("getaddrinfo: dnsResult is null ");
			}
		}
		if(dnsResult)
		{
			freeaddrinfo(dnsResult);
		}
		
#elif defined(ECHO_WII_NETWORKING)
		// There is a function net_gethostbyname() but when I tested it the system froze.
		// This will do for IP addresses.
		output = inet_addr(hostnameOrIP.c_str());
		return output!=0;
#endif
		return false;
	}
}
