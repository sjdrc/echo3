#ifndef _ECHONODEINTERFACE_H_
#define _ECHONODEINTERFACE_H_

#include <echo/Maths/Vector3.h>
#include <echo/Util/InheritableEnableSharedFromThis.h>
namespace Echo
{
	//NodeInterface provides an interface to a node object.
	class NodeInterface : public InheritableEnableSharedFromThis<NodeInterface>
	{
	public:

		NodeInterface()
		{
		}

		virtual ~NodeInterface()
		{
		}

		//Derived accessors
		virtual const Vector3& GetDerivedPosition() const = 0;
		virtual const Vector3& GetDerivedScale() const = 0;
		virtual const Quaternion& GetDerivedOrientation() const = 0;

		//Derived setters
		virtual void SetDerivedPosition(const Vector3&) = 0;
		virtual void SetDerivedScale(const Vector3&) = 0;
		virtual void SetDerivedOrientation(const Quaternion&) = 0;

		//Local accessors
		virtual void SetPosition(const Vector3& position) = 0;
		virtual void SetScale(const Vector3& scale) = 0;
		virtual void SetOrientation(const Quaternion& orientation) = 0;
		virtual const Vector3& GetPosition() const = 0;
		virtual const Vector3& GetScale() const = 0;
		virtual const Quaternion& GetOrientation() const = 0;

	};
}
#endif //_ECHONODEINTERFACE_H_
