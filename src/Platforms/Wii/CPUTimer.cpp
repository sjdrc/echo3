#include <echo/Chrono/Chrono.h>
#include <echo/Chrono/CPUTimer.h>
#include <ogc/lwp_watchdog.h>

namespace Echo
{
	namespace Timer
	{
		void CPUTimer::InitialiseImplementation()
		{
		}

		void CPUTimer::CleanupImplementation()
		{
		}

		Nanoseconds CPUTimer::GetCPUTime()
		{
			//This is all we need to do.
			u64 t=gettime();
			return Nanoseconds(ticks_to_nanosecs(t));
		}
	}
}
