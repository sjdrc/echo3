#include <echo/Network/NetRedefinitions.h>
#include <echo/Types.h>
#include <echo/Network/WebSocketNetworkSystem.h>
#include <echo/Network/WebSocketConnection.h>
#include <echo/Util/StringUtils.h>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <sstream>
#include <algorithm>

namespace Echo
{
	//////////////////////////////////////////////////////////////////////////

	WebSocketNetworkSystem::WebSocketNetworkSystem(NetworkManager& networkManager) : NetworkSystem("WebSocket", networkManager)
	{
		mStarted = false;
	}

	WebSocketNetworkSystem::~WebSocketNetworkSystem()
	{
		CleanUp();
	}

	bool WebSocketNetworkSystem::Initialise()
	{
		// We don't need to perform any initialisation.
		return true;
	}

	bool WebSocketNetworkSystem::Start()
	{
		std::vector< shared_ptr<Thread> >::iterator it = mThreads.begin();
		std::vector< shared_ptr<Thread> >::iterator itEnd = mThreads.end();
		while(it != itEnd)
		{
			(*it)->Execute();
			++it;
		}
		mStarted = true;
		return true;
	}
	
	void WebSocketNetworkSystem::DisconnectAll()
	{
		// Create a copy of the connections list since a connect request can affect
		// the list and we can't hold the lock while attempting to disconnect. A
		// disconnect will not do anything the connection is already disconnected.
		mConnectionsMutex.Lock();
		std::list< shared_ptr<WebSocketConnectionBase> > connections = std::move(mConnections);
		std::map< std::string, shared_ptr<WebSocketConnectionBase> > listeningConnections = std::move(mListeningConnections);
		mConnectionsMutex.Unlock();
		std::list< shared_ptr<WebSocketConnectionBase> >::iterator it = connections.begin();
		std::list< shared_ptr<WebSocketConnectionBase> >::iterator itEnd = connections.end();
		while(it!=itEnd)
		{
			(*it)->Disconnect();
			++it;
		}
		
		std::map< std::string, shared_ptr<WebSocketConnectionBase> >::iterator lit = listeningConnections.begin();
		std::map< std::string, shared_ptr<WebSocketConnectionBase> >::iterator litEnd = listeningConnections.end();
		while(lit!=litEnd)
		{
			lit->second->StopListening();
			lit->second->Disconnect();
			++lit;
		}
	}

	void WebSocketNetworkSystem::CleanUp()
	{
		DisconnectAll();
		std::vector< shared_ptr<Thread> >::iterator it = mThreads.begin();
		std::vector< shared_ptr<Thread> >::iterator itEnd = mThreads.end();
		while(it != itEnd)
		{
			(*it)->Terminate(true);
			++it;
		}
		mThreads.clear();
		mStarted = false;
	}

	void WebSocketNetworkSystem::GetSupportedConnectionTypes(std::vector< std::string >& outSupportedConnectionTypes)
	{
		outSupportedConnectionTypes.push_back("direct");
	}

	void WebSocketNetworkSystem::GetAdditionalConnectionInformationFormat(std::vector< std::string >& outAdditionalInformation)
	{
		outAdditionalInformation.push_back("port,[security=tls,ca=fileName],[privateKey=fileName],[publicCert=fileName],[certChain=fileName],[tlsMode=MOZILLA_MODERN|MOZILLA_INTERMEDIATE],[dh=fileName]");
	}

	bool WebSocketNetworkSystem::Listen(IncomingConnectionListener* listener, const ConnectionDetails& connectionDetails)
	{
		if(!connectionDetails.HasAddress() || !connectionDetails.HasAdditionalInfo())
		{
			return false;
		}

		std::vector< std::string > options;
		if(!connectionDetails.HasAdditionalInfo())
		{
			ECHO_LOG_ERROR("Incomplete connection string. direct:interface:port is required");
			return false;
		}

		// We only support direct
		std::string connectionType;
		if(connectionDetails.HasType())
		{
			connectionType = connectionDetails.GetType();
		} else
		{
			connectionType = "direct";
		}
		
		if(connectionType != "direct")
		{
			ECHO_LOG_ERROR("Unsupported connection type. '" << connectionDetails.GetType() << "'");
			return false;
		}
		
		std::string security = connectionDetails.GetAdditionalInfo<std::string>("security","");
		if(security=="tls")
		{
			return _ListenTLS(listener, connectionDetails);
		}
		return _Listen(listener, connectionDetails);
	}
	
	bool WebSocketNetworkSystem::_Listen( IncomingConnectionListener* listener, const ConnectionDetails& connectionDetails )
	{
		shared_ptr< WebSocketServer > newListener(new WebSocketServer());
		shared_ptr< WebSocketConnection<WebSocketServer> > connection(new WebSocketConnection<WebSocketServer>(*this, newListener));
		connection->SetConnectionDetails(connectionDetails);

		newListener->set_access_channels(websocketpp::log::alevel::connect & websocketpp::log::alevel::disconnect);
		newListener->init_asio();
		newListener->set_reuse_addr(true);
		newListener->set_open_handler(websocketpp::lib::bind(&WebSocketNetworkSystem::AcceptConnection<WebSocketServer, WebSocketConnection<WebSocketServer> >,
				this,websocketpp::lib::placeholders::_1, newListener, connection));

		std::string port = connectionDetails.GetAdditionalInfoWithIndexFallback<std::string>("port",0,"");

		if(port.empty())
		{
			ECHO_LOG_ERROR("port is required");
			return false;
		}

		websocketpp::lib::error_code ec;
		if(connectionDetails.GetAddress() == "ANY")
		{
			optional<u16> porti = Utils::String::FromString<u16>(port);
			if(!porti)
			{
				ECHO_LOG_ERROR("Failed to convert port to u16");
				return false;
			}
			newListener->listen(porti.value(), ec);
		}else
		{
			newListener->listen(connectionDetails.GetAddress(), port, ec);
		}

		mConnectionsMutex.Lock();
		mListeningConnections[port] = connection;
		mIncomingConnectionListeners[connection] = listener;
		mConnectionsMutex.Unlock();

		newListener->start_accept(ec);
		if (ec)
		{
			ECHO_LOG_ERROR("Failed to set up listening. " << ec.message());
			return false;
		}else
		{
			shared_ptr<Thread> thread(new Thread("Websocket listen thread", std::bind(&WebSocketServer::run,newListener)));
			mThreads.push_back(thread);
			if(mStarted)
			{
				thread->Execute();
			}
		}

		ECHO_LOG_INFO("Listening on port: " << port);
		return true;
	}
	
	bool WebSocketNetworkSystem::_ListenTLS( IncomingConnectionListener* listener, const ConnectionDetails& connectionDetails )
	{
		shared_ptr< WebSocketTLSServer > newListener(new WebSocketTLSServer());
		shared_ptr< WebSocketConnection<WebSocketTLSServer> > connection(new WebSocketConnection<WebSocketTLSServer>(*this, newListener));
		connection->SetConnectionDetails(connectionDetails);

		newListener->set_access_channels(websocketpp::log::alevel::connect & websocketpp::log::alevel::disconnect);
		newListener->init_asio();
		newListener->set_reuse_addr(true);
		newListener->set_open_handler(websocketpp::lib::bind(&WebSocketNetworkSystem::AcceptConnection<WebSocketTLSServer, WebSocketConnection<WebSocketTLSServer> >,
				this,websocketpp::lib::placeholders::_1, newListener, connection));

		// TLS handler
		newListener->set_tls_init_handler([connectionDetails](websocketpp::connection_hdl hdl){
			using websocketpp::lib::placeholders::_1;
			using websocketpp::lib::placeholders::_2;
			using websocketpp::lib::bind;

			namespace asio = websocketpp::lib::asio;
			typedef websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> context_ptr;

			// See https://wiki.mozilla.org/Security/Server_Side_TLS for more details about
			// the TLS modes. The code below demonstrates how to implement both the modern
			std::string tlsMode = connectionDetails.GetAdditionalInfo<std::string>("tlsMode","MOZILLA_MODERN");
			if(tlsMode!="MOZILLA_INTERMEDIATE")
			{
				// Defaults to this if it isn't INTERMEDIATE
				tlsMode = "MOZILLA_MODERN";
			}
			
			ECHO_LOG_INFO("on_tls_init called with hdl: " << hdl.lock().get());
			ECHO_LOG_INFO("using TLS mode: " << tlsMode);

			context_ptr ctx = websocketpp::lib::make_shared<asio::ssl::context>(asio::ssl::context::sslv23);
			
			try {
				if (tlsMode == "MOZILLA_MODERN")
				{
					// Modern disables TLSv1
					ctx->set_options(asio::ssl::context::default_workarounds |
									 asio::ssl::context::no_sslv2 |
									 asio::ssl::context::no_sslv3 |
									 asio::ssl::context::no_tlsv1 |
									 asio::ssl::context::single_dh_use);
				} else
				{
					ctx->set_options(asio::ssl::context::default_workarounds |
									 asio::ssl::context::no_sslv2 |
									 asio::ssl::context::no_sslv3 |
									 asio::ssl::context::single_dh_use);
				}
				//ctx->set_password_callback(bind(&get_password));

				std::string caFile = connectionDetails.GetAdditionalInfo<std::string>("ca","");
				if(!caFile.empty())
				{
					ctx->load_verify_file(caFile);
				}

				std::string privateKeyFile = connectionDetails.GetAdditionalInfo<std::string>("privateKey","");
				if(!privateKeyFile.empty())
				{
					ctx->use_private_key_file(privateKeyFile,asio::ssl::context::pem);
				}

				std::string publicCertFile = connectionDetails.GetAdditionalInfo<std::string>("publicCert","");
				if(!publicCertFile.empty())
				{
					ctx->use_certificate_file(publicCertFile,asio::ssl::context::pem);
				}
				
				std::string certChainFile = connectionDetails.GetAdditionalInfo<std::string>("certChain","");
				if(!certChainFile.empty())
				{
					ctx->use_certificate_chain_file(certChainFile);
				}

				std::string dhFile = connectionDetails.GetAdditionalInfo<std::string>("dh","");
				if(!dhFile.empty())
				{
					// Example method of generating this file:
					// `openssl dhparam -out dh.pem 2048`
					// Mozilla Intermediate suggests 1024 as the minimum size to use
					// Mozilla Modern suggests 2048 as the minimum size to use.
					ctx->use_tmp_dh_file(dhFile);
				}

				std::string ciphers;

				if (tlsMode == "MOZILLA_MODERN")
				{
					ciphers = "ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES256-GCM-SHA384:DHE-RSA-AES128-GCM-SHA256:DHE-DSS-AES128-GCM-SHA256:kEDH+AESGCM:ECDHE-RSA-AES128-SHA256:ECDHE-ECDSA-AES128-SHA256:ECDHE-RSA-AES128-SHA:ECDHE-ECDSA-AES128-SHA:ECDHE-RSA-AES256-SHA384:ECDHE-ECDSA-AES256-SHA384:ECDHE-RSA-AES256-SHA:ECDHE-ECDSA-AES256-SHA:DHE-RSA-AES128-SHA256:DHE-RSA-AES128-SHA:DHE-DSS-AES128-SHA256:DHE-RSA-AES256-SHA256:DHE-DSS-AES256-SHA:DHE-RSA-AES256-SHA:!aNULL:!eNULL:!EXPORT:!DES:!RC4:!3DES:!MD5:!PSK";
				} else
				{
					ciphers = "ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES256-GCM-SHA384:DHE-RSA-AES128-GCM-SHA256:DHE-DSS-AES128-GCM-SHA256:kEDH+AESGCM:ECDHE-RSA-AES128-SHA256:ECDHE-ECDSA-AES128-SHA256:ECDHE-RSA-AES128-SHA:ECDHE-ECDSA-AES128-SHA:ECDHE-RSA-AES256-SHA384:ECDHE-ECDSA-AES256-SHA384:ECDHE-RSA-AES256-SHA:ECDHE-ECDSA-AES256-SHA:DHE-RSA-AES128-SHA256:DHE-RSA-AES128-SHA:DHE-DSS-AES128-SHA256:DHE-RSA-AES256-SHA256:DHE-DSS-AES256-SHA:DHE-RSA-AES256-SHA:AES128-GCM-SHA256:AES256-GCM-SHA384:AES128-SHA256:AES256-SHA256:AES128-SHA:AES256-SHA:AES:CAMELLIA:DES-CBC3-SHA:!aNULL:!eNULL:!EXPORT:!DES:!RC4:!MD5:!PSK:!aECDH:!EDH-DSS-DES-CBC3-SHA:!EDH-RSA-DES-CBC3-SHA:!KRB5-DES-CBC3-SHA";
				}

				if (SSL_CTX_set_cipher_list(ctx->native_handle() , ciphers.c_str()) != 1)
				{
					ECHO_LOG_INFO("Error setting cipher list");
				}
			} catch (std::exception& e) {
				 ECHO_LOG_ERROR("Exception: " << e.what());
			}
			return ctx;
		});
		
		std::string port = connectionDetails.GetAdditionalInfoWithIndexFallback<std::string>("port",0,"");

		if(port.empty())
		{
			ECHO_LOG_ERROR("port is required");
			return false;
		}

		websocketpp::lib::error_code ec;
		if(connectionDetails.GetAddress() == "ANY")
		{
			optional<u16> porti = Utils::String::FromString<u16>(port);
			if(!porti)
			{
				ECHO_LOG_ERROR("Failed to convert port to u16");
				return false;
			}
			newListener->listen(porti.value(), ec);
		}else
		{
			newListener->listen(connectionDetails.GetAddress(), port, ec);
		}

		mConnectionsMutex.Lock();
		mListeningConnections[port] = connection;
		mIncomingConnectionListeners[connection] = listener;
		mConnectionsMutex.Unlock();

		newListener->start_accept(ec);
		if (ec)
		{
			ECHO_LOG_ERROR("Failed to set up listening. " << ec.message());
			return false;
		}else
		{
			shared_ptr<Thread> thread(new Thread("Websocket listen thread", std::bind(&WebSocketTLSServer::run,newListener)));
			mThreads.push_back(thread);
			if(mStarted)
			{
				thread->Execute();
			}
		}

		ECHO_LOG_INFO("Listening on port: " << port);
		return true;
	}
	
	shared_ptr<Connection> WebSocketNetworkSystem::Connect(const ConnectionDetails& connectionDetails, Connection::ConnectCallback connectionCallback, Connection::DisconnectCallback disconnectCallback)
	{
		return nullptr;
//		if(!connectionDetails.HasAddress() || !connectionDetails.HasAdditionalInfo())
//		{
//			return shared_ptr<Connection>();
//		}
//		//Type defaults to Direct
//		//Type
//		shared_ptr<Connection> connection;
//
//		std::vector< std::string > options;
//		if(!connectionDetails.HasAdditionalInfo())
//			return shared_ptr<Connection>();
//
//		Utils::String::Split(connectionDetails.GetAdditionalInfo(), ":", options);
//
//		if(options.empty())
//			return shared_ptr<Connection>();
//
//		u16 port = boost::lexical_cast<u16>(options[0]);
//
//		std::string connectionType;
//		if(connectionDetails.HasType())
//		{
//			connectionType = connectionDetails.GetType();
//		} else
//		{
//			connectionType = "direct";
//		}
//		if(connectionType == "passive")
//		{
//			if(port == 0)
//				return shared_ptr<Connection>();
//			shared_ptr<UDPConnection> udpConnection(new UDPConnection(*this));
//			udpConnection->SetConnectionDetails(connectionDetails);
//			udpConnection->RegisterConnectCallback("",connectionCallback);
//			udpConnection->RegisterDisconnectCallback("",disconnectCallback);
//			udpConnection->Connect();
//			connection = udpConnection;
//		} else
//			if(connectionType == "direct")
//		{
//			shared_ptr<TCPConnection> tcpConnection(new TCPConnection(*this));
//			tcpConnection->SetConnectionDetails(connectionDetails);
//			tcpConnection->RegisterConnectCallback("",connectionCallback);
//			tcpConnection->RegisterDisconnectCallback("",disconnectCallback);
//			tcpConnection->Connect();
//			connection = tcpConnection;
//		} else
//		{
//			ECHO_LOG_ERROR("SocketNetworkSystem::Listen(): Unsupported connection type. '" << connectionDetails.GetType() << "'");
//			return shared_ptr<Connection>();
//		}
//		return connection;
	}

}
