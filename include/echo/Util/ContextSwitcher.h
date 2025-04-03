#ifndef ECHOCONTEXTSWITCHER_H
#define ECHOCONTEXTSWITCHER_H

#include <echo/Types.h>
#include <map>
#include <list>
#include <string>

namespace Echo
{
	class Task;
	class TaskManager;
	
	/**
	 * ContextSwitcher is a control class that can resume and pause tasks in groups.
	 * When changing application state, even if you encapsulate all major states within
	 * separate TaskManagers, you will probably end up pausing at least one TaskGroup
	 * (current state) then activating another (new state). This, of course, can be
	 * accomplished by two function calls, for example tm1->Pause() then a tm2->Resume(),
	 * but if you're performing this context switch context from another system like in
	 * a Menu callback, you'll either end up writing a helper function to wrap these
	 * calls or rely on the menu data files to include both event functions.
	 * 
	 * A better way to accomplish a context switch would be to use a ContextSwitcher which
	 * can be configured to pause and resume sets of tasks with a single request. This
	 * removes the detail of the context switch out code that wants to perform the context
	 * switch. The ContextSwitcher also operates at a high level by only performing actions
	 * based on names which means you can configure a ContextSwitcher regardless of whether
	 * the named tasks exist yet or not. This gives you the flexibility of configuring
	 * ContextSwitchers using a data driven approach.
	 */
	class ContextSwitcher
	{
	public:
		/**
		 * Before a context switch is executed the switcher search for all tasks.
		 * If one of the tasks is not found it is considered a failure. When a failure
		 * is detected the ContextSwitcher will perform one of the following.
		 * If the  component of the context switch will fail, 
		 */
		struct FailureActions
		{
			enum _
			{
				IGNORE,				/// Ignore and execute anyway.
				IGNORE_BUT_WARN,	/// Ignore and execute anyway, a warning will be output.
				STOP,				/// Stop the context switch, an error will be output.
				FALL_BACK_CONTEXT	/// Switch to a different context, a notice will be output,
									/// if the context isn't found an error is output and
									///	the context switch stops.
			};
		};
		typedef FailureActions::_ FailureAction;
		
		typedef std::list< std::string > TaskNameList;

		/**
		 * A context essentially contains a couple of list of tasks to pause and resume it is activated.
		 * You can modify the lists by getting Context objects from a ContextSwitcher. You can also 
		 * modify the failure action and fallback context after creation by using the Context object.
		 */
		class Context
		{
		public:
			~Context();
			void SetContextOnFailure(std::string contextOnFailure);
			bool HasContextOnFailure() const;
			std::string GetContextOnFailure() const;
			void AddTaskToResume(const std::string& taskName);
			const TaskNameList& GetTasksToResume() const;
			void RemoveTaskToResume(const std::string& taskName);
			void AddTaskToPause(const std::string& taskName);
			const TaskNameList& GetTasksToPause() const;
			void RemoveTaskToPause(const std::string& taskName);
			void SetFailureAction(FailureAction mFailureAction);
			FailureAction GetFailureAction() const;
			std::string GetName() const;
		private:	
			std::string mName;
			FailureAction mFailureAction;
			TaskNameList mTasksToPause;
			TaskNameList mTasksToResume;
			std::string mContextOnFailure;
			
			friend class ContextSwitcher;
			Context(const std::string& name, FailureAction failureAction=FailureActions::IGNORE_BUT_WARN, std::string contextOnFailure = "");
		};
		
		/**
		 * Create a context manager.
		 * @param taskManager The internal TaskManager, @see SetTaskManager().
		 */
		ContextSwitcher(shared_ptr<TaskManager> taskManager = nullptr);

		/**
		 * Create a context.
		 * @param name The name of the context, used when calling a SwitchContext* method.
		 * @param failureAction The failure action, @see FailureActions
		 * @param contextOnFailure Name of the context to switch to on failure when using FailureActions::FALL_BACK_CONTEXT.
		 * @return the new Context or a nullptr if the a Context with the specified name already exists.
		 */
		shared_ptr<Context> CreateContext(const std::string& name, FailureAction failureAction = FailureActions::IGNORE_BUT_WARN, std::string contextOnFailure = "");
		/**
		 * Get a context by name.
		 * @param name The name of the context.
		 * @return The context with the specifed name.
		 */
		shared_ptr<Context> GetContext(const std::string& name, bool createIfNotFound = false);

		/**
		 * Add a task to resume to a context.
		 * If the context does not exist, it is created.
		 * @param context The context name.
		 * @param taskName The name of the task.
		 */
		void AddTaskToResume(const std::string& context, const std::string& taskName);

		/**
		 * Remove a task to resume from a context.
		 * If the context does not exist, no action is taken.
		 * @param context The context name.
		 * @param taskName The name of the task.
		 */
		void RemoveTaskToResume(const std::string& context, const std::string& taskName);

		/**
		 * Add a task to pause to a context.
		 * If the context does not exist, no action is taken.
		 * @param context The context name.
		 * @param taskName The name of the task.
		 */
		void AddTaskToPause(const std::string& context, const std::string& taskName);

		/**
		 * Remove a task to pause from a context.
		 * If the context does not exist, no action is taken.
		 * @param context The context name.
		 * @param taskName The name of the task.
		 */
		void RemoveTaskToPause(const std::string& context, const std::string& taskName);

		/**
		 * Check if a context exists.
		 * @param name The name of the context to check for.
		 * @return true if the context exists, otherwise false.
		 */
		bool HasContext(const std::string& name);
		
		/**
		 * Remove a context by name.
		 * @param name The name of the context you want to remove.
		 * @return true if the context was removed, false if the context does not exist.
		 */
		bool RemoveContext(const std::string& name);

		/**
		 * Attempt a context switch with the specified TaskManager.
		 * @note Context Switches perform a deep search on the TaskManager to find tasks by name.
		 * @note The method is not an overload to save time if one of the Switch methods is used with
		 * bind, i.e. saves a function cast to select the overload.
		 * @param name The name of the context to switch to.
		 * @return true if a context was successful, the context may not be the one specified if there
		 * was a failure but a fallback context was used.
		 */
		bool SwitchContextWithManager(const std::string& name, TaskManager& taskManager);
		
		/**
		 * Attempt a context switch with the internal TaskManager.
		 * @note Context Switches perform a deep search on the TaskManager to find tasks by name.
		 * @param name The name of the context to switch to.
		 * @return true if a context was successful, the context may not be the one specified if there
		 * was a failure but a fallback context was used.
		 */
		bool SwitchContext(const std::string& name);
		
		/**
		 * Set the internal TaskManager to be used in SwitchContext() calls.
		 * @param taskManager the TaskManager to perform the context switch on.
		 */
		void SetTaskManager(shared_ptr<TaskManager> taskManager) {mTaskManager = taskManager;}
		
		/**
		 * Get the internal TaskManager.
		 * @return the internal TaskManager, will be a nullptr if one is not set.
		 */
		shared_ptr<TaskManager> GetTaskManager() const {return mTaskManager;}
	private:
		typedef std::list< Task* > TaskList;
		typedef std::pair< bool, std::string > ContinueOrNewSwitchPair;
		typedef std::map< std::string, shared_ptr<Context> > NameContextMap;
		NameContextMap mContexts;
		shared_ptr<TaskManager> mTaskManager;

		/**
		 * A context switch helper function that builds a cache of Tasks by searching
		 * for each task in the name list in the taskManager.
		 * @return ContinueOrNewSwitchPair with first set to true if to continue the
		 * context switch, otherwise false. If second is not empty it is the new
		 * context to attempt to switch to.
		 * 
		 */
		ContinueOrNewSwitchPair CollectTasks(const Context& context, const TaskNameList& nameList, TaskManager& taskManager, TaskList& cache);
	};
}

#endif
