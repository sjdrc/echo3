#include <echo/Graphics/Terrain.h>
#include <echo/Graphics/Mesh.h>
#include <echo/Graphics/SubMesh.h>
#include <echo/Resource/MeshManager.h>
#include <echo/Resource/TextureManager.h>

namespace Echo
{
	
	Terrain::Terrain(const std::string& terrainName, MeshManager& meshManager, TextureManager& textureManager) :
		mMeshManager(meshManager),
		mTextureManager(textureManager),
		mMeshOutOfDate(true),
		mPixelMode(PixelModes::PIXEL_IS_VERTEX),
		mResolution(10,10,0.1)
	{
		SetMesh(meshManager.CreateMesh(terrainName));
	}
	
	Terrain::~Terrain()
	{
		
	}

	void Terrain::SetTexture(const std::string& textureName)
	{
		mTexture = mTextureManager.GetResource(textureName);
		mMeshOutOfDate=true;
	}

	void Terrain::SetPixelMode(PixelMode pixelMode)
	{
		mPixelMode=pixelMode;
		mMeshOutOfDate=true;
	}

	void Terrain::SetResolution(const Vector3& resolution)
	{
		mResolution=resolution;
		mMeshOutOfDate=true;
	}

	void Terrain::UpdateMesh()
	{
		if(!mTexture)
		{
			ECHO_LOG_ERROR("Null texture.");
			return;
		}
		
		Texture::Format format = mTexture->GetFormat();
		if(format!=Texture::Formats::R8G8B8A8)
		{
			ECHO_LOG_ERROR("Texture format not supported. Must be R8G8B8A8.");
			return;
		}
		switch(mPixelMode)
		{
			case PixelModes::PIXEL_IS_QUAD:
				BuildQuadMesh();
				break;
			case PixelModes::PIXEL_IS_VERTEX:
			default:
				BuildVertexMesh();
				break;
		}
		mMeshOutOfDate=false;
	}
	
	void Terrain::Render(RenderContext& renderContext, Colour compoundDiffuse)
	{
		if(mMeshOutOfDate)
		{
			UpdateMesh();
		}
		SceneEntity::Render(renderContext,compoundDiffuse);
	}
	
	void Terrain::BuildVertexMesh()
	{
		shared_ptr<Mesh> mesh = GetMesh();
		mesh->Clear();
		
		u32 width = mTexture->GetWidth();
		u32 height = mTexture->GetHeight();
		shared_ptr<u8> buffer = mTexture->GetBuffer();
		u32* pixelBuffer = reinterpret_cast<u32*>(buffer.get());
		shared_ptr<SubMesh> subMesh = mesh->CreateCommonSubMesh();
		shared_ptr<VertexBuffer> vertexbuffer = subMesh->GetVertexBuffer();
		vertexbuffer->Allocate(height*width);
		VertexBuffer::Accessor<Vector3> vertices = vertexbuffer->GetAccessor<Vector3>("Position");
		VertexBuffer::Accessor<Vector3> normals = vertexbuffer->GetAccessor<Vector3>("Normal");
		VertexBuffer::Accessor<VertexColour> colours = vertexbuffer->GetAccessor<VertexColour>("Colour");
		VertexBuffer::Accessor<TextureUV> uv0 = vertexbuffer->GetAccessor<TextureUV>("UV0");

		Size numberOfElements = (height-1)*(width-1)*2;
		subMesh->SetElementBuffer(ElementBuffer::Types::STATIC, ElementBuffer::IndexTypes::UNSIGNED_16BIT, ElementBuffer::ElementTypes::TRIANGLE, numberOfElements);
		shared_ptr<ElementBuffer> elementBuffer=subMesh->GetElementBuffer();
		auto triangles = elementBuffer->GetAccessor< ElementBuffer::Triangle<u16> >();
		Size triangleIndex = 0;

		Size numVertices = 0;
		//Build the vertices
		for(u32 py=0; py<height;++py)
		{
			for(u32 px=0; px<width;++px)
			{
				u32 pixel = pixelBuffer[px + py*width];
				u32 depth = (pixel&0xFF000000) >> 24;
				Vector3 position;
				position.x = px * mResolution.x;
				position.y = depth * mResolution.z;
				position.z = py * mResolution.y;
				
				f32 u=static_cast<f32>(px)/static_cast<f32>(width);
				f32 v=static_cast<f32>(py)/static_cast<f32>(height);
				vertices[numVertices] = position;
				normals[numVertices] = Vector3(0,position.y,0);
				colours[numVertices] = Colours::WHITE;
				uv0[numVertices] = TextureUV(u,v);
				++numVertices;
			}
		}
		//Build the triangles
		for(u32 py=0; py<height-1;++py)
		{
			for(u32 px=0; px<width-1;++px)
			{
				u16 x=px + py*width;
				u16 nextX=x+1;
				
				u16 y=px + (py+1)*width;
				u16 nextY=y+1;

				ElementBuffer::Triangle<u16>& triangle1 = triangles[triangleIndex++];
				triangle1.mA=y;
				triangle1.mB=nextX;
				triangle1.mC=x;

				ElementBuffer::Triangle<u16>& triangle2 = triangles[triangleIndex++];
				triangle2.mA=y;
				triangle2.mB=nextY;
				triangle2.mC=nextX;
			}
		}
		mesh->CentreMeshToOrigin();
		subMesh->Finalise();
	}

	void Terrain::BuildQuadMesh()
	{
		ECHO_LOG_ERROR("Not implemented.");
	}
}
