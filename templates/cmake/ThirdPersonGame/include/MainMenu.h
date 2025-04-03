#ifndef _ThirdPersonGame_MAINMENU_H_
#define _ThirdPersonGame_MAINMENU_H_

#include <echo/Kernel/TaskGroup.h>
#include <echo/Graphics/Scene.h>
#include <echo/Application.h>

using namespace Echo;

namespace MyProject
{
	class MainMenu : public TaskGroup
	{
	public:
		MainMenu(shared_ptr<Application> application, shared_ptr<FunctionBinder> appFunctions);
		~MainMenu();
		
	private:
		bool Initialise();
		virtual bool OnStart() override;
		shared_ptr<Application> mApplication;
		shared_ptr<FunctionBinder> mAppFunctionBinder;
		bool mInitialised;
	};
}

#endif
