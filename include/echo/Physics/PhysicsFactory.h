#ifndef PHYSICSFACTORY_H
#define PHYSICSFACTORY_H

#include <echo/Types.h>
#include <echo/Maths/AxisAlignedBox.h>

namespace Echo
{
	class SceneEntity;
	class PhysicsShape;
	class PhysicsBody;
	class PhysicsWorld;
	class MotionState;
	class Mesh;
	
	/**
	 * An abstract factory for creating physics worlds and objects.
	 */
	class PhysicsFactory
	{
	public:
		PhysicsFactory(){}
		virtual ~PhysicsFactory(){}
		
		/**
		 * Create a box shape.
		 * @param extents The size of the box, e.g. Vector(1,1,1) creates a unit cube.
		 * @return A PhysicsShape as whatever specialisation is required for a box.
		 */
		virtual shared_ptr<PhysicsShape> CreateBox(Vector3 extents) = 0;
		
		/**
		 * Create a sphere shape.
		 * @param radius Radius of the sphere.
		 * @return A PhysicsShape as whatever specialisation is required for a sphere.
		 */
		virtual shared_ptr<PhysicsShape> CreateSphere(Scalar radius) = 0;

		/**
		 * Create a static plane shape.
		 * The shape should be used on a static body (i.e. mass of 0, non moving).
		 * @param planeNormal The normal that defines the face of the plane, Vector3(0,1,0) is a plane facing up.
		 * @param planeConstant The constant that defines where the plan is from the origin.
		 * @return A PhysicsShape as whatever specialisation is required for a plane.
		 */
		virtual shared_ptr<PhysicsShape> CreateStaticPlane(Vector3 planeNormal, Scalar planeConstant) = 0;
		
		/**
		 * Create a capsule shape.
		 * The capsule is orientated along the Y axis.
		 * @param radius The radius of the capsule.
		 * @param height The height of the capsule from the centre of the two hemisphere ends, so the total
		 * height is height+2*radius.
		 * @return A PhysicsShape as whatever specialisation is required for a capsule;
		 */
		virtual shared_ptr<PhysicsShape> CreateCapsule(Scalar radius, Scalar height) = 0;

		/**
		 * Create a capsule along the X axis.
		 * @see CreateCapsule().
		 * @return A PhysicsShape as whatever specialisation is required for a capsule orientated on the X axis.
		*/
		virtual shared_ptr<PhysicsShape> CreateCapsuleX(Scalar radius, Scalar height) = 0;

		/**
		 * Create a capsule along the Z axis.
		 * @see CreateCapsule().
		 * @return A PhysicsShape as whatever specialisation is required for a capsule orientated on the X axis.
		*/
		virtual shared_ptr<PhysicsShape> CreateCapsuleZ(Scalar radius, Scalar height) = 0;

		/**
		 * Create a collision shape from a Mesh.
		 * It is up to the implementation as to whether the mesh data is used directly or if it is copied.
		 * If the data is used directory the implementation will return a PhysicsShape whose implementation
		 * has a copy of the shared pointer.
		 * @param mesh The mesh to be used as a collision shape.
		 * @param usedForDynamics provide a hint to the implementation as to whether the shape will be added
		 * to a dynamic Physics body.
		 * @return A PhysicsShape as whatever specialisation is required for mesh.
		 */
		virtual shared_ptr<PhysicsShape> CreateMesh(shared_ptr<Mesh> mesh, bool usedForDynamics) = 0;
		
		/**
		 * Create a default dynamics world.
		 * @param worldExtent world AABB extent, there is no guarantee the implementation will use this.
		 * @param maxProxies The maximum number of objects that can be in the world, there is no guarantee the
		 * implementation will use this.
		 * @return A PhysicsWorld.
		 */
		virtual shared_ptr<PhysicsWorld> CreateDefaulDynamicsWorld(AxisAlignedBox worldExtent = AxisAlignedBox(Vector3(-1000, -1000, -1000), Vector3(1000, 1000, 1000)),
																		int maxProxies=8192) = 0;
		
		/**
		 * Create a rigid body.
		 * This method will create a body for the specified entity using the specified shape. A 
		 * DynamicMotionState will be created to link the SceneEntity with the body.
		 * The body created will update the entity's position.
		 * @note Once this method returns, if addToWorld is not true, then the object will need to be stored
		 * somewhere otherwise it will be deleted.
		 * @param mass The mass of the object, 0 is static (i.e. does not move).
		 * @param forEntity The entity to link this body with.
		 * @param shape The shape to use.
		 * @param addToWorld if true the body will be added to the world, false if you plan to add it later.
		 * @return The newly created rigid body.
		 */
		virtual shared_ptr<PhysicsBody> CreateRigidBody(Scalar mass, shared_ptr<SceneEntity> forEntity, shared_ptr<PhysicsShape> shape, bool addToWorld = true) = 0;
		
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
		virtual shared_ptr<PhysicsBody> CreateRigidBody(Scalar mass, shared_ptr<PhysicsShape> shape, bool addToWorld = true) = 0;

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
		virtual shared_ptr<MotionState> CreateDynamicMotionState(shared_ptr<SceneEntity> forEntity, shared_ptr<PhysicsBody> body, bool activate = true) = 0;
		
		/**
		 * Create a SceneEntity setup with a simple debug drawer.
		 * This method sets the world's debug drawer.
		 * @param debugMod The debug drawing mode, check the implementation details for flags (this needs
		 * further abstraction).
		 * @return A scene entity that, when added to a scene, will draw the physics objects in the scene.
		 */
		virtual shared_ptr<SceneEntity> CreateDebugSceneEntity(int debugMod) = 0;
	};

}
#endif

