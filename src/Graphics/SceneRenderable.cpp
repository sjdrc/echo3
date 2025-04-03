#include <echo/Graphics/SceneRenderable.h>

namespace Echo
{
	SceneRenderable::SceneRenderable(const Vector3& position, const Quaternion& orientation, const Vector3& scale)
		: Node(position, orientation, scale), mVisible(true), mParentSceneRenderable(nullptr)
	{
		
	}
	
	SceneRenderable::~SceneRenderable()
	{
		// Higher level classes may dynamic cast to a SceneRenderable which will fail when this node is removed
		// from the parent because the SceneRenderable part of the object will have been deleted.
		if(GetParent())
		{
			GetParent()->RemoveChild(*this);
		}
	}
	
	SceneRenderable::SceneRenderable(const SceneRenderable& other)
	{
		mParentSceneRenderable = 0;
		*this = other;
	}
	
	SceneRenderable& SceneRenderable::operator=(const SceneRenderable& rhs)
	{
		if(this==&rhs)
		{
			return *this;
		}
		//mParentSceneRenderable	// Leave.
		Node::operator=(rhs);
		
		mVisible = rhs.mVisible;
		return *this;
	}

	AxisAlignedBox SceneRenderable::GetSceneAxisAlignedBox() const
	{
		AxisAlignedBox localAABB = GetAxisAlignedBox(false);
		localAABB.Transform(Node::GetTransform());
		return localAABB;
	}
	
	void SceneRenderable::Accept(SceneRenderableVisitor& visitor)
	{
		if(mVisible)
		{
			visitor.SceneRenderableVisit(*this);
		}
	}

	void SceneRenderable::Leave(SceneRenderableVisitor&)
	{
		//Default is to do nothing
	}
	
	bool SceneRenderable::GetVisible() const
	{
		if(mParentSceneRenderable)
		{
			return mVisible && mParentSceneRenderable->GetVisible();
		}
		return mVisible;
	}
	
	void SceneRenderable::OnParentSet(Node* node)
	{
		mParentSceneRenderable = dynamic_cast<SceneRenderable*>(node);
	}
}
