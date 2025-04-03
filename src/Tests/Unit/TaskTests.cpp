#include <echo/Kernel/TaskGroup.h>
#include <doctest/doctest.h>

void AddRemoveTaskTest()
{
	using namespace Echo;
	TaskManager manager;
	Task task1;
	CHECK(manager.AddTask(task1));
	CHECK(manager.GetNumberOfTasks(false)==1);
	CHECK(task1.GetNumberOfTaskManagers()==1);
	manager.RemoveTask(task1);
	CHECK(manager.GetNumberOfTasks(false)==0);
}

void AddRemoveMultipleTaskTest()
{
	using namespace Echo;
	TaskManager manager;
	Task task1;
	shared_ptr<Task> task2(new Task("Task2"));
	CHECK(manager.AddTask(task1));
	CHECK(manager.AddTask(task2));
	CHECK(manager.GetNumberOfTasks(false)==2);
	CHECK(manager.StartTasks());
	CHECK(manager.GetNumberOfActiveTasks(false)==2);
	
	Task* taskSearch = manager.FindTask("Task2",false);
	CHECK(taskSearch!=nullptr);
	CHECK(taskSearch==task2.get());
	
	shared_ptr<Task> taskSharedSearch = manager.FindSharedTask("Task2",false);
	CHECK(taskSharedSearch!=nullptr);
	CHECK(taskSharedSearch==task2);
	
	manager.RemoveTask(task1);
	manager.RemoveTask("Task2");
	CHECK(manager.GetNumberOfTasks(false)==0);
}

class SimpleTask : public Echo::Task
{
public:
	SimpleTask(const std::string& name, Echo::u32 priority = 5000) : Echo::Task(name, priority), mUpdateCount(0)
	{
	}
	size_t GetUpdateCount() const {return mUpdateCount;}
private:
	size_t mUpdateCount;
	void Update(Echo::Seconds lastFrameTime)
	{
		std::cout << GetTaskName() << std::endl;
		++mUpdateCount;
	}
};
	
void UpdateTasksTest()
{
	using namespace Echo;
	TaskManager manager;
	
	SimpleTask task1("1",10);
	shared_ptr<SimpleTask> task2(new SimpleTask("2",20));
	manager.AddTask(task1);
	manager.AddTask(task2);
	manager.StartTasks();
	manager.UpdateTasks(Seconds(1.0f));
	CHECK(task1.GetUpdateCount()==1);
	CHECK(task2->GetUpdateCount()==1);
	manager.RemoveTask(task1);
	manager.RemoveTask(task2);
	CHECK(manager.GetNumberOfTasks(false)==0);
}

void HierarchyTest()
{
	using namespace Echo;
	TaskManager manager;
	TaskGroup group;
	
	SimpleTask taskA1("taskA1");
	SimpleTask taskA2("taskA2");
	SimpleTask taskB1("taskB1");
	SimpleTask taskB2("taskB2");
	
	group.AddTask(taskB1);
	group.AddTask(taskB2);
	
	manager.AddTask(group);
	manager.StartTasks();

	CHECK_MESSAGE(manager.GetNumberOfActiveTasks(true)==3,"Not all tasks are active that should be");
	
	manager.AddTask(taskA1);
	manager.AddTask(taskA2);

	CHECK_MESSAGE(manager.GetNumberOfActiveTasks(true)==5,"Not all tasks are active that should be after adding more tasks");
	
	//Perform some searches
	//Should be top level
	CHECK_MESSAGE(manager.FindTask("taskA1", false)==&taskA1,"Could not find task taskA1");
	CHECK_MESSAGE(manager.FindTask("taskA2", false)==&taskA2,"Could not find task taskA2");
	CHECK_MESSAGE(manager.FindTask("taskB1", false)==nullptr,"Found task taskB1 in child group when not attempting a deep search");
	CHECK_MESSAGE(manager.FindTask("taskB2", false)==nullptr,"Found task taskB2 in child group when not attempting a deep search");
	CHECK_MESSAGE(manager.FindTask("taskB1", true)==&taskB1,"Could not find task taskB1 when attempting a deep search");
	CHECK_MESSAGE(manager.FindTask("taskB2", true)==&taskB2,"Could not find task taskB1 when attempting a deep search");
	
	//We know searches work, lets try a casted search.
	CHECK_MESSAGE(manager.FindTask<SimpleTask>("taskB1", true)==&taskB1,"Could not find specialised taskB1 when attempting a deep search and cast");
	
	manager.UpdateTasks(Seconds(1.0f));
	CHECK(taskA1.GetUpdateCount()==1);
	CHECK(taskA2.GetUpdateCount()==1);
	CHECK(taskB1.GetUpdateCount()==1);
	CHECK(taskB2.GetUpdateCount()==1);
}

TEST_CASE("Task")
{
	// Turn off log output, we will just use output from this test.
	Echo::gDefaultLogger.SetLogMask(Echo::Logger::LogLevels::NONE);

	AddRemoveTaskTest();
	AddRemoveMultipleTaskTest();
	UpdateTasksTest();
	HierarchyTest();
}
