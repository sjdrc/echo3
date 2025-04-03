#include <echo/Kernel/Task.h>
#include <echo/Kernel/TaskManager.h>

#include <iostream>
#include <algorithm>

namespace Echo
{
	Task::Task(u32 priority) :
		mHasStarted(false),
		mPausable(true),
		mPaused(false),
		mPriority(priority),
		mTaskName(""),
		mManagers()
	{
	}
	
	Task::Task(const std::string& taskName, u32 priority) :
		mHasStarted(false),
		mPausable(true),
		mPaused(false),
		mPriority(priority),
		mTaskName(taskName),
		mManagers()
	{
	}
	
	Task::Task(const Task& other)
		:
		mHasStarted(other.mHasStarted),
		mPausable(other.mPausable),
		mPaused(other.mPaused),
		mPriority(other.mPriority),
		mTaskName(other.mTaskName),
		mManagers(other.mManagers)
	{
	}
	
	Task::~Task()
	{
		while(!mManagers.empty())
		{
			TaskManager* manager = mManagers.front();
			manager->RemoveTask(this);
			mManagers.remove(manager);
		}
	}

	bool Task::IsInManager(const TaskManager& manager) const
	{
		if(std::find(mManagers.begin(), mManagers.end(), &manager) == mManagers.end())
			return false;
		return true;
	}
	
	TaskManager* Task::GetSingleTaskManager() const
	{
		if(mManagers.size()==1)
		{
			return mManagers.front();
		}
		return nullptr;
	}

	bool Task::Start()
	{
		if(mHasStarted)
		{
			return true;
		}
		if(OnStart())
		{
			mHasStarted = true;
			return true;
		}
		return false;
	}
	
	void Task::Stop()
	{
		if(mHasStarted)
		{
			OnStop();
			mHasStarted = false;
		}
	}

	void Task::Pause(bool applicationPause, bool forcePause)
	{
		if(GetPausable() || forcePause)
		{
			if(!GetPaused())
			{
				SetPaused(true);
				std::list<TaskManager*>::iterator it = mManagers.begin();
				std::list<TaskManager*>::iterator itEnd = mManagers.end();
				while(it != itEnd)
				{
					TaskManager* manager = *it;
					manager->TaskWasPaused(*this);
					it++;
				}
				OnPause(applicationPause);
			}
		}
	}
	
	void Task::Resume(bool applicationResume)
	{
		if(GetPaused())
		{
			SetPaused(false);
			std::list<TaskManager*>::iterator it = mManagers.begin();
			std::list<TaskManager*>::iterator itEnd = mManagers.end();
			while(it != itEnd)
			{
				TaskManager* manager = *it;
				manager->TaskWasResumed(*this);
				it++;
			}
			OnResume(applicationResume);
		}
	}

	bool Task::OnStart()
	{
		return true;
	}

	void Task::OnStop()
	{
	}

	void Task::OnPause(bool applicationPause)
	{
	}

	void Task::OnResume(bool applicationPause)
	{
	}

	void Task::Update(Seconds lastFrameTime)
	{
	}
	
	void Task::SetPaused(bool paused)
	{
		mPaused = paused;
	}

	void Task::AddTaskManager(TaskManager& manager)
	{
		if(std::find(mManagers.begin(), mManagers.end(), &manager) == mManagers.end())
		{
			mManagers.push_back(&manager);
			OnTaskManagerAdded(manager);
		}
	}

	void Task::RemoveTaskManager(TaskManager& manager)
	{
		//We need to search first
		std::list< TaskManager* >::iterator it = std::find(mManagers.begin(), mManagers.end(), &manager);
		if(it != mManagers.end())
		{
			mManagers.erase(it);
			OnTaskManagerRemoved(manager);
		}
	}
	
	void Task::OnTaskManagerAdded(TaskManager& manager)
	{
		//We don't need to do anything. This method is for overrides.
	}

	void Task::OnTaskManagerRemoved(TaskManager& manager)
	{
		//We don't need to do anything. This method is for overrides.
	}
}
