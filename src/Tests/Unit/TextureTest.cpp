#include <echo/Graphics/Texture.h>
#include <doctest/doctest.h>

using namespace Echo;

namespace TextureTest
{
	TEST_CASE("TextureCustomFormat")
	{
		// Custom buffer representing some kind of compressed data format.
		const Size customSize = 500;
		const u32 width = 300;
		const u32 height = 300;
		u8* customBuffer = new u8[customSize];
		Texture textureCopy(customSize,customBuffer,width,height);
		
		REQUIRE_MESSAGE(textureCopy.GetBuffer().get()!=customBuffer,"Buffer was meant to copy, not wrap");
		CHECK(textureCopy.GetBytesPerPixel()==0);
		CHECK(textureCopy.GetDataSize()==customSize);
		CHECK(textureCopy.GetWidth()==width);
		CHECK(textureCopy.GetHeight()==height);
		CHECK(textureCopy.GetFormat()==Texture::Formats::UNKNOWN);
		
		Texture textureOwned(customSize,customBuffer,width,height,Texture::BufferOptions::TAKE_OWNERSHIP);

		REQUIRE_MESSAGE(textureOwned.GetBuffer().get()==customBuffer,"Buffer was meant to take ownership, not copy");
		CHECK(textureOwned.GetBytesPerPixel()==0);
		CHECK(textureOwned.GetDataSize()==customSize);
		CHECK(textureOwned.GetWidth()==width);
		CHECK(textureOwned.GetHeight()==height);
		CHECK(textureOwned.GetFormat()==Texture::Formats::UNKNOWN);
	}

	TEST_CASE("TextureClone")
	{
		// Custom buffer representing some kind of compressed data format.
		const u32 width = 300;
		const u32 height = 300;
		const Texture::Format format = Texture::Formats::R8G8B8A8;
		Texture texture(width,height,format);

		shared_ptr<Texture> clonedTexture = texture.Clone();

		// We're not going to care about the actual contents of the buffer, we'll just check the pointers are different.
		REQUIRE(clonedTexture);
		
		REQUIRE_MESSAGE(texture.GetBuffer().get()!=clonedTexture->GetBuffer().get(),"Buffer was meant to copy");
		CHECK(clonedTexture->GetBuffer().get()!=nullptr);
		CHECK(clonedTexture->GetWidth()==width);
		CHECK(clonedTexture->GetHeight()==height);
		CHECK(clonedTexture->GetFormat()==Texture::Formats::R8G8B8A8);
		
		const Size customSize = 500;
		u8* customBuffer = new u8[customSize];
		Texture textureUnknown(customSize,customBuffer,width,height,Texture::BufferOptions::TAKE_OWNERSHIP);
		clonedTexture = textureUnknown.Clone();
		REQUIRE_MESSAGE(textureUnknown.GetBuffer().get()==customBuffer,"Buffer was meant to take ownership, not copy");
		CHECK(textureUnknown.GetBuffer().get()!=clonedTexture->GetBuffer().get());
		CHECK(clonedTexture->GetBytesPerPixel()==0);
		CHECK(clonedTexture->GetDataSize()==customSize);
		CHECK(clonedTexture->GetWidth()==width);
		CHECK(clonedTexture->GetHeight()==height);
		CHECK(clonedTexture->GetFormat()==Texture::Formats::UNKNOWN);
	}
}