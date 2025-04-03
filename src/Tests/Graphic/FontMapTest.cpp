#include <echo/Graphics/Renderer.h>
#include <echo/Graphics/Scene.h>
#include <echo/Graphics/SceneRenderable.h>
#include <echo/Graphics/SceneEntity.h>
#include <echo/Graphics/Camera.h>
#include <echo/Graphics/Viewport.h>
#include <echo/Graphics/MultiRenderer.h>
#include <echo/Kernel/Kernel.h>
#include <echo/Resource/MeshManager.h>
#include <echo/FileSystem/FileSystem.h>
#include <echo/Application.h>
#include <echo/Graphics/Mesh.h>
#include <echo/Resource/TextureManager.h>
#include <echo/Resource/FontManager.h>
#include <echo/Graphics/Font.h>
#include <echo/Resource/MaterialManager.h>
#include <echo/Chrono/CountDownTimer.h>
#include <echo/cpp/functional>

#include <doctest/doctest.h>

using namespace Echo;

class FontMapTest : public Application
{
public:
	FontMapTest() : mTimeout(Seconds(30.0),bind(&TaskManager::RemoveAllTasks,this),"RemoveAllTasks")
	{
		GetConfiguration().Set<u32>("resolution.x",800);
		GetConfiguration().Set<u32>("resolution.y",600);
		Initialise("FontMapTest");
		
		GetMaterialManager()->LoadList(GetFileSystem()->Open("data/materials.list"));
		GetMeshManager()->LoadList(GetFileSystem()->Open("data/meshes.list"));
		GetFontManager()->LoadList(GetFileSystem()->Open("data/fonts.list"));
		
		shared_ptr<Camera> camera = mScene.CreateCamera();
		camera->SetPosition(0,0,2);
		GetRenderer()->CreateRenderer(make_shared<Viewport>(0,0,1,1),camera);
		AddTask(GetRenderer().get());
		AddTask(mTimeout);

		//Load a font.
		mFont = GetFontManager()->GetResource("TestFont");
		mFont->GetMaterial()->GetPass(0)->SetCullMode(RenderPass::CullModes::FRONT);

		shared_ptr<Mesh> cube = GetMeshManager()->GetResource("UnitCube");

		mEntity = shared_ptr< SceneEntity >(new SceneEntity());
		mEntity->SetMesh(cube);
		//The unit cube asset has vertically flipped UV coordinates.
		mEntity->Roll(Degree(180));
		cube->SetMaterial(mFont->GetMaterial());

		mScene.AddRenderable(mEntity);
	}
private:
	Scene mScene;
	shared_ptr< SceneEntity > mEntity;
	shared_ptr<Font> mFont;
	CountDownTimer mTimeout;
};

TEST_CASE("FontMap")
{
	// Turn off log output, we will just use output from this test.
	//gDefaultLogger.SetLogMask(Echo::Logger::LogLevels::NONE);
	shared_ptr<Application> application(new FontMapTest());
	application->Execute();
}
