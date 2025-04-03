#ifndef _ECHOFRAMERATELIMITER_H_
#define _ECHOFRAMERATELIMITER_H_

#include <echo/Chrono/CPUTimer.h>

namespace Echo
{
	/**
	 * A FrameRateLimiter can be used to limit the frame rate of a thread.
	 * It is designed to be used in two parts to accomplish the frame rate limiting:
	 * 
	 * 1. Call GetLastFrameTime() to get the last frame's time. This marks the
	 * start of the frame processing.
	 * 2. After frame processing, call Limit() to sleep the thread for a calculated
	 * amount of time based on the time your frame took to process.
	 * 
	 * Even though the two methods could be incorporated into one, they are kept
	 * separate for use cases where frame limiting should not be performed because
	 * it would interfere with the platform's execution. The Kernel deals with these
	 * types of executions and only calls Limit() when required.
	 * 
	 * Example simple usage:
	 *	FrameRateLimiter limiter(Seconds(1./60.),Seconds(1.));
	 * 
	 *	bool notFinished=true;
	 *	while(notFinished)
	 *	{
	 *		Seconds lastFrameTime = limiter.GetLastFrameTime();
	 *		ProcessFrames(lastFrameTime);
	 *		limiter.Limit();
	 *	}
	 * 
	 * @note The Kernel uses are FrameRateLimiter internally so it is unnecessary to
	 * implement one yourself. You can access the Kernel's FrameRateLimiter instance
	 * and modify the limits instead.
	 * @note The minimum frame time is only an approximation and is not guaranteed.
	 * FrameRateLimiter will reduce the actual sleep time for the thread to account
	 * for the fact that the operating system cannot instantaneously switch processes.
	 * Due to this, you may often end up with an extra frame count as the time the extra
	 * thread is awake would accumulates such that it is greater than or equal to the time
	 * your frame processing would take. This will be more obvious in applications where
	 * frame processing is very fast. It is easy to think about with a 1 frame per second
	 * rate, if your processing takes up 10ms then the thread should sleep for 990ms but
	 * if the adjustment was 1ms (it is not) then after 10 frames the extra awake time over
	 * 10 seconds would match the frame processing time resulting in 11 frames over 10
	 * seconds. The adjustment has been implemented to avoid frame rate halving due to
	 * CPU stalls while waiting on vsync if it is enabled.
 	 */
	class FrameRateLimiter
	{
	public:
		FrameRateLimiter(Seconds minFrameTime, Seconds maxFrameTime);
		
		/**
		 * Start the internal timer.
		 * If this method is not called the first call to GetLastFrameTime() will
		 * result in a time of 0.
		 */
		void Start()
		{
			mFrameTimer.Start();
		}
		
		/**
		 * Get the last frame time.
		 * @return The time in seconds that the last frame took to process.
		 */
		inline Seconds GetLastFrameTime()
		{
			return mLastFrameTime;
		}
		
		/**
		 * Sleep the current thread for a period of time to increase the frame
		 * time to approximately be the minimum frame time.
		 * This method is designed to be called after your frame has processed.
		 * This method will reset the frame timer.
		 */
		void Limit();

		/**
		 * Set the maximum frame time that can be returned by GetLastFrameTime().
		 * @param maxFrameTime The maximum frame time in seconds.
		 */
		void SetMaxFrameTime(Seconds maxFrameTime)
		{
			mMaxFrameTime = maxFrameTime;
		}

		const Seconds& GetMaxFrameTime() const
		{
			return mMaxFrameTime;
		}

		/**
		 * Set the minimum frame time.
		 * If the time between calling GetLastFrameTime() and Limit() is less than this amount
		 * the thread will be put to sleep during the call to Limit() for approximately
		 * minFrameTime - frameProcessingTime.
		 * @param minFrameTime The minimum time in seconds the frame should take to execute.
		 * @note This value is not used to enforce the minimum time that can be returned by
		 * GetLastFrameTime() as the max frame time does.
		 */
		void SetMinFrameTime(const Seconds& minFrameTime)
		{
			mMinFrameTime = minFrameTime;
		}

		const Seconds& GetMinFrameTime() const
		{
			return mMinFrameTime;
		}
	private:
		Seconds mMinFrameTime;
		Seconds mMaxFrameTime;
		Seconds mLastFrameTime;
		Timer::CPUTimer mFrameTimer;
	};
}
#endif
