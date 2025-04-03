
#include <echo/Physics/BulletMeshPhysicsShape.h>
#include <echo/Graphics/Mesh.h>
#include <echo/Graphics/SubMesh.h>
#include <btBulletCollisionCommon.h>
#include <BulletCollision/Gimpact/btGImpactShape.h>

namespace Echo
{
	
	BulletMeshPhysicsShape::BulletMeshPhysicsShape(shared_ptr<Mesh> mesh, Mode mode)
	{
		mIndexVertexArrays = new btTriangleIndexVertexArray();
		SetMesh(mesh);
		
		switch(mode)
		{
			case Modes::DYNAMIC_BODY:
			{
				btGImpactMeshShape* collisionShape = new btGImpactMeshShape(mIndexVertexArrays);
				collisionShape->setLocalScaling(btVector3(1.f, 1.f, 1.f));
				collisionShape->setMargin(0.0f);
				collisionShape->updateBound();
				SetCollisionShape(collisionShape);
			}break;
			
			default:
			case Modes::STATIC_BODY:
			{
				btBvhTriangleMeshShape* collisionShape = new btBvhTriangleMeshShape(mIndexVertexArrays, true, true);
				SetCollisionShape(collisionShape);
			}break;
		}
	}

	BulletMeshPhysicsShape::~BulletMeshPhysicsShape()
	{
		delete mIndexVertexArrays;
	}
	
	void BulletMeshPhysicsShape::SetMesh(shared_ptr<Mesh> mesh)
	{
		if(!mesh)
		{
			assert(mesh && "Mesh cannot be null");
			return;
		}
		mMesh = mesh;
		Size numberOfSubMeshes = mMesh->GetNumberOfSubMeshes();
		for(Size s = 0; s < numberOfSubMeshes; ++s)
		{
			SubMesh& subMesh=*mMesh->GetSubMesh(s);
			VertexAttribute* attribute = subMesh.GetVertexBuffer()->GetVertexAttribute("Position");
			if(!attribute)
			{
				ECHO_LOG_WARNING("Could not find vertex attribute using label \"Position\". Ensure your vertex buffers have a \"Position\" attribute to use them as physics shapes." );
				continue;
			}
			
			shared_ptr<ElementBuffer> elementBuffer = subMesh.GetElementBuffer();
			if(!elementBuffer)
			{
				ECHO_LOG_WARNING("SubMesh is incomplete, ElementBuffer is null");
				continue;
			}
			if(elementBuffer->GetElementType()!=ElementBuffer::ElementTypes::TRIANGLE && elementBuffer->GetElementType()!=ElementBuffer::ElementTypes::TRIANGLE_STRIP)
			{
				ECHO_LOG_WARNING("SubMesh ElementBuffer does not contain compatible triangles.");
				continue;
			}
			
			btIndexedMesh indexedMesh;
			indexedMesh.m_numTriangles = elementBuffer->GetNumberOfElements();
			indexedMesh.m_triangleIndexBase = reinterpret_cast<const unsigned char*>(elementBuffer->GetDataPointer());
			indexedMesh.m_triangleIndexStride = elementBuffer->GetElementStride();
			indexedMesh.m_numVertices = subMesh.GetVertexBuffer()->GetNumberOfElements();
			indexedMesh.m_vertexStride = subMesh.GetVertexBuffer()->GetStride();

			if(attribute->GetWidth()==sizeof(Vector3Double))
			{
				VertexBuffer::Accessor<Vector3Double> vertices = subMesh.GetVertexBuffer()->GetAccessor<Vector3Double>("Position");
				if(!vertices)
				{
					ECHO_LOG_WARNING("Could not find vertices using label \"Position\" unable to use SubMesh as physics shape. Ensure your vertex buffers have a \"Position\" attribute to use them as physics shapes." );
					continue;
				}
				indexedMesh.m_vertexBase = reinterpret_cast<const unsigned char*>(&(vertices[0]));
				indexedMesh.m_vertexType = PHY_DOUBLE;
			}else
			{
				VertexBuffer::Accessor<Vector3> vertices = subMesh.GetVertexBuffer()->GetAccessor<Vector3>("Position");
				if(!vertices)
				{
					ECHO_LOG_WARNING("Could not find vertices using label \"Position\" unable to use SubMesh as physics shape. Ensure your vertex buffers have a \"Position\" attribute to use them as physics shapes." );
					continue;
				}
				indexedMesh.m_vertexBase = reinterpret_cast<const unsigned char*>(&(vertices[0]));
				indexedMesh.m_vertexType = PHY_FLOAT;
			}

			mIndexVertexArrays->addIndexedMesh(indexedMesh, PHY_SHORT);
		}
	}
}
