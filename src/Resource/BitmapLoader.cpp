#include <string>
#include <iostream>
#include <bitset>

#include <echo/Util/Utils.h>
#include <echo/Resource/BitmapLoader.h>
#include <echo/FileSystem/File.h>
#include <echo/Graphics/Colour.h>
#include <echo/Graphics/Texture.h>

//#define DEBUG_LOADBMP

namespace Echo
{
#define BYTECAST(x)  ((u8)((x) & 0xFF))  // truncate int to byte without warnings

	static int shiftsigned(int v, int shift, int bits)
	{
	   int result;
	   int z=0;

	   if (shift < 0) v <<= -shift;
	   else v >>= shift;
	   result = v;

	   z = bits;
	   while (z < 8) {
		  result += v >> z;
		  z += bits;
	   }
	   return result;
	}
	
	BitmapLoader::BitmapLoader() : mWidth(0), mHeight(0), mFormat(Texture::Formats::UNKNOWN), mFile(0), mFileLineBuffer(0)
	{}
	
	BitmapLoader::~BitmapLoader()
	{
		CleanUp();
	}
		
	bool BitmapLoader::ProcessFile(File& textureFile)
	{
		mFile = 0;
		
		if(!textureFile.IsOpen())
		{
			ECHO_LOG_ERROR("BitmapLoader: Error input file is not open: " << textureFile.GetActualFileName());
			return false;
		}

	#ifdef ECHO_BIG_ENDIAN
		mAlphaMask = 0x000000FF;
		mRedMask =	0x0000FF00;
		mGreenMask = 0x00FF0000;
		mBlueMask =	0xFF000000;
	#else
		mAlphaMask = 0xFF000000;
		mRedMask =	0x00FF0000;
		mGreenMask = 0x0000FF00;
		mBlueMask =	0x000000FF;
	#endif
		u32 startOffSet;
		u16 bitsPerPixel = 0;
		u32 dataV = 0;

		textureFile.SetEndianMode(File::EndianModes::LITTLE);
		textureFile.SetEndianConversionEnabled(true);

		mFile = &textureFile;

		textureFile.Seek(0x0a);
		textureFile.Read((void*) (&startOffSet), 4, 1);

		textureFile.Seek(0x1c);
		textureFile.Read((void*) (&bitsPerPixel), 2, 1);

		textureFile.Seek(0x12);
		textureFile.Read((void*) (&mWidth), 4, 1);

		textureFile.Seek(0x16);
		textureFile.Read((void*) (&mHeight), 4, 1);

		textureFile.Seek(0x1e);
		textureFile.Read((void*) (&dataV), 4, 1);
		if(dataV == 3)
		{
			//The bitmasks are not Default
			textureFile.Seek(0x36);
			textureFile.Read((char*) (&dataV), 4, 1);
			mRedMask = dataV;
			textureFile.Read((char*) (&dataV), 4, 1);
			mGreenMask = dataV;
			textureFile.Read((char*) (&dataV), 4, 1);
			mBlueMask = dataV;
			textureFile.Read((char*) (&dataV), 4, 1);
			mAlphaMask = dataV;
		}
		
		mRCount = BitCount(mRedMask);
		mGCount = BitCount(mGreenMask);
		mBCount = BitCount(mBlueMask);
		mACount = BitCount(mAlphaMask);
		
        mRShift = HighestBit(mRedMask)-(mRCount-1);
        mGShift = HighestBit(mGreenMask)-(mGCount-1);
        mBShift = HighestBit(mBlueMask)-(mBCount-1);

		if(mACount)
        {
			mAShift = HighestBit(mAlphaMask)-(mACount-1);
		}else
		{
			mAShift = 0;
		}
		
		mBytesPerPixel = bitsPerPixel/8;
		if(mWidth!=0)
		{
			delete [] mFileLineBuffer;
			mFileLineBuffer = new u8[mBytesPerPixel*mWidth];
		}

		if(mRCount==8 && mGCount==8 && mBCount==8)
		{
			if(bitsPerPixel==24)
			{
				mFormat = Texture::Formats::R8G8B8;
			}else
			if(mACount==0)
			{
				mFormat = Texture::Formats::R8G8B8X8;
			}else
			{	
				mFormat = Texture::Formats::R8G8B8A8;
			}
		}else
		if(mRCount==5 && mGCount==6 && mBCount==5 && mACount==0)
		{
			mFormat = Texture::Formats::R5G6B5;
		}else
		if(mRCount==4 && mGCount==4 && mBCount==4 && mACount==4)
		{
			mFormat = Texture::Formats::R4G4B4A4;
		}else
		if(mRCount==5 && mGCount==5 && mBCount==5)	//It doesn't matter how the alpha was counted.
		{
			mACount = 1;
			mFormat = Texture::Formats::R5G5B5A1;
		}else
		{
			ECHO_LOG_ERROR("BitmapLoader: Error Unknown Texture Format R:" << mRCount << " G:" << mGCount << " B:" << mBCount << " A:" << mACount);
			return false;
		}

		#ifdef DEBUG_LOADBMP
		ECHO_LOG_DEBUG("LoadBMP Debug information:");
		ECHO_LOG_DEBUG("\tSize: " << mWidth << "x" << mHeight);
		ECHO_LOG_DEBUG("\tBPP: " << bitsPerPixel);
		ECHO_LOG_DEBUG("\tFormat:" << mFormat);
		ECHO_LOG_DEBUG("\tRed: Shift: " << mRShift << " Bits:" << mRCount << " Mask(" << std::bitset<32>(mRedMask).to_string() << ")");
		ECHO_LOG_DEBUG("\tGreen: Shift: " << mGShift << " Bits:" << mGCount << " Mask(" << std::bitset<32>(mGreenMask).to_string() << ")");
		ECHO_LOG_DEBUG("\tBlue Shift: " << mBShift << " Bits:" << mBCount << " Mask(" << std::bitset<32>(mBlueMask).to_string() << ")");
		ECHO_LOG_DEBUG("\tAlpha Shift: " << mAShift << " Bits:" << mACount << " Mask(" << std::bitset<32>(mAlphaMask).to_string() << ")");
		#endif		
		textureFile.Seek(startOffSet);
		return true;
	}
	
	std::vector<std::string> BitmapLoader::GetFileExtensions() const
	{
		return {"bmp"};
	}
	
	u32 BitmapLoader::GetWidth() const
	{
		return mWidth;
	}
	
	u32 BitmapLoader::GetHeight() const
	{
		return mHeight;
	}
	
	Texture::Format BitmapLoader::GetFormat() const
	{
		return mFormat;
	}
	
	bool BitmapLoader::GetLoadInverted() const
	{
		return true;
	}
	
	void BitmapLoader::ReadLine(u8* lineStart, u32 maxLength)
	{
		if(!mFile)
		{
			return;
		}

		//Get each line
		u32 extraToGet = (mBytesPerPixel * mWidth) % 4;
		mFile->Read((void*)mFileLineBuffer, mBytesPerPixel, mWidth);
		if(extraToGet) //It likes to be on a four byte boundary
		{
			u32 fakeBuff;
			mFile->Read(&fakeBuff, 1, extraToGet);
		}

		//Convert the Data to the right format
		for(u32 i = 0; i < mWidth; ++i)
		{
			if(mBytesPerPixel==2)
			{
				u16 pixel = *reinterpret_cast<u16*>(&(mFileLineBuffer[i * mBytesPerPixel]));	//Read 16bits.
				u32 r = BYTECAST(shiftsigned(pixel & mRedMask, mRShift, mRCount));
				u32 g = BYTECAST(shiftsigned(pixel & mGreenMask, mGShift, mGCount));
				u32 b = BYTECAST(shiftsigned(pixel & mBlueMask, mBShift, mBCount));
				switch(mFormat)
				{
					case Texture::Formats::R5G6B5:
						*((u16*)(&(lineStart[i * mBytesPerPixel]))) = (r << 11) | (g << 5) | (b);
					break;
					case Texture::Formats::R5G5B5A1:
					{
						u32 a = (mAlphaMask ? shiftsigned(pixel & mAlphaMask, mAShift, mACount) : 0x1);
						*((u16*)(&(lineStart[i * mBytesPerPixel]))) = (r << 11) | (g << 6) | (b<<1) | a;
					}break;
					case Texture::Formats::R4G4B4A4:
					{
						u32 a = (mAlphaMask ? shiftsigned(pixel & mAlphaMask, mAShift, mACount) : 0xF);
						*((u16*)(&(lineStart[i * mBytesPerPixel]))) = (r << 12) | (g << 8) | (b << 4) | (a);
					}break;
					case Texture::Formats::RGB_F32:
					case Texture::Formats::RGBA_F32:
					case Texture::Formats::LUMINANCE32:
					case Texture::Formats::LUMINANCE32_ALPHA32:
					case Texture::Formats::B8G8R8:
					case Texture::Formats::B8G8R8A8:
					case Texture::Formats::GREYSCALE8:
					case Texture::Formats::GREYSCALE16:
					case Texture::Formats::UNKNOWN:
					case Texture::Formats::R8G8B8X8:
					case Texture::Formats::R8G8B8A8:
					case Texture::Formats::R8G8B8:
					case Texture::Formats::LUMINANCE8:
					case Texture::Formats::LUMINANCE8_ALPHA8:
						ECHO_LOG_ERROR("BitmapLoader::ReadLine() Unsupported texture format");
						break;
				}
			}else
			{
				u32 pixel = *reinterpret_cast<u32*>(&(mFileLineBuffer[i * mBytesPerPixel]));	//Read 32bits.
				u32 r = BYTECAST(shiftsigned(pixel & mRedMask, mRShift, mRCount));
				u32 g = BYTECAST(shiftsigned(pixel & mGreenMask, mGShift, mGCount));
				u32 b = BYTECAST(shiftsigned(pixel & mBlueMask, mBShift, mBCount));

				if(mFormat==Texture::Formats::R8G8B8)
				{
					lineStart[(i * mBytesPerPixel)] = r;
					lineStart[(i * mBytesPerPixel)+1] = g;
					lineStart[(i * mBytesPerPixel)+2] = b;
				}else
				if(mFormat==Texture::Formats::R8G8B8X8)
				{
					u32 a = 0xFF;
					lineStart[(i * mBytesPerPixel)] = r;
					lineStart[(i * mBytesPerPixel)+1] = g;
					lineStart[(i * mBytesPerPixel)+2] = b;
					lineStart[(i * mBytesPerPixel)+3] = a;
				}else
				if(mFormat==Texture::Formats::R8G8B8A8)
				{
					u32 a = (mAlphaMask ? shiftsigned(pixel & mAlphaMask, mAShift, mACount) : 0xFF);
					lineStart[(i * mBytesPerPixel)] = r;
					lineStart[(i * mBytesPerPixel)+1] = g;
					lineStart[(i * mBytesPerPixel)+2] = b;
					lineStart[(i * mBytesPerPixel)+3] = a;
				}
			}
		}
	}

	void BitmapLoader::CleanUp()
	{
		delete [] mFileLineBuffer;
		mFileLineBuffer = 0;
	}
	
}
