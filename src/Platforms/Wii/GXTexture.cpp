#include <echo/Platforms/Wii/GXTexture.h>
#include <echo/Graphics/Texture.h>
#include <echo/Graphics/TextureUnit.h>
#include <echo/Util/Utils.h>
#include <echo/Util/Texture.h>

#include <ogc/cache.h>
#include <malloc.h>

namespace Echo
{
	GXTexture::GXTexture(Texture& texture) : mTextureObject(nullptr), mTextureLookup(nullptr), mBuffer(nullptr)
	{
		mMinFilter=GX_LINEAR;
		mMagFilter=GX_LINEAR;
		mUWrapMode=GX_REPEAT;
		mVWrapMode=GX_REPEAT;
		
		//These will be set in CreateAndConvertData() if the texture can be created.
		mWidth = 0;
		mHeight = 0;
		
		// Create a target GX texture object, we'll create a texture regardless of whether the
		// input texture can be converted.
		mTextureObject = new GXTexObj();
		DCStoreRange(mTextureObject,sizeof(GXTexObj));

		u8 gxTextureFormat=GetGXTextureFormat(texture.GetFormat());
		mBuffer=CreateAndConvertData(texture);
		if(mBuffer && mWidth!=0 && mHeight!=0)
		{
			//Initialise the texture object.
			GX_InitTexObj(mTextureObject, mBuffer, mWidth, mHeight, gxTextureFormat, mUWrapMode, mVWrapMode, GX_FALSE);
		}else
		{
			// Create an texture to indicate an error. This is different to the TextureManager error
			// texture so there is a visual difference between a missing resource (TextureManager error
			// texture) and a texture conversion error.
			u32* buffer = (u32*)memalign(32,sizeof(u32)*16*16);
			for(u32 i=0;i<16*16;++i)
			{
				buffer[i]=0xFF00FFFF;
			}
			DCStoreRange(buffer,sizeof(u32)*16*16);
			GX_InitTexObj(mTextureObject, buffer, 16, 16, GX_TF_RGBA8, mUWrapMode, mVWrapMode, GX_FALSE);
		}
		//Is the texture index coloured? Then initialise a TLUT (Texture Look Up Table) object.
		// GX_InitTlutObj();
	}
	
	GXTexture::~GXTexture()
	{
		delete mTextureObject;
		delete mTextureLookup;
		free(mBuffer);
	}
	
	void GXTexture::SetMinFilter(const TextureUnit::TextureFilter& filter)
	{
		mMinFilter=GetGXFilterMode(filter);
		GX_InitTexObjFilterMode(mTextureObject,mMinFilter,mMagFilter);
	}
	
	void GXTexture::SetMagFilter(const TextureUnit::TextureFilter& filter)
	{
		mMagFilter=GetGXFilterMode(filter);
		GX_InitTexObjFilterMode(mTextureObject,mMinFilter,mMagFilter);
	}
	
	void GXTexture::SetWrapModeU(const TextureUnit::TextureWrapMode& mode)
	{
		mUWrapMode=GetGXWrapMode(mode);
		GX_InitTexObjWrapMode(mTextureObject,mUWrapMode,mVWrapMode);
	}
	
	void GXTexture::SetWrapModeV(const TextureUnit::TextureWrapMode& mode)
	{
		mVWrapMode=GetGXWrapMode(mode);
		GX_InitTexObjWrapMode(mTextureObject,mUWrapMode,mVWrapMode);
	}
	
	u8 GXTexture::GetGXTextureFormat(const Texture::Format& format)
	{
		switch(format)
		{
			case Texture::Formats::R5G6B5:
				return GX_TF_RGB565;
			break;
			case Texture::Formats::R4G4B4A4:
			case Texture::Formats::R5G5B5A1:
				return GX_TF_RGB5A3;
			case Texture::Formats::LUMINANCE8_ALPHA8:
				return GX_TF_IA8;
			case Texture::Formats::R8G8B8A8:
			case Texture::Formats::R8G8B8:
			case Texture::Formats::R8G8B8X8:
				return GX_TF_RGBA8;
			case Texture::Formats::LUMINANCE8:
				return GX_TF_I8;
			case Texture::Formats::RGB_F32:
			break;
			case Texture::Formats::RGBA_F32:
			break;
			case Texture::Formats::LUMINANCE32:
			break;
			case Texture::Formats::LUMINANCE32_ALPHA32:
			break;
			case Texture::Formats::B8G8R8:
			break;
			case Texture::Formats::B8G8R8A8:
			break;
			case Texture::Formats::GREYSCALE8:
			break;
			case Texture::Formats::GREYSCALE16:
			break;
			case Texture::Formats::UNKNOWN:
			break;
		};
		return UNSUPPORTED_TEXTURE_FORMAT;
	}
	
	u8 GXTexture::GetGXFilterMode(const TextureUnit::TextureFilter& filter)
	{
		switch(filter)
		{
			case TextureUnit::TextureFilters::NEAREST:
				return GX_NEAR;
			case TextureUnit::TextureFilters::LINEAR:
				return GX_LINEAR;
			//Not implemented until mipmapping is supported
			//GX_NEAR_MIP_NEAR	/*!< Point sampling, discrete mipmap */
			//GX_LIN_MIP_NEAR	/*!< Bilinear filtering, discrete mipmap */
			//GX_NEAR_MIP_LIN	/*!< Point sampling, linear mipmap */
			//GX_LIN_MIP_LIN	/*!< Trilinear filtering */
		}
		return GX_NEAR;
	}
	
	u8 GXTexture::GetGXWrapMode(const TextureUnit::TextureWrapMode& mode)
	{
		switch(mode)
		{
			case TextureUnit::TextureWrapModes::CLAMP_TO_EDGE:
			case TextureUnit::TextureWrapModes::CLAMP:
				return GX_CLAMP;
			case TextureUnit::TextureWrapModes::REPEAT:
				return GX_REPEAT;
			//Not implemented GX_MIRROR
		}
		return GX_REPEAT;
	}
	
	void* GXTexture::CreateAndConvertData(Texture& texture)
	{
		//Depending on the texture format we'll need to read differently
		//Textures are stored in main memory as a row-column matrix of tiles with the following attributes:
		// - Each tile is a small sub-rectangle from the texture image.
		// - Each tile is a 4x4, 8x4, or 8x8 texel rectangle.
		//		4x4
		//			32 bit textures
		//				GX_TF_RGBA8
		//			16 bit textures
		//				GX_TF_IA8
		//				GX_TF_C14X2 - Colour indexed 14b index
		//				GX_TF_RGB565 - RGB565
		//				GX_TF_RGB5A3 - When MSB = 1 RGB555 opaque
		//							   When MSB = 0 RGB4443 transparent
		//		8x4		//			16 bit textures
		//				GX_TF_IA8
		//				GX_TF_C14X2 - Colour indexed 14b index
		//				GX_TF_RGB565 - RGB565
		//				GX_TF_RGB5A3 - When MSB = 1 RGB555 opaque
		//							   When MSB = 0 RGB4443 transparent

		//			8 bit textures
		//				GX_TF_I8	- Intensity 8bit
		//				GX_TF_IA4	- Intensity 4+4bit
		//				GX_TF_C8	- Colour indexed
		//		8x8
		//			4 bit textures
		//				GX_TF_I4	- Intensity 4bit
		//				GX_TF_C4	- Colour Indexed
		//				GX_TF_CMPR  - Compressed ~RGBA8A1
		// - Each tile is 32B, corresponding to the texture cache line size.
		// - Textures must be aligned to 32B in main memory and be a multiple of 32B in size.
		// - Texels are packed differently within a 32B tile depending on their type.
		u32 tileWidth=4;		//Overridden when needed.
		u32 tileHeight=4;		//Overridden when needed.
		u32 targetFormat;
		u32 dstBytesPerPixel=2;
		u8* textureBuffer = texture.GetBuffer().get();

		//Make sure the texture is a power of two.
		u32 width=NextPow2(texture.GetWidth());
		width = (width > MAX_GX_TEXTURE_WIDTH) ? MAX_GX_TEXTURE_WIDTH : width;
		u32 height=NextPow2(texture.GetHeight());
		height = (height > MAX_GX_TEXTURE_HEIGHT) ? MAX_GX_TEXTURE_HEIGHT : height;

		//If the texture isn't the right size for GX then we will have to resize it.
		if(width!=texture.GetWidth() || height!=texture.GetHeight())
		{
			mResizedTexture = Utils::CreateResizedTexture(texture,width,height);
			if(!mResizedTexture)
			{
				return 0;
			}
			//Use the resized texture data instead.
			textureBuffer = mResizedTexture->GetBuffer().get();
		}

		mWidth = width;
		mHeight = height;
		
		switch(texture.GetFormat())
		{
			case Texture::Formats::R5G6B5:		//16bit
				targetFormat=GX_TF_RGB565;		//16bit
			break;
			case Texture::Formats::R4G4B4A4:	//16bit
			case Texture::Formats::R5G5B5A1:	//16bit
				targetFormat=GX_TF_RGB5A3;
				break;
			case Texture::Formats::LUMINANCE8_ALPHA8:	//16bit
				targetFormat=GX_TF_IA8;
				break;
			case Texture::Formats::R8G8B8A8:		//32bit
			case Texture::Formats::R8G8B8:			//32bit
			case Texture::Formats::R8G8B8X8:		//32bit
				//GX_TF_RGBA8 - requires a different format in memory so pass the job to the other method.
				if(mResizedTexture)
				{
					return CreateRGBA8(*mResizedTexture);
				}
				return CreateRGBA8(texture);
			case Texture::Formats::LUMINANCE8:		//8bit
				tileHeight=8;
				dstBytesPerPixel=1;
				targetFormat=GX_TF_I8;
				break;
			default:
				return 0;
		};

		//u32 totalTileSize=tileWidth*tileHeight;
		
		u32 srcBytesPerPixel=texture.GetBytesPerPixel();
		void* out=memalign(32,width*height*dstBytesPerPixel);
		
		u32 numberOfXTiles=(width+(width%tileWidth))/tileWidth;
		u32 numberOfYTiles=(height+(height%tileHeight))/tileHeight;
		u32 dstIndex=0;
		
		for(u32 tileY=0; tileY<numberOfYTiles; ++tileY)
		{
			for(u32 tileX=0; tileX<numberOfXTiles; ++tileX)
			{
				for(u32 tilePixelY=0; tilePixelY<tileHeight; ++tilePixelY)
				{
					for(u32 tilePixelX=0; tilePixelX<tileWidth; ++tilePixelX)
					{
						u32 srcIndex = (tilePixelX+(tileX*tileWidth) + (tilePixelY+(tileY*tileHeight))*width) * srcBytesPerPixel;
						switch(texture.GetFormat())
						{
							case Texture::Formats::R5G6B5:				//-> GX_TF_RGB565:
							{
								//Copy one to one
								((u16*)out)[dstIndex] = (*reinterpret_cast<u16*>(&textureBuffer[srcIndex]));
							}break;
							case Texture::Formats::R4G4B4A4:			//-> GX_TF_RGB5A3
							{
								u16 pixel = *reinterpret_cast<u16*>(&textureBuffer[srcIndex]);
								u16 a = pixel & 0xF;
								if(a==15)	// Full opaque
								{
									//When MSB = 1 RGB555 opaque
											// 4 bits to 5 using the MSBs to fill the LSB
									u16 r = ((pixel & 0xF000) >> 1) | ((pixel & 0x8000) >> 4);
									u16 g = ((pixel & 0xF00) >> 2) | ((pixel & 0x800) >> 4);
									u16 b = ((pixel & 0xF0) >> 3) | ((pixel & 0x80) >> 4);
									((u16*)out)[dstIndex] = 0x8000 | r | g | b;
								}else
								{
									//When MSB = 0 RGB4443 transparent
									u16 r = ((pixel & 0xF000) >> 12);
									u16 g = (pixel & 0xF00) >> 8;
									u16 b = (pixel & 0xF0) >> 4;
									u16 a = (pixel & 0xF) >> 1;			// We will lose the LSB
									((u16*)out)[dstIndex] = (a<<12)|(r << 8) | (g << 4) | b;
								}
							}break;
							case Texture::Formats::R5G5B5A1:			//-> GX_TF_RGB5A3
							{
								u16 pixel = *reinterpret_cast<u16*>(&textureBuffer[srcIndex]);
								u16 a = pixel & 0x1;
								if(a)	// Full opaque
								{
									//When MSB = 1 RGB555 opaque
									u16 r=(pixel & 0xF800) >> 11;
									u16 g=(pixel & 0x7C0) >> 6;
									u16 b=(pixel & 0x3E) >> 1;
									
									((u16*)out)[dstIndex] = (1<<15) | (r << 10) | (g << 5) | b;
								}else
								{
									//When MSB = 0 RGB4443 transparent
									//Deliberately remove the LSB. We are going from 5 bits to 4.
									//In this conversion the tile is just 0, we don't actually need to do anything.
									//((u16*)out)[dstIndex] = (r << 8) | (g << 4) | b;
									((u16*)out)[dstIndex] = 0;
								}
							}break;
							case Texture::Formats::LUMINANCE8_ALPHA8:	//->GX_TF_IA8
							{
								u16 pixel = *reinterpret_cast<u16*>(&textureBuffer[srcIndex]);
								u16 i = (pixel & 0xFF00) >> 8;
								u16 a = (pixel & 0x00FF) << 8;
								((u16*)out)[dstIndex] = i | a;
							}break;
							case Texture::Formats::LUMINANCE8:			//-> GX_TF_I8
							{
								((u8*)out)[dstIndex] = textureBuffer[srcIndex];
							}break;
							default:
								ECHO_LOG_ERROR("Unsupported texture format" << texture.GetFormat() << " this should have been caught earlier.");
							break;
						}
						dstIndex++;
					}
				}
			}
		}

		DCStoreRange(out,width*height*dstBytesPerPixel);
		return out;
	}
	
	void* GXTexture::CreateRGBA8(Texture& texture)
	{
		u8* textureBuffer = texture.GetBuffer().get();
		u32 width=texture.GetWidth();
		u32 height=texture.GetHeight();
		u32 texturePixelSize=texture.GetBytesPerPixel();
		u32* out=(u32*)memalign(32,width*height*sizeof(u32));
		
		u32* tempArrangeBuff=new u32[4*width];
		u32 tempArrangeColourBuff[16];	//64 byte buffer for Colour re-arrangement
		
		for(u32 tileY=0; tileY<height/4; ++tileY)
		{
			for(u32 tileX=0; tileX<width/4; ++tileX)
			{
				for(u32 tilePixelY=0; tilePixelY<4; ++tilePixelY)
				{
					for(u32 tilePixelX=0; tilePixelX<4; ++tilePixelX)
					{
						//tileWidth=4;
						//totalTileSize=16;                    \/        \/
						u32 index = (tilePixelX+(tileX*4) + (tilePixelY+(tileY*4))*width) * texturePixelSize;
						u32 r=0;
						u32 g=0;
						u32 b=0;
						u32 a=0xFF;
						switch(texturePixelSize)
						{
							case 3:	//RGB - but with endian converted
							{	u32 pixel = *reinterpret_cast<u32*>(&textureBuffer[index]);
								g=(pixel & 0xFF0000) >> 16;
								b=(pixel & 0xFF00) >> 8;
								r=(pixel & 0xFF);
								a=0xFF;
								break;
							}
							case 4:	//RGBA
							{
								u32 pixel = *reinterpret_cast<u32*>(&textureBuffer[index]);
								r=(pixel & 0xFF000000) >> 24;
								g=(pixel & 0xFF0000) >> 16;
								b=(pixel & 0xFF00) >> 8;
								a=(pixel & 0xFF);
							}break;
						}
						//Stored as ARGB
						tempArrangeBuff[tilePixelX+(tilePixelY*4)+(tileX*16)] = (a<<24)|(r<<16)|(g<<8)|(b);
					}
				}

			}

			//Do two tile formatting here
			for(u32 tileX=0; tileX<width/4; tileX+=1)
			{
				//tilePixelX now means the current pixel in the 
				for(u32 tilePixelX=0; tilePixelX<16; ++tilePixelX)
				{
					tempArrangeColourBuff[tilePixelX]=tempArrangeBuff[tilePixelX+(tileX*16)];
				}
				for(u32 tilePixelX=0; tilePixelX<8; ++tilePixelX)
				{
					//ABCD .... WXYZ
					// [1a] = [1a]&F0 | (( [1b]&F0)>>16 );
					// [1b] = (( [1a]&F)<<16 ) | [1b]&;
					//ABWX .... CDYZ
					//So split the arrays in two and do two pixels at once

					u32 a=0,b=0,c=0;
					a = tempArrangeColourBuff[tilePixelX*2 ];
					b = tempArrangeColourBuff[tilePixelX*2 +1];
					c = (a&0xFFFF0000) | ((b&0xFFFF0000)>>16);
					tempArrangeBuff[tilePixelX + (tileX*16)]=c;
					c = ((a&0x0000FFFF)<<16) | (b&0x0000FFFF);
					tempArrangeBuff[tilePixelX + 8 + (tileX*16)]=c;
				}
			}

			//Copy the temp buffer to the old
			for(u32 i=0; i<width*4;++i)
			{
				out[(tileY*width*4)+i]=tempArrangeBuff[i];
			}
		}

		DCStoreRange(out,width*height*sizeof(u32));
		delete tempArrangeBuff;
		return out;
	}
}

