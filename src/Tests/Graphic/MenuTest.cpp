#include <echo/Application.h>
#include <echo/Chrono/CountDownTimer.h>
#include <echo/GUI/Menu.h>
#include <echo/Graphics/MultiRenderer.h>
#include <echo/Util/FunctionBinder.h>
#include <echo/cpp/functional>

#include <doctest/doctest.h>

using namespace Echo;
using namespace Echo::GUI;

class MenuTest : public Application
{
public:
	MenuTest()
		: mTimeout(Seconds(30.0), bind(&TaskManager::RemoveAllTasks, this), "RemoveAllTasks")
	{
		Initialise("MenuTest","data/MenuTest.config");
		
		AddTask(GetRenderer());
		AddTask(mTimeout);
		
		shared_ptr<FunctionBinder> functionBinder = make_shared<FunctionBinder>();
		functionBinder->RegisterVoid("Exit", bind(&Kernel::Stop, this));
		functionBinder->RegisterVoid("ResetTimeout", bind(&CountDownTimer::SetTimeout, ref(mTimeout), Seconds(30.)));
		
		shared_ptr<Menu> menu(new Menu(GetFileSystem(), GetMaterialManager(), GetFontManager(), GetInputManager(), GetRenderer(), functionBinder));
		menu->AddGUI("data/MenuA.gui","MenuA");
		menu->AddGUI("data/MenuB.gui","MenuB");
		menu->AddGUI("data/MenuC.gui","MenuC");
		menu->Push("MenuA");
		AddTask(menu);
	}
private:
	CountDownTimer mTimeout;
};

TEST_CASE("Menu")
{
	// Turn off log output, we will just use output from this test.
	gDefaultLogger.SetLogMask(Echo::Logger::LogLevels::NONE);
	MenuTest menuTest;
	menuTest.Execute();
}
