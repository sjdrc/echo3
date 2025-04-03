#include <echo/Kernel/Kernel.h>
#include <echo/Kernel/TaskThread.h>
#include <echo/Kernel/ExecutionModel.h>

#include <iostream>
#include <algorithm>

#include <echo/Chrono/FrameRateLimiter.h>

namespace Echo
{
	const Seconds Kernel::DEFAULT_MIN_FRAME_TIME=Seconds(1./60.);
	const Seconds Kernel::DEFAULT_MAX_FRAME_TIME=Seconds(1.);
	
	Kernel::Kernel() : mFrameLimiter(DEFAULT_MIN_FRAME_TIME,DEFAULT_MAX_FRAME_TIME),  mPauseHint(true)
	{
		SetTaskManagerName("Kernel");
	}

	Kernel::~Kernel()
	{
	}

	bool Kernel::AddTaskThread(TaskThread* t)
	{
		if(std::find(mThreads.begin(), mThreads.end(), t) == mThreads.end())
		{
			mThreads.push_back(t);
			return true;
		}
		return false;
	}

	void Kernel::RemoveTaskThread(TaskThread* t)
	{
		mThreads.remove(t);
	}

	bool Kernel::ProcessFrame()
	{
		TaskManager::UpdateTasks(mFrameLimiter.GetLastFrameTime());

		if(mExecutionModel)
		{
			u32 attempt = 0;
			while(!mExecutionModel->SendUpdateRequest())
			{
				++attempt;
				if(attempt == 10)
				{
					ECHO_LOG_ERROR("Failed to send Update Request via execution model. Tried 10 times");
					break;
				}
			}
		}
		return HasAtLeastOneTask();
	}

	Kernel::ExecutionResult Kernel::Execute()
	{
		if(!mExecutionModel)
		{
			ECHO_LOG_ERROR("Failed to execute. ExecutionModel not set. Use SetExecutionModel() before executing the kernel.");
			return ExecutionResults::FAILURE;
		}
		mFrameLimiter.Start();
		StartTasks();

		switch(mExecutionModel->GetCurrentModel())
		{
			case ExecutionModel::Models::NONE:
				while(HasAtLeastOneTask())
				{
					TaskManager::UpdateTasks(mFrameLimiter.GetLastFrameTime());
					mFrameLimiter.Limit();
				}
				break;
			case ExecutionModel::Models::COOPERATE:
				while(HasAtLeastOneTask())
				{
					TaskManager::UpdateTasks(mFrameLimiter.GetLastFrameTime());
					mExecutionModel->ProcessEvents(1.0f / 60.0f);
					mFrameLimiter.Limit();
				}
				break;
			case ExecutionModel::Models::CONTROLLER:
				if(!mExecutionModel->EnterSystemEventManager())
				{
					ECHO_LOG_ERROR("Failed to execute. ExecutionModel does not support Models::CONTROLLER.");
					StopExecution();
					return ExecutionResults::FAILURE;
				}
				break;
			case ExecutionModel::Models::EXTERNAL_CONTROLLER:
			{
				//Model EXTERNAL_CONTROLLER means something else will call ProcessFrame();
				return ExecutionResults::INCOMPLETE;
			}
		}
		StopExecution();
		return ExecutionResults::SUCCESS;
	}
	
	void Kernel::StopExecution()
	{
		StopTasks();
		if(!mThreads.empty())
		{
			ECHO_LOG_INFO("About to kill " << mThreads.size() << " threads");
			//Now wait for threads
			while(!mThreads.empty())
			{
				TaskThread* t = mThreads.front();
				t->Terminate(true);
			}
			ECHO_LOG_INFO("All threads terminated");
		}
	}

	void Kernel::PauseKernel(bool applicationPause)
	{
		PauseAllActiveTasks(applicationPause);
		std::list < TaskThread* >::iterator it = mThreads.begin();
		std::list < TaskThread* >::iterator itEnd = mThreads.end();
		while(it != itEnd)
		{
			(*it)->PauseAllActiveTasks(applicationPause);
			++it;
		}
	}

	void Kernel::ResumeKernel(bool applicationResume)
	{
		ResumeAllPreviouslyActiveTasks(applicationResume);
		std::list < TaskThread* >::iterator it = mThreads.begin();
		std::list < TaskThread* >::iterator itEnd = mThreads.end();
		while(it != itEnd)
		{
			(*it)->ResumeAllPreviouslyActiveTasks(applicationResume);
			++it;
		}
	}
	
	void Kernel::Stop()
	{
		TaskManager::RemoveAllTasks();
	}
}
