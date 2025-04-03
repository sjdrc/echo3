#include <echo/Platforms/GL/GLSupport.h>
#ifndef ECHO_PLATFORM_WINDOWS
#ifdef ECHO_RENDERER_GL
#include <echo/Graphics/Renderer.h>
#include <echo/Graphics/Scene.h>
#include <echo/Graphics/SceneEntity.h>
#include <echo/Graphics/Mesh.h>
#include <echo/Graphics/Camera.h>
#include <echo/Graphics/Viewport.h>
#include <echo/Graphics/MultiRenderer.h>
#include <echo/Resource/MeshManager.h>
#include <echo/Application.h>
#include <echo/Resource/MaterialManager.h>
#include <echo/Resource/TextureManager.h>
#include <echo/cpp/functional>
#include <echo/Platforms/GL/GLRenderTexture.h>
#include <echo/Graphics/OrbitCameraController.h>

#include "SpinningCube.h"

#include <doctest/doctest.h>
#undef INFO

using namespace Echo;


/**
 * Test that loads a colour image and single channel "depth" image and generates a point cloud.
 * Two TrackPad instances are used for the left and right mouse in combination with dragging for camera controls.
 */
class RenderToTexture : public Application
{
public:
	RenderToTexture()
	{
		Initialise("RenderToTexture","data/RenderToTextureTest.config");

		SetupTextureRenderTarget();
		SetupMainView();
		SetupScene();
	}

	void SetupTextureRenderTarget()
	{
		shared_ptr<Camera> textureCamera = mScene.CreateCamera("Texture");
		textureCamera->SetPosition(-10,0,8);
		textureCamera->LookAt(-10,0,0);
		textureCamera->SetNearPlane(0.1);
		textureCamera->SetFarPlane(10);

		// NOTE: This configuration isn't optimal. Using a separate GLContext from the main renderer means we will be copying the rendered 
		shared_ptr<GLRenderTexture> textureTarget(new GLRenderTexture(make_shared<GLContext>(),"TextureTarget",512,512,Texture::Formats::R8G8B8,Texture::Formats::DEPTH16));

		// Lets add these textures to the Application TextureManager so we can reference them
		GetTextureManager()->AddResource("TextureTarget.Colour",textureTarget);
		GetTextureManager()->AddResource("TextureTarget.Depth",textureTarget->GetDepthTexture());

		// We technically don't need to add a multiRenderer to render to the RenderTarget but the MultiRenderer sets the clear settings each update which
		// works around an issue in the GLRenderTarget which doesn't set the clear colour on activate.
		shared_ptr<MultiRenderer> multiRenderer = make_shared<MultiRenderer>("TextureMultiRenderer");
		multiRenderer->CreateRenderer(make_shared<Viewport>(0,0,1,1), textureCamera, "TextureTargetRenderer");
		multiRenderer->SetClearColour(Colours::AMBER);
		multiRenderer->AddRenderTarget(textureTarget);
		AddTask(multiRenderer);
	}

	void SetupMainView()
	{
		// Now lets create a view with a window that we'll work
		shared_ptr<Camera> camera = mScene.CreateCamera("Main");
		camera->SetPosition(2.5,5,6);
		camera->LookAt(2.5,0,0);
		camera->SetNearPlane(0.1);
		camera->SetFarPlane(10);
		shared_ptr<Renderer> renderer = GetRenderer()->CreateRenderer(make_shared<Viewport>(0,0,1,1),camera);

		// This renderer will have the primary target which is set up from the config file
		AddTask(GetRenderer().get());

		shared_ptr<OrbitCameraController> controller(new OrbitCameraController(camera,renderer,nullptr,GetInputManager()));
		controller->SetFallbackTargetPlane(Plane(Vector3(0,1,0),-5));
		AddTask(controller);

	}

	void SetupScene()
	{
		// Add a spinning cube with the Echo texture on it for the render texture to look at
		{
			shared_ptr<Tests::SpinningCube> spinningCube(new Tests::SpinningCube(0.1f));
			spinningCube->GetMesh()->SetMaterial(GetMaterialManager()->GetResource("Echo"));
			spinningCube->SetPosition(Vector3(-10,0,0));
			mScene.AddRenderable(spinningCube);
			mScene.AddTask(*spinningCube);
		}
		
		// Create a cube mesh with the colour texture
		{
			shared_ptr<Mesh> mesh(new Mesh());
			mesh->CreateCubeSubMesh(Vector3(2,2,2));
			shared_ptr<Material> material = make_shared<Material>();
			material->SetToDefaultMaterial();
			material->SetTexture(GetTextureManager()->GetResource("TextureTarget.Colour"));
			mesh->SetMaterial(material);

			// Create a SceneEntity and give it the cube mesh. 
			shared_ptr<SceneEntity> entity(new SceneEntity());
			entity->SetMesh(mesh);
			entity->SetPosition(Vector3(0,0,0));
			
			//Add the entity to be rendered to the scene.
			mScene.AddRenderable(entity);
		}

		// Create a cube mesh with the colour texture
		{
			shared_ptr<Mesh> mesh(new Mesh());
			mesh->CreateCubeSubMesh(Vector3(2,2,2));
			shared_ptr<Material> material = make_shared<Material>();
			material->SetToDefaultMaterial();
			material->SetTexture(GetTextureManager()->GetResource("TextureTarget.Depth"));
			mesh->SetMaterial(material);

			// Create a SceneEntity and give it the cube mesh. 
			shared_ptr<SceneEntity> entity(new SceneEntity());
			entity->SetMesh(mesh);
			entity->SetPosition(Vector3(5,0,0));
			
			//Add the entity to be rendered to the scene.
			mScene.AddRenderable(entity);
		}

		AddTask(mScene);
	}
private:
	Scene mScene;
};

TEST_CASE("RenderToTexture")
{
	// Turn off log output, we will just use output from this test.
	//gDefaultLogger.SetLogMask(Echo::Logger::LogLevels::NONE);
	RenderToTexture renderToTexture;
	renderToTexture.Execute();
}
#endif
#endif
