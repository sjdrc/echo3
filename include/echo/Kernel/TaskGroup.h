#ifndef _ECHO_TASKGROUP_H_
#define _ECHO_TASKGROUP_H_
#include <echo/Kernel/TaskManager.h>

namespace Echo
{
	/**
	 */
	class TaskGroup : public TaskManager, public Task
	{
	public:
		TaskGroup();
		TaskGroup(const std::string& taskGroupName, u32 priority = 5000);
		virtual ~TaskGroup();
		virtual bool OnStart() override;
		virtual void OnStop() override;
		virtual void OnPause(bool applicationPause) override;
		virtual void OnResume(bool applicationResume) override;
		virtual void Update(Seconds lastFrameTime) override;

		/**
		 * Overrides to make sure names are synchronised in the TaskManager and the Task.
		 */
		virtual void SetTaskName(const std::string& name) override;
		virtual void SetTaskManagerName(const std::string& name) override;
	};
}
#endif
