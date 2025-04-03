#include <echo/FileSystem/FileReferenceFile.h>
#ifndef ECHO_PLATFORM_ANDROID
#include <boost/filesystem.hpp>
#endif
#include <iostream>

namespace Echo
{
	size_t FileReferenceFile::Read(void* buffer, size_t typeSize, size_t numToRead)
	{
		size_t dataSize=typeSize*numToRead;
		mFile.read(reinterpret_cast<char*>(buffer),dataSize);
		if(mFile.fail())
		{
			mPosition += (size_t)(mFile.gcount());
			mFile.clear();
			return (size_t)(mFile.gcount());
		}else
		{		
			mPosition+=dataSize;
		}
		return dataSize;
	}

	size_t FileReferenceFile::Write(const void* buffer, size_t typeSize, size_t numberToWrite)
	{
		size_t dataSize=typeSize*numberToWrite;
		mFile.write(reinterpret_cast<const char*>(buffer),dataSize);
		if(mFile.fail())
		{
			mFile.clear();
			return 0;
		}
		mFileSize+=dataSize;
		return dataSize;
	}

	size_t FileReferenceFile::Seek(size_t position)
	{
		mFile.seekp(position);
		mFile.seekg(position);
		if(mFile.fail())
		{
			mFile.clear();
		}else
		{
			mPosition=position;
		}
		return mPosition;
	}


	bool FileReferenceFile::EndOfFile()
	{
		return (mFileSize==mPosition);
	}
}
