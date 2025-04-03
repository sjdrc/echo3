#ifndef ECHOBULLETMANIFOLDRESULT_H
#define ECHOBULLETMANIFOLDRESULT_H

#include <echo/Physics/CollisionResult.h>
#include <BulletCollision/CollisionDispatch/btManifoldResult.h>

namespace Echo
{
	class BulletCollisionResult : public CollisionResult, public btManifoldResult
	{
	public:
		BulletCollisionResult(const btCollisionObjectWrapper* body0, const btCollisionObjectWrapper* body1) :
			btManifoldResult(body0, body1),
			mNumberOfContactPoints(0),
			mLastContactPoint(Vector3::ZERO)
		{}

		~BulletCollisionResult(){}

		virtual Size GetNumberOfContactPoints() const override {return mNumberOfContactPoints;}
		virtual Vector3 GetLastContactPoint() const override { return mLastContactPoint;}
	private:
		/**
		 * Called by bullet to add a contact point.
		 */
		virtual void addContactPoint(const btVector3& normalOnBInWorld, const btVector3& pointInWorld, btScalar depth) override
		{
			mNumberOfContactPoints++;
			mLastContactPoint.x = pointInWorld.getX();
			mLastContactPoint.y = pointInWorld.getY();
			mLastContactPoint.z = pointInWorld.getZ();
			btManifoldResult::addContactPoint(normalOnBInWorld, pointInWorld, depth);
		}
		
		Size mNumberOfContactPoints;
		Vector3 mLastContactPoint;
	};
}
#endif /* BULLETMANIFOLDRESULT_H */

