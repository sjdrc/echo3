#ifndef _ECHOMOTIONSTATE_H_
#define _ECHOMOTIONSTATE_H_

#include <echo/Types.h>

namespace Echo
{
	class PhysicsBody;
	class NodeInterface;
	
	/**
	 * MotionState.
	 * A motion state is used to synchronise the physics object with Echo.
	 * Typical types are kinematic and dynamic.
	 *	- Kinematic motion states get a transform from Echo and updates the physics world.
	 *	- Dynamic motion states get a transform from the physics world and update Echo.
	 * You will need an API specific motion state.
	 */
	class MotionState
	{
	public:
		MotionState();
		virtual ~MotionState();

		/**
		 * Activate the motion state.
		 * Cause the body to use this motion state. Usually a body can only use a single
		 * motion state at once so activating and deactivating allows the motion states
		 * to set update which one is in use.
		 */
		virtual void Activate() = 0;

		/**
		 * Deactivate the motion state.
		 */
		virtual void Deactivate() = 0;
		virtual PhysicsBody& GetBody() = 0;
		virtual shared_ptr<NodeInterface> GetNodeInterface() const = 0;
	};
}
#endif
