#include <echo/Kernel/TaskManager.h>
#include <echo/Kernel/Task.h>
#include <boost/foreach.hpp>
#include <iostream>
#include <algorithm>

namespace Echo
{
	TaskManager::TaskManager(std::string name) :
		mManagerName(name),
		mProfilerAveragerCurrentTime(0),
		mProfilerAveragerIntervalTime(1),
		mProfilingEnabled(false),
		mExecuting(false),
		mPaused(false),
		mUpdating(false)
	{
	}

	TaskManager::~TaskManager()
	{
		RemoveAllTasks();
		// Process the remove. This normally happens after an update but we need it
		// to happen right away so any shared pointers are cleaned up.
		UpdateLists();
	}

	bool TaskManager::StartTasks()
	{
		mExecuting = true;
		
		BOOST_FOREACH(TaskInfo& taskInfo,mTaskList)
		{
			if(taskInfo.GetHasBeenRemoved())
			{
				continue;
			}
			Task* task = taskInfo.GetTask();
			if(task && !task->Start())
			{
				ECHO_LOG_ERROR("Error starting " << task->GetTaskName());
				task->SetPaused(true);
				StopTasks();
				return false;
			}
		}
		return true;
	}

	void TaskManager::StopTasks()
	{
		mExecuting = false;
		BOOST_FOREACH(TaskInfo& taskInfo,mTaskList)
		{
			if(taskInfo.GetHasBeenRemoved())
			{
				continue;
			}
			Task* task = taskInfo.GetTask();
			if(task && task->GetStarted())
			{
				task->Stop();
			}
		}
	}

	bool TaskManager::HasTask(Task* task) const
	{
		TaskListConstIterator it = std::find(mTaskList.begin(), mTaskList.end(), task);
		if(it != mTaskList.end() && !it->GetHasBeenRemoved() && it->GetTask())
		{
			return true;
		}
		return false;
	}
	
	bool TaskManager::PauseTask(const std::string& taskName, bool deepSearch)
	{
		Task* task=FindTask(taskName, deepSearch);
		if(task)
		{
			task->Pause();
			return true;
		}
		return false;
	}

	bool TaskManager::ResumeTask(const std::string& taskName, bool deepSearch)
	{
		Task* task=FindTask(taskName, deepSearch);
		if(task)
		{
			task->Resume();
			return true;
		}
		return false;
	}
		
	Task* TaskManager::FindTask(const std::string& name, bool deepSearch)
	{
		BOOST_FOREACH(TaskInfo& taskInfo,mTaskList)
		{
			if(taskInfo.GetHasBeenRemoved())
			{
				continue;
			}
			Task* task = taskInfo.GetTask();
			if(task)
			{
				if(task->GetTaskName() == name)
				{
					return task;
				}
			}
		}
		if(deepSearch)
		{
			BOOST_FOREACH(TaskInfo& taskInfo,mTaskList)
			{
				if(taskInfo.GetHasBeenRemoved())
				{
					continue;
				}
				TaskManager* taskAsManager = dynamic_cast<TaskManager*>(taskInfo.GetTask());
				if(taskAsManager)
				{
					Task* task = taskAsManager->FindTask(name, true);
					if(task)
					{
						return task;
					}
				}
			}
		}
		return 0;
	}
	
	shared_ptr<Task> TaskManager::FindSharedTask(const std::string& name, bool deepSearch)
	{
		BOOST_FOREACH(TaskInfo& taskInfo,mTaskList)
		{
			if(taskInfo.GetHasBeenRemoved())
			{
				continue;
			}
			Task* task = taskInfo.GetTask();
			if(task)
			{
				if(task->GetTaskName() == name)
				{
					//This will be valid if it was added correctly, otherwise null.
					return taskInfo.GetSharedTask();
				}
			}
		}
		if(deepSearch)
		{
			BOOST_FOREACH(TaskInfo& taskInfo,mTaskList)
			{
				if(taskInfo.GetHasBeenRemoved())
				{
					continue;
				}
				TaskManager* taskAsManager = dynamic_cast<TaskManager*>(taskInfo.GetTask());
				if(taskAsManager)
				{
					shared_ptr<Task> task = taskAsManager->FindSharedTask(name, true);
					if(task)
					{
						return task;
					}
				}
			}
		}
		return shared_ptr<Task>();
	}
	
	bool TaskManager::AddTask(Task* task, shared_ptr<Task> taskPtr)
	{
		if(!task)
		{
			return false;
		}

		TaskListIterator it = std::find(mTaskList.begin(), mTaskList.end(), task);
		if(it != mTaskList.end())
		{
			Task* existingTask = it->GetTask();
			//You can add a task back to a task manager if you've just removed it.
			//But you cannot add it twice.
			if(existingTask && !it->GetHasBeenRemoved())
			{
				ECHO_LOG_ERROR("Failed to Add Task. Already been added.");
				return false;
			}
			//Reset the removed flag. This may happen if the task was removed before it has been cleaned out of the task list.
			it->SetHasBeenRemoved(false);
		}

		if(mExecuting)
		{
			if(!task->Start())
			{
				ECHO_LOG_ERROR("Task Failed to start! Check [Your]Task::Start()");
				return false;
			}
		}

		//Revive so the task is ready to go.
		task->SetPaused(false);

		// If the task isn't known then we need to insert a new entry into the task lists, otherwise we already have
		// an entry in mTaskList, but we just need to add one ot the active task list with an updated priority.
		if(it == mTaskList.end())
		{
			TaskInfo taskInfo(task,taskPtr);
			mTaskList.push_back(taskInfo);
			if(mUpdating)
			{
				taskInfo.SetWaitingForResume(true);
			}else
			{
				mActiveTaskList.push_back(taskInfo);
				mActiveTaskList.sort();
			}
		}else
		{
			//The task has effectively just resumed.
			TaskWasResumed(*task);
		}
		
		task->AddTaskManager(*this);
		return true;
	}
	
	void TaskManager::PauseAllActiveTasks(bool applicationPause)
	{
		if(mPaused)
		{
			return;
		}
		
		BOOST_FOREACH(TaskInfo& taskInfo,mTaskList)
		{
			if(taskInfo.GetHasBeenRemoved())
			{
				continue;
			}
			Task* task = taskInfo.GetTask();
			if(task)
			{
				//If the task is paused at all already we will keep
				//a reference so we know not to resume it. It is already
				//paused so we don't need to pause it again
				if(!task->GetPaused())
				{
					if(task->GetPausable())
					{
						task->Pause(applicationPause);
					}
					mPreviouslyActiveTaskList.push_back(taskInfo);
				}
			}
		}
		mPaused = true;
	}

	void TaskManager::ResumeAllPreviouslyActiveTasks(bool applicationResume)
	{
		if(!mPaused)
		{
			return;
		}
		mPaused = false;
		BOOST_FOREACH(TaskInfo& taskInfo,mPreviouslyActiveTaskList)
		{
			if(taskInfo.GetHasBeenRemoved())
			{
				continue;
			}
			Task* task = taskInfo.GetTask();
			if(task)
			{
				if(task->GetPaused())
				{
					task->Resume(applicationResume);
				}
			}
		}
		mPreviouslyActiveTaskList.clear();
	}
	
	void TaskManager::ResumeAllTasks(bool applicationResume)
	{
		BOOST_FOREACH(TaskInfo& taskInfo,mTaskList)
		{
			Task* task = taskInfo.GetTask();
			if(task)
			{
				task->Resume(applicationResume);
			}
		}		
	}

	void TaskManager::RemoveTask(Task* task)
	{
		TaskListIterator it = std::find(mTaskList.begin(), mTaskList.end(), task);
		if(it != mTaskList.end())
		{
			task->RemoveTaskManager(*this);
			if(task->GetNumberOfTaskManagers()==0)
			{
				task->Stop();
			}
			mPreviouslyActiveTaskList.remove(task);
			if(!mUpdating)
			{
				//Remove now.
				mActiveTaskList.remove(*it);
				mTaskList.erase(it);
			}else
			{
				//Remove later.
				it->SetHasBeenRemoved(true);
			}
		}
	}

	void TaskManager::RemoveTask(const std::string& taskName)
	{
		TaskListIterator it = mTaskList.begin();
		TaskListIterator itEnd = mTaskList.end();
		while(it != itEnd)
		{
			TaskInfo& taskInfo=*it;
			if(taskInfo.GetHasBeenRemoved())
			{
				++it;
				continue;
			}
			Task* task = taskInfo.GetTask();
			if(task && task->GetTaskName()==taskName)
			{
				task->RemoveTaskManager(*this);
				if(task->GetNumberOfTaskManagers()==0)
				{
					task->Stop();
				}
				mPreviouslyActiveTaskList.remove(task);
				if(!mUpdating)
				{
					//Remove now.
					mActiveTaskList.remove(taskInfo);
					mTaskList.erase(it);
				}else
				{
					//Remove later.
					taskInfo.SetHasBeenRemoved(true);
				}
				return;
			}
			it++;
		}
	}

	void TaskManager::RemoveAllTasks()
	{
		BOOST_FOREACH(TaskInfo& taskInfo,mTaskList)
		{
			if(taskInfo.GetHasBeenRemoved())
			{
				continue;
			}
			Task* task = taskInfo.GetTask();
			if(task)
			{
				task->RemoveTaskManager(*this);
				if(task->GetNumberOfTaskManagers()==0)
				{
					task->Stop();
				}
			}
			mPreviouslyActiveTaskList.remove(taskInfo);
			taskInfo.SetHasBeenRemoved(true);
		}
		if(!mUpdating)
		{
			UpdateLists();
		}
	}

	void TaskManager::TaskWasPaused(Task& task)
	{
		TaskListIterator it = std::find(mTaskList.begin(), mTaskList.end(), &task);
		if(it != mTaskList.end())
		{
			it->SetHasBeenPaused(true);
			it->SetWaitingForResume(false);
			it->SetWaitingForPauseProcessing(false);
		}
		if(!mUpdating)
		{
			TaskListIterator ait = std::find(mActiveTaskList.begin(), mActiveTaskList.end(), &task);
			if(ait!=mActiveTaskList.end())
			{
				//Remove now.
				mActiveTaskList.erase(ait);
			}
			ait = std::find(mPreviouslyActiveTaskList.begin(), mPreviouslyActiveTaskList.end(), &task);
			if(ait!=mPreviouslyActiveTaskList.end())
			{
				//Remove now.
				mPreviouslyActiveTaskList.erase(ait);
			}
		}else
		{
			it->SetWaitingForPauseProcessing(true);
		}
	}

	void TaskManager::TaskWasResumed(Task& task)
	{
		TaskListIterator it = std::find(mTaskList.begin(), mTaskList.end(), &task);
		if(it != mTaskList.end())
		{
			it->SetHasBeenPaused(false);
			it->SetWaitingForResume(true);
			it->SetWaitingForPauseProcessing(false);
		}
		if(!mUpdating)
		{
			it->SetWaitingForResume(false);
			
			TaskListIterator ait = std::find(mActiveTaskList.begin(), mActiveTaskList.end(), &task);
			if(ait==mActiveTaskList.end())
			{
				//Add to active list now.
				it->UpdatePriority();
				mActiveTaskList.push_back(*it);
				mActiveTaskList.sort();
			}
		}
	}

	void TaskManager::UpdateTasks(Seconds lastFrameTime)
	{
		mUpdating = true;
		if(mProfilingEnabled)
		{
			mProfilerAveragerCurrentTime+=lastFrameTime;
			if(!mProfilerAverager.HasStarted())
			{
				mProfilerAverager.Start(GetTaskManagerName());
			}
			mProfiler.Start(GetTaskManagerName());
		}
		BOOST_FOREACH(TaskInfo& taskInfo,mActiveTaskList)
		{
			if(taskInfo.GetHasBeenRemoved() || taskInfo.GetHasBeenPaused())
			{
				continue;
			}
			Task* task = taskInfo.GetTask();
			if(task)
			{
				task->Update(lastFrameTime);
				if(mProfilingEnabled)
				{
					mProfiler.SaveCheckpoint(task->GetTaskName());
				}
			}
		}
		mUpdating = false;
		if(mProfilingEnabled)
		{
			mProfiler.Stop();
			mProfilerAverager.Report(mProfiler);
			if(mProfilerAveragerCurrentTime>mProfilerAveragerIntervalTime)
			{
				mProfilerAverager.Stop();
				mProfilerAveragerCurrentTime = Seconds(0);
			}
		}
		if(!mPostUpdateActions.empty())
		{
			std::vector<Action> actions = std::move(mPostUpdateActions);
			for(Action& action : actions)
			{
				action();
			}
		}
		UpdateLists();
	}

	void TaskManager::UpdateLists()
	{
		TaskListIterator it = mTaskList.begin();
		TaskListIterator itEnd = mTaskList.end();
		while(it != itEnd)
		{
			if(it->GetHasBeenRemoved())
			{
				//This task has been removed we aren't going to try and stop.
				mActiveTaskList.remove(*it);
				mPreviouslyActiveTaskList.remove(*it);
				it = mTaskList.erase(it);
			}else
			if(it->GetWaitingForPauseProcessing())
			{
				it->SetWaitingForPauseProcessing(false);
				TaskListIterator ait = std::find(mActiveTaskList.begin(), mActiveTaskList.end(), it->GetTask());
				if(ait!=mActiveTaskList.end())
				{
					mActiveTaskList.remove(*it);
				}
				// If we're not paused then we need to remove it from our active list since it wouldn't have happened
				// during the update, if we are paused then it arrived in the active list as a result of the TaskManager
				// being paused so we will leave the list alone.
				if(!GetPaused())
				{
					ait = std::find(mPreviouslyActiveTaskList.begin(), mPreviouslyActiveTaskList.end(), it->GetTask());
					if(ait!=mPreviouslyActiveTaskList.end())
					{
						mPreviouslyActiveTaskList.erase(ait);
					}
				}
			}else
			if(it->GetWaitingForResume())
			{
				it->SetWaitingForResume(false);
				TaskListIterator ait = std::find(mActiveTaskList.begin(), mActiveTaskList.end(), it->GetTask());
				if(ait==mActiveTaskList.end())
				{
					//Add to active list now.
					it->UpdatePriority();
					mActiveTaskList.push_back(*it);
					mPreviouslyActiveTaskList.remove(*it);
					mActiveTaskList.sort();
				}
			}
			it++;
		}
	}
	
	size_t TaskManager::GetNumberOfActiveTasks(bool includeChildTaskManagerTasks, bool outputTaskList,size_t indentDepth) const
	{
		std::string indent(indentDepth,' ');

		size_t numberOfTasks = 0;
		BOOST_FOREACH(const TaskInfo& taskInfo,mActiveTaskList)
		{
			if(taskInfo.GetHasBeenRemoved())
			{
				continue;
			}
			if(outputTaskList)
			{
				ECHO_LOG_INFO(indent << "Task: " << taskInfo.GetTask()->GetTaskName() << "(" << std::hex << taskInfo.GetTask() << std::dec << ")");
			}
			if(!taskInfo.GetTask()->GetPaused())
			{
				++numberOfTasks;
			}
			if(includeChildTaskManagerTasks)
			{
				TaskManager* taskManager = dynamic_cast<TaskManager*>(taskInfo.GetTask());
				if(taskManager)
				{
					numberOfTasks+=taskManager->GetNumberOfActiveTasks(true,outputTaskList,indentDepth+1);
				}
			}
		}
		return numberOfTasks;
	}

	size_t TaskManager::GetNumberOfTasks(bool includeChildTaskManagerTasks, bool outputTaskList,size_t indentDepth) const
	{
		std::string indent(indentDepth,' ');
		size_t numberOfTasks = 0;
		BOOST_FOREACH(const TaskInfo& taskInfo,mTaskList)
		{
			if(taskInfo.GetHasBeenRemoved())
			{
				continue;
			}
			++numberOfTasks;
			if(outputTaskList)
			{
				if(taskInfo.GetTask()->GetPaused())
				{
					ECHO_LOG_INFO(indent << "Task: " << taskInfo.GetTask()->GetTaskName() << "(" << std::hex << (taskInfo.GetTask()) << std::dec << ") - Inactive");
				}else
				{
					ECHO_LOG_INFO(indent << "Task: " << taskInfo.GetTask()->GetTaskName() << "(" << std::hex << (taskInfo.GetTask()) << std::dec << ") - Active");
				}
			}
			if(includeChildTaskManagerTasks)
			{
				TaskManager* taskManager = dynamic_cast<TaskManager*>(taskInfo.GetTask());
				if(taskManager)
				{
					numberOfTasks+=taskManager->GetNumberOfTasks(true,outputTaskList,indentDepth+1);
				}
			}
		}
		return numberOfTasks;
	}
	
	void TaskManager::QueuePostUpdateAction(Action action)
	{
		assert(action && "Action must not be null");
		mPostUpdateActions.push_back(action);
	}
	
	const ProfilerAverager& TaskManager::GetProfilerAverager() const
	{
		return mProfilerAverager;
	}

	Profiler TaskManager::GetProfiler() const
	{
		return mProfiler;
	}
	
	bool TaskManager::RequestNextProfilerResults(function<void(const Profiler&)> callback)
	{
		if(!mProfilingEnabled || !callback)
		{
			return false;
		}
		QueuePostUpdateAction([this,callback](){
				callback(GetProfiler());
			});
		return true;
	}
}
