#include <echo/Network/NetRedefinitions.h>
#include <echo/Network/TCPConnection.h>
#include <echo/Network/DataPacket.h>
#include <echo/Network/SocketNetworkSystem.h>
#include <echo/Util/StringUtils.h>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <errno.h>
#include <unistd.h>
namespace Echo
{
	TCPConnection::TCPConnection(SocketNetworkSystem& manager, const std::string& address, u16 port, Socket s) :
		Connection(manager.GetNetworkManager()),
		mManager(manager)
	{
		memset(&mSocketAddress, 0, sizeof(SocketAddressIn));
		u32 addr;
		if(address == "ANY")
		{
			addr = INADDR_ANY;
		}else
		{
			addr = inet_addr(address.c_str());
		}
		SetPort_(htons(port));
		SetIP(addr);
		mSocket = s;
		mSocketAddress.sin_family = AF_INET;
		mSocketAddress.sin_port = htons(port);
		mSocketAddress.sin_addr.s_addr = addr;
		mListenConnection = true;
	}
	
	TCPConnection::TCPConnection(SocketNetworkSystem& manager, SocketAddressIn sockaddr, Socket s) :
		Connection(manager.GetNetworkManager()),
		mManager(manager)
	{
		memset(&mSocketAddress, 0, sizeof(SocketAddressIn));
		SetIP(sockaddr.sin_addr.s_addr);
		SetPort_(sockaddr.sin_port);
		mSocket = s;
		mSocketAddress = sockaddr;
		
		std::stringstream friendlyName;
		friendlyName << "(Socket)direct:" << inet_ntoa(sockaddr.sin_addr) << ":" << sockaddr.sin_port;
		ConnectionDetails details(friendlyName.str());
		SetConnectionDetails(details);
		mListenConnection = false;
	}
	
	TCPConnection::TCPConnection(SocketNetworkSystem& manager, const ConnectionDetails& connectionDetails) :
		Connection(manager.GetNetworkManager()),
		mManager(manager)
	{
		memset(&mSocketAddress, 0, sizeof(SocketAddressIn));
		mSocket = -1;
		SetConnectionDetails(connectionDetails);
		mListenConnection = false;
	}
	
	TCPConnection::~TCPConnection()
	{
	}

	shared_ptr<TCPConnection> TCPConnection::Accept()
	{
		if(!mListenConnection)
		{
			return nullptr;
		}

		SocketAddressIn sockaddrFrom;
		socklen_t len = sizeof(SocketAddressIn);
		memset(&sockaddrFrom, 0, len);

		Socket newSocket = echo_accept(mSocket, (SocketAddress*) &sockaddrFrom, &len);
		if(!mManager.SetSocketBlockingEnabled(newSocket, false))
		{
			ECHO_LOG_ERROR("Failed to set socket to non-blocking.");
			mManager.CleanSocket(newSocket);
			return nullptr;
		}
		shared_ptr<TCPConnection> connection(new TCPConnection(mManager, sockaddrFrom, newSocket));
		connection->SetState(Connection::States::CONNECTED);
		
		std::stringstream friendlyName;
		friendlyName << "(Socket)direct:" << inet_ntoa(sockaddrFrom.sin_addr) << ":" << sockaddrFrom.sin_port;
		ConnectionDetails details(friendlyName.str());
		connection->SetConnectionDetails(details);
		
		return connection;
	}
	
	void TCPConnection::SetIP(u32 ip)
	{
		mIP = ip;
	}

	void TCPConnection::SetPort_(u16 port)
	{
		mPort = port;
	}

	bool TCPConnection::_Connect()
	{
		if(!IsConnected() && !GetConnecting())
		{
			SetState(States::CONNECTING);
			u16 port = mSocketAddress.sin_port;

			mSocket = echo_socket(AF_INET, SOCK_STREAM, 0);
			if(!SocketNetworkSystem::HandleError(mSocket))
			{
				mSocket = -1;
				ECHO_LOG_ERROR("Unable to create socket");
				SetState(States::DISCONNECTED);
				mManager.UpdateSocket(mSocket, shared_from_this());
				return false;
			}

			if(!mManager.SetSocketBlockingEnabled(mSocket, false))
			{
				SetState(States::DISCONNECTED);
				mManager.CleanSocket(mSocket);
				mSocket = -1;
				mManager.UpdateSocket(mSocket, shared_from_this());
				return false;
			}

			std::vector< std::string > options;
			if(mConnectionDetails.HasAdditionalInfo())
			{
				port = mConnectionDetails.GetAdditionalInfoWithIndexFallback<u16>("port",0,0);

				if(port==0)
				{
					ECHO_LOG_ERROR("Unable to connect, port is required");
					SetState(States::DISCONNECTED);
					return false;
				}
			}
			u32 addr;
			if(mConnectionDetails.GetAddress() == "ANY")
			{
				addr = INADDR_ANY;
			}else
			{
				if(!SocketNetworkSystem::GetHostByName(mConnectionDetails.GetAddress(),addr))
				{
					ECHO_LOG_ERROR("Unable to resolve hostname " << mConnectionDetails.GetAddress());
					return false;
				}
			}
			SetIP(addr);

			mSocketAddress.sin_family = AF_INET;
			mSocketAddress.sin_port = htons(port);
			mSocketAddress.sin_addr.s_addr = mIP;

			ECHO_LOG_INFO("Connecting to " << mConnectionDetails.GetAddress() << "... ");
			
			if(!_HandleError(echo_connect(mSocket, (const sockaddr*)&mSocketAddress, sizeof (SocketAddressIn))))
			{
				ECHO_LOG_ERROR("ERROR initiating connection attempt.");
				SetState(States::DISCONNECTED);
				mManager.CleanSocket(mSocket);
				mSocket = -1;
				return false;
			}

			mManager.UpdateSocket(mSocket, shared_from_this());
			return true;
		}
		return false;
	}

	bool TCPConnection::_Disconnect()
	{
		if(mState!=States::DISCONNECTED)
		{
			SetState(States::DISCONNECTED);
			shared_ptr<Connection> thisConnection=shared_from_this();
			mManager.UpdateSocket(-1, thisConnection);
			mManager.CleanSocket(mSocket);
			mSocket = -1;
			mManager.UpdateConnect(thisConnection);
			return true;
		}
		return false;
	}

	void TCPConnection::_dropped()
	{
		ECHO_LOG_INFO("Disconnected: " << GetFriendlyIdentifier());
		if(mSocket != -1)
			mManager.CleanSocket(mSocket);
		Connection::_dropped();
	}

	void TCPConnection::_established()
	{
		ECHO_LOG_INFO("Connected: " << GetFriendlyIdentifier());

		SocketAddressIn socketAddress;
		socklen_t addressLength=sizeof(SocketAddressIn);
		echo_getsockname(mSocket,(sockaddr*)&socketAddress,&addressLength);

		std::stringstream friendlyName;
		friendlyName << "(Socket)direct:" << inet_ntoa(socketAddress.sin_addr) << ":" << socketAddress.sin_port;
		SetLocalConnectionDetails(ConnectionDetails(friendlyName.str()));
		
		Connection::_established();
	}

	bool TCPConnection::_HandleError(int code)
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
				//case WSAENETDOWN:
				//	ECHO_LOG_ERROR("WSAENETDOWN - A Problem with the network has been encountered.\n");
				//	break;
			case WSAEINTR:
				ECHO_LOG_ERROR("WSAEINTR - Blocking function canceled.\n");
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
				//case WSAENETUNREACH:
				//	ECHO_LOG_ERROR("WSAENETUNREACH - Network unreachable\n");
				//	break;
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
				ECHO_LOG_ERROR("WSAEINPROGRESS - A blocking Winsock call is in progress, or the service provider is still processing a callback function. ");
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
				//	ECHO_LOG_INFO("WSAEWOULDBLOCK - Resource temporarily unavailable.");
				//#endif
				return true;
				break;
			case WSAENOTSOCK:
				//ECHO_LOG_ERROR("WSAENOTSOCK - The descriptor is not a socket. ");
			case WSAENETUNREACH:
			case WSAENETDOWN:
			case WSAECONNABORTED:
			case WSAECONNRESET:
				_Disconnect();
				break;
			default:
				ECHO_LOG_ERROR("Unknown Error: " << e << " : " << std::hex << e << std::dec);
				break;
			}
			return false;
		}
		return true;
	#else //( ECHO_PLATFORM_LINUX ) || defined (ECHO_PLATFORM_MAC) and all others
		switch(code)
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
			return true;
			break;

		}
		return false;
	#endif
	}

	Connection::ReceiveResult TCPConnection::Receive(u8* buffer, int bufferSizeInBytes)
	{
		int code = echo_recv(mSocket, (char*)buffer, bufferSizeInBytes, ECHO_SOCKET_RECV_NO_SIG_FLAGS);;
		if(code == 0)
		{
			return ReceiveResult{0,ReceiveStatuses::DISCONNECT};
		}

	#ifdef ECHO_PLATFORM_WINDOWS
		if(code == SOCKET_ERROR)
		{
			int e = WSAGetLastError();
			switch(e)
			{
				//case WSAENETDOWN:
				//	ECHO_LOG_ERROR("WSAENETDOWN - A Problem with the network has been encountered.\n");
				//	break;
			case WSAEADDRNOTAVAIL:
				ECHO_LOG_ERROR("WSAEADDRNOTAVAIL - Invalid address\n");
				break;
			case WSAECONNREFUSED:
				ECHO_LOG_ERROR("WSAECONNREFUSED - Connection was refused by host\n");
				break;
			case WSAEISCONN:
				ECHO_LOG_ERROR("WSAEISCONN - Socket already connected\n");
				break;
				//case WSAENETUNREACH:
				//	ECHO_LOG_ERROR("WSAENETUNREACH - Network unreachable\n");
				//	break;
			case WSAEHOSTUNREACH:
				ECHO_LOG_ERROR("WSAEHOSTUNREACH - Host unreachable\n");
				break;
			case WSAEINVAL:
				ECHO_LOG_ERROR("WSAEINVAL - One of the specified parameters was invalid such as the window handle not referring to an existing window, or the specified socket is in an invalid state. ");
				break;
			case WSAEINPROGRESS:
				ECHO_LOG_ERROR("WSAEINPROGRESS - A blocking Winsock call is in progress, or the service provider is still processing a callback function. ");
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
				//	ECHO_LOG_INFO("WSAEWOULDBLOCK - Resource temporarily unavailable.");
				//#endif
				return ReceiveResult{0,ReceiveStatuses::WAIT};
			case WSAENETUNREACH:
			case WSAENETDOWN:
			case WSAECONNABORTED:
			case WSAECONNRESET:
				break;
			default:
				ECHO_LOG_ERROR("Unknown Error: " << e << " : " << std::hex << e << std::dec);
				break;
			}
			return ReceiveResult{0,ReceiveStatuses::DISCONNECT};
		}
		return ReceiveResult{Size(code),ReceiveStatuses::SUCCESS};
	#elif defined (ECHO_PLATFORM_MAC)
		if(code != -1)
		{
			return ReceiveResult{code,ReceiveStatuses::SUCCESS};
		}

		switch(errno)
		{
		case EAGAIN:
			//ECHO_LOG_ERROR("The socket is marked non-blocking, and the receive operation would block, or a receive timeout had been set, and the timeout expired before data were received.");
			return ReceiveResult{0,ReceiveStatuses::WAIT};
		case EBADF:
			ECHO_LOG_ERROR("The argument socket is an invalid descriptor.");
			break;
		case ECONNRESET:
			ECHO_LOG_ERROR("The connection is closed by the peer during a receive attempt on a socket.");
			break;
		case EFAULT:
			ECHO_LOG_ERROR("The receive buffer pointer(s) point outside the process's address space.");
			break;
		case EINTR:
			ECHO_LOG_ERROR("The receive was interrupted by delivery of a signal before any data were available.");
			break;
		case EINVAL:
			ECHO_LOG_ERROR("MSG_OOB is set, but no out-of-band data is available.");
			break;
		case ENOBUFS:
			ECHO_LOG_ERROR("An attempt to allocate a memory buffer fails.");
			break;
		case ENOTCONN:
			ECHO_LOG_ERROR("The socket is associated with a connection-oriented protocol and has not been connected (see connect(2) and accept(2)).");
			break;
		case ENOTSOCK:
			ECHO_LOG_ERROR("The argument socket does not refer to a socket.");
			break;
		case EOPNOTSUPP:
			ECHO_LOG_ERROR("The type and/or protocol of socket do not support the option(s) specified in flags.");
			break;
		case ETIMEDOUT:
			ECHO_LOG_ERROR("The connection timed out.");
			break;
		default:
			break;
		}
		return ReceiveResult{0,ReceiveStatuses::DISCONNECT};
	#elif defined (ECHO_PLATFORM_NINTENDO_GAMECUBE_OR_WII)
		code=-code;		//Transform the code from negative to positive.
		switch(code)
		{
		case EWOULDBLOCK:
			//ECHO_LOG_INFO("The socket is marked non-blocking and the receive operation would block, or a receive timeout had been set and the timeout expired before data was received.");
			//This should be ok to continue.
			return ReceiveResult{0,ReceiveStatuses::WAIT};
		case EBADF:
			ECHO_LOG_ERROR("The argument s is an invalid descriptor.");
			break;
		case ECONNREFUSED:
			ECHO_LOG_ERROR("A remote host refused to allow the network connection (typically because it is not running the requested service).");
			break;
		case EFAULT:
			ECHO_LOG_ERROR("The receive buffer pointer(s) point outside the process's address space.");
			break;
		case EINTR:
			ECHO_LOG_ERROR("The receive was interrupted by delivery of a signal before any data were available.");
			break;
		case EINVAL:
			ECHO_LOG_ERROR("Invalid argument passed.");
			break;
		case ENOMEM:
			ECHO_LOG_ERROR("Could not allocate memory for recvmsg().");
			break;
		case ENOTCONN:
			ECHO_LOG_ERROR("The socket is associated with a connection-oriented protocol and has not been connected (see connect(2) and accept(2)).");
			break;
		case ENOTSOCK:
			ECHO_LOG_ERROR("The argument s does not refer to a socket.");
			break;
		default:
			ECHO_LOG_ERROR("Recv error: " << code);
			break;
		}
		return ReceiveResult{0,ReceiveStatuses::DISCONNECT};
	#else //ECHO_PLATFORM_LINUX and all other platforms
		if(code > 0)
		{
			return ReceiveResult{Size(code),ReceiveStatuses::SUCCESS};
		}

		switch(errno)
		{
		case EAGAIN:
			//ECHO_LOG_ERROR("The socket is marked non-blocking and the receive operation would block, or a receive timeout had been set and the timeout expired before data was received.");
			return ReceiveResult{0,ReceiveStatuses::WAIT};
			case EBADF:
			ECHO_LOG_ERROR("The argument s is an invalid descriptor.");
			break;
		case ECONNREFUSED:
			ECHO_LOG_ERROR("A remote host refused to allow the network connection (typically because it is not running the requested service).");
			break;
		case EFAULT:
			ECHO_LOG_ERROR("The receive buffer pointer(s) point outside the process's address space.");
			break;
		case EINTR:
			ECHO_LOG_ERROR("The receive was interrupted by delivery of a signal before any data were available.");
			break;
		case EINVAL:
			ECHO_LOG_ERROR("Invalid argument passed.");
			break;
		case ENOMEM:
			ECHO_LOG_ERROR("Could not allocate memory for recvmsg().");
			break;
		case ENOTCONN:
			ECHO_LOG_ERROR("The socket is associated with a connection-oriented protocol and has not been connected (see connect(2) and accept(2)).");
			break;
		case ENOTSOCK:
			ECHO_LOG_ERROR("The argument s does not refer to a socket.");
			break;
		default:
			break;
		}
		return ReceiveResult{0,ReceiveStatuses::DISCONNECT};
	#endif
	}

	Connection::SendResult TCPConnection::Send(const u8 * buffer, int numberOfBytesToSend)
	{
		int code = echo_send(mSocket, (char*)buffer, numberOfBytesToSend, ECHO_SOCKET_SEND_NO_SIG_FLAGS);
		
	#ifdef ECHO_PLATFORM_WINDOWS
		if(code == SOCKET_ERROR)
		{
			int e = WSAGetLastError();
			switch(e)
			{
			case WSAEADDRNOTAVAIL:
				ECHO_LOG_ERROR("WSAEADDRNOTAVAIL - Invalid address");
				break;
			case WSAECONNREFUSED:
				ECHO_LOG_ERROR("WSAECONNREFUSED - Connection was refused by host");
				break;
			case WSAEISCONN:
				ECHO_LOG_ERROR("WSAEISCONN - Socket already connected");
				break;
			case WSAEHOSTUNREACH:
				ECHO_LOG_ERROR("WSAEHOSTUNREACH - Host unreachable");
				break;
			case WSAEINVAL:
				ECHO_LOG_ERROR("WSAEINVAL - One of the specified parameters was invalid such as the window handle not referring to an existing window, or the specified socket is in an invalid state.");
				break;
			case WSAEINPROGRESS:
				ECHO_LOG_ERROR("WSAEINPROGRESS - A blocking Winsock call is in progress, or the service provider is still processing a callback function.");
				break;
			case WSAENOTSOCK:
				ECHO_LOG_ERROR("WSAENOTSOCK - The descriptor is not a socket.");
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
				mManager.EnableSocketWriteCheck(mSocket);
				return SendResult{0,SendStatuses::WAIT};
			case WSAEWOULDBLOCK:
				//This one is ok :)
				//#ifdef _DEBUG
				//	ECHO_LOG_INFO("WSAEWOULDBLOCK - Resource temporarily unavailable.");
				//#endif
				mManager.EnableSocketWriteCheck(mSocket);
				return SendResult{0,SendStatuses::WAIT};
			case WSAENETUNREACH:
			case WSAENETDOWN:
			case WSAECONNABORTED:
			case WSAECONNRESET:
				if(e == WSAECONNRESET)
					ECHO_LOG_ERROR("WSAECONNRESET - The connection was reset");
				if(e == WSAENETUNREACH)
					ECHO_LOG_ERROR("WSAENETUNREACH - Network unreachable");
				if(e == WSAENETDOWN)
					ECHO_LOG_ERROR("WSAENETDOWN - A Problem with the network has been encountered.");
			default:
				ECHO_LOG_ERROR("Unknown Error: " << e << " : " << std::hex << e << std::dec);
				break;
			}
			return SendResult{0,SendStatuses::DISCONNECT};
		}
		return SendResult{Size(code),SendStatuses::SUCCESS};
	#elif defined (ECHO_PLATFORM_MAC)
		if(code > 0)
		{
			return SendResult{code,SendStatuses::SUCCESS};
		}

		switch(errno)
		{
		case EACCES:
			ECHO_LOG_ERROR("The SO_BROADCAST option is not set on the socket and a broadcast address is given as the destination.");
			break;
		case EAGAIN:
			ECHO_LOG_ERROR("The socket is marked non-blocking and the requested operation would block.");
			mManager.EnableSocketWriteCheck(mSocket);
			return SendResult{0,SendStatuses::WAIT};
		case EBADF:
			ECHO_LOG_ERROR("An invalid descriptor is specified.");
			break;
		case ECONNRESET:
			ECHO_LOG_ERROR("A connection is forcibly closed by a peer.");
			break;
		case EFAULT:
			ECHO_LOG_ERROR("An invalid user space address is specified for a parameter.");
			break;
		case EHOSTUNREACH:
			ECHO_LOG_ERROR("The destination address specifies an unreachable host.");
			break;
		case EINTR:
			ECHO_LOG_ERROR("A signal interrupts the system call before any data is transmitted.");
			break;
		case EMSGSIZE:
			ECHO_LOG_ERROR("The socket requires that message be sent atomically, and the size of the message to be sent makes this impossible.  IOV_MAX.");
			break;
		case ENETDOWN:
			ECHO_LOG_ERROR("The local network interface used to reach the destination is down.");
			break;
		case ENETUNREACH:
			ECHO_LOG_ERROR("No route to the network is present.");
			break;
		case ENOBUFS:
			mManager.EnableSocketWriteCheck(mSocket);
			ECHO_LOG_ERROR("Either: The system is unable to allocate an internal buffer.  The operation may succeed when buffers become available.");
			ECHO_LOG_ERROR("OR: The output queue for a network interface is full.  This generally indicates that the interface has stopped sending, but may be caused by transient congestion.");
			return SendResult{0,SendStatuses::WAIT};
		case ENOTSOCK:
			ECHO_LOG_ERROR("The argument socket is not a socket.");
			break;
		case EOPNOTSUPP:
			ECHO_LOG_ERROR("socket does not support (some of) the option(s) specified in flags.");
			break;
		case EPIPE:
			ECHO_LOG_ERROR("The socket is shut down for writing or the socket is connection-mode and is no \
							longer connected.  In the latter case, and if the socket is of type SOCK_STREAM, the \
							SIGPIPE signal is generated to the calling thread.");
			break;
		case EDESTADDRREQ:
			ECHO_LOG_ERROR("The socket is not connection-mode and no peer address is set.");
			break;
		case ENOTCONN:
			ECHO_LOG_ERROR("The socket is not connected or otherwise has not had the peer pre-specified.");
			break;
		default:
			break;
		}
		
		return SendResult{0,SendStatuses::DISCONNECT};
	#elif defined (ECHO_PLATFORM_NINTENDO_GAMECUBE_OR_WII)
		if(code >= 0)
		{
			return SendResult{Size(code),SendStatuses::SUCCESS};
		}
		code=-code;		//Transform the code from negative to positive.
		switch(errno)
		{
		case EACCES:
			ECHO_LOG_ERROR("(For Unix domain sockets, which are identified by pathname) Write permission is denied on the destination socket file, or search permission is denied for one of the directories the path prefix. (See path_resolution(2).)");
			break;
		//case EAGAIN:
		case EWOULDBLOCK:
			//This is ok, we'll just try again when write becomes available.
			//ECHO_LOG_INFO("The socket is marked non-blocking and the requested operation would block.");
			mManager.EnableSocketWriteCheck(mSocket);
			return SendResult{0,SendStatuses::WAIT};
		case EBADF:
			ECHO_LOG_ERROR("An invalid descriptor was specified.");
			break;
		case ECONNRESET:
			ECHO_LOG_ERROR("Connection reset by peer.");
			break;
		case EDESTADDRREQ:
			ECHO_LOG_ERROR("The socket is not connection-mode, and no peer address is set.");
			break;
		case EFAULT:
			ECHO_LOG_ERROR("An invalid user space address was specified for a parameter.");
			break;
		case EINTR:
			ECHO_LOG_ERROR("A signal occurred before any data was transmitted.");
			break;
		case EINVAL:
			ECHO_LOG_ERROR("Invalid argument passed.");
			break;
		case EISCONN:
			ECHO_LOG_ERROR("The connection-mode socket was connected already but a recipient was specified. (Now either this error is returned, or the recipient specification is ignored.)");
			break;
		case EMSGSIZE:
			ECHO_LOG_ERROR("The socket type requires that message be sent atomically, and the size of the message to be sent made this impossible.");
			break;
		case ENOBUFS:
			mManager.EnableSocketWriteCheck(mSocket);
			ECHO_LOG_ERROR("The output queue for a network interface was full. This generally indicates that the interface has stopped sending, but may be caused by transient congestion. (Normally, this does not occur in Linux. Packets are just silently dropped when a device queue overflows.)");
			return SendResult{0,SendStatuses::WAIT};
		case ENOMEM:
			ECHO_LOG_ERROR("No memory available.");
			break;
		case ENOTCONN:
			ECHO_LOG_ERROR("The socket is not connected, and no target has been given.");
			break;
		case ENOTSOCK:
			ECHO_LOG_ERROR("The argument s is not a socket.");
			break;
		case EOPNOTSUPP:
			ECHO_LOG_ERROR("Some bit in the flags argument is inappropriate for the socket type.");
			break;
		case EPIPE:
			ECHO_LOG_ERROR("The local end has been shut down on a connection oriented socket. In this case the process will also receive a SIGPIPE unless MSG_NOSIGNAL is set.");
			break;
		default:
			break;
		}
		return SendResult{0,SendStatuses::DISCONNECT};
	#else //ECHO_PLATFORM_LINUX and all other platforms (assuming default.)
		if(code >= 0)
		{
			return SendResult{Size(code),SendStatuses::SUCCESS};
		}

		switch(errno)
		{
		case EACCES:
			ECHO_LOG_ERROR("(For Unix domain sockets, which are identified by pathname) Write permission is denied on the destination socket file, or search permission is denied for one of the directories the path prefix. (See path_resolution(2).)");
			break;
		//case EAGAIN:
		case EWOULDBLOCK:
			//This is ok, we'll just try again.
			//ECHO_LOG_ERROR("The socket is marked non-blocking and the requested operation would block.");
			mManager.EnableSocketWriteCheck(mSocket);
			return SendResult{0,SendStatuses::WAIT};
		case EBADF:
			ECHO_LOG_ERROR("An invalid descriptor was specified.");
			break;
		case ECONNRESET:
			ECHO_LOG_ERROR("Connection reset by peer.");
			break;
		case EDESTADDRREQ:
			ECHO_LOG_ERROR("The socket is not connection-mode, and no peer address is set.");
			break;
		case EFAULT:
			ECHO_LOG_ERROR("An invalid user space address was specified for a parameter.");
			break;
		case EINTR:
			ECHO_LOG_ERROR("A signal occurred before any data was transmitted.");
			break;
		case EINVAL:
			ECHO_LOG_ERROR("Invalid argument passed.");
			break;
		case EISCONN:
			ECHO_LOG_ERROR("The connection-mode socket was connected already but a recipient was specified. (Now either this error is returned, or the recipient specification is ignored.)");
			break;
		case EMSGSIZE:
			ECHO_LOG_ERROR("The socket type requires that message be sent atomically, and the size of the message to be sent made this impossible.");
			break;
		case ENOBUFS:
			mManager.EnableSocketWriteCheck(mSocket);
			ECHO_LOG_ERROR("The output queue for a network interface was full. This generally indicates that the interface has stopped sending, but may be caused by transient congestion. (Normally, this does not occur in Linux. Packets are just silently dropped when a device queue overflows.)");
			return SendResult{0,SendStatuses::WAIT};
		case ENOMEM:
			ECHO_LOG_ERROR("No memory available.");
			break;
		case ENOTCONN:
			ECHO_LOG_ERROR("The socket is not connected, and no target has been given.");
			break;
		case ENOTSOCK:
			ECHO_LOG_ERROR("The argument s is not a socket.");
			break;
		case EOPNOTSUPP:
			ECHO_LOG_ERROR("Some bit in the flags argument is inappropriate for the socket type.");
			break;
		case EPIPE:
			ECHO_LOG_ERROR("The local end has been shut down on a connection oriented socket. In this case the process will also receive a SIGPIPE unless MSG_NOSIGNAL is set.");
			break;
		default:
			break;
		}
		return SendResult{0,SendStatuses::DISCONNECT};
	#endif
	}
}
