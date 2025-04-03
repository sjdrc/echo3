#include <echo/Graphics/SubMesh.h>
#include <echo/Graphics/Mesh.h>
#include <echo/Kernel/ScopedLock.h>
#include <iostream>

#include <echo/Graphics/RenderTarget.h>

namespace Echo
{
	SubMesh::SubMesh(const std::string& name, Mesh& parent) :
		mParent(parent),
		mAxisAlignedBox(Vector3::ZERO,Vector3::ZERO),
		mName(name),
		mVisible(true),
		mAxisAlignedBoxOutOfDate(true),
		mType(MeshTypes::TRIANGLES)
	{
	}

	Vector3 SubMesh::GetDimensions() const
	{
		UpdateAxisAlignedBox();
		return mAxisAlignedBox.GetSize();
	}

	void SubMesh::SetElementBuffer(shared_ptr<ElementBuffer> elementBuffer)
	{
		mElementBuffer = elementBuffer;
	}
	
	bool SubMesh::SetElementBuffer(ElementBuffer::Type type, ElementBuffer::IndexType indexType, ElementBuffer::ElementType elementType, Size numberOfElements)
	{
		shared_ptr<ElementBuffer> elementBuffer(new ElementBuffer(type));
		if(elementBuffer->Allocate(indexType,elementType, numberOfElements))
		{
			mElementBuffer = elementBuffer;
			return true;
		}
		return false;
	}
	
	void SubMesh::SetVertexBuffer(shared_ptr<VertexBuffer> vertexBuffer)
	{
		mVertexBuffer = vertexBuffer;
		Finalise();
	}
		
	shared_ptr<VertexBuffer> SubMesh::GetVertexBuffer(VertexBuffer::Type type)
	{
		if(!mVertexBuffer)
		{
			mVertexBuffer.reset(new VertexBuffer(type));
		}
		return mVertexBuffer;
	}
	
	const AxisAlignedBox& SubMesh::GetAxisAlignedBox() const
	{
		UpdateAxisAlignedBox();
		return mAxisAlignedBox;
	}

	shared_ptr<SubMesh> SubMesh::Clone(Mesh& clonesParent) const
	{
		SubMesh* newMesh = new SubMesh(mName, clonesParent);
		if(mMaterial)
		{
			newMesh->mMaterial = mMaterial->Clone();
		}
		if(mOriginalVertices)
		{
			newMesh->mOriginalVertices = shared_ptr< std::vector<Vector3> >(new std::vector<Vector3>(*mOriginalVertices));
		}
		if(mBoneWeights)
		{
			newMesh->mBoneWeights = shared_ptr< std::vector<BoneBinding*> >(new std::vector<BoneBinding*>(*mBoneWeights));
		}
		if(mVertexBuffer)
		{
			newMesh->mVertexBuffer.reset(new VertexBuffer(*mVertexBuffer));
		}
		if(mElementBuffer)
		{
			newMesh->mElementBuffer.reset(new ElementBuffer(*mElementBuffer));
		}

		newMesh->mAxisAlignedBox = mAxisAlignedBox;
		newMesh->mType = mType;
		return shared_ptr<SubMesh>(newMesh);
	}

	void SubMesh::SetAxisAlignedBox(AxisAlignedBox localAxisAlignedBox)
	{
		mManualAxisAlignedBox = localAxisAlignedBox;
		mAxisAlignedBoxOutOfDate = true;
		mParent.MarkExtentsOutOfDate();
	}

	void SubMesh::RemoveManualAxisAlignedBox()
	{
		mManualAxisAlignedBox = none;
	}

	void SubMesh::UpdateAxisAlignedBox() const
	{
		ScopedLock lock(mAxisAlignedBoxMutex);
		if(!mAxisAlignedBoxOutOfDate)
		{
			return;
		}
		if(mManualAxisAlignedBox)
		{
			mAxisAlignedBox = mManualAxisAlignedBox.value();
		}else
		{
			mAxisAlignedBox.SetNull();
			mAxisAlignedBox.SetExtents(Vector3::ZERO,Vector3::ZERO);
			if(mVertexBuffer && mVertexBuffer->GetNumberOfElements()!=0)
			{
				VertexBuffer::Accessor<Vector3> vertices = GetComponents<Vector3>("Position");
				if(vertices)
				{
					Size numberOfElements = mVertexBuffer->GetNumberOfElements();
					for(Size i = 0; i < numberOfElements; ++i)
					{
						mAxisAlignedBox.Merge(vertices[i]);
					}
				}
			}
		}
		mAxisAlignedBoxOutOfDate = false;
	}
	
	void SubMesh::TranslateVertices(const Vector3& translation)
	{
		if(!mVertexBuffer)
		{
			return;
		}
		//This is faster than performing a matrix operation on each vertex.
		VertexBuffer::Accessor<Vector3> vertices = GetComponents<Vector3>("Position");
		if(vertices)
		{
			Size numberOfElements = mVertexBuffer->GetNumberOfElements();
			for(Size i = 0; i < numberOfElements; ++i)
			{
				vertices[i]+=translation;
			}
			mAxisAlignedBox.Translate(translation);
		}
	}

	SubMesh::~SubMesh()
	{
	}

	void SubMesh::Clear(bool deallocate)
	{
		if(deallocate)
		{
			if(mVertexBuffer)
			{
				mVertexBuffer->Deallocate();
			}
			if(mElementBuffer)
			{
				mElementBuffer->Deallocate();
			}
		} else
		{
			if(mVertexBuffer)
			{
				mVertexBuffer->SetNumberOfElements(0);
			}
			if(mElementBuffer)
			{
				mElementBuffer->SetNumberOfElements(0);
			}
		}
		mAxisAlignedBox.SetNull();
		mAxisAlignedBoxOutOfDate = true;
		mParent.MarkExtentsOutOfDate();
	}

	void SubMesh::SetMaterial(shared_ptr<Material> material)
	{
		mMaterial = material;
	}

	void SubMesh::GenerateNormals()
	{
		// This method is not generalised properly
		if(mType!=MeshTypes::TRIANGLES)
		{
			ECHO_LOG_ERROR("GenerateNormals only supports triangle meshes, either add your own support for this type of mesh or request the feature.");
			return;
		}

		if(!mVertexBuffer || !mElementBuffer)
		{
			ECHO_LOG_ERROR("Cannot generate normals. Vertex Buffer and Element buffers need to be set.");
			return;
		}

		// Some standard mesh information that you should have lying around.
		// vertex is your vertex structure that just contains a normal and position here.
		// mVertices is a pointer to the first vertex
		// indices is a pointer to the first index
		// num_verts is number of mVertices
		// num_indices is number of indices
		// each face of the mesh is made up of three mVertices.
		
		auto triangles = mElementBuffer->GetAccessor< ElementBuffer::Triangle<u16> >();
		
		VertexBuffer::Accessor<Vector3> vertices = GetComponents<Vector3>("Position");
		if(!vertices)
		{
			ECHO_LOG_ERROR("Attempted to generate normals but the SubMesh does not have a Position compoent");
			return;
		}
		VertexBuffer::Accessor<Vector3> normals = GetComponents<Vector3>("Normal");
		if(!normals)
		{
			ECHO_LOG_ERROR("Attempted to generate normals but the SubMesh does not have a Normal compoent");
			return;
		}
		Size numVertices = vertices.GetCapacity();

		std::vector<Vector3>* normal_buffer = new std::vector<Vector3>[numVertices];
		for(Size i = 0; i < triangles.GetCapacity(); ++i)
		{
			// get the three mVertices that make the mFaces
			auto triangle = triangles[i];
			Vector3 p1 = vertices[ triangle.mA ];
			Vector3 p2 = vertices[ triangle.mB ];
			Vector3 p3 = vertices[ triangle.mC ];

			Vector3 v1 = p2 - p1;
			Vector3 v2 = p3 - p1;
			Vector3 normal = v1.Cross(v2);

			normal.Normalise();

			// Store the face's normal for each of the mVertices that make up the face.
			normal_buffer[triangle.mA].push_back(normal);
			normal_buffer[triangle.mB].push_back(normal);
			normal_buffer[triangle.mC].push_back(normal);
		}

		// Now loop through each vertex vector, and average out all the mNormals stored.
		for(Size i = 0; i < numVertices; ++i)
		{
			for(Size j = 0; j < normal_buffer[i].size(); ++j)
			{
				normals[i] += normal_buffer[i][j];
			}

			if(normal_buffer[i].size()!=0)
			{
				normals[i] /= (f32) (normal_buffer[i].size());
			}
			normals[i].Normalise();
		}
		delete [] normal_buffer;
	}

	void SubMesh::GenerateTangents(bool logError)
	{
		// This method is not generalised properly
		if(mType!=MeshTypes::TRIANGLES)
		{
			if(logError)
			{
				ECHO_LOG_ERROR("GenerateTangents only supports triangle meshes, either add your own support for this type of mesh or request the feature.");
			}
			return;
		}

		if(!mVertexBuffer || !mElementBuffer)
		{
			if(logError)
			{
				ECHO_LOG_ERROR("Cannot generate tangents. Vertex Buffer and Element buffers need to be set.");
			}
			return;
		}
		
		auto triangles = mElementBuffer->GetAccessor< ElementBuffer::Triangle<u16> >();
		
		VertexBuffer::Accessor<Vector3> vertices = GetComponents<Vector3>("Position");
		if(!vertices)
		{
			if(logError)
			{
				ECHO_LOG_ERROR("Attempted to generate tangents but the SubMesh does not have a Posiition compoent");
			}
			return;
		}
		VertexBuffer::Accessor<TextureUV> uv0 = GetComponents<TextureUV>("UV0");
		if(!uv0)
		{
			if(logError)
			{
				ECHO_LOG_ERROR("Attempted to generate tangents but the SubMesh does not have a UV0 compoent");
			}
			return;
		}
		VertexBuffer::Accessor<Vector3> tangents = GetComponents<Vector3>("Tangent");
		if(!tangents)
		{
			if(logError)
			{
				ECHO_LOG_ERROR("Attempted to generate tangents but the SubMesh does not have a Tangent compoent");
			}
			return;
		}
		VertexBuffer::Accessor<Vector3> normals = GetComponents<Vector3>("Normal");
		if(!normals)
		{
			if(logError)
			{
				ECHO_LOG_ERROR("Attempted to generate tangents but the SubMesh does not have a Normal compoent");
			}
			return;
		}

		// This algorithm is from https://gamedev.stackexchange.com/questions/68612/how-to-compute-tangent-and-bitangent-vectors

		// Zero the tangents
		for(Size i = 0; i < tangents.GetCapacity(); ++i)
		{
			tangents[i] = Vector3::ZERO;
		}

		Size numVertices = vertices.GetCapacity();

		for(Size i = 0; i < triangles.GetCapacity(); ++i)
		{
			// get the three mVertices that make the mFaces
			auto triangle = triangles[i];
			const Vector3& p1 = vertices[ triangle.mA ];
			const Vector3& p2 = vertices[ triangle.mB ];
			const Vector3& p3 = vertices[ triangle.mC ];

			const TextureUV& uv1 = uv0[ triangle.mA ];
			const TextureUV& uv2 = uv0[ triangle.mB ];
			const TextureUV& uv3 = uv0[ triangle.mC ];

			f32 x1 = p2.x - p1.x;
			f32 x2 = p3.x - p1.x;
			f32 y1 = p2.y - p1.y;
			f32 y2 = p3.y - p1.y;
			f32 z1 = p2.z - p1.z;
			f32 z2 = p3.z - p1.z;

			f32 s1 = uv2.u - uv1.u;
			f32 s2 = uv3.u - uv1.u;
			f32 t1 = uv2.v - uv1.v;
			f32 t2 = uv3.v - uv1.v;

			f32 r = 1.0F / (s1 * t2 - s2 * t1);

			Vector3 tangent((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
			tangents[ triangle.mA ] += tangent;
			tangents[ triangle.mB ] += tangent;
			tangents[ triangle.mC ] += tangent;
		}

		// Now loop through each vertex vector, and average out all the normals stored.
		for (Size a = 0; a < numVertices; a++)
		{
			const Vector3& n = normals[a];
			Vector3& t = tangents[a];
			
			// Gram-Schmidt orthogonalize
			t = (t - n * n.Dot(t));
			t.Normalise();
		}
	}

	Vector3 SubMesh::GetCentre() const
	{
		UpdateAxisAlignedBox();
		return mAxisAlignedBox.GetCentre();
	}

	void SubMesh::Render(RenderContext& renderContext, const Matrix4& world, const Matrix4& worldView, Colour compoundDiffuse)
	{
		if(!mVisible || !mVertexBuffer || !mElementBuffer || !mMaterial)
		{
			return;
		}
		
		if(mParent.GetUseSkeleton())
		{
			ApplyVertexBoneTransforms();
		}
		mMaterial->ApplyAndRender(renderContext, *this, world, worldView, compoundDiffuse);
	}

	void SubMesh::GenerateTransformBuffers()
	{
		if(!mVertexBuffer)
		{
			return;
		}
		mOriginalVertices.reset();
		VertexBuffer::Accessor<Vector3> vertices = GetComponents<Vector3>("Position");
		if(vertices)
		{
			std::vector<Vector3>* newBuffer = new std::vector<Vector3>(vertices.GetCapacity());
			
			std::vector<Vector3>& dest= *newBuffer;
			for(Size i=0; i<vertices.GetCapacity(); ++i)
			{
				dest[i] = vertices[i];
			}
			
			mOriginalVertices = shared_ptr< std::vector<Vector3> >(newBuffer);
		}
	}

	void SubMesh::ApplyVertexBoneTransforms()
	{
		if(!mVertexBuffer || !mBoneWeights)
		{
			return;
		}

		// Before applying transforms make sure we have preserved the original
		if(!mOriginalVertices)
		{
			GenerateTransformBuffers();
		}
		
		VertexBuffer::Accessor<Vector3> vertices = GetComponents<Vector3>("Position");
		if(!vertices)
		{
			return;
		}
		std::vector<Vector3>& originalVerts = *mOriginalVertices;
		Size numVerts = mVertexBuffer->GetNumberOfElements();
		assert(originalVerts.size() == numVerts);
		for(Size v = 0; v < numVerts; ++v)
		{
			BoneBinding& binding = *(*mBoneWeights)[v];
			vertices[v].SetZero();
			std::map< size_t, f32 >::iterator it = binding.mBoneWeights.begin();
			std::map< size_t, f32 >::iterator itEnd = binding.mBoneWeights.end();
			while(it != itEnd)
			{
				const Matrix4& trans = mParent.GetSkeleton()->GetBone(it->first)->GetOffsetTransform();

				f32& weight = it->second;

				vertices[v].x += (trans[0][0] * originalVerts[v].x +
								trans[0][1] * originalVerts[v].y +
								trans[0][2] * originalVerts[v].z +
								trans[0][3])
						* weight;
				vertices[v].y += (trans[1][0] * originalVerts[v].x +
								trans[1][1] * originalVerts[v].y +
								trans[1][2] * originalVerts[v].z +
								trans[1][3])
						* weight;
				vertices[v].z += (trans[2][0] * originalVerts[v].x +
								trans[2][1] * originalVerts[v].y +
								trans[2][2] * originalVerts[v].z +
								trans[2][3])
						* weight;
				++it;
			}
		}
		mAxisAlignedBoxOutOfDate = true;
		mParent.MarkExtentsOutOfDate();
	}

	void SubMesh::Render(RenderContext& renderContext, const RenderPass& pass, Colour compoundDiffuse)
	{
		if(!mVertexBuffer)
		{
			return;
		}
		
		RenderTarget& renderTarget = renderContext.mRenderTarget;

		// This is for backward compatibility
		renderTarget.SetVertexColourEnabled(pass.GetVertexColouringEnabled());
		for(Size i = 0; i < pass.GetNumTextureUnits(); ++i)
		{
			Size setToUse = pass.GetTextureUnit(i)->GetTextureUnitSetToUse();
			renderTarget.SetTextureCoordinateSourceIndex(i,setToUse);
		}

		//Reset the arrays
		renderTarget.ClearSources();
		if(pass.mProgram)
		{
			//Make sure the program is built
			renderTarget.BuildProgram(pass.mProgram);
			renderTarget.ActivateProgram(pass.mProgram);
		}
		renderTarget.SetVertexBuffer(mVertexBuffer);

		if(mElementBuffer)
		{
			renderTarget.DrawElements(*mElementBuffer);
		}

		if(pass.mProgram)
		{
			renderTarget.DeactivateProgram(pass.mProgram);
		}

		// This is for backward compatibility
		renderTarget.SetVertexColourEnabled(!pass.GetVertexColouringEnabled());

		renderTarget.SetVertexBuffer(nullptr);
	}

	void SubMesh::MarkExtentsOutOfDate()
	{
		// This can only come from Mesh when it is marked out of date (someone knows something
		// I don't about my data).
		mAxisAlignedBoxOutOfDate = true;
	}
	
	void SubMesh::Finalise()
	{
		// When someone knows about my data and tells me but not my Mesh.
		mAxisAlignedBoxOutOfDate = true;
		mParent.MarkExtentsOutOfDate();

		if(mBoneWeights)
		{
			GenerateTransformBuffers();
		}else
		{
			mOriginalVertices.reset();
		}
	}

}
