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
			return 0;
		}

		size_t GetMemeoryInUse()
		{
			return 0;
		}
		
		OS GetOSInformation()
		{
			return OS();
		}
	};
}
