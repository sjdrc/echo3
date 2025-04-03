#include <echo/Physics/BulletPhysicsWorld.h>
#include <echo/Physics/BulletRigidBody.h>
#include <echo/Physics/BulletCollisionResult.h>
#include <echo/Physics/BulletTypeConverters.h>
#include <BulletDynamics/Dynamics/btDynamicsWorld.h>
#include <BulletCollision/BroadphaseCollision/btCollisionAlgorithm.h>

namespace Echo
{
	//To avoid forward declaring a bullet types in the header
	void BulletPhysicsWorldNearCallback(btBroadphasePair& collisionPair, btCollisionDispatcher& dispatcher, const btDispatcherInfo& dispatchInfo);
	void BulletPhysicsWorldTickCallback(btDynamicsWorld *world, btScalar timeStep);
	
	BulletPhysicsWorld::BulletPhysicsWorld()
	{
	}
	
	BulletPhysicsWorld::BulletPhysicsWorld(shared_ptr<btDynamicsWorld> dynamicsWorld)
	{
		SetDynamicsWorld(dynamicsWorld);
	}
	
	BulletPhysicsWorld::~BulletPhysicsWorld()
	{
		
	}

	void BulletPhysicsWorld::SetDynamicsWorld(shared_ptr<btDynamicsWorld> dynamicsWorld)
	{
		if(mDynamicsWorld)
		{
			btCollisionDispatcher* collisionDispatcher = dynamic_cast<btCollisionDispatcher*>(mDynamicsWorld->getDispatcher());
			if(collisionDispatcher)
			{
				collisionDispatcher->setNearCallback(0);
			}
		}
		mDynamicsWorld = dynamicsWorld;
		if(mDynamicsWorld)
		{
			mDynamicsWorld->setInternalTickCallback(&BulletPhysicsWorldTickCallback,reinterpret_cast<void*>(this),false);
		}
	}

	void BulletPhysicsWorld::Update(Seconds lastFrameTime)
	{
		TaskGroup::Update(lastFrameTime);
		if(mDynamicsWorld)
		{
			mDynamicsWorld->stepSimulation(lastFrameTime.count());
			if(mDynamicsWorld->getDebugDrawer())
			{
				mDynamicsWorld->debugDrawWorld();
			}
		}
	}

	void BulletPhysicsWorld::AddBody(shared_ptr<PhysicsBody> body)
	{
		shared_ptr<BulletRigidBody> bulletBody = dynamic_pointer_cast<BulletRigidBody>(body);
		if(bulletBody && mDynamicsWorld)
		{
			mDynamicsWorld->addRigidBody(bulletBody->GetBulletBody().get());
			mBodies.insert(body);
		}
	}

	void BulletPhysicsWorld::RemoveBody(shared_ptr<PhysicsBody> body)
	{
		shared_ptr<BulletRigidBody> bulletBody = dynamic_pointer_cast<BulletRigidBody>(body);
		if(bulletBody && mDynamicsWorld)
		{
			mDynamicsWorld->removeRigidBody(bulletBody->GetBulletBody().get());
			mBodies.erase(body);
		}
	}
	
	void BulletPhysicsWorldTickCallback(btDynamicsWorld *world, btScalar timeStep)
	{
		int numManifolds = world->getDispatcher()->getNumManifolds();
		for (int i = 0; i < numManifolds; i++)
		{
			btPersistentManifold* contactManifold =  world->getDispatcher()->getManifoldByIndexInternal(i);
			const btCollisionObject* obA = contactManifold->getBody0();
			const btCollisionObject* obB = contactManifold->getBody1();
			//Collisions are processed, lets get out of here if our objects do not have user pointers.
			PhysicsBody* object0 = reinterpret_cast<PhysicsBody*>(obA->getUserPointer());
			PhysicsBody* object1 = reinterpret_cast<PhysicsBody*>(obB->getUserPointer());
			SimpleCollisionResult collisionResultA;
			SimpleCollisionResult collisionResultB;

			int numContacts = contactManifold->getNumContacts();
			for (int j = 0; j < numContacts; j++)
			{
				btManifoldPoint& pt = contactManifold->getContactPoint(j);
				if (pt.getDistance() < 0.f)
				{
					const btVector3& ptA = pt.getPositionWorldOnA();
					const btVector3& ptB = pt.getPositionWorldOnB();
					//const btVector3& normalOnB = pt.m_normalWorldOnB;
					collisionResultA.AddContactPoint(Convert(ptA));
					collisionResultB.AddContactPoint(Convert(ptB));
				}
			}
			if(object0 && object1)
			{
				object1->OnCollide(*object0, collisionResultA);
				object0->OnCollide(*object1, collisionResultB);
			}
		}
	}
}
