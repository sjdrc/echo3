#include <echo/Kernel/Thread.h>
#include <echo/cpp/thread>
#include <algorithm>
#include <iostream>
#include <thread>

namespace Echo
{
	class Thread::Implementation
	{
	public:
		Implementation() : mNotified(false), mThreadsWaiting(0){}
		~Implementation()
		{
			if(mThread.joinable())
			{
				mThread.join();
			}
		}
		thread mThread;
		condition_variable mCondition;
		mutex mMutex;
		bool mNotified;
		Size mThreadsWaiting;
	};

	void Thread::InitialiseImplementation()
	{
		mImplementation.reset(new Thread::Implementation());
	}
	
	void Thread::CleanImplementation()
	{
		mImplementation.reset();
	}

	bool Thread::Execute()
	{
		if(mExecuting)
		{
			ECHO_LOG_ERROR("Thread \"" << GetName() << "\" already executing.");
			return false;
		}

		mImplementation->mThread = thread(bind(&Thread::_Execute, this));
		return true;
	}

	void Thread::Wait()
	{
		std::unique_lock<std::mutex> lock(mImplementation->mMutex);
		mImplementation->mThreadsWaiting++;
		mImplementation->mCondition.wait(lock, [this](){return (mImplementation->mNotified && mImplementation->mThreadsWaiting > 0);});
		mImplementation->mThreadsWaiting--;
		if(mImplementation->mThreadsWaiting==0)
		{
			mImplementation->mNotified = false;
		}
	}

	void Thread::Suspend()
	{
		ECHO_LOG_ERROR("Not implemented");
	}

	void Thread::Continue()
	{
		ECHO_LOG_ERROR("Not implemented");
	}

	void Thread::Notify()
	{
		std::unique_lock<std::mutex> lock(mImplementation->mMutex);
		mImplementation->mNotified = true;
		mImplementation->mCondition.notify_all();
	}

	void Thread::Join()
	{
		if(mImplementation->mThread.joinable())
		{
			mImplementation->mThread.join();
		}
	}
	
	void Thread::Sleep(Seconds sleepTime)
	{
		this_thread::sleep_for(ThreadSeconds(sleepTime.count()));
	}
}
