#include <echo/Graphics/MultiRenderer.h>
#include <echo/Graphics/Viewport.h>
#include <echo/Graphics/Scene.h>
#include <echo/Graphics/SceneEntity.h>
#include <echo/Graphics/Camera.h>
#include <echo/Graphics/Mesh.h>
#include <echo/Graphics/Light.h>
#include <echo/Resource/MaterialManager.h>
#include <echo/Util/Oscillator.h>

#include <echo/GUI/Menu.h>

#include <boost/bind.hpp>
#include <MyProjectApplication.h>
#include <Game.h>

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
		
		// Welcome to your Echo application.
		//
		// Echo applications are made up of Tasks that are managed by one or more TaskManagers.
		// A Task is just an object that can be started, updated, paused, resumed and stopped.
		// You will find yourself working with tasks a lot. You will create custom Tasks and
		// configure hierarchies of Tasks so common Tasks can be active at the same time.
		//
		// TaskManagers are objects that manage the activation and updates of tasks. There are
		// three types of TaskManagers.
		//	1. TaskManager, the base type, objects of this type maintain a list of Tasks and
		//		are asked to start, stop, pause and resume tasks. They will update any active
		//		Tasks when TaskManager::UpdateTasks() is called.
		//  2. Kernel (such as this Application). These are special TaskManager types that are
		//		given an ExecutionModel which determines when and how Tasks will be updated.
		//		You won't often create these but I just want to point out that Kernel is a
		//		TaskManager which will make the hierarchy below make more sense.
		//	3. TaskGroup - Just a TaskManager that is also a Task. These TaskManagers update
		//		their tasks when they are updated by another TaskManager.
		//
		// We will be creating an application that has the following hierarchy of Tasks.
		//
		//	Kernel (the MyProjectApplication instance)
		//		InputManager (created and added by Application)
		//		Menu
		//			GUI (Main)
		//				Scene
		//			GUI (Settings)
		//				Scene
		//			MultiRenderer*
		//				Renderer (GUI Main Scene)
		//				Renderer (GUI Settings Scene)
		//		Game
		//			Scene (includes a SceneEntity with a cube mesh)
		//				Oscillator (spins the SceneEntity)
		//			Menu
		//				GUI (HUD)
		//			MultiRenderer **
		//				Renderer (Left Game Scene Viewport)
		//				Renderer (Right Game Scene Viewport)
		//				Renderer (HUD GUI Scene)
		//
		//	The Menu and Game objects added to the Kernel are TaskGroups that we will use to
		//	define our application states.
		//
		//	* The MultiRenderer in Menu is controlled by Menu and only one of the Renderers will
		//		be active at once. Menu will actually switch out the active Renderer based on which
		//		menu screen to display so only one Renderer will be in the MultiRenderer at any one
		//		time. More information is below about MultiRenderers.
		//	** This MultiRenderer will have three Scenes rendering at once so the HUD scene is
		//		rendered on top of the Game Scene.
		//
		// This might seem like a lot of work, but GUI::Menu does all of the set up for the GUI
		// related scenes and Renderers. We mostly just set up our scene and tell MultiRenderer
		// how we want to render things in the Game mode.
		//
		// Switching between Game and the Menu states
		// ------------------------------------------
		//
		// This template application is configured such that when buttons are pressed to change
		// application state they first deactivate the expected current Task then activate the next
		// Task. This is a manual way to accomplish an application state switch and is fine when your
		// application is relatively simple or if your states are explicitly defined by TaskGroups.
		// If you have more complicated combinations of Tasks where some tasks might be active in
		// multiple states but not in others then you might prefer to configure your active task
		// combinations with a ContextSwitcher.
		//
		// A ContextSwitcher allows you to create multiple Contexts which are just lists of tasks
		// that should be active and inactive. You can switch context using SwitchContext("Name")
		// which could be bound here instead of the task manager methods below.
		//
		// This application is configured to have two states - a Game state and a Menu state. The
		// Menu operates mostly within the GUI system (which supports switching between multiple
		// Menu Scenes through various default bound functions - this is why the template can switch
		// between multiple menus via the buttons).
		//
		//Bind some functions that will be called from the menu buttons.
		shared_ptr<FunctionBinder> appFunctions(new FunctionBinder());
		appFunctions->Register("State.Activate",boost::bind(&TaskManager::ResumeTask,this,_1,true),false,boost::fusion::vector<std::string>());
		appFunctions->Register("State.Deactivate",boost::bind(&TaskManager::PauseTask,this,_1,true),false,boost::fusion::vector<std::string>());

		// This binding works as an application exit because Application is a Kernel. When there are
		// no more tasks in a Kernel the Kernel will exit.
		appFunctions->RegisterVoid("Application.Exit",boost::bind(&TaskManager::RemoveAllTasks,this));

		// Create a Game object and set it to initially be paused.
		shared_ptr<Game> game=make_shared<Game>(shared_from_this<Application>(),appFunctions);
		AddTask(game);
		game->Pause();

		// To render using Echo's render system you use a Renderer Task. Renderers are given a
		// Viewport, a Camera and a RenderTarget to render to. They are simple objects that set up
		// a RenderTarget for rendering which might include clearing the frame, depth or other
		// buffers.
		//
		// Renderers will perform the rendering of a Scene (through a Camera) whenever Update() is
		// called (it is just a Task). This gives you great flexibility as to when Rendering is
		// performed. You can Render before, after, or in parallel to, some other Task. You can
		// perform some background Rendering task in another thread if needed. In short - you decide
		// when Rendering takes place.
		//
		// A MultiRenderer is used to help to automatically configure one or more Renderers so they
		// can render in the same frame without clearing any buffers. You could accomplish this
		// yourself by setting the appropriate flags on Renderer, but since it is a common
		// requirement MultiRenderer can be used for convenience. You can still refine a Renderer's
		// settings after MultiRenderer has set the defaults if you need finer control.
		//
		// You would use a MultiRenderer if you wanted to render multiple scenes in the same frame.
		// A Scene is defined as a hierarchy of objects that should be rendered together in the same
		// space. In this template, we set up multiple states. One that switches between GUI Scenes
		// that are active exclusively (one Renderer is active at a time during this state), as well
		// as a Game state that contains a 3D Scene and a GUI Scene, each of which are independent
		// of one another but need to be rendered in the same frame.
		//
		// Application is set up with a MultiRenderer by default so you do not necessarily need to
		// create one. However you still need to add the MultiRenderer to a TaskManager to make it
		// render (Application does not make an assumption that the default MultiRenderer should
		// render in every update).
		// 
		// You will probably create more than one MultiRenderer yourself, as is done in this template
		// application. A MultiRenderer is used in each state that is intended to be exclusively
		// active (Game and Menu). The Menu uses the default MultiRenderer object which is activated
		// as part of the Menu below. The other MultiRenderer in the game state is created in Game
		// and is intended to be active during the Game. You may notice that Game passes its
		// MultiRenderer to a GUI object during the GUI construction. This is so the GUI can render
		// during the same Frame (GUI requires a MultiRenderer so it does not need to manage Renderer
		// settings).
		shared_ptr<GUI::Menu> menu(new GUI::Menu(GetFileSystem(),GetMaterialManager(),GetFontManager(),GetInputManager(),GetRenderer(),appFunctions));
		menu->SetTaskName("Menu");
		menu->LoadGUIList("resources/Menu/Menus.list");
		
		// If we do not add the default renderer to the Menu Task (which is active by default) then
		// we will not see anything rendered. You can experiment here and add the Game MultiRenderer
		// instead:
		//
		// //menu->AddTask(GetRenderer());
		// game->FindTask("Renderer",false)->Resume();
		// menu->AddTask(game->FindTask("Renderer",false));
		//
		// Note that we need to explicitly resume the task here because it is initially configured
		// to be paused. On a relevant side note, TaskManagers preserve activation state so you can
		// can pause the TaskManager and when you resume it the tasks that were previously active
		// will be active again, but the previously inactive tasks won't be.
		//
		// You can see the effect of having multiple MultiRenderers active at once by adding both
		// to the menu state. You will see that they oscillate rendering.
		//
		// menu->AddTask(GetRenderer());
		// game->FindTask("Renderer",false)->Resume();
		// menu->AddTask(game->FindTask("Renderer",false));
		//
		// A final thing to note is that the Renderer for Game, when activated in this case renders
		// the same frame, but if you navigate to the Game state the cube is spinning. This
		// demonstrates that a Renderer can render a Scene that is not actively updating. Also note
		// that adding the Game MultiRenderer to the Menu state causes it to become paused when the
		// Menu is paused. This means the Game state no longer renders (you could resolve this
		// issue by using Context's to ensure the MultiRenderer was active in both Contexts as
		// mentioned above).
		
		menu->AddTask(GetRenderer());
		AddTask(menu);
		
		return true;
	}
}
