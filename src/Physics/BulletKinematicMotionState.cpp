#include <echo/Types.h>
#include <echo/Physics/BulletKinematicMotionState.h>
#include <echo/Physics/BulletTypeConverters.h>
#include <echo/Graphics/NodeInterface.h>

namespace Echo
{
	BulletKinematicMotionState::BulletKinematicMotionState(BulletRigidBody& body, shared_ptr<NodeInterface> nodeInterface) :
		BulletMotionState(body,nodeInterface)
	{
		assert(nodeInterface && "nodeInterface must not be null");
	}
	
	BulletKinematicMotionState::~BulletKinematicMotionState()
	{
	}

	//Bullet overrides
	void BulletKinematicMotionState::getWorldTransform(btTransform& worldTrans ) const
	{
		worldTrans.setRotation(Convert(mNodeInterface->GetDerivedOrientation()));
		worldTrans.setOrigin(Convert(mNodeInterface->GetDerivedPosition()));
	}

	void BulletKinematicMotionState::setWorldTransform(const btTransform&)
	{
		//Kinematic states don't modify the node.
	}
}
