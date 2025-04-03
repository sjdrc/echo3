#ifndef _ECHOKERNEL_H_
#define _ECHOKERNEL_H_
#include <list>
#include <string>
#include <echo/Chrono/FrameRateLimiter.h>

#include <echo/Types.h>
#include <echo/Kernel/TaskGroup.h>
#include <echo/Kernel/Task.h>

namespace Echo
{
	//////////////////////////////////////////////////////////////////////////
	//Forward Declarations
	class Task;
	class TaskThread;
	class ExecutionModel;

	class Kernel : public TaskManager
	{
	public:
		static const Seconds DEFAULT_MIN_FRAME_TIME;	//The default minimum frame time for the FrameRateLimiter.
		static const Seconds DEFAULT_MAX_FRAME_TIME;	//The default maximum frame time for the FrameRateLimiter.
		
		Kernel();
		virtual ~Kernel();

		bool AddTaskThread(TaskThread* t);
		void RemoveTaskThread(TaskThread* t);

		void PauseKernel(bool applicationPause = false);
		void ResumeKernel(bool applicationResume = false);
		
		/**
		 * Stop execution of the kernel.
		 * Execution stops when all tasks are removed from the Kernel task list (just like a TaskManager).
		 * This is the same as calling TaskManager::RemoveAllTasks().
		 */
		void Stop();

		//ProcessFrame() is called from ExecutionModel::CONTROLLER implementations.
		//See ExecutionModel.h for more information.
		//Unless you adding platform support to Echo you can safely ignore this function.
		//ProcessFrame() updates and Renders().
		bool ProcessFrame();

		struct ExecutionResults
		{
			enum _
			{
				SUCCESS,
				FAILURE,
				INCOMPLETE
			};
		};
		typedef ExecutionResults::_ ExecutionResult;

		ExecutionResult Execute();
		
		void SetExecutionModel(shared_ptr<ExecutionModel> executionModel) {mExecutionModel=executionModel;}
		shared_ptr<ExecutionModel> GetExecutionModel() {return mExecutionModel;}
		
		/**
		 * Set a flag to hint to the platform that the kernel would prefer not to pause.
		 * The default behaviour is to want a pause.
		 * On desktop platforms where your window can lose focus often the behaviour is to send an application pause.
		 * This isn't always the desired behaviour, setting this flag will tell the platform that you would prefer
		 * not to pause.
		 * @note Not all platforms support not pausing. For example, pausing is always done on Android and iOS.
		 * @param allow
		 */
		void SetPauseHint(bool allow) {mPauseHint=allow;}
		
		/**
		 * Get the pause hint flat.
		 * Platforms usually decide an application should pause when it loses focus. This usually results in
		 * Platform implementers should call this method before pausing a kernel. This is only a hind though
		 * the platform implementation ultimately determines if a kernel needs to be paused.
		 * @see SetPauseHint
		 * @return 
		 */
		bool GetPauseHint() const {return mPauseHint;}
		
		/**
		 * Get the frame rate limiter object.
		 * This allows you to modify the frame limiter parameters.
		 * @return the frame rate limiter.
		 */
		FrameRateLimiter& GetFrameRateLimiter() {return mFrameLimiter;}
	private:
		shared_ptr<ExecutionModel> mExecutionModel;
		std::list< TaskThread* > mThreads;
		FrameRateLimiter mFrameLimiter;
		bool mPauseHint;
		/**
		 * Stop all tasks and threads.
		 * This method performs the stopping logic. If you're looking to stop execution you should call Stop().
		 */
		void StopExecution();
	};
}
#endif
