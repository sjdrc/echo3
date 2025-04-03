/* 
 * File:   RenderTest.cpp
 * Author: sean
 *
 * Created on 17-Aug-2014, 10:55:49
 */

#include <stdlib.h>
#include <iostream>
#include <echo/Graphics/Renderer.h>
#include <echo/Graphics/Scene.h>
#include <echo/Graphics/Light.h>
#include <echo/Graphics/SceneRenderable.h>
#include <echo/Graphics/Camera.h>
#include <echo/Graphics/Viewport.h>
#include <echo/Graphics/Mesh.h>
#include <echo/Graphics/MultiRenderer.h>
#include <echo/Resource/MaterialManager.h>
#include <echo/Kernel/Kernel.h>
#include <echo/Application.h>
#include <echo/FileSystem/FileSystem.h>
#include <echo/Chrono/CountDownTimer.h>
#include <echo/Graphics/Terrain.h>
#include <echo/cpp/functional>

#include <doctest/doctest.h>

using namespace Echo;

class TerrainTest : public Application
{
public:
	class Spinner : public Task
	{
	public:
		typedef function<void(f32,f32)> SetFunction;

		Spinner(SetFunction setFunction, f32 magnitudeMultiplier, f32 speedMultiplier, f32 startAngle) :
			mSetFunction(setFunction),
			mAngle(startAngle),
			mSpeedMultiplier(speedMultiplier),
			mMagnitudeMultiplier(magnitudeMultiplier)
		{
			
		}
		
		void Update(Seconds lastFrameTime)
		{
			mAngle += static_cast<f32>(lastFrameTime.count()) * mSpeedMultiplier;
			mSetFunction(Maths::Sin(mAngle,false) * mMagnitudeMultiplier,Maths::Cos(mAngle,false) * mMagnitudeMultiplier);
		}
		SetFunction mSetFunction;
		f32 mAngle;
		f32 mSpeedMultiplier;
		f32 mMagnitudeMultiplier;
	};

	TerrainTest() : mTimeout(Seconds(30.0),bind(&TaskManager::RemoveAllTasks,this),"RemoveAllTasks")
	{
		GetConfiguration().Set<u32>("resolution.x",800);
		GetConfiguration().Set<u32>("resolution.y",600);
		Initialise("RenderTest");

		GetMaterialManager()->LoadList(GetFileSystem()->Open("data/materials.list"));
		
		//The first camera will be stationary and tracking a node at the same position as the first entity.
		shared_ptr<Camera> camera = mScene.CreateCamera();
		camera->SetPosition(0,0.5,2);
		camera->SetAutoTracking(true,make_shared<Node>(),Vector3(0,0,0));
		
		//This spinner moves the camera.
		mSpinner = shared_ptr<Spinner>(new Spinner(bind(static_cast<void(Camera::*)(f32,f32,f32)>(&Camera::SetPosition),camera,placeholders::_1,64,placeholders::_2),256.f,0.5f,0.f));
		AddTask(*mSpinner);

		
		shared_ptr<Light> light = mScene.CreateLight();
		light->SetPosition(Vector3(0,25,0));
		light->SetAttenuation(150,1.0,0.1,0);

		mSpinner2 = shared_ptr<Spinner>(new Spinner(bind(static_cast<void(Node::*)(f32,f32,f32)>(&Light::SetPosition),light,placeholders::_1,32,placeholders::_2),100.f,-0.5f,0.f));
		AddTask(*mSpinner2);

		GetRenderer()->CreateRenderer(make_shared<Viewport>(0,0,1,1),camera);

		//Add the renderer to the kernel otherwise it won't update which means it wouldn't render.
		AddTask(GetRenderer().get());

		//The demo can be closed using the window or it will timeout automatically.
		AddTask(mTimeout);

		mTerrain = shared_ptr< Terrain >(new Terrain("ATerrain",*GetMeshManager(),*GetTextureManager()));
		mTerrain->SetTexture("data/Terrain.png");
		mTerrain->SetResolution(Vector3(10,10,0.2));
		mTerrain->UpdateMesh();
		mTerrain->GetMesh()->SetMaterial(GetMaterialManager()->GetResource("LitEcho"));
		mScene.AddRenderable(mTerrain);
	}
private:
	Scene mScene;
	shared_ptr< Terrain > mTerrain;
	shared_ptr<Spinner> mSpinner;
	shared_ptr<Spinner> mSpinner2;
	CountDownTimer mTimeout;
};

TEST_CASE("Terrain")
{
	// Turn off log output, we will just use output from this test.
	gDefaultLogger.SetLogMask(Echo::Logger::LogLevels::NONE);
	TerrainTest terrainTest;
	terrainTest.Execute();
}
