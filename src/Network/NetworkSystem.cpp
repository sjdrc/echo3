#include <echo/Network/NetworkSystem.h>
#include <echo/Network/NetworkManager.h>
#include <echo/Network/Connection.h>

namespace Echo
{
	void NetworkSystem::UpdateIncoming(shared_ptr<Connection> connection, IncomingConnectionListener* listener)
	{
		mNetworkManager.ConnectionIncoming(connection,listener);
	}

	void NetworkSystem::UpdateConnect( shared_ptr<Connection> connection )
	{
		if(connection->IsConnected())
		{
			mNetworkManager.ConnectionEstablished(connection);
		}else
		{
			mNetworkManager.ConnectionDropped(connection);
		}
	}

	void NetworkSystem::UpdateReceive( shared_ptr<Connection> connection )
	{
		connection->UpdateReceive();
	}

	void NetworkSystem::UpdateSend( shared_ptr<Connection> connection )
	{
		connection->UpdateSend(true);
	}
}
