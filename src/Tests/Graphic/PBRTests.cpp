#include <doctest/doctest.h>
#undef INFO
#include <echo/Graphics/Renderer.h>
#include <echo/Graphics/MultiRenderer.h>
#include <echo/Graphics/Scene.h>
#include <echo/Graphics/SceneRenderable.h>
#include <echo/Graphics/SceneEntity.h>
#include <echo/Graphics/Mesh.h>
#include <echo/Graphics/Camera.h>
#include <echo/Graphics/Light.h>
#include <echo/Graphics/Viewport.h>
#include <echo/Graphics/Font.h>
#include <echo/Kernel/Kernel.h>
#include <echo/Resource/MeshManager.h>
#include <echo/Application.h>
#include <echo/Graphics/Sprite.h>
#include <echo/Graphics/MaterialAnimation.h>
#include <echo/FileSystem/FileSystem.h>
#include <echo/FileSystem/FileSystemSourceFile.h>
#include <echo/Resource/TextureManager.h>
#include <echo/Resource/MaterialManager.h>
#include <echo/Resource/SkeletonManager.h>
#include <echo/Animation/SkeletonAnimationState.h>
#include <echo/Chrono/CountDownTimer.h>
#include <echo/Util/Oscillator.h>
#include <echo/Graphics/OrbitCameraController.h>

using namespace Echo;

class PBRTest : public Application
{
public:
	bool Initialise()
	{
		if (!Application::Initialise("PBRTest", "data/PBRTest.config"))
		{
			return false;
		}
		
		shared_ptr<MultiRenderer> multiRenderer=make_shared<MultiRenderer>("Renderer");
		multiRenderer->AddRenderTarget(GetRenderTarget());
		AddTask(multiRenderer);

		AddTask(mScene);
		shared_ptr<Viewport> viewport(new Viewport(0,0,1,1));
		shared_ptr<Camera> camera = mScene.CreateCamera();
		camera->UpdateAspectForViewport(*viewport, *GetRenderTarget());
		camera->CalculateAndSetDistanceForYFOV(10,Vector3(0,0,0),Vector3(0,5,-5));
		
		shared_ptr<Renderer> renderer = multiRenderer->CreateRenderer(viewport, camera);

		const Configuration& configuration = GetConfiguration();
		Size numberOfMaterials = configuration.GetNumberOfOptionsNamed("PBRMaterial");
		for(Size i=0; i < numberOfMaterials; i++)
		{
			std::string material = configuration.GetAtIndex<std::string>("PBRMaterial","",i);
			Size x = (i % 10);
			Size y = i / 10;

			{
				shared_ptr<Mesh> mesh(new Mesh());
				mesh->CreateUVSphereSubMesh(3.5, 10, 10);
				mesh->SetMaterial(GetMaterialManager()->GetResource(material));
				shared_ptr<SceneEntity> entity(new SceneEntity());
				entity->SetMesh(mesh);
				entity->SetPosition(Vector3(x*10,y*10,5));
				mScene.AddRenderable(entity,true);
			}

			{
				shared_ptr<Mesh> mesh(new Mesh());
				mesh->CreateCubeSubMesh(Vector3(5,5,5),10,10,10);
				mesh->SetMaterial(GetMaterialManager()->GetResource(material));
				shared_ptr<SceneEntity> entity(new SceneEntity());
				entity->SetMesh(mesh);
				entity->SetPosition(Vector3(x*10,y*10,-5));
				mScene.AddRenderable(entity,true);
			}
		}

		FunctionBinder binder;
		binder.EnableBuiltIn();
		binder.Register("Light",[this](Vector3 position, Colour colour, f32 power){
			shared_ptr<Light> light = mScene.CreateLight();
			light->SetPosition(position);
			light->SetDiffuseColour(colour);
			light->SetPowerScale(power);

			{
				shared_ptr<Mesh> mesh(new Mesh());
				mesh->CreateUVSphereSubMesh(0.5,10,10,10);
				shared_ptr<Material> lightMaterial = make_shared<Material>();
				lightMaterial->SetToDefaultMaterial();
				lightMaterial->GetPass(0)->SetDiffuse(colour);
				mesh->SetMaterial(lightMaterial);
				shared_ptr<SceneEntity> entity(new SceneEntity());
				entity->SetMesh(mesh);
				entity->SetPosition(position);
				mScene.AddRenderable(entity,true);
			}

		},false,boost::fusion::vector<Vector3,Colour,f32>());

		Size numberOfLights = configuration.GetNumberOfOptionsNamed("light");
		for(Size i=0; i < numberOfLights; i++)
		{
			std::string lightDetails = configuration.GetAtIndex<std::string>("light","",i);
			binder.Call(lightDetails);
		}

		shared_ptr<Mesh> mesh(new Mesh());
		mesh->CreateUVSphereSubMesh(0.1, 10, 10, 10);
		mesh->SetMaterial(GetMaterialManager()->GetResource("Widget"));
		shared_ptr<SceneEntity> entity(new SceneEntity());
		entity->SetMesh(mesh);
		mScene.AddRenderable(entity,false);

		shared_ptr<OrbitCameraController> controller(new OrbitCameraController(camera,renderer,entity,GetInputManager()));
		controller->SetFallbackTargetPlane(Plane(Vector3(0,1,0),-5));

		AddTask(controller);

		return true;
	}
	Scene mScene;
};

TEST_CASE("PBR")
{
	// Turn off log output, we will just use output from this test.
	//gDefaultLogger.SetLogMask(Echo::Logger::LogLevels:NONE);

	PBRTest pbrTest;
	REQUIRE(pbrTest.Initialise());
	pbrTest.Execute();
}
