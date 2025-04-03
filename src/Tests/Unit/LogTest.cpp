#include <echo/Util/StringUtils.h>
#include <echo/Util/Utils.h>

#include <doctest/doctest.h>
#undef INFO

TEST_CASE("Logger")
{
	using namespace Echo;
	// This test is just manually verifable
	// TODO: Capture the output and compare

	Logger::LogMask customMask = gDefaultLogger.RegisterCustomLogLevel("CUSTOM");
	gDefaultLogger.SetLogMask(customMask | Logger::LogLevels::INFO);
	ECHO_LOG_INFO("This is just information");
	ECHO_LOG("CUSTOM","This should appear in the log as CUSTOM");

	gDefaultLogger.SetLogOutputStream("cout2",&std::cout);
	ECHO_LOG("CUSTOM","This should appear twice as we output to the default and our new cout2");

	gDefaultLogger.RemoveLogOutputStream("stdout");
	ECHO_LOG("CUSTOM","This should appear once as we removed stdout (default) leaving the custom cout2");

	gDefaultLogger.RemoveLogOutputStream("cout2");
	ECHO_LOG("CUSTOM","This should not appear.");
}
