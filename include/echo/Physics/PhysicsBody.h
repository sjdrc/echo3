#ifndef _ECHOPHYSICSBODY_H_
#define _ECHOPHYSICSBODY_H_

#include <echo/Types.h>
#include <echo/Chrono/Chrono.h>
#include <echo/Maths/Vector3.h>
#include <echo/cpp/functional>
#include <map>
#include <list>

namespace Echo
{
	class PhysicsShape;
	class MotionState;
	class PhysicsWorld;
	class CollisionResult;
	
	/**
	 * PhysicsBody.
	 */
	class PhysicsBody
	{
	public:
		/**
		 * Collide callback
		 * @param this object
		 * @param the other object
		 * @param Information about the collision. The collision result is temporary
		 * so references to it should not be stored.
		 */
		typedef function<void(PhysicsBody&, PhysicsBody&, const CollisionResult&)> CollideCallback;
		
		PhysicsBody(shared_ptr<PhysicsWorld> world);
		virtual ~PhysicsBody();
		
		shared_ptr<PhysicsWorld> GetWorld() const {return mWorld;}
		
		virtual shared_ptr<MotionState> GetMotionState() const = 0;
		virtual shared_ptr<PhysicsShape> GetShape() const = 0;
		
		virtual Vector3 GetPosition() const = 0;
		virtual void SetPosition(const Vector3& position) = 0;
		virtual Quaternion GetOrientation() const = 0;
		virtual void SetOrientation(const Quaternion& orientation) = 0;
		virtual bool IsInWorld() const = 0;
		virtual void SetGravity(const Vector3& acceleration) = 0;
		virtual Vector3 GetGravity() const = 0;
		virtual void SetDamping(Scalar linearDamping, Scalar angularDamping) = 0;
		virtual Scalar GetLinearDamping() const = 0;
		virtual Scalar GetAngularDamping() const = 0;
		virtual Scalar GetLinearSleepingThreshold() const = 0;
		virtual Scalar GetAngularSleepingThreshold() const = 0;
		virtual void ApplyDamping(Seconds timeStep) const = 0;
		virtual void SetMassProperties(Scalar mass, const Vector3& inertia) = 0;
		virtual void SetCentreOfMass(const Vector3& centreOfMass) = 0;
		virtual void ApplyCentralForce(const Vector3& force) = 0;
		virtual Vector3 GetTotalForce() const = 0;
		virtual Vector3 GetTotalTorque() const = 0;
		virtual void SetSleepingThresholds(Scalar linear,Scalar angular) = 0;
		virtual void ApplyTorque(const Vector3& torque) = 0;
		virtual void ApplyForce(const Vector3& force, const Vector3& relativePosition) = 0;
		virtual void ApplyCentralImpulse(const Vector3& impulse) = 0;
		virtual void ApplyTorqueImpulse(const Vector3& torque) = 0;
		virtual void ApplyImpulse(const Vector3& impulse, const Vector3& relativePosition) = 0;
		virtual void ClearForces() = 0;
		virtual Vector3 GetCentreOfMass() const = 0;
		virtual Vector3 GetLinearVelocity() const = 0;
		virtual Vector3 GetAngularVelocity() const = 0;
		virtual void SetLinearVelocity(const Vector3& linearVelocity) = 0;
		virtual void SetAngularVelocity(const Vector3& angularVelocity) = 0;
		virtual Vector3 GetVelocityInLocalPoint(const Vector3& relativePosition) const = 0;
		virtual AxisAlignedBox GetAxisAlignedBox() const = 0;
		virtual void SetDeactivationEnabled(bool deactivationEnabled) = 0;
		
		/**
		 * Set the name of this object.
		 * @param name The name.
		 */
		void SetName(const std::string& name){mName=name;}

		/**
		 * Get the name of the object.
		 * The font name isn't derived from the font data file, it is obtained from the .font meta
		 * data or Font list file.
		 * @return the font name.
		 */
		const std::string& GetName() const {return mName;}
		
		/**
		 * Implementations will call this when collisions with other bodies occur.
		 * The default implementation will call any registered CollideCallbacks.
		 * - Per object Callbacks are processed first, followed by general collide callbacks.
		 * @param other The other PhysicsBody involved in the collision.
		 * @param collisionResult Additional information about the collision.
		 */
		virtual void OnCollide(PhysicsBody& other, const CollisionResult& collisionResult);

		/**
		 * Register a collide callback.
		 * General callbacks are called for all collisions.
		 * @param callbackID The callback ID, used to identify a list of callbacks and to be used to remove
		 * all callbacks associated with a the id. Use RegisterGeneralCollideCallbackT to avoid id
		 * conflicts.
		 * @param callback The callback.
		 */
		void RegisterGeneralCollideCallback(const std::string& callbackID, CollideCallback callback);

		/**
		 * Register a collide callback for a named object.
		 * @param id The callback ID, used to identify a list of callbacks and to be used to remove
		 * all callbacks associated with a the id. Use RegisterObjectCollideCallbackT to avoid id
		 * conflicts.
		 * @param objectName The name of the object you're interested in events about. This is the
		 * name of the other object.
		 * @param callback The callback.
		 */
		void RegisterObjectCollideCallback(const std::string& id, const std::string& objectName, CollideCallback callback);
		
		/**
		 * Register a collide callback using an object as the identifier.
		 * General callbacks are called for all collisions.
		 * The method just takes the object address to use as the callbackID.
		 * @param object The object to associate with the callback. Use the same object to
		 * deregister callback.
		 * @param callback The callback.
		 */
		template< typename T > 
		void RegisterGeneralCollideCallbackT(const T& object, CollideCallback callback)
		{
			std::stringstream ss; ss << (reinterpret_cast<Size>(&object));
			RegisterGeneralCollideCallback(ss.str(), callback);
		}
		
		/**
		 * Register a collide callback for a named object using an object as the identifier.
		 * The method just takes the object address to use as the callbackID.
		 * @param object The object to associate with the callback. Use the same object to
		 * deregister callback.
		 * @param callback The callback.
		 */
		template< typename T >
		void RegisterObjectCollideCallbackT(const T& object, const std::string& objectName,  CollideCallback callback)
		{
			std::stringstream ss; ss << (reinterpret_cast<Size>(&object));
			RegisterObjectCollideCallback(ss.str(), objectName, callback);
		}
		
		/**
		 * Deregister collide callbacks for specific objects and a given ID.
		 * @param id the callback list id.
		 * @param objectName the name of the other object to remove callbacks about.
		 */
		void DeregisterObjectCollideCallbacks(const std::string& id, const std::string& objectName);
		
		/**
		 * Deregister General collide callbacks for a given id.
		 * @param id the callback list id.
		 */
		void DeregisterGeneralCollideCallbacks(const std::string& id);
		
		/**
		 * Deregister all callbacks for a given id.
		 * This includes General and Object callbacks.
		 * @param id list Id used when registering.
		 */
		void DeregisterAllCollideCallbacks(const std::string& id);

		/**
		 * Deregister all collide callbacks.
		 */
		void DeregisterAllCollideCallbacks();
		
		template< typename T >
		void DeregisterObjectCollideCallbacksT(const T& object, const std::string& objectName)
		{
			std::stringstream ss; ss << (reinterpret_cast<Size>(&object));
			DeregisterObjectCollideCallbacks(ss.str(), objectName);
		}
		
		/**
		 * Deregister a collide callback using an object as the identifier.
		 * The method just takes the object address to use as the callbackID.
		 * @param object The object to associate with the callback. Use the same object to
		 * deregister callback.
		 * @param callback The callback.
		 */
		template< typename T > 
		void DeregisterGeneralCollideCallbacksT(const T& object)
		{
			std::stringstream ss; ss << (reinterpret_cast<Size>(&object));
			DeregisterGeneralCollideCallbacks(ss.str());
		}
	private:
		shared_ptr<PhysicsWorld> mWorld;
		std::string mName;
		typedef std::list< CollideCallback > CallbackList;
		typedef std::map<std::string, CallbackList > NamedCallbackLists;
		typedef std::pair<const std::string, CallbackList > NamedCallbacksPair;
		typedef std::map< std::string, NamedCallbackLists > ObjectCollideCallbacksMap;
		ObjectCollideCallbacksMap mPerObjectCollideCallbacks;
		NamedCallbackLists mGeneralCollideCallbacks;
	};
}
#endif
