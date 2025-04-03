#include <MainMenu.h>
#include <echo/Graphics/MultiRenderer.h>
#include <echo/Graphics/Viewport.h>
#include <echo/Resource/MaterialManager.h>
#include <echo/Util/Oscillator.h>
#include <echo/GUI/Menu.h>

#include <boost/bind.hpp>

namespace MyProject
{
	MainMenu::MainMenu(shared_ptr<Application> application, shared_ptr<FunctionBinder> appFunctions) :
		mApplication(application),
		mInitialised(false),
		mAppFunctionBinder(appFunctions)
	{
		SetTaskName("MainMenu");
		Initialise();
	}
		
	MainMenu::~MainMenu()
	{
	}

	bool MainMenu::Initialise()
	{
		if(!mApplication)
		{
			std::cout << "Error: MainMenu::Initialise(): mApplication is null" << std::endl;
			return false;
		}
		if(mInitialised)
		{
			return true;
		}
		RemoveAllTasks();

		//A Menu is used as a HUD.
		shared_ptr<GUI::Menu> menu(new GUI::Menu(mApplication->GetFileSystem(),
												 mApplication->GetMaterialManager(),
												 mApplication->GetFontManager(),
												 mApplication->GetInputManager(),
												 mApplication->GetRenderer(),
												 mAppFunctionBinder));
		menu->SetTaskName("MainMenu.Menu");
		menu->LoadGUIList("resources/Menu/Menus.list");
		AddTask(menu);
		
		mInitialised = true;
		return true;
	}
	
	bool MainMenu::OnStart()
	{
		if(!Initialise())
		{
			return false;
		}
		return TaskGroup::OnStart();
	}
}
