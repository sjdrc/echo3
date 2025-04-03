#ifndef _ECHOBULLETPHYSICSSHAPE_H_
#define _ECHOBULLETPHYSICSSHAPE_H_

#include <echo/Types.h>
#include <echo/Physics/PhysicsShape.h>
#include <vector>

class btCollisionShape;

namespace Echo
{
	/**
	 * BulletPhysicsShape.
	 * Manages the lifetime of a bullet collision shape object. BulletPhysicsShape instances own
	 * the shape they are constructed with.
	 */
	class BulletPhysicsShape : public PhysicsShape
	{
	public:
		/**
		 * Constructor
		 * @param shape Must not be null
		 */
		BulletPhysicsShape(btCollisionShape* shape);
		virtual ~BulletPhysicsShape();
		btCollisionShape* GetBulletShape() const
		{
			return mShape;
		}
		
		/**
		 * Take ownership of the shape.
		 * This sets the internal pointer to the bullet shape to null. After this method
		 * returns the caller will be responsible for clean up of the btCollisionShape.
		 * @note If the shape is a btCompoundShape you may be responsible for cleaning
		 * up the sub shape memory too. Check with GetDeleteChildrenIfCompoundShape().
		 */
		btCollisionShape* TakeShape()
		{
			btCollisionShape* shape = mShape;
			mShape = nullptr;
			return shape;
		}
		
		/**
		 * Set whether or not to delete child shapes if the provided collision shape is a
		 * btCompoundShape. This is for convenience since bullet doesn't take ownership of
		 * shapes and it would be less convenient to separately track shapes after you've
		 * created an instance of BulletPhysicsShape since it takes ownership of the shape.
		 * 
		 * This option true by default and is provided since you may want to share some
		 * shapes in multiple compound shapes.
		 */
		void SetDeleteChildrenIfCompoundShape(bool deleteChildrenIfCompoundShape)
		{
			mDeleteChildrenIfCompoundShape = deleteChildrenIfCompoundShape;
		}
		
		/**
		 * Get whether or not to delete child shapes in btCompoundShape
		 * @see SetDeleteChildrenIfCompoundShape
		 */
		bool GetDeleteChildrenIfCompoundShape() const
		{
			return mDeleteChildrenIfCompoundShape;
		}
		
		/**
		 * Store a sub shape within this shape.
		 * BulletPhysicsShape is a wrapper around btCollisionShape to manage ownership of
		 * the Bullet instance. If you are building a btCompoundShape* from shapes that
		 * require the management of additional data, such as BulletMeshPhysicsShape,
		 * which maintains a reference to the Mesh object after creating a bullet object
		 * then the release of this reference needs to occur when this object is deleted.
		 * Since we don't always know what references we need to hold on to and as such
		 * do not have a base type to store, the corresponding BulletPhysicsShape references
		 * can be stored as sub shapes here. You can use TakeShape() to take the ownership
		 * of the btCollisionShape* then add it to the btCompoundShape before creating this
		 * object. This will manage the memory correctly. For example:
		 * 
		 *	btCompoundShape* compoundShape = new btCompoundShape();
		 *	shared_ptr<BulletPhysicsShape> finalShape(compoundShape);
		 *	shared_ptr<BulletMeshPhysicsShape> meshShape(new BulletMeshPhysicsShape(someMesh,BulletMeshPhysicsShape::Modes::DYNAMIC));
		 *	// meshShape maintains a reference to someMesh to ensure the mesh data is around for the life of the shape
		 *	btTransform transform;
		 *	transform.setIdentity();
		 *	compoundShape->addChildShape(transform,meshShape->TakeShape());
		 *	// Store meshShape so it sticks around and ultimately the mesh data.
		 *	compoundShape->StoreSubShape(meshShape);
		 * 
		 *	// Now we've finished the object shuffle, when finalShape is deleted, the compound
		 *	// shape will be cleaned up properly (providing mDeleteChildrenIfCompoundShape is true)
		 *	// as well as the mesh data not being deleted unnecessarily.
		 */
		void StoreSubShape(shared_ptr<BulletPhysicsShape> shape);
	protected:
		/**
		 * Specialisations are allowed to set the shape a bit later if needed.
		 */
		BulletPhysicsShape();

		/**
		 * If any specialisation needs to do some initialisation before deciding 
		 * on what collision shape to use they can call this method when ready.
		 * The shape MUST be set before this instance is used.
		 * @param shape The collision shape.
		 */
		void SetCollisionShape(btCollisionShape* shape) {mShape=shape;}
	private:		
		btCollisionShape* mShape;
		std::vector< shared_ptr<BulletPhysicsShape> > mSubShapes;
		bool mDeleteChildrenIfCompoundShape;
	};
}
#endif
