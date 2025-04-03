#ifndef _ECHOTASK_H_
#define _ECHOTASK_H_

#include <echo/Types.h>
#include <echo/Chrono/Chrono.h>
#include <echo/Util/InheritableEnableSharedFromThis.h>
#include <list>

namespace Echo
{
	class TaskManager;

	/**
	 * Task
	 * Tasks are the basic processing unit in Echo applications. You can create a heirarchy of tasks
	 * with TaskManagers and TaskGroups. Tasks are updated once per frame if the tasks' task manager
	 * is updated. They can be controlled using the Start, Stop, Pause, Resume methods which will
	 * call the corresponding On* method (e.g. OnPause()) as appropriate.
	 * 
	 * The Task class is intended to be a base class that you inherit to build your application
	 * components out of.
	 * 
	 * There is no restriction on what you might make a task. TaskManagers are designed so that they
	 * only update what is active so there isn't any processing overhead in updating only a few of
	 * many tasks in a task manager (e.g. 10 of 1000 tasks).
	 */
	class Task : public InheritableEnableSharedFromThis<Task>
	{
	public:
		/**
		 * Task constructor
		 * @param priority tasks are updated in priority order where lower values are updated first.
		 */
		Task(u32 priority = 5000);

		/**
		 * Task constructor
		 * @param taskName name of the task, named tasks can be easier to debug.
		 * @param priority tasks are updated in priority order where lower values are updated first.
		 */
		Task(const std::string& taskName, u32 priority = 5000);
		Task(const Task& other);
		virtual ~Task();
		
		/**
		 * Set a new task name.
		 * @param name
		 */
		virtual void SetTaskName(const std::string& name)
		{
			mTaskName = name;
		}
		
		/**
		 * Get the Task's name.
		 * @return the task name
		 */
		virtual const std::string& GetTaskName() const
		{
			return mTaskName;
		}
		
		/**
		 * Get the task priority.
		 * @return the task priority.
		 */
		const u32& GetPriority() const
		{
			return mPriority;
		}
		
		/**
		 * Get the number of task managers that this task manager is a member of.
		 */
		size_t GetNumberOfTaskManagers() const
		{
			return mManagers.size();
		}

		/**
		 * Find out if this task is a member of a TaskManager.
		 * @param manager
		 * @return true or false to indicate if the task is a member of the manager.
		 */
		bool IsInManager(const TaskManager& manager) const;

		/**
		 * Get a list of managers the task is a member of.
		 */
		std::list< TaskManager* > GetManagers() const
		{
			return mManagers;
		}
		
		/**
		 * If the Task is a member of a single task manager this method is a helper
		 * that will return that task manager. If there are multiple TaskManagers
		 * managing the task then this method will return null.
		 * @return A pointer to a task manager if there is a single TaskManager
		 * managing this task, otherwise null.
		 */
		TaskManager* GetSingleTaskManager() const;

		/**
		 * Get whether the task has started.
		 * @return 
		 */
		inline bool GetStarted() const
		{
			return mHasStarted;
		}

		/**
		 * Get whether the task is paused.
		 * @return whether the task is paused.
		 */
		inline bool GetPaused() const
		{
			return mPaused;
		}
		
		/**
		 * Set whether this task can be paused.
		 * @param enable true to allow the task to be paused, otherwise false.
		 */
		inline void SetPausable(bool enable)
		{
			mPausable = enable;
		}
		
		/**
		 * Get whether this task can be paused.
		 * @return whether this task can be paused.
		 */
		inline bool GetPausable() const
		{
			return mPausable;
		}

		/**
		 * Attempt to start this task.
		 * Calling this method will cause OnStart() to be called if the task has not already started.
		 */
		bool Start();

		/**
		 * Attempt to stop this task.
		 * Calling this method will cause OnStop() to be called if the task has not already stopped.
		 */
		void Stop();
		
		/**
		 * Pause a Task's processing.
		 * OnPause will be called if the task was not already paused.
		 * @param applicationPause indicates whether the pause was triggered by the application pause.
		 * @param forcePause if true forces the method to pause, this flag overrides the mCanPauseUpdate flag,
		 *					 normally you shouldn't use this flag, it is provided to allow Kernel objects to
		 *					 force pause if the execution model doesn't support execution while paused.
		 */
		void Pause(bool applicationPause = false, bool forcePause = false);

		/**
		 * Resume a Task's processing.
		 * OnResume will be called if the task was paused.
		 * @note Processing is dependant on TaskManagers which also need to be active.
		 * @param applicationResume indicates whether the resume was triggered by the application resuming.
		 */
		void Resume(bool applicationResume = false);

		/**
		 * OnStart is called when a task is started.
		 * OnStart is called when a task is added to a TaskManager. If the manager is not running this will
		 * be called on when the manager starts or is resumed (in the case of it being paused).
		 * If this method returns false the task is not updated.
		 * @return false to prevent the task from starting.
		 */
		virtual bool OnStart();

		/**
		 * OnStop is called when a task stops.
		 * A task is stopped when:
		 * - The Task is Removed from its last task manager
		 * - A TaskManager Group is Stopped or
		 * - The Kernel is stopped
		 */
		virtual void OnStop();

		/**
		 * Onpause is called when a task pauses.
		 * 
		 * This method will only be called during a transition from paused to resume. If calling Resume()
		 * multiple times does not cause this method to be called multiple times.
		 * 
		 * Tasks do not have to pause when the application pauses which means that this method is not normally
		 * called upon application pause, however, keep in mind that if the ExecutionModel is EXTERNAL_CONTROLLER
		 * then this method may still be called.
		 * 
		 * This method is not called if the task is already paused. That means tasks that have been paused will
		 * not receive another notification for application pause.
		 * 
		 * @param applicationPause Flag to indicate that the application was pause.
		 */
		virtual void OnPause(bool applicationPause);
		
		/**
		 * OnResume is called when a task resumes.
		 * 
		 * This method will only be called during a transition from paused to resume. If calling Resume()
		 * multiple times does not cause this method to be called multiple times.
		 * 
		 * Tasks do not have to pause when the application pauses which means that this method is not called
		 * upon application resume, however keep in mind that if the ExecutionModel is EXTERNAL_CONTROLLER
		 * then this method may still be called.
		 * 
		 * @param applicationResume Flag to indicate that the application was resumed.
		 */
		virtual void OnResume(bool applicationResume);

		/**
		 * Update method called when the task is to be updated.
		 * @param lastFrameTime Time since the last frame. This value is time it took the application's last frame
		 *						to process. It is NOT the time since this method was last updated.
		 */
		virtual void Update(Seconds lastFrameTime);
	private:
		friend class TaskManager;
		/// Flag to indicate if the task has started.
		bool mHasStarted;
		/// Pausable flag.
		bool mPausable;
		/// Pause flag.
		bool mPaused;
		/// Lower values are updated first
		u32 mPriority;
		/// Task name
		std::string mTaskName;
		///The managers that manage this task.
		std::list< TaskManager* > mManagers;
		
		/**
		 * Sets whether this task is paused.
		 * This method is used by TaskManagers and the Task to set the flag. Task managers may use this method to
		 * avoid calling Pause() in situations where a call to OnPause would be unexpected, e.g. if OnStart()
		 * returned false.
		 * @param paused
		 */
		void SetPaused(bool paused);
		
		/**
		 * Remove a task manager from the task manager list.
		 * Used by TaskManagers.
		 * @param manager
		 */
		void RemoveTaskManager(TaskManager& manager);

		/**
		 * Add a task manager from the task manager list.
		 * Used by TaskManagers.
		 * @param manager
		 */
		void AddTaskManager(TaskManager& manager);
		
		/**
		 * Notification method called when a TaskManager is added.
		 * This method is called when the process of adding the task manager has completed.
		 * Override this method if your class needs to be notified when a manager is added.
		 * @param manager The new task manager.
		 */
		virtual void OnTaskManagerAdded(TaskManager& manager);

		/**
		 * Notification method called when a TaskManager is removed.
		 * This method is called after the process of removing the task manager has completed.
		 * Override this method if your class needs to be notified when a manager is removed.
		 * @param manager The task manager being removed.
		 */
		virtual void OnTaskManagerRemoved(TaskManager& manager);
	};

}
#endif
