#include <echo/Resource/TextureLoader.h>
#include <echo/FileSystem/File.h>
#include <echo/Util/Utils.h>

namespace Echo
{
	TextureLoader::TextureLoader(){}
	TextureLoader::~TextureLoader()
	{
	}

	Texture* TextureLoader::LoadTexture(File& file, bool forcePowerOfTwoTextures, Texture* textureToLoadInto)
	{
		if(!file.IsOpen())
		{
			CleanUp();
			return 0;
		}
		if(!ProcessFile(file))
		{
			CleanUp();
			return 0;
		}
		if(GetFormat()==Texture::Formats::UNKNOWN)
		{
			CleanUp();
			return 0;
		}
		u32 width = GetWidth();
		u32 imageHeight = GetHeight();
		u32 height = imageHeight;
		
		if(forcePowerOfTwoTextures)
		{
			width = NextPow2(width);
			height = NextPow2(height);
		}
		Texture* resource = nullptr;
		if (textureToLoadInto)
		{
			textureToLoadInto->mFormat = GetFormat();
			textureToLoadInto->mWidth = width;
			textureToLoadInto->mHeight = height;
			textureToLoadInto->SetupBuffer();
			resource = textureToLoadInto;
		}
		else
		{
			resource = new Texture(width, height, GetFormat());
		}
		u32 bytesPerPixel = Texture::Formats::GetBytesPerPixel(GetFormat());
		shared_ptr<u8> buffer = resource->GetBuffer();
		if(GetLoadInverted())
		{
			for(u32 y = imageHeight; y>0; --y)
			{
				u8* currentLine = &(buffer.get()[bytesPerPixel*width*(y-1)]);
				ReadLine(currentLine,bytesPerPixel*width);
			}
		}else
		{
			for(u32 y = 0; y < imageHeight; ++y)
			{
				u8* currentLine = &(buffer.get()[bytesPerPixel*width*y]);
				ReadLine(currentLine,bytesPerPixel*width);
			}
		}
		CleanUp();
		return resource;
	}
}
