#ifndef _ECHOBULLETRIGIDBODY_H_
#define _ECHOBULLETRIGIDBODY_H_

#include <echo/Types.h>
#include <echo/Maths/Vector3.h>
#include <echo/Maths/AxisAlignedBox.h>
#include <echo/Physics/PhysicsBody.h>

class btRigidBody;

namespace Echo
{
	class PhysicsShape;
	class MotionState;
	class BulletPhysicsShape;
	class BulletMotionState;
	class BulletPhysicsWorld;
	
	/**
	 * PhysicsBody.
	 */
	class BulletRigidBody : public PhysicsBody
	{
	public:
		/**
		 * Constructor.
		 * @param world Must not be null.
		 * @param bulletBody Must not be null.
		 */
		BulletRigidBody(shared_ptr<BulletPhysicsWorld> world, shared_ptr<btRigidBody> bulletBody);
		virtual ~BulletRigidBody();
		void SetMotionState(shared_ptr<BulletMotionState> motionState);
		virtual shared_ptr<MotionState> GetMotionState() const override;
		virtual shared_ptr<PhysicsShape> GetShape() const override;
		void SetShape(shared_ptr<BulletPhysicsShape> shape);
		virtual Vector3 GetPosition() const override;
		virtual void SetPosition(const Vector3& position) override;
		virtual Quaternion GetOrientation() const override;
		virtual void SetOrientation(const Quaternion& orientation) override;
		virtual bool IsInWorld() const override;
		virtual void SetGravity(const Vector3& acceleration) override;
		virtual Vector3 GetGravity() const override;
		virtual void SetDamping(Scalar linearDamping, Scalar angularDamping) override;
		virtual Scalar GetLinearDamping() const override;
		virtual Scalar GetAngularDamping() const override;
		virtual Scalar GetLinearSleepingThreshold() const override;
		virtual Scalar GetAngularSleepingThreshold() const override;
		virtual void ApplyDamping(Seconds timeStep) const override;
		virtual void SetMassProperties(Scalar mass, const Vector3& inertia) override;
		virtual void SetCentreOfMass(const Vector3& centreOfMass) override;
		virtual void ApplyCentralForce(const Vector3& force) override;
		virtual Vector3 GetTotalForce() const override;
		virtual Vector3 GetTotalTorque() const override;
		virtual void SetSleepingThresholds(Scalar linear,Scalar angular) override;
		virtual void ApplyTorque(const Vector3& torque) override;
		virtual void ApplyForce(const Vector3& force, const Vector3& relativePosition) override;
		virtual void ApplyCentralImpulse(const Vector3& impulse) override;
		virtual void ApplyTorqueImpulse(const Vector3& torque) override;
		virtual void ApplyImpulse(const Vector3& impulse, const Vector3& relativePosition) override;
		virtual void ClearForces() override;
		virtual Vector3 GetCentreOfMass() const override;
		virtual Vector3 GetLinearVelocity() const override;
		virtual Vector3 GetAngularVelocity() const override;
		virtual void SetLinearVelocity(const Vector3& linearVelocity) override;
		virtual void SetAngularVelocity(const Vector3& angularVelocity) override;
		virtual Vector3 GetVelocityInLocalPoint(const Vector3& relativePosition) const override;
		virtual AxisAlignedBox GetAxisAlignedBox() const override;
		virtual void SetDeactivationEnabled(bool deactivationEnabled) override;

		shared_ptr<btRigidBody> GetBulletBody() const {return mBulletBody;}
		bool IsStaticObject() const;
	private:
		shared_ptr<BulletPhysicsShape> mShape;
		shared_ptr<MotionState> mMotionState;
		shared_ptr<btRigidBody> mBulletBody;
	};
}
#endif
