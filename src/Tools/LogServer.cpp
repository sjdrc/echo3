#include <echo/Platform.h>
#include <echo/Network/NetworkManager.h>
#include <echo/Network/SocketNetworkSystem.h>
#include <echo/Network/NetworkExecutionModel.h>
#include <echo/Network/DataPacket.h>
#include <echo/Network/ConnectionOwner.h>
#include <echo/Network/Connection.h>

using namespace Echo;

class LogServer : public IncomingConnectionListener, public ConnectionOwner
{
private:
	std::list< shared_ptr<Connection> > mConnections;
	NetworkManager mNetworkManager;
public:
	bool Initialise(std::string listenPort)
	{
		shared_ptr<SocketNetworkSystem> socketSystem(new SocketNetworkSystem(mNetworkManager,1,2000));
		mNetworkManager.InstallSystem(socketSystem,true);

		std::stringstream ss;
		ss << "direct:ANY:" << listenPort;
		if(!mNetworkManager.Listen(this,ss.str()))
		{
			ECHO_LOG_ERROR("Failed to open listening socket on port " << listenPort);
			return false;
		}
		return true;
	}
	
	void IncomingConnection(shared_ptr<Connection> connection)
	{
		std::string friendlyIdentifier = connection->GetFriendlyIdentifier();
		ECHO_LOG_INFO("Server: Incoming Connection " << connection->GetFriendlyIdentifier());
		connection->RegisterLabelledPacketCallback("networkstream",[friendlyIdentifier](shared_ptr<Connection>, shared_ptr<DataPacket> packet, const u8* data, Size length)
			{
				// The strings are encoded in packets as bytesPerChar(4)length(4)content(length) so we'll skip 8 bytes.
				if(length>8)
				{
					std::cout << friendlyIdentifier << ": " << std::string(data+8,data+length);
				}
			});
		mConnections.push_back(connection);
	}

	void Run()
	{
		Kernel kenel;
		kenel.AddTask(mNetworkManager);
		//shared_ptr<ExecutionModel> model = Platform::CreateExecutionModel();
		//model->SetModelToUse(ExecutionModel::Models::NONE);
		shared_ptr<NetworkExecutionModel> model(new NetworkExecutionModel(kenel));
		kenel.SetExecutionModel(model);
		mNetworkManager.AddNetworkEventListener(model);
		kenel.Execute();
		mNetworkManager.RemoveNetworkEventListener(model);
	}
};

int main(int argc, const char* args[])
{
	if(argc<2)
	{
		ECHO_LOG_INFO("Usage: " << args[0] << " port");
		return 1;
	}
	LogServer logServer;
	if(!logServer.Initialise(args[1]))
	{
		return 1;
	}
	logServer.Run();
	return 0;
}
