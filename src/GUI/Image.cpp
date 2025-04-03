#include <echo/GUI/Image.h>
#include <echo/Graphics/Mesh.h>
#include <echo/Graphics/SubMesh.h>
#include <echo/Graphics/Material.h>
#include <echo/Graphics/RenderPass.h>
#include <echo/Graphics/TextureUnit.h>

namespace Echo
{
	namespace GUI
	{
		Image::Image(shared_ptr<Material> material) :
			mFlipHorizontally(false),
			mFlipVertically(false)
		{
			SetMesh(make_shared<Mesh>());
			GetMesh()->CreateQuadSubMesh();
			GetMesh()->SetMaterial(material);
			SetSize(1.f, 1.f);
			VertexBuffer::Accessor<TextureUV> uvs = GetMesh()->GetSubMeshComponents<TextureUV>(0,"UV0");
			mMinUV = uvs[0];
			mMaxUV = uvs[3];
		}
		
		Image::~Image()
		{
		}
		
		Image::Image(const Image& other)
		{
			SetMesh(make_shared<Mesh>());
			GetMesh()->CreateQuadSubMesh();
			//Images should always have meshes.
			assert(other.GetMesh() && "Image does not have a mesh.");
			assert(other.GetMesh()->GetSubMesh(0) && "Image does not have a mesh.");
			GetMesh()->SetMaterial(other.GetMesh()->GetSubMesh(0)->GetMaterial());
			*this = other;
		}
		
		Image& Image::operator=(const Image& rhs)
		{
			if(this==&rhs)
			{
				return *this;
			}
			Element::operator=(rhs);		//This will preserve the mesh for us.
			mFlipHorizontally = rhs.mFlipHorizontally;
			mFlipVertically = rhs.mFlipVertically;
			mMinUV = rhs.mMinUV;
			mMaxUV = rhs.mMaxUV;
			Image::UpdateSize(GetAbsoluteWidth(), GetAbsoluteHeight());
			return *this;
		}

		shared_ptr<Element> Image::_Clone() const
		{
			return shared_ptr<Element>(new Image(*this));
		}
			
		void Image::SetMaterial(shared_ptr<Material> material)
		{
			assert(GetMesh());
			GetMesh()->SetMaterial(material);
		}
		
		void Image::SetFlipHorizontally(bool flipHorizontally)
		{
			if(mFlipHorizontally!=flipHorizontally)
			{
				mFlipHorizontally = flipHorizontally;
				Image::UpdateSize(GetAbsoluteWidth(), GetAbsoluteHeight());
			}
		}
		
		void Image::SetFlipVertically(bool flipVertically)
		{
			if(mFlipVertically!=flipVertically)
			{
				mFlipVertically = flipVertically;
				Image::UpdateSize(GetAbsoluteWidth(), GetAbsoluteHeight());
			}
		}

		void Image::SetMinUV(TextureUV minUV)
		{
			if(mMinUV!=minUV)
			{
				mMinUV=minUV;
				Image::UpdateSize(GetAbsoluteWidth(), GetAbsoluteHeight());
			}
		}
		
		void Image::SetMaxUV(TextureUV maxUV)
		{
			if(mMaxUV!=maxUV)
			{
				mMaxUV=maxUV;
				Image::UpdateSize(GetAbsoluteWidth(), GetAbsoluteHeight());
			}
		}
		
		Scalar Image::GetNativeAspectRatio()
		{
			// The Material isn't always going to be what we set it in the constructor
			// almost always the case when this class is used as intended.

			//Get the aspect ratio.
			shared_ptr<Material> material = GetMesh()->GetSubMesh(0)->GetMaterial();
			if(material)
			{
				// We need to make a decision about which pass to use. The first seems
				// the most appropriate because in a GUI sense the first is likely to contain
				// a texture for the base size. Any additional passes are likely to be for
				// aesthetic purposes.
				RenderPass* pass = material->GetPass(0);
				if(pass)
				{
					// We need to make a decision about which texture unit to use. The first seems
					// the most appropriate because in a GUI sense the first is likely to be the
					// base size. Any additional units are likely to be for aesthetic purposes.
					TextureUnit* tu = pass->GetTextureUnit(0);
					if(tu)
					{
						//Getting close
						shared_ptr<Texture> texture = tu->GetTexture();
						if(texture)
						{
							Scalar width = static_cast<Scalar>(texture->GetWidth());
							Scalar height = static_cast<Scalar>(texture->GetHeight());
							return (width/height);
						}
					}
				}
			}
			return 1.f;
		}		

		void Image::UpdateSize(Scalar newWidth, Scalar newHeight)
		{
			assert(GetMesh());
			assert(GetMesh()->GetSubMesh(0));
			
			shared_ptr<SubMesh> subMesh = GetMesh()->GetSubMesh(0);
			VertexBuffer::Accessor<Vector3> verts = subMesh->GetComponents<Vector3>("Position");
			assert(verts.GetCapacity()>=4);
			f32 halfWidth = mFlipHorizontally ? -(newWidth / 2) : (newWidth / 2);
			f32 halfHeight = mFlipVertically ? -(newHeight / 2) : (newHeight / 2);
			verts[0].x = -halfWidth;
			verts[0].y = halfHeight;

			verts[1].x = halfWidth;
			verts[1].y = halfHeight;

			verts[2].x = -halfWidth;
			verts[2].y = -halfHeight;

			verts[3].x = halfWidth;
			verts[3].y = -halfHeight;
			
			VertexBuffer::Accessor<TextureUV> uvs = subMesh->GetComponents<TextureUV>("UV0");
			uvs[0] = mMinUV;
			uvs[1] = TextureUV(mMaxUV.u,mMinUV.v);
			uvs[2] = TextureUV(mMinUV.u,mMaxUV.v);
			uvs[3] = mMaxUV;

			subMesh->Finalise();
		}
	}
}
