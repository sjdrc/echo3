#ifndef _ECHOPHYSICSWORLD_H_
#define _ECHOPHYSICSWORLD_H_

#include <echo/Kernel/TaskGroup.h>
#include <map>

namespace Echo
{
	class PhysicsBody;

	class PhysicsWorld : public TaskGroup
	{
	public:
		PhysicsWorld();
		virtual ~PhysicsWorld();

		virtual void AddBody(shared_ptr<PhysicsBody> body) = 0;
		virtual void RemoveBody(shared_ptr<PhysicsBody> body) = 0;
	};
}
#endif
