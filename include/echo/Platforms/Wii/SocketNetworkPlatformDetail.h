#include <echo/Network/SocketNetworkSystem.h>

namespace Echo
{
	struct SocketNetworkPlatformDetail
	{
		SocketNetworkPlatformDetail(){}

		bool Initialise()
		{
			ECHO_LOG_INFO("Initialising Networking...");
			char localIP[256] = {0};
			if(if_config(localIP,0,0,1,10) < 0)
			{
				ECHO_LOG_WARNING("Network access unavailable.");
				return false;
			}

			ECHO_LOG_INFO("Network access available. IP is " << localIP);
			return true;
		}

		~SocketNetworkPlatformDetail()
		{

		}

		static shared_ptr<SocketNetworkPlatformDetail> Create()
		{
			static weak_ptr<SocketNetworkPlatformDetail> globalWeakState;
			shared_ptr<SocketNetworkPlatformDetail> globalState = globalWeakState.lock();
			if(globalState)
			{
				return globalState;
			}
			globalState = make_shared<SocketNetworkPlatformDetail>();
			if(globalState->Initialise())
			{
				globalWeakState = globalState;
				return globalState;
			}
			return nullptr;
		}
	};
}
