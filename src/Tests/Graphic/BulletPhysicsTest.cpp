#include <echo/Application.h>
#include <echo/Graphics/Scene.h>

#include <echo/Graphics/MultiRenderer.h>
#include <echo/Graphics/Viewport.h>
#include <echo/Graphics/Scene.h>
#include <echo/Graphics/SceneEntity.h>
#include <echo/Graphics/Camera.h>
#include <echo/Physics/BulletPhysicsWorld.h>
#include <echo/Physics/BulletRigidBody.h>
#include <echo/Physics/BulletFactory.h>

#include <echo/Graphics/Mesh.h>
#include <echo/Resource/MaterialManager.h>
#include <echo/Chrono/CountDownTimer.h>
#include <echo/cpp/functional>

#include "SimpleControls.h"
#include <echo/Graphics/OrbitCameraController.h>

#include <doctest/doctest.h>
using namespace Echo;

namespace BulletPhysicsTest
{
	class BulletPhysicsTestApplication : public Application
	{
	public:
		bool Initialise();
		void AddCube(shared_ptr<BulletPhysicsWorld> world);
		Scene mScene;
	};
	
	void BulletPhysicsTestApplication::AddCube(shared_ptr<BulletPhysicsWorld> world)
	{
		BulletFactory factory(world);	//Using a specific world.

		//Create an entity
		shared_ptr<SceneEntity> cube(new SceneEntity());
		cube->SetMesh(make_shared<Mesh>());
		cube->GetMesh()->CreateCubeSubMesh(Vector3(1,2,3));
		cube->GetMesh()->SetMaterial(GetMaterialManager()->GetResource("Echo"));
		mScene.AddRenderable(cube);
		
		//Create a box body for the entity.
		shared_ptr<PhysicsBody> body = factory.CreateRigidBody(1,cube,factory.CreateBox(cube->GetAxisAlignedBox(false).GetSize()),true);
		body->SetPosition(Vector3(0,10,0));
		body->SetOrientation(Quaternion(Degree(45).ValueRadians(),Degree(44).ValueRadians(),Degree(46).ValueRadians()));
	}

	bool BulletPhysicsTestApplication::Initialise()
	{
		//Initialise the application from a config file.
		if (!Application::Initialise("BulletPhysicsTestApplication", "data/BulletPhysicsTest.config"))
		{
			return false;
		}

		shared_ptr<Viewport> viewport(new Viewport(0,0,1,1));
		shared_ptr<Camera> camera = mScene.CreateCamera();
		camera->UpdateAspectForViewport(*viewport, *GetRenderTarget());
		camera->CalculateAndSetDistanceForXFOV(45,Vector3(0,0,0),Vector3(0,0,1));
		shared_ptr<Renderer> renderer = GetRenderer()->CreateRenderer(viewport, camera);
		AddTask(mScene);


		//Create an entity
		shared_ptr<SceneEntity> target(new SceneEntity());
		target->SetMesh(make_shared<Mesh>());
		target->GetMesh()->CreateUVSphereSubMesh(0.1,10,10);
		target->GetMesh()->SetMaterial(GetMaterialManager()->GetResource("Echo"));
		mScene.AddRenderable(target,false);	// We don't want this pickable

		shared_ptr<OrbitCameraController> controller = make_shared<OrbitCameraController>(camera,renderer,target,GetInputManager());
		controller->SetFallbackTargetPlane(Plane(Vector3(0,1,0),0));

		//AddTask(make_shared<Testing::SimpleControls>(camera,*this));
		AddTask(controller);
		
		
		{
			BulletFactory factory;
			mScene.AddTask(factory.GetWorld());		//This stops the world from being destroyed after we're done with the factory.
			shared_ptr<CountDownTimer> createCubeTimer(new CountDownTimer(Seconds(0.5),bind(&BulletPhysicsTestApplication::AddCube,this,factory.GetWorld()),"PhysicsStuffApplication"));
			createCubeTimer->SetRepeat(true);
			mScene.AddTask(createCubeTimer);
		
			//Create a box for the floor.
			shared_ptr<SceneEntity> bigBox(new SceneEntity());
			bigBox->SetMesh(make_shared<Mesh>());
			bigBox->GetMesh()->CreateCubeSubMesh(Vector3(20,20,20));
			bigBox->SetMaterial(GetMaterialManager()->GetResource("Echo"));
			mScene.AddRenderable(bigBox);
			
			factory.CreateRigidBody(0,bigBox,factory.CreateBox(bigBox->GetAxisAlignedBox(false).GetSize()),true)->SetPosition(Vector3(0,-15,0));
		
			AddCube(factory.GetWorld());

			mScene.AddRenderable(factory.CreateDebugSceneEntity(),false);
		}

		AddTask(GetRenderer());
		return true;
	}

	TEST_CASE("BulletPhysics")
	{
		// Turn off log output, we will just use output from this test.
		//gDefaultLogger.SetLogMask(Echo::Logger::LogLevels::NONE);
		BulletPhysicsTestApplication bulletPhysicsTestApplication;
		REQUIRE(bulletPhysicsTestApplication.Initialise());
		bulletPhysicsTestApplication.Execute();
	}
}