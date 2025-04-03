#include <echo/Physics/BulletFactory.h>
#include <echo/Physics/BulletRigidBody.h>
#include <echo/Physics/BulletPhysicsShape.h>
#include <echo/Physics/BulletPhysicsWorld.h>
#include <echo/Physics/BulletDynamicMotionState.h>
#include <echo/Physics/BulletKinematicMotionState.h>
#include <echo/Physics/BulletMeshDebugDrawer.h>
#include <echo/Physics/BulletTypeConverters.h>
#include <echo/Physics/BulletMeshPhysicsShape.h>

#include <echo/Graphics/SceneEntity.h>

#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h>
#include <BulletCollision/BroadphaseCollision/btAxisSweep3.h>
#include <BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h>

#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletCollision/CollisionShapes/btSphereShape.h>
#include <BulletCollision/CollisionShapes/btStaticPlaneShape.h>
#include <BulletCollision/CollisionShapes/btCapsuleShape.h>

namespace Echo
{
	BulletFactory::BulletFactory(AxisAlignedBox worldExtent, int maxProxies)
	{
		mWorld = _CreateDefaulDynamicsWorld(worldExtent,maxProxies);
	}
	
	BulletFactory::BulletFactory(shared_ptr<BulletPhysicsWorld> world) : mWorld(world)
	{
		assert(mWorld && "World must not be null.");
	}
	
	BulletFactory::~BulletFactory()
	{
		
	}

	shared_ptr<PhysicsShape> BulletFactory::CreateBox(Vector3 extents)
	{
		return shared_ptr<BulletPhysicsShape>(new BulletPhysicsShape(new btBoxShape(Convert(extents*0.5f))));
	}
	
	shared_ptr<PhysicsShape> BulletFactory::CreateSphere(Scalar radius)
	{
		return shared_ptr<BulletPhysicsShape>(new BulletPhysicsShape(new btSphereShape(radius)));
	}
	
	shared_ptr<PhysicsShape> BulletFactory::CreateStaticPlane(Vector3 planeNormal, Scalar planeConstant)
	{
		return shared_ptr<BulletPhysicsShape>(new BulletPhysicsShape(new btStaticPlaneShape(Convert(planeNormal),planeConstant)));
	}
	
	shared_ptr<PhysicsShape> BulletFactory::CreateCapsule(Scalar radius, Scalar height)
	{
		return shared_ptr<BulletPhysicsShape>(new BulletPhysicsShape(new btCapsuleShape(radius,height)));
	}
	
	shared_ptr<PhysicsShape> BulletFactory::CreateCapsuleX(Scalar radius, Scalar height)
	{
		return shared_ptr<PhysicsShape>(new BulletPhysicsShape(new btCapsuleShapeX(radius,height)));
	}
	
	shared_ptr<PhysicsShape> BulletFactory::CreateCapsuleZ(Scalar radius, Scalar height)
	{
		return shared_ptr<PhysicsShape>(new BulletPhysicsShape(new btCapsuleShapeZ(radius,height)));
	}
	
	shared_ptr<PhysicsShape> BulletFactory::CreateMesh(shared_ptr<Mesh> mesh, bool usedForDynamics)
	{
		return shared_ptr<BulletMeshPhysicsShape>(new BulletMeshPhysicsShape(mesh,
			usedForDynamics ? BulletMeshPhysicsShape::Modes::DYNAMIC_BODY : BulletMeshPhysicsShape::Modes::STATIC_BODY));
	}

	shared_ptr<PhysicsWorld> BulletFactory::CreateDefaulDynamicsWorld(AxisAlignedBox worldExtent, int maxProxies)
	{
		return _CreateDefaulDynamicsWorld(worldExtent,maxProxies);
	}

	shared_ptr<BulletPhysicsWorld> BulletFactory::_CreateDefaulDynamicsWorld(AxisAlignedBox worldExtent, int maxProxies)
	{
		btCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
		btCollisionDispatcher* dispatcher=new btCollisionDispatcher(collisionConfiguration);
		btBroadphaseInterface* pairCache=new btAxisSweep3(Convert(worldExtent.GetMinimum()), Convert(worldExtent.GetMaximum()), maxProxies);
		btConstraintSolver* constraintSolver=new btSequentialImpulseConstraintSolver();
		shared_ptr<btDynamicsWorld> dynamicsWorld(new btDiscreteDynamicsWorld(dispatcher,pairCache,constraintSolver,collisionConfiguration));
		return shared_ptr<BulletPhysicsWorld>(new BulletPhysicsWorld(dynamicsWorld));
	}
	
	shared_ptr<PhysicsBody> BulletFactory::CreateRigidBody(Scalar mass, shared_ptr<SceneEntity> forEntity, shared_ptr<PhysicsShape> shape, bool addToWorld)
	{
		shared_ptr<BulletPhysicsShape> bulletShape = dynamic_pointer_cast<BulletPhysicsShape>(shape);
		if(!bulletShape)
		{
			ECHO_LOG_ERROR("BulletFactory::CreateRigidBody: provided shape is not a BulletPhysicsShape.");
			return nullptr;
		}
		
		btRigidBody::btRigidBodyConstructionInfo constructInfo(mass, 0, 0);
		shared_ptr<btRigidBody> bulletBody(new btRigidBody(constructInfo));
		shared_ptr<BulletRigidBody> body(new BulletRigidBody(mWorld,bulletBody));
		shared_ptr<BulletDynamicMotionState> motionState = dynamic_pointer_cast<BulletDynamicMotionState>(CreateDynamicMotionState(forEntity,body,true));
		if(!motionState)
		{
			// This shouldn't happen, but I want to allow extending of the BulletFactory in the future and in doing so
			// allow changes to CreateDynamicMotionState(), in which case we'll this method will also want to be overridden.
			ECHO_LOG_ERROR("BulletFactory::CreateRigidBody: internal error. Unable to create BulletDynamicMotionState.");
			return nullptr;
		}
		body->SetMotionState(motionState);
		body->SetShape(bulletShape);
		bulletBody->setCcdMotionThreshold(1);	//Continuous Collision Detection won't be performed if the motion exceeds 1 metre.
		if(addToWorld)
		{
			mWorld->AddBody(body);
		}
		return body;
	}
	
	shared_ptr<PhysicsBody> BulletFactory::CreateRigidBody(Scalar mass, shared_ptr<PhysicsShape> shape, bool addToWorld)
	{
		shared_ptr<BulletPhysicsShape> bulletShape = dynamic_pointer_cast<BulletPhysicsShape>(shape);
		if(!bulletShape)
		{
			ECHO_LOG_ERROR("BulletFactory::CreateRigidBody: provided shape is not a BulletPhysicsShape.");
			return nullptr;
		}
		
		btRigidBody::btRigidBodyConstructionInfo constructInfo(mass, 0, 0);
		shared_ptr<btRigidBody> bulletBody(new btRigidBody(constructInfo));
		shared_ptr<BulletRigidBody> body(new BulletRigidBody(mWorld,bulletBody));
		body->SetShape(bulletShape);
		bulletBody->setCcdMotionThreshold(1);	//Continuous Collision Detection won't be performed if the motion exceeds 1 metre.
		if(addToWorld)
		{
			mWorld->AddBody(body);
		}
		return body;
	}
	
	shared_ptr<MotionState> BulletFactory::CreateDynamicMotionState(shared_ptr<SceneEntity> forEntity, shared_ptr<PhysicsBody> body, bool activate)
	{
		shared_ptr<BulletRigidBody> bulletBody = dynamic_pointer_cast<BulletRigidBody>(body);
		if(!bulletBody)
		{
			ECHO_LOG_ERROR("BulletFactory::CreateDynamicMotionState: passed body is not a BulletRigidBody.");
			return nullptr;
		}
		
		shared_ptr<BulletDynamicMotionState> motionState(new BulletDynamicMotionState(*bulletBody,forEntity));
		bulletBody->SetMotionState(motionState);
		if(activate)
		{
			motionState->Activate();
		}
		return motionState;
	}
	
	shared_ptr<SceneEntity> BulletFactory::CreateDebugSceneEntity(int debugMode)
	{
		shared_ptr<BulletMeshDebugDrawer> debugDrawer(new BulletMeshDebugDrawer());
		debugDrawer->setDebugMode(debugMode);

		//Create the scene entity.
		shared_ptr<SceneEntity> entity(new SceneEntity());
		entity->SetMesh(debugDrawer);
		
		//Set the world debug drawer.
		mWorld->GetDynamicsWorld()->setDebugDrawer(debugDrawer.get());
		return entity;
	}

}
