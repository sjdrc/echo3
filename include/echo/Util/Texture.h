#ifndef _ECHOTEXTUREUTILS_H_
#define _ECHOTEXTUREUTILS_H_
#include <echo/Types.h>
#include <echo/Graphics/TextureUnit.h>

namespace Echo
{
	class Texture;
	
	namespace Utils
	{
		/**
		 * Create a new Texture with the specified size from an existing Texture.
		 * @param texture The original texture to copy from
		 * @param newWidth The width of the new texture
		 * @param newHeight The height of the new texture
		 * @param filter The filter method to use when sampling the original image.
		 */
		shared_ptr<Texture> CreateResizedTexture(const Texture& texture, Size newWidth, Size newHeight, TextureUnit::TextureFilter filter = TextureUnit::TextureFilters::LINEAR);

		/**
		 * Given a texture create a copy of it in float format.
		 * @param texture The texture to copy.
		 * @return new texture, or a null pointer if there was a problem.
		 */
		shared_ptr<Texture> CreateFloatTexture(const Texture& texture);

		/**
		 * Create a texture.
		 * @param texture The texture to build a texture from.
		 * @param newWidth New texture width.
		 * @param newHeight New texture height.
		 * @param centre Whether or not to centre the texture
		 * @return The new texture or a null pointer if the texture is larger than or equal to the specified size.
		 */
		shared_ptr<Texture> CreatePaddedTexture(const Texture& texture, Size newWidth, Size newHeight, bool centre);
		
		/**
		 * Fill a texture with zeros data;
		 * @return true on success, false indicates a null buffer.
		 */
		bool ZeroTexture(Texture& texture);
	}
}
#endif
