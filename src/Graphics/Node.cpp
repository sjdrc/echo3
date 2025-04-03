#include <echo/Graphics/Node.h>
#include <echo/Kernel/ScopedLock.h>
#include <boost/foreach.hpp>

namespace Echo
{
	Node::Node(const Vector3& position, const Quaternion& orientation, const Vector3& scale)
	{
		ApplyDefaults(position, orientation,scale);
	}

	Node::~Node()
	{
		RemoveAllChildren();
		
		if(mParent)
		{
			mParent->RemoveChild(*this);
		}
	}
	
	Node::Node(const Node& other)
	{
		mParent = 0;
		*this = other;
	}

	Node& Node::operator=(const Node& rhs)
	{
		if(this==&rhs)
		{
			return *this;
		}
		//mChildNodes		// Leave.
		//mChildrenToUpdate // Leave.
		//mSharedChildren	// Leave.
		//mParent			// Leave.

		mOrientation = rhs.mOrientation;
		mDerivedOrientation = mOrientation;		//This will be properly calculated later.
		mPosition = rhs.mPosition;
		mDerivedPosition = Vector3::ZERO;		//This will be properly calculated later.
		mScale = rhs.mScale;
		mDerivedScale = Vector3::UNIT_SCALE;	//This will be properly calculated later.
		mTransform = Matrix4::IDENTITY;			//This will be properly calculated later.
		mLocalTransform = Matrix4::IDENTITY;	//This will be properly calculated later.
		mInitialOrientation = rhs.mInitialOrientation;
		mInitialPosition = rhs.mInitialPosition;
		mInitialScale = rhs.mInitialScale;
		mInheritOrientation = rhs.mInheritOrientation;
		mInheritScale = rhs.mInheritScale;

		mDerivedPropertiesOutOfDate = true;
		mNotifiedParentOfUpdate=false;
		mNeedChildUpdate=true;
		mTransformOutOfDate=true;
		mLocalTransformOutOfDate = true;
		mName = rhs.mName;
		return *this;
	}
	
	void Node::ApplyDefaults(const Vector3& position, const Quaternion& orientation, const Vector3& scale)
	{
		mParent=0;
		mOrientation = orientation;
		mDerivedOrientation = orientation;
		mPosition = position;
		mDerivedPosition = position;
		mScale = scale;
		mDerivedScale = scale;
		mTransform=Matrix4::IDENTITY;
		mInitialOrientation = orientation;
		mInitialPosition = position;
		mInitialScale = scale;
		mInheritOrientation=true;
		mInheritScale=true;

		mDerivedPropertiesOutOfDate=true;
		mNotifiedParentOfUpdate=false;
		mNeedChildUpdate=true;
		mTransformOutOfDate=true;
		mLocalTransformOutOfDate = true;
	}

	const std::string& Node::GetName() const
	{
		return mName;
	}

	void Node::SetName( const std::string& val )
	{
		mName = val;
	}

	const Quaternion& Node::GetOrientation() const
	{
		return mOrientation;
	}

	const Quaternion& Node::GetDerivedOrientation() const
	{
		UpdateDerivedProperties();
		return mDerivedOrientation;
	}

	const Vector3& Node::GetPosition() const
	{
		return mPosition;
	}

	const Vector3& Node::GetDerivedPosition() const
	{
		UpdateDerivedProperties();
		return mDerivedPosition;
	}

	const Vector3& Node::GetScale() const
	{
		return mScale;
	}

	const Vector3& Node::GetDerivedScale() const
	{
		UpdateDerivedProperties();
		return mDerivedScale;
	}
	
	void Node::SetDerivedPosition(const Vector3& derivedPosition)
	{
		if(mParent)
		{
			SetPosition(derivedPosition - mParent->GetDerivedPosition());
		}else
		{
			SetPosition(derivedPosition);
		}
	}
	
	void Node::SetDerivedScale(const Vector3& derivedScale)
	{
		if(mParent && !mParent->GetDerivedScale().HasZeroComponent())
		{
			SetScale(derivedScale / mParent->GetDerivedScale());
		}else
		{
			SetScale(derivedScale);
		}
	}
	
	void Node::SetDerivedOrientation(const Quaternion& derivedOrientation)
	{
		if(mParent)
		{
			SetOrientation(derivedOrientation * mParent->GetDerivedOrientation().Inverse());
		}else
		{
			SetOrientation(derivedOrientation);
		}
	}

	const Matrix4& Node::GetTransform() const
	{
		ScopedLock lock(mTransformMutex);
		if(mTransformOutOfDate)
		{
			mTransform.MakeTransform(GetDerivedPosition(), GetDerivedScale(), GetDerivedOrientation());
			mTransformOutOfDate=false;
		}
		return mTransform;
	}
	
	const Matrix4& Node::GetLocalTransform() const
	{
		ScopedLock lock(mLocalTransformMutex);
		if(mLocalTransformOutOfDate)
		{
			mLocalTransform.MakeTransform(GetPosition(), GetScale(), GetOrientation());
			mLocalTransformOutOfDate=false;
		}
		return mLocalTransform;
	}

	const Quaternion& Node::GetInitialOrientation() const
	{
		return mInitialOrientation;
	}

	const Vector3& Node::GetInitialPosition() const
	{
		return mInitialPosition;
	}

	const Vector3& Node::GetInitialScale() const
	{
		return mInitialScale;
	}

	bool Node::GetInheritOrientation() const
	{
		return mInheritOrientation;
	}

	bool Node::GetInheritScale() const
	{
		return mInheritScale;
	}

	void Node::SetOrientation( const Quaternion& val )
	{
		mOrientation = val;
		mOrientation.Normalise();
		NeedUpdate();
	}
	
	void Node::SetOrientationEuler(Radian yaw, Radian pitch, Radian roll)
	{
		SetOrientation(Quaternion(yaw,pitch,roll));
	}

	void Node::SetPosition( const Vector3& val )
	{
		mPosition = val;
		NeedUpdate();
	}

	void Node::SetPosition(f32 x, f32 y, f32 z)
	{
		mPosition.Set(x,y,z);
		NeedUpdate();
	}
	
	void Node::SetUniformScale(f32 scale)
	{
		mScale.Set(scale,scale,scale);
		NeedUpdate();
	}
		
	void Node::SetScale( const Vector3& val )
	{
		mScale = val;
		NeedUpdate();
	}

	void Node::SetInitialOrientation( const Quaternion& val )
	{
		mInitialOrientation = val;
		mInitialOrientation.Normalise();
	}

	void Node::SetInitialPosition( const Vector3& val )
	{
		mInitialPosition = val;
	}

	void Node::SetInitialScale( const Vector3& val )
	{
		mInitialScale = val;
	}

	void Node::SetInheritOrientation( bool val )
	{
		mInheritOrientation = val;
	}

	void Node::SetInheritScale( bool val )
	{
		mInheritScale = val;
	}

	void Node::Scale( const Vector3& s )
	{
		mScale+=s;
		NeedUpdate();
	}

	void Node::Translate( const Vector3& t )
	{
		mPosition += t;
		NeedUpdate();
	}

	void Node::TranslateRelative( const Vector3& t )
	{
		// Transform the axes of the relative vector by the node's local axes.
		Vector3 trans = mOrientation * t;
		mPosition = mPosition + trans;
		NeedUpdate();
	}
	
	void Node::Rotate( const Quaternion& rotation )
	{
		Quaternion norman=rotation.Normalised();
		mOrientation=mOrientation * norman;
		//mOrientation = mOrientation * GetDerivedOrientation().Inverse() * norman * GetDerivedOrientation();

		// Normalise quaternion to avoid drift
		//Quaternion qnorm = q;
		//qnorm.normalise();

		//switch(relativeTo)
		//{
		//case TS_PARENT:
		//	// Rotations are normally relative to local axes, transform up
		//	mOrientation = qnorm * mOrientation;
		//	break;
		//case TS_WORLD:
		//	// Rotations are normally relative to local axes, transform up
		//	mOrientation = mOrientation * _getDerivedOrientation().Inverse()
		//		* qnorm * _getDerivedOrientation();
		//	break;
		//case TS_LOCAL:
		//	// Note the order of the mult, i.e. q comes after
		//	mOrientation = mOrientation * qnorm;
		//	break;
		//}

		NeedUpdate();
	}

	void Node::Rotate( const Vector3& axis, const Radian& angle )
	{
		Quaternion q(angle,axis);
		Rotate(q);
	}

	void Node::Yaw( Radian angle )
	{
		Vector3 yAxis = mOrientation * Vector3(0,1,0);
		Rotate(yAxis,angle);
	}

	void Node::Pitch( Radian angle )
	{
		Vector3 xAxis = mOrientation * Vector3(1,0,0);
		Rotate(xAxis,angle);
	}

	void Node::Roll( Radian angle )
	{
		Vector3 zAxis = mOrientation * Vector3(0,0,1);
		Rotate(zAxis,angle);
	}

	void Node::SetParent( Node* node )
	{
		if(mParent)
		{
			mParent->RemoveChild(*this);
		}
		mParent=node;
		OnParentSet(mParent);
		if(mParent)
		{
			mParent->AddChild(*this);
		}
	}

	Node* Node::GetParent() const
	{
		return mParent;
	}

	void Node::AddChild( Node& node )
	{
		AddChild(&node,false);
	}
	
	void Node::AddChild(Node* node, bool takeOwnerShip)
	{
		if(node)
		{
			if(node->GetParent()==this)
			{
				return;
			}
			node->SetParent(this);
			mChildNodes.insert(node);
			if(takeOwnerShip)
			{
				mSharedChildren.insert(shared_ptr<Node>(node));
			}
			ChildAdded(*node);
		}
	}
	
	void Node::AddChild(shared_ptr<Node> node)
	{
		if(node)
		{
			if(node->GetParent()==this)
			{
				return;
			}
			node->SetParent(this);
			mChildNodes.insert(node.get());
			mSharedChildren.insert(node);
			ChildAdded(*node);
		}
	}	

	Node* Node::FindChild( const std::string& name, bool searchChildren) const
	{
		std::set< Node* >::const_iterator it=mChildNodes.begin();
		std::set< Node* >::const_iterator itEnd=mChildNodes.end();
		while(it!=itEnd)
		{
			Node* n=(*it);
			if(n->GetName()==name)
			{
				return n;
			}
			++it;
		}
		if(searchChildren)
		{
			std::set< Node* >::const_iterator it=mChildNodes.begin();
			std::set< Node* >::const_iterator itEnd=mChildNodes.end();
			while(it!=itEnd)
			{
				Node* n=(*it)->FindChild(name,true);
				if(n)
				{
					return n;
				}
				++it;
			}
		}
		return 0;
	}

	shared_ptr<Node> Node::FindSharedChild( const std::string& name, bool searchChildren) const
	{
		std::set< shared_ptr<Node> >::const_iterator it=mSharedChildren.begin();
		std::set< shared_ptr<Node> >::const_iterator itEnd=mSharedChildren.end();
		while(it!=itEnd)
		{
			shared_ptr<Node> n=(*it);
			if(n->GetName()==name)
			{
				return n;
			}
			++it;
		}
		if(searchChildren)
		{
			std::set< shared_ptr<Node> >::const_iterator it=mSharedChildren.begin();
			std::set< shared_ptr<Node> >::const_iterator itEnd=mSharedChildren.end();
			while(it!=itEnd)
			{
				shared_ptr<Node> n=(*it)->FindSharedChild(name,true);
				if(n)
				{
					return n;
				}
				++it;
			}
		}
		return shared_ptr<Node>();
	}
	
	shared_ptr<Node> Node::CreateChild(const std::string& name)
	{
		shared_ptr<Node> node(new Node());
		node->SetName(name);
		AddChild(node);
		return node;
	}

	shared_ptr<Node> Node::CreateChild(const std::string& name, const Vector3& position, const Quaternion& rotation)
	{
		shared_ptr<Node> node(new Node());
		node->SetName(name);
		node->SetInitialPosition(position);
		node->SetInitialOrientation(rotation);
		AddChild(node);
		return node;
	}

	void Node::RemoveAllChildren()
	{
		while(!mChildNodes.empty())
		{
			Node* child=*mChildNodes.begin();
			RemoveChild(*child);
		}
	}
	
	bool Node::RemoveChild( const std::string& name )
	{
		Node* childNode=FindChild(name, false);
		if(childNode)
		{
			return RemoveChild(*childNode);
		}
		return false;
	}

	bool Node::RemoveChild( Node& node )
	{
		std::set< Node* >::iterator it=mChildNodes.find(&node);
		if(it==mChildNodes.end())
		{
			return false;
		}
		mChildNodes.erase(it);
		node.SetParent(0);
		ChildRemoved(node);

		
		std::set< shared_ptr<Node> >::iterator sit = mSharedChildren.begin();
		while(sit!=mSharedChildren.end())
		{
			if((*sit).get()==&node)
			{
				mSharedChildren.erase(sit);
				break;
			}
			sit++;
		}
		return true;
	}

	void Node::Reset()
	{
		mPosition=mInitialPosition;
		mOrientation=mInitialOrientation;
		mScale=mInitialScale;
		NeedUpdate();
	}

	void Node::UpdateDerivedProperties() const
	{
		ScopedLock lock1(mDerivedPropertiesMutex);
		
		//Do we need to update?
		if(!mDerivedPropertiesOutOfDate)
			return;
		
		if (mParent)
		{
			const Quaternion& parentOrientation = mParent->GetDerivedOrientation();
			const Vector3& parentScale = mParent->GetDerivedScale();

			if(mInheritOrientation)
				mDerivedOrientation = parentOrientation * mOrientation;
			else
				mDerivedOrientation = mOrientation;

			if(mInheritScale)
				mDerivedScale = parentScale * mScale;
			else
				mDerivedScale = mScale;

			mDerivedPosition = parentOrientation * (mPosition * parentScale);
			mDerivedPosition += mParent->GetDerivedPosition();
		}else
		{
			mDerivedOrientation = mOrientation;
			mDerivedPosition = mPosition;
			mDerivedScale = mScale;
		}
		mTransformOutOfDate = true;
		mLocalTransformOutOfDate = true;
		mDerivedPropertiesOutOfDate = false;
	}

	void Node::_Update(bool updateChildren, bool parentHasChanged) const
	{
		// always clear information about parent notification
		mNotifiedParentOfUpdate = false ;

		// Short circuit the off case
		if (!updateChildren && !mDerivedPropertiesOutOfDate && !mNeedChildUpdate && !parentHasChanged )
		{
			return;
		}

		// See if we should process everyone
		if (mDerivedPropertiesOutOfDate || parentHasChanged)
		{
			// Update transforms from parent
			UpdateDerivedProperties();
		}

		if (mNeedChildUpdate || parentHasChanged)
		{
			std::set< Node* >::const_iterator it=mChildNodes.begin();
			std::set< Node* >::const_iterator itEnd=mChildNodes.end();
			while(it!=itEnd)
			{
				Node* child = *it;
				child->_Update(true, true);
				++it;
			}
			mChildrenToUpdate.clear();
		}else
		{
			// Just update selected children
			std::set< const Node* >::const_iterator it=mChildrenToUpdate.begin();
			std::set< const Node* >::const_iterator itEnd=mChildrenToUpdate.end();
			while(it!=itEnd)
			{
				const Node* child = *it;
				child->_Update(true, false);
				++it;
			}

			mChildrenToUpdate.clear();
		}
	}

	void Node::NeedUpdate(bool forceParent) const
	{
		mNeedChildUpdate=true;
		mDerivedPropertiesOutOfDate=true;
		mTransformOutOfDate=true;
		mLocalTransformOutOfDate=true;

		std::set< Node* >::const_iterator it=mChildNodes.begin();
		std::set< Node* >::const_iterator itEnd=mChildNodes.end();
		while(it!=itEnd)
		{
			Node* child = *it;
			child->NeedUpdate(false);
			++it;
		}
		mChildrenToUpdate.clear();

		if(mParent && (!mNotifiedParentOfUpdate || forceParent))
		{
			mParent->ChildRequestUpdate(*this,forceParent);
			mNotifiedParentOfUpdate=true;
		}
	}

	void Node::ChildRequestUpdate(const Node& node, bool forceParentUpdate) const
	{
		if(mNeedChildUpdate)
			return;

		mChildrenToUpdate.insert(&node);

		if(mParent && (!mNotifiedParentOfUpdate || forceParentUpdate))
		{
			mParent->ChildRequestUpdate(*this,forceParentUpdate);
			mNotifiedParentOfUpdate=true;
		}
	}

	void Node::CancelChildUpdate(Node& node)
	{
		mChildrenToUpdate.erase(&node);

		if (mChildrenToUpdate.empty() && mParent && !mNeedChildUpdate)
		{
			mParent->CancelChildUpdate(*this);
			mNotifiedParentOfUpdate = false ;
		}
	}

	void Node::ChildAdded(Node& node)
	{
	}
	
	void Node::ChildRemoved(Node& node)
	{
	}
	
	void Node::OnParentSet(Node* node)
	{
	}
	
}
