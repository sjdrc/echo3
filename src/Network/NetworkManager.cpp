#include <echo/Network/NetRedefinitions.h>
#include <echo/Network/NetworkManager.h>
#include <echo/Network/NetworkSystem.h>
#include <echo/Network/Connection.h>
#include <echo/Network/DataPacket.h>
#include <echo/Network/ConnectionDetails.h>
#include <echo/Network/SimpleDataPacketPool.h>
#include <echo/Util/StringUtils.h>
#include <echo/Kernel/Thread.h>
#include <echo/Kernel/ScopedLock.h>
#include <iostream>
#include <cctype>

namespace Echo
{
	NetworkManager::NetworkManager(shared_ptr<DataPacketFactory> dataPacketFactory) :
		mTotalBytesSent(0),
		mTotalBytesReceived(0),
		mBytesSentPerSecond(0),
		mBytesReceviedPerSecond(0),
		mCurrentBytesSentPerSecond(0),
		mCurrentBytesReceviedPerSecond(0),
		mNewConnectionBufferSize(1024*1024*5)			// 5MB Buffer, this can be reconfigured
	{
		if(!dataPacketFactory)
		{
			// Default is a simple pool with 512 8KiB packets - taking up 4MiB. This is fairly
			// arbitrary and can be overridden by providing a custom factory.
			mDataPacketFactory.reset(new SimpleDataPacketPool(512,8192));
		}else
		{
			mDataPacketFactory = dataPacketFactory;
		}
	}

	NetworkManager::~NetworkManager()
	{
		UninstallAllSystems();
		mPacketsPendingProcessing.clear();
		mConnectionsDropped.clear();
		mConnectionsEstablished.clear();
		mConnectionsIncoming.clear();
		mNetworkEventListeners.clear();
		mDefaultSystem.reset();
		// We explicitly need to clean this up last because we want any packets to be cleaned up first
		mDataPacketFactory.reset();
	}

	bool NetworkManager::Listen( IncomingConnectionListener* listener, const std::string& connectionDetails )
	{
		ConnectionDetails detailsExploded(connectionDetails);
		shared_ptr<NetworkSystem> system;
		if(detailsExploded.HasSystem())
		{
			system=GetInstalledSystem(detailsExploded.GetSystem());
		}else
		{
			system=mDefaultSystem;
		}
		if(!system)
			return false;
		return system->Listen(listener,detailsExploded);
	}

	shared_ptr<Connection> NetworkManager::Connect( const std::string& connectionDetails, Connection::ConnectCallback connectCallback, Connection::DisconnectCallback disconnectCallback)
	{
		ConnectionDetails detailsExploded(connectionDetails);
		shared_ptr<NetworkSystem> system;
		if(detailsExploded.HasSystem())
		{
			system=GetInstalledSystem(detailsExploded.GetSystem());
		}else
		{
			system=mDefaultSystem;
		}
		if(!system)
			return shared_ptr<Connection>();
		return system->Connect(detailsExploded,connectCallback,disconnectCallback);
	}

	bool NetworkManager::InstallSystem( shared_ptr<NetworkSystem> networkSystem, bool defaultSystem/*=false */ )
	{
		if(!networkSystem)
		{
			ECHO_LOG_ERROR("NetworkManager::InstallSystem(): Null System.");
			return false;
		}

		std::map< std::string, shared_ptr<NetworkSystem> >::iterator it=mSystems.find(networkSystem->GetName());
		if(it!=mSystems.end())
		{
			ECHO_LOG_ERROR("NetworkManager::InstallSystem(): System with name '" << networkSystem->GetName() << "' already exists.");
			return false;
		}

		if(!networkSystem->Initialise())
		{
			ECHO_LOG_ERROR("NetworkManager::InstallSystem(): Network system '" << networkSystem->GetName() << "' failed to initialise.");
			return false;
		}

		mSystems[networkSystem->GetName()]=networkSystem;
		if(defaultSystem || !mDefaultSystem)
			mDefaultSystem=networkSystem;
		return true;
	}

	bool NetworkManager::UninstallSystem( const std::string& name )
	{
		std::map< std::string, shared_ptr<NetworkSystem> >::iterator it=mSystems.find(name);
		if(it==mSystems.end())
		{
			ECHO_LOG_ERROR("NetworkManager::UninstallSystem(): System with name '" << name << "' not found.");
			return false;
		}
		it->second->DisconnectAll();
		//Update for disconnect notifications.
		Update(Seconds(0));
		it->second->CleanUp();
		mSystems.erase(it);
		return true;
	}

	shared_ptr<NetworkSystem> NetworkManager::GetInstalledSystem( const std::string& name )
	{
		std::map< std::string, shared_ptr<NetworkSystem> >::iterator it=mSystems.find(name);
		if(it==mSystems.end())
		{
			ECHO_LOG_ERROR("NetworkManager::GetInstalledSystem(): System with name '" << name << "' not found.");
			return shared_ptr<NetworkSystem>();
		}
		return it->second;
	}

	void NetworkManager::UninstallAllSystems()
	{
		while(!mSystems.empty())
		{
			UninstallSystem(mSystems.begin()->first);
		}
	}


	void NetworkManager::ConnectionEstablished( shared_ptr<Connection> connection )
	{
		{
			ScopedLock lock(mConnectionListsMutex);
			ConnectionSet::index<ConnectionIndex>::type& connectionIndex= mConnectionsEstablished.get<ConnectionIndex>();
			if(connectionIndex.find(connection)!=connectionIndex.end())
			{
				return;
			}
			mConnectionsEstablished.push_back(connection);
		}

		if(!mNetworkEventListeners.empty())
		{
			std::list< shared_ptr<NetworkEventListener> >::iterator it = mNetworkEventListeners.begin();
			std::list< shared_ptr<NetworkEventListener> >::iterator itEnd = mNetworkEventListeners.end();
			while(it!=itEnd)
			{
				(*it)->OnNetworkEvent(NetworkEventListener::NetworkEventTypes::ESTABLISHED);
				++it;
			}
		}
	}

	void NetworkManager::ConnectionIncoming(shared_ptr<Connection> connection, IncomingConnectionListener* listener)
	{
		mConnectionListsMutex.Lock();
		mConnectionsIncoming.push_back(std::make_pair(connection,listener));
		mConnectionListsMutex.Unlock();

		if(!mNetworkEventListeners.empty())
		{
			std::list< shared_ptr<NetworkEventListener> >::iterator it = mNetworkEventListeners.begin();
			std::list< shared_ptr<NetworkEventListener> >::iterator itEnd = mNetworkEventListeners.end();
			while(it!=itEnd)
			{
				(*it)->OnNetworkEvent(NetworkEventListener::NetworkEventTypes::INCOMING_CONNECTION);
				++it;
			}
		}
	}

	void NetworkManager::ConnectionDropped( shared_ptr<Connection> connection )
	{
		{
			ScopedLock lock(mConnectionListsMutex);
			ConnectionSet::index<ConnectionIndex>::type& connectionIndex= mConnectionsDropped.get<ConnectionIndex>();
			if(connectionIndex.find(connection)!=connectionIndex.end())
			{
				return;
			}
			mConnectionsDropped.push_back(connection);
		}

		if(!mNetworkEventListeners.empty())
		{
			std::list< shared_ptr<NetworkEventListener> >::iterator it = mNetworkEventListeners.begin();
			std::list< shared_ptr<NetworkEventListener> >::iterator itEnd = mNetworkEventListeners.end();
			while(it!=itEnd)
			{
				(*it)->OnNetworkEvent(NetworkEventListener::NetworkEventTypes::DISCONNECTED);
				++it;
			}
		}
	}

	void NetworkManager::ConnectionPacketReceived( shared_ptr<Connection> connection, shared_ptr<DataPacket> packet)
	{
		{
			ScopedLock lock(mPacketListsMutex);
			mPacketsPendingProcessing.push_back({connection,packet});
		}

		if(!mNetworkEventListeners.empty())
		{
			std::list< shared_ptr<NetworkEventListener> >::iterator it = mNetworkEventListeners.begin();
			std::list< shared_ptr<NetworkEventListener> >::iterator itEnd = mNetworkEventListeners.end();
			while(it!=itEnd)
			{
				(*it)->OnNetworkEvent(NetworkEventListener::NetworkEventTypes::PACKET_RECEIVED);
				++it;
			}
		}
	}

	bool NetworkManager::OnStart()
	{
		std::map< std::string, shared_ptr<NetworkSystem> >::iterator it=mSystems.begin();
		std::map< std::string, shared_ptr<NetworkSystem> >::iterator itEnd=mSystems.end();
		while(it!=itEnd)
		{
			if(!it->second->Start())
			{
				ECHO_LOG_ERROR("NetworkManager::Start(): System '" << it->second->GetName() << "' failed to start.");
			}
			++it;
		}
		mSpeedTimer.Start();
		return true;
	}

	void NetworkManager::OnStop()
	{
		std::map< std::string, shared_ptr<NetworkSystem> >::iterator it=mSystems.begin();
		std::map< std::string, shared_ptr<NetworkSystem> >::iterator itEnd=mSystems.end();
		while(it!=itEnd)
		{
			it->second->DisconnectAll();
			//Update for disconnect notifications.
			Update(Seconds(0));
			it->second->CleanUp();
			++it;
		}
	}

	void NetworkManager::Update( Seconds lastFrametime )
	{
		TaskGroup::Update(lastFrametime);
		
		// The following used to be in separate functions but that resulted in locking the connections mutex
		
		// Lets get all the containers to process. We'll move them and unlock the mutex so the
		// network threads can continue to notify while we're processing.
		mConnectionListsMutex.Lock();
		ConnectionSet connectionsEstablished = std::move(mConnectionsEstablished);
		std::list< std::pair< shared_ptr<Connection>, IncomingConnectionListener* > > incomingConnections = std::move(mConnectionsIncoming);
		ConnectionSet connectionsDropped = std::move(mConnectionsDropped);
		mConnectionListsMutex.Unlock();

		mPacketListsMutex.Lock();
		std::vector< ConnectionPacketPair > packetsPendingProcessing = std::move(mPacketsPendingProcessing);
		mPacketListsMutex.Unlock();
		
		// Processed established connections - this prepares their state for incoming listener connection listeners to have
		// them in the correct state.
		if(!connectionsEstablished.empty())
		{
			ConnectionSet::index<InsertOrderIndex>::type& insertOrderIndex= connectionsEstablished.get<InsertOrderIndex>();
			for(auto& connection : insertOrderIndex)
			{
				connection->_established();
			}
		}
		
		// Process incoming connections
		if(!incomingConnections.empty())
		{
			for(auto& connectionListenerPair : incomingConnections)
			{
				connectionListenerPair.second->IncomingConnection(connectionListenerPair.first);
			}
		}
		
		// Process any packets
		if(!packetsPendingProcessing.empty())
		{
			for(auto& connectionPacket : packetsPendingProcessing)
			{
				connectionPacket.mConnection->ProcessReceivedPacket(connectionPacket.mPacket);
			}
		}
		
		// Process dropped connections
		if(!connectionsDropped.empty())
		{
			ConnectionSet::index<InsertOrderIndex>::type& insertOrderIndex= connectionsDropped.get<InsertOrderIndex>();
			for(auto& connection : insertOrderIndex)
			{
				connection->_dropped();
			}
		}

		Seconds speedTime = mSpeedTimer.GetElapsed();
		if(speedTime>=Seconds(1))
		{
			mBytesSentPerSecond = static_cast<Size>( (mCurrentBytesSentPerSecond)/speedTime.count() );
			mBytesReceviedPerSecond = static_cast<Size>( (mCurrentBytesReceviedPerSecond)/speedTime.count() );
			mCurrentBytesSentPerSecond = 0;
			mCurrentBytesReceviedPerSecond = 0;
			mSpeedTimer.Start();
		}
	}

	u32 NetworkManager::GetAddrFromString(const std::string& address)
	{
	//#ifdef ECHO_PLATFORM_WINDOWS
		struct hostent* remoteHost;//=gethostbyname();
		struct in_addr addr;

		// If the user input is an alpha name for the host, use gethostbyname()
		// If not, get host by addr (assume IPv4)
		if (isalpha(address.c_str()[0]))
		{
			// host address is a name
			//ECHO_LOG_INFO("Calling gethostbyname with " << address);
			remoteHost = gethostbyname(address.c_str());		
			addr.s_addr=inet_addr(remoteHost->h_addr_list[0]);
		}else
		{
			//ECHO_LOG_INFO("Calling gethostbyaddr with " << address);
			addr.s_addr = inet_addr(address.c_str());
			if(addr.s_addr == INADDR_NONE)
			{
				ECHO_LOG_WARNING("The IPv4 address entered must be a legal address");
				return 0;
			}else
				remoteHost = gethostbyaddr((char *) &addr, 4, AF_INET);
		}

		if (remoteHost == NULL)
		{
			//int dwError = WSAGetLastError();
			//if (dwError != 0)
			//{
			//	if (dwError == WSAHOST_NOT_FOUND)
			//	{
			//		ECHO_LOG_WARNING("Host not found\n");
			//	} else
			//	if (dwError == WSANO_DATA)
			//	{
			//		ECHO_LOG_WARNING("No data record found");
			//	} else
			//	{
			//		ECHO_LOG_WARNING("Function failed with error: " << dwError);				
			//	}
			//}
		}else
		{
			//ECHO_LOG_INFO("\tOfficial name: " << remoteHost->h_name);
			//ECHO_LOG_INFO("\tAlternate names: " << remoteHost->h_aliases);
			//ECHO_LOG_INFO("\tAddress type: ";
			//switch (remoteHost->h_addrtype)
			//{
			//case AF_INET:
			//	ECHO_LOG_INFO("\tAF_INET");
			//	break;
			//case AF_INET6:
			//	ECHO_LOG_INFO("\tAF_INET6");
			//	break;
			//case AF_NETBIOS:
			//	ECHO_LOG_INFO("\tAF_NETBIOS");
			//	break;
			//default:
			//	ECHO_LOG_INFO("\t" << remoteHost->h_addrtype);
			//	break;
			//}
		}
		return addr.s_addr;
	//#endif
	//	return 0;
	}

	void NetworkManager::AddNetworkEventListener( shared_ptr<NetworkEventListener> listener )
	{
		mNetworkEventListeners.push_back(listener);
	}

	void NetworkManager::RemoveNetworkEventListener( shared_ptr<NetworkEventListener> listener )
	{
		mNetworkEventListeners.remove(listener);
	}

	void NetworkManager::ClearAllEventListeners()
	{
		mNetworkEventListeners.clear();
	}

	shared_ptr<DataPacket> NetworkManager::NewDataPacket()
	{
		return mDataPacketFactory->NewDataPacket();
	}
	
	Size NetworkManager::GetPreallocationPoolSize() const
	{
		return mDataPacketFactory->GetPreallocationPoolSize();
	}
	
	void NetworkManager::SetNewConnectionBufferSize(Size sizeInBytes)
	{
		mNewConnectionBufferSize = sizeInBytes;
	}
	
	Size NetworkManager::GetNewConnectionBufferSize() const
	{
		return mNewConnectionBufferSize;
	}
	
	void NetworkManager::ReportSentData(Size numberOfBytesSent)
	{
		mTotalBytesSent+=numberOfBytesSent;
		mCurrentBytesSentPerSecond+=numberOfBytesSent;
	}
	
	void NetworkManager::ReportReceivedData(Size numberOfBytesReceived)
	{
		mTotalBytesReceived+=numberOfBytesReceived;
		mCurrentBytesReceviedPerSecond+=numberOfBytesReceived;
	}
}
