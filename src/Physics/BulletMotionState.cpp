#include <echo/Physics/BulletMotionState.h>
#include <echo/Physics/BulletRigidBody.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>

namespace Echo
{
	BulletMotionState::BulletMotionState(BulletRigidBody& body, shared_ptr<NodeInterface> nodeInterface) :
		mBody(body),
		mNodeInterface(nodeInterface)
	{
		assert(nodeInterface && "nodeInterface must not be null");
	}
	
	BulletMotionState::~BulletMotionState()
	{
		if(mBody.GetBulletBody()->getMotionState()==this)
		{
			mBody.GetBulletBody()->setMotionState(nullptr);
		}
	}

	void BulletMotionState::Activate()
	{
		mBody.GetBulletBody()->setMotionState(this);
	}
	
	void BulletMotionState::Deactivate()
	{
		if(mBody.GetBulletBody()->getMotionState()==this)
		{
			mBody.GetBulletBody()->setMotionState(nullptr);
		}
	}
	
	BulletRigidBody& BulletMotionState::GetBody()
	{
		return mBody;
	}

	shared_ptr<NodeInterface> BulletMotionState::GetNodeInterface() const
	{
		return mNodeInterface;
	}
}
