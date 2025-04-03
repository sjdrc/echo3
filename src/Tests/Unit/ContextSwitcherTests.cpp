#include <echo/Util/ContextSwitcher.h>
#include <echo/Kernel/TaskGroup.h>
#include <doctest/doctest.h>

using namespace Echo;

void SimpleSwitch()
{
	// Set the context and make sure you can switch from one to another.
	
	shared_ptr<TaskManager> manager(new TaskManager());
	shared_ptr<Task> a(new Task("A"));
	shared_ptr<Task> b(new Task("B"));
	shared_ptr<Task> c(new Task("C"));
	shared_ptr<Task> d(new Task("D"));
	manager->AddTask(a);
	manager->AddTask(b);
	manager->AddTask(c);
	manager->AddTask(d);
	manager->StartTasks();
	a->Pause();
	b->Pause();
	
	//Verify the pre-task states.
	CHECK(a->GetPaused());
	CHECK(b->GetPaused());
	CHECK(!c->GetPaused());
	CHECK(!d->GetPaused());
	
	ContextSwitcher switcher(manager);
	shared_ptr<ContextSwitcher::Context> one = switcher.CreateContext("One");
	one->AddTaskToResume("A");
	one->AddTaskToPause("B");
	one->AddTaskToPause("C");
	
	shared_ptr<ContextSwitcher::Context> two = switcher.CreateContext("Two");
	two->AddTaskToPause("A");
	two->AddTaskToResume("B");
	two->AddTaskToPause("C");

	//Now switch some contexts.
	CHECK_MESSAGE(switcher.SwitchContext("One"),"Unexpected SwitchContext failure for context \"One\"");
	
	//Verify the "One" task states
	CHECK(!a->GetPaused());
	CHECK(b->GetPaused());
	CHECK(c->GetPaused());
	CHECK(!d->GetPaused());

	//"Two"
	CHECK_MESSAGE(switcher.SwitchContext("Two"),"Unexpected SwitchContext failure for context \"Two\"");

	//Verify the "Two" task states
	CHECK(a->GetPaused());
	CHECK(!b->GetPaused());
	CHECK(c->GetPaused());
	CHECK(!d->GetPaused());
	
	//Back to "One"
	CHECK_MESSAGE(switcher.SwitchContext("One"),"Unexpected SwitchContext failure for context \"One\"");
	
	//Verify the "One" task states
	CHECK(!a->GetPaused());
	CHECK(b->GetPaused());
	CHECK(c->GetPaused());
	CHECK(!d->GetPaused());
}

void SwitchFail()
{
	// Set the context and make sure context switch failure doesn't change the tasks states.
	shared_ptr<TaskManager> manager(new TaskManager());
	shared_ptr<Task> a(new Task("A"));
	shared_ptr<Task> b(new Task("B"));
	shared_ptr<Task> c(new Task("C"));
	shared_ptr<Task> d(new Task("D"));
	manager->AddTask(a);
	manager->AddTask(b);
	manager->AddTask(c);
	manager->AddTask(d);
	manager->StartTasks();
	a->Pause();
	b->Pause();
	
	//Verify the pre-task states.
	CHECK(a->GetPaused());
	CHECK(b->GetPaused());
	CHECK(!c->GetPaused());
	CHECK(!d->GetPaused());
	
	ContextSwitcher switcher(manager);
	shared_ptr<ContextSwitcher::Context> one = switcher.CreateContext("One", ContextSwitcher::FailureActions::STOP);
	one->AddTaskToResume("A");
	one->AddTaskToPause("B");
	one->AddTaskToPause("C");
	one->AddTaskToPause("MakeBelieve");

	//Try and switch to "One", it should fail.
	CHECK(!switcher.SwitchContext("One"));
	CHECK(a->GetPaused());
	CHECK(b->GetPaused());
	CHECK(!c->GetPaused());
	CHECK(!d->GetPaused());
}

void SwitchFailWithFallback()
{
	// Set the context and make sure context switch failure doesn't change the tasks states.
	shared_ptr<TaskManager> manager(new TaskManager());
	shared_ptr<Task> a(new Task("A"));
	shared_ptr<Task> b(new Task("B"));
	shared_ptr<Task> c(new Task("C"));
	shared_ptr<Task> d(new Task("D"));
	manager->AddTask(a);
	manager->AddTask(b);
	manager->AddTask(c);
	manager->AddTask(d);
	manager->StartTasks();
	a->Pause();
	b->Pause();
	
	//Verify the pre-task states.
	CHECK(a->GetPaused());
	CHECK(b->GetPaused());
	CHECK(!c->GetPaused());
	CHECK(!d->GetPaused());
	
	ContextSwitcher switcher(manager);
	shared_ptr<ContextSwitcher::Context> one = switcher.CreateContext("One",ContextSwitcher::FailureActions::FALL_BACK_CONTEXT,"Two");
	one->AddTaskToResume("A");
	one->AddTaskToPause("B");
	one->AddTaskToPause("C");
	one->AddTaskToPause("MakeBelieve");
	
	//Try and switch to "One", it should fail, but fall back to Context "Two"
	shared_ptr<ContextSwitcher::Context> two = switcher.CreateContext("Two");
	two->AddTaskToPause("A");
	two->AddTaskToResume("B");
	two->AddTaskToPause("C");

	//Try and switch to "One", it should fail.
	CHECK(switcher.SwitchContext("One"));

	//Make sure all of the tasks have "Two" states.
	CHECK(a->GetPaused());
	CHECK(!b->GetPaused());
	CHECK(c->GetPaused());
	CHECK(!d->GetPaused());
}

TEST_CASE("ContextSwitcher")
{
	// Turn off log output, we will just use output from this test.
	gDefaultLogger.SetLogMask(Echo::Logger::LogLevels::NONE);

	SimpleSwitch();
	SwitchFail();
	SwitchFailWithFallback();
}

