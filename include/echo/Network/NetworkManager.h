#ifndef _NETWORKMANAGER_H_
#define _NETWORKMANAGER_H_
#include <echo/Kernel/TaskGroup.h>
#include <echo/Network/IncomingConnectionListener.h>
#include <echo/Network/NetworkEventListener.h>
#include <echo/Network/Connection.h>
#include <echo/Kernel/Mutex.h>
#include <echo/Chrono/CPUTimer.h>
#include <echo/Network/DataPacketFactory.h>
#include <list>
#include <map>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/sequenced_index.hpp>

/*Concepts
DataPacket	

Connection
	ConnectionTypeInfo*
	Send
	Recv
ConnectionOwner/Listener
	NotifyReceiveData
ConnectionTypeInfo
ConnectionTypeManager

NetworkManager (Manager)

Passive Connection
Direct Connection

When listening

A Passive connection is already "established" by definition. So the network manager
will provide a means to establish a secondary connection from a request on the
passive listening connection.

Passive Listen - Opens a passive connection 
Receive Data
Is Connection request?
Yes		Connection request contains information about establishing another 
connection (in the format of connectionDetails)
No		Discard Data

*/
namespace Echo
{
	class NetworkSystem;
	class ConnectionDetails;

	class NetworkManager : public TaskGroup, public DataPacketFactory
	{
	private:
		Mutex mConnectionListsMutex;
		Mutex mPacketListsMutex;
		
		struct InsertOrderIndex{};
		struct ConnectionIndex{};

		/**
		 * A ConnectionSet is a multi-index for look up of connection and iteration by insert order.
		 */
		typedef boost::multi_index_container< shared_ptr<Connection>,
					boost::multi_index::indexed_by<
						// Sort order added
						boost::multi_index::sequenced< boost::multi_index::tag<InsertOrderIndex> >,
						// Sort by connection
						boost::multi_index::ordered_unique< boost::multi_index::tag<ConnectionIndex>, boost::multi_index::identity< shared_ptr<Connection> > >
					>
				> ConnectionSet;
		
		struct ConnectionPacketPair
		{
			shared_ptr<Connection> mConnection;
			shared_ptr<DataPacket> mPacket;
		};
		std::vector< ConnectionPacketPair > mPacketsPendingProcessing;
		ConnectionSet mConnectionsDropped;
		ConnectionSet mConnectionsEstablished;
		std::list< std::pair< shared_ptr<Connection>, IncomingConnectionListener* > > mConnectionsIncoming;
		std::list< shared_ptr<NetworkEventListener> > mNetworkEventListeners;

		std::map< std::string, shared_ptr<NetworkSystem> > mSystems;
		shared_ptr<NetworkSystem> mDefaultSystem;
		
		Size mTotalBytesSent;
		Size mTotalBytesReceived;
		Size mBytesSentPerSecond;
		Size mBytesReceviedPerSecond;
		Size mCurrentBytesSentPerSecond;
		Size mCurrentBytesReceviedPerSecond;
		Size mNewConnectionBufferSize;
		Timer::CPUTimer mSpeedTimer;
		shared_ptr<DataPacketFactory> mDataPacketFactory;
	public:
		/**
		 * @param dataPacketFactory if null, the NetworkManager will create an internal default.
		 */
		NetworkManager(shared_ptr<DataPacketFactory> dataPacketFactory = nullptr);
		~NetworkManager();

		//Setup a listening connection.
		//listener must be a valid listener
		bool Listen( IncomingConnectionListener* listener, const std::string& connectionDetails );

		//Connection details contain the format
		//
		//(system)type:address:addressAdditionalDetails
		//
		//depending on the system, the address and address details will vary. Address details may or
		//may not be optional.
		//
		//Examples are.
		//
		//	(Socket)passive:emblementertainment.com.au:1234
		//	(Socket)direct:emblementertainment.com.au:5647
		//
		//In both cases the Socket system is used.
		//	passive indicates a UDP connection
		//	direct indicates a TCP connection
		shared_ptr<Connection> Connect( const std::string& connectionDetails, Connection::ConnectCallback connectCallback = Connection::ConnectCallback(), Connection::DisconnectCallback disconnectCallback = Connection::DisconnectCallback());

		//Install a Network System.
		//This will cause Initialise to be called on the Network system.
		bool InstallSystem( shared_ptr<NetworkSystem> networkSystem, bool defaultSystem=false );
		bool UninstallSystem( const std::string& name );
		shared_ptr<NetworkSystem> GetInstalledSystem( const std::string& name );
		void UninstallAllSystems();

		void AddNetworkEventListener(shared_ptr<NetworkEventListener> listener);
		void RemoveNetworkEventListener(shared_ptr<NetworkEventListener> listener);
		void ClearAllEventListeners();

		//These three functions can operate on any thread. When NetworkManager is updated
		//any processing of notifications to ConnectionOwner is done. This means
		//that what ever thread NetworkManager is running on is the thread ConnectionOwner
		//will receive on.
		void ConnectionEstablished(shared_ptr<Connection> connection);
		void ConnectionIncoming(shared_ptr<Connection> connection, IncomingConnectionListener* listener);
		void ConnectionDropped(shared_ptr<Connection> connection);
		void ConnectionPacketReceived(shared_ptr<Connection> connection, shared_ptr<DataPacket> packet);

		u32 GetAddrFromString(const std::string& address);
		
		shared_ptr<DataPacket> NewDataPacket() override;
		Size GetPreallocationPoolSize() const override;

		void ReportSentData(Size numberOfBytesSent);
		void ReportReceivedData(Size numberOfBytesReceived);

		//Start is called like any other EchoTask Start and will call Start
		//for each installed system.h This is a chance for each system to start
		//any threads they might be using.
		//
		//Note: NetworkManager::Start() will never fail even if a system fails to start
		bool OnStart() override;

		void OnStop() override;

		void Update(Seconds lastFrametime) override;
		
		void SetNewConnectionBufferSize(Size sizeInBytes);
		Size GetNewConnectionBufferSize() const;
		
		Size GetBytesSentPerSecond() const {return mBytesSentPerSecond;}
		Size GetBytesReceviedPerSecond() const {return mBytesReceviedPerSecond;}
		Size GetTotalBytesSent() const {return mTotalBytesSent;}
		Size GetTotalBytesRecevied() const {return mTotalBytesReceived;}
	};
}
#endif
