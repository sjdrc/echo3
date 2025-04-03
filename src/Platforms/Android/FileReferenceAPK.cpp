#include <echo/Platforms/Android/FileReferenceAPK.h>
#include <iostream>
#include <android/asset_manager.h>

namespace Echo
{
	FileReferenceAPK::FileReferenceAPK(FileSystemSource* source,AAsset* asset) :
		FileReference(source),
		mFile(asset)
	{
		//Because we only support read mode, the file size can be reliably cached.
		if(GetFileSize()==0)
		{
			mFileSize = AAsset_getLength(mFile);
		}
	}
	
	FileReferenceAPK::~FileReferenceAPK()
	{
		AAsset_close(mFile);
	}
	
	size_t FileReferenceAPK::Read(void* buffer, size_t typeSize, size_t numToGet)
	{
		size_t s=typeSize*numToGet;
		int numberRead = AAsset_read(mFile,buffer,s);
		if(numberRead==0)
		{
			//Force EOF
			mPosition = GetFileSize();
			return 0;
		}

		if(numberRead<0)
		{
			ECHO_LOG_ERROR("Failed to read "  << s << "bytes");
			return 0;
		}

		mPosition+=s;

		size_t fileSize = GetFileSize();
		if(mPosition>fileSize)
		{
			mPosition = fileSize;
		}

		return s;
	}

	size_t FileReferenceAPK::Write(const void* buffer, size_t typeSize, size_t numToWrite)
	{
		//APK files don't support writing.
		return 0;
	}

	size_t FileReferenceAPK::Seek(size_t position)
	{
		off_t o = AAsset_seek(mFile,position,SEEK_SET);
		if(o==-1)
		{
			ECHO_LOG_ERROR("Failed to seek to "  << position);
			return mPosition;
		}else
		{
			mPosition=position;
		}
		return mPosition;
	}

	bool FileReferenceAPK::EndOfFile()
	{
		return (GetPosition()==GetFileSize());
	}
}
