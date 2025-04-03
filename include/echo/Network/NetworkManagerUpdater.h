#ifndef _ECHONETWORKMANAGERUPDATER_H_
#define _ECHONETWORKMANAGERUPDATER_H_

#include <echo/Network/NetworkEventListener.h>
#include <echo/Chrono/CPUTimer.h>

namespace Echo
{
	class NetworkManager;
	/**
	 * Update a network manager on a network event.
	 * This allows you to create a NetworkManager and use it without a Kernel.
	 */
	class NetworkManagerUpdater : public NetworkEventListener
	{
	public:
		NetworkManagerUpdater(NetworkManager& networkManager);
		~NetworkManagerUpdater();

		void OnNetworkEvent(NetworkEventType eventType);
	private:
		NetworkManager& mNetworkManager;
		Timer::CPUTimer mFrameTimer;
	};
}
#endif
