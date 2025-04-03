#include <echo/Network/NetworkManager.h>
#include <echo/Network/NetworkManagerUpdater.h>

namespace Echo
{
	NetworkManagerUpdater::NetworkManagerUpdater(NetworkManager& networkManager) : mNetworkManager(networkManager)
	{
	}

	NetworkManagerUpdater::~NetworkManagerUpdater()
	{
	}

	void NetworkManagerUpdater::OnNetworkEvent(NetworkEventType)
	{
		Seconds frameTime = mFrameTimer.GetElapsed();
		mFrameTimer.Start();
		mNetworkManager.Update(frameTime);
	}
}
