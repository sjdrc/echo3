#ifndef ECHO_PROFILERAVERAGER_H
#define ECHO_PROFILERAVERAGER_H

#include <echo/Kernel/Task.h>
#include <echo/Chrono/Profiler.h>

namespace Echo
{
	/**
	 * Average the results profilers.
	 */
	class ProfilerAverager
	{
	public:
		ProfilerAverager() : mHasStarted(false)
		{
		}
		
		~ProfilerAverager()
		{
		}
		
		struct Accumulation
		{
			Milliseconds mTime;
			Size mSamples;
		};
		typedef Accumulation Average;
		typedef std::map< std::string, Accumulation > CheckpointAccumulations;
		typedef std::map< std::string, Average > CheckpointAverages;
		
		/**
		 * Start point to average from.
		 */
		inline void Start(std::string name)
		{
			mName = name;
			mCheckpointAccumulations.clear();
			mHasStarted = true;
		}
		
		inline bool HasStarted() const
		{
			return mHasStarted;
		}
		
		inline void Report(const Profiler& profiler)
		{
			for(const auto& checkpoint : profiler.GetCheckpoints())
			{
				auto it = mCheckpointAccumulations.find(checkpoint.first);
				if(it!=mCheckpointAccumulations.end())
				{
					it->second.mTime += checkpoint.second;
					it->second.mSamples++;
				}else
				{
					mCheckpointAccumulations.insert({checkpoint.first,Accumulation{checkpoint.second,1}});
				}
			}
		}
		
		/**
		 * Stop the profiler.
		 */
		inline void Stop()
		{
			mHasStarted = false;
			mLastAverages.clear();
			for(const auto& checkpoint : mCheckpointAccumulations)
			{
				mLastAverages.insert({checkpoint.first, {Milliseconds(checkpoint.second.mTime.count() / checkpoint.second.mSamples),checkpoint.second.mSamples}});
			}
		}
		
		const std::string GetName() const
		{
			return mName;
		}
		
		const CheckpointAccumulations& GetCheckpointAccumulations() const
		{
			return mCheckpointAccumulations;
		}

		const CheckpointAverages& GetCheckpointAverages() const
		{
			return mLastAverages;
		}
	private:
		std::string mName;
		CheckpointAccumulations mCheckpointAccumulations;
		CheckpointAverages mLastAverages;
		bool mHasStarted;
	};
	
	inline std::ostream& operator<<(std::ostream& o, const ProfilerAverager& profiler)
	{
		o << profiler.GetName() << ": ";
		bool first = true;
		const ProfilerAverager::CheckpointAverages& averages = profiler.GetCheckpointAverages();
		for(const auto& checkpoint : averages)
		{
			if(!first)
			{
				o << " | ";
			}
			o << checkpoint.first << ": " << checkpoint.second.mTime << "(" << checkpoint.second.mSamples <<")";
			first = false;
		}
		return o;
	}
}

#endif

