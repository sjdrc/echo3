#include <echo/Util/SystemInformation.h>

#include <doctest/doctest.h>
#undef INFO

TEST_CASE("SystemInformation")
{
	// Turn off log output, we will just use output from this test.
	Echo::gDefaultLogger.SetLogMask(Echo::Logger::LogLevels::INFO);

	ECHO_LOG_INFO("SystemInformation Test");
	using namespace Echo::SystemInformation;
	std::vector< Processor > processors = GetCPUInformation();
	for(size_t i=0; i <processors.size() ; ++i)
	{
		std::cout << processors[i] << std::endl;
	}
	
	ECHO_LOG_INFO(GetOSInformation());
	size_t totalMemory = GetTotalMemeory();
	size_t memoryInUse = GetMemeoryInUse();
	CHECK(totalMemory>0);
	CHECK(memoryInUse>0);
	ECHO_LOG_INFO("Total Memory: " << totalMemory);
	ECHO_LOG_INFO("Memory in Use: " << memoryInUse);
}
