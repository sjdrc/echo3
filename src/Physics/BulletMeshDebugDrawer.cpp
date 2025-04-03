#include <echo/Physics/BulletMeshDebugDrawer.h>
#include <echo/Graphics/SubMesh.h>
#include <echo/Physics/BulletTypeConverters.h>
#include <echo/Kernel/ScopedLock.h>

namespace Echo
{
	BulletMeshDebugDrawer::BulletMeshDebugDrawer() :
		mDebugMode(0),
		mNumberOfVertices(0),
		mNumberOfLines(0),
		mJustRendered(true)
	{
		mSubMesh = CreateSubMesh();
		mSubMesh->SetType(SubMesh::MeshTypes::LINES);
		mVertexBuffer = mSubMesh->GetVertexBuffer(VertexBuffer::Types::DYNAMIC);
		mVertexBuffer->AddVertexAttribute("Position",VertexAttribute(VertexAttribute::ComponentTypes::VECTOR3, 1));		//Position
		mVertexBuffer->AddVertexAttribute("Colour",VertexAttribute(VertexAttribute::ComponentTypes::COLOUR_8, 1));		//Colour
		mVertexBuffer->Allocate(4096);
		mVertexBuffer->SetNumberOfElements(0);
		mVertices = mVertexBuffer->GetAccessor<Vector3>("Position");
		mColours = mVertexBuffer->GetAccessor<VertexColour>("Colour");

		if(mSubMesh->SetElementBuffer(ElementBuffer::Types::DYNAMIC,ElementBuffer::IndexTypes::UNSIGNED_32BIT,ElementBuffer::ElementTypes::LINES, 4096))
		{
			mElementBuffer = mSubMesh->GetElementBuffer();
			mLines = mSubMesh->GetElementBuffer()->GetAccessor< ElementBuffer::Line< u32 > >();
			mElementBuffer->SetNumberOfElements(0);
		}

		shared_ptr<Material> material(new Material());
		material->SetToDefaultMaterial();
		material->GetPass(0)->SetVertexColouringEnabled(true);
		SetMaterial(material);
	}
	
	BulletMeshDebugDrawer::~BulletMeshDebugDrawer()
	{
		
	}

	void BulletMeshDebugDrawer::Render(RenderContext& renderContext, const Matrix4& world, const Matrix4& worldView, Colour compoundDiffuse)
	{
		ScopedLock lock(mMutex);
		mVertexBuffer->SetNumberOfElements(mNumberOfVertices);
		mSubMesh->Finalise();
		Mesh::Render(renderContext, world, worldView, compoundDiffuse);
		mJustRendered = true;
	}

	void BulletMeshDebugDrawer::Lock()
	{
		mMutex.Lock();
	}
	
	void BulletMeshDebugDrawer::Unlock()
	{
		mMutex.Unlock();
	}

	void BulletMeshDebugDrawer::drawLine(const btVector3& from,const btVector3& to,const btVector3& colour)
	{
		if(mJustRendered)
		{
			mJustRendered = false;
			mNumberOfVertices = 0;
			mNumberOfLines = 0;
			mElementBuffer->SetNumberOfElements(0);
		}
		if(mNumberOfVertices>=mVertices.GetCapacity())
		{
			// We could reallocate at the end of Render() to a new size if we need to.
			ECHO_LOG_DEBUG("Hit vertex limit: " << mNumberOfVertices << " of " << mVertices.GetCapacity());
			return;
		}

		mVertices[mNumberOfVertices] = Convert(from);
		mColours[mNumberOfVertices] = Colour(colour.x(),colour.y(),colour.z());
		++mNumberOfVertices;
		
		mVertices[mNumberOfVertices] = Convert(to);
		mColours[mNumberOfVertices] = Colour(colour.x(),colour.y(),colour.z());
		++mNumberOfVertices;

		auto& line = mLines[mNumberOfLines];
		line.mA = mNumberOfVertices-2;
		line.mB = mNumberOfVertices-1;
		++mNumberOfLines;
		mElementBuffer->SetNumberOfElements(mNumberOfLines);
	}

	void BulletMeshDebugDrawer::setDebugMode(int debugMode)
	{
		mDebugMode = debugMode;
	}
	
	int BulletMeshDebugDrawer::getDebugMode() const
	{
		return mDebugMode;
	}

	void BulletMeshDebugDrawer::reportErrorWarning(const char* warningString)
	{
		ECHO_LOG_WARNING(warningString);
	}

	void BulletMeshDebugDrawer::drawContactPoint(const btVector3& pointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& colour)
	{
		if (mDebugMode & btIDebugDraw::DBG_DrawContactPoints)
		{
			ECHO_LOG_WARNING("btIDebugDraw::DBG_DrawContactPoints is not supported");
		}
	}
	
	void BulletMeshDebugDrawer::draw3dText(const btVector3& location,const char* textString)
	{
		if (mDebugMode & btIDebugDraw::DBG_DrawText)
		{
			ECHO_LOG_WARNING("btIDebugDraw::DBG_DrawText is not supported");
		}
	}
	
}
