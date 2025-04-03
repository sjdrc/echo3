#ifndef _ECHOBULLETPHYSICSWORLD_H_
#define _ECHOBULLETPHYSICSWORLD_H_

#include <echo/Physics/PhysicsWorld.h>
#include <set>

class btDynamicsWorld;

namespace Echo
{
	class PhysicsShape;
	class BulletRigidBody;
	class BulletPhysicsBody;

	class BulletPhysicsWorld : public PhysicsWorld
	{
	public:
		BulletPhysicsWorld();
		BulletPhysicsWorld(shared_ptr<btDynamicsWorld> dynamicsWorld);
		virtual ~BulletPhysicsWorld();
		
		void SetDynamicsWorld(shared_ptr<btDynamicsWorld> dynamicsWorld);
		shared_ptr<btDynamicsWorld> GetDynamicsWorld() const {return mDynamicsWorld;}
		
		virtual void Update(Seconds lastFrameTime) override;

		virtual void AddBody(shared_ptr<PhysicsBody> body) override;
		virtual void RemoveBody(shared_ptr<PhysicsBody> body) override;
	private:
		shared_ptr<btDynamicsWorld> mDynamicsWorld;
		std::set< shared_ptr<PhysicsBody> > mBodies;
	};
}
#endif
