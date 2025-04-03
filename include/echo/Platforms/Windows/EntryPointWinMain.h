#include <echo/Kernel/Kernel.h>
#include <windows.h>
#undef ERROR

shared_ptr<Echo::Kernel> EchoInitialise();

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nShowCmd)
{
	shared_ptr<Echo::Kernel> kernel = EchoInitialise();
	if(kernel)
	{
		kernel->Execute();
		return 0;
	}
	ECHO_LOG_ERROR("Initialisation failed");
	return 1;
}
