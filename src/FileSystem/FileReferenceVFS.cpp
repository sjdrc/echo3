#include <echo/FileSystem/FileSystemSourceVFS.h>
#include <echo/FileSystem/FileReferenceVFS.h>
#include <echo/FileSystem/FileSystem.h>
#include <boost/foreach.hpp>
#include <iostream>

namespace Echo
{
	size_t FileReferenceVFS::Read(void* buffer, size_t typeSize, size_t numToRead)
	{
//		if(!mFileMutex.Lock())
//		{
//			ECHO_LOG_ERROR("Unable to lock file mutex for VFS: " << GetVFSFileName());
//			return 0;
//		}
		size_t seekPosition=mPosition + mFileEntry.mMainOffset;
		static_cast<FileSystemSourceVFS*>(mSource)->mVFSFile.Seek(seekPosition);

		size_t bytesTotal=typeSize*numToRead;
		size_t bytesRemaining=mFileSize-mPosition;
		if(bytesTotal>bytesRemaining)
		{
			numToRead = bytesRemaining / typeSize;
		}

		size_t bytesRead=static_cast<FileSystemSourceVFS*>(mSource)->mVFSFile.Read(buffer,typeSize,numToRead);
		mPosition+=bytesRead;
//		if(!mFileMutex.Release())
//			if(!mFileMutex.Release())		//Try twice
//			{
//				ECHO_LOG_ERROR("Unable to release file mutex for VFS: " << GetVFSFileName());
//			}
		return bytesRead;
	}

	size_t FileReferenceVFS::Write(const void* /*buffer*/, size_t /*typeSize*/, size_t /*numToWrite*/)
	{
		//buffer;typeSize;numToWrite;reference;
		//VFS is Read only atm
		//if(!reference)
		//	return 0;
		//EFileReferenceVFS* fileReference=static_cast<EFileReferenceVFS*>(reference);

		//u32 seekPosition=fileReference->GetPosition()+fileReference->mFileEntry.mMainOffset;
		//mVFSFile.Seek(seekPosition);
		//return mVFSFile.Write(buffer,typeSize,numToGet);
		return 0;
	}

	size_t FileReferenceVFS::Seek(size_t position)
	{
		if(position<mFileSize)
		{
			mPosition=position;
		}
		return mPosition;
	}

	bool FileReferenceVFS::EndOfFile()
	{
		return (mPosition>=mFileSize);
	}
}
