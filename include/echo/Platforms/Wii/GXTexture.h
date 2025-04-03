#ifndef _ECHOGXTEXTURE_H_
#define _ECHOGXTEXTURE_H_

#include <echo/Graphics/Texture.h>
#include <echo/Graphics/TextureUnit.h>

#include <ogc/gx.h>

namespace Echo
{
	class Texture;

	class GXTexture
	{
	public:
		//!\brief Constructor.
		GXTexture(Texture& texture);
		~GXTexture();

		GXTexObj* GetTextureObject() const {return mTextureObject;}
		
		bool GetIsIndexed() const {return mTextureLookup!=0;}
		GXTlutObj* GetTextureLookupObject() const {return mTextureLookup;}
		
		static u8 GetGXTextureFormat(const Texture::Format& format);
		static u8 GetGXFilterMode(const TextureUnit::TextureFilter& format);
		static u8 GetGXWrapMode(const TextureUnit::TextureWrapMode& format);
		void SetMinFilter(const TextureUnit::TextureFilter& filter);
		void SetMagFilter(const TextureUnit::TextureFilter& filter);
		void SetWrapModeU(const TextureUnit::TextureWrapMode& mode);
		void SetWrapModeV(const TextureUnit::TextureWrapMode& mode);

	private:
		void* CreateAndConvertData(Texture& texture);
		void* CreateRGBA8(Texture& texture);
		static const u8 UNSUPPORTED_TEXTURE_FORMAT=0xFF;
		static const size_t MAX_GX_TEXTURE_WIDTH=1024;
		static const size_t MAX_GX_TEXTURE_HEIGHT=1024;
		GXTexObj* mTextureObject;
		GXTlutObj* mTextureLookup;
		void* mBuffer;
		shared_ptr<Texture> mResizedTexture;
		u8 mMinFilter;
		u8 mMagFilter;
		u8 mUWrapMode;
		u8 mVWrapMode;
		u32 mWidth;
		u32 mHeight;
	};
}
#endif
