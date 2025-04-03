#include <echo/GUI/Image.h>
#include <echo/Graphics/Mesh.h>
#include <echo/GUI/Button.h>

namespace Echo
{
	namespace GUI
	{
		Button::Button(shared_ptr<Material> idleMaterial) : Image(idleMaterial), mIdleMaterial(idleMaterial)
		{
		}
		
		Button::~Button()
		{

		}
		
		Button::Button(const Button& other) : Image(other.GetIdle())
		{
			*this = other;
		}
		
		Button& Button::operator=(const Button& rhs)
		{
			if(this==&rhs)
			{
				return *this;
			}
			Image::operator=(rhs);
			mIdleMaterial = rhs.mIdleMaterial;
			mPressedMaterial = rhs.mPressedMaterial;
			mReleasedMaterial = rhs.mReleasedMaterial;
			mHoverMaterial = rhs.mHoverMaterial;
			return *this;
		}
		
		shared_ptr<Element> Button::_Clone() const
		{
			return shared_ptr<Element>(new Button(*this));
		}

		Scalar Button::GetNativeAspectRatio()
		{
			//We'll find a material to use based on the following order:
			// - idle
			// - hover
			// - pressed
			// - released
			
			shared_ptr<Material> material = mIdleMaterial;
			if(!material)
			{
				material = mHoverMaterial;
			}
			
			if(!material)
			{
				material = mPressedMaterial;
			}

			if(!material)
			{
				material = mReleasedMaterial;
			}
			
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
		
		void Button::SetIdle(shared_ptr<Material> material)
		{
			mIdleMaterial = material;
		}
		
		void Button::SetPressed(shared_ptr<Material> material)
		{
			mPressedMaterial = material;
		}
		
		void Button::SetReleased(shared_ptr<Material> material)
		{
			mReleasedMaterial = material;
		}
		
		void Button::SetHover(shared_ptr<Material> material)
		{
			mHoverMaterial = material;
		}

		void Button::OnCursorEnter(Scalar absoluteX, Scalar absoluteY)
		{
			if(mHoverMaterial)
			{
				SetMaterial(mHoverMaterial);
			}else
			{
				SetMaterial(mIdleMaterial);
			}
		}
		
		void Button::OnCursorHover(Scalar absoluteX, Scalar absoluteY)
		{
			if(mHoverMaterial)
			{
				SetMaterial(mHoverMaterial);
			}else
			{
				SetMaterial(mIdleMaterial);
			}
			Element::OnCursorHover(absoluteX, absoluteY);
		}
		
		void Button::OnCursorLeave(Scalar absoluteX, Scalar absoluteY)
		{
			SetMaterial(mIdleMaterial);
			Element::OnCursorLeave(absoluteX, absoluteY);
		}
		
		void Button::OnCursorPress(Scalar absoluteX, Scalar absoluteY)
		{
			if(mPressedMaterial)
			{
				SetMaterial(mPressedMaterial);
			}else
			{
				SetMaterial(mIdleMaterial);
			}
			Element::OnCursorPress(absoluteX, absoluteY);
		}
		
		void Button::OnCursorHold(Scalar absoluteX, Scalar absoluteY)
		{
			if(mPressedMaterial)
			{
				SetMaterial(mPressedMaterial);
			}else
			{
				SetMaterial(mIdleMaterial);
			}
			Element::OnCursorHold(absoluteX, absoluteY);
		}
		
		void Button::OnCursorRelease(Scalar absoluteX, Scalar absoluteY)
		{
			if(mReleasedMaterial)
			{
				SetMaterial(mReleasedMaterial);
			}else
			{
				SetMaterial(mIdleMaterial);
			}
			Element::OnCursorRelease(absoluteX, absoluteY);
		}
	}
}
