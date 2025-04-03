#ifndef _ECHO_TASKMANAGER_H_
#define _ECHO_TASKMANAGER_H_
#include <list>
#include <queue>
#include <echo/Types.h>
#include <echo/Chrono/ProfilerAverager.h>
#include <echo/Kernel/Task.h>
#include <echo/cpp/functional>

namespace Echo
{
	class Task;

	/**
	 * TaskManagers manages a list of tasks.
	 * Tasks are added to task managers in order to be updated, receive pause and resume
	 * notifications and are started and stopped as needed.
	 * 
	 */
	class TaskManager : public InheritableEnableSharedFromThis<TaskManager>
	{
	public:
		typedef function< void(void) > Action;
		
		TaskManager(std::string name = "");
		virtual ~TaskManager();

		/**
		 * Check if the manager has a task.
		 */
		bool HasTask(Task* task) const;
		
		/**
		 * Check if the manager has a task.
		 */
		bool HasTask(Task& task) const
		{
			return HasTask(&task);
		}
		
		/**
		 * Check if the manager has at least one task.
		 */
		inline bool HasAtLeastOneTask() const
		{
			return !mTaskList.empty();
		}
		
		/**
		 * Set the TaskManager name.
		 */
		virtual void SetTaskManagerName(const std::string& name)
		{
			mManagerName = name;
		}

		/**
		 * Get the TaskManager name.
		 */
		const std::string& GetTaskManagerName() const
		{
			return mManagerName;
		}
		
		/**
		 * Get whether the TaskManager is executing.
		 */
		bool GetExecuting() const
		{
			return mExecuting;
		}
		
		/**
		 * Get whether the TaskManager is paused.
		 * The paused state of a manager changes when PauseAllActiveTasks(),
		 * ResumeAllPreviouslyActiveTasks() or ResumeAllTasks() is called.
		 */
		bool GetPaused() const
		{
			return mPaused;
		}
		
		/**
		 * Calling this method will cause the Task's OnStart() method to be called.
		 * If any task fails to start (OnStart() returns false) then all tasks are stopped and
		 * this method will return false. This method returns true if all tasks successfully start.
		 *
		 * Normally this method is called by another TaskManager or TaskGroup when a Kernel starts,
		 * however if you are managing the Updates of a TaskManager yourself call this method to
		 * before you begin calling UpdateTasks(). This essentially changes the state of
		 * TaskManager to "executing" to ensure newly added tasks are started and updated. Call
		 * this method on your manually updated TaskManager before updates even if you haven't
		 * added any tasks.
		 *
		 * The main purpose of StartTasks() is to provide tasks an opportunity to do something at
		 * the beginning of their execution, which is different to Puase() and Resume(), which might
		 * happen many times during execution. This provides tasks an OnStart() notification.
		 *
		 * All tasks in the list, whether active or not, will have their Start(), and subsequently
		 * OnStart() methods called.
		 */
		bool StartTasks();

		/**
		 * Stop all tasks.
		 * This modifies the internal state of the manager to not executing.
		 *
		 * The method does not modify the task lists. i.e. the active task list will remain active.
		 * It is expected that after StopTasks() you will no longer call UpdateTasks(), however if
		 * you do the active tasks will still be updated. This is not a bug and by design to allow
		 * task manages top be stopped, moved then started again without changing the task lists.
		 *
		 * Similar to StartTasks() the main purpose of StopTasks() is to provide tasks an opportunity
		 * to do something at end of their execution, which is different to Puase() and Resume(),
		 * which might happen many times during execution. This provides tasks an OnStart()
		 * notification.
		 * 
		 * All tasks in the list, whether active or not, will have their Stop(), and subsequently
		 * OnStop() called.
		 */
		void StopTasks();

		/**
		 * Add a task to the manager.
		 *
		 * After a Tasks is added it will be resumed if it was previously paused. This is due to the
		 * assumption that a Task that is added to the manager is ready to be updated. If you want
		 * a task to be paused, add it then pause it.
		 *
		 * If the TaskManager is executing the Task's Start() method will be called.
		 *
		 * Tasks will notify any managers they are members of when they are deleted so this is
		 * effectively as safe as using a shared pointer.
		 * @returns false if the task has already been added to this manager or if the Task's Start()
		 * method returns false.
		 */
		bool AddTask(Task* task)
		{
			return AddTask(task, shared_ptr<Task>());
		}

		/**
		 * Overload of AddTask() that takes a reference.
		 * Tasks will notify any managers they are members of when they are deleted so this is
		 * effectively as safe as using a shared pointer.
		 * @see AddTask(Task* task)
		 */
		bool AddTask(Task& task)
		{
			return AddTask(&task,shared_ptr<Task>());
		}

		/**
		 * Overload of AddTask() that takes a shared pointer.
		 * @see AddTask(Task* task)
		 */
		bool AddTask(shared_ptr<Task> task)
		{
			return AddTask(task.get(),task);
		}

		/**
		 * Overload of AddTask() that allows you to add a callable as a task.
		 * @note This is useful for 
		 * @see AddTask(Task* task)
		 */
		template< typename Callable >
		bool AddTask(std::string name, Callable callable)
		{
			class CallableTask : public Task
			{
			public:
				CallableTask(Callable callable) : mCallable(callable){}
				void Update(Seconds lastFrameTime)
				{
					mCallable(lastFrameTime);
				};
			private:
				Callable mCallable;
			};
			
			return AddTask(make_shared<CallableTask>(callable));
		}

		/**
		 * Remove a task from the manager.
		 */
		void RemoveTask(Task* tTask);

		/**
		 * Remove a task from the manager.
		 */
		void RemoveTask(Task& task)
		{
			RemoveTask(&task);
		}

		/**
		 * Remove a task from the manager.
		 * The task does not need to be added using a shared pointer for this to work since
		 * TaskManagers use raw pointers for comparison.
		 */
		void RemoveTask(shared_ptr<Task> task)
		{
			RemoveTask(task.get());
		}

		/**
		 * Remove a task by name from the manager.
		 * The name of a task is not cached when added, so the comparison is against any current task names.
		 */
		void RemoveTask(const std::string& taskName);

		/**
		 * Remove all tasks from the manager.
		 * This clears all tasks from the task list.
		 */
		void RemoveAllTasks();
		
		/**
		 * Pause a task by name.
		 * This method performs a search and if a task is found it is paused.
		 * @see FindTask()
		 * @param taskName the name of the task to look for.
		 * @param deepSearch whether or not to perform a deepSearch.
		 * @return true if the task was found.
		 */
		bool PauseTask(const std::string& taskName, bool deepSearch);

		/**
		 * Resume a task by name.
		 * This method performs a search and if a task is found it is resumed.
		 * @see FindTask()
		 * @param taskName the name of the task to look for.
		 * @param deepSearch whether or not to perform a deepSearch.
		 * @return true if the task was found.
		 */
		bool ResumeTask(const std::string& taskName, bool deepSearch);
		
		/**
		 * Search for a task.
		 * Searching is performed in this object first then if deepSearch is true the search
		 * will continue in each child TaskManager.
		 * @param name the name of the task to search for.
		 * @param deepSearch if a matching task isn't found in the TaskManager each task will be checked to see if
		 * it is a TaskManager and continue the search from there.
		 * @return A pointer to the task if found, otherwise a null pointer.
		 */
		Task* FindTask(const std::string& name, bool deepSearch);

		/**
		 * Search for a task by name and attempt to get the shared pointer for it.
		 * @note The task needs to have been added using AddTask(shared_ptr<Task>).
		 * @see FindTask()
		 * @return A pointer to the task if found, otherwise a null pointer.
		 */
		shared_ptr<Task> FindSharedTask(const std::string& name, bool deepSearch);

		/**
		 * Search for a task by name and attempt to get the cast pointer for it.
		 * @see FindTask()
		 * @return A pointer to the task if found, otherwise null pointer.
		 */
		template< class T >
		T* FindTask(const std::string& name, bool deepSearch)
		{
			return dynamic_cast<T*>(FindTask(name, deepSearch));
		}
		
		/**
		 * Search for a task by name and attempt to get the cast shared pointer for it.
		 * @note The task needs to have been added using AddTask(shared_ptr<Task>).
		 * @see FindSharedTask()
		 * @return A pointer to the task if found, otherwise a null pointer.
		 */
		template< class T >
		shared_ptr<T> FindSharedTask(const std::string& name, bool deepSearch)
		{
			return dynamic_pointer_cast<T>(FindSharedTask(name, deepSearch));
		}
		
		/**
		 * Pause all active tasks.
		 * This results in the TaskManager state changing to paused.
		 */
		void PauseAllActiveTasks(bool applicationPause = false);

		/**
		 * Resume all previously active tasks.
		 * This results in the task manager's state to change to resumed.
		 */
		void ResumeAllPreviouslyActiveTasks(bool applicationResume = false);

		/**
		 * Resume all tasks.
		 * This method resumes all active tasks but does not change the task managers resumed state.
		 */
		void ResumeAllTasks(bool applicationResume = false);

		void TaskWasPaused(Task& task);
		void TaskWasResumed(Task& task);

		/**
		 * Update all active tasks.
		 * Calling this after StopTasks() may still result in tasks being updated. @see StopTasks() for more information.
		 */
		void UpdateTasks(Seconds lastFrameTime);
		
		//!\ brief Get the number of active tasks in this manager.
		//!\ param includeChildTaskManagerTasks if true include the number of tasks active in child state manages too.
		//!\ param outputTaskList Outputs task names to std cout.
		//!\ param indendDepth this method is recursive and this parameter is used to set the indent depth for the task lists.
		size_t GetNumberOfActiveTasks(bool includeChildTaskManagerTasks = true, bool outputTaskList = false,size_t indentDepth = 0) const;

		//!\ brief Get the number of tasks in this manager.
		//!\ param includeChildTaskManagerTasks if true include the number of tasks active in child state manages too.
		//!\ param outputTaskList Outputs task names to std cout.
		//!\ param indendDepth this method is recursive and this parameter is used to set the indent depth for the task lists.
		size_t GetNumberOfTasks(bool includeChildTaskManagerTasks = false, bool outputTaskList = false,size_t indentDepth = 0) const;
		
		/**
		 * Queue some action to be executed following the completion of the current or next UpdateTasks() call.
		 * Actions are executed after UpdateTasks() finishes updating tasks so if the action is queued during an update it will
		 * be performed immediately after, otherwise it will be performed after the next UpdateTasks() call.
		 * updated.
		 * This can be used when you're in a situation that you want to clean up some task in a hierarchy but doing so would delete
		 * object that are in use/part way through updating. TaskManager tasks care of these situations with tasks automatically
		 * and never updates deleted, removed or paused tasks even part way through an update but it has no control over
		 * specialised Tasks.
		 * @note Actions cannot be cancelled so only queue actions that you know will be valid when the TaskManager gets around to
		 * executing them.
		 * @param action Generic action to be called. The action is executed once then discarded.
		 */
		void QueuePostUpdateAction(Action action);
		
		/**
		 * Enable task profiling.
		 * This will report 
		 */
		void SetProfilingEnabled(bool enabled)
		{
			mProfilingEnabled = enabled;
		}
		
		/**
		 * Get the ProfilerAverager from which you can Get the last averages.
		 */
		const ProfilerAverager& GetProfilerAverager() const;

		/**
		 * Get a copy of the current the profiler.
		 */
		Profiler GetProfiler() const;

		/**
		 * Request the next results from the Profiler.
		 * This method ensures the profiler isn't accessed while profiling.
		 * @return false if profiling isn't enabled, otherwise true.
		 */
		bool RequestNextProfilerResults(function<void(const Profiler&)> callback);
	private:
		void UpdateLists();

		/**
		 * Internal structure used to manage the different ways a task might be added to a manager and
		 * to provide a consistent way for the manager to use Tasks without checking how each task was
		 * added to the manager.
		 */
		class TaskInfo
		{
		public:
			TaskInfo(Task* task, shared_ptr<Task> taskPtr = shared_ptr<Task>())
			{
				mHasBeenRemoved = make_shared<bool>(false);
				mHasBeenPaused = make_shared<bool>(false);
				mWaitingForResume = make_shared<bool>(false);
				mWaitingForPauseProcessing = make_shared<bool>(false);
				mCurrentPriority = task->GetPriority();
				mTask = task;
				mTaskPtr = taskPtr;
			}
			
			TaskInfo(const TaskInfo& otherTaskInfo)
			{
				mTask = otherTaskInfo.mTask;
				mTaskPtr = otherTaskInfo.mTaskPtr;
				mHasBeenRemoved = otherTaskInfo.mHasBeenRemoved;
				mHasBeenPaused = otherTaskInfo.mHasBeenPaused;
				mWaitingForResume = otherTaskInfo.mWaitingForResume;
				mWaitingForPauseProcessing = otherTaskInfo.mWaitingForPauseProcessing;
				mCurrentPriority = otherTaskInfo.mCurrentPriority;
			}
			
			TaskInfo& operator=(const TaskInfo& otherTaskInfo)
			{
				if (this == &otherTaskInfo)
					return *this;
				
				mTask = otherTaskInfo.mTask;
				mTaskPtr = otherTaskInfo.mTaskPtr;
				mHasBeenRemoved = otherTaskInfo.mHasBeenRemoved;
				mHasBeenPaused = otherTaskInfo.mHasBeenPaused;
				mWaitingForResume = otherTaskInfo.mWaitingForResume;
				mWaitingForPauseProcessing = otherTaskInfo.mWaitingForPauseProcessing;
				mCurrentPriority = otherTaskInfo.mCurrentPriority;
				return *this;
			}
			
			bool operator<(const TaskInfo& other) const
			{
				return (mCurrentPriority < other.mCurrentPriority);
			}
			
			bool operator==(const TaskInfo& otherTaskInfo) const
			{
				return (mTask == otherTaskInfo.GetTask());
			}
			
			bool operator==(const Task* otherTask) const
			{
				return (mTask == otherTask);
			}
			
			inline Task* GetTask() const
			{
				return mTask;
			}
			
			inline shared_ptr<Task> GetSharedTask()
			{
				return mTaskPtr;
			}
			
			inline bool GetHasBeenRemoved() const
			{
				return *mHasBeenRemoved;
			}
			
			void SetHasBeenRemoved(bool hasBeenRemoved)
			{
				*mHasBeenRemoved = hasBeenRemoved;
			}
			
			inline bool GetHasBeenPaused() const
			{
				return *mHasBeenPaused;
			}
			
			void SetHasBeenPaused(bool hasBeenPaused)
			{
				*mHasBeenPaused = hasBeenPaused;
			}
			
			inline bool GetWaitingForResume() const
			{
				return *mWaitingForResume;
			}
			inline bool GetWaitingForPauseProcessing() const
			{
				return *mWaitingForPauseProcessing;
			}
			
			void SetWaitingForResume(bool hasBeenResumed)
			{
				*mWaitingForResume = hasBeenResumed;
			}
			void SetWaitingForPauseProcessing(bool hasBeenPaused)
			{
				*mWaitingForPauseProcessing = hasBeenPaused;
			}			
			void UpdatePriority()
			{
				if(mTask)
				{
					mCurrentPriority = mTask->GetPriority();
				}
			}
		private:
			Task* mTask;
			shared_ptr<Task> mTaskPtr;					//!< If the task is added as a shared pointer this will be set.
			u32 mCurrentPriority;
			/*
			 * These members are always going to be valid pointers. They are shared
			 * pointers to reflect the state between lists.
			 */
			shared_ptr<bool> mHasBeenRemoved;			//!< Task has been removed.
			shared_ptr<bool> mHasBeenPaused;			//!< Task has been paused.
			shared_ptr<bool> mWaitingForResume;			//!< Task is waiting for resume processing to get into the active list.
			shared_ptr<bool> mWaitingForPauseProcessing;//!< Task is waiting for pause processing to be removed from the active list.
		};
		typedef std::list< TaskInfo > TaskList;
		typedef TaskList::iterator TaskListIterator;
		typedef TaskList::const_iterator TaskListConstIterator;
		
		bool AddTask(Task* task, shared_ptr<Task> taskPtr);
		
		std::vector<Action> mPostUpdateActions;
		std::string mManagerName;
		TaskList mTaskList;
		TaskList mActiveTaskList;
		TaskList mPreviouslyActiveTaskList;
		Profiler mProfiler;
		ProfilerAverager mProfilerAverager;
		Seconds mProfilerAveragerCurrentTime;
		Seconds mProfilerAveragerIntervalTime;
		bool mProfilingEnabled;
		bool mExecuting;
		bool mPaused;
		bool mUpdating;
	};
}
#endif
