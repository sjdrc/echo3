#include <echo/Platform.h>
#include <echo/Shell/Shell.h>
#include <echo/cpp/functional>

#include <doctest/doctest.h>
#undef INFO

using namespace Echo;

std::string stdOutputResult;

void StdOutCallback(const std::string& line)
{
	stdOutputResult=line;
	ECHO_LOG_INFO("StdOutCallback() was called: " << line);
}

void ExecuteTest()
{
	std::cout << "ShellTest execute test" << std::endl;
	shared_ptr<Shell> shell = Platform::CreateShell();
	REQUIRE(bool(shell));
	shell->SetStdOutCallback(bind(&StdOutCallback,placeholders::_2));
	int returnResult = shell->Execute("echo Hello World",false);
	CHECK(returnResult!=127);
	CHECK(stdOutputResult=="Hello World");
}

TEST_CASE("Shell")
{
	// Turn off log output, we will just use output from this test.
	gDefaultLogger.SetLogMask(Echo::Logger::LogLevels::INFO);

	ExecuteTest();
}

