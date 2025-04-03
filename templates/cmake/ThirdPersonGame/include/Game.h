#ifndef _ThirdPersonGame_GAME_H_
#define _ThirdPersonGame_GAME_H_

#include <echo/Kernel/TaskGroup.h>
#include <echo/Graphics/Scene.h>
#include <echo/Application.h>

using namespace Echo;

namespace MyProject
{
	class Game : public TaskGroup
	{
	public:
		Game(shared_ptr<Application> application, shared_ptr<FunctionBinder> appFunctions, u32 priority);
		~Game();
		
	private:
		bool Initialise();
		virtual bool OnStart() override;
		Scene mScene;
		shared_ptr<Application> mApplication;
		shared_ptr<FunctionBinder> mAppFunctionBinder;
		bool mInitialised;
	};
}

#endif
