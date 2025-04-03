#ifndef _ECHONODE_H_
#define _ECHONODE_H_

#include <echo/Types.h>
#include <echo/Maths/Quaternion.h>
#include <echo/Maths/Matrix4.h>
#include <echo/Graphics/NodeInterface.h>
#include <echo/Kernel/Mutex.h>
#include <string>
#include <set>

namespace Echo
{
	class Node : public NodeInterface
	{
	public:
		Node(const Vector3& position = Vector3::ZERO,
			 const Quaternion& orientation = Quaternion::IDENTITY,
			 const Vector3& scale = Vector3::UNIT_SCALE);
		virtual ~Node();
		
		/**
		 * Copy constructor.
		 * @see Node::operator=() for documentation on the behaviour.
		 */
		Node(const Node& other);
		
		/**
		 * Assignment operator.
		 * Since Node's are designed to be a hierarchical structure we cannot easily copy the
		 * structure when assigning one Node to another without asking questions about what
		 * the intent is. Do you intend to copy the node, give it the same parent and make
		 * copies of all of the child objects? Or do you want some other combination of these
		 * or something else entirely?
		 * 
		 * Because of the ambiguity of intent when it comes to copying we set the convention
		 * that assignment operators in Node hierarchies will only be required to copy the
		 * object's members to make that object equivalent but not copy any child hierarchies
		 * or add it to any parent.
		 * 
		 * Here is an example:
		 * \code{.cpp}
		 * 
		 * SomethingThatTakesNodes nodeTaker;
		 * Node a;
		 * a.SetPosition(Vector3(100,100,100));
		 * Node b;
		 * a.AddChild(b);
		 * nodeTaker.AddNode(a);
		 * 
		 * Node c;
		 * a.SetPosition(Vector3(-100,-100,-100));
		 * c = a;
		 * 
		 * \endcode
		 * 
		 * - The position of c is modified to match that of a, along with other member variables.
		 * - c does not have b as a child.
		 * - c does not have a copy of b as a child.
		 * - c has not been added to nodeTaker (this would be complicated to make consistent anyway)
		 * 
		 * The same applies for derived classes.
		 * 
		 * Where one might like to make a full copy of an object you should look to see if the object
		 * you want to copy has a Clone() method.
		 * 
		 * These same rules apply for copy constructors.
		 * 
		 * @param rhs The other Node.
		 * @return 
		 */
		Node& operator=(const Node& rhs);
		
		const std::string& GetName() const;
		void SetName(const std::string& val);

		virtual const Quaternion& GetOrientation() const;
		virtual const Quaternion& GetDerivedOrientation() const;
		virtual const Vector3& GetPosition() const;
		virtual const Vector3& GetDerivedPosition() const;
		virtual const Vector3& GetScale() const;
		virtual const Vector3& GetDerivedScale() const;
		
		virtual void SetDerivedPosition(const Vector3& derivedPosition);
		virtual void SetDerivedScale(const Vector3& derivedScale);
		virtual void SetDerivedOrientation(const Quaternion& derivedOrientation);
		
		/**
		 * Get the transform of this node.
		 * The transform returned reflects the derived position, scale and orientation.
		 * @return transform matrix for the node.
		 */
		const Matrix4& GetTransform() const;

		/**
		 * Get the local transform of this node.
		 * The transform returned reflects the local position, scale and orientation.
		 * @return 
		 */
		const Matrix4& GetLocalTransform() const;
		const Quaternion& GetInitialOrientation() const;
		const Vector3& GetInitialPosition() const;
		const Vector3& GetInitialScale() const;
		bool GetInheritOrientation() const;
		bool GetInheritScale() const;

		virtual void SetOrientation(const Quaternion& val);
		virtual void SetOrientationEuler(Radian yaw, Radian pitch, Radian roll);
		
		/**
		 * Set the position of the node.
		 */
		virtual void SetPosition(const Vector3& val);
		/**
		 * Set the scale of the node.
		 * You can set a non-uniform scale.
		 */
		virtual void SetScale(const Vector3& val);
		/**
		 * Set the position of the node.
		 */
		virtual void SetPosition(f32 x, f32 y, f32 z);
		/**
		 * Set a uniform scale.
		 * @param scale
		 */
		virtual void SetUniformScale(f32 scale);
		void SetTransform(const Matrix4& val);
		void SetInitialOrientation(const Quaternion& val);
		void SetInitialPosition(const Vector3& val);
		void SetInitialScale(const Vector3& val);
		void SetInheritOrientation(bool val);
		void SetInheritScale(bool val);
		
		virtual void Scale(const Vector3& s);
		virtual void Translate(const Vector3& t);
		virtual void TranslateRelative(const Vector3& t);
		virtual void Rotate(const Vector3& axis, const Radian& angle);
		virtual void Rotate(const Quaternion& rotation);

		virtual void Yaw(Radian angle);
		virtual void Pitch(Radian angle);
		virtual void Roll(Radian angle);

		//SetParent() CHANGES the parent of the node.
		void SetParent(Node* node);
		Node* GetParent() const;
		void AddChild(Node* node, bool takeOwnerShip = false);
		void AddChild(Node& node);
		void AddChild(shared_ptr<Node> node);
		shared_ptr<Node> CreateChild(const std::string& name);
		shared_ptr<Node> CreateChild(const std::string& name, const Vector3& position = Vector3::ZERO, const Quaternion& rotation = Quaternion::IDENTITY);
		bool RemoveChild(const std::string& name);
		bool RemoveChild(Node& node);
		void RemoveAllChildren();

		void _Update(bool updateChildren, bool parentHasChanged) const;

		/**
		 * Attempt to find a child node by name.
		 * The object's child list is searched first and an object with matching name is returned if found,
		 * otherwise if searchChildren is true then the method is called on each of the child nodes.
		 * @param name The name of the node to search for.
		 * @param searchChildren If true and this instance does not have a child node matching name then
		 * the method is called on each child.
		 * @return A node whose name matches the passed parameter.
		 */
		Node* FindChild(const std::string& name, bool searchChildren) const;
		
		/**
		 * Attempt to find a child node by name that was added as a shared pointer.
		 * @see FindChild().
		 */
		shared_ptr<Node> FindSharedChild( const std::string& name, bool searchChildren) const;

		/**
		 * Convenience down cast template method for finding nodes.
		 */
		template< class T >
		T* FindChild(const std::string& name, bool searchChildren) const
		{
			return dynamic_cast<T*>(FindChild(name, searchChildren));
		}

		/**
		 * Convenience down cast template method for finding shared nodes.
		 */
		template< class T >
		shared_ptr<T> FindSharedChild(const std::string& name, bool searchChildren) const
		{
			return dynamic_pointer_cast< T >(FindSharedChild(name, searchChildren));
		}

		//Reset the node to the initial position
		void Reset();
	protected:
		std::set< Node* > mChildNodes;
		mutable std::set< const Node* > mChildrenToUpdate;
		std::set< shared_ptr<Node> > mSharedChildren;
		Node* mParent;

		Quaternion mOrientation;
		mutable Quaternion mDerivedOrientation;
		Vector3 mPosition;
		mutable Vector3 mDerivedPosition;
		Vector3 mScale;
		mutable Vector3 mDerivedScale;
		mutable Matrix4 mTransform;
		mutable Matrix4 mLocalTransform;
		Quaternion mInitialOrientation;
		Vector3 mInitialPosition;
		Vector3 mInitialScale;
		bool mInheritOrientation;
		bool mInheritScale;

		mutable bool mDerivedPropertiesOutOfDate;
		mutable bool mNotifiedParentOfUpdate;
		mutable bool mNeedChildUpdate;
		mutable bool mTransformOutOfDate;
		mutable bool mLocalTransformOutOfDate;

		mutable Mutex mDerivedPropertiesMutex;
		mutable Mutex mTransformMutex;
		mutable Mutex mLocalTransformMutex;
		std::string mName;
		
		void ApplyDefaults(const Vector3& position, const Quaternion& orientation, const Vector3& scale);

		/**
		 * Update the derived position, scale and orientation if needed.
		 * This method updates the cached derived member values which is why it is const.
		 * This method will only update if the parameters are out of date.
		 */
		void UpdateDerivedProperties() const;

		virtual void OnParentSet(Node* node);
		virtual void NeedUpdate(bool forceParent = false) const;
		virtual void ChildRequestUpdate(const Node& node, bool forceParentUpdate) const;
		virtual void CancelChildUpdate(Node& node);
		virtual void ChildAdded(Node& node);
		virtual void ChildRemoved(Node& node);
	};
}
#endif 
