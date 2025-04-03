#ifndef _ECHOBULLETFACTORY_H_
#define _ECHOBULLETFACTORY_H_

#include <echo/Physics/PhysicsFactory.h>
#include <echo/Maths/AxisAlignedBox.h>
#include <LinearMath/btIDebugDraw.h>
#include <echo/Physics/BulletPhysicsShape.h>
#include <echo/Physics/BulletRigidBody.h>
#include <echo/Physics/BulletPhysicsShape.h>
#include <echo/Physics/BulletPhysicsWorld.h>
#include <echo/Physics/BulletDynamicMotionState.h>
#include <echo/Physics/BulletKinematicMotionState.h>

namespace Echo
{

	/**
	 * A factory class that makes creating physics worlds easier.
	 * This is a higher level interface to building a physics world. You can create a physics world
	 * manually by using bullet directly but keep in mind you'll also need to manage the bullet
	 * objects memory appropriately. The Echo Bullet classes encapsulate bullet objects for common
	 * interfaces and to keep track of the bullet objects so you do not have to.
	 * 
	 * This factory will create a world if you do not provide it one during construction. This
	 * provides convenience as you do not explicitly need to create a PhysicsWorld but if you
	 * want to instantiate a factory to create objects for an existing world you can pass one in.
	 * Once used, you can destroy a BulletFactory safely. If any PhysicsBodies have been created
	 * for the world they will have a reference to it, so as long as you keep the PhysicsBody
	 * instances the world will continue to exist. Although for any simulation, something will
	 * need a reference to the world in order to update the simulation (usually a TaskManager since
	 * PhysicsWorld is a TaskGroup).
	 */
	class BulletFactory : public PhysicsFactory
	{
	public:
		/**
		 * Default constructor.
		 * Since functions depend on a physics world if one isn't defined a default one will
		 * be created.
		 * @see CreateDefaultDynamicsWorld() for parameter information.
		 */
		BulletFactory(AxisAlignedBox worldExtent = AxisAlignedBox(Vector3(-1000, -1000, -1000), Vector3(1000, 1000, 1000)),
																		int maxProxies=8192);
		BulletFactory(shared_ptr<BulletPhysicsWorld> world);
		~BulletFactory();
		
		/**
		 * Create a box shape.
		 * @param extents The size of the box, e.g. Vector(1,1,1) creates a unit cube.
		 * @return A BulletPhysicsShape containing a btBoxShape.
		 */
		shared_ptr<PhysicsShape> CreateBox(Vector3 extents) override;
		
		/**
		 * Create a sphere shape.
		 * @param radius Radius of the sphere.
		 * @return A BulletPhysicsShape containing a btSphereShape.
		 */
		shared_ptr<PhysicsShape> CreateSphere(Scalar radius) override;

		/**
		 * Create a static plane shape.
		 * The shape should be used on a static body (i.e. mass of 0, non moving).
		 * @param planeNormal The normal that defines the face of the plane, Vector3(0,1,0) is a plane facing up.
		 * @param planeConstant The constant that defines where the plan is from the origin.
		 * @return A BulletPhysicsShape containing a btStaticPlaneShape.
		 */
		shared_ptr<PhysicsShape> CreateStaticPlane(Vector3 planeNormal, Scalar planeConstant) override;
		
		/**
		 * Create a capsule shape.
		 * The capsule is orientated along the Y axis.
		 * @param radius The radius of the capsule.
		 * @param height The height of the capsule from the centre of the two hemisphere ends, so the total
		 * height is height+2*radius.
		 * @return A BulletPhysicsShape containing a btCapsuleShape;
		 */
		shared_ptr<PhysicsShape> CreateCapsule(Scalar radius, Scalar height) override;

		/**
		 * Create a capsule along the X axis.
		 * @see CreateCapsule().
		 * @return A BulletPhysicsShape containing a btCapsuleShapeX;
		*/
		shared_ptr<PhysicsShape> CreateCapsuleX(Scalar radius, Scalar height) override;

		/**
		 * Create a capsule along the Z axis.
		 * @see CreateCapsule().
		 * @return A BulletPhysicsShape containing a btCapsuleShapeZ;
		*/
		shared_ptr<PhysicsShape> CreateCapsuleZ(Scalar radius, Scalar height) override;

		/**
		 * Create a collision mesh from the provided mesh.
		 * @param mesh The mesh to copy data from.
		 * @return A BulletPhysicsShape 
		 */
		shared_ptr<PhysicsShape> CreateMesh(shared_ptr<Mesh> mesh, bool usedForDynamics) override;

		/**
		 * Create a default dynamics world.
		 * A default world is a btDiscreteDynamicsWorld constructed using:
		 *	- btDefaultCollisionConfiguration
		 *  - btCollisionDispatcher
		 *  - btAxisSweep3
		 *  - btSequentialImpulseConstraintSolver
		 * @param worldExtent world AABB extent.
		 * @param maxProxies The maximum number of objects that can be in the world.
		 * @return A BulletPhysicsWorld.
		 */
		shared_ptr<PhysicsWorld> CreateDefaulDynamicsWorld(AxisAlignedBox worldExtent = AxisAlignedBox(Vector3(-1000, -1000, -1000), Vector3(1000, 1000, 1000)),
																		int maxProxies=8192) override;
		
		/**
		 * Create a rigid body.
		 * This method will create a body for the specified entity using the specified shape. A dynamic
		 * motion state will be created to link the SceneEntity with the body.
		 * The body created will update the entity's position.
		 * @note Once this method returns, if addToWorld is not true, then the object will need to be stored
		 * somewhere otherwise it will be deleted.
		 * @param mass The mass of the object, 0 is static (i.e. does not move).
		 * @param forEntity The entity to link this body with.
		 * @param shape The shape to use.
		 * @param addToWorld if true the body will be added to the world, false if you plan to add it later.
		 * @return The newly created rigid body.
		 */
		shared_ptr<PhysicsBody> CreateRigidBody(Scalar mass, shared_ptr<SceneEntity> forEntity, shared_ptr<PhysicsShape> shape, bool addToWorld = true) override;
		
		/**
		 * Create a rigid body for the scene.
		 * Similar to the CreateRigidBody() that includes a SceneEntity parameter, but does not create a
		 * motion state to link an entity with a physics body. This method can be used if you want to
		 * create an "invisible" object in the scene.
		 * @note Once this method returns, if addToWorld is not true, then the object will need to be stored
		 * somewhere otherwise it will be deleted.
		 * @param mass The mass of the object, 0 is static (i.e. does not move).
		 * @param shape The shape to use.
		 * @param addToWorld if true the body will be added to the world, false if you plan to add it later.
		 * @return The newly created rigid body.
		 */
		shared_ptr<PhysicsBody> CreateRigidBody(Scalar mass, shared_ptr<PhysicsShape> shape, bool addToWorld = true) override;

		/**
		 * Create a dynamic motion state for an entity and body.
		 * A motion state is essentially the link between a physics object and a SceneEntity. The Entity
		 * should be given a mesh that matches the body's shape you've provided in order for your
		 * simulation to look correct. For example, you could create a body with a unit cube shape but if
		 * the mesh is a sphere with a radius of 10 then the sphere will behave contrary to how people
		 * might expect.
		 * @param forEntity The entity that will be linked to the body.
		 * @param body The body that will be linked with the entity.
		 * @param activate If true, the motion state will be activated. A motion state can be deactivated
		 * to stop the SceneEntity updating to the physics object.
		 * @return A new motion state.
		 */
		shared_ptr<MotionState> CreateDynamicMotionState(shared_ptr<SceneEntity> forEntity, shared_ptr<PhysicsBody> body, bool activate = true) override;
		
		/**
		 * Create a SceneEntity setup with a simple debug drawer.
		 * This method sets the world's debug drawer.
		 * @param debugMod The debug drawing mode. See BulletMeshDebugDrawer and btIDebugDraw for what is supported.
		 * @return A scene entity that, when added to a scene, will draw the physics objects in the scene.
		 */
		shared_ptr<SceneEntity> CreateDebugSceneEntity(int debugMod = (btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_DrawAabb)) override;
		
		/**
		 * Get the bullet physics world.
		 * @return The bullet physics world.
		 */
		shared_ptr<BulletPhysicsWorld> GetWorld() {return mWorld;}
	private:
		shared_ptr<BulletPhysicsWorld> mWorld;
		/**
		 * Internal method used to create a dynamics world to avoid a cast in the constructor.
		 * @see CreateDefaulDynamicsWorld().
		 */
		virtual shared_ptr<BulletPhysicsWorld> _CreateDefaulDynamicsWorld(AxisAlignedBox worldExtent, int maxProxies);
	};
}
#endif
