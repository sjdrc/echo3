/**
 * This file includes the appropriate platform stub header file which should be
 * included only once in your application. If you are creating a library do not
 * include the platform stub.
 * 
 * The platform stub will provide the appropriate entry point for the target
 * platform. Platforms that do not use main() as an entry point and others, such
 * as those platforms that only supports the EXTERNAL_CONTROLLER controller
 * execution model, cannot be tied up in an entry point function. This is why
 * we use the EchoInitialise() function which will return an Kernel for the
 * platform to execute appropriately.
 * 
 * The stub code does not need to be used. If your application has special
 * requirements then you can manage the execution yourself. Keep in mind that
 * applications not using the standard platform stub code cannot be guaranteed
 * to build and run successfully across all supported platforms.
 */
#if defined (ECHO_PLATFORM_LINUX)
#include <echo/Platforms/EntryPointMain.h>
#endif
#if defined (ECHO_PLATFORM_NINTENDO_WII)
#include <echo/Platforms/Wii/EntryPoint.h>
#endif
#if defined (ECHO_PLATFORM_WINDOWS)
#include <echo/Platforms/Windows/EntryPointWinMain.h>
#endif
