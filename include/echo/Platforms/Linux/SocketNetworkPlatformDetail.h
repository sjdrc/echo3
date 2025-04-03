#include <echo/Network/SocketNetworkSystem.h>

namespace Echo
{
	struct SocketNetworkPlatformDetail
	{
		SocketNetworkPlatformDetail(){}

		bool Initialise()
		{
			return true;
		}

		~SocketNetworkPlatformDetail()
		{
		}

		static shared_ptr<SocketNetworkPlatformDetail> Create()
		{
			return make_shared<SocketNetworkPlatformDetail>();
			// static weak_ptr<SocketNetworkPlatformDetail> globalWeakState;
			// shared_ptr<SocketNetworkPlatformDetail> globalState = globalWeakState.lock();
			// if(globalState)
			// {
			// 	return globalState;
			// }
			// globalState = make_shared<SocketNetworkPlatformDetail>();
			// if(globalState->Initialise())
			// {
			// 	globalWeakState = globalState;
			// 	return globalState;
			// }
			// return nullptr;
		}
	};
}
