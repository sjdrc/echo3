#include <echo/Graphics/Renderer.h>
#include <echo/Graphics/Scene.h>
#include <echo/Graphics/SceneRenderable.h>
#include <echo/Graphics/SceneEntity.h>
#include <echo/Graphics/Camera.h>
#include <echo/Graphics/Viewport.h>
#include <echo/Graphics/MultiRenderer.h>
#include <echo/Graphics/Mesh.h>
#include <echo/Graphics/Font.h>
#include <echo/Graphics/Sprite.h>
#include <echo/Graphics/MaterialAnimation.h>
#include <echo/Resource/TextureManager.h>
#include <echo/Resource/MaterialManager.h>
#include <echo/FileSystem/FileSystem.h>
#include <echo/Chrono/CountDownTimer.h>
#include <echo/Kernel/Kernel.h>
#include <echo/Application.h>

#include <doctest/doctest.h>

using namespace Echo;

class MaterialAnimationTest : public Application
{
public:
	MaterialAnimationTest() : mTimeout(Seconds(30.0),bind(&TaskManager::RemoveAllTasks,this),"RemoveAllTasks")
	{
		GetConfiguration().Set<u32>("resolution.x",800);
		GetConfiguration().Set<u32>("resolution.y",600);
		Initialise("MaterialAnimationTest");
		
		GetMaterialManager()->LoadList(GetFileSystem()->Open("data/materials.list"));
		shared_ptr<Texture> echoTexture = GetMaterialManager()->GetResource("Echo")->GetTexture();
		
		shared_ptr<Camera> camera = mScene.CreateCamera();
		camera->SetPosition(0,0,2);
		GetRenderer()->CreateRenderer(make_shared<Viewport>(0,0,1,1),camera);
		AddTask(GetRenderer().get());
		AddTask(mTimeout);
		
		// Set up MaterialAnimation frames:
		std::vector<MaterialAnimation::Frame> frames;
		frames = MaterialAnimation::FilmStrip(Seconds(0.4), echoTexture, TextureUV(0.19f, 0.22f), false, 4, TextureUV(0.215f, 0.635f));
		frames.push_back(MaterialAnimation::Frame(Seconds(0.8), echoTexture));
		
		// Set up MaterialAnimation:
		shared_ptr<MaterialAnimation> material = make_shared<MaterialAnimation>("AnimatorTask", frames);
		AddTask(material.get());
		
		// Add MaterialAnimation to a sprite:
		mEntity = make_shared<Sprite>(material);
		mScene.AddRenderable(mEntity);
	}
	
private:
	Scene mScene;
	shared_ptr<SceneRenderable> mEntity;
	CountDownTimer mTimeout;
};

TEST_CASE("MaterialAnimation")
{
	// Turn off log output, we will just use output from this test.
	gDefaultLogger.SetLogMask(Echo::Logger::LogLevels::NONE);
	shared_ptr<Application> application(new MaterialAnimationTest());
	application->Execute();
}
