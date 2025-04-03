#ifndef _BOOSTASIONETWORKSYSTEM_H_
#define _BOOSTASIONETWORKSYSTEM_H_
#include <echo/Network/NetRedefinitions.h>
#include <echo/Network/NetworkManager.h>
#include <echo/Network/NetworkSystem.h>
#include <echo/Kernel/Thread.h>
#include <echo/Kernel/Mutex.h>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <list>
#include <map>

namespace Echo
{
	class Connection;
	class ConnectionDetails;
	class IncomingConnectionListener;
	class BoostTCPConnection;

	class BoostASIONetworkSystem : public NetworkSystem
	{
	protected:
		typedef boost::asio::ip::tcp::socket TCPSocket;
		Mutex mConnectionsMutex;
		struct ListenState
		{
			shared_ptr<boost::asio::ip::tcp::endpoint> mEndpoint;
			shared_ptr<boost::asio::ip::tcp::acceptor> mAcceptor;
			shared_ptr<TCPSocket> mCurrentSocket;
			IncomingConnectionListener* mListener;
			u16 mPort;
		};
		std::map< u16, shared_ptr<ListenState> > mListeners;
		std::set< BoostTCPConnection* > mConnections;
		boost::asio::io_context mIOContext;
		Size mNumberOfThreads;
		Mutex mThreadMutex;
		std::vector< shared_ptr<Thread> > mThreads;
		boost::asio::executor_work_guard<boost::asio::io_context::executor_type> mWorkGuard;
		bool mStarted;
		void AcceptHandler(const boost::system::error_code &ec, u16 port);
	public:

		struct InterfaceInformation
		{
			std::string mHostname;
			std::string mIPAddress;
			std::string mSubnetMask;
			std::string mGateway;
			std::string mDNS1;
			std::string mDNS2;
			std::string mMacAddress;
		};

		BoostASIONetworkSystem(NetworkManager& networkManager, Size numberOfThread = 1);
		~BoostASIONetworkSystem();

		//Initialise the Network System. Perform initialisation/allocations here
		bool Initialise();

		//Start is called when ENetwork begins execution. This is a good
		//point to execute any threads.
		bool Start();

		void CleanUp();

		void DisconnectAll();
		void StopListener(u16 port);
		void StopAllListeners();

		//This method should fill outSupportedConnectionTypes with any connection types this system supports.
		//Common connection types include, Passive and Direct
		void GetSupportedConnectionTypes( std::vector< std::string >& outSupportedConnectionTypes );

		//This method should fill outAdditionalInformation with any additional information required or optional
		//for each of the supported connection types. Each entry in this vector should correlate with each
		//corresponding element in the vector in GetSupportedConnectionTypes().
		void GetAdditionalConnectionInformationFormat( std::vector< std::string >& outAdditionalInformation );

		static bool GetInterfaceInformation(std::vector<InterfaceInformation>& outInterfaceInformation);

		//Perform an Asynchronous Listen. IMPORTANT: This function should not block
		bool Listen( IncomingConnectionListener* listener, const ConnectionDetails& connectionDetails );

		//Perform an Asynchronous Connect. IMPORTANT: This function should not block
		shared_ptr<Connection> Connect( const ConnectionDetails& connectionDetails, Connection::ConnectCallback connectionCallback, Connection::DisconnectCallback disconnectCallback);

		/**
		 * Attempts to get the host IP by name or string IP.
		 * @param hostNameOrIP The hostname or IP address as a string, e.g. www.emblem.net.au or 192.168.1.1.
		 * @param output The IPv4 address will be written to this value upon success. The value is not modified
		 * if there is an error.
		 * @return false if the DNS lookup failed.
		 */
		static bool GetHostByName(const std::string& hostnameOrIP, u32& output);
		
		/**
		 * Internal method called by BoostTCPConnection on destruction to notify the manager.
		 */
		void _RemoveConnection( BoostTCPConnection* );
	};
}
#endif
