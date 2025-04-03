#ifndef ECHO_WEBSOCKETCONNECTION_H
#define ECHO_WEBSOCKETCONNECTION_H
#include <echo/Network/Connection.h>
#include <echo/Network/NetRedefinitions.h>
#include <echo/Network/WebSocketNetworkSystem.h>
#include <queue>

namespace Echo
{
	// Templated version of WebsocketConnection that handles the connection, disconnection and message receiving.
	// Messages are passed to the base which behaves a bit more like a buffered socket.
	template< typename WebSocketEndPointType >
	class WebSocketConnection : public WebSocketConnectionBase
	{
	public:
		WebSocketConnection(WebSocketNetworkSystem& manager, shared_ptr<WebSocketEndPointType> endPoint) :
			WebSocketConnectionBase(manager),
			mEndPoint(endPoint),
			mIsListener(true)
		{}

		WebSocketConnection(WebSocketNetworkSystem& manager, shared_ptr<WebSocketEndPointType> endPoint, websocketpp::connection_hdl connectionHandle) :
			WebSocketConnectionBase(manager),
			mEndPoint(endPoint),
			mConnectionHandle(connectionHandle),
			mIsListener(false)
		{}

		~WebSocketConnection()
		{
			Close();
		}

		void AssignHandlers(typename WebSocketEndPointType::connection_ptr connection)
		{
			connection->set_message_handler(websocketpp::lib::bind(&WebSocketConnection::OnMessage,this,websocketpp::lib::placeholders::_1,websocketpp::lib::placeholders::_2));
			connection->set_open_handler(websocketpp::lib::bind(&WebSocketConnection::OnOpen,this,websocketpp::lib::placeholders::_1));
			connection->set_close_handler(websocketpp::lib::bind(&WebSocketConnection::OnClose,this,websocketpp::lib::placeholders::_1));
		}

		void ClearHandlers(typename WebSocketEndPointType::connection_ptr connection)
		{
			connection->set_message_handler(typename WebSocketEndPointType::message_handler());
			connection->set_open_handler(typename websocketpp::open_handler());
			connection->set_close_handler(typename websocketpp::close_handler());
		}
		
		void StopListening() override
		{
			if(mEndPoint)
			{
				websocketpp::lib::error_code ec;
				mEndPoint->stop_listening(ec);
				mEndPoint->close(mConnectionHandle,websocketpp::close::status::no_status,"",ec);
			}
			mConnectionHandle.reset();
		}
	private:
		friend class WebSocketNetworkSystem;

		// This will either be a server or client, TLS or not. Determined by the manager.
		shared_ptr<WebSocketEndPointType> mEndPoint;
		websocketpp::connection_hdl mConnectionHandle;
		bool mIsListener;

		bool _Connect() override
		{
			if(!(IsConnected()))
			{
				ECHO_LOG_INFO("Connecting to " << mConnectionDetails.GetAddress() << "... ");
				//SetURI(mURI);
				return true;
			}
			return false;
		}

		bool _Disconnect() override
		{
			if(mState!=States::DISCONNECTED)
			{
				SetState(States::DISCONNECTED);
				shared_ptr<Connection> thisConnection=shared_from_this();
				ECHO_LOG_INFO("Disconnected: " << mConnectionDetails.ToString() << "... ");
				Close();
				mManager.UpdateConnect(thisConnection);
				return true;
			}
			return false;
		}
		
		virtual SendResult Send(const u8 * buffer, int numberOfBytesToSend) override
		{
			if(!mEndPoint)
			{
				return SendResult{0, SendStatuses::DISCONNECT};
			}

			websocketpp::lib::error_code ec;
			mEndPoint->send(mConnectionHandle, std::string(reinterpret_cast<const char*>(buffer),numberOfBytesToSend),websocketpp::frame::opcode::BINARY, ec);
			if (ec)
			{
				return SendResult{0, SendStatuses::DISCONNECT};
			}
			return SendResult{static_cast<Size>(numberOfBytesToSend), SendStatuses::SUCCESS};
		}
		
		void OnOpen(websocketpp::connection_hdl handle)
		{
			mManager.UpdateConnect(shared_from_this());
		}
		
		void OnClose(websocketpp::connection_hdl handle)
		{
			if(mState!=States::DISCONNECTED)
			{
				SetState(States::DISCONNECTED);
				shared_ptr<Connection> thisConnection=shared_from_this();
				ECHO_LOG_INFO("Disconnected: " << mConnectionDetails.GetAddress() << "... ");
				mManager.UpdateConnect(thisConnection);
			}
		}
		
		void OnMessage(websocketpp::connection_hdl handle, typename WebSocketEndPointType::message_ptr msg)
		{
			// We're running DataPacket's over the connection.
			const char* bytes = msg->get_payload().c_str();
			size_t numberOfBytes = msg->get_payload().length();
			for(size_t i=0; i < numberOfBytes; ++i)
			{
				mBufferedData.push(bytes[i]);
			}
			mManager.UpdateReceive(shared_from_this());
		}

		void Close()
		{
			if(mIsListener)
			{
				StopListening();
			}else
			{
				if(mEndPoint)
				{
					websocketpp::lib::error_code ec;
					mEndPoint->close(mConnectionHandle,websocketpp::close::status::no_status,"",ec);
				}
				mConnectionHandle.reset();
			}
		}
	};
	
}
#endif
