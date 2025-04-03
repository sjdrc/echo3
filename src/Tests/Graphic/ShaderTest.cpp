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
#include <echo/Resource/TextureManager.h>
#include <echo/Graphics/RenderPass.h>
#include <echo/Chrono/CountDownTimer.h>
#include <echo/Util/Oscillator.h>
#include <echo/cpp/functional>

#include <echo/GUI/Cursor.h>
#include <echo/GUI/Screen.h>
#include <echo/Input/Input.h>

#include <doctest/doctest.h>

using namespace Echo;

class ShaderTest : public Application
{
public:
	ShaderTest() : mTimeout(Seconds(30.0),bind(&TaskManager::RemoveAllTasks,this),"RemoveAllTasks")
	{
		Initialise("RenderTest","data/ShaderTest.config");
		
		//Load resource lists
		GetMaterialManager()->LoadList(GetFileSystem()->Open("data/materials.list"));
		
		shared_ptr<Camera> camera = mScene.CreateCamera();
		camera->SetPosition(0,0,2);
		camera->LookAt(0,0,0);
		GetRenderer()->CreateRenderer(make_shared<Viewport>(0,0,1,1),camera);
		camera->CalculateAndSetDistanceForYFOV(15,Vector3(0,0,0),Vector3(0,0,-1));
		AddTask(GetRenderer().get());
		AddTask(mTimeout);
		
		shared_ptr<Mesh> mesh(new Mesh());
		mesh->CreateCubeSubMesh(Vector3(5,5,5),20,20,20);
		mesh->SetMaterial(GetMaterialManager()->GetResource("BasicShader"));
		
		//Create a couple of Scene entities and set them up to use the cube meshes.
		mEntity = shared_ptr< SceneEntity >(new SceneEntity());
		mEntity->SetMesh(mesh);
		
		shared_ptr< Oscillator<Radian> > spinner(new Oscillator<Radian>(bind(&Node::SetOrientationEuler,mEntity,placeholders::_1,placeholders::_2,Degree(0))));
		mScene.AddTask(spinner);
		
		mScene.AddRenderable(mEntity);
		AddTask(mScene);
	}
private:
	Scene mScene;
	shared_ptr< SceneEntity > mEntity;
	CountDownTimer mTimeout;
};

TEST_CASE("Shader")
{
	// Turn off log output, we will just use output from this test.
	gDefaultLogger.SetLogMask(Echo::Logger::LogLevels::NONE);
	shared_ptr<Application> application = shared_ptr<Application>(new ShaderTest());
	application->Execute();
}
