#ifndef _ECHOCOUNTDOWNTIMER_H_
#define _ECHOCOUNTDOWNTIMER_H_

#include <echo/Kernel/Task.h>
#include <echo/cpp/functional>
#include <boost/foreach.hpp>
#include <map>

namespace Echo
{
	/**
	 * A count down timer task.
	 * A CountDownTimer is used to call a function when a certain amount of time has passed.
	 * The timer is task driven so it will only count while active. Pausing the task either
	 * directly or through a task group will result in the timer pausing.
	 */
	class CountDownTimer : public Task
	{
	public:
		typedef function<void(void)> TimeoutFunction;
		
		/**
		 * Default constructor.
		 * The default timeout is 1 seconds.
		 */
		CountDownTimer() : mTimeout(1.f), mTimePassed(0.), mRepeat(false)
		{
		}
		
		/**
		 * Constructor.
		 * @param timeout Timeout in seconds.
		 */
		CountDownTimer(Seconds timeout) : mTimeout(timeout), mTimePassed(0.), mRepeat(false)
		{
		}
		
		/**
		 * Constructor.
		 * @param timeout Timeout in seconds.
		 * @param function A function to call when the timer expires. You can add more with AddTimeoutFunction().
		 * @param functionReference Function reference, see AddTimeoutFunction();
		 */
		CountDownTimer(Seconds timeout, TimeoutFunction function, const std::string& functionReference) :
			mTimeout(timeout), mTimePassed(0), mRepeat(false)
		{
			AddTimeoutFunction(function,functionReference);
		}
		
		/**
		 * Reset and start or resume the timer.
		 */
		void Reset()
		{
			mTimePassed = Seconds(0);
			Resume();
		}

		/**
		 * Set the timeout.
		 * The new timeout is used immediately but does not cause the timer to reset. So if 1 second has
		 * passed and the new timeout is 5 seconds, then the timer will expire in 4 seconds.
		 * @param seconds
		 */
		void SetTimeout(Seconds seconds)
		{
			mTimeout=seconds;
		}
		
		const Seconds& GetTimeout() const
		{
			return mTimeout;
		}
		
		/**
		 * Get whether the timer will repeat.
		 * @return 
		 */
		bool GetRepeat() const {return mRepeat;}

		/**
		 * Set whether the timer will repeat.
		 * By default repeat is off.
		 * @param repeat
		 */
		void SetRepeat(bool repeat) {mRepeat = repeat;}
		
		/**
		 * Functors called when the timer expires.
		 * @param function boost bind function to call when the function expires.
		 * @param functionReference reference for later removal. Choose a unique name as the same name
		 *		  will overwrite any existing references making it impossible to remove without
		 *		destroying the timer object.
		 */
		void AddTimeoutFunction(TimeoutFunction function, const std::string& functionReference)
		{
			mFunctions[functionReference] = function;
		}

		/**
		 * Remove timout functors.
		 * @param functionReference reference used when calling AddTimeoutFunction.
		 */
		void RemoveTimeoutFunction(const std::string& functionReference)
		{
			mFunctions.erase(functionReference);
		}
		
		void Update(Seconds lastFrameTime) override
		{
			mTimePassed+=lastFrameTime;
			if(mTimePassed>=mTimeout)
			{
				std::map< std::string, TimeoutFunction >::iterator it=mFunctions.begin();
				std::map< std::string, TimeoutFunction >::iterator itEnd=mFunctions.end();
				while(it!=itEnd)
				{
					//Avoid potential iterator invalidation if the callback wants to remove itself.
					std::map< std::string, TimeoutFunction >::iterator thisIt = it;
					it++;
					thisIt->second();
				}
				//Repeat if repeat is on and one of the callbacks didn't pause the task.
				if(mRepeat && !GetPaused())
				{
					Reset();
				}else
				{
					Pause();
				}
			}
		}
	private:
		Seconds mTimeout;
		Seconds mTimePassed;
		bool mRepeat;
		std::map< std::string, TimeoutFunction > mFunctions;
	};	
}
#endif
