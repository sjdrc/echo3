#include <echo/FileSystem/FileReferenceMemory.h>
#include <iostream>

namespace Echo
{
	FileReferenceMemory::FileReferenceMemory(FileSystemSource* source, const void* dataLocation, size_t dataSize) : FileReference(source),
		mConstData(dataLocation),
		mData(nullptr)
	{
		mFileSize = dataSize;
	}

	FileReferenceMemory::FileReferenceMemory(FileSystemSource* source, void* dataLocation, size_t dataSize) : FileReference(source),
		mConstData(dataLocation),
		mData(dataLocation)
	{
		mFileSize = dataSize;
	}
	
	size_t FileReferenceMemory::Read(void* buffer, size_t typeSize, size_t numToRead) 
	{
		size_t dataSize=typeSize*numToRead;
		//Don't read past the known file size
		if((mPosition+dataSize)>mFileSize)
		{
			dataSize=mFileSize-mPosition;
		}

		const u8* byteData = &(reinterpret_cast<const u8*>(mConstData)[mPosition]);
		std::copy(byteData,byteData+dataSize,reinterpret_cast<u8*>(buffer));
		mPosition+=dataSize;
		return dataSize;
	}

	size_t FileReferenceMemory::Write(const void* buffer, size_t typeSize, size_t numberToWrite)
	{
		if(!mData)
		{
			return 0;
		}
		size_t dataSize=typeSize*numberToWrite;
		
		//Don't write past the known file size
		if((mPosition+dataSize)>mFileSize)
		{
			dataSize=mFileSize-mPosition;
		}
		
		u8* destination = &(reinterpret_cast<u8*>(mData)[mPosition]);
		const u8* source = reinterpret_cast<const u8*>(buffer);
		std::copy(source,source+dataSize,destination);
		mPosition+=dataSize;
		return dataSize;
	}

	size_t FileReferenceMemory::Seek(size_t position)
	{
		//If the file size is known then we need to do a bounds check
		//Otherwise just set the position
		if(mFileSize>0)
		{
			if(position<mFileSize)
			{
				mPosition=position;
			}
		}else
		{
			mPosition=position;
		}
		return mPosition;
	}

	bool FileReferenceMemory::EndOfFile()
	{
		return (mPosition>=mFileSize);
	}
}
