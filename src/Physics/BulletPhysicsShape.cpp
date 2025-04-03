#include <echo/Types.h>
#include <echo/Physics/BulletPhysicsShape.h>
#include <BulletCollision/CollisionShapes/btCollisionShape.h>
#include <BulletCollision/CollisionShapes/btCompoundShape.h>

namespace Echo
{
	BulletPhysicsShape::BulletPhysicsShape(btCollisionShape* shape) : mShape(shape), mDeleteChildrenIfCompoundShape(true)
	{
		assert(mShape && "mShape should not be null");
	}

	BulletPhysicsShape::BulletPhysicsShape() : mShape(0)
	{
	}

	BulletPhysicsShape::~BulletPhysicsShape()
	{
		if(mDeleteChildrenIfCompoundShape)
		{
			btCompoundShape* compoundShape = dynamic_cast<btCompoundShape*>(mShape);
			if(compoundShape)
			{
				Size numberOfChildren = compoundShape->getNumChildShapes();
				for(Size i=0;i<numberOfChildren;++i)
				{
					delete compoundShape->getChildShape(i);
				}
			}
		}
		delete mShape;
	}
	
	void BulletPhysicsShape::StoreSubShape(shared_ptr<BulletPhysicsShape> shape)
	{
		mSubShapes.push_back(shape);
	}
}
