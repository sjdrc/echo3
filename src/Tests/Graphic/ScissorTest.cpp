#include <echo/Application.h>
#include <echo/Graphics/Scene.h>
#include <echo/Resource/MaterialManager.h>
#include <echo/Graphics/MultiRenderer.h>
#include <echo/Graphics/Viewport.h>
#include <echo/Graphics/Camera.h>
#include <echo/Graphics/Renderer.h>
#include "SpinningCube.h"

#include <doctest/doctest.h>

using namespace Echo;

namespace ScissorTest
{
	class ScissorTestApplication : public Application
	{
	public:
		bool Initialise()
		{
			//Initialise the application from a config file.
			if (!Application::Initialise("ScissorTest", "data/ScissorTest.config"))
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
			shared_ptr<Renderer> rightRenderer = GetRenderer()->CreateRenderer(rightViewport, camera2);

			shared_ptr<Tests::SpinningCube> spinningCube(new Tests::SpinningCube(0.1f));
			spinningCube->GetMesh()->SetMaterial(GetMaterialManager()->GetResource("MultiStageEcho"));

			mScene.AddRenderable(spinningCube);
			mScene.AddTask(*spinningCube);
			AddTask(mScene);
			
			GetRenderer()->SetScissorBox(Viewport::Rectangle(0.25f,0.25f,0.75f,0.75f));
			GetRenderer()->SetScissor(true);

			AddTask(GetRenderer().get());
			return true;
		}
	
		Scene mScene;
	};

	TEST_CASE("Scissor")
	{
		// Turn off log output, we will just use output from this test.
		gDefaultLogger.SetLogMask(Echo::Logger::LogLevels::NONE);
		shared_ptr<ScissorTestApplication> application(new ScissorTestApplication());
		REQUIRE(application->Initialise());
		application->Execute();
	}
}