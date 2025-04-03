#include <Game.h>
#include <echo/Graphics/MultiRenderer.h>
#include <echo/Graphics/Viewport.h>
#include <echo/Graphics/SceneEntity.h>
#include <echo/Graphics/Camera.h>
#include <echo/Graphics/Mesh.h>
#include <echo/Graphics/Light.h>
#include <echo/Resource/MaterialManager.h>
#include <echo/Util/Oscillator.h>
#include <echo/GUI/Menu.h>

#include <boost/bind.hpp>

namespace MyProject
{
	Game::Game(shared_ptr<Application> application, shared_ptr<FunctionBinder> appFunctions) :
		mApplication(application),
		mInitialised(false),
		mAppFunctionBinder(appFunctions)
	{
		SetTaskName("Game");
		Initialise();
	}
		
	Game::~Game()
	{
	}

	bool Game::Initialise()
	{
		if(!mApplication)
		{
			std::cout << "Error: Game::Initialise(): mApplication is null" << std::endl;
			return false;
		}
		if(mInitialised)
		{
			return true;
		}
		RemoveAllTasks();

		shared_ptr<MultiRenderer> renderer=make_shared<MultiRenderer>("Renderer");
		renderer->AddRenderTarget(mApplication->GetRenderTarget());
		AddTask(renderer);

		AddTask(mScene);
		
		// Each camera will be rendered through a viewport. A viewport defines the
		// area of a render target that will be rendered to. These two viewports
		// define the left half and right half of the render target.
		shared_ptr<Viewport> leftViewport(new Viewport(0,0,0.5,1));
		shared_ptr<Viewport> rightViewport(new Viewport(0.5,0,1,1));

		// We'll create two cameras, each looking from different positions at 0,0,0.
		
		// Camera 1
		shared_ptr<Camera> camera1 = mScene.CreateCamera();
		
		// Adjust the aspect ratio for the viewport so things don't appear stretched.
		// This method is called by Renderers when rendering but we need to call it now because
		// we need the aspect ratio adjusted for the next method.
		camera1->UpdateAspectForViewport(*leftViewport, *mApplication->GetRenderTarget());

		// Horizontal view takes up 10 units looking at 0,0,0 from 0,0,1
		camera1->CalculateAndSetDistanceForXFOV(6,Vector3(0,0,0),Vector3(0,0,1));

		// Camera 2
		shared_ptr<Camera> camera2 = mScene.CreateCamera();
		camera2->UpdateAspectForViewport(*rightViewport, *mApplication->GetRenderTarget());

		// Vertical view takes up 50 units at looking at 0,0,0 from 0,0,-1
		camera2->CalculateAndSetDistanceForYFOV(15,Vector3(0,0,0),Vector3(0,0,-1));
		
		shared_ptr<Renderer> leftRenderer = renderer->CreateRenderer(leftViewport, camera1);
		renderer->CreateRenderer(rightViewport, camera2);
		
		//Create a cube mesh with the Echo logo as a texture.
		shared_ptr<Mesh> mesh(new Mesh());
		mesh->CreateCubeSubMesh(Vector3(5,5,5));
		mesh->SetMaterial(mApplication->GetMaterialManager()->GetResource("LitEcho"));

		// Create a SceneEntity and give it the cube mesh. 
		shared_ptr<SceneEntity> entity(new SceneEntity());
		entity->SetMesh(mesh);
		
		//Add the entity to be rendered to the scene.
		mScene.AddRenderable(entity);
		
		//Create an Oscillator to rotate the entity.
		shared_ptr< Oscillator<Radian> > spinner(new Oscillator<Radian>(boost::bind(&Node::SetOrientationEuler,entity,_1,_2,Radian(0))));

		// The Oscillator needs to be added to a TaskManager to be updated, the
		// Scene is a good choice because we'll likely want the spinning to stop
		// when this Scene is paused.
		mScene.AddTask(spinner);

		//Create some lights to light up the scene.
		shared_ptr<Light> light = mScene.CreateLight();
		light->SetPosition(Vector3(0,0,10));
		light->SetAttenuation(50,0,0.1,0);

		light = mScene.CreateLight();
		light->SetPosition(Vector3(-5,2,0));
		light->SetAttenuation(50,0,0.1,0);
		light->SetDiffuseColour(Colours::RED);

		light = mScene.CreateLight();
		light->SetPosition(Vector3(5,2,0));
		light->SetAttenuation(50,0,0.1,0);
		light->SetDiffuseColour(Colours::BLUE);

		light = mScene.CreateLight();
		light->SetPosition(Vector3(0,-2,-10));
		light->SetAttenuation(50,0,0.1,0);
		light->SetDiffuseColour(Colours::PURPLE);
		
		//A Menu is used as a HUD.
		shared_ptr<GUI::Menu> menu(new GUI::Menu(mApplication->GetFileSystem(),
												 mApplication->GetMaterialManager(),
												 mApplication->GetFontManager(),
												 mApplication->GetInputManager(),
												 renderer,
												 mAppFunctionBinder));
		menu->AddGUI("resources/Menu/HUD.gui","HUD");
		menu->Push("HUD");
		AddTask(menu);
		
		mInitialised = true;
		return true;
	}
	
	bool Game::OnStart()
	{
		if(!Initialise())
		{
			return false;
		}
		return TaskGroup::OnStart();
	}
}
