#include <echo/Util/SystemInformation.h>

namespace Echo
{
	namespace SystemInformation
	{
		std::vector< Processor > GetCPUInformation()
		{
			std::vector< Processor > processors;
			processors.push_back(Processor());
			return processors;
		}

		size_t GetTotalMemeory()
		{
			ECHO_LOG_ERROR("GetTotalMemeory() is not implemented");
			return 0;
		}

		size_t GetMemeoryInUse()
		{
			ECHO_LOG_ERROR("GetMemeoryInUse() is not implemented");
			return 0;
		}
		
		OS GetOSInformation()
		{
			ECHO_LOG_ERROR("GetOSInformation() is not implemented");
			return OS();
		}
	};
}
