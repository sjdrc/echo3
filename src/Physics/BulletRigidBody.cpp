#include <echo/Physics/BulletRigidBody.h>
#include <echo/Physics/BulletPhysicsShape.h>
#include <echo/Physics/BulletPhysicsWorld.h>
#include <echo/Physics/BulletMotionState.h>
#include <echo/Graphics/NodeInterface.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <BulletCollision/CollisionShapes/btCollisionShape.h>
#include <echo/Physics/BulletTypeConverters.h>

namespace Echo
{
	BulletRigidBody::BulletRigidBody(shared_ptr<BulletPhysicsWorld> world, shared_ptr<btRigidBody> bulletBody) :
		PhysicsBody(world),
		mBulletBody(bulletBody)
	{
		assert(world && "world must not be null");
		assert(bulletBody && "body must not be null");
		mBulletBody->setUserPointer(this);
	}

	BulletRigidBody::~BulletRigidBody()
	{
		mBulletBody->setUserPointer(0);
	}

	void BulletRigidBody::SetMotionState(shared_ptr<BulletMotionState> motionState)
	{
		mMotionState = motionState;
	}

	shared_ptr<MotionState> BulletRigidBody::GetMotionState() const
	{
		return mMotionState;
	}

	shared_ptr<PhysicsShape> BulletRigidBody::GetShape() const
	{
		return mShape;
	}

	void BulletRigidBody::SetShape(shared_ptr<BulletPhysicsShape> shape)
	{
		mShape = shape;
		if(mShape)
		{
			if(!IsStaticObject() && mBulletBody->getInvMass()!=0.f)
			{
				btVector3 intertia;
				btScalar mass = 1.f / mBulletBody->getInvMass();
				mShape->GetBulletShape()->calculateLocalInertia(mass,intertia);
				mBulletBody->setMassProps(mass,intertia);
			}
			mBulletBody->setCollisionShape(mShape->GetBulletShape());
		}
	}

	Vector3 BulletRigidBody::GetPosition() const
	{
		return Convert(mBulletBody->getWorldTransform().getOrigin());
	}
	
	void BulletRigidBody::SetPosition(const Vector3& position)
	{
		mBulletBody->getWorldTransform().setOrigin(Convert(position));
		if(mMotionState && mMotionState->GetNodeInterface())
		{
			mMotionState->GetNodeInterface()->SetPosition(position);
		}
	}
	
	Quaternion BulletRigidBody::GetOrientation() const
	{
		return Convert(mBulletBody->getWorldTransform().getRotation());
	}
	
	void BulletRigidBody::SetOrientation(const Quaternion& orientation)
	{
		mBulletBody->getWorldTransform().setRotation(Convert(orientation));
	}
	
	bool BulletRigidBody::IsInWorld() const
	{
		return mBulletBody->isInWorld();
	}

	void BulletRigidBody::SetGravity(const Vector3& acceleration)
	{
		mBulletBody->setGravity(Convert(acceleration));
	}

	Vector3 BulletRigidBody::GetGravity() const
	{
		return Convert(mBulletBody->getGravity());
	}

	void BulletRigidBody::SetDamping(Scalar linearDamping, Scalar angularDamping)
	{
		mBulletBody->setDamping(linearDamping,angularDamping);
	}

	Scalar BulletRigidBody::GetLinearDamping() const
	{
		return mBulletBody->getLinearDamping();
	}

	Scalar BulletRigidBody::GetAngularDamping() const
	{
		return mBulletBody->getAngularDamping();
	}

	Scalar BulletRigidBody::GetLinearSleepingThreshold() const
	{
		return mBulletBody->getLinearSleepingThreshold();
	}

	Scalar BulletRigidBody::GetAngularSleepingThreshold() const
	{
		return mBulletBody->getAngularSleepingThreshold();
	}

	void BulletRigidBody::ApplyDamping(Seconds timeStep) const
	{
		mBulletBody->applyDamping(timeStep.count());
	}

	void BulletRigidBody::SetMassProperties(Scalar mass, const Vector3& inertia)
	{
		return mBulletBody->setMassProps(mass, Convert(inertia));
	}

	void BulletRigidBody::SetCentreOfMass(const Vector3& centreOfMass)
	{
		mBulletBody->setCenterOfMassTransform(btTransform(btQuaternion(0.0f, 0.0f, 0.0f, 1.0f), Convert(centreOfMass)));
	}

	void BulletRigidBody::ApplyCentralForce(const Vector3& force)
	{
		mBulletBody->applyCentralForce(Convert(force));
	}

	Vector3 BulletRigidBody::GetTotalForce() const
	{
		return Convert(mBulletBody->getTotalForce());
	}

	Vector3 BulletRigidBody::GetTotalTorque() const
	{
		return Convert(mBulletBody->getTotalTorque());
	}

	void BulletRigidBody::SetSleepingThresholds(Scalar linear,Scalar angular)
	{
		mBulletBody->setSleepingThresholds(linear,angular);
	}

	void BulletRigidBody::ApplyTorque(const Vector3& torque)
	{
		mBulletBody->applyTorque(Convert(torque));
	}

	void BulletRigidBody::ApplyForce(const Vector3& force, const Vector3& relativePosition)
	{
		mBulletBody->applyForce(Convert(force),Convert(relativePosition));
	}

	void BulletRigidBody::ApplyCentralImpulse(const Vector3& impulse)
	{
		mBulletBody->applyCentralImpulse(Convert(impulse));
	}

	void BulletRigidBody::ApplyTorqueImpulse(const Vector3& torque)
	{
		mBulletBody->applyTorqueImpulse(Convert(torque));
	}

	void BulletRigidBody::ApplyImpulse(const Vector3& impulse, const Vector3& relativePosition)
	{
		mBulletBody->applyImpulse(Convert(impulse),Convert(relativePosition));
	}

	void BulletRigidBody::ClearForces()
	{
		mBulletBody->clearForces();
	}

	Vector3 BulletRigidBody::GetCentreOfMass() const
	{
		return Convert(mBulletBody->getCenterOfMassPosition());
	}

	Vector3 BulletRigidBody::GetLinearVelocity() const
	{
		return Convert(mBulletBody->getLinearVelocity());
	}

	Vector3 BulletRigidBody::GetAngularVelocity() const
	{
		return Convert(mBulletBody->getAngularVelocity());
	}

	void BulletRigidBody::SetLinearVelocity(const Vector3& linearVelocity)
	{
		mBulletBody->setLinearVelocity(Convert(linearVelocity));
	}

	void BulletRigidBody::SetAngularVelocity(const Vector3& angularVelocity)
	{
		mBulletBody->setLinearVelocity(Convert(angularVelocity));
	}

	Vector3 BulletRigidBody::GetVelocityInLocalPoint(const Vector3& relativePosition) const
	{
		return Convert(mBulletBody->getVelocityInLocalPoint(Convert(relativePosition)));
	}

	AxisAlignedBox BulletRigidBody::GetAxisAlignedBox() const
	{
		btVector3 aabbMin;
		btVector3 aabbMax;
		mBulletBody->getAabb(aabbMin,aabbMax);
		return AxisAlignedBox(Convert(aabbMin),Convert(aabbMax));
	}

	void BulletRigidBody::SetDeactivationEnabled(bool deactivationEnabled)
	{
		if(!deactivationEnabled)
		{
			mBulletBody->forceActivationState(DISABLE_DEACTIVATION);
		}else
		{
			if(mBulletBody->getActivationState() & DISABLE_DEACTIVATION)
			{
				mBulletBody->forceActivationState(mBulletBody->getActivationState()^DISABLE_DEACTIVATION);
			}
		}
	}
	
	bool BulletRigidBody::IsStaticObject() const
	{
		return (mBulletBody->getCollisionFlags()&btCollisionObject::CF_STATIC_OBJECT);
	}

}
