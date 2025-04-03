#ifndef _ECHOTASKTHREAD_H_
#define _ECHOTASKTHREAD_H_
#include <echo/Types.h>
#include <echo/Kernel/Kernel.h>
#include <echo/Kernel/Thread.h>

namespace Echo
{
	class TaskThread : public Kernel
	{
	public:
		TaskThread(const std::string& threadName) : mThread(threadName,this){}
		~TaskThread()
		{
			Terminate(true);
		}
		bool Execute()
		{
			return mThread.Execute();
		}
		void Wait()
		{
			mThread.Wait();
		}
		void Suspend()
		{
			mThread.Suspend();
		}
		void Continue()
		{
			mThread.Continue();
		}
		void Notify()
		{
			mThread.Notify();
		}
		void Join()
		{
			mThread.Join();
		}
		void Terminate(bool waitForTermination = false)
		{
			mThread.Terminate(waitForTermination);
		}

		void SetThreadCompleteCallback(Thread::ThreadCompleteCallback callback)
		{
			mThread.SetCompleteCallback(callback);
		}
		
	private:
		Thread mThread;
	};
}
#endif
