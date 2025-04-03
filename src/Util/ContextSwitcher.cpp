#include <echo/Util/ContextSwitcher.h>
#include <echo/Kernel/Task.h>
#include <echo/Kernel/TaskManager.h>
#include <boost/foreach.hpp>

namespace Echo
{
	//ContextSwitcher::Context
	
	ContextSwitcher::Context::~Context(){}

	void ContextSwitcher::Context::SetContextOnFailure(std::string contextOnFailure)
	{
		mContextOnFailure = contextOnFailure;
	}

	bool ContextSwitcher::Context::HasContextOnFailure() const
	{
		return !mContextOnFailure.empty();
	}

	std::string ContextSwitcher::Context::GetContextOnFailure() const
	{
		return mContextOnFailure;
	}

	void ContextSwitcher::Context::AddTaskToResume(const std::string& taskName)
	{
		mTasksToResume.push_back(taskName);
	}

	const ContextSwitcher::TaskNameList& ContextSwitcher::Context::GetTasksToResume() const
	{
		return mTasksToResume;
	}

	void ContextSwitcher::Context::RemoveTaskToResume(const std::string& taskName)
	{
		mTasksToResume.remove(taskName);
	}

	void ContextSwitcher::Context::AddTaskToPause(const std::string& taskName)
	{
		mTasksToPause.push_back(taskName);
	}

	const ContextSwitcher::TaskNameList& ContextSwitcher::Context::GetTasksToPause() const
	{
		return mTasksToPause;
	}

	void ContextSwitcher::Context::RemoveTaskToPause(const std::string& taskName)
	{
		mTasksToPause.remove(taskName);
	}

	void ContextSwitcher::Context::SetFailureAction(FailureAction failureAction)
	{
		mFailureAction = failureAction;
	}

	ContextSwitcher::FailureAction ContextSwitcher::Context::GetFailureAction() const
	{
		return mFailureAction;
	}

	std::string ContextSwitcher::Context::GetName() const
	{
		return mName;
	}
	
	ContextSwitcher::Context::Context(const std::string& name, ContextSwitcher::FailureAction failureAction, std::string contextOnFailure) :
		mName(name),
		mFailureAction(failureAction),
		mContextOnFailure(contextOnFailure)
	{}
	
	//ContextSwitcher
	
	ContextSwitcher::ContextSwitcher(shared_ptr<TaskManager> taskManager) : mTaskManager(taskManager)
	{
		
	}

	shared_ptr<ContextSwitcher::Context> ContextSwitcher::CreateContext(const std::string& name, ContextSwitcher::FailureAction failureAction, std::string contextOnFailure)
	{
		if(name.empty() || HasContext(name))
		{
			return nullptr;
		}
		shared_ptr<Context> context(new Context(name,failureAction,contextOnFailure));
		mContexts[name] = context;
		return context;
	}

	shared_ptr<ContextSwitcher::Context> ContextSwitcher::GetContext(const std::string& name, bool createIfNotFound)
	{
		NameContextMap::iterator it = mContexts.find(name);
		if(it==mContexts.end())
		{
			if(createIfNotFound)
			{
				return CreateContext(name);
			}
			return nullptr;
		}
		return it->second;
	}
	
	void ContextSwitcher::AddTaskToResume(const std::string& context, const std::string& taskName)
	{
		//Second param forces a context to be returned.
		GetContext(context,true)->AddTaskToResume(taskName);
	}
	
	void ContextSwitcher::RemoveTaskToResume(const std::string& context, const std::string& taskName)
	{
		//Second param forces a context to be returned.
		GetContext(context,true)->RemoveTaskToResume(taskName);
	}
	
	void ContextSwitcher::AddTaskToPause(const std::string& context, const std::string& taskName)
	{
		//Second param forces a context to be returned.
		GetContext(context,true)->AddTaskToPause(taskName);
	}
	
	void ContextSwitcher::RemoveTaskToPause(const std::string& context, const std::string& taskName)
	{
		//Second param forces a context to be returned.
		GetContext(context,true)->RemoveTaskToPause(taskName);
		
	}

	bool ContextSwitcher::HasContext(const std::string& name)
	{
		return (mContexts.find(name)!=mContexts.end());
	}

	bool ContextSwitcher::RemoveContext(const std::string& name)
	{
		NameContextMap::iterator it = mContexts.find(name);
		if(it!=mContexts.end())
		{
			mContexts.erase(it);
			return true;
		}
		return false;
	}

	bool ContextSwitcher::SwitchContext(const std::string& name)
	{
		if(!mTaskManager)
		{
			ECHO_LOG_ERROR("No task manager to perform a context switch.");
			return false;
		}
		return SwitchContextWithManager(name, *mTaskManager);
	}

	bool ContextSwitcher::SwitchContextWithManager(const std::string& name, TaskManager& taskManager)
	{
		shared_ptr<Context> context = GetContext(name);
		if(!context)
		{
			ECHO_LOG_ERROR("Context not found: ");
			return false;
		}

		// So we don't perform a double search for each task.
		TaskList resumeCache;
		TaskList pauseCache;
		const TaskNameList& toResume = context->GetTasksToResume();
		const TaskNameList& toPause = context->GetTasksToPause();
		ContinueOrNewSwitchPair cons = CollectTasks(*context, toResume, taskManager, resumeCache);
		if(cons.first)
		{
			cons = CollectTasks(*context, toPause, taskManager, pauseCache);
			if(cons.first)
			{
				//Pause and resume tasks
				BOOST_FOREACH(Task* task, pauseCache)
				{
					task->Pause();
				}
				BOOST_FOREACH(Task* task, resumeCache)
				{
					task->Resume();
				}

				return true;
			}

			if(!cons.second.empty())
			{
				//Failed to find all pause tasks but we have a new context.
				return SwitchContextWithManager(cons.second,taskManager);
			}
		}
		if(!cons.second.empty() && SwitchContextWithManager(cons.second,taskManager))
		{
			ECHO_LOG_ERROR("Failed to switch context: " << name);
		}
		return false;
	}

	ContextSwitcher::ContinueOrNewSwitchPair ContextSwitcher::CollectTasks(const Context& context, const TaskNameList& nameList, TaskManager& taskManager, TaskList& cache)
	{
		ContinueOrNewSwitchPair continueSwitch(true,"");
		BOOST_FOREACH(const std::string& taskName, nameList)
		{
			Task* task = taskManager.FindTask(taskName,true);
			if(task)
			{
				cache.push_back(task);
			} else
			{
				std::string message = "Task not found: " + taskName;
				switch(context.GetFailureAction())
				{
					case FailureActions::FALL_BACK_CONTEXT:
						continueSwitch.first = false;
						if(context.HasContextOnFailure())
						{
							ECHO_LOG_INFO(message << ". Attempting to switch to " << context.GetContextOnFailure());
							continueSwitch.second = context.GetContextOnFailure();

							//We can return immediately because we expect to try and switch.
							return continueSwitch;
						}
						ECHO_LOG_ERROR(" " << message << " and the context was set to switch on failure but no context name was given.");
					break;
					case FailureActions::IGNORE_BUT_WARN:
						ECHO_LOG_WARNING(message << " continuing...");
					break;
					case FailureActions::STOP:
						ECHO_LOG_ERROR(message);
						continueSwitch.first = false;
					break;
					case FailureActions::IGNORE:break;
				}
			}
		}
		return continueSwitch;
	}
}
