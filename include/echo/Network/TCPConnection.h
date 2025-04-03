#ifndef _TCPCONNECTION_H_
#define _TCPCONNECTION_H_
#include <echo/Network/Connection.h>
#include <echo/Network/NetRedefinitions.h>

namespace Echo
{
	class SocketNetworkSystem;

	class TCPConnection : public Connection
	{
	protected:
		friend class SocketNetworkSystem;
		u32 mIP;
		u16 mPort;
		Socket mSocket;
		SocketAddressIn mSocketAddress;
		SocketNetworkSystem& mManager;
		virtual void _dropped() override;
		virtual void _established() override;
		bool _Connect() override;
		bool _Disconnect() override;
		bool mListenConnection;

		/**
		 * Constructor for accepted connections.
		 */
		TCPConnection(SocketNetworkSystem& manager, SocketAddressIn sockaddr, Socket s);
	public:
		/**
		 * Constructor for connections in listen state
		 */
		TCPConnection(SocketNetworkSystem& manager, const std::string& address, u16 port, Socket s);
		/**
		 * Constructor for connections to Connect();
		 */
		TCPConnection(SocketNetworkSystem& manager, const ConnectionDetails& connectionDetails);
		~TCPConnection();

		/**
		 * Accept a connection and return a new connection for it.
		 * @note This will always return null on non-listen connections.
		 * @return Accepted connection ready to send or receive data.
		 */
		shared_ptr<TCPConnection> Accept();
		
		SocketAddressIn& GetSocketAddress() {return mSocketAddress;}
		Socket GetSocket() {return mSocket;}

		void SetIP(u32 ip);
		void SetPort_(u16 newPort);	//This is passive. Nothing but the internal
									//port value changes. The port is used on the
									//next connect. This is handy if a connection
									//is broken but isn't destroyed.
									//	1. connection drops
									//		- internal mConnected=false
									//	2. need to reconnect to host on hosts
									//	   listening port. Change port
									//	3. Connect();

		SendResult Send(const u8 * buffer, int numberOfBytesToSend) override;
		ReceiveResult Receive(u8* buffer, int bufferSizeInBytes) override;
		bool _HandleError(int code);
	};
}
#endif
