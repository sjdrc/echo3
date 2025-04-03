#include <echo/Network/NetRedefinitions.h>
#include <echo/Network/SocketNetworkSystem.h>

namespace Echo
{
	struct SocketNetworkPlatformDetail
	{
		SocketNetworkPlatformDetail(){}
		
		bool Initialise()
		{
			//Need to initialise Winsock on windows
			WSADATA wsaData;
			ECHO_LOG_INFO("Starting Winsock v2.2");
			int wsaErr = WSAStartup(MAKEWORD(2, 2), &wsaData);
			if(!HandleError(wsaErr))
			{
				ECHO_LOG_ERROR("" << wsaErr << "(0x" << std::hex << wsaErr << std::dec << ")");
				WSACleanup();
				return false;
			}

			if(wsaData.wVersion != MAKEWORD(2, 2))
			{
				WSACleanup();
				ECHO_LOG_WARNING("Unable to start Winsock v2.2!");
				ECHO_LOG_INFO("Starting Winsock v1.1...");
				if(!HandleError(WSAStartup(MAKEWORD(1, 1), &wsaData)))
				{
					WSACleanup();
					ECHO_LOG_ERROR("Unable to start. Networking is unavailable...");
					return false;
				}
				return false;
			}
			ECHO_LOG_INFO("Done!");
		}

		~SocketNetworkPlatformDetail()
		{
			WSACleanup();
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
