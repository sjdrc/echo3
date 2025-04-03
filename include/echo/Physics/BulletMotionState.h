#ifndef _ECHOBULLETMOTIONSTATE_H_
#define _ECHOBULLETMOTIONSTATE_H_

#include <echo/Physics/MotionState.h>
#include <echo/Physics/BulletRigidBody.h>
#include <LinearMath/btMotionState.h>

namespace Echo
{
	class NodeInterface;
	
	/**
	 * BulletMotionState.
	 */
	class BulletMotionState : public MotionState, public btMotionState
	{
	public:
		/**
		 * Constructor.
		 * @param body the body the motion state is intended for.
		 * @param nodeInterface Must not be null.
		 */
		BulletMotionState(BulletRigidBody& body, shared_ptr<NodeInterface> nodeInterface);
		virtual ~BulletMotionState();
		virtual void Activate() override;
		virtual void Deactivate() override;
		virtual BulletRigidBody& GetBody() override;
		virtual shared_ptr<NodeInterface> GetNodeInterface() const override;
	protected:
		BulletRigidBody& mBody;
		shared_ptr<NodeInterface> mNodeInterface;
	};
}
#endif
