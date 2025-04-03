#include <MyProjectApplication.h>

// This include determines the appropriate entry point based on the platform.
// For most platforms it will include a file that defines main(). Make sure this file
// is only included once in your project. We recommend keeping this include in main.cpp.
#include <echo/PlatformStub.h>

using namespace MyProject;

shared_ptr<Kernel> EchoInitialise()
{
	// Create an instance of ThirdPersonGame, initialise it and return.
	shared_ptr<MyProjectApplication> application(new MyProjectApplication());
	if (application->Initialise())
	{
		return application;
	}
	// Failure to initialise an Application object will occur if the target platform has an incomplete configuration.
	return shared_ptr<Kernel>();
}
