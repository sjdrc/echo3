#include <echo/Types.h>
#include <echo/Chrono/CPUTimer.h>

namespace Echo
{
	namespace Timer
	{
		CPUTimer::CPUTimer() : mStartTime(0), mEndTime(0), mStarted(false)
		{
			InitialiseImplementation();
		}

		CPUTimer::~CPUTimer()
		{
			CleanupImplementation();
		}

		bool CPUTimer::IsStopped() const
		{
			return !mStarted;
		}

		bool CPUTimer::IsRunning() const
		{
			return mStarted;
		}

		Nanoseconds CPUTimer::GetElapsed()
		{
			if(IsStopped())
			{
				return mEndTime - mStartTime;
			}

			Nanoseconds currentTime = GetCPUTime();
			return currentTime - mStartTime;
		}

		void CPUTimer::Start()
		{
			mStartTime = GetCPUTime();
			mStarted=true;
		}

		Nanoseconds CPUTimer::Stop()
		{
			if(IsRunning())
			{
				mEndTime = GetCPUTime();
				mStarted=false;
			}
			return (mEndTime-mStartTime);
		}

		void CPUTimer::Resume()
		{
			if(IsStopped())
			{
				Nanoseconds currentTime = GetCPUTime();
				//Adjust the start time by the time we were paused.
				mStartTime += currentTime - mEndTime;
			}
		}
	}
}
