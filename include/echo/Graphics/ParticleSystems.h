#ifndef _PARTICLESYSTEMS_H_
#define _PARTICLESYSTEMS_H_

#include <echo/Graphics/Colour.h>
#include <boost/foreach.hpp>
#include <echo/Maths/EchoMaths.h>
#include <echo/Maths/Vector2.h>
#include <echo/Graphics/SubMesh.h>
#include <echo/Graphics/Mesh.h>
#include <echo/Graphics/SceneEntity.h>
#include <echo/Graphics/ElementBuffer.h>

namespace Echo
{
	/**
	 * StandardParticle is the particle type used by SimpleStandardParticleProcessor, StandardParticleQuadMeshBuilder
	 */
	struct StandardParticle
	{
		StandardParticle(Vector3 position = Vector3::ZERO,
			Vector3 velocity = Vector3::ZERO,
			Vector3 acceleration = Vector3::ZERO,
			Vector3 linearDamping = Vector3::ZERO,
			Vector3 scale = Vector3::UNIT_SCALE,
			Vector3 scaleVelocity = Vector3::ZERO,
			Vector3 scaleAcceleration = Vector3::ZERO,
			Vector3 scaleDamping = Vector3::ZERO,
			Vector3 angle = Vector3::ZERO,
			Vector3 angularVelocity = Vector3::ZERO,
			Vector3 angularAcceleration = Vector3::ZERO,
			Vector3 angularDamping = Vector3::ZERO,
			Colour initialColour = Colours::WHITE,
			Colour finalColour = Colours::WHITE,
			Seconds timeLeft = Seconds(1)) :
				mPosition(position),
				mVelocity(velocity),
				mAcceleration(acceleration),
				mLinearDamping(linearDamping),
				mScale(scale),
				mScaleVelocity(scaleVelocity),
				mScaleAcceleration(scaleAcceleration),
				mScaleDamping(scaleDamping),
				mAngle(angle),
				mAngularVelocity(angularVelocity),
				mAngularAcceleration(angularAcceleration),
				mAngularDamping(angularDamping),
				mColour(initialColour),
				mInitialColour(initialColour),
				mFinalColour(finalColour),
				mTimeLeft(timeLeft),
				mInitialTime(timeLeft)
		{}
		Vector3 mPosition;			// Position of particle
		Vector3 mVelocity;			// Units per second
		Vector3 mAcceleration;		// Units per second per second
		Vector3 mLinearDamping;		// Velocity damping per second.
		Vector3 mScale;				// Scale of particle, renderer determines base size.
		Vector3 mScaleVelocity;		// Scale velocity.
		Vector3 mScaleAcceleration;	// Scale per second per second
		Vector3 mScaleDamping;		// Damping of scale velocity.
		Vector3 mAngle;				// Angle of particle
		Vector3 mAngularVelocity;	// Arcs per second
		Vector3 mAngularAcceleration;// Arcs per second per second
		Vector3 mAngularDamping;	// Angular velocity damping per second.
		Colour mColour;				// Colour
		Colour mInitialColour;		// Initial colour
		Colour mFinalColour;		// Final colour
		Seconds mTimeLeft;			// Time left for this particle
		Seconds mInitialTime;		// Time initially alive for

		/**
		 * Randomise this particle to values within lower and upper limits.
		 * This method is designed to be used when a particle is emitted and as such it
		 * sets the colour to the initial colour and sets the initial time and time left
		 * values to the same.
		 * @param lowerLimits The particle representing minimal values for the particle.
		 * @param upperLimits The particle representing maximum values for the particle.
		 */
		void Randomise(StandardParticle& lowerLimits, StandardParticle& upperLimits)
		{
			mPosition = Vector3::Random(lowerLimits.mPosition, upperLimits.mPosition);
			mVelocity = Vector3::Random(lowerLimits.mVelocity, upperLimits.mVelocity);
			mAcceleration = Vector3::Random(lowerLimits.mAcceleration, upperLimits.mAcceleration);
			mLinearDamping = Vector3::Random(lowerLimits.mLinearDamping, upperLimits.mLinearDamping);
			mScale = Vector3::Random(lowerLimits.mScale, upperLimits.mScale);
			mScaleVelocity = Vector3::Random(lowerLimits.mScaleVelocity, upperLimits.mScaleVelocity);
			mScaleAcceleration = Vector3::Random(lowerLimits.mScaleAcceleration, upperLimits.mScaleAcceleration);
			mScaleDamping = Vector3::Random(lowerLimits.mScaleDamping, upperLimits.mScaleDamping);
			mAngle = Vector3::Random(lowerLimits.mAngle, upperLimits.mAngle);
			mAngularVelocity = Vector3::Random(lowerLimits.mAngularVelocity, upperLimits.mAngularVelocity);
			mAngularAcceleration = Vector3::Random(lowerLimits.mAngularAcceleration, upperLimits.mAngularAcceleration);
			mAngularDamping = Vector3::Random(lowerLimits.mAngularDamping, upperLimits.mAngularDamping);
			mColour = mInitialColour = Colours::Random(lowerLimits.mInitialColour, upperLimits.mInitialColour);
			mFinalColour = Colours::Random(lowerLimits.mFinalColour, upperLimits.mFinalColour);
			mInitialTime = mTimeLeft = Seconds(Maths::RangeRandom<f32>(lowerLimits.mTimeLeft.count(), upperLimits.mTimeLeft.count()));
		}
		
		StandardParticle& SetPosition(Vector3 position){mPosition=position;return *this;}
		StandardParticle& SetScale(Vector3 scale){mScale=scale;return *this;}
		StandardParticle& SetScaleVelocity(Vector3 scaleVelocity){mScaleVelocity=scaleVelocity;return *this;}
		StandardParticle& SetScaleAcceleration(Vector3 scaleAcceleration){mScaleAcceleration=scaleAcceleration;return *this;}
		StandardParticle& SetScaleDamping(Vector3 scaleDamping){mScaleDamping=scaleDamping;return *this;}
		StandardParticle& SetVelocity(Vector3 velocity){mVelocity=velocity;return *this;}
		StandardParticle& SetAcceleration(Vector3 acceleration){mAcceleration=acceleration;return *this;}
		StandardParticle& SetLinearDamping(Vector3 linearDamping){mLinearDamping=linearDamping;return *this;}
		StandardParticle& SetAngle(Vector3 angle){mAngle=angle;return *this;}
		StandardParticle& SetAngularVelocity(Vector3 angularVelocity){mAngularVelocity=angularVelocity;return *this;}
		StandardParticle& SetAngularAcceleration(Vector3 angularAcceleration){mAngularAcceleration=angularAcceleration;return *this;}
		StandardParticle& SetAngularDamping(Vector3 angularDamping){mAngularDamping=angularDamping;return *this;}
		StandardParticle& SetColour(Colour colour){mColour=colour;return *this;}
		StandardParticle& SetInitialColour(Colour initialColour){mInitialColour=initialColour;return *this;}
		StandardParticle& SetFinalColour(Colour finalColour){mFinalColour=finalColour;return *this;}
		StandardParticle& SetTime(Seconds t){mTimeLeft=t;return *this;}
	};
	
	/**
	 * A processor that moves particles around, changes their velocity, acceleration etc.
	 */
	class SimpleStandardParticleProcessor
	{
	public:
		void ProcessParticles(std::vector<StandardParticle>& particles, Seconds lastFrameTime)
		{
			size_t numParticles = particles.size();
			for(size_t i=0;i<numParticles;)
			{
				StandardParticle& p = particles[i];
				p.mTimeLeft-=lastFrameTime;;
				if(p.mTimeLeft.count()<=0)
				{
					//Switch this particle with the end particle.
					numParticles--;
					particles[i] = particles[numParticles];
					continue;
				}
				p.mPosition += p.mVelocity * lastFrameTime.count();
				p.mVelocity += p.mAcceleration * lastFrameTime.count();
				p.mScale += p.mScaleVelocity * lastFrameTime.count();
				p.mScaleVelocity += p.mScaleAcceleration * lastFrameTime.count();
				p.mAngle += p.mAngularVelocity * lastFrameTime.count();
				p.mAngularVelocity += p.mAngularAcceleration * lastFrameTime.count();

				//Damp
				p.mVelocity -= p.mVelocity * p.mLinearDamping * lastFrameTime.count();
				p.mAngularVelocity -= p.mAngularVelocity * p.mAngularDamping * lastFrameTime.count();
				p.mScaleVelocity -= p.mScaleVelocity * p.mScaleDamping * lastFrameTime.count();
				
				//Update colour
				Scalar percentTransition = p.mTimeLeft.count()/p.mInitialTime.count();
				p.mColour = Maths::LinearInterpolate(p.mInitialColour,p.mFinalColour,1.f-percentTransition);
				++i;
			}
			particles.resize(numParticles);
		}
	};
	
	/**
	 * ParticleQuadMeshBuilder builds a mesh full of quads for each particle.
	 * The ParticleType needs the following members:
	 *  - mPosition - A Vector3 for the position.
	 *	- mScale - A Vector3 representing the scale of the particle on each axis.
	 *  - mAngle - A Vector3 with Euler angles.
	 *  - mColour - The colour of the particle.
	 */
	template< class ParticleType >
	class ParticleQuadMeshBuilder
	{
	public:
		ParticleQuadMeshBuilder(Vector2 particleSize) : mParticleHalfSize(particleSize.x*0.5, particleSize.y*0.5,0){}
		void BuildVisual(std::vector<ParticleType>& particles, SceneEntity& entity)
		{
			assert(entity.GetMesh() && "SceneEntity needs a mesh");
			assert(entity.GetMesh()->GetSubMesh(0) && "SceneEntity's mesh needs at least one sub mesh");
			shared_ptr<SubMesh> subMesh=entity.GetMesh()->GetSubMesh(0);
			
			shared_ptr<VertexBuffer> vertexBuffer = subMesh->GetVertexBuffer();
			
			const Size VERTICES_PER_PARTICLE = 4;
			const Size TRIANGLES_PER_PARTICLE = 2;
			Size numberOfVertices = particles.size() * VERTICES_PER_PARTICLE;
			
			// We need a minimum of one vertex allocated, even if there aren't any particles since we might
			// be cleaning up so we cannot bail if particles.empty()
			if(vertexBuffer->GetCapacity() < numberOfVertices+1)
			{
				vertexBuffer->Allocate(numberOfVertices+1);
			}
			
			//Clear the mesh but don't deallocate
			subMesh->Clear(false);
			
			if(particles.empty())
			{
				return;
			}
			
			VertexBuffer::Accessor<Vector3> normals=subMesh->GetComponents<Vector3>("Normal");
			VertexBuffer::Accessor<Vector3> vertices=subMesh->GetComponents<Vector3>("Position");
			VertexBuffer::Accessor<VertexColour> colours=subMesh->GetComponents<VertexColour>("Colour");
			
			shared_ptr< ElementBuffer > elementBuffer;
			Size numberOfTriangles = TRIANGLES_PER_PARTICLE*particles.size();
			elementBuffer = subMesh->GetElementBuffer();
			if(!elementBuffer)
			{
				if(!subMesh->SetElementBuffer(ElementBuffer::Types::STATIC,ElementBuffer::IndexTypes::UNSIGNED_32BIT,ElementBuffer::ElementTypes::TRIANGLE,numberOfTriangles))
				{
					ECHO_LOG_ERROR("Unable to setup Element Buffer for ParticleQuadMeshBuilder");
					return;
				}
				elementBuffer = subMesh->GetElementBuffer();
			}else
			if(elementBuffer->GetCapacity() < numberOfTriangles)
			{
				if(!elementBuffer->Allocate(numberOfTriangles * 2))
				{
					ECHO_LOG_ERROR("Reallocation of Element Buffer data failed for ParticleQuadMeshBuilder");
					return;
				}
			}
		
			VertexBuffer::Accessor<TextureUV> textureCoordinates = subMesh->GetComponents<TextureUV>("UV0");
			
			vertexBuffer->SetNumberOfElements(numberOfVertices);
			elementBuffer->SetNumberOfElements(particles.size());
			ElementBuffer::Accessor<ElementBuffer::Triangle< u32 > > triangles = elementBuffer->GetAccessor<ElementBuffer::Triangle<u32> >();

			Size vBase = 0;	//vertex base
			Size triangleBase=0;
			BOOST_FOREACH(StandardParticle& p, particles)
			{
				//Vertex
				Vector3 phs = mParticleHalfSize * p.mScale;
				Quaternion orientation = Quaternion(Radian(p.mAngle.x),Radian(p.mAngle.y),Radian(p.mAngle.z));
				normals[vBase  ] = vertices[vBase  ] = p.mPosition + (orientation * Vector3(-phs.x,phs.y,0));
				normals[vBase+1] = vertices[vBase+1] = p.mPosition + (orientation * Vector3(phs.x,phs.y,0));
				normals[vBase+2] = vertices[vBase+2] = p.mPosition + (orientation * Vector3(-phs.x,-phs.y,0));
				normals[vBase+3] = vertices[vBase+3] = p.mPosition + (orientation * Vector3(phs.x,-phs.y,0));

				textureCoordinates[vBase  ] = TextureUV(0.f, 0.f);
				textureCoordinates[vBase+1] = TextureUV(1.f, 0.f);
				textureCoordinates[vBase+2] = TextureUV(0.f, 1.f);
				textureCoordinates[vBase+3] = TextureUV(1.f, 1.f);

				colours[vBase  ] = p.mColour;
				colours[vBase+1] = p.mColour;
				colours[vBase+2] = p.mColour;
				colours[vBase+3] = p.mColour;
				
				//A couple of triangles
				auto& triangle1 = triangles[triangleBase++];
				triangle1.mA = vBase+2;		// 0-1
				triangle1.mB = vBase+1;		// |/
				triangle1.mC = vBase;		// 2 3
				
				auto& triangle2 = triangles[triangleBase++];
				triangle2.mA = vBase+3;		// 0 1
				triangle2.mB = vBase+1;		//  /|
				triangle2.mC = vBase+2;		// 2-3
				
				vBase+=VERTICES_PER_PARTICLE;
			}
			subMesh->Finalise();
		}
	private:
		Vector3 mParticleHalfSize;
	};
	
	typedef ParticleQuadMeshBuilder<StandardParticle> StandardParticleQuadMeshBuilder;
	
	template< class ParticleType >
	class ParticleEmitterInterface : public InheritableEnableSharedFromThis<ParticleEmitterInterface<ParticleType> >
	{
	public:
		ParticleEmitterInterface(){}
		virtual ~ParticleEmitterInterface() {}
		virtual void Emit(ParticleType particle) = 0;
		virtual void Emit(std::vector<ParticleType> particleBatch) = 0;
	};
	/**
	 * A simple ParticleSystem which can be be used on any particle type with
	 * a customised particle processor and particle renderer.
	 * 
	 * The template types need to be compatible with one another.
	 * 
	 * ParticleType - the particle class.
	 * ParticleProcessor - must be able to be copied and have a method:
	 *		ProcessParticles(std::vector<StandardParticle>& particles, Seconds lastFrameTime)
	 * The processor is responsible for updating all particle properties. The vector can be
	 * modified, including resized. The result is passed to the ParticleRenderer for building
	 * a renderable.
	 * ParticleRenderer - must be able to be copied and have a method:
	 *		BuildVisual(std::vector<StandardParticle>& particles, SceneEntity& entity)
	 */
	template<class ParticleType, class ParticleProcessor, class ParticleRenderer>
	class ParticleSystem : public SceneEntity, public TaskGroup, public ParticleEmitterInterface<ParticleType>
	{
	public:
		ParticleSystem(ParticleProcessor processor, ParticleRenderer renderer, size_t particlesToPreAllocateFor = 512) :
			mRenderer(renderer),
			mParticleProcessor(processor)
		{
			shared_ptr<Mesh> mesh(new Mesh());
			mesh->CreateCommonSubMesh();
			shared_ptr<Material> material(new Material());
			material->SetToDefaultMaterial();
			mesh->SetMaterial(material);
			SetMesh(mesh);
			AllocateForParticles(particlesToPreAllocateFor);
		}
		~ParticleSystem()
		{
		}
		void AllocateForParticles(size_t minimum)
		{
			mParticles.reserve(minimum);
		}

		void Emit(ParticleType particle) override
		{
			mParticles.push_back(particle);
		}

		void Emit(std::vector<ParticleType> particleBatch) override
		{
			mParticles.insert(mParticles.end(), particleBatch.begin(), particleBatch.end());
		}

		void Update(Seconds lastFrameTime) override
		{
			TaskGroup::Update(lastFrameTime);
			mParticleProcessor.ProcessParticles(mParticles,lastFrameTime);
			mRenderer.BuildVisual(mParticles,*this);
		}
		
		void Clear()
		{
			mParticles.resize(0);
		}
	private:
		std::vector< ParticleType > mParticles;
		ParticleRenderer mRenderer;
		ParticleProcessor mParticleProcessor;
	};
	
	/**
	 * SimpleParticleSystem typedef for quad particles.
	 * Create with:
	 *	shared_ptr<SimpleStandardParticleSystem>(new SimpleStandardParticleSystem(SimpleParticleProcessor(),StandardParticleQuadMeshBuilder(Vector2(quadWidth,quadHeight))))
	 * then add the particle system to a scene with AddRenderable() and to be updated with AddTask()
	 */
	typedef ParticleSystem<StandardParticle, SimpleStandardParticleProcessor, StandardParticleQuadMeshBuilder> SimpleStandardParticleSystem;
	
	/**
	 * A point emitter that is compatible with StandardParticle systems.
	 * Provide a lower and upper StandardParticle objects which are used for randomising the particle range.
	 * @note The point emitter will determine the position of a particle from the range given so it can
	 * actually be used as a volume emitter.
	 */
	template< class ParticleSystemType >
	class PointEmitter : public Task, public Node
	{
	public:
		PointEmitter(	shared_ptr<ParticleSystemType> particleSystem,
						StandardParticle lowerLimit,
						StandardParticle upperLimit,
						Seconds timeBetweenEmissions = Seconds(1),
						Scalar particlesPerEmit = 1) :
			mParticleSystem(particleSystem),
			mEmitTime(timeBetweenEmissions),
			mParticlesPerEmit(particlesPerEmit),
			mTimeUntilNextEmission(timeBetweenEmissions),
			mLowerLimit(lowerLimit),
			mUpperLimit(upperLimit)
		{}
		
		void Update(Seconds lastFrameTime) override
		{
			mTimeUntilNextEmission-=lastFrameTime;
			if(mTimeUntilNextEmission.count() <= 0)
			{
				mTimeUntilNextEmission = mEmitTime;
				
				Vector3 angle(	GetOrientation().GetPitch().ValueRadians(),
								GetOrientation().GetYaw().ValueRadians(),
								GetOrientation().GetRoll().ValueRadians());

				//Emit the batch
				std::vector<StandardParticle> particles(mParticlesPerEmit);
				BOOST_FOREACH(StandardParticle& p, particles)
				{
					p.Randomise(mLowerLimit,mUpperLimit);
					p.mPosition += GetPosition();
					p.mScale += GetScale();
					p.mAngle += angle;
				}
				mParticleSystem->Emit(particles);
			}
		}
	private:
		shared_ptr<ParticleSystemType> mParticleSystem;
		Seconds mEmitTime;
		Scalar mParticlesPerEmit;
		Seconds mTimeUntilNextEmission;
		StandardParticle mLowerLimit;
		StandardParticle mUpperLimit;
	};
	
	/**
	 * A volume emitter that is compatible with StandardParticle systems.
	 * Provide a lower and upper StandardParticle objects which are used for randomising the particle range.
	 * @note The position of the particle is overridden by the volume emitter so the position range does
	 * not affect it.
	 */
	template< class ParticleSystemType, class Volume>
	class VolumeEmitter : public Task, public Node
	{
	public:
		VolumeEmitter(	shared_ptr<ParticleSystemType> particleSystem,
						Volume volume,
						StandardParticle lowerLimit,
						StandardParticle upperLimit,
						Seconds timeBetweenEmissions = Seconds(1),
						Scalar particlesPerEmit = 1) :
			mParticleSystem(particleSystem),
			mEmitTime(timeBetweenEmissions),
			mParticlesPerEmit(particlesPerEmit),
			mTimeUntilNextEmission(timeBetweenEmissions),
			mLowerLimit(lowerLimit),
			mUpperLimit(upperLimit),
			mVolume(volume)
		{}
		
		void Update(Seconds lastFrameTime) override
		{
			mTimeUntilNextEmission-=lastFrameTime;
			if(mTimeUntilNextEmission.count() <= 0)
			{
				mTimeUntilNextEmission = mEmitTime;
				
				Vector3 angle(	GetOrientation().GetPitch().ValueRadians(),
								GetOrientation().GetYaw().ValueRadians(),
								GetOrientation().GetRoll().ValueRadians());

				//Emit the batch
				std::vector<StandardParticle> particles(mParticlesPerEmit);
				BOOST_FOREACH(StandardParticle& p, particles)
				{
					p.Randomise(mLowerLimit,mUpperLimit);
					p.mPosition = mVolume.GetRandomPointInVolume();
					p.mScale += GetScale();
					p.mAngle += angle;
				}
				mParticleSystem->Emit(particles);
			}
		}
	private:
		shared_ptr<ParticleSystemType> mParticleSystem;
		Seconds mEmitTime;
		Scalar mParticlesPerEmit;
		Seconds mTimeUntilNextEmission;
		StandardParticle mLowerLimit;
		StandardParticle mUpperLimit;
		Volume mVolume;
	};	
}
#endif 
