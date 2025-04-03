#include <echo/Physics/BulletDynamicMotionState.h>
#include <echo/Graphics/NodeInterface.h>
#include <echo/Physics/BulletTypeConverters.h>

namespace Echo
{
	BulletDynamicMotionState::BulletDynamicMotionState(BulletRigidBody& body, shared_ptr<NodeInterface> nodeInterface) :
		BulletMotionState(body,nodeInterface)
	{
		assert(nodeInterface && "nodeInterface must not be null");
	}
	BulletDynamicMotionState::~BulletDynamicMotionState()
	{
	}

	//Bullet overrides
	void BulletDynamicMotionState::getWorldTransform(btTransform& transform) const
	{
		// Dynamic states don't modify the worldTransform, although if the motion state
		// is specified during the body's construction it will need the start position.		
		transform.setOrigin(Convert(mNodeInterface->GetDerivedPosition()));
		transform.setRotation(Convert(mNodeInterface->GetDerivedOrientation()));
	}

	void BulletDynamicMotionState::setWorldTransform(const btTransform& worldTrans)
	{
		//Update the node.
		mNodeInterface->SetDerivedOrientation(Convert(worldTrans.getRotation()));
		mNodeInterface->SetDerivedPosition(Convert(worldTrans.getOrigin()));
	}
}
