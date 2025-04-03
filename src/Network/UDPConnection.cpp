#include <echo/Network/NetRedefinitions.h>
#include <echo/Network/UDPConnection.h>
#include <echo/Network/DataPacket.h>
#include <echo/Network/SocketNetworkSystem.h>
#include <echo/Network/NetworkManager.h>
#include <echo/Maths/EchoMaths.h>
#include <echo/Util/StringUtils.h>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <errno.h>
#include <unistd.h>

namespace Echo
{
	UDPConnection::UDPConnection(SocketNetworkSystem& manager) : Connection(manager.GetNetworkManager()), mManager(manager)
	{
		memset(&mFromSockname, 0, sizeof (mFromSockname));
		memset(&mToSockName, 0, sizeof (mToSockName));
		SetTo("224.1.2.3");
	}

	UDPConnection::~UDPConnection() { }

	void UDPConnection::SetSocket(Socket sock)
	{
		mSocket = sock;
	}

	void UDPConnection::MakeToMatchFrom()
	{
		mToSockName = mFromSockname;
	}

	bool UDPConnection::_Connect()
	{
		if(IsConnected())
			return true;

		Socket s = -1;
		s = echo_socket(AF_INET, SOCK_DGRAM, 0);
		if(!_HandleError(s))
		{
			ECHO_LOG_ERROR("Unable to create socket");
			return false;
		}
		mToSockName.sin_family = AF_INET;
		mToSockName.sin_addr.s_addr = INADDR_ANY;

		u16 port = 0;
		bool enableBraodcast = false;
		std::vector< std::string > options;
		if(mConnectionDetails.HasAdditionalInfo())
		{
			port = mConnectionDetails.GetAdditionalInfoWithIndexFallback<u16>("port",0,0);

			if(port==0)
			{
				ECHO_LOG_ERROR("port is required");
				return false;
			}

			SetPort_(port);
			enableBraodcast = mConnectionDetails.GetAdditionalInfoWithIndexFallback<bool>("broadcast",1,false);
		}

		if(!_HandleError(echo_bind(s, (SocketAddress*) & mToSockName, sizeof (mToSockName))))
		{
			ECHO_LOG_ERROR("Unable To Bind UDP Socket on port. " << ntohs(mToSockName.sin_port));
			echo_closesocket(s);
			mManager.UpdateSocket(-1, shared_from_this());
			return false;
		}

		if(mConnectionDetails.GetAddress() == "ANY")
			mToSockName.sin_addr.s_addr = INADDR_ANY;
		else
			mToSockName.sin_addr.s_addr = inet_addr(mConnectionDetails.GetAddress().c_str());

		if(!mManager.SetSocketBlockingEnabled(s, false))
		{
			mManager.CleanSocket(s);
			mManager.UpdateSocket(-1, shared_from_this());
			return false;
		}
		mSocket = s;
		if(enableBraodcast)
		{
			EnableBroadcast();
		}
		
		SocketAddressIn socketAddress;
		socklen_t addressLength=sizeof(SocketAddressIn);
		echo_getsockname(mSocket,(sockaddr*)&socketAddress,&addressLength);

		std::stringstream friendlyName;
		friendlyName << "(Socket)passive:" << inet_ntoa(socketAddress.sin_addr) << ":" << socketAddress.sin_port;
		SetLocalConnectionDetails(ConnectionDetails(friendlyName.str()));
		
		SetState(States::CONNECTED);
		mManager.UpdateConnect(shared_from_this());
		mManager.UpdateSocket(mSocket, shared_from_this());
		return true;
	}

	bool UDPConnection::_Disconnect()
	{
		if(IsConnected())
		{
			SetState(States::DISCONNECTED);
			mManager.UpdateSocket(-1, shared_from_this());
			mManager.UpdateConnect(shared_from_this());
			mManager.CleanSocket(mSocket);
			return true;
		}
		return false;
	}

	void UDPConnection::GetLastSender(sockaddr_in *outFromSockName)
	{
		(*outFromSockName) = mFromSockname;
	}

	u16 UDPConnection::GetLastPort() const
	{
		return ntohs(mFromSockname.sin_port);
	}

	bool UDPConnection::_HandleError(int code)
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
				ECHO_LOG_ERROR("WSAEINTR - Blocking function cancelled.\n");
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
				//	ECHO_LOG_DEBUG("WSAEWOULDBLOCK - Resource temporarily unavailable.");
				//#endif
				return true;
				break;
			default:
				ECHO_LOG_ERROR("Unknown Error: " << e << " : " << std::hex << e << std::dec);
				break;
			}
			return false;
		}
	#endif
		return true;
	}

	void UDPConnection::EnableBroadcast()
	{
		int optionValue = 1;
		socklen_t optionLength = sizeof (int);
		_HandleError(echo_setsockopt(mSocket, SOL_SOCKET, SO_BROADCAST, (char*)&optionValue, optionLength));
		mToSockName.sin_addr.s_addr = INADDR_BROADCAST;
	}

	void UDPConnection::DisableBroadcast()
	{
		int optionValue = 0;
		socklen_t optionLength = sizeof (int);
		_HandleError(echo_setsockopt(mSocket, SOL_SOCKET, SO_BROADCAST, (char*)&optionValue, optionLength));
	}

	void UDPConnection::SetTo(u32 addr)
	{
		mToSockName.sin_addr.s_addr = addr;
	}

	void UDPConnection::SetTo(std::string address)
	{
		u32 addr = inet_addr(address.c_str());
		mToSockName.sin_addr.s_addr = addr;
	}

	void UDPConnection::SetPort_(u16 port)
	{
		mToSockName.sin_port = htons(port);
	}

	void UDPConnection::SetAddressFamily(u16 addressFamily)
	{
		mToSockName.sin_family = addressFamily;
	}

	void UDPConnection::SendDataPacket(shared_ptr<DataPacket> packet, std::string address)
	{
		SendDataPacket(packet, GetNetworkManager().GetAddrFromString(address));
	}

	void UDPConnection::SendDataPacket(shared_ptr<DataPacket> packet, u32 addr)
	{
		if(addr != 0)
			mToSockName.sin_addr.s_addr = addr;
		Connection::SendDataPacket(packet);
	}

	void UDPConnection::SendData(u8* data, u32 dataSize, u32 packetType, std::string address)
	{
		SendData(data, dataSize, GetNetworkManager().GetAddrFromString(address), packetType);
	}

	void UDPConnection::SendData(u8* data, u32 dataSize, u32 packetType, u32 addr)
	{
		if(addr != 0)
			mToSockName.sin_addr.s_addr = addr;
		Connection::SendData(data, dataSize, packetType);
	}

	std::string UDPConnection::GetLastSender()
	{
		std::string outString;
		outString += inet_ntoa(mFromSockname.sin_addr);
		return outString;
	}

	Connection::ReceiveResult UDPConnection::Receive(u8* buffer, int bufferSizeInBytes)
	{
		socklen_t slen = sizeof (mFromSockname);
		int code = echo_recvfrom(mSocket, (char*)buffer, bufferSizeInBytes, ECHO_SOCKET_RECV_NO_SIG_FLAGS, (struct sockaddr*)&mFromSockname, &slen);
		
	#ifdef ECHO_PLATFORM_WINDOWS
		if(code == SOCKET_ERROR)
		{
			int e = WSAGetLastError();
			switch(e)
			{
			case WSAENETDOWN:
				ECHO_LOG_ERROR("WSAENETDOWN - A Problem with the network has been encountered.\n");
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
				//	ECHO_LOG_DEBUG("WSAEWOULDBLOCK - Resource temporarily unavailable.");
				//#endif
				return ReceiveResult{0,ReceiveStatuses::WAIT};
			case WSAECONNRESET:
				break;
			default:
				ECHO_LOG_ERROR("Unknown Error: " << e << " : " << std::hex << e << std::dec);
				break;
			}
			return ReceiveResult{0,ReceiveStatuses::DISCONNECT};
		}
		return ReceiveResult{Size(code),ReceiveStatuses::SUCCESS};
	#else //ECHO_PLATFORM_LINUX and default
		if(code>0)
		{
			return ReceiveResult{Size(code),ReceiveStatuses::SUCCESS};
		}
		switch(code)
		{
		case 0:
			return ReceiveResult{0,ReceiveStatuses::DISCONNECT};
		case EAGAIN:
			ECHO_LOG_ERROR("The socket is marked non-blocking and the receive operation would block, or a receive timeout had been set and the timeout expired before data was received.");
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
	#ifdef ECHO_PLATFORM_MAC
		if(code != -1)
		{
			return ReceiveResult{Size(code),ReceiveStatuses::SUCCESS};
		}

		switch(code)
		{
		case EAGAIN:
			ECHO_LOG_ERROR("The socket is marked non-blocking, and the receive operation would block, or a receive timeout had been set, and the timeout expired before data were received.");
			break;
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
			ECHO_LOG_ERROR("The total of the iov_len values overflows a ssize_t.");
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
	#endif
	}

	Connection::SendResult UDPConnection::Send(const u8 * buffer, int numberOfBytesToSend)
	{
		int code = echo_sendto(mSocket, (const char*)buffer, numberOfBytesToSend, ECHO_SOCKET_SEND_NO_SIG_FLAGS, (const struct sockaddr*)&mToSockName, sizeof (mToSockName));
	#ifdef ECHO_PLATFORM_WINDOWS
		if(code == SOCKET_ERROR)
		{
			int e = WSAGetLastError();
			switch(e)
			{
			case WSAENETDOWN:
				ECHO_LOG_ERROR("WSAENETDOWN - A Problem with the network has been encountered.\n");
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
				//	ECHO_LOG_DEBUG("WSAEWOULDBLOCK - Resource temporarily unavailable.");
				//#endif
				mManager.EnableSocketWriteCheck(mSocket);
				return SendResult{0,SendStatuses::WAIT};
			case WSAECONNRESET:
				break;
			default:
				ECHO_LOG_ERROR("Unknown Error: " << e << " : " << std::hex << e << std::dec);
				break;
			}
			return SendResult{0,SendStatuses::DISCONNECT};
		}
		return SendResult{Size(code),SendStatuses::SUCCESS};
	#else //ECHO_PLATFORM_LINUX and default
		if(code != -1)
		{
			return SendResult{Size(code),SendStatuses::SUCCESS};
		}

		switch(errno)
		{
		case EACCES:
			ECHO_LOG_ERROR("(For Unix domain sockets, which are identified by pathname) Write permission is denied on the destination socket file, or search permission is denied for one of the directories the path prefix. (See path_resolution(2).)");
			break;
		case EAGAIN:
			//case EWOULDBLOCK:
			ECHO_LOG_ERROR("The socket is marked non-blocking and the requested operation would block.");
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
			ECHO_LOG_ERROR("The output queue for a network interface was full. This generally indicates that the interface has stopped sending, but may be caused by transient congestion. (Normally, this does not occur in Linux. Packets are just silently dropped when a device queue overflows.)");
			break;
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
	#ifdef ECHO_PLATFORM_MAC
		if(code != -1)
		{
			return SendResult{Size(code),SendStatuses::SUCCESS};
		}

		switch(errno)
		{
		case EACCES:
			ECHO_LOG_ERROR("The SO_BROADCAST option is not set on the socket and a broadcast address is given as the destination.");
			break;
		case EAGAIN:
			ECHO_LOG_ERROR("The socket is marked non-blocking and the requested operation would block.");
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
			ECHO_LOG_ERROR("Either: The system is unable to allocate an internal buffer.  The operation may succeed when buffers become available.");
			ECHO_LOG_ERROR("OR: The output queue for a network interface is full.  This generally indicates that the interface has stopped sending, but may be caused by transient congestion.");
			break;
		case ENOTSOCK:
			ECHO_LOG_ERROR("The argument socket is not a socket.");
			break;
		case EOPNOTSUPP:
			ECHO_LOG_ERROR("socket does not support (some of) the option(s) specified in flags.");
			break;
		case EPIPE:
			ECHO_LOG_ERROR("The socket is shut down for writing or the socket is connection-mode and is no \
						longer connected.  In the latter case, and if the socket is of type SOCK_STREAM, the\
						SIGPIPE signal is generated to the calling thread.");
			break;
		case EAFNOSUPPORT:
			ECHO_LOG_ERROR("Addresses in the specified address family cannot be used with this socket.");
			break;
		case EDESTADDRREQ:
			ECHO_LOG_ERROR("The socket is not connection-mode and does not have its peer address set, and no destination address is specified.");
			break;
		case EISCONN:
			ECHO_LOG_ERROR("A destination address was specified and the socket is already connected.");
			break;
		case ENOENT:
			ECHO_LOG_ERROR("A component of the pathname does not name an existing file or the path name is an empty string.");
			break;
		case ENOMEM:
			ECHO_LOG_ERROR("Insufficient memory is available to fulfill the request.");
			break;
		case ENOTCONN:
			ECHO_LOG_ERROR("The socket is connection-mode, but is not connected.");
			break;
		case ENOTDIR:
			ECHO_LOG_ERROR("A component of the path prefix of the pathname in the socket address is not a directory.");
			break;
		default:break;
		}
		return SendResult{0,SendStatuses::DISCONNECT};
	#endif
	}
}
