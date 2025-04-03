#include <echo/Graphics/SceneEntity.h>
#include <echo/Graphics/Mesh.h>
#include <echo/Graphics/SubMesh.h>
#include <echo/Graphics/Material.h>
#include <echo/Util/Utils.h>
#include <boost/foreach.hpp>

namespace Echo
{
	SceneEntity::SceneEntity(const Vector3& position, const Quaternion& orientation, const Vector3& scale)
		: SceneRenderable(position, orientation, scale),
		mParentSceneEntity(nullptr),
		mColour(Colours::WHITE),
		mRenderAABB(false),
		mInheritParentColour(true),
		mInheritParentAlpha(true)
	{
		
	}
	
	SceneEntity::~SceneEntity()
	{
		// Higher level classes (like this one) dynamic cast to a SceneRenderable which will fail when this node is removed
		// from the parent because the higher level class part of the object will have been deleted.
		if(GetParent())
		{
			GetParent()->RemoveChild(*this);
		}
		if(mMesh)
		{
			mMesh->RemoveListener(this);
		}
	}
	
	SceneEntity::SceneEntity(const SceneEntity& other)
	{
		mParentSceneEntity = 0;
		*this = other;
	}
	
	SceneEntity& SceneEntity::operator=(const SceneEntity& rhs)
	{
		if(this==&rhs)
		{
			return *this;
		}
		SceneRenderable::operator=(rhs);
		mMesh = rhs.mMesh;
		//mAABBMesh				//Updated automatically later.
		//mChildren				//Leave.
		//mParentSceneEntity	//Leave.
		mColour = rhs.mColour;
		mRenderAABB = rhs.mRenderAABB;
		mInheritParentColour = rhs.mInheritParentColour;
		mInheritParentAlpha = rhs.mInheritParentAlpha;
		return *this;
	}

	void SceneEntity::SetAxisAlignedBox(AxisAlignedBox localAxisAlignedBox)
	{
		mManualAxisAlignedBox = localAxisAlignedBox;
	}

	void SceneEntity::RemoveManualAxisAlignedBox()
	{
		mManualAxisAlignedBox = none;
	}
	
	AxisAlignedBox SceneEntity::GetAxisAlignedBox(bool applyLocalTransform) const
	{
		AxisAlignedBox aabb;
		if(mManualAxisAlignedBox)
		{
			aabb = mManualAxisAlignedBox.value();
		}else
		{
			if(mMesh)
			{
				aabb = mMesh->GetAxisAlignedBox();
			}
		}
		if(applyLocalTransform)
		{
			aabb.Transform(GetLocalTransform());
		}
		return aabb;
	}

	void SceneEntity::Render(RenderContext& renderContext, Colour compoundDiffuse)
	{
		const Matrix4& world = GetTransform();
		Matrix4 worldView = renderContext.mViewMatrix * world;
		if(mMesh)
		{
			mMesh->Render(renderContext, world, worldView, compoundDiffuse * GetColour(true));
		}
		if(mRenderAABB)
		{
			UpdateAABBMesh();
			mAABBMesh->Render(renderContext, world, worldView, Colours::WHITE);
		}
	}

	void SceneEntity::SetMesh(shared_ptr<Mesh> mesh)
	{
		if(mMesh)
		{
			mMesh->RemoveListener(this);
		}
		mMesh = mesh;
		if(mMesh)
		{
			mMesh->AddListener(this);
		}
	}
	
	
	void SceneEntity::SetMaterial(shared_ptr<Material> material)
	{
		if (mMesh)
		{
			mMesh->SetMaterial(material);
		}
	}
	
	void SceneEntity::ChildAdded(Node& node)
	{
		SceneRenderable* renderable = dynamic_cast<SceneRenderable*>(&node);
		if(renderable)
		{
			mChildren.push_back(renderable);
		}
	}
	
	void SceneEntity::ChildRemoved(Node& node)
	{
		SceneRenderable* renderable = dynamic_cast<SceneRenderable*>(&node);
		if(renderable)
		{
			mChildren.remove(renderable);
		}
	}
	
	void SceneEntity::Accept(SceneRenderableVisitor& visitor)
	{
		if(!GetVisibilityFlag())
		{
			return;
		}
		visitor.SceneRenderableVisit(*this);
		BOOST_FOREACH(SceneRenderable* renderable, mChildren)
		{
			renderable->Accept(visitor);
		}
	}
	
	void SceneEntity::Leave(SceneRenderableVisitor& visitor)
	{
		//Pass notification on to children.
		BOOST_FOREACH(SceneRenderable* renderable, mChildren)
		{
			renderable->Leave(visitor);
		}
	}
	
	void SceneEntity::UpdateAABBMesh()
	{
		if(!mAABBMesh)
		{
			mAABBMesh = shared_ptr<Mesh>(new Mesh());
			shared_ptr<SubMesh> subMesh = mAABBMesh->CreateSubMesh();
			shared_ptr<VertexBuffer> vertexBuffer = subMesh->GetVertexBuffer();
			vertexBuffer->AddVertexAttribute("Position",VertexAttribute(VertexAttribute::ComponentTypes::VECTOR3,1));
			vertexBuffer->Allocate(8);
			subMesh->SetType(SubMesh::MeshTypes::LINES);
			subMesh->SetElementBuffer(ElementBuffer::Types::STATIC, ElementBuffer::IndexTypes::UNSIGNED_16BIT, ElementBuffer::ElementTypes::LINES, 12);
			shared_ptr<ElementBuffer> elementBuffer = subMesh->GetElementBuffer();
			auto lines = elementBuffer->GetAccessor< ElementBuffer::Line<u16> >();
			
			u16 lineEdges[] = {	0,1, 1,2, 2,3, 3,0,
								4,5, 5,6, 6,7, 7,4,
								0,4, 1,5, 2,6, 3,7};
			
			for(size_t i=0; i < 12; ++i)
			{
				auto& line = lines[i];
				line.mA = lineEdges[(i*2)];
				line.mB = lineEdges[(i*2) + 1];
			}
			shared_ptr<Material> material(new Material());
			material->SetToDefaultMaterial();
			subMesh->SetMaterial(material);
		}
		
		shared_ptr<SubMesh> subMesh = mAABBMesh->GetSubMesh();
		shared_ptr<VertexBuffer> vertexBuffer = subMesh->GetVertexBuffer();
		//We always want the local non-transformed AABB because the transform is applied when rendering.
		AxisAlignedBox box = GetAxisAlignedBox(false);
		Vector3 minimum = box.GetMinimum();
		Vector3 maximum = box.GetMaximum();
		VertexBuffer::Accessor<Vector3> vertices = vertexBuffer->GetAccessor<Vector3>("Position");
		vertices[0] = Vector3(minimum.x,minimum.y,minimum.z);
		vertices[1] = Vector3(minimum.x,maximum.y,minimum.z);
		vertices[2] = Vector3(maximum.x,maximum.y,minimum.z);
		vertices[3] = Vector3(maximum.x,minimum.y,minimum.z);
		vertices[4] = Vector3(minimum.x,minimum.y,maximum.z);
		vertices[5] = Vector3(minimum.x,maximum.y,maximum.z);
		vertices[6] = Vector3(maximum.x,maximum.y,maximum.z);
		vertices[7] = Vector3(maximum.x,minimum.y,maximum.z);
		subMesh->Finalise();
	}
	
	Colour SceneEntity::GetColour(bool combineWithParentIfEnabled) const
	{
		if(combineWithParentIfEnabled && mParentSceneEntity && (mInheritParentColour || mInheritParentAlpha))
		{
			Colour combinedColour = mParentSceneEntity->GetColour(true) * mColour;
			if(!mInheritParentColour)
			{
				mColour.CopyColourComponentsTo(combinedColour);
			}
			if(!mInheritParentAlpha)
			{
				mColour.CopyAlphaTo(combinedColour);
			}
			return combinedColour;
		}
		return mColour;
	}

	void SceneEntity::OnParentSet(Node* node)
	{
		mParentSceneEntity = dynamic_cast<SceneEntity*>(node);
		SceneRenderable::OnParentSet(node);
	}

	void SceneEntity::OnMeshAABBNeedsUpdate(const Mesh&) const
	{
		//We need to update the node so any child classes in the tree can update as needed.
		NeedUpdate();
	}
	
	void SceneEntity::OnMeshAABBUpdated(const Mesh&) const
	{
		//We only need to know when the AABB needs an update because after we mark the node flag as needs an update
		//the next time the AABB (or transform) is required the mesh will be updated as needed.
	}
}
