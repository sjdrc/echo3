#include <echo/Application.h>
#include <echo/Graphics/MultiRenderer.h>
#include <echo/Graphics/Viewport.h>
#include <echo/Graphics/Scene.h>
#include <echo/Graphics/Camera.h>
#include <echo/Graphics/ParticleSystems.h>
#include <echo/Resource/MaterialManager.h>
#include <echo/Util/Oscillator.h>
#include <echo/cpp/functional>

#include <doctest/doctest.h>

using namespace Echo;

class ParticleTest : public Application
{
public:
	bool Initialise()
	{
		//Initialise the application from a config file.
		if (!Application::Initialise("ParticleTest", "data/ParticleTest.config"))
		{
			return false;
		}

		shared_ptr<Viewport> viewport(new Viewport(0,0,1,1));
		shared_ptr<Camera> camera = mScene.CreateCamera();
		camera->UpdateAspectForViewport(*viewport, *GetRenderTarget());
		camera->CalculateAndSetDistanceForXFOV(45,Vector3(0,0,0),Vector3(0,0,1));
		GetRenderer()->CreateRenderer(viewport, camera);
		AddTask(mScene);

		shared_ptr<SimpleStandardParticleSystem> particleSystem(new SimpleStandardParticleSystem(SimpleStandardParticleProcessor(),
																		StandardParticleQuadMeshBuilder(Vector2(0.5,0.5))));
		particleSystem->SetMaterial(GetMaterialManager()->GetResource("Particle"));

		StandardParticle lowerLimit(Vector3::ZERO,			//Position
									Vector3(-7,-5,-5),		//Velocity
									Vector3(-1,-1,-1),		//Acceleration
									Vector3::ZERO,			//Damping
									Vector3::UNIT_SCALE,	//Scale
									Vector3::ZERO,			//Scale velocity
									Vector3::ZERO,			//Scale acceleration
									Vector3::ZERO,			//Scale damping
									Vector3::ZERO,			//Angle
									Vector3(-2,-2,-2),		//Angular Velocity
									Vector3::ZERO,			//Angular acceleration
									Vector3::ZERO,			//Angular Damping
									Colours::WHITE,			//Initial Colour
									Colour(1.f,1.f,1.f,0.f),//Final Colour
									Seconds(2));			//Time
		StandardParticle upperLimit(Vector3::ZERO,			//Position
									Vector3(7,5,7),			//Velocity
									Vector3(1,1,1),			//Acceleration
									Vector3(1.0,10.0,1.0),	//Damping
									Vector3::UNIT_SCALE,	//Scale
									Vector3::ZERO,			//Scale velocity
									Vector3::ZERO,			//Scale acceleration
									Vector3::ZERO,			//Scale damping
									Vector3::ZERO,			//Angle
									Vector3(2,2,2),			//Angular Velocity
									Vector3::ZERO,			//Angular acceleration
									Vector3::ZERO,			//Angular Damping
									Colours::RED,			//Initial Colour
									Colour(1.f,0.f,0.f,0.f),//Final Colour
									Seconds(4));			//Time
		shared_ptr< PointEmitter<SimpleStandardParticleSystem> > emitter(
			new PointEmitter<SimpleStandardParticleSystem>(	particleSystem,
															lowerLimit,
															upperLimit,
															Seconds(0.1),
															50));
		particleSystem->AddTask(emitter);
		mScene.AddRenderable(particleSystem);
		mScene.AddTask(particleSystem);
		
		
		StandardParticle sphereParticleLowerLimit;
		sphereParticleLowerLimit.SetTime(Seconds(2)).
			SetScale(Vector3(1,1,1)).
			SetScaleVelocity(Vector3(-0.1,0.1,0.1)).
			SetInitialColour(Colours::SAPPHIRE).
			SetFinalColour(Colour(Colours::SAPPHIRE.mRed,Colours::SAPPHIRE.mGreen,Colours::SAPPHIRE.mBlue,0.f));
		StandardParticle sphereParticleUpperLimit;
		sphereParticleUpperLimit.SetTime(Seconds(5)).
			SetScale(Vector3(2,2,2)).
			SetScaleVelocity(Vector3(-0.3,0.3,0.3)).
			SetAngle(Vector3(Maths::TWO_PI,Maths::TWO_PI,Maths::TWO_PI)).
			SetInitialColour(Colours::SAPPHIRE).
			SetFinalColour(Colour(Colours::SAPPHIRE.mRed,Colours::SAPPHIRE.mGreen,Colours::SAPPHIRE.mBlue,0.f));
		
		shared_ptr<SimpleStandardParticleSystem> sphereParticleSystem(
			new SimpleStandardParticleSystem(	SimpleStandardParticleProcessor(),
												StandardParticleQuadMeshBuilder(Vector2(0.25,0.25))));
		sphereParticleSystem->SetMaterial(GetMaterialManager()->GetResource("Particle"));
		sphereParticleSystem->SetPosition(Vector3(0,10,0));
		
		shared_ptr< VolumeEmitter<SimpleStandardParticleSystem,Sphere> > sphereEmitter(
			new VolumeEmitter<SimpleStandardParticleSystem,Sphere>(	sphereParticleSystem,
																	Sphere(Vector3::ZERO,3.),
																	sphereParticleLowerLimit,
																	sphereParticleUpperLimit,
																	Seconds(0.01),
																	10));
		sphereParticleSystem->AddTask(sphereEmitter);
		mScene.AddRenderable(sphereParticleSystem);
		mScene.AddTask(sphereParticleSystem);

		shared_ptr<SimpleStandardParticleSystem> boxParticleSystem(
			new SimpleStandardParticleSystem(SimpleStandardParticleProcessor(),
											StandardParticleQuadMeshBuilder(Vector2(0.35,0.35))));
		boxParticleSystem->SetMaterial(GetMaterialManager()->GetResource("Particle"));
		boxParticleSystem->SetPosition(Vector3(0,-10,0));
		
		shared_ptr< VolumeEmitter<SimpleStandardParticleSystem,AxisAlignedBox> > boxEmitter(
			new VolumeEmitter<SimpleStandardParticleSystem,AxisAlignedBox>(	boxParticleSystem,
																			AxisAlignedBox(Vector3(-2.5,-2.5,-2.5),Vector3(2.5,2.5,2.5)),
																			sphereParticleLowerLimit,
																			sphereParticleUpperLimit,
																			Seconds(0.01),
																			10));
		boxParticleSystem->AddTask(boxEmitter);
		mScene.AddRenderable(boxParticleSystem);
		mScene.AddTask(boxParticleSystem);
		
		//Create an Oscillator to move the camera around
		shared_ptr< Oscillator<f32> > cameraMover(new Oscillator<f32>(bind(static_cast<void(Camera::*)(f32,f32,f32)>(&Camera::SetPosition),camera,placeholders::_1,placeholders::_1,placeholders::_2),50.f,1.25f,0.f));
		camera->SetAutoTracking(true,particleSystem);
		AddTask(cameraMover);
		
		AddTask(GetRenderer());
		return true;
	}
private:
	Scene mScene;
};

TEST_CASE("Particle")
{
	// Turn off log output, we will just use output from this test.
	gDefaultLogger.SetLogMask(Echo::Logger::LogLevels::NONE);

	ParticleTest particleTest;
	REQUIRE(particleTest.Initialise());
	particleTest.Execute();
}
