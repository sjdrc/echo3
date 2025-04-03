#ifndef _ECHOBITMAPLOADER_H_
#define _ECHOBITMAPLOADER_H_

#include <string>
#include <echo/Resource/TextureLoader.h>

namespace Echo
{
	class BitmapLoader : public TextureLoader
	{
	public:
		BitmapLoader();
		~BitmapLoader();
		
		std::vector<std::string> GetFileExtensions() const;
		bool ProcessFile(File& textureFile);
		u32 GetWidth() const;
		u32 GetHeight() const;
		Texture::Format GetFormat() const;
		bool GetLoadInverted() const;
		void ReadLine(u8* lineStart, u32 maxLength);
		void CleanUp();
	private:
		u32 mWidth;
		u32 mHeight;
		u32 mBytesPerPixel;
		u32 mAlphaMask;
		u32 mRedMask;
		u32 mGreenMask;
		u32 mBlueMask;
		
		u32 mRShift;
		u32 mGShift;
		u32 mBShift;
		u32 mAShift;
		
		u32 mRCount;
		u32 mGCount;
		u32 mBCount;
		u32 mACount;
		
		Texture::Format mFormat;
		File* mFile;
		u8* mFileLineBuffer;
	};
}
#endif
