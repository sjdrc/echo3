#include <stdlib.h>
#include <iostream>
#include <echo/Graphics/Renderer.h>
#include <echo/Graphics/Scene.h>
#include <echo/Graphics/SceneEntity.h>
#include <echo/Graphics/Mesh.h>
#include <echo/Graphics/Camera.h>
#include <echo/Graphics/Viewport.h>
#include <echo/Graphics/Font.h>
#include <echo/Graphics/MultiRenderer.h>
#include <echo/Kernel/Kernel.h>
#include <echo/Resource/MeshManager.h>
#include <echo/Application.h>
#include <echo/FileSystem/FileSystem.h>
#include <echo/FileSystem/FileSystemSourceFile.h>
#include <echo/Resource/MaterialManager.h>
#include <echo/Graphics/RenderPass.h>
#include <echo/Chrono/CountDownTimer.h>
#include <echo/cpp/functional>

#include <doctest/doctest.h>

using namespace Echo;

class AlphaLoop : public Task
{
public:
	AlphaLoop(RenderPass& pass, f32 speedMultiplier) :
		mPass(pass),
		mAngle(0),
		mSpeedMultiplier(speedMultiplier)
	{
		
	}
	
	void Update(Seconds lastFrameTime)
	{
		mAngle += static_cast<f32>(lastFrameTime.count()) * mSpeedMultiplier;
		Colour colour = mPass.GetDiffuse();
		colour.mAlpha=Maths::Abs(Maths::Cos(mAngle,false));
		mPass.SetDiffuse(colour);
	}
	RenderPass& mPass;
	f32 mAngle;
	f32 mSpeedMultiplier;
};

class AlphaRejectTest : public Application
{
public:
	AlphaRejectTest() : mTimeout(Seconds(30.0),bind(&TaskManager::RemoveAllTasks,this),"RemoveAllTasks")
	{
		Initialise("AlphaRejectTest","data/AlphaRejectTest.config");
		
		//Load resource lists
		GetMaterialManager()->LoadList(GetFileSystem()->Open("data/materials.list"));
		
		shared_ptr<Camera> camera = mScene.CreateCamera();
		camera->SetPosition(0,0,2);
		camera->LookAt(0,0,0);
		GetRenderer()->CreateRenderer(make_shared<Viewport>(0,0,1,1),camera);
		AddTask(GetRenderer().get());
		AddTask(mTimeout);

		//Load a cube and clone it so we can have two cubes and use two different materials. 
		shared_ptr<Mesh> quad = GetMeshManager()->CreateMesh("Quad");
		quad->CreateQuadSubMesh(1,1);
		shared_ptr<Material> glow=GetMaterialManager()->GetResource("EchoLogoWithAlphaRejectTest");
		if(glow)
		{
			ECHO_LOG_INFO("Got resource EchoLogoWithAlphaRejectTest");
			if(glow->GetNumberOfPasses()>=2)
			{
				ECHO_LOG_INFO("Creating alpha loop on EchoLogoWithAlphaRejectTest passes:" << glow->GetNumberOfPasses());
				mAlphaLoop.reset(new AlphaLoop(*glow->GetPass(1),1.f));
				AddTask(*mAlphaLoop);
			}
			quad->SetMaterial(glow);
		}else
		{
			ECHO_LOG_ERROR("Could not acquire resource EchoLogoWithAlphaRejectTest so I can't animate it");
		}

		//Create a couple of Scene entities and set them up to use the cube meshes.
		mEntity = shared_ptr< SceneEntity >(new SceneEntity());
		mEntity->SetMesh(quad);
		mScene.AddRenderable(mEntity);
	}
private:
	Scene mScene;
	shared_ptr< SceneEntity > mEntity;
	shared_ptr< SceneEntity > mEntity2;
	shared_ptr< AlphaLoop > mAlphaLoop;
	CountDownTimer mTimeout;
};

TEST_CASE("AlphaReject")
{
	// Turn off log output, we will just use output from this test.
	//gDefaultLogger.SetLogMask(Echo::Logger::LogLevels::NONE);
	gDefaultLogger.RegisterCustomLogLevel("Material");
	shared_ptr<Application> application(new AlphaRejectTest());
	application->Execute();
}
