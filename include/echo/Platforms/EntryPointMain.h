#include <echo/Kernel/Kernel.h>

Echo::shared_ptr<Echo::Kernel> EchoInitialise();

int main(int argc, char** argv)
{
	Echo::shared_ptr<Echo::Kernel> kernel = EchoInitialise();
	if(kernel)
	{
		kernel->Execute();
		return 0;
	}
	using namespace Echo;
	ECHO_LOG_ERROR("Initialisation failed");
	return 1;
}
