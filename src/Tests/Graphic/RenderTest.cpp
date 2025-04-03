#include <echo/Graphics/Renderer.h>
#include <echo/Graphics/Scene.h>
#include <echo/Graphics/SceneRenderable.h>
#include <echo/Graphics/SceneEntity.h>
#include <echo/Graphics/Mesh.h>
#include <echo/Graphics/Camera.h>
#include <echo/Graphics/Light.h>
#include <echo/Graphics/Viewport.h>
#include <echo/Graphics/Font.h>
#include <echo/Graphics/MultiRenderer.h>
#include <echo/Kernel/Kernel.h>
#include <echo/Resource/MeshManager.h>
#include <echo/Application.h>
#include <echo/Graphics/Sprite.h>
#include <echo/Graphics/MaterialAnimation.h>
#include <echo/FileSystem/FileSystem.h>
#include <echo/FileSystem/FileSystemSourceFile.h>
#include <echo/Resource/FontManager.h>
#include <echo/Resource/TextureManager.h>
#include <echo/Resource/MaterialManager.h>
#include <echo/Resource/SkeletonManager.h>
#include <echo/Animation/SkeletonAnimationState.h>
#include <echo/Chrono/CountDownTimer.h>

#include <echo/Resource/PNGLoader.h>
#include <fstream>
#include <doctest/doctest.h>

using namespace Echo;

class RenderTest : public Application
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

	RenderTest() : mTimeout(Seconds(30.0),bind(&TaskManager::RemoveAllTasks,this),"RemoveAllTasks")
	{
		Initialise("RenderTest","data/RenderTest.config");
		
		//Load resource lists
		GetMaterialManager()->LoadList(GetFileSystem()->Open("data/materials.list"));
		GetSkeletonManager()->LoadList(GetFileSystem()->Open("data/skeleton.list"));
		GetMeshManager()->LoadList(GetFileSystem()->Open("data/meshes.list"));
		GetFontManager()->LoadList(GetFileSystem()->Open("data/fonts.list"));
		
		//The first camera will be stationary and tracking a node at the same position as the first entity.
		shared_ptr<Camera> camera = mScene.CreateCamera();
		camera->SetPosition(0,0.5,2);
		camera->SetAutoTracking(true,make_shared<Node>(),Vector3(-1.5,0,0));

		//The second camera will be moving around the Z axis at a distance and tacking another stationary node.
		shared_ptr<Camera> camera2 = mScene.CreateCamera();
		camera2->SetAutoTracking(true,make_shared<Node>(),Vector3(0,1,2));

		//This spinner moves the second camera.
		mSpinner = shared_ptr<Spinner>(new Spinner(bind(static_cast<void(Camera::*)(f32,f32,f32)>(&Camera::SetPosition),camera2,placeholders::_1,placeholders::_2,7),5.f,1.0f,0.f));
		AddTask(mSpinner.get());

		//We'll set up some lights that will be moving around the scene. These lights will light up the lit cube.
		mLight = mScene.CreateLight();
		mLight->SetDiffuseColour(Colours::RED);

		mLight2 = mScene.CreateLight();
		mLight2->SetDiffuseColour(Colours::GREEN);

		mLight3 = mScene.CreateLight();
		mLight3->SetDiffuseColour(Colours::BLUE);

		//Move light 1 and 2, light 3 can be stationary.
		mSpinner2 = shared_ptr<Spinner>(new Spinner(bind(static_cast<void(Node::*)(f32,f32,f32)>(&Light::SetPosition),mLight,placeholders::_1,10,placeholders::_2),10.f,5.0f,0.f));
		AddTask(mSpinner2.get());

		mSpinner3 = shared_ptr<Spinner>(new Spinner(bind(static_cast<void(Node::*)(f32,f32,f32)>(&Light::SetPosition),mLight2,0,placeholders::_1,placeholders::_2),10.f,5.0f,0.f));
		AddTask(mSpinner3.get());

		//Now that we have our cameras set up. Create a Renderer object managed by the MultiRenderer.
		//Renderers are given a viewport and camera to render and usually a render target, the render
		//target for these two Renderers is the one that the MultiRender uses.
		GetRenderer()->CreateRenderer(make_shared<Viewport>(0,0,0.5,1),camera);
		GetRenderer()->CreateRenderer(make_shared<Viewport>(0.5,0,1,1),camera2);

		//Add the renderer to the kernel otherwise it won't update which means it wouldn't render.
		AddTask(GetRenderer().get());

		class TextureWriter : public Task
		{
		public:
			TextureWriter(Application& application) : mApplication(application)
			{
			}
			void Update(Seconds)
			{
				if(mFrame%30==0)
				{
					shared_ptr<Texture> renderedTexture = dynamic_pointer_cast<Texture>(mApplication.GetRenderTarget("Texture"));
					if(renderedTexture)
					{
						std::string fileName = "RenderTest-Frame-" + std::to_string(mFrame) + ".png";
						std::ofstream file;
						file.open(fileName);
						if(!file.is_open() || !PNGLoader::WritePNG(*renderedTexture, file))
						{
							ECHO_LOG_ERROR("Failed to write " << fileName);
						}
						file.close();
					}else
					{
						ECHO_LOG_ERROR("Render target is not a Texture");
					}
				}
				mFrame++;
			}
		private:
			Application& mApplication;
			Size mFrame = 0;
		};

		AddTask(shared_ptr<TextureWriter>(new TextureWriter(*this)));

		//The demo can be closed using the window or it will timeout automatically.
		AddTask(mTimeout);

		//Load a cube and clone it so we can have two cubes and use two different materials. 
		shared_ptr<Mesh> cube = GetMeshManager()->GetResource("Kenji");
		shared_ptr<Mesh> litCube = cube->Clone();
		
		//Create a couple of Scene entities and set them up to use the cube meshes.
		mEntity = shared_ptr< SceneEntity >(new SceneEntity());
		mEntity->SetPosition(Vector3(-1.5,0,0));
		mEntity->SetMesh(cube);
		mEntity->SetRenderAABB(true);

		if(cube->GetSkeleton())
		{
			cube->SetUseSkeleton(true);
			mRunAnimationState = cube->GetSkeleton()->CreateAnimationState("Run");
			if(mRunAnimationState)
			{
				mRunAnimationState->SetTimeMultiplier(0.5f);
				mRunAnimationState->AddWeightInterpolationEndFunction("WeightChanger",bind(&RenderTest::ChangeRunWeightTarget,this,placeholders::_1));
				mRunAnimationState->SetWeightTarget(0.f,Seconds(5.f),false);
			}
			AddTask(cube->GetSkeleton());
			
			litCube->SetSkeleton(cube->GetSkeleton()->Clone());
		}

		mScene.AddRenderable(mEntity);

		mEntity2 = shared_ptr< SceneEntity >(new SceneEntity());
		mEntity2->SetPosition(Vector3(1.5,0,0));
		mEntity2->SetMesh(litCube);
		mEntity2->SetName("mEntity2");

		if(litCube->GetSkeleton())
		{
			litCube->SetUseSkeleton(true);
			litCube->GetSkeleton()->CreateAnimationState("Idle");
			AddTask(litCube->GetSkeleton());
		}

		mScene.AddRenderable(mEntity2);
		
		// This will cause the TestFont to be loaded and create the "TestFont CharacterMap" texture
		// resource in the TextureManager. We'll use TestFont CharacterMap in the following animation.
		GetFontManager()->GetResource("TestFont");

		shared_ptr<SpriteAnimation> spriteAnimation(new SpriteAnimation(*GetTextureManager(),"frameAnimation"));
		spriteAnimation->AddFrame("data/EchoLogo.png", Seconds(0.5));
		spriteAnimation->AddFrame("TestFont CharacterMap", Seconds(0.5));
		spriteAnimation->AddFrame("data/rotator.png", Seconds(0.5));
		spriteAnimation->AddEndFunction(bind(&RenderTest::FlipSprite,this));
		
		// Add MaterialAnimation to a sprite:
		mSprite1 = make_shared<Sprite>();
		mSprite1->SetAnimation(spriteAnimation);
		mSprite1->SetName("sprite");
		mSprite1->SetPosition(Vector3(0,0,0));
		mSprite1->SetSizeInPixels(50,50);
		if(cube->GetSkeleton())
		{
			cube->GetSkeleton()->GetBone("rightHAND")->AddChild(*mSprite1);
		}
		mSprite1->SetAlwaysFaceCamera(true);
		mScene.AddRenderable(mSprite1);
		AddTask(*mSprite1);
		
		spriteAnimation.reset(new SpriteAnimation(*GetTextureManager(),"rotator"));
		for(size_t i=0;i<360;i+=20)
		{
			f32 size = 1+Maths::Abs(Maths::Sin(Maths::DegreesToRadians(i))*200);
			spriteAnimation->AddFrame("data/rotator.png", Seconds(0.05)).orientation(Quaternion(Degree(i),Vector3(0,0,1))).width(size);
		}
		spriteAnimation->SetHeightInPixels(100.f);
		spriteAnimation->AddFrameFunction(5,bind(&RenderTest::FrameCallback,this,placeholders::_3));
		spriteAnimation->AddFrameFunction(10,bind(&RenderTest::FrameCallback,this,placeholders::_3));
		spriteAnimation->AddFrameFunction(15,bind(&RenderTest::FrameCallback,this,placeholders::_3));
		spriteAnimation->AddEndFunction(bind(&RenderTest::EndFrameCallback,this));

		shared_ptr<Sprite> sprite2 = make_shared<Sprite>();
		sprite2->SetAnimation(spriteAnimation);
		sprite2->SetName("Sprite 2");
		sprite2->SetPosition(Vector3(0,1.0,1));
		sprite2->SetPixelToWorldUnitRatio(100.f);
		mScene.AddRenderable(sprite2);
		AddTask(*sprite2);
		PerformSearchTests();
	}
private:
	
	void PerformSearchTests()
	{
		CHECK_MESSAGE(mScene.FindRenderable("mEntity2", false)!=nullptr,"Could not find mEntity2 when attempting search");
		CHECK_MESSAGE(mScene.FindRenderable<Sprite>("sprite", false)!=nullptr,"Could not find sprite when attempting search and cast");
	}
	void FrameCallback(size_t frame)
	{
		std::cout << "Frame " << frame << " callback triggered." << std::endl;
	}
	void EndFrameCallback()
	{
		std::cout << "End frame callback triggered." << std::endl;
	}
	void FlipSprite()
	{
		//At the end of the animation of mSprite1 we'll flip.
		// The order will be:
		//	- Flip horizontally
		//  - Flip vertically
		//	- Flip both at once
		//	- None flipped

		if(!mSprite1->GetFlippedHorizontally() && !mSprite1->GetFlippedVertically())
		{
			mSprite1->SetFlippedHorizontally(true);
			mEntity2->SetVisibilityFlag(false);
		}else
		if(mSprite1->GetFlippedHorizontally() && !mSprite1->GetFlippedVertically())
		{	
			mSprite1->SetFlippedHorizontally(false);
			mSprite1->SetFlippedVertically(true);
			mEntity2->SetVisibilityFlag(true);
		}else
		if(!mSprite1->GetFlippedHorizontally() && mSprite1->GetFlippedVertically())
		{
			mSprite1->SetFlippedHorizontally(true);
			mSprite1->SetFlippedVertically(true);
			mEntity2->SetVisibilityFlag(false);
		}else
		{
			mSprite1->SetFlippedHorizontally(false);
			mSprite1->SetFlippedVertically(false);
			mEntity2->SetVisibilityFlag(true);
		}
	}
	void ChangeRunWeightTarget(AnimationState& animationState)
	{
		if(animationState.GetWeight()<0.5)
		{
			std::cout << "Setting \"Run\" weight to 100% over 5 seconds." << std::endl;
			animationState.SetWeightTarget(1.f,Seconds(5.f),false);
		}else
		{
			std::cout << "Setting \"Run\" weight to 0% over 5 seconds." << std::endl;
			animationState.SetWeightTarget(0.f,Seconds(5.f),false);
		}
	}
	Scene mScene;
	shared_ptr< SceneEntity > mEntity;
	shared_ptr< SceneEntity > mEntity2;
	shared_ptr<Light> mLight;
	shared_ptr<Light> mLight2;
	shared_ptr<Light> mLight3;
	shared_ptr<Spinner> mSpinner;
	shared_ptr<Spinner> mSpinner2;
	shared_ptr<Spinner> mSpinner3;
	shared_ptr<Sprite> mSprite1;
	shared_ptr<AnimationState> mRunAnimationState;
	CountDownTimer mTimeout;
};

TEST_CASE("RenderTest")
{
	// Turn off log output, we will just use output from this test.
	//gDefaultLogger.SetLogMask(Echo::Logger::LogLevels::NONE);
	shared_ptr<Application> application = shared_ptr<Application>(new RenderTest());
	application->Execute();
}
