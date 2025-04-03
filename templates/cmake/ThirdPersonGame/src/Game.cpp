#include <Game.h>
#include <echo/Graphics/MultiRenderer.h>
#include <echo/Graphics/Viewport.h>
#include <echo/Graphics/SceneEntity.h>
#include <echo/Graphics/Camera.h>
#include <echo/Graphics/Mesh.h>
#include <echo/Graphics/Light.h>
#include <echo/Resource/MaterialManager.h>
#include <echo/Resource/MeshManager.h>
#include <echo/Util/Oscillator.h>
#include <echo/GUI/Menu.h>
#include <echo/Graphics/Terrain.h>
#include <echo/Animation/SkeletonAnimationState.h>
#include <echo/Physics/BulletFactory.h>
#include <echo/Chrono/CountDownTimer.h>
#include <echo/Input/Input.h>

#include <ThirdPersonCameraController.h>
#include <GameEntity.h>
#include <GameEntityState.h>
#include <boost/bind.hpp>
#include <echo/Input/InputDevice.h>
#include <echo/Input/MappedInputDevice.h>

namespace MyProject
{
	void AddCube(shared_ptr<BulletPhysicsWorld> world, Scene& scene, shared_ptr<Material> material)
	{
		BulletFactory factory(world);	//Using a specific world.

		//Create an entity
		shared_ptr<SceneEntity> cube(new SceneEntity());
		cube->SetMesh(make_shared<Mesh>());
		cube->GetMesh()->CreateCubeSubMesh(Vector3(2,2,2),1,1,1);
		cube->GetMesh()->SetMaterial(material);
		scene.AddRenderable(cube);
		
		//Create a box body for the entity.
		shared_ptr<PhysicsBody> body = factory.CreateRigidBody(1,cube,factory.CreateBox(cube->GetAxisAlignedBox(false).GetSize()),true);
		body->SetPosition(Vector3(0,10,0));
		body->SetOrientation(Quaternion(Degree(45).ValueRadians(),Degree(44).ValueRadians(),Degree(46).ValueRadians()));
	}
	
	void OnCollision(PhysicsBody& a, PhysicsBody& b)
	{
	}
	
	void MovePlayer(shared_ptr<AnimationState> runAnimation, shared_ptr<PhysicsBody> body, shared_ptr<Camera> camera, Vector2 direction)
	{
		if(direction.x==0 && direction.y==0)
		{
			return;
		}
		Quaternion camori=camera->GetOrientation();
		Vector3 left=camori.XAxis();
		Vector3 forward=camori.ZAxis();
		forward.y=0;			//Project to XZ
		left.y=0;				//Project to XZ
		left.Normalise();
		forward.Normalise();

		left*=direction.x;
		forward*=-direction.y;
		left+=forward;
		body->ApplyCentralImpulse(left);
		Quaternion newFacing;
		newFacing.SetAlongVector(left, Vector3::NEGATIVE_UNIT_Y);
		body->SetOrientation(newFacing);
	}
	
	void UpdateAnimationSpeed(shared_ptr<AnimationState> runAnimation, Vector3 velocity)
	{
		const f32 MAX_RUN_SPEED=5;	//Metres per second
		f32 animationSpeed = velocity.Length() / MAX_RUN_SPEED;
		runAnimation->SetTimeMultiplier(animationSpeed);
	}
	
	Game::Game(shared_ptr<Application> application, shared_ptr<FunctionBinder> appFunctions, u32 priority) :
		TaskGroup("Game", priority),
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

		AddTask(mScene);
		
		shared_ptr<Viewport> viewport(new Viewport(0,0,1,1));
		shared_ptr<Camera> camera = mScene.CreateCamera();
	
		mApplication->GetRenderer()->CreateRenderer(viewport, camera, "Game.Renderer", 1000);
		BulletFactory factory;
		mScene.AddTask(factory.GetWorld());		//This stops the world from being destroyed after we're done with the factory.
		//mScene.AddRenderable(factory.CreateDebugSceneEntity());
		
		shared_ptr< Terrain > terrain(new Terrain("ATerrain",*mApplication->GetMeshManager(),*mApplication->GetTextureManager()));
		terrain->SetTexture("resources/Terrain/Terrain.png");
		terrain->SetResolution(Vector3(10,10,0.1));
		terrain->UpdateMesh();
		terrain->GetMesh()->SetMaterial(mApplication->GetMaterialManager()->GetResource("LitEcho"));
		mScene.AddRenderable(terrain);
		factory.CreateRigidBody(0,terrain,factory.CreateMesh(terrain->GetMesh(),false))->SetName("Ground");
		
		shared_ptr<GameEntity> entity(new GameEntity("Player"));
		shared_ptr<Mesh> mesh = mApplication->GetMeshManager()->GetResource("Kenji");
		if(mesh)
		{
			mesh->SetUseSkeleton(true);
			shared_ptr<Skeleton> skeleton = mesh->GetSkeleton();
			//We create an animation state from an animation. The animation Idle is loaded from the skeleton.
			entity->CreateState("Electric",skeleton);
			entity->CreateState("Fall",skeleton);
			entity->CreateState("FallStart",skeleton);
			entity->CreateState("FromBall",skeleton);
			entity->CreateState("Grapple",skeleton);
			entity->CreateState("GrappleMiss",skeleton);
			entity->CreateState("GrappleStart",skeleton);
			entity->CreateState("GrappleThrow",skeleton);
			entity->CreateState("Hit",skeleton);
			entity->CreateState("Idle",skeleton);
			entity->CreateState("ItemIdle",skeleton);
			entity->CreateState("ItemPickup",skeleton);
			entity->CreateState("ItemRun",skeleton);
			entity->CreateState("ItemThrow",skeleton);
			entity->CreateState("Jump",skeleton);
			entity->CreateState("Land",skeleton);
			entity->CreateState("Push",skeleton);
			
			shared_ptr<AnimationState> runAnimation=skeleton->CreateAnimationState("Run",false);
			entity->CreateState("Run",runAnimation);
			
			entity->CreateState("Select",skeleton);
			entity->CreateState("Skid",skeleton);
			entity->CreateState("Special",skeleton);
			entity->CreateState("StandPush",skeleton);
			entity->CreateState("ToBall",skeleton);
			
			entity->SetMesh(mesh);
			entity->SetPosition(0,2,0);
			mScene.AddRenderable(entity);
			mScene.AddTask(entity);
			entity->ActivateState("Run");

			shared_ptr<PhysicsBody> body=factory.CreateRigidBody(10,entity,factory.CreateCapsule(0.5,0.7));
			body->SetPosition(Vector3(0,10,0));
			body->SetName("Kenji");
			body->SetDamping(0.75,1);
			body->SetMassProperties(5, Vector3(0,0,0));
			body->RegisterObjectCollideCallbackT(*entity,"Ground",boost::bind(&OnCollision,_1,_2));
			body->SetDeactivationEnabled(false);

			shared_ptr<MappedInputDevice> playerInput=dynamic_pointer_cast<MappedInputDevice>(mApplication->GetInputManager()->GetDevice("Controller"));
			if(playerInput)
			{
				playerInput->AddFunctorInput<Vector3, PhysicsBody>("Velocity",body,boost::bind(&PhysicsBody::GetLinearVelocity,body),Vector3::ZERO);
				std::vector<std::string> inputSources;
				inputSources.push_back("Controller:X");
				inputSources.push_back("Controller:Y");
				playerInput->CreateMappableInput<Vector2>("Stick", inputSources, Vector2::ZERO);
				playerInput->AddActiveCallback<Vector2>("Stick",boost::bind(&MovePlayer,runAnimation,body,camera,_1));
				playerInput->AddActiveCallback<Vector3>("Velocity",boost::bind(&UpdateAnimationSpeed,runAnimation,_1));
			}
		}
		mScene.AddTask(make_shared<ThirdPersonCameraController>(camera, entity, mesh->GetAxisAlignedBox().GetSize().x*4));
		
		shared_ptr<CountDownTimer> createCubeTimer(new CountDownTimer(Seconds(5),boost::bind(&AddCube,factory.GetWorld(),boost::ref(mScene),mApplication->GetMaterialManager()->GetResource("LitEcho")),"PhysicsStuffApplication"));
		createCubeTimer->SetRepeat(true);
		mScene.AddTask(createCubeTimer);		

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
												 mApplication->GetRenderer(),
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
