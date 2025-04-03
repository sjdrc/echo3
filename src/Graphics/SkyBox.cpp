#include <echo/Graphics/SkyBox.h>
#include <echo/Graphics/Mesh.h>
#include <echo/Graphics/SubMesh.h>
#include <echo/Resource/MeshManager.h>
#include <echo/Resource/TextureManager.h>
#include <echo/Resource/MaterialManager.h>

namespace Echo
{
	SkyBox::SkyBox(const std::string& name, MeshManager& meshManager, TextureManager& textureManager, MaterialManager& materialManager) :
		mMeshManager(meshManager),
		mTextureManager(textureManager),
		mMaterialManager(materialManager),
		mMeshOutOfDate(true),
		mDistance(1000.f)
	{
		shared_ptr<Mesh> mesh = meshManager.CreateMesh(name);
		SetMesh(mesh);
		mMaterial = mMaterialManager.CreateMaterial(name);
		mMaterial->SetToDefaultMaterial();
		mMaterial->GetPass(0)->SetCullMode(RenderPass::CullModes::NONE);
	}
	
	SkyBox::~SkyBox()
	{
	}
		
	void SkyBox::SetDistance(f32 distance)
	{
		mDistance=distance;
		mMeshOutOfDate=true;
	}

	void SkyBox::SetTexture(const std::string& textureName)
	{
		mTexture = mTextureManager.GetResource(textureName);
		if(mTexture)
		{
			mMaterial->SetTexture(mTexture);
			mMaterial->GetPass(0)->GetTextureUnit(0)->SetWrapMode(TextureUnit::TextureWrapModes::CLAMP);
		}
		mMeshOutOfDate = true;
	}

	void SkyBox::SetTextureLeft(const std::string& textureName)
	{
		mTextureLeft = mTextureManager.GetResource(textureName);
		if(!mTextureLeft)
		{
			ECHO_LOG_ERROR("Unable to set left texture");
			return;
		}

		if(!mMaterialLeft)
		{
			mMaterialLeft = mMaterialManager.CreateMaterial(textureName);
			mMaterialLeft->SetToDefaultMaterial();
			mMaterialLeft->GetPass(0)->SetCullMode(RenderPass::CullModes::NONE);
		}
		mMaterialLeft->SetTexture(mTextureLeft);
		mMaterialLeft->GetPass(0)->GetTextureUnit(0)->SetWrapMode(TextureUnit::TextureWrapModes::CLAMP_TO_EDGE);
		mMeshOutOfDate = true;
	}
	
	void SkyBox::SetTextureRight(const std::string& textureName)
	{
		mTextureRight = mTextureManager.GetResource(textureName);
		if(!mTextureRight)
		{
			ECHO_LOG_ERROR("Unable to set right texture");
			return;
		}
		if(!mMaterialRight)
		{
			mMaterialRight = mMaterialManager.CreateMaterial(textureName);
			mMaterialRight->SetToDefaultMaterial();
			mMaterialRight->GetPass(0)->SetCullMode(RenderPass::CullModes::NONE);
		}
		mMaterialRight->SetTexture(mTextureRight);
		mMaterialRight->GetPass(0)->GetTextureUnit(0)->SetWrapMode(TextureUnit::TextureWrapModes::CLAMP_TO_EDGE);
		mMeshOutOfDate = true;
	}
	
	void SkyBox::SetTextureFront(const std::string& textureName)
	{
		mTextureFront = mTextureManager.GetResource(textureName);
		if(!mTextureFront)
		{
			ECHO_LOG_ERROR("Unable to set front texture");
			return;
		}
		if(!mMaterialFront)
		{
			mMaterialFront = mMaterialManager.CreateMaterial(textureName);
			mMaterialFront->SetToDefaultMaterial();
			mMaterialFront->GetPass(0)->SetCullMode(RenderPass::CullModes::NONE);
		}
		mMaterialFront->SetTexture(mTextureFront);
		mMaterialFront->GetPass(0)->GetTextureUnit(0)->SetWrapMode(TextureUnit::TextureWrapModes::CLAMP_TO_EDGE);
		mMeshOutOfDate = true;
	}
	
	void SkyBox::SetTextureBack(const std::string& textureName)
	{
		mTextureBack = mTextureManager.GetResource(textureName);
		if(!mTextureBack)
		{
			ECHO_LOG_ERROR("Unable to set back texture");
			return;
		}
		if(!mMaterialBack)
		{
			mMaterialBack = mMaterialManager.CreateMaterial(textureName);
			mMaterialBack->SetToDefaultMaterial();
			mMaterialBack->GetPass(0)->SetCullMode(RenderPass::CullModes::NONE);
		}
		mMaterialBack->SetTexture(mTextureBack);
		mMaterialBack->GetPass(0)->GetTextureUnit(0)->SetWrapMode(TextureUnit::TextureWrapModes::CLAMP_TO_EDGE);
		mMeshOutOfDate = true;
	}
	
	void SkyBox::SetTextureTop(const std::string& textureName)
	{
		mTextureTop = mTextureManager.GetResource(textureName);
		if(!mTextureTop)
		{
			ECHO_LOG_ERROR("Unable to set top texture");
			return;
		}
		if(!mMaterialTop)
		{
			mMaterialTop = mMaterialManager.CreateMaterial(textureName);
			mMaterialTop->SetToDefaultMaterial();
			mMaterialTop->GetPass(0)->SetCullMode(RenderPass::CullModes::NONE);
		}
		mMaterialTop->SetTexture(mTextureTop);
		mMaterialTop->GetPass(0)->GetTextureUnit(0)->SetWrapMode(TextureUnit::TextureWrapModes::CLAMP_TO_EDGE);
		mMeshOutOfDate = true;
	}
	
	void SkyBox::SetTextureBottom(const std::string& textureName)
	{
		mTextureBottom = mTextureManager.GetResource(textureName);
		if(!mTextureBottom)
		{
			ECHO_LOG_ERROR("Unable to set bottom texture");
			return;
		}
		if(!mMaterialBottom)
		{
			mMaterialBottom = mMaterialManager.CreateMaterial(textureName);
			mMaterialBottom->SetToDefaultMaterial();
			mMaterialBottom->GetPass(0)->SetCullMode(RenderPass::CullModes::NONE);
		}
		mMaterialBottom->SetTexture(mTextureBottom);
		mMaterialBottom->GetPass(0)->GetTextureUnit(0)->SetWrapMode(TextureUnit::TextureWrapModes::CLAMP_TO_EDGE);
		mMeshOutOfDate = true;
	}
		
	void SkyBox::UpdateMesh()
	{
		GetMesh()->Clear();
		if(mTexture)
		{
			//A single texture is used so we only need to create one submesh.
			//It is assumed that the coordinates can be sourced using the following:
			// -U--
			// BLFR
			// -D--
			// U=(0.25,0) - (0.5,0.333333)
			// B=(0,0.333333) - (0.25,0.666666)
			// L=(0.25,0.333333) - (0.5,0.666666)
			// F=(0.5,0.333333) - (0.75,0.666666)
			// R=(0.75,0.333333) - (1.0,0.666666)
			// D=(0.25,0.666666) - (0.5,1.0)
			
			shared_ptr<SubMesh> subMesh = GetMesh()->CreateSubMesh();
			shared_ptr<VertexBuffer> vertexBuffer = subMesh->GetVertexBuffer();
			vertexBuffer->AddVertexAttribute("Position", VertexAttribute(VertexAttribute::ComponentTypes::VECTOR3,1));
			vertexBuffer->AddVertexAttribute("UV0", VertexAttribute(VertexAttribute::ComponentTypes::TEXTUREUV,1));
			vertexBuffer->Allocate(24);
			
			VertexBuffer::Accessor<Vector3> vertices = vertexBuffer->GetAccessor<Vector3>("Position");
			VertexBuffer::Accessor<TextureUV> uvs = vertexBuffer->GetAccessor<TextureUV>("UV0");

			subMesh->SetElementBuffer(ElementBuffer::Types::STATIC, ElementBuffer::IndexTypes::UNSIGNED_16BIT, ElementBuffer::ElementTypes::TRIANGLE, 12);
			shared_ptr<ElementBuffer> elementBuffer=subMesh->GetElementBuffer();
			auto triangles = elementBuffer->GetAccessor< ElementBuffer::Triangle<u16> >();
			Size triangleIndex=0;
			u16 indexBase=0;

			{
				//Front
				vertices[indexBase] = Vector3(-mDistance,-mDistance,-mDistance); uvs[indexBase] = TextureUV(0.5f,0.665f);
				vertices[indexBase+1] = Vector3(-mDistance,mDistance,-mDistance); uvs[indexBase+1] = TextureUV(0.5f,0.334f);
				vertices[indexBase+2] = Vector3(mDistance,mDistance,-mDistance); uvs[indexBase+2] = TextureUV(0.75f,0.334);
				vertices[indexBase+3] = Vector3(mDistance,-mDistance,-mDistance); uvs[indexBase+3] = TextureUV(0.75f,0.665);
				ElementBuffer::Triangle<u16>& triangle1 = triangles[triangleIndex];
				triangle1.mA=indexBase;
				triangle1.mB=indexBase+1;
				triangle1.mC=indexBase+2;
				triangleIndex++;

				ElementBuffer::Triangle<u16>& triangle2 = triangles[triangleIndex];
				triangle2.mA=indexBase+2;
				triangle2.mB=indexBase+3;
				triangle2.mC=indexBase;
				triangleIndex++;
				indexBase+=4;
			}
			{
				//Left
				vertices[indexBase] = Vector3(-mDistance,-mDistance,-mDistance); uvs[indexBase] = TextureUV(0.5f,0.665);
				vertices[indexBase+1] = Vector3(-mDistance,mDistance,-mDistance); uvs[indexBase+1] = TextureUV(0.5f,0.334);
				vertices[indexBase+2] = Vector3(-mDistance,-mDistance,mDistance); uvs[indexBase+2] = TextureUV(0.25f,0.665f);
				vertices[indexBase+3] = Vector3(-mDistance,mDistance,mDistance); uvs[indexBase+3] = TextureUV(0.25f,0.334f);
				ElementBuffer::Triangle<u16>& triangle1 = triangles[triangleIndex];
				triangle1.mA=indexBase;
				triangle1.mB=indexBase+1;
				triangle1.mC=indexBase+2;
				triangleIndex++;

				ElementBuffer::Triangle<u16>& triangle2 = triangles[triangleIndex];
				triangle2.mA=indexBase+2;
				triangle2.mB=indexBase+1;
				triangle2.mC=indexBase+3;
				triangleIndex++;
				indexBase+=4;
			}
			{
				//Back
				vertices[indexBase] = Vector3(-mDistance,-mDistance,mDistance); uvs[indexBase] = TextureUV(0.25f,0.665f);
				vertices[indexBase+1] = Vector3(-mDistance,mDistance,mDistance); uvs[indexBase+1] = TextureUV(0.25f,0.334f);
				vertices[indexBase+2] = Vector3(mDistance,mDistance,mDistance); uvs[indexBase+2] = TextureUV(0.f,0.334f);
				vertices[indexBase+3] = Vector3(mDistance,-mDistance,mDistance); uvs[indexBase+3] = TextureUV(0.f,0.665f);
				ElementBuffer::Triangle<u16>& triangle1 = triangles[triangleIndex];
				triangle1.mA=indexBase;
				triangle1.mB=indexBase+1;
				triangle1.mC=indexBase+2;
				triangleIndex++;

				ElementBuffer::Triangle<u16>& triangle2 = triangles[triangleIndex];
				triangle2.mA=indexBase+2;
				triangle2.mB=indexBase+3;
				triangle2.mC=indexBase;
				triangleIndex++;
				indexBase+=4;
			}
			{
				//Right
				vertices[indexBase] = Vector3(mDistance,-mDistance,-mDistance); uvs[indexBase] = TextureUV(0.75f,0.665f);
				vertices[indexBase+1] = Vector3(mDistance,mDistance,-mDistance); uvs[indexBase+1] = TextureUV(0.75f,0.334f);
				vertices[indexBase+2] = Vector3(mDistance,-mDistance,mDistance); uvs[indexBase+2] = TextureUV(1.f,0.665f);
				vertices[indexBase+3] = Vector3(mDistance,mDistance,mDistance); uvs[indexBase+3] = TextureUV(1.f,0.334f);
				ElementBuffer::Triangle<u16>& triangle1 = triangles[triangleIndex];
				triangle1.mA=indexBase;
				triangle1.mB=indexBase+1;
				triangle1.mC=indexBase+2;
				triangleIndex++;

				ElementBuffer::Triangle<u16>& triangle2 = triangles[triangleIndex];
				triangle2.mA=indexBase+2;
				triangle2.mB=indexBase+1;
				triangle2.mC=indexBase+3;
				triangleIndex++;
				indexBase+=4;
			}
			{
				//Top
				vertices[indexBase] = Vector3(-mDistance,mDistance,-mDistance); uvs[indexBase] = TextureUV(0.5f,0.333333f);
				vertices[indexBase+1] = Vector3(-mDistance,mDistance,mDistance); uvs[indexBase+1] = TextureUV(0.25f,0.333333f);
				vertices[indexBase+2] = Vector3(mDistance,mDistance,mDistance); uvs[indexBase+2] = TextureUV(0.25f,0.f);
				vertices[indexBase+3] = Vector3(mDistance,mDistance,-mDistance); uvs[indexBase+3] = TextureUV(0.5f,0.f);
				ElementBuffer::Triangle<u16>& triangle1 = triangles[triangleIndex];
				triangle1.mA=indexBase+2;
				triangle1.mB=indexBase+1;
				triangle1.mC=indexBase;
				triangleIndex++;

				ElementBuffer::Triangle<u16>& triangle2 = triangles[triangleIndex];
				triangle2.mA=indexBase;
				triangle2.mB=indexBase+3;
				triangle2.mC=indexBase+2;
				triangleIndex++;
				indexBase+=4;
			}
			{
				//Bottom
				vertices[indexBase] = Vector3(-mDistance,-mDistance,-mDistance); uvs[indexBase] = TextureUV(0.25f,1.f);
				vertices[indexBase+1] = Vector3(-mDistance,-mDistance,mDistance); uvs[indexBase+1] = TextureUV(0.5f,1.f);
				vertices[indexBase+2] = Vector3(mDistance,-mDistance,mDistance); uvs[indexBase+2] = TextureUV(0.5f,0.666666f);
				vertices[indexBase+3] = Vector3(mDistance,-mDistance,-mDistance); uvs[indexBase+3] = TextureUV(0.25f,0.666666f);
				ElementBuffer::Triangle<u16>& triangle1 = triangles[triangleIndex];
				triangle1.mA=indexBase+2;
				triangle1.mB=indexBase+1;
				triangle1.mC=indexBase;
				triangleIndex++;

				ElementBuffer::Triangle<u16>& triangle2 = triangles[triangleIndex];
				triangle2.mA=indexBase;
				triangle2.mB=indexBase+3;
				triangle2.mC=indexBase+2;
				triangleIndex++;
			}

			subMesh->Finalise();
			GetMesh()->SetMaterial(mMaterial);
		}else
		{
			//A single texture is used so we only need to create one submesh.
			//It is assumed that the coordinates can be sourced using the following:
			// -U--
			// BLFR
			// -D--
			// U=(0.25,0) - (0.5,0.333333)
			// B=(0,0.333333) - (0.25,0.666666)
			// L=(0.25,0.333333) - (0.5,0.666666)
			// F=(0.5,0.333333) - (0.75,0.666666)
			// R=(0.75,0.333333) - (1.0,0.666666)
			// D=(0.25,0.666666) - (0.5,1.0)
			
			if(mMaterialFront)
			{
				shared_ptr<SubMesh> subMesh = GetMesh()->CreateSubMesh();
				shared_ptr<VertexBuffer> vertexBuffer = subMesh->GetVertexBuffer();
				vertexBuffer->AddVertexAttribute("Position", VertexAttribute(VertexAttribute::ComponentTypes::VECTOR3,1));
				vertexBuffer->AddVertexAttribute("UV0", VertexAttribute(VertexAttribute::ComponentTypes::TEXTUREUV,1));
				vertexBuffer->Allocate(4);

				subMesh->SetElementBuffer(ElementBuffer::Types::STATIC, ElementBuffer::IndexTypes::UNSIGNED_16BIT, ElementBuffer::ElementTypes::TRIANGLE, 1);
				shared_ptr<ElementBuffer> elementBuffer=subMesh->GetElementBuffer();
				auto triangles = elementBuffer->GetAccessor< ElementBuffer::Triangle<u16> >();

				VertexBuffer::Accessor<Vector3> vertices = vertexBuffer->GetAccessor<Vector3>("Position");
				VertexBuffer::Accessor<TextureUV> uvs = vertexBuffer->GetAccessor<TextureUV>("UV0");

				//Front
				vertices[0] = Vector3(-mDistance,-mDistance,-mDistance);
				vertices[1] = Vector3(-mDistance,mDistance,-mDistance);
				vertices[2] = Vector3(mDistance,mDistance,-mDistance);
				vertices[3] = Vector3(mDistance,-mDistance,-mDistance);
				uvs[0] = TextureUV(0,1);
				uvs[1] = TextureUV(0,0);
				uvs[2] = TextureUV(1,0);
				uvs[3] = TextureUV(1,1);
				
				ElementBuffer::Triangle<u16>& triangle1 = triangles[0];
				triangle1.mA=0;
				triangle1.mB=1;
				triangle1.mC=2;

				ElementBuffer::Triangle<u16>& triangle2 = triangles[1];
				triangle2.mA=2;
				triangle2.mB=3;
				triangle2.mC=0;
				
				subMesh->Finalise();
				subMesh->SetMaterial(mMaterialFront);
			}

			if(mMaterialLeft)
			{
				shared_ptr<SubMesh> subMesh = GetMesh()->CreateSubMesh();
				shared_ptr<VertexBuffer> vertexBuffer = subMesh->GetVertexBuffer();
				vertexBuffer->AddVertexAttribute("Position", VertexAttribute(VertexAttribute::ComponentTypes::VECTOR3,1));
				vertexBuffer->AddVertexAttribute("UV0", VertexAttribute(VertexAttribute::ComponentTypes::TEXTUREUV,1));
				vertexBuffer->Allocate(4);

				subMesh->SetElementBuffer(ElementBuffer::Types::STATIC, ElementBuffer::IndexTypes::UNSIGNED_16BIT, ElementBuffer::ElementTypes::TRIANGLE, 1);
				shared_ptr<ElementBuffer> elementBuffer=subMesh->GetElementBuffer();
				auto triangles = elementBuffer->GetAccessor< ElementBuffer::Triangle<u16> >();

				VertexBuffer::Accessor<Vector3> vertices = vertexBuffer->GetAccessor<Vector3>("Position");
				VertexBuffer::Accessor<TextureUV> uvs = vertexBuffer->GetAccessor<TextureUV>("UV0");

				//Left
				vertices[0] = Vector3(-mDistance,-mDistance,-mDistance);
				vertices[1] = Vector3(-mDistance,mDistance,-mDistance);
				vertices[2] = Vector3(-mDistance,-mDistance,mDistance);
				vertices[3] = Vector3(-mDistance,mDistance,mDistance);
				uvs[0] = TextureUV(1,1);
				uvs[1] = TextureUV(1,0);
				uvs[2] = TextureUV(0,1);
				uvs[3] = TextureUV(0,0);

				ElementBuffer::Triangle<u16>& triangle1 = triangles[0];
				triangle1.mA=0;
				triangle1.mB=1;
				triangle1.mC=2;

				ElementBuffer::Triangle<u16>& triangle2 = triangles[1];
				triangle2.mA=2;
				triangle2.mB=1;
				triangle2.mC=3;

				subMesh->Finalise();
				subMesh->SetMaterial(mMaterialLeft);
			}

			if(mMaterialBack)
			{
				shared_ptr<SubMesh> subMesh = GetMesh()->CreateSubMesh();
				shared_ptr<VertexBuffer> vertexBuffer = subMesh->GetVertexBuffer();
				vertexBuffer->AddVertexAttribute("Position", VertexAttribute(VertexAttribute::ComponentTypes::VECTOR3,1));
				vertexBuffer->AddVertexAttribute("UV0", VertexAttribute(VertexAttribute::ComponentTypes::TEXTUREUV,1));
				vertexBuffer->Allocate(4);

				subMesh->SetElementBuffer(ElementBuffer::Types::STATIC, ElementBuffer::IndexTypes::UNSIGNED_16BIT, ElementBuffer::ElementTypes::TRIANGLE, 1);
				shared_ptr<ElementBuffer> elementBuffer=subMesh->GetElementBuffer();
				auto triangles = elementBuffer->GetAccessor< ElementBuffer::Triangle<u16> >();
				
				VertexBuffer::Accessor<Vector3> vertices = vertexBuffer->GetAccessor<Vector3>("Position");
				VertexBuffer::Accessor<TextureUV> uvs = vertexBuffer->GetAccessor<TextureUV>("UV0");

				//Back
				vertices[0] = Vector3(-mDistance,-mDistance,mDistance);
				vertices[1] = Vector3(-mDistance,mDistance,mDistance);
				vertices[2] = Vector3(mDistance,mDistance,mDistance);
				vertices[3] = Vector3(mDistance,-mDistance,mDistance);
				uvs[0] = TextureUV(1,1);
				uvs[1] = TextureUV(1,0);
				uvs[2] = TextureUV(0,0);
				uvs[3] = TextureUV(0,1);
				
				ElementBuffer::Triangle<u16>& triangle1 = triangles[0];
				triangle1.mA=0;
				triangle1.mB=1;
				triangle1.mC=2;

				ElementBuffer::Triangle<u16>& triangle2 = triangles[1];
				triangle2.mA=2;
				triangle2.mB=3;
				triangle2.mC=0;

				subMesh->Finalise();
				subMesh->SetMaterial(mMaterialBack);
			}
			
			if(mMaterialRight)
			{
				shared_ptr<SubMesh> subMesh = GetMesh()->CreateSubMesh();
				shared_ptr<VertexBuffer> vertexBuffer = subMesh->GetVertexBuffer();
				vertexBuffer->AddVertexAttribute("Position", VertexAttribute(VertexAttribute::ComponentTypes::VECTOR3,1));
				vertexBuffer->AddVertexAttribute("UV0", VertexAttribute(VertexAttribute::ComponentTypes::TEXTUREUV,1));
				vertexBuffer->Allocate(4);
				
				subMesh->SetElementBuffer(ElementBuffer::Types::STATIC, ElementBuffer::IndexTypes::UNSIGNED_16BIT, ElementBuffer::ElementTypes::TRIANGLE, 1);
				shared_ptr<ElementBuffer> elementBuffer=subMesh->GetElementBuffer();
				auto triangles = elementBuffer->GetAccessor< ElementBuffer::Triangle<u16> >();

				VertexBuffer::Accessor<Vector3> vertices = vertexBuffer->GetAccessor<Vector3>("Position");
				VertexBuffer::Accessor<TextureUV> uvs = vertexBuffer->GetAccessor<TextureUV>("UV0");

				//Right
				vertices[0] = Vector3(mDistance,-mDistance,-mDistance);
				vertices[1] = Vector3(mDistance,mDistance,-mDistance);
				vertices[2] = Vector3(mDistance,-mDistance,mDistance);
				vertices[3] = Vector3(mDistance,mDistance,mDistance);
				uvs[0] = TextureUV(0,1);
				uvs[1] = TextureUV(0,0);
				uvs[2] = TextureUV(1,1);
				uvs[3] = TextureUV(1,0);
				
				ElementBuffer::Triangle<u16>& triangle1 = triangles[0];
				triangle1.mA=0;
				triangle1.mB=1;
				triangle1.mC=2;

				ElementBuffer::Triangle<u16>& triangle2 = triangles[1];
				triangle2.mA=2;
				triangle2.mB=1;
				triangle2.mC=3;
				
				subMesh->Finalise();
				subMesh->SetMaterial(mMaterialRight);
			}

			if(mMaterialTop)
			{
				shared_ptr<SubMesh> subMesh = GetMesh()->CreateSubMesh();
				shared_ptr<VertexBuffer> vertexBuffer = subMesh->GetVertexBuffer();
				vertexBuffer->AddVertexAttribute("Position", VertexAttribute(VertexAttribute::ComponentTypes::VECTOR3,1));
				vertexBuffer->AddVertexAttribute("UV0", VertexAttribute(VertexAttribute::ComponentTypes::TEXTUREUV,1));
				vertexBuffer->Allocate(4);

				subMesh->SetElementBuffer(ElementBuffer::Types::STATIC, ElementBuffer::IndexTypes::UNSIGNED_16BIT, ElementBuffer::ElementTypes::TRIANGLE, 1);
				shared_ptr<ElementBuffer> elementBuffer=subMesh->GetElementBuffer();
				auto triangles = elementBuffer->GetAccessor< ElementBuffer::Triangle<u16> >();

				VertexBuffer::Accessor<Vector3> vertices = vertexBuffer->GetAccessor<Vector3>("Position");
				VertexBuffer::Accessor<TextureUV> uvs = vertexBuffer->GetAccessor<TextureUV>("UV0");

				//Top
				vertices[0] = Vector3(-mDistance,mDistance,-mDistance);
				vertices[1] = Vector3(-mDistance,mDistance,mDistance);
				vertices[2] = Vector3(mDistance,mDistance,mDistance);
				vertices[3] = Vector3(mDistance,mDistance,-mDistance);
				uvs[0] = TextureUV(0,1);
				uvs[1] = TextureUV(0,0);
				uvs[2] = TextureUV(1,0);
				uvs[3] = TextureUV(1,1);
				
				ElementBuffer::Triangle<u16>& triangle1 = triangles[0];
				triangle1.mA=2;
				triangle1.mB=1;
				triangle1.mC=0;

				ElementBuffer::Triangle<u16>& triangle2 = triangles[1];
				triangle2.mA=0;
				triangle2.mB=3;
				triangle2.mC=2;

				subMesh->Finalise();
				subMesh->SetMaterial(mMaterialTop);
			}

			if(mMaterialBottom)
			{
				shared_ptr<SubMesh> subMesh = GetMesh()->CreateSubMesh();
				shared_ptr<VertexBuffer> vertexBuffer = subMesh->GetVertexBuffer();
				vertexBuffer->AddVertexAttribute("Position", VertexAttribute(VertexAttribute::ComponentTypes::VECTOR3,1));
				vertexBuffer->AddVertexAttribute("UV0", VertexAttribute(VertexAttribute::ComponentTypes::TEXTUREUV,1));
				vertexBuffer->Allocate(4);

				subMesh->SetElementBuffer(ElementBuffer::Types::STATIC, ElementBuffer::IndexTypes::UNSIGNED_16BIT, ElementBuffer::ElementTypes::TRIANGLE, 1);
				shared_ptr<ElementBuffer> elementBuffer=subMesh->GetElementBuffer();
				auto triangles = elementBuffer->GetAccessor< ElementBuffer::Triangle<u16> >();

				VertexBuffer::Accessor<Vector3> vertices = vertexBuffer->GetAccessor<Vector3>("Position");
				VertexBuffer::Accessor<TextureUV> uvs = vertexBuffer->GetAccessor<TextureUV>("UV0");

				//Bottom
				vertices[0] = Vector3(-mDistance,-mDistance,-mDistance);
				vertices[1] = Vector3(-mDistance,-mDistance,mDistance);
				vertices[2] = Vector3(mDistance,-mDistance,mDistance);
				vertices[3] = Vector3(mDistance,-mDistance,-mDistance);
				uvs[0] = TextureUV(0,0);
				uvs[1] = TextureUV(0,1);
				uvs[2] = TextureUV(1,1);
				uvs[3] = TextureUV(1,0);
				
				ElementBuffer::Triangle<u16>& triangle1 = triangles[0];
				triangle1.mA=2;
				triangle1.mB=1;
				triangle1.mC=0;

				ElementBuffer::Triangle<u16>& triangle2 = triangles[1];
				triangle2.mA=0;
				triangle2.mB=3;
				triangle2.mC=2;

				subMesh->Finalise();
				subMesh->SetMaterial(mMaterialBottom);
			}
		}
		mMeshOutOfDate = false;
	}		

	void SkyBox::Render(RenderContext& renderContext, Colour compoundDiffuse)
	{
		if(mMeshOutOfDate)
		{
			UpdateMesh();
		}
		SceneEntity::Render(renderContext, compoundDiffuse);
	}
}
