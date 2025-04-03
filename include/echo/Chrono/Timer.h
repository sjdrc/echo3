#ifndef _ECHO_CPUTIMER_
#define _ECHO_CPUTIMER_

#include <echo/Chrono/Chrono.h>

namespace Echo
{
	namespace Timer
	{
		/**
		 * A CPU Timer allows you to measure time based on the CPU clock.
		 */
		class CPUTimer
		{
		public:
			CPUTimer();
			
			~CPUTimer();

			/**
			 * Get whether the timer is in the stopped state or not.
             * @return true if the timer is in the stopped state.
             */
			bool IsStopped() const;

			/**
			 * Get whether this timer is running or not.
             * @return true if the timer is running.
             */
			bool IsRunning() const;
			
			/**
			 * Get the time that this timer has been running.
			 */
			Nanoseconds GetElapsed();

			/**
			 * Start the timer.
			 * This method records the current CPU time in mTime.
             */
			void Start();

			/**
			 * Start the timer.
			 * @return the total elapsed time that the timer has been running.
             */
			Nanoseconds Stop();
			
			/**
			 * Resume the timer.
			 * If the timer is already running this method does nothing.
			 */
			void Resume();
		private:
			class Implementation;
			Implementation* mImplementation;
			Nanoseconds mStartTime;
			Nanoseconds mEndTime;
			bool mStarted;
			/**
			 * Initialise the implementation object.
			 * @note Platform developers should implement this method.
             */
			void InitialiseImplementation();
			/**
			 * Cleanup the implementation object.
			 * @note Platform developers should implement this method.
             */
			void CleanupImplementation();
			
			/**
			 * Get the CPU time in nanoseconds.
			 * @note Platform developers should implement this method.
             * @return the CPU time in nanoseconds.
             */
			Nanoseconds GetCPUTime();
		};
	}
}
#endif
