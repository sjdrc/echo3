#include <echo/Chrono/FrameRateLimiter.h>
#include <echo/Kernel/Thread.h>

namespace Echo
{
	FrameRateLimiter::FrameRateLimiter(Seconds minFrameTime, Seconds maxFrameTime) :
		mMinFrameTime(minFrameTime),
		mMaxFrameTime(maxFrameTime),
		mLastFrameTime(0.)
	{
	}

	void FrameRateLimiter::Limit()
	{
		mLastFrameTime = mFrameTimer.GetElapsed();
		if(mLastFrameTime>mMaxFrameTime)
		{
			mLastFrameTime = mMaxFrameTime;
		}
		mFrameTimer.Start();

		// The thread needs to stop processing for a certain amount of time. The 500 microseconds is a little
		// arbitrary but a value I found to work.
		// The start of the frame timing is when GetLastFrameTime() is called so we use the timer's current
		// elapsed time as it is the time of the current frame.
		Seconds sleepTime = (mMinFrameTime - mFrameTimer.GetElapsed()) - Microseconds(500.0);
		if(sleepTime>Seconds(0.))
		{
			Thread::Sleep(sleepTime);
		}
	}
}
