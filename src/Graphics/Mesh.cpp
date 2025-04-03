#include <echo/Graphics/Mesh.h>
#include <echo/Graphics/RenderTarget.h>
#include <echo/Graphics/SubMesh.h>
#include <echo/Graphics/RenderPass.h>
#include <echo/Kernel/ScopedLock.h>
#include <boost/foreach.hpp>
#include <limits>
#include <iostream>

namespace Echo
{
	Mesh::Mesh() : Resource<Mesh>(true),
		mSubMeshNames(0),
		mAxisAlignedBox(Vector3::ZERO,Vector3::ZERO), //Subtle difference between NULL and zero.
		mAxisAlignedBoxOutOfDate(true),
		mUseSkeleton(false)
	{
	}

	Mesh::~Mesh()
	{
		Clear();
	}

	Vector3 Mesh::GetCentre() const
	{
		UpdateAxisAlignedBox();
		return mAxisAlignedBox.GetCentre();
	}

	BoneBinding* Mesh::CreateBinding(const std::map< Size, f32 >& bindingMap)
	{
		//Find an existing binding
		for(u32 b = 0; b < mBoneBindings.size(); ++b)
		{
			if((*mBoneBindings[b]) == bindingMap)
			{
				return mBoneBindings[b];
			}
		}
		//None? Create a new one
		BoneBinding* binding = new BoneBinding();
		binding->mBoneWeights = bindingMap;
		mBoneBindings.push_back(binding);
		return binding;
	}

	void Mesh::CacheBindings()
	{
		if(mSkeleton)
		{
			for(u32 b = 0; b < mBoneBindings.size(); ++b)
			{
				BoneBinding* binding = mBoneBindings[b];
				binding->mPosition.SetZero();
				binding->mScale.Set(1, 1, 1);
				binding->mOrientation = Quaternion::IDENTITY;
				std::map< Size, f32 >::iterator bit = (*mBoneBindings[b]).mBoneWeights.begin();
				std::map< Size, f32 >::iterator bitEnd = (*mBoneBindings[b]).mBoneWeights.end();
				while(bit != bitEnd)
				{
					shared_ptr<Bone> bone = mSkeleton->GetBone(bit->first);

					Vector3 pos = Maths::LinearInterpolate(Vector3(0, 0, 0), bone->GetDerivedPosition(), bit->second);
					Vector3 scaleInter = Maths::LinearInterpolate(Vector3(1, 1, 1), bone->GetDerivedScale(), bit->second);
					Quaternion orientation = Quaternion::IDENTITY.Nlerp(bone->GetDerivedOrientation(), bit->second, true);

					Vector3 scale = scaleInter * bone->GetBindDerivedInverseScale();
					Quaternion rotate = orientation * bone->GetBindDerivedInverseOrientation();
					Vector3 translate = pos + rotate * (scale * bone->GetBindDerivedInversePosition());

					binding->mOffsetTransform.MakeTransform(translate, scale, rotate);

					++bit;
				}
			}
		}
	}

	shared_ptr<SubMesh> Mesh::CreateSubMesh(std::string name)
	{
		if(name.empty())
		{
			std::stringstream ss;
			while(HasSubmesh(ss.str()))
			{
				ss.clear();
				++mSubMeshNames;
				ss << mSubMeshNames;
			}
			name = ss.str();
		}else
		{
			if(HasSubmesh(name))
			{
				ECHO_LOG_ERROR("Mesh with name '" << name << "' already exists");
				return shared_ptr<SubMesh>();
			}
		}
		shared_ptr<SubMesh> subMesh = shared_ptr<SubMesh>(new SubMesh(name, *this));
		mSubMeshes.push_back(subMesh);
		return subMesh;
	}
	
	shared_ptr<SubMesh> Mesh::CreateCommonSubMesh(std::string name)
	{
		shared_ptr<SubMesh> submesh = CreateSubMesh(name);
		if(!submesh)
		{
			return nullptr;
		}
		
		shared_ptr<VertexBuffer> vertexBuffer = submesh->GetVertexBuffer(VertexBuffer::Types::DYNAMIC);
		vertexBuffer->AddVertexAttribute("Position",VertexAttribute(VertexAttribute::ComponentTypes::VECTOR3, 1));	//Position
		vertexBuffer->AddVertexAttribute("Normal",VertexAttribute(VertexAttribute::ComponentTypes::VECTOR3, 1));	//Normal
		vertexBuffer->AddVertexAttribute("Colour",VertexAttribute(VertexAttribute::ComponentTypes::COLOUR_8, 1));	//Colour
		vertexBuffer->AddVertexAttribute("UV0",VertexAttribute(VertexAttribute::ComponentTypes::TEXTUREUV, 1));		//UV0
		vertexBuffer->AddVertexAttribute("Tangent",VertexAttribute(VertexAttribute::ComponentTypes::VECTOR3, 1));	//Tangent

		return submesh;
	}
	
	shared_ptr<SubMesh> Mesh::CreateQuadSubMesh(f32 dimension)
	{
		return CreateQuadSubMesh(dimension, dimension);
	}
	
	shared_ptr<SubMesh> Mesh::CreateQuadSubMesh(f32 width, f32 height)
	{
		return CreateQuadSubMesh(width, height, TextureUVPair{ TextureUV(0.f, 0.f), TextureUV(1.f, 1.f) } );
	}
	
	shared_ptr<SubMesh> Mesh::CreateQuadSubMesh(f32 width, f32 height, const TextureUVPair& textureUVPair)
	{
		shared_ptr<SubMesh> quad = CreateCommonSubMesh();
		shared_ptr<VertexBuffer> vertexBuffer = quad->GetVertexBuffer();
		vertexBuffer->Allocate(4);
		
		f32 halfWidth = width/2.f;
		f32 halfHeight = height/2.f;
		
		VertexBuffer::Accessor<Vector3> positions = vertexBuffer->GetAccessor<Vector3>(0);
		VertexBuffer::Accessor<Vector3> normals = vertexBuffer->GetAccessor<Vector3>(1);
		VertexBuffer::Accessor<VertexColour> colours = vertexBuffer->GetAccessor<VertexColour>(2);
		VertexBuffer::Accessor<TextureUV> uv0 = vertexBuffer->GetAccessor<TextureUV>(3);
		normals[0] = positions[0] = Vector3(-halfWidth,  halfHeight, 0.f);
		normals[1] = positions[1] = Vector3( halfWidth,  halfHeight, 0.f);
		normals[2] = positions[2] = Vector3(-halfWidth, -halfHeight, 0.f);
		normals[3] = positions[3] = Vector3( halfWidth, -halfHeight, 0.f);
		colours[0] = colours[1] = colours[2] = colours[3] = Colours::WHITE;
		
		const auto& topLeft = textureUVPair.first;
		const auto& bottomRight = textureUVPair.second;
		uv0[0] = TextureUV(topLeft.u,     topLeft.v);
		uv0[1] = TextureUV(bottomRight.u, topLeft.v);
		uv0[2] = TextureUV(topLeft.u,     bottomRight.v);
		uv0[3] = TextureUV(bottomRight.u, bottomRight.v);
		
		quad->SetElementBuffer(ElementBuffer::Types::STATIC, ElementBuffer::IndexTypes::UNSIGNED_16BIT, ElementBuffer::ElementTypes::TRIANGLE, 2);
		shared_ptr<ElementBuffer> elementBuffer=quad->GetElementBuffer();
		auto elements = elementBuffer->GetAccessor< ElementBuffer::Triangle<u16> >();
		ElementBuffer::Triangle<u16>& triangle1 = elements[0];
		triangle1.mA = 0;
		triangle1.mB = 2;
		triangle1.mC = 1;
		ElementBuffer::Triangle<u16>& triangle2 = elements[1];
		triangle2.mA = 3;
		triangle2.mB = 1;
		triangle2.mC = 2;
		quad->Finalise();
		
		return quad;
	}

	shared_ptr<SubMesh> Mesh::CreateCubeSubMesh(Vector3 extents, Size xResolution, Size yResolution, Size zResolution)
	{
		shared_ptr<SubMesh> sm = CreateCommonSubMesh();
		shared_ptr<VertexBuffer> vertexBuffer = sm->GetVertexBuffer();

		const Size NUM_VERTS_PER_QUAD = 4;
		const Size TOTAL_NUMBER_OF_VERTICES =
			(xResolution * yResolution * NUM_VERTS_PER_QUAD * 2)+		// XY * 2
			(xResolution * zResolution * NUM_VERTS_PER_QUAD * 2)+		// XZ * 2
			(yResolution * zResolution * NUM_VERTS_PER_QUAD * 2);		// YZ * 2
		vertexBuffer->Allocate(TOTAL_NUMBER_OF_VERTICES);

		VertexBuffer::Accessor<Vector3> positions = vertexBuffer->GetAccessor<Vector3>(0);
		VertexBuffer::Accessor<Vector3> normals = vertexBuffer->GetAccessor<Vector3>(1);
		VertexBuffer::Accessor<VertexColour> colours = vertexBuffer->GetAccessor<VertexColour>(2);
		VertexBuffer::Accessor<TextureUV> uv0 = vertexBuffer->GetAccessor<TextureUV>(3);
		
		const f32 hX = extents.x/2.f;
		const f32 hY = extents.y/2.f;
		const f32 hZ = extents.z/2.f;
		const f32 xStep = extents.x / xResolution;
		const f32 yStep = extents.y / yResolution;
		const f32 zStep = extents.z / zResolution;
		Size faceBase = 0;
		Size triangleIndex = 0;
		Size numberOfElements = ((2 * xResolution * yResolution) + (2 * zResolution * yResolution) + (2 * xResolution * zResolution))*2;
		sm->SetElementBuffer(ElementBuffer::Types::STATIC, ElementBuffer::IndexTypes::UNSIGNED_16BIT, ElementBuffer::ElementTypes::TRIANGLE, numberOfElements);
		shared_ptr<ElementBuffer> elementBuffer=sm->GetElementBuffer();
		auto triangles = elementBuffer->GetAccessor< ElementBuffer::Triangle<u16> >();
		
		//Z+
		for(Size x=0;x<xResolution;++x)
		{
			for(Size y=0;y<yResolution;++y)
			{
				const f32 uStep = (1.f/xResolution);
				const f32 vStep = (1.f/yResolution);
				f32 u = 1.f - (x*uStep);
				f32 s = 1.f - ((x+1)*uStep);
				f32 v = (y+1) * vStep;
				f32 t = y * vStep;
				f32 xP0 = hX - (x*xStep);
				f32 xP1 = hX - ((x+1)*xStep);
				f32 yP0 = hY - (y*yStep);
				f32 yP1 = hY - ((y+1)*yStep);

				positions[faceBase]		= Vector3(xP0, yP1, hZ);
				normals[faceBase]		= Vector3(0, 0, hZ).NormalisedCopy();
				colours[faceBase]		= Colours::WHITE;
				uv0[faceBase]			= TextureUV(u, v);

				positions[faceBase+1]	= Vector3(xP1, yP1, hZ);
				normals[faceBase+1]		= Vector3(0, 0, hZ).NormalisedCopy();
				colours[faceBase+1]		= Colours::WHITE;
				uv0[faceBase+1]			= TextureUV(s, v);

				positions[faceBase+2]	= Vector3(xP0, yP0, hZ);
				normals[faceBase+2]		= Vector3(0, 0, hZ).NormalisedCopy();
				colours[faceBase+2]		= Colours::WHITE;
				uv0[faceBase+2]			= TextureUV(u, t);

				positions[faceBase+3]	= Vector3(xP1, yP0, hZ);
				normals[faceBase+3]		= Vector3(0, 0, hZ).NormalisedCopy();
				colours[faceBase+3]		= Colours::WHITE;
				uv0[faceBase+3]			= TextureUV(s, t);

				ElementBuffer::Triangle<u16>& triangle1 = triangles[triangleIndex];
				triangle1.mA=faceBase+2;
				triangle1.mB=faceBase+1;
				triangle1.mC=faceBase;
				triangleIndex++;

				ElementBuffer::Triangle<u16>& triangle2 = triangles[triangleIndex];
				triangle2.mA=faceBase+3;
				triangle2.mB=faceBase+1;
				triangle2.mC=faceBase+2;
				triangleIndex++;
				faceBase+=NUM_VERTS_PER_QUAD;
			}
		}

		//Z-
		for(Size x=0;x<xResolution;++x)
		{
			for(Size y=0;y<yResolution;++y)
			{
				const f32 uStep = (1.f/xResolution);
				const f32 vStep = (1.f/yResolution);
				f32 u = 1.f - (x*uStep);
				f32 s = 1.f - ((x+1)*uStep);
				f32 v = (y+1) * vStep;
				f32 t = y * vStep;
				f32 xP0 = -hX + (x*xStep);
				f32 xP1 = -hX + ((x+1)*xStep);
				f32 yP0 = hY - (y*yStep);
				f32 yP1 = hY - ((y+1)*yStep);
				
				positions[faceBase]		= Vector3(xP0, yP1, -hZ);
				normals[faceBase]		= Vector3(0, 0, -hZ).NormalisedCopy();
				colours[faceBase]		= Colours::WHITE;
				uv0[faceBase]			= TextureUV(u, v);

				positions[faceBase+1]	= Vector3(xP1, yP1, -hZ);
				normals[faceBase+1]		= Vector3(0, 0, -hZ).NormalisedCopy();
				colours[faceBase+1]		= Colours::WHITE;
				uv0[faceBase+1]			= TextureUV(s, v);

				positions[faceBase+2]	= Vector3(xP0, yP0, -hZ);
				normals[faceBase+2]		= Vector3(0, 0, -hZ).NormalisedCopy();
				colours[faceBase+2]		= Colours::WHITE;
				uv0[faceBase+2]			= TextureUV(u, t);

				positions[faceBase+3]	= Vector3(xP1, yP0, -hZ);
				normals[faceBase+3]		= Vector3(0, 0, -hZ).NormalisedCopy();
				colours[faceBase+3]		= Colours::WHITE;
				uv0[faceBase+3]			= TextureUV(s, t);
				
				ElementBuffer::Triangle<u16>& triangle1 = triangles[triangleIndex];
				triangle1.mA=faceBase+2;
				triangle1.mB=faceBase+1;
				triangle1.mC=faceBase;
				triangleIndex++;

				ElementBuffer::Triangle<u16>& triangle2 = triangles[triangleIndex];
				triangle2.mA=faceBase+3;
				triangle2.mB=faceBase+1;
				triangle2.mC=faceBase+2;
				triangleIndex++;
				faceBase+=NUM_VERTS_PER_QUAD;
			}
		}

		//X+
		for(Size x=0;x<zResolution;++x)
		{
			for(Size y=0;y<yResolution;++y)
			{
				const f32 uStep = (1.f/zResolution);
				const f32 vStep = (1.f/yResolution);
				f32 u = 1.f - (x*uStep);
				f32 s = 1.f - ((x+1)*uStep);
				f32 v = (y+1) * vStep;
				f32 t = y * vStep;
				f32 xP0 = -hZ + (x*zStep);
				f32 xP1 = -hZ + ((x+1)*zStep);
				f32 yP0 = hY - (y*yStep);
				f32 yP1 = hY - ((y+1)*yStep);
				positions[faceBase]		= Vector3(hX, yP1, xP0);
				normals[faceBase]		= Vector3(hX, 0, 0).NormalisedCopy();
				colours[faceBase]		= Colours::WHITE;
				uv0[faceBase]			= TextureUV(u, v);

				positions[faceBase+1]	= Vector3(hX, yP1, xP1);
				normals[faceBase+1]		= Vector3(hX, 0, 0).NormalisedCopy();
				colours[faceBase+1]		= Colours::WHITE;
				uv0[faceBase+1]			= TextureUV(s, v);

				positions[faceBase+2]	= Vector3(hX, yP0, xP0);
				normals[faceBase+2]		= Vector3(hX, 0, 0).NormalisedCopy();
				colours[faceBase+2]		= Colours::WHITE;
				uv0[faceBase+2]			= TextureUV(u, t);

				positions[faceBase+3]	= Vector3(hX, yP0, xP1);
				normals[faceBase+3]		= Vector3(hX, 0, 0).NormalisedCopy();
				colours[faceBase+3]		= Colours::WHITE;
				uv0[faceBase+3]			= TextureUV(s, t);
				
				ElementBuffer::Triangle<u16>& triangle1 = triangles[triangleIndex];
				triangle1.mA=faceBase+2;
				triangle1.mB=faceBase+1;
				triangle1.mC=faceBase;
				triangleIndex++;

				ElementBuffer::Triangle<u16>& triangle2 = triangles[triangleIndex];
				triangle2.mA=faceBase+3;
				triangle2.mB=faceBase+1;
				triangle2.mC=faceBase+2;
				triangleIndex++;
				faceBase+=NUM_VERTS_PER_QUAD;
			}
		}

		//X-
		for(Size x=0;x<zResolution;++x)
		{
			for(Size y=0;y<yResolution;++y)
			{
				const f32 uStep = (1.f/zResolution);
				const f32 vStep = (1.f/yResolution);
				f32 u = 1.f - (x*uStep);
				f32 s = 1.f - ((x+1)*uStep);
				f32 v = (y+1) * vStep;
				f32 t = y * vStep;
				f32 xP0 = hZ - (x*zStep);
				f32 xP1 = hZ - ((x+1)*zStep);
				f32 yP0 = hY - (y*yStep);
				f32 yP1 = hY - ((y+1)*yStep);
				
				positions[faceBase]		= Vector3(-hX, yP1, xP0);
				normals[faceBase]		= Vector3(-hX, 0, 0).NormalisedCopy();
				colours[faceBase]		= Colours::WHITE;
				uv0[faceBase]			= TextureUV(u, v);

				positions[faceBase+1]	= Vector3(-hX, yP1, xP1);
				normals[faceBase+1]		= Vector3(-hX, 0, 0).NormalisedCopy();
				colours[faceBase+1]		= Colours::WHITE;
				uv0[faceBase+1]			= TextureUV(s, v);

				positions[faceBase+2]	= Vector3(-hX, yP0, xP0);
				normals[faceBase+2]		= Vector3(-hX, 0, 0).NormalisedCopy();
				colours[faceBase+2]		= Colours::WHITE;
				uv0[faceBase+2]			= TextureUV(u, t);

				positions[faceBase+3]	= Vector3(-hX, yP0, xP1);
				normals[faceBase+3]		= Vector3(-hX, 0, 0).NormalisedCopy();
				colours[faceBase+3]		= Colours::WHITE;
				uv0[faceBase+3]			= TextureUV(s, t);

				ElementBuffer::Triangle<u16>& triangle1 = triangles[triangleIndex];
				triangle1.mA=faceBase+2;
				triangle1.mB=faceBase+1;
				triangle1.mC=faceBase;
				triangleIndex++;

				ElementBuffer::Triangle<u16>& triangle2 = triangles[triangleIndex];
				triangle2.mA=faceBase+3;
				triangle2.mB=faceBase+1;
				triangle2.mC=faceBase+2;
				triangleIndex++;
				faceBase+=NUM_VERTS_PER_QUAD;
			}
		}

		//Y-
		for(Size x=0;x<xResolution;++x)
		{
			for(Size y=0;y<zResolution;++y)
			{
				const f32 uStep = (1.f/xResolution);
				const f32 vStep = (1.f/zResolution);
				f32 u = 1.f - (x*uStep);
				f32 s = 1.f - ((x+1)*uStep);
				f32 v = (y+1) * vStep;
				f32 t = y * vStep;
				f32 xP0 = hX - (x*xStep);
				f32 xP1 = hX - ((x+1)*xStep);
				f32 yP0 = hZ - (y*zStep);
				f32 yP1 = hZ - ((y+1)*zStep);
				
				positions[faceBase]		= Vector3(xP0, -hY, yP1);
				normals[faceBase]		= Vector3(0, -hY, 0).NormalisedCopy();
				colours[faceBase]		= Colours::WHITE;
				uv0[faceBase]			= TextureUV(u, v);

				positions[faceBase+1]	= Vector3(xP1, -hY, yP1);
				normals[faceBase+1]		= Vector3(0, -hY, 0).NormalisedCopy();
				colours[faceBase+1]		= Colours::WHITE;
				uv0[faceBase+1]			= TextureUV(s, v);

				positions[faceBase+2]	= Vector3(xP0, -hY, yP0);
				normals[faceBase+2]		= Vector3(0, -hY, 0).NormalisedCopy();
				colours[faceBase+2]		= Colours::WHITE;
				uv0[faceBase+2]			= TextureUV(u, t);

				positions[faceBase+3]	= Vector3(xP1, -hY, yP0);
				normals[faceBase+3]		= Vector3(0, -hY, 0).NormalisedCopy();
				colours[faceBase+3]		= Colours::WHITE;
				uv0[faceBase+3]			= TextureUV(s, t);
				
				ElementBuffer::Triangle<u16>& triangle1 = triangles[triangleIndex];
				triangle1.mA=faceBase+2;
				triangle1.mB=faceBase+1;
				triangle1.mC=faceBase;
				triangleIndex++;

				ElementBuffer::Triangle<u16>& triangle2 = triangles[triangleIndex];
				triangle2.mA=faceBase+3;
				triangle2.mB=faceBase+1;
				triangle2.mC=faceBase+2;
				triangleIndex++;
				faceBase+=NUM_VERTS_PER_QUAD;
			}
		}

		//Y+
		for(Size x=0;x<xResolution;++x)
		{
			for(Size y=0;y<zResolution;++y)
			{
				const f32 uStep = (1.f/xResolution);
				const f32 vStep = (1.f/zResolution);
				f32 u = 1.f - (x*uStep);
				f32 s = 1.f - ((x+1)*uStep);
				f32 v = (y+1) * vStep;
				f32 t = y * vStep;
				f32 xP0 = hX - (x*xStep);
				f32 xP1 = hX - ((x+1)*xStep);
				f32 yP0 = -hZ + (y*zStep);
				f32 yP1 = -hZ + ((y+1)*zStep);
				
				positions[faceBase]		= Vector3(xP0, hY, yP1);
				normals[faceBase]		= Vector3(0, hY, 0).NormalisedCopy();
				colours[faceBase]		= Colours::WHITE;
				uv0[faceBase]			= TextureUV(u, v);

				positions[faceBase+1]	= Vector3(xP1, hY, yP1);
				normals[faceBase+1]		= Vector3(0, hY, 0).NormalisedCopy();
				colours[faceBase+1]		= Colours::WHITE;
				uv0[faceBase+1]			= TextureUV(s, v);

				positions[faceBase+2]	= Vector3(xP0, hY, yP0);
				normals[faceBase+2]		= Vector3(0, hY, 0).NormalisedCopy();
				colours[faceBase+2]		= Colours::WHITE;
				uv0[faceBase+2]			= TextureUV(u, t);

				positions[faceBase+3]	= Vector3(xP1, hY, yP0);
				normals[faceBase+3]		= Vector3(0, hY, 0).NormalisedCopy();
				colours[faceBase+3]		= Colours::WHITE;
				uv0[faceBase+3]			= TextureUV(s, t);
				
				ElementBuffer::Triangle<u16>& triangle1 = triangles[triangleIndex];
				triangle1.mA=faceBase+2;
				triangle1.mB=faceBase+1;
				triangle1.mC=faceBase;
				triangleIndex++;

				ElementBuffer::Triangle<u16>& triangle2 = triangles[triangleIndex];
				triangle2.mA=faceBase+3;
				triangle2.mB=faceBase+1;
				triangle2.mC=faceBase+2;
				triangleIndex++;
				faceBase+=NUM_VERTS_PER_QUAD;
			}
		}
		sm->GenerateTangents(true);
		sm->Finalise();
		return sm;
	}
	
	shared_ptr<SubMesh> Mesh::CreateUVSphereSubMesh(Scalar radius, Size meridians, Size parallels, bool uvsVertical)
	{
		// The code below was adapted from https://github.com/caosdoar/spheres Thanks to Oscar Sebio Cajaraville. At the
		// time of including this a license file was not included. But I contacted Oscar and received the following response:
		// Hi Sean,
		// 
		// First sorry for not having answer you sooner.
		// I am glad you found the reading interesting. Feel free to use the code or modify it with any purpose. The code
		// is so simple that I didn't thought in adding a license to it.
		// If you need a license for legal reasons I will make a commit adding a non restrictive one (MIT license).
		// 
		// Cheers.
		// Oscar S.C.
		
		shared_ptr<SubMesh> sm = CreateCommonSubMesh();
		shared_ptr<VertexBuffer> vertexBuffer = sm->GetVertexBuffer();
		
		if(meridians == 0)
		{
			meridians=radius * 10;
		}
		if(parallels==0)
		{
			parallels=radius * 10;
		}
		
		const Size TOTAL_NUMBER_OF_VERTICES = ((parallels-1) * meridians) + 2;
		vertexBuffer->Allocate(TOTAL_NUMBER_OF_VERTICES);

		VertexBuffer::Accessor<Vector3> positions = vertexBuffer->GetAccessor<Vector3>(0);
		VertexBuffer::Accessor<VertexColour> colours = vertexBuffer->GetAccessor<VertexColour>(2);
		VertexBuffer::Accessor<TextureUV> uv0 = vertexBuffer->GetAccessor<TextureUV>(3);
		
		Size vertexIndex = 0;
		positions[vertexIndex] = Vector3(0.0f, 1.0f, 0.0f) * radius;
		colours[vertexIndex] = Colours::WHITE;
		uv0[vertexIndex] = uvsVertical ? TextureUV(0.f, 0.5f) : TextureUV(0.5f, 0.f);
		++vertexIndex;
		
		for (Size j = 0; j < parallels - 1; ++j)
		{
			const Scalar yPercent = Scalar(j+1) / Scalar(parallels);
			const Scalar polar = M_PI * yPercent;
			const Scalar sp = std::sin(polar);
			const Scalar cp = std::cos(polar);
			for (uint32_t i = 0; i < meridians; ++i)
			{
				const Scalar xPercent = Scalar(i) / Scalar(meridians);
				const Scalar azimuth = 2.0 * M_PI * xPercent;
				const Scalar sa = std::sin(azimuth);
				const Scalar ca = std::cos(azimuth);
				const Scalar x = sp * ca;
				const Scalar y = cp;
				const Scalar z = sp * sa;
				
				positions[vertexIndex] = Vector3(x, y, z) * radius;
				colours[vertexIndex] = Colours::WHITE;
				uv0[vertexIndex] = uvsVertical ? TextureUV(xPercent, yPercent) : TextureUV(yPercent, xPercent);
				++vertexIndex;
			}
		}
		positions[vertexIndex] = Vector3(0.0f, -1.0f, 0.0f) * radius;
		colours[vertexIndex] = Colours::WHITE;
		uv0[vertexIndex] = uvsVertical ? TextureUV(1.f, 0.5f) : TextureUV(0.5f, 1.f);

		Size numberOfElements = (2 * meridians) + ((parallels - 2) * meridians * 2);
		sm->SetElementBuffer(ElementBuffer::Types::STATIC, ElementBuffer::IndexTypes::UNSIGNED_16BIT, ElementBuffer::ElementTypes::TRIANGLE, numberOfElements);
		shared_ptr<ElementBuffer> elementBuffer=sm->GetElementBuffer();
		auto triangles = elementBuffer->GetAccessor< ElementBuffer::Triangle<u16> >();
		Size triangleIndex = 0;
		
		// That was the last vertex, no need to increment
		const Size LAST_VERTEX_INDEX = vertexIndex;

		for (Size i = 0; i < meridians; ++i)
		{
			const Size a = i + 1;
			const Size b = (i + 1) % meridians + 1;
			auto& triangle = triangles[triangleIndex];
			triangle.mA = 0;
			triangle.mB = b;
			triangle.mC = a;
			triangleIndex++;
		}		

		for (Size j = 0; j < parallels - 2; ++j)
		{
			Size aStart = j * meridians + 1;
			Size bStart = (j + 1) * meridians + 1;
			for (Size i = 0; i < meridians; ++i)
			{
				const Size a = aStart + i;
				const Size a1 = aStart + (i + 1) % meridians;
				const Size b = bStart + i;
				const Size b1 = bStart + (i + 1) % meridians;
				auto& triangle = triangles[triangleIndex];
				triangle.mA = a;
				triangle.mB = a1;
				triangle.mC = b1;
				triangleIndex++;
				auto& triangle2 = triangles[triangleIndex];
				triangle2.mA = a;
				triangle2.mB = b1;
				triangle2.mC = b;
				triangleIndex++;
			}
		}
		
		for (Size i = 0; i < meridians; ++i)
		{
			const Size baseLine = meridians * (parallels - 2) + 1;
			const Size a = baseLine + i;
			const Size b = baseLine + ((i + 1) % meridians);
			auto& triangle = triangles[triangleIndex];
			triangle.mA = LAST_VERTEX_INDEX;
			triangle.mB = a;
			triangle.mC = b;
			triangleIndex++;
		}
		
		sm->GenerateNormals();
		sm->GenerateTangents(true);
		sm->Finalise();

		return sm;
	}
	
	shared_ptr<SubMesh> Mesh::GetSubMesh(const std::string& name) const
	{
		for(u32 s = 0; s < mSubMeshes.size(); ++s)
		{
			if(mSubMeshes[s]->GetName() == name)
				return mSubMeshes[s];
		}
		return shared_ptr<SubMesh>();
	}

	shared_ptr<SubMesh> Mesh::GetSubMesh(u32 index) const
	{
		if(index < mSubMeshes.size())
			return mSubMeshes[index];
		return shared_ptr<SubMesh>();
	}

	shared_ptr<Mesh> Mesh::Clone() const
	{
		Mesh* mesh = new Mesh();
		for(u32 s = 0; s < mSubMeshes.size(); ++s)
		{
			mesh->mSubMeshes.push_back(mSubMeshes[s]->Clone(*mesh));
		}
		return shared_ptr<Mesh>(mesh);
	}

	
	bool Mesh::RemoveSubmesh(const std::string& name)
	{
		for(u32 s = 0; s < mSubMeshes.size(); ++s)
		{
			if(mSubMeshes[s]->GetName() == name)
			{
				mSubMeshes[s] = mSubMeshes[mSubMeshes.size() - 1];
				mSubMeshes.pop_back();
				return true;
			}
		}
		return false;
	}
	
	bool Mesh::HasSubmesh(const std::string& name) const
	{
		for(u32 s = 0; s < mSubMeshes.size(); ++s)
		{
			if(mSubMeshes[s]->GetName() == name)
				return true;
		}
		return false;
	}

	void Mesh::Clear()
	{
		mSubMeshNames = 0;
		mSubMeshes.clear();
		{
			ScopedLock lock(mAxisAlignedBoxMutex);
			mAxisAlignedBoxOutOfDate = true;
		}
		mSkeleton.reset();
		mBoneBindings.clear();
		mUseSkeleton = false;
	}

	void Mesh::SetMaterial(shared_ptr<Material> material)
	{
		for(u32 s = 0; s < mSubMeshes.size(); ++s)
		{
			mSubMeshes[s]->SetMaterial(material);
		}
	}

	const AxisAlignedBox& Mesh::GetAxisAlignedBox() const
	{
		UpdateAxisAlignedBox();
		return mAxisAlignedBox;
	}

	void Mesh::Render(RenderContext& renderContext, const Matrix4& world, const Matrix4& worldView, Colour compoundDiffuse)
	{
		if(mSkeleton)
		{
			//Update transforms if required.
			mSkeleton->UpdateTransforms();
		}
		renderContext.mRenderTarget.SetModelViewMatrix(worldView);
		for(Size i = 0; i < mSubMeshes.size(); ++i)
		{
			mSubMeshes[i]->Render(renderContext, world, worldView, compoundDiffuse);
		}
	}

	void Mesh::UpdateAxisAlignedBox() const
	{
		{
			ScopedLock lock(mAxisAlignedBoxMutex);
			if(!mAxisAlignedBoxOutOfDate)
			{
				return;
			}
			//Start with a null box so the logic of merging other boxes makes sense.
			mAxisAlignedBox.SetNull();
			mAxisAlignedBox.SetExtents(Vector3::ZERO,Vector3::ZERO);
			if(!mSubMeshes.empty())
			{
				for(Size i = 0; i < mSubMeshes.size(); ++i)
				{
					mAxisAlignedBox.Merge(mSubMeshes[i]->GetAxisAlignedBox());
				}
			}
			mAxisAlignedBoxOutOfDate = false;
		}
		BOOST_FOREACH(Listener* listener, mListeners)
		{
			listener->OnMeshAABBUpdated(*this);
		}
	}

	void Mesh::CentreMeshToOrigin()
	{
		Vector3 translation = -GetCentre();
		for(Size i = 0; i < mSubMeshes.size(); ++i)
		{
			mSubMeshes[i]->TranslateVertices(translation);
		}
		//Move the AABB so it doens't have to be recalculated again.
		mAxisAlignedBox.Translate(translation);
	}
	
	void Mesh::SetDiffuseColour(const Colour& diffuseColour)
	{
		for(Size s = 0; s < mSubMeshes.size(); ++s)
		{
			shared_ptr<Material> material=mSubMeshes[s]->GetMaterial();
			if(material)
			{
				RenderPass* pass = material->GetPass(0);
				if(pass)
				{
					pass->SetDiffuse(diffuseColour);
				}
			}
		}
	}
	
	void Mesh::MarkExtentsOutOfDate() const
	{
		mAxisAlignedBoxOutOfDate = true;
		for(Size s = 0; s < mSubMeshes.size(); ++s)
		{
			mSubMeshes[s]->MarkExtentsOutOfDate();
		}
		BOOST_FOREACH(Listener* listener, mListeners)
		{
			listener->OnMeshAABBNeedsUpdate(*this);
		}
	}
	
	void Mesh::AddListener(Listener* listener)
	{
		if(listener)
		{
			mListeners.push_back(listener);
		}
	}

	void Mesh::RemoveListener(Listener* listener)
	{
		mListeners.remove(listener);
	}

	bool Mesh::_Unload()
	{
		Clear();
		return true;
	}

	Size Mesh::OnRequestMemoryRelease()
	{
		return 0;
	}

}
