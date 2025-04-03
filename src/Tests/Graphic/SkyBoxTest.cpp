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
#include <echo/Graphics/SkyBox.h>
#include <echo/cpp/functional>

#include <doctest/doctest.h>

using namespace Echo;

class SkyBoxTest : public Application
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

	static void SetNodePosition(shared_ptr<Node> node, f32 x, f32 y, f32 z)
	{
		node->SetPosition(Vector3(x,y,z));
	}

	SkyBoxTest() : mTimeout(Seconds(30.0),bind(&TaskManager::RemoveAllTasks,this),"RemoveAllTasks")
	{
		GetConfiguration().Set<u32>("resolution.x",800);
		GetConfiguration().Set<u32>("resolution.y",600);
		Initialise("RenderTest");

		GetMaterialManager()->LoadList(GetFileSystem()->Open("data/materials.list"));
		
		//The first camera will be stationary and tracking a node at the same position as the first entity.
		shared_ptr<Camera> camera = mScene.CreateCamera();
		
		shared_ptr<Node> node = make_shared<Node>();
		camera->SetAutoTracking(true,node,Vector3(0,0,0));
		camera->SetPosition(Vector3(0,0,10));
		mLookAtSpinner = shared_ptr<Spinner>(new Spinner(bind(&SetNodePosition,node,0,placeholders::_1,0),500.f,1.0f,0.f));
		AddTask(*mLookAtSpinner);
		
		//This spinner moves the camera.
		mSpinner = shared_ptr<Spinner>(new Spinner(bind(static_cast<void(Camera::*)(f32,f32,f32)>(&Camera::SetPosition),camera,placeholders::_1,0,placeholders::_2),500.f,0.25f,0.f));
		AddTask(*mSpinner);

		GetRenderer()->CreateRenderer(make_shared<Viewport>(0,0,1,1),camera);

		//Add the renderer to the kernel otherwise it won't update which means it wouldn't render.
		AddTask(GetRenderer().get());

		//The demo can be closed using the window or it will timeout automatically.
		AddTask(mTimeout);

		mSkyBox = shared_ptr< SkyBox >(new SkyBox("ASkyBox",*GetMeshManager(),*GetTextureManager(),*GetMaterialManager()));
		mSkyBox->SetTexture("data/SkyBox.png");
//		mSkyBox->SetTextureLeft("data/SkyBox/Left.png");
//		mSkyBox->SetTextureRight("data/SkyBox/Right.png");
//		mSkyBox->SetTextureBack("data/SkyBox/Back.png");
//		mSkyBox->SetTextureFront("data/SkyBox/Front.png");
//		mSkyBox->SetTextureTop("data/SkyBox/Top.png");
//		mSkyBox->SetTextureBottom("data/SkyBox/Bottom.png");
		mSkyBox->UpdateMesh();
		mScene.SetSkyBox(mSkyBox);
		//mScene.AddRenderable(mSkyBox);
	}
private:
	Scene mScene;
	shared_ptr< SkyBox > mSkyBox;
	shared_ptr<Spinner> mSpinner;
	shared_ptr<Spinner> mLookAtSpinner;
	CountDownTimer mTimeout;
};

TEST_CASE("SkyBox")
{
	// Turn off log output, we will just use output from this test.
	gDefaultLogger.SetLogMask(Echo::Logger::LogLevels::NONE);
	SkyBoxTest skyboxTest;
	skyboxTest.Execute();
}
