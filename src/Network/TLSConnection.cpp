#include <echo/Network/NetRedefinitions.h>
#include <echo/Network/TLSConnection.h>
#include <echo/Network/DataPacket.h>
#include <echo/Network/SocketNetworkSystem.h>
#include <echo/Util/StringUtils.h>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <errno.h>
#include <unistd.h>

namespace Echo
{
	
	TLSConnection::TLSConnection(SocketNetworkSystem& manager, const ConnectionDetails& connectionDetails, u16 port, Socket s) : Connection(manager.GetNetworkManager()), mManager(manager)
	{
		memset(&mSocketAddress, 0, sizeof(SocketAddressIn));
		std::string address = connectionDetails.GetAddress();
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
		mReadWriteTLSContext = nullptr;
		mTLSContext = nullptr;
		SetConnectionDetails(connectionDetails);
		mListenConnection = true;
		mNeedsClientTLSUpgrade = false;
		mNeedsHandshake = true;
		
	}
	
	TLSConnection::TLSConnection(SocketNetworkSystem& manager, SocketAddressIn sockaddr, Socket s) : Connection(manager.GetNetworkManager()), mManager(manager)
	{
		memset(&mSocketAddress, 0, sizeof(SocketAddressIn));
		SetIP(sockaddr.sin_addr.s_addr);
		SetPort_(sockaddr.sin_port);
		mSocket = s;
		mSocketAddress = sockaddr;
		mReadWriteTLSContext = nullptr;
		mListenConnection = false;
		mNeedsClientTLSUpgrade = false;
		mNeedsHandshake = true;
		mTLSContext = nullptr;
		
		std::stringstream friendlyName;
		friendlyName << "(Socket)direct:" << inet_ntoa(sockaddr.sin_addr) << ":" << sockaddr.sin_port;
		ConnectionDetails details(friendlyName.str());
		SetConnectionDetails(details);
	}

	TLSConnection::TLSConnection(SocketNetworkSystem& manager, const ConnectionDetails& connectionDetails) :
		Connection(manager.GetNetworkManager()),
		mManager(manager),
		mTLSContext(nullptr),
		mReadWriteTLSContext(nullptr)
	{
		memset(&mSocketAddress, 0, sizeof(SocketAddressIn));
		mSocket = -1;
		SetConnectionDetails(connectionDetails);
		mListenConnection = false;
		mNeedsClientTLSUpgrade = false;
		mNeedsHandshake = true;
	}
	
	TLSConnection::~TLSConnection()
	{
		CleanTLS();
	}

	shared_ptr<TLSConnection> TLSConnection::Accept()
	{
		if(!mListenConnection)
		{
			return nullptr;
		}
		SocketAddressIn sockaddrFrom;
		socklen_t len = sizeof(SocketAddressIn);
		memset(&sockaddrFrom, 0, len);

		Socket newSocket = echo_accept(mSocket, (SocketAddress*) &sockaddrFrom, &len);
		
		shared_ptr<TLSConnection> connection(new TLSConnection(mManager, sockaddrFrom, newSocket));
		if(!connection->UpgradeServer(GetConnectionDetails()))
		{
			ECHO_LOG_ERROR("Unable to upgrade to TLS connection.");
			return nullptr;
		}

		// All okay!
		connection->SetState(Connection::States::CONNECTED);
		return connection;
	}
	
	bool TLSConnection::UpgradeServer(const ConnectionDetails& connectionDetails)
	{
		if(mTLSContext)
		{
			return false;
		}
		
		if(!mManager.SetSocketBlockingEnabled(mSocket, false))
		{
			return false;
		}
		
		mTLSContext = tls_server();
		
		if(!Configure(mTLSContext, connectionDetails))
		{
			ECHO_LOG_ERROR("Failed to configure TLS server context");
			return false;
		}

		// https://man.openbsd.org/tls_init.3
		// A server can accept a new client connection by calling tls_accept_socket(3) on an already established socket connection.
		if (tls_accept_socket(mTLSContext,&mReadWriteTLSContext,mSocket) == -1)
		{
			ECHO_LOG_ERROR("Failed to accept TLS connection" << tls_error(mTLSContext));
			return false;
		}
		
		mNeedsHandshake = true;

		mManager.UpdateSocket(mSocket, shared_from_this());
		mManager.EnableSocketWriteCheck(mSocket);
		
		return true;
	}
	
	bool TLSConnection::Configure(tls* tlsContext, const ConnectionDetails& connectionDetails)
	{
		tls_config* tlsConfig = tls_config_new();
		if(!tlsConfig)
		{
			ECHO_LOG_ERROR("Failed to create tls_config");
			return false;
		}
		bool allSuccess = true;
		
		std::string caFile = connectionDetails.GetAdditionalInfo<std::string>("ca","");
		if(!caFile.empty())
		{
			if(tls_config_set_ca_file(tlsConfig,caFile.c_str()) == -1)
			{
				ECHO_LOG_ERROR("Failed to set CA file: " << tls_config_error(tlsConfig));
				allSuccess = false;
			}
		}
		
		std::string privateKeyFile = connectionDetails.GetAdditionalInfo<std::string>("privateKey","");
		
		if(!privateKeyFile.empty())
		{
			if(tls_config_set_key_file(tlsConfig,privateKeyFile.c_str()) == -1)
			{
				ECHO_LOG_ERROR("Failed to set private key file: " << privateKeyFile << " Error: " << tls_config_error(tlsConfig));
				allSuccess = false;
			}
		}
		
		std::string publicCertFile = connectionDetails.GetAdditionalInfo<std::string>("publicCert","");
		if(!publicCertFile.empty())
		{
			if(tls_config_set_cert_file(tlsConfig,publicCertFile.c_str()) == -1)
			{
				ECHO_LOG_ERROR("Failed to set public key file: " << publicCertFile << " Error: " << tls_config_error(tlsConfig));
				allSuccess = false;
			}
		}
		
		if (tls_configure(tlsContext, tlsConfig) == -1)
		{
			ECHO_LOG_ERROR("Failed to configure TLS server context: " << tls_error(mTLSContext));
			ECHO_LOG_ERROR("\tAdditional Info");
			ECHO_LOG_ERROR("\t\tcaFile: " << caFile);
			ECHO_LOG_ERROR("\t\tprivateKey: " << privateKeyFile);
			ECHO_LOG_ERROR("\t\tpublicCert: " << publicCertFile);
			allSuccess = false;
		}

		tls_config_free(tlsConfig);
		return allSuccess;
	}

	void TLSConnection::CleanTLS()
	{
		if(mReadWriteTLSContext)
		{
			tls_close(mReadWriteTLSContext);
			mReadWriteTLSContext = nullptr;
		}
		
		if(mTLSContext)
		{
			tls_close(mTLSContext);
			mTLSContext = nullptr;
		}
		mNeedsHandshake = true;
	}

	void TLSConnection::SetIP(u32 ip)
	{
		mIP = ip;
	}

	void TLSConnection::SetPort_(u16 port)
	{
		mPort = port;
	}

	bool TLSConnection::_Connect()
	{
		if(!(IsConnected()))
		{
			CleanTLS();
			
			SetState(States::CONNECTING);

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

			u16 port = mConnectionDetails.GetAdditionalInfoWithIndexFallback("port", 0, 0);
			if(port == 0)
			{
				SetState(States::DISCONNECTED);
				CleanTLS();
				return false;
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
					CleanTLS();
					return false;
				}
			}
			SetIP(addr);

			mSocketAddress.sin_family = AF_INET;
			mSocketAddress.sin_port = htons(port);
			mSocketAddress.sin_addr.s_addr = mIP;

			ECHO_LOG_INFO("Connecting " << mConnectionDetails.ToString() << "... ");
			
			if(!_HandleError(echo_connect(mSocket, (const sockaddr*)&mSocketAddress, sizeof (SocketAddressIn))))
			{
				ECHO_LOG_ERROR("ERROR initiating connection attempt.");
				SetState(States::DISCONNECTED);
				mManager.CleanSocket(mSocket);
				mSocket = -1;
				CleanTLS();
				return false;
			}

			mNeedsClientTLSUpgrade = true;
			mNeedsHandshake = true;

			mManager.UpdateSocket(mSocket, shared_from_this());
			mManager.EnableSocketWriteCheck(mSocket);
			return true;
		}
		return false;
	}

	bool TLSConnection::_Disconnect()
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

	void TLSConnection::_dropped()
	{
		ECHO_LOG_INFO("Disconnected: " << GetFriendlyIdentifier());
		if(mSocket != -1)
		{
			mManager.CleanSocket(mSocket);
		}
		Connection::_dropped();
	}

	void TLSConnection::_established()
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

	bool TLSConnection::_HandleError(int code)
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
	
	bool TLSConnection::UpgradeClient()
	{
		if ((mReadWriteTLSContext = tls_client()) == NULL)
		{
			ECHO_LOG_ERROR("Failed to create TLS client context");
			return false;
		}

		if(!Configure(mReadWriteTLSContext, mConnectionDetails))
		{
			ECHO_LOG_ERROR("Failed to configure TLS context when attempting to connect");
			CleanTLS();
			return false;
		}
		
		std::string commonName = mConnectionDetails.GetAdditionalInfo("commonName", mConnectionDetails.GetAddress());

		if (tls_connect_socket(mReadWriteTLSContext, mSocket, commonName.c_str()) == -1)
		{
			ECHO_LOG_ERROR("Failed to upgrade socket to TLS socket." << tls_error(mReadWriteTLSContext));
			CleanTLS();
			return false;
		}
		
		return true;
	}
	
	Connection::ReceiveStatus TLSConnection::ReceivePackets()
	{
		// Not ideal but _established might be called after the first Receive() due to a design issue that probably should be resolved one day.
		if(mNeedsClientTLSUpgrade)
		{
			if(!UpgradeClient())
			{
				// The upgrade call performs logging
				return ReceiveStatuses::DISCONNECT;
			}
			mNeedsClientTLSUpgrade = false;
		}
		if(mNeedsHandshake)
		{
			int handshakeResult = tls_handshake(mReadWriteTLSContext);
			if(handshakeResult==0)
			{
				mNeedsHandshake = false;
				ECHO_LOG_INFO("TLS Handshake succeeded: " << GetFriendlyIdentifier());
			}else
			{
				if(handshakeResult==TLS_WANT_POLLIN || handshakeResult==TLS_WANT_POLLOUT)
				{
					//ECHO_LOG_DEBUG("tls_handshake: TLS_WANT_POLLIN || TLS_WANT_POLLOUT : " << handshakeResult << " : " << this);
					mManager.EnableSocketWriteCheck(mSocket);
					return ReceiveStatuses::WAIT;
				}
				ECHO_LOG_ERROR("tls_handshake failed with: " << handshakeResult << " : " << this);
				return ReceiveStatuses::DISCONNECT;
			}
		}
		return Connection::ReceivePackets();
	}
	
	Connection::SendStatus TLSConnection::SendPackets(bool reenable)
	{
		// Not ideal but _established might be called after the first Receive() due to a design issue that probably should be resolved one day.
		if(mNeedsClientTLSUpgrade)
		{
			if(!UpgradeClient())
			{
				// The upgrade call performs logging
				return SendStatuses::DISCONNECT;
			}
			mNeedsClientTLSUpgrade = false;
		}
		if(mNeedsHandshake)
		{
			int handshakeResult = tls_handshake(mReadWriteTLSContext);
			if(handshakeResult==0)
			{
				mNeedsHandshake = false;
				ECHO_LOG_INFO("TLS Handshake succeeded: " << GetFriendlyIdentifier());
			}else
			{
				if(handshakeResult==TLS_WANT_POLLIN || handshakeResult==TLS_WANT_POLLOUT)
				{
					//ECHO_LOG_DEBUG("tls_handshake: TLS_WANT_POLLIN || TLS_WANT_POLLOUT : " << handshakeResult << " : " << this);
					mManager.EnableSocketWriteCheck(mSocket);
					return SendStatuses::WAIT;
				}
				ECHO_LOG_ERROR("tls_handshake failed with: " << handshakeResult << " : " << this);
				return SendStatuses::DISCONNECT;
			}
		}
		return Connection::SendPackets(true);
	}

	Connection::ReceiveResult TLSConnection::Receive(u8* buffer, int bufferSizeInBytes)
	{
		int bytesRead = TLS_WANT_POLLIN;
		bytesRead = tls_read(mReadWriteTLSContext, (char*)buffer, bufferSizeInBytes);
		if(bytesRead >= 0)
		{
			return ReceiveResult{static_cast<Size>(bytesRead),ReceiveStatuses::SUCCESS};
		}
		if(bytesRead==TLS_WANT_POLLIN || bytesRead==TLS_WANT_POLLOUT)
		{
			//ECHO_LOG_DEBUG("tls_read: TLS_WANT_POLLIN || TLS_WANT_POLLOUT: " << bytesRead);
			mManager.EnableSocketWriteCheck(mSocket);
			return ReceiveResult{0,ReceiveStatuses::WAIT};
		}
		ECHO_LOG_ERROR("Failed to tls_read " << tls_error(mReadWriteTLSContext));
		return ReceiveResult{0,ReceiveStatuses::DISCONNECT};
	}

	Connection::SendResult TLSConnection::Send(const u8 * buffer, int numberOfBytesToSend)
	{
		// Not ideal but _established might be called after the first Send() due to a design issue that probably should be resolved one day.
		if(mNeedsClientTLSUpgrade)
		{
			if(!UpgradeClient())
			{
				// The upgrade call performs logging
				return SendResult{0,SendStatuses::DISCONNECT};
			}
			mNeedsClientTLSUpgrade = false;
		}
		
		int bytesSent = TLS_WANT_POLLOUT;
		bytesSent = tls_write(mReadWriteTLSContext, (char*)buffer, numberOfBytesToSend);
		if(bytesSent >= 0)
		{
			return SendResult{static_cast<Size>(bytesSent),SendStatuses::SUCCESS};
		}
		if(bytesSent==TLS_WANT_POLLIN || bytesSent==TLS_WANT_POLLOUT)
		{
			//ECHO_LOG_DEBUG("tls_write: TLS_WANT_POLLIN || TLS_WANT_POLLOUT: " << bytesSent);
			mManager.EnableSocketWriteCheck(mSocket);
			return SendResult{0,SendStatuses::WAIT};
		}
		ECHO_LOG_ERROR("Failed to tls_write " << tls_error(mReadWriteTLSContext));
		return SendResult{0,SendStatuses::DISCONNECT};
	}
}
