#include <echo/Network/NetRedefinitions.h>
#include <echo/Types.h>
#include <echo/Platforms/Boost/BoostASIONetworkSystem.h>
#include <echo/Platforms/Boost/BoostTCPConnection.h>
#include <echo/Util/StringUtils.h>
#include <echo/Kernel/ScopedLock.h>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <sstream>
#include <errno.h>
#include <unistd.h>
#include <algorithm>

namespace Echo
{
	//////////////////////////////////////////////////////////////////////////
	BoostASIONetworkSystem::BoostASIONetworkSystem(NetworkManager &networkManager, Size numberOfThreads) :
		NetworkSystem("ASIO", networkManager),
		mWorkGuard(boost::asio::make_work_guard(mIOContext))
	{
		mStarted = false;
		mNumberOfThreads = numberOfThreads;
	}

	BoostASIONetworkSystem::~BoostASIONetworkSystem()
	{
		CleanUp();
	}

	bool BoostASIONetworkSystem::Initialise()
	{
		ScopedLock locky(mThreadMutex);
		if (!mThreads.empty())
		{
			ECHO_LOG_WARNING("Already initialised");
			return false;
		}
		for (Size i = 0; i < mNumberOfThreads; ++i)
		{
			mThreads.push_back(make_shared<Thread>("ASIONet" + std::to_string(i), [this]()
			{
				ECHO_LOG_INFO("IO Context run");
				mIOContext.run();
				ECHO_LOG_INFO("IO Context run complete");
			}));
		}
		return true;
	}

	bool BoostASIONetworkSystem::Start()
	{
		if (mStarted)
		{
			ECHO_LOG_ERROR("Already started");
			return false;
		}
		ECHO_LOG_INFO("Starting Boost ASIO NetworkSystem");
		ScopedLock locky(mThreadMutex);
		ECHO_LOG_DEBUG("Starting " << mThreads.size() << " worker threads");
		for (auto &thread : mThreads)
		{
			thread->Execute();
		}
		mStarted = true;
		return true;
	}

	void BoostASIONetworkSystem::DisconnectAll()
	{
		// Create a copy of the connections list since a connect request can affect
		// the list and we can't hold the lock while attempting to disconnect. A
		// disconnect will not do anything the connection is already disconnected.
		mConnectionsMutex.Lock();
		std::set< BoostTCPConnection* > connections = mConnections;
		mConnectionsMutex.Unlock();
		for (auto connection : connections)
		{
			connection->Disconnect();
		}

		StopAllListeners();
	}
	
	void BoostASIONetworkSystem::StopListener(u16 port)
	{
		ScopedLock lock(mConnectionsMutex);
		auto it = mListeners.find(port);
		if(it==mListeners.end())
		{
			ECHO_LOG_WARNING("StopListener port " << port << " but not listening");
			return;
		}

		ECHO_LOG_WARNING("Stopping listen port " << it->second->mPort);
		if(it->second->mAcceptor->is_open())
		{
			it->second->mAcceptor->close();
		}
	}

	void BoostASIONetworkSystem::StopAllListeners()
	{
		ScopedLock lock(mConnectionsMutex);
		for(auto& listener : mListeners)
		{
			ECHO_LOG_WARNING("Stopping listen port " << listener.first);
			if(listener.second->mAcceptor->is_open())
			{
				listener.second->mAcceptor->close();
			}
		}
	}

	void BoostASIONetworkSystem::CleanUp()
	{
		ECHO_LOG_INFO("Cleaning up Boost ASIO NetworkSystem");
		DisconnectAll();
		mWorkGuard.reset();
		mIOContext.stop();
		ECHO_LOG_INFO("Terminating " << mThreads.size() << " worker threads");
		for (auto t : mThreads)
		{
			t->Terminate(true);
		}
		mStarted = false;
	}

	void BoostASIONetworkSystem::_RemoveConnection( BoostTCPConnection* connection)
	{
		mConnections.erase(connection);
	}

	void BoostASIONetworkSystem::GetSupportedConnectionTypes(std::vector<std::string> &outSupportedConnectionTypes)
	{
		outSupportedConnectionTypes.push_back("direct");
	}

	void BoostASIONetworkSystem::GetAdditionalConnectionInformationFormat(std::vector<std::string> &outAdditionalInformation)
	{
		outAdditionalInformation.push_back("port");
	}

	bool BoostASIONetworkSystem::Listen(IncomingConnectionListener *listener, const ConnectionDetails &connectionDetails)
	{
		if (!connectionDetails.HasAddress() || !connectionDetails.HasAdditionalInfo())
		{
			ECHO_LOG_INFO("Socket listening requires an address (can be ANY) and a port to be specified, using port=number or just the port number first");
			return false;
		}

		u16 port = connectionDetails.GetAdditionalInfoWithIndexFallback<u16>("port", 0, 0);
		if (port == 0)
		{
			return false;
		}

		std::string connectionType;
		if (connectionDetails.HasType())
		{
			connectionType = connectionDetails.GetType();
		}
		else
		{
			connectionType = "direct";
		}
		if (connectionType == "direct")
		{
			shared_ptr<ListenState> listenState(new ListenState());
			listenState->mEndpoint.reset(new boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port));
			listenState->mAcceptor.reset(new boost::asio::ip::tcp::acceptor(mIOContext, *listenState->mEndpoint));
			listenState->mCurrentSocket.reset(new TCPSocket(mIOContext));
			listenState->mAcceptor->listen();
			listenState->mAcceptor->async_accept(*listenState->mCurrentSocket, bind(&BoostASIONetworkSystem::AcceptHandler, this, placeholders::_1, port));
			listenState->mListener = listener;
			listenState->mPort = port;

			ECHO_LOG_INFO("Listening on port: " << port);

			{
				ScopedLock lock(mConnectionsMutex);
				mListeners[port] = listenState;
			}

			return true;
		}

		ECHO_LOG_ERROR("Unsupported connection type. '" << connectionDetails.GetType() << "'");
		return false;
	}

	void BoostASIONetworkSystem::AcceptHandler(const boost::system::error_code &ec, u16 port)
	{
		ScopedLock lock(mConnectionsMutex);
		if (ec)
		{
			ECHO_LOG_ERROR("Accept failed with error: " << ec.message());
			mListeners.erase(port);
			return;
		}
		std::map<u16, shared_ptr<ListenState>>::iterator it = mListeners.find(port);

		if (it == mListeners.end())
		{
			ECHO_LOG_ERROR("No associated listener for port " << port);
			return;
		}

		ECHO_LOG_INFO("Accepted connection on port " << port);

		shared_ptr<TCPSocket> socket = std::move(it->second->mCurrentSocket);

		// Create a new socket and accept on it
		it->second->mCurrentSocket.reset(new TCPSocket(mIOContext));
		it->second->mAcceptor->async_accept(*it->second->mCurrentSocket, bind(&BoostASIONetworkSystem::AcceptHandler, this, placeholders::_1, port));

		shared_ptr<BoostTCPConnection> connection(new BoostTCPConnection(*this, mIOContext));
		connection->Accept(socket);
		mConnections.insert(connection.get());
		UpdateIncoming(connection, it->second->mListener);
	}

	shared_ptr<Connection> BoostASIONetworkSystem::Connect(const ConnectionDetails &connectionDetails, Connection::ConnectCallback connectionCallback, Connection::DisconnectCallback disconnectCallback)
	{
		if (!connectionDetails.HasAddress() || !connectionDetails.HasAdditionalInfo())
		{
			return nullptr;
		}

		u16 port = connectionDetails.GetAdditionalInfoWithIndexFallback("port", 0, 0);
		if (port == 0)
		{
			ECHO_LOG_INFO("Socket connections need at minimum a port.");
			return nullptr;
		}

		std::string connectionType;
		if (connectionDetails.HasType())
		{
			connectionType = connectionDetails.GetType();
		}
		else
		{
			connectionType = "direct";
		}

		if (connectionType == "direct")
		{
			shared_ptr<BoostTCPConnection> connection(new BoostTCPConnection(*this, mIOContext, connectionDetails));
			connection->RegisterConnectCallback("", connectionCallback);
			connection->RegisterDisconnectCallback("", disconnectCallback);
			connection->Connect();
			{
				ScopedLock lock(mConnectionsMutex);
				mConnections.insert(connection.get());
			}
			return connection;
		}
		ECHO_LOG_ERROR("Unsupported connection type: '" << connectionDetails.GetType() << "'");
		return nullptr;
	}
}