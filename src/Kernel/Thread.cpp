#include <echo/Kernel/Thread.h>
#include <algorithm>
#include <iostream>
#include <echo/Kernel/Kernel.h>
#ifdef ECHO_PLATFORM_LINUX
#include <sys/prctl.h>
#endif

namespace Echo
{
	Thread::Thread(const std::string& name) :
		mKernel(nullptr),
		mName(name),
		mTerminate(false),
		mExecuting(false)
	{
		InitialiseImplementation();
	}

	Thread::Thread(const std::string& name, Kernel* kernel) :
		mKernel(kernel),
		mName(name),
		mTerminate(false),
		mExecuting(false)
	{
		InitialiseImplementation();
	}

	Thread::Thread(const std::string& name, ThreadFunction threadFunction) :
		mKernel(nullptr),
		mThreadFunction(threadFunction),
		mName(name),
		mTerminate(false),
		mExecuting(false)
	{
		InitialiseImplementation();
	}


	Thread::~Thread()
	{
		if(mExecuting)
		{
			ECHO_LOG_ERROR("Thread is being destroyed while still executing. You should ensure you stop threads before clean up. We cannot automatically join here as it could deadlock.");
		}
		CleanImplementation();
	}

	const std::string& Thread::GetName()
	{
		return mName;
	}

	void Thread::_Execute()
	{
		#ifdef ECHO_PLATFORM_LINUX
		::prctl(PR_SET_NAME, GetName().c_str(), 0, 0, 0);
		#endif
		
		//Has the thread function been set?
		if(mThreadFunction)
		{
			ECHO_LOG_DEBUG("Thread \"" << GetName() << "\" executing bound function.");
			mExecuting=true;
			mThreadFunction();
			mExecuting=false;
			if(mThreadCompleteCallback)
			{
				mThreadCompleteCallback(*this);
			}
			ECHO_LOG_DEBUG("Thread completed: " << GetName());
			return;
		}
		
		if(!mKernel)
		{
			ECHO_LOG_ERROR("Neither a thread function or Kernel was provided for the thread: " << GetName());
			return;
		}
		
		if(mKernel->GetExecuting())
		{
			ECHO_LOG_ERROR("The provided Kernel object is already executing so this thread will exit: " << GetName());
			return;
		}
		
		if(!mKernel->GetExecutionModel())
		{
			//No execution model was found so lets set a continuous one.
			mKernel->SetExecutionModel(shared_ptr<ThreadExecutionModel>(new ThreadExecutionModel()));
		}
		
		mExecuting=true;
		
		ECHO_LOG_DEBUG("Thread \"" << GetName() << "\" executing a kernel.");
		mKernelExecutionResult=mKernel->Execute();
		mExecuting=false;
		switch(mKernelExecutionResult)
		{
			case Kernel::ExecutionResults::SUCCESS:
				ECHO_LOG_DEBUG("Thread \"" << GetName() << "\" completed successfully.");
			break;
			case Kernel::ExecutionResults::FAILURE:
				ECHO_LOG_DEBUG("Thread \"" << GetName() << "\" completed with failure.");
			break;
			case Kernel::ExecutionResults::INCOMPLETE:
				ECHO_LOG_DEBUG("Thread \"" << GetName() << "\" completed with the Kernel status INCOMPLETE.");
			break;
		}
		if(mThreadCompleteCallback)
		{
			mThreadCompleteCallback(*this);
		}
	}

	void Thread::Terminate(bool waitForTermination)
	{
		if(!mExecuting)
		{
			return;
		}
		ECHO_LOG_DEBUG("Setting kill flag for thread: " << GetName());
		if(mKernel)
		{
			mKernel->Stop();
		}
		if(waitForTermination)
		{
			ECHO_LOG_DEBUG("Waiting for termination of thread: " << GetName());
			Join();
			ECHO_LOG_DEBUG("Thread " << GetName() << " terminated");
		}
	}
}
