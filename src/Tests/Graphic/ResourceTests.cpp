#include <echo/Graphics/SceneEntity.h>
#include <echo/Kernel/Task.h>
#include <echo/Application.h>
#include <echo/Graphics/Scene.h>
#include <echo/Resource/MaterialManager.h>
#include <echo/Resource/TextureManager.h>
#include <echo/Graphics/MultiRenderer.h>
#include <echo/Graphics/Viewport.h>
#include <echo/Graphics/Camera.h>
#include <echo/Input/Input.h>
#include <echo/cpp/functional>

#include "SpinningCube.h"

#include <doctest/doctest.h>
#undef INFO

using namespace Echo;

namespace ResourceReloadTest
{
	class ResourceReloadTestApplication : public Application
	{
	public:
		bool Initialise()
		{
			//Initialise the application from a config file.
			if (!Application::Initialise("ResourceReloadTestApplication", "data/ResourceReloadTest.config"))
			{
				return false;
			}

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
			camera1->UpdateAspectForViewport(*leftViewport, *GetRenderTarget());

			// Horizontal view takes up 10 units looking at 0,0,0 from 0,0,1
			camera1->CalculateAndSetDistanceForXFOV(10,Vector3(0,0,0),Vector3(0,0,1));

			// Camera 2
			shared_ptr<Camera> camera2 = mScene.CreateCamera();
			camera2->UpdateAspectForViewport(*rightViewport, *GetRenderTarget());

			// Vertical view takes up 50 units at looking at 0,0,0 from 0,0,-1
			camera2->CalculateAndSetDistanceForYFOV(50,Vector3(0,0,0),Vector3(0,0,-1));

			GetRenderer()->CreateRenderer(leftViewport, camera1);
			GetRenderer()->CreateRenderer(rightViewport, camera2);

			shared_ptr<Tests::SpinningCube> spinningCube(new Tests::SpinningCube());
			spinningCube->GetMesh()->SetMaterial(GetMaterialManager()->GetResource("Echo"));

			mScene.AddRenderable(spinningCube);
			mScene.AddTask(*spinningCube);
			AddTask(mScene);

			shared_ptr<InputDevice> keyboard = GetInputManager()->GetDevice("Keyboard");
			if(keyboard)
			{
				keyboard->AddChangeCallback<bool>("Delete", bind(&ResourceReloadTestApplication::ForceUnload,this,placeholders::_1));
				keyboard->AddChangeCallback<bool>("d", bind(&ResourceReloadTestApplication::ForceUnload,this,placeholders::_1));
				keyboard->AddChangeCallback<bool>("Space", bind(&ResourceReloadTestApplication::ForceReload,this,placeholders::_1));
			}else
			{
				return false;
			}
			
			AddTask(GetRenderer().get());
			return true;
		}

		void ForceUnload(bool state)
		{
			if(state)
			{
				ECHO_LOG_INFO("Attempting to unload textures");
				GetTextureManager()->UnloadResources();
			}
		}

		void ForceReload(bool state)
		{
			if(state)
			{
				GetTextureManager()->ReloadResources();
			}
		}
		Scene mScene;
	};

	TEST_CASE("ResourceReload")
	{
		// Turn off log output, we will just use output from this test.
		gDefaultLogger.SetLogMask(Echo::Logger::LogLevels::INFO);

		shared_ptr<ResourceReloadTestApplication> application(new ResourceReloadTestApplication());
		REQUIRE(application->Initialise());
		application->Execute();
	}
}
