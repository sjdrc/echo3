#include <echo/Application.h>
#include <echo/Chrono/CountDownTimer.h>
#include <echo/Kernel/SimpleExecutionModel.h>
#include <echo/Platform.h>
#include <echo/cpp/functional>

#include <doctest/doctest.h>

using namespace Echo;

class CountDownTimerTests : public Application
{
public:
	CountDownTimerTests() : mRepeatCount(0), mSingleFinishCount(0)
	{
		SetExecutionModel(make_shared<SimpleExecutionModel>());
		
		Initialise("CountDownTimerTests");
		SetPauseHint(false);

		mSingleTest = new CountDownTimer(Seconds(0.5f));
		mSingleTest->AddTimeoutFunction(bind(&CountDownTimerTests::SingleTestFinished,this),"CountDownExpire");
		AddTask(*mSingleTest);

		mRepeatTest = new CountDownTimer(Seconds(0.5f));
		mRepeatTest->SetRepeat(true);
		mRepeatTest->AddTimeoutFunction(bind(&CountDownTimerTests::RepeatTestFinished,this),"CountDownRepeat");
		AddTask(*mRepeatTest);
	}
	~CountDownTimerTests()
	{
		CHECK(mRepeatCount==5);
		RemoveTask(*mSingleTest);
		RemoveTask(*mRepeatTest);
	}
	
	void SingleTestFinished()
	{
		mSingleFinishCount++;
		CHECK(mSingleFinishCount==1);
		RemoveTask(mSingleTest);
	}

	void RepeatTestFinished()
	{
		mRepeatCount++;
		if(mRepeatCount==5)
		{
			RemoveAllTasks();
		}
	}
private:
	CountDownTimer* mSingleTest;
	CountDownTimer* mRepeatTest;
	Size mRepeatCount;
	Size mSingleFinishCount;
};

TEST_CASE("CountDownTimer")
{
	// Turn off log output, we will just use output from this test.
	gDefaultLogger.SetLogMask(Echo::Logger::LogLevels::NONE);
	CountDownTimerTests countDownTimerTests;
	countDownTimerTests.Execute();
}
