#include <echo/Application.h>
#include <doctest/doctest.h>

using namespace Echo;

TEST_CASE("Application")
{
	// Turn off log output, we will just use output from this test.
	gDefaultLogger.SetLogMask(Echo::Logger::LogLevels::NONE);
	Application application;
	Task task("ATask");
	application.AddTask(task);

	Configuration configuration;
	configuration.Set<u32>("resolution.x",800);
	configuration.Set<u32>("resolution.y",600);
	REQUIRE(application.Initialise("ApplicationTest", configuration));
	application.Execute();
}
