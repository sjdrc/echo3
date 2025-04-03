#ifndef ECHO_WEBSOCKETNETWORKSYSTEM_H
#define ECHO_WEBSOCKETNETWORKSYSTEM_H
#include <echo/Network/NetRedefinitions.h>
#include <echo/Network/NetworkManager.h>
#include <echo/Network/NetworkSystem.h>
#include <echo/Kernel/Thread.h>
#include <echo/Kernel/Mutex.h>
#include <websocketpp/config/core.hpp>
#include <websocketpp/config/asio.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <list>
#include <map>

namespace Echo
{
	class Connection;
	class ConnectionDetails;
	class IncomingConnectionListener;
	class WebSocketNetworkSystem;

	class WebSocketConnectionBase : public Connection
	{
	public:
		WebSocketConnectionBase(WebSocketNetworkSystem& manager);
		virtual ~WebSocketConnectionBase();
		virtual void StopListening() = 0;
	protected:
		WebSocketNetworkSystem& mManager;
		friend class WebSocketNetworkSystem;
		std::string mURI;
		std::queue<char> mBufferedData;

		const int WRITE_ERROR_DISCONNECTED = -1;
		const int READ_ERROR_DISCONNECTED = -1;
		
		virtual ReceiveResult Receive(u8* buffer, int bufferSizeInBytes) override;
	};

	class WebSocketNetworkSystem : public NetworkSystem
	{
	public:
		typedef websocketpp::connection_hdl ConnectionHandle;
		typedef websocketpp::server<websocketpp::config::asio> WebSocketServer;
		typedef websocketpp::server<websocketpp::config::asio_tls> WebSocketTLSServer;
	protected:
		Mutex mConnectionsMutex;

		std::map< std::string, shared_ptr<WebSocketConnectionBase> > mListeningConnections;
		std::map< shared_ptr<WebSocketConnectionBase>, IncomingConnectionListener* > mIncomingConnectionListeners;
		std::list< shared_ptr<WebSocketConnectionBase> > mConnections;

		std::vector< shared_ptr<Thread> > mThreads;
		bool mStarted;
	public:
		WebSocketNetworkSystem(NetworkManager& networkManager);
		~WebSocketNetworkSystem();

		//Initialise the Network System.
		bool Initialise() override;

		//Start is called when Network begins execution. This is a good
		//point to execute any threads.
		bool Start() override;

		void CleanUp() override;

		void DisconnectAll() override;

		//This method should fill outSupportedConnectionTypes with any connection types this system supports.
		//Common connection types include, Passive and Direct
		void GetSupportedConnectionTypes( std::vector< std::string >& outSupportedConnectionTypes ) override;

		//This method should fill outAdditionalInformation with any additional information required or optional
		//for each of the supported connection types. Each entry in this vector should correlate with each
		//corresponding element in the vector in GetSupportedConnectionTypes().
		void GetAdditionalConnectionInformationFormat( std::vector< std::string >& outAdditionalInformation ) override;

		//Perform an Asynchronous Listen. IMPORTANT: This function should not block
		bool Listen( IncomingConnectionListener* listener, const ConnectionDetails& connectionDetails ) override;

		//Perform an Asynchronous Connect. IMPORTANT: This function should not block
		shared_ptr<Connection> Connect( const ConnectionDetails& connectionDetails, Connection::ConnectCallback connectionCallback, Connection::DisconnectCallback disconnectCallback) override;
	private:
		bool _Listen( IncomingConnectionListener* listener, const ConnectionDetails& connectionDetails );
		bool _ListenTLS( IncomingConnectionListener* listener, const ConnectionDetails& connectionDetails );

		template< typename WebsocketServerType, typename WebSocketConnectionType >
		void AcceptConnection(websocketpp::connection_hdl handle, shared_ptr<WebsocketServerType> server, shared_ptr< WebSocketConnectionBase > listenConnectionBase)
		{
			shared_ptr< WebSocketConnectionType > listenConnection = std::static_pointer_cast< WebSocketConnectionType >(listenConnectionBase);
			IncomingConnectionListener* listener = 0;
			mConnectionsMutex.Lock();
			std::map< shared_ptr<WebSocketConnectionBase>, IncomingConnectionListener* >::iterator it = mIncomingConnectionListeners.find(listenConnection);
			if(it != mIncomingConnectionListeners.end())
			{
				listener = it->second;
			}
			mConnectionsMutex.Unlock();

			if(!listener)
			{
				ECHO_LOG_ERROR("Received incoming connection request but a listener cannot be found for the connection");
				return;
			}

			websocketpp::lib::error_code ec;
			typename WebsocketServerType::connection_ptr websocketConnection = server->get_con_from_hdl(handle, ec);

			if(ec)
			{
				ECHO_LOG_ERROR("Failed to obtain connection from handle." << ec.message());
				return;
			}

			shared_ptr< WebSocketConnectionType > connection(new WebSocketConnectionType(*this, server, handle));
			connection->SetState(Connection::States::CONNECTED);
			connection->AssignHandlers(websocketConnection);
			ECHO_LOG_INFO("Client connected");

			mConnectionsMutex.Lock();
			mConnections.push_back(connection);
			mConnectionsMutex.Unlock();

			UpdateIncoming(connection, listener);
		}	
	};
}
#endif
