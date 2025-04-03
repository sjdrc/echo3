#ifndef _ECHOIMAGEELEMENT_H_
#define _ECHOIMAGEELEMENT_H_

#include <echo/GUI/Element.h>
#include <echo/Graphics/PrimitiveTypes.h>

namespace Echo
{
	namespace GUI
	{
		/**
		 * A GUI Image applies a material to a Quad.
		 */
		class Image : public Element
		{
		public:
			Image(shared_ptr<Material> material);
			virtual ~Image();
			
			/**
			 * Since an Image is primarily a mesh but not intended to be a shared resource,
			 * copying an Image will copy the image settings but the copy will have its own
			 * Mesh.
			 * @see Element::operator=() for documentation on Element copying.
			 * @see Node::operator=() for documentation on copying.
			 */
			Image(const Image& other);
			Image& operator=(const Image& rhs);
			
			void SetMaterial(shared_ptr<Material> material);
			void SetFlipHorizontally(bool flipHorizontally);
			void SetFlipVertically(bool flipVertically);

			void SetMinUV(TextureUV minUV);
			void SetMaxUV(TextureUV maxUV);
		private:
			virtual shared_ptr<Element> _Clone() const override;
			virtual Scalar GetNativeAspectRatio() override;
			virtual void UpdateSize(Scalar newWidth, Scalar newHeight) override;
			bool mFlipHorizontally;
			bool mFlipVertically;
			TextureUV mMinUV;
			TextureUV mMaxUV;
		};
	}
}
#endif 
