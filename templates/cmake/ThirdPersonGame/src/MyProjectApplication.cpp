#include <MyProjectApplication.h>
#include <echo/Graphics/MultiRenderer.h>
#include <boost/bind.hpp>
#include <Game.h>
#include <MainMenu.h>
#include <echo/Util/StringUtils.h>

namespace MyProject
{
	MyProjectApplication::MyProjectApplication()
	{
	}

	MyProjectApplication::~MyProjectApplication()
	{
	}

	bool MyProjectApplication::Initialise()
	{
		//Initialise the application from a config file.
		if (!Application::Initialise("MyProjectApplication", "resources/MyProjectApplication.config"))
		{
			return false;
		}
		
		// Set up a binder for application context switches (you might want to add more features to this
		// later if you decide you need more global binder functionality)
		shared_ptr<FunctionBinder> appFunctions(new FunctionBinder());
		appFunctions->Register("Application.SwitchState",boost::bind(&ContextSwitcher::SwitchContext,
						boost::ref(GetContextSwitcher()),_1),false,boost::fusion::vector<std::string>());

		// This binding works as an application exit because Application is a Kernel. When there are
		// no more tasks in a Kernel the Kernel will exit.
		appFunctions->RegisterVoid("Application.Exit",boost::bind(&TaskManager::RemoveAllTasks,this));

		//Create our main tasks
		AddTask(make_shared<MainMenu>(shared_from_this<Application>(),appFunctions));
		//Set the game state to be an earlier priority than any menu (default priority is 5000).
		AddTask(make_shared<Game>(shared_from_this<Application>(),appFunctions, 2000));

		mContextSwitcher.SetTaskManager(shared_from_this());
		
		// In the Echo Application Template we define explicit states that are not designed to ever
		// be active at the same time. This makes set up easy since we can create a MultiRenderer
		// for each explicit state and just be sure that only one is active at once (easily done with
		// TaskManagers), however it is not uncommon to want more than one high level state active at
		// once. For example you might want to reuse the menu state which can be visible on start up
		// over a scene (or in our case here, nothing), and if the game is paused the same menu is
		// displayed except over a paused game.
		//
		// If you were to continue along the same pattern as the Echo Application Template you would
		// probably find yourself repeating yourself to duplicate the same menu. You might only repeat
		// yourself a little but you can avoid it by creating multiple Contexts that define what should
		// be active together.
		// 
		// In the unmodified version of this template, we do what I mentioned just before - start in
		// the menu then enter into a game. If the game is paused then the menu is displayed over the
		// top of a paused frame.
		//
		// The menu would need to be duplicated without using Contexts, which isn't ideal. But instead
		// all we need to do is create three contexts which define the following:
		//
		//	Context:MainMenu
		//		MainMenu active and rendering
		//		Game paused and not rendering
		//
		//	Context:Game
		//		MainMenu paused and not rendering
		//		Game active and rendering
		//
		//	Context:GamePause
		//		MainMenu active and rendering
		//		Game pause but rendering
		//
		// Context switching is bound through the above function binder which is passed on through
		// the application. 
		
		// This function binder is set up to pass the responsibility of configuring context application configuration.
		// We set up two functions for this. This binder is discarded when we're done.
		//	- Resume(contextName, taskName)
		//  - Pause(contextName, taskName)
		shared_ptr<FunctionBinder> contextSetupFunctions(new FunctionBinder());
		contextSetupFunctions->Register("Resume",boost::bind(&ContextSwitcher::AddTaskToResume,
						boost::ref(mContextSwitcher),_1,_2),false,boost::fusion::vector<std::string,std::string>());
		contextSetupFunctions->Register("Pause",boost::bind(&ContextSwitcher::AddTaskToPause,
						boost::ref(mContextSwitcher),_1,_2),false,boost::fusion::vector<std::string,std::string>());
		std::string contextList=GetConfiguration().Get<std::string>("context","");

		// Process lines: context=Pause|Resume(contextName,taskName)
		std::vector<std::string> contextFunctions=GetConfiguration().GetAllAsStrings("context");
		BOOST_FOREACH(const std::string& contextFunction, contextFunctions)
		{
			contextSetupFunctions->Call(contextFunction);
		}
		
		// We're going to use the same multi renderer across all states and each state will
		// pause its renderer as needed.
		AddTask(*GetRenderer());
		
		// Set the initial context, we'll default to MainMenu if this isn't set.
		mContextSwitcher.SwitchContext(GetConfiguration().Get("initial.context","MainMenu"));
		return true;
	}
}
