#include <echo/Chrono/Chrono.h>
#include <echo/Chrono/CPUTimer.h>
#include <boost/timer/timer.hpp>

namespace Echo
{
	namespace Timer
	{
		class CPUTimer::Implementation
		{
		public:
			boost::timer::cpu_timer mTimer;
		};

		void CPUTimer::InitialiseImplementation()
		{
			//boost cpu_timers start as soon as you create them.
			mImplementation.reset(new Implementation());
		}

		void CPUTimer::CleanupImplementation()
		{
		}

		Nanoseconds CPUTimer::GetCPUTime()
		{
			return Nanoseconds(mImplementation->mTimer.elapsed().wall);
		}				
	}
}
