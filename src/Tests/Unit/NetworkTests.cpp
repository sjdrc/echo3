#include <echo/Network/NetworkManager.h>
#include <echo/Network/SocketNetworkSystem.h>
#include <echo/Platforms/Boost/BoostASIONetworkSystem.h>
#include <echo/Network/ConnectionOwner.h>
#include <echo/Network/IncomingConnectionListener.h>
#include <echo/Network/Connection.h>
#include <echo/Network/DataPacket.h>
#include <echo/Network/NetworkExecutionModel.h>
#include <echo/Chrono/CountDownTimer.h>
#include <echo/Chrono/CPUTimer.h>
#include <echo/Kernel/Kernel.h>
#include <echo/Platform.h>
#include <echo/Kernel/SimpleExecutionModel.h>

#include <doctest/doctest.h>
#undef INFO

using namespace Echo;

class ConnectionOutput : public ConnectionOwner
{
private:
	shared_ptr<Connection> mConnection;
public:
	ConnectionOutput(shared_ptr<Connection> connection) : mConnection(connection)
	{
		connection->SetOwner(this);
		connection->RegisterPacketCallback(100,bind(&ConnectionOutput::OnResponderPacket,this,placeholders::_1,placeholders::_2));
	}
	void ConnectionEstablished(shared_ptr<Connection> connection)
	{

	}
	void ConnectionDrop(shared_ptr<Connection> connection)
	{
		ECHO_LOG_INFO("Server: Client Terminated Connection");
		delete this;
	}
	
	void OnResponderPacket(shared_ptr<Connection> connection, shared_ptr<DataPacket> packet)
	{
		ECHO_LOG_INFO("Sever: Received packet that I'll respond to...");
		connection->SendMessageResponse(packet,"I got your message!",0);
	}
		
	void ReceivedPacket(shared_ptr<Connection> connection, shared_ptr<DataPacket> packet)
	{
		ECHO_LOG_INFO("Server: Received Data: ");
		ECHO_LOG_INFO("PacketTypeID: 0x" << std::hex << packet->GetPacketTypeID() << std::dec << " Size: " << packet->GetDataSize() << " PacketID: " << packet->GetPacketID());
		for(u32 d=0; d< packet->GetDataSize();++d)
		{
			ECHO_LOG_INFO(" : " << packet->GetData()[d]);
		}
		connection->SendControlPacket(0x1);
	}
};

class ConnectionListener : public IncomingConnectionListener, public ConnectionOwner
{
private:
	shared_ptr<Connection> mConnection;
	NetworkManager& mManager;
	bool mFailed;
	bool mTriggeredThreshold;
public:
	ConnectionListener(NetworkManager& manager, int listenPort, bool connect, bool useTLS, std::string system = "Socket") : mManager(manager), mFailed(false)
	{
		if(connect)
		{
			ECHO_LOG_INFO("Acting as Client on port " << listenPort);
			std::stringstream ssc;
			ssc << "(" << system << ")direct:127.0.0.1:" << listenPort;
			if(useTLS)
			{
				ssc << ":security=tls:ca=data/tls/cacert.pem:commonName=tlstest";
			}
			mConnection=mManager.Connect(ssc.str());
			CHECK(bool(mConnection));
			if(mConnection)
			{
				mConnection->SetOwner(this);
				mConnection->RegisterPacketCallback(1,bind(&ConnectionListener::NonOwnerReceivedPacket,this,placeholders::_1,placeholders::_2));
				mConnection->RegisterConnectCallback("ConnectionListener::NonOwnerConnectionEstablished",bind(&ConnectionListener::NonOwnerConnectionEstablished,this,placeholders::_1));
				mConnection->RegisterDisconnectCallback("ConnectionListener::NonOwnerConnectionDrop",bind(&ConnectionListener::NonOwnerConnectionDrop,this,placeholders::_1));
				mConnection->SendControlPacket(1);
				mConnection->SendControlPacket(2);
				mConnection->SendControlPacket(1);
				mConnection->SendControlPacket(3);
				mConnection->SendControlPacket(100,bind(&ConnectionListener::OnResponsePacket,this,placeholders::_1,placeholders::_2));
				mConnection->SendControlPacket(1);
				mConnection->SendControlPacket(4);
				mConnection->SendControlPacket(1);
				mConnection->SendControlPacket(5);
			}else
			{
				ECHO_LOG_INFO("Failed to connect to server");
			}
		}else
		{
			std::stringstream ss;
			ss << "(" << system << ")direct:ANY:" << listenPort;
			if(useTLS)
			{
				ss << ":security=tls:privateKey=data/tls/tlstest-key.pem:publicCert=data/tls/tlstest-cert.pem:ca=data/tls/cacert.pem";
			}
			if(!mManager.Listen(this,ss.str()))
			{
				mFailed = true;
			}else
			{
				ECHO_LOG_INFO("Acting as Server");
				ECHO_LOG_INFO("Opened on port " << listenPort);
			}
		}
	}
	~ConnectionListener()
	{
		
	}
	
	bool GetFailed() const
	{
		return mFailed;
	}
	
	void IncomingConnection(shared_ptr<Connection> connection)
	{
		ECHO_LOG_INFO("Server: Incoming Connection " << connection->GetFriendlyIdentifier());
		new ConnectionOutput(connection);
	}

	void ConnectionEstablished(shared_ptr<Connection> connection)
	{
		ECHO_LOG_INFO("Client: Established Connection to Test Server");
	}

	void ConnectionDrop(shared_ptr<Connection> connection)
	{
		ECHO_LOG_INFO("Client: Server Terminated Connection");
	}

	void NonOwnerConnectionEstablished(shared_ptr<Connection> connection)
	{
		ECHO_LOG_INFO("Client(NonOwner): Established Connection to Test Server");
	}

	void NonOwnerConnectionDrop(shared_ptr<Connection> connection)
	{
		ECHO_LOG_INFO("Client(NonOwner): Server Terminated Connection");
	}
	
	void NonOwnerReceivedPacket(shared_ptr<Connection> connection, shared_ptr<DataPacket> packet)
	{
		ECHO_LOG_INFO("Client (NonOwner): Received Data: ");
		ECHO_LOG_INFO("PacketTypeID: 0x" << std::hex << packet->GetPacketTypeID() << std::dec << " PacketID: " << packet->GetPacketID());
	}
	
	void OnResponsePacket(shared_ptr<Connection> connection, shared_ptr<DataPacket> packet)
	{
		std::string response;
		if(packet->GetStringFromDataPacket(response))
		{
			ECHO_LOG_INFO("Client: Received response: " << response);
		}else
		{
			ECHO_LOG_INFO("Client: Received response but I couldn't read the data");
		}
	}

	void ReceivedPacket(shared_ptr<Connection> connection, shared_ptr<DataPacket> packet)
	{
		ECHO_LOG_INFO("Client: Received Data: 0x" << std::hex << packet->GetPacketTypeID() << std::dec);
	}
};

void SimpleTest(bool useTLS)
{
	Kernel kernel;
	NetworkManager manager;
	shared_ptr<SocketNetworkSystem> socketSystem(new SocketNetworkSystem(manager));
	manager.InstallSystem(socketSystem, true);
	
	ConnectionListener listener(manager,useTLS ? 1234 : 12345,false,useTLS);
	
	REQUIRE(!listener.GetFailed());
	
	ConnectionListener listener2(manager,useTLS ? 1234 : 12345,true,useTLS);
	shared_ptr<ExecutionModel> model = Platform::CreateExecutionModel();
	model->SetModelToUse(ExecutionModel::Models::NONE);

	//Close the application after 5 seconds.
	CountDownTimer timeout(Seconds(5.0),bind(&TaskManager::RemoveAllTasks,&kernel),"RemoveAllTasks");
	kernel.AddTask(timeout);
	
	kernel.SetExecutionModel(model);
	
	kernel.AddTask(manager);
	kernel.Execute();
}

void DataPacketTests()
{
	{
		// Test that the string function works.
		DataPacket stringPacket;
		const std::string inString = "Hello world";
		stringPacket.Configure(inString);
		std::string outString;
		CHECK_MESSAGE(stringPacket.GetStringFromDataPacket(outString),"Failed to process string packet");
		CHECK(outString==inString);
	}
	{
		// Test that the string function work with an offset as expected.
		DataPacket stringPacket;
		const std::string inString = "Hello world";
		stringPacket.Configure(0,inString.length()*3);	// This will be plenty of space.
		u64 someValue = 1234567890;
		stringPacket.AppendData(&someValue,sizeof(someValue));
		stringPacket.AppendString(inString);
		
		u64 someValueOut = stringPacket.Get<u64>(0,0);
		CHECK(someValueOut==someValue);
		
		std::string outString;
		CHECK_MESSAGE(stringPacket.GetStringFromDataPacket(outString,sizeof(someValue)),"Failed to process string packet");
		CHECK(outString==inString);
	}
	{
		// Same as previous test but with a labelled packet (the gets should skip the label)
		DataPacket stringPacket;
		const std::string inString = "Hello world";
		stringPacket.Configure("ExcellentLabel",inString.length()*3);
		u64 someValue = 1234567890;
		stringPacket.AppendData(&someValue,sizeof(someValue));
		stringPacket.AppendString(inString);
		
		u64 someValueOut = stringPacket.Get<u64>(0,0);
		CHECK(someValueOut==someValue);
		
		std::string outString;
		CHECK_MESSAGE(stringPacket.GetStringFromDataPacket(outString,sizeof(someValue)),"Failed to process string packet");
		CHECK(outString==inString);
	}
	{
		// Test that the string function works.
		DataPacket stringPacket;
		std::vector<std::string> inStrings;
		inStrings.push_back("Hello world");
		inStrings.push_back("Goodbye Pluto");
		stringPacket.Configure(inStrings);

		std::vector<std::string> outStrings;
		CHECK_MESSAGE(stringPacket.GetStringFromDataPacket(outStrings),"Failed to process string packet");
		CHECK_MESSAGE(outStrings.size()==inStrings.size(),"Failed to extract the same number of strings that were written.");
		for(Size i=0;i<outStrings.size();++i)
		{
			CHECK(outStrings[i]==inStrings[i]);
		}
	}
	{
		// Now label packet tests
		DataPacket stringPacket;
		std::string label = "magicLabel";
		std::string inString = "RUN!";
		stringPacket.Configure(label,inString);
		std::string outString;
		CHECK_MESSAGE(stringPacket.GetStringFromDataPacket(outString),"Failed to process string packet");
		CHECK(outString==inString);
		std::string outLabel = stringPacket.GetLabel();
		CHECK(outLabel==label);
	}
}

class SelfContainedNetworkNode
{
public:
	SelfContainedNetworkNode()
	{
		shared_ptr<SocketNetworkSystem> system(new SocketNetworkSystem(mNetworkManager));
		mNetworkManager.InstallSystem(system,true);
		mNetworkManager.SetNewConnectionBufferSize(128);
		
		shared_ptr<NetworkExecutionModel> executionModel(new NetworkExecutionModel(mKernel));
		executionModel->SetUseCommandProcessing(false);
		executionModel->SetUpdateFrequency(Seconds(1.0));
		mNetworkManager.AddNetworkEventListener(executionModel);

		mExecutionModel = executionModel;

		mKernel.SetExecutionModel(mExecutionModel);
		mKernel.AddTask(mNetworkManager);
	}
	
	~SelfContainedNetworkNode()
	{
		mExecutionModel->Stop();
		if(mThread)
		{
			mThread->Join();
		}
	}
	
	NetworkManager& GetNetworkManager() {return mNetworkManager;}
	
	void RunInBackground(std::string name)
	{
		mThread.reset(new Thread(name,&mKernel));
		mThread->Execute();
	}

	void Run()
	{
		mKernel.Execute();
	}
	void Stop()
	{
		mKernel.RemoveAllTasks();
	}
private:
	NetworkManager mNetworkManager;
	Kernel mKernel;
	shared_ptr<Thread> mThread;
	std::vector< shared_ptr<Connection> > mConnectedClients;
	shared_ptr<NetworkExecutionModel> mExecutionModel;
};

void TwoThreadTestWithNetworkExecutionModel(bool useTLS)
{
	// This test is designed to check that clients and servers can connect when using separately initialised systems
	// that run in their own threads. It uses the NetworkExecutionModel to update the Kernel on network events.
	SelfContainedNetworkNode serverNode;
	serverNode.RunInBackground("Server");
	ConnectionListener server(serverNode.GetNetworkManager(),useTLS ? 4321 : 43210,false, useTLS);
	REQUIRE_MESSAGE(!server.GetFailed(), "Failed to set up listening");

	SelfContainedNetworkNode clientNode;
	clientNode.RunInBackground("Client");
	ConnectionListener client(clientNode.GetNetworkManager(),useTLS ? 4321 : 43210,true, useTLS);
	
	Thread::Sleep(Seconds(5));
	serverNode.Stop();
	clientNode.Stop();
}

TEST_CASE("Network")
{
	// Turn off log output, we will just use output from this test.
	gDefaultLogger.SetLogMask(Echo::Logger::LogLevels::INFO);

	ECHO_LOG_INFO("DataPacketTests");
	DataPacketTests();
	ECHO_LOG_INFO("SimpleTest(false)");
	SimpleTest(false);
	ECHO_LOG_INFO("TwoThreadTestWithNetworkExecutionModel(false)");
	TwoThreadTestWithNetworkExecutionModel(false);

	// TLS testing
	ECHO_LOG_INFO("SimpleTest(true)");
	SimpleTest(true);

	ECHO_LOG_INFO("TwoThreadTestWithNetworkExecutionModel(true)");
	TwoThreadTestWithNetworkExecutionModel(true);
}

TEST_CASE("NetworkBacklogTrigger")
{
	SelfContainedNetworkNode serverNode;
	// We don't expect this to connect. We just want to trigger the send callback
	shared_ptr<Connection> connection = serverNode.GetNetworkManager().Connect("direct:127.0.0.1:1624");

	std::string myStory = "Once upon a time there was some data that needed to be sent to a far away land to bring joy to the eyes of the receiver."
		" This, of course, was the purpose of data and so the data encapsulated itself into a packet and asked the Connection if it could be sent to"
		" the destination. The path ahead was scary and unknown but the Connection assured the data that it would be sent over TCP and that there"
		" were friendly devices along the way known as routers that will help guarantee a safe journey.";

	shared_ptr<DataPacket> storyPacket = connection->NewDataPacket(1000,myStory.length());
	storyPacket->AppendData(myStory.c_str(),myStory.length());

	bool triggered = false;
	bool* triggeredPtr = &triggered;

	connection->SetBacklogCallback([storyPacket,triggeredPtr,myStory](shared_ptr<Connection> connection,Size triggerAmount,Size triggerThreshold,shared_ptr<DataPacket> packet)
	{
		CHECK(connection!=nullptr);
		CHECK(storyPacket.get()==packet.get());
		CHECK(triggerThreshold==myStory.length()-1);
		CHECK(triggerAmount==(myStory.length()+(sizeof(u32)*3)));	// + header size
		*triggeredPtr = true;
		return false;
	});
	connection->SetBacklogCallbackTriggerThreshold(myStory.length()-1);
	connection->SendDataPacket(storyPacket);

	// Clear the callback which will clear the lambda capture of the packet and reduce the use count to 1 if the packet was discarded.
	connection->ClearBacklogCallback();
	CHECK(triggered);
	CHECK(storyPacket.use_count()==1);
	CHECK(connection->GetBytesQueuedToSend()==0);	// Make sure the bytes weren't queued;
}

TEST_CASE("ConnectionReceiveProcessSpeed")
{
	class FixedConnection : public Connection
	{
	public:
		FixedConnection(NetworkManager& manager) : Connection(manager)
		{
			// std::string myStory = "Once upon a time there was some data that needed to be sent to a far away land to bring joy to the eyes of the receiver."
			// " This, of course, was the purpose of data and so the data encapsulated itself into a packet and asked the Connection if it could be sent to"
			// " the destination. The path ahead was scary and unknown but the Connection assured the data that it would be sent over TCP and that there"
			// " were friendly devices along the way known as routers that will help guarantee a safe journey.";
			//std::string myStory = "Once upon a time";
			// std::string myStory = "Once upon a time there was some data that needed to be sent to a far away land to bring joy to the eyes of the receiver."
			// " This, of course, was the purpose of data and so the data encapsulated itself into a packet and asked the Connection if it could be sent to"
			// " the destination. The path ahead was scary and unknown but the Connection assured the data that it would be sent over TCP and that there"
			// " This, of course, was the purpose of data and so the data encapsulated itself into a packet and asked the Connection if it could be sent to"
			// " the destination. The path ahead was scary and unknown but the Connection assured the data that it would be sent over TCP and that there"
			// " This, of course, was the purpose of data and so the data encapsulated itself into a packet and asked the Connection if it could be sent to"
			// " the destination. The path ahead was scary and unknown but the Connection assured the data that it would be sent over TCP and that there"
			// " were friendly devices along the way known as routers that will help guarantee a safe journey.";
			std::string myStory;
			for(Size i =0; i < 1; ++i)
			{
				myStory+="abcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcd";
			}
			mPacket = NewDataPacket(1024*1024,myStory.length());
			mPacket->AppendData(myStory.c_str(),myStory.length());
		}
		virtual ~FixedConnection(){}

		virtual bool _Disconnect()
		{
			return true;
		}

		virtual Connection::SendResult Send(const u8*, int)
		{
			return Connection::SendResult{0,Connection::SendStatuses::DISCONNECT};
		}

		virtual ReceiveResult Receive(u8* buffer, int bufferSizeInBytes)
		{
			DataPacketHeader header;
			header.BuildForPacket(*mPacket);
			const Size numberOfPacketsAtOnce = 8192;
			Size totalBytes = (header.GetHeaderDataSizeInBytes()+mPacket->GetReceivedDataSize())*numberOfPacketsAtOnce;
			if(bufferSizeInBytes < totalBytes)
			{
				ECHO_LOG_ERROR("bufferSizeInBytes < totalBytes : " << bufferSizeInBytes << " < " << totalBytes);
				return Connection::ReceiveResult{0,ReceiveStatuses::DISCONNECT};
			}

			for(Size i = 0; i < numberOfPacketsAtOnce; ++i)
			{
				std::copy(header.GetHeaderData(),header.GetHeaderData() + header.GetHeaderDataSizeInBytes(),buffer);
				buffer+=header.GetHeaderDataSizeInBytes();
				std::copy(mPacket->GetData(),mPacket->GetData()+mPacket->GetReceivedDataSize(),buffer);
				buffer+=mPacket->GetReceivedDataSize();
			}
			return Connection::ReceiveResult{totalBytes,ReceiveStatuses::SUCCESS};
		}

		virtual bool _Connect() {return true;}
	private:
		shared_ptr<DataPacket> mPacket;
	};

	Kernel kernel;

	kernel.SetExecutionModel(make_shared<SimpleExecutionModel>());
	//kernel.GetFrameRateLimiter().SetMinFrameTime(Seconds(1.0/10.0));

	Thread kernelThread("NetworkThread",&kernel);
	
	NetworkManager networkManager;
	kernel.AddTask(networkManager);

	kernelThread.Execute();

	auto runTest = [&networkManager]()
	{
		shared_ptr<FixedConnection> fixedConnection = make_shared<FixedConnection>(networkManager);
		fixedConnection->Connect();
		Timer::CPUTimer timer;
		timer.Start();
		const Size totalIterations = 1000;
		for(Size i = 0; i < totalIterations; ++i)
		{
			fixedConnection->UpdateReceive();
			if(timer.GetElapsed()>Seconds(1))
			{
				const double megabyte = 1024.0 * 1024.0;
				ECHO_LOG_INFO("Send: " << (networkManager.GetBytesSentPerSecond()/megabyte) << "MiB/s Send total: " << (networkManager.GetTotalBytesSent()/megabyte) << "MiB Receive: " << (networkManager.GetBytesReceviedPerSecond()/megabyte) << "MiB/s Receive total: " << (networkManager.GetTotalBytesRecevied()/megabyte) << "MiB " << ((static_cast<double>(i)/static_cast<double>(totalIterations))*100) << "%");
				timer.Start();
			}
		}
	};
	std::vector< shared_ptr<Thread> > threads;
	const Size numRunners = 1;
	for(Size r=0;r < numRunners;++r)
	{
		threads.push_back(make_shared<Thread>("runner"+std::to_string(r),runTest));
	}

	for(shared_ptr<Thread>& runnerThread : threads)
	{
		runnerThread->Execute();
	}

	for(shared_ptr<Thread>& runnerThread : threads)
	{
		runnerThread->Join();
	}

	kernelThread.Terminate(true);

}

TEST_CASE("BoostASIOTCP")
{
	gDefaultLogger.SetLogMask(Echo::Logger::LogLevels::INFO | Echo::Logger::LogLevels::ERROR | Echo::Logger::LogLevels::WARNING);
	Kernel kernel;
	NetworkManager manager;
	shared_ptr<SocketNetworkSystem> socketSystem(new SocketNetworkSystem(manager));
	shared_ptr<BoostASIONetworkSystem> boostASIOSystem(new BoostASIONetworkSystem(manager));
	manager.InstallSystem(socketSystem, true);
	manager.InstallSystem(boostASIOSystem, false);
	
	ConnectionListener listener(manager,12346,false,false,"ASIO");
	
	REQUIRE(!listener.GetFailed());
	
	ConnectionListener listener2(manager,12346,true,false,"ASIO");
	shared_ptr<ExecutionModel> model = Platform::CreateExecutionModel();
	model->SetModelToUse(ExecutionModel::Models::NONE);

	//Close the application after 5 seconds.
	CountDownTimer timeout(Seconds(5.0),bind(&TaskManager::RemoveAllTasks,&kernel),"RemoveAllTasks");
	kernel.AddTask(timeout);
	
	kernel.SetExecutionModel(model);
	
	kernel.AddTask(manager);
	kernel.Execute();
}
