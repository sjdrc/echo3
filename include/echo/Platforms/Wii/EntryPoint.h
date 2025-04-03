#include <echo/Kernel/Kernel.h>
#include <debug.h>

Echo::shared_ptr<Echo::Kernel> EchoInitialise();

int main(int argc, char** argv)
{
	#ifdef ECHO_DEBUG
	Echo::shared_ptr<NetworkManager> debugNetworkManager = make_shared<NetworkManager>();
	debugNetworkManager->InstallSystem(Echo::make_shared<SocketNetworkSystem>(*debugNetworkManager),true);

	DEBUG_Init(GDBSTUB_DEVICE_TCP, 5656);
	_break();
	#endif
	Echo::shared_ptr<Echo::Kernel> kernel = EchoInitialise();
	if(kernel)
	{
		#ifdef ECHO_DEBUG
		if(debugNetworkManager)
		{
			// Keep our debug network manager alive
			kernel->AddTask(debugNetworkManager);
		}
		#endif
		kernel->Execute();
		return 0;
	}
	using namespace Echo;
	ECHO_LOG_ERROR("Initialisation failed");
	return 1;
}
