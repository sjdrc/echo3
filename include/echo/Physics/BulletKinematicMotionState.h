#ifndef _ECHOBULLETKINEMATICMOTIONSTATE_H_
#define _ECHOBULLETKINEMATICMOTIONSTATE_H_

#include <echo/Physics/BulletMotionState.h>

namespace Echo
{
	class BulletRigidBody;
	class NodeInterface;
	
	/**
	 * BulletDynamicMotionState.
	 * @see MotionState.
	 */
	class BulletKinematicMotionState : public BulletMotionState
	{
	public:
		/**
		 * Constructor.
		 * @param body the body the motion state is intended for.
		 * @param nodeInterface Must not be null.
		 */
		BulletKinematicMotionState(BulletRigidBody& body, shared_ptr<NodeInterface> nodeInterface);
		virtual ~BulletKinematicMotionState();

		//Bullet overrides
		virtual void getWorldTransform(btTransform& worldTrans ) const override;
		virtual void setWorldTransform(const btTransform&) override;
	};
}
#endif
