#ifndef _ECHOBULLETDYNAMICMOTIONSTATE_H_
#define _ECHOBULLETDYNAMICMOTIONSTATE_H_

#include <echo/Physics/BulletMotionState.h>

namespace Echo
{
	class BulletRigidBody;
	class NodeInterface;
	
	/**
	 * BulletDynamicMotionState.
	 * @see MotionState.
	 */
	class BulletDynamicMotionState : public BulletMotionState
	{
	public:
		/**
		 * Constructor.
		 * @param body the body the motion state is intended for.
		 * @param nodeInterface Must not be null.
		 */
		BulletDynamicMotionState(BulletRigidBody& body, shared_ptr<NodeInterface> nodeInterface);
		virtual ~BulletDynamicMotionState();

		//Bullet overrides
		virtual void getWorldTransform(btTransform& ) const override;
		virtual void setWorldTransform(const btTransform& worldTrans) override;
	};
}
#endif
