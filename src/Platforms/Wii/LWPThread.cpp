#include <echo/Kernel/Thread.h>
#include <ogc/lwp.h>

namespace Echo
{
	class Thread::Implementation
	{
	public:
		lwp_t mThread;
		Implementation() : mThread(0){}
		~Implementation(){}
		
		static void* RunThread(void* arg)
		{
			Thread* threadArg = reinterpret_cast<Thread*>(arg);
			threadArg->_Execute();
			return 0;
		}
	};

	void Thread::InitialiseImplementation()
	{
		mImplementation.reset(new Thread::Implementation());
	}
	
	void Thread::CleanImplementation()
	{
	}

	bool Thread::Execute()
	{
		if(mExecuting)
		{
			return false;
		}

		s32 r = LWP_CreateThread(&mImplementation->mThread,&Thread::Implementation::RunThread,(void*)this,0,0,0);
		if(r!=0)
		{
			ECHO_LOG_ERROR("Could not create thread: " << r);
			return false;
		}
		return true;
	}

	void Thread::Wait()
	{
		ECHO_LOG_ERROR("Not implemented");
	}

	void Thread::Suspend()
	{
		if(LWP_ThreadIsSuspended(mImplementation->mThread)==FALSE)
		{
			if(LWP_SuspendThread(mImplementation->mThread)!=0)
			{
				ECHO_LOG_ERROR("Could not suspend thread.");
			}
		}
	}

	void Thread::Continue()
	{
		if(LWP_ThreadIsSuspended(mImplementation->mThread)==TRUE)
		{
			if(LWP_ResumeThread(mImplementation->mThread)!=0)
			{
				ECHO_LOG_ERROR("Could not resume thread.");
			}
		}
	}

	void Thread::Notify()
	{
		ECHO_LOG_ERROR("Not implemented");
	}

	void Thread::Join()
	{
		u64 threadReturnValue=0;
		u64* threadReturnValuePtr=&threadReturnValue;
		if(LWP_JoinThread(mImplementation->mThread,(void**)&threadReturnValuePtr)<0)
		{
			ECHO_LOG_ERROR("Unable to join thread: " << mImplementation->mThread);
		}
	}
	
	void Thread::Sleep(Seconds sleepTime)
	{
		LWP_YieldThread();
	}
}
