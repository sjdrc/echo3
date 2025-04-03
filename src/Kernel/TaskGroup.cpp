#include <echo/Kernel/TaskGroup.h>

#include <iostream>
#include <algorithm>

namespace Echo
{

	TaskGroup::TaskGroup()
	{
	}

	TaskGroup::TaskGroup(const std::string& taskGroupName, u32 priority) : Task(taskGroupName,priority)
	{
		SetTaskManagerName(taskGroupName);
	}

	TaskGroup::~TaskGroup()
	{
	}

	bool TaskGroup::OnStart()
	{
		if(!TaskManager::StartTasks())
		{
			TaskManager::StopTasks();
			return false;
		}
		return true;
	}

	void TaskGroup::OnStop()
	{
		StopTasks();
	}

	void TaskGroup::OnPause(bool applicationPause)
	{
		PauseAllActiveTasks(applicationPause);
	}

	void TaskGroup::OnResume(bool applicationResume)
	{
		ResumeAllPreviouslyActiveTasks(applicationResume);
	}

	void TaskGroup::Update(Seconds lastFrameTime)
	{
		TaskManager::UpdateTasks(lastFrameTime);
	}
	
	void TaskGroup::SetTaskName(const std::string& name)
	{
		Task::SetTaskName(name);
		TaskManager::SetTaskManagerName(name);
	}
	
	void TaskGroup::SetTaskManagerName(const std::string& name)
	{
		Task::SetTaskName(name);
		TaskManager::SetTaskManagerName(name);
	}
}
