#ifndef _SSLCONNECTION_H_
#define _SSLCONNECTION_H_
#include <echo/Network/Connection.h>
#include <echo/Network/NetRedefinitions.h>
#include <tls.h>

struct tls;

namespace Echo
{
	class SocketNetworkSystem;

	class TLSConnection : public Connection
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
		tls* mTLSContext;
		tls* mReadWriteTLSContext;
		std::string mPrivateKeyFile;
		std::string mPublicCertificateFile;
		bool mListenConnection;
		bool mNeedsClientTLSUpgrade;
		bool mNeedsHandshake;
		void CleanTLS();
		/**
		 * Constructor for creating a TLS connection that has been accepted.
		 * This constructor will take the new socket and set up TLS on it.
		 */
		TLSConnection(SocketNetworkSystem& manager, SocketAddressIn sockaddr, Socket s);

		/**
		 * Upgrade the connection to a TLS connection.
		 * @param connectionDetails The connection details that includes options for setting up the connection, this
		 * will come from the listen connection.
		 * @return true if the connection was upgrade, otherwise false on failure and in which case an error will be logged.
		 */
		bool UpgradeServer(const ConnectionDetails& connectionDetails);

		/**
		 * Upgrades connection to a TLS connection as a client.
		 * @return true if the connection was upgrade, otherwise false on failure and in which case an error will be logged.
		 */
		bool UpgradeClient();

		/**
		 * Configure a tls context based on Connection detail additional info.
		 */
		bool Configure(tls* tlsContext, const ConnectionDetails& connectionDetails);
		
		/**
		 * Override that performs connection upgrade and handshake if required.
		 */
		virtual ReceiveStatus ReceivePackets() override;

		/**
		 * Override that performs connection upgrade and handshake if required.
		 */
		virtual SendStatus SendPackets(bool reenable) override;
	public:
		/**
		 * Constructor for creating a server side TLS connection that will be in a listen state
		 */
		TLSConnection(SocketNetworkSystem& manager, const ConnectionDetails& connectionDetails, u16 port, Socket s);

		/**
		 * Constructor for creating a TLS connection to later call Connect() on.
		 */
		TLSConnection(SocketNetworkSystem& manager, const ConnectionDetails& connectionDetails);
		
		~TLSConnection();
		
		/**
		 * Accept a connection and return a new secure connection for it.
		 * @note This will always return null on non-listen connections.
		 * @return Accepted TLS connection ready to send or receive data, or null if there is an error.
		 */
		shared_ptr<TLSConnection> Accept();

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
