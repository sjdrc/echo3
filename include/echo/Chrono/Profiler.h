#ifndef ECHO_PROFILER_H
#define ECHO_PROFILER_H

#include <echo/Types.h>
#include <echo/Chrono/CPUTimer.h>

namespace Echo
{
	/**
	 * A Simple profiler class
	 * There are two main features of this implementation:
	 * 
	 * - Profiling with checkpoints
	 * - Checkpoint averaging
	 * 
	 * Example usage:
	 *	Profiler myProfiler;
	 * 
	 *	myProfiler.Start("Coffee");
	 * 	myProfiler.Checkpoint("Grind Coffee");
	 * 	myProfiler.Checkpoint("Extract Shot");
	 *	myProfiler.Checkpoint("Steam Milk");
	 * 	myProfiler.Stop();
	 * 	ECHO_LOG_INFO(myProfiler);
	 * 
	 * The output will be something like:
	 * 		Coffee: Grind Coffee: 15564milliseconds | Extract Shot: 21345milliseconds | Steam Milk: 68997 | Total: 105906
	 */
	class Profiler
	{
	public:
		typedef std::pair<std::string, Milliseconds > Checkpoint;
		
		/**
		 * Start the profiler.
		 * @param name The name of the profiling session. This will be used in output.
		 */
		inline void Start(std::string name)
		{
			mName = name;
			mTimer.Start();
			mCheckpoints.resize(0);
			mTotalTime = Milliseconds(0);
		}
		
		/**
		 * Add a checkpoint.
		 * Each checkpoint will record the time since the last checkpoint.
		 * @param name the checkpoint name.
		 */
		inline void SaveCheckpoint(std::string name)
		{
			Milliseconds elapsed = mTimer.GetElapsed();
			mTotalTime += elapsed;
			mCheckpoints.push_back(Checkpoint(std::move(name),elapsed));
			mTimer.Start();
		}
		
		/**
		 * Stop the profiler.
		 * This adds one last checkpoint called "Final" and also records the total time.
		 */
		inline void Stop()
		{
			Milliseconds elapsed = mTimer.Stop();
			mCheckpoints.push_back(Checkpoint("Final",elapsed));
			mTotalTime += elapsed;
			mCheckpoints.push_back(Checkpoint("Total",mTotalTime));
		}
		
		const std::string GetName() const
		{
			return mName;
		}
		
		const std::vector< Checkpoint >& GetCheckpoints() const
		{
			return mCheckpoints;
		}
	private:
		Echo::Timer::CPUTimer mTimer;
		Milliseconds mTotalTime;
		std::string mName;
		std::vector< Checkpoint > mCheckpoints;
	};
	
	inline std::ostream& operator<<(std::ostream& o, const Profiler& profiler)
	{
		o << profiler.GetName() << ": ";
		bool first = true;
		for(auto& checkpoint : profiler.GetCheckpoints())
		{
			if(!first)
			{
				o << " | ";
			}
			o << checkpoint.first << ": " << checkpoint.second;
			first = false;
		}
		return o;
	}
}

#endif

