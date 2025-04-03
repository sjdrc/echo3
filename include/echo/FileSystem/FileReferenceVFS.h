#ifndef _ECHOFILEREFERENCEVFS_H_
#define _ECHOFILEREFERENCEVFS_H_
#include <echo/FileSystem/FileReference.h>
#include <echo/FileSystem/FileSystemSourceVFS.h>

namespace Echo
{
	class FileReferenceVFS : public FileReference
	{
	public:	
		FileReferenceVFS(FileSystemSourceVFS* source, FileSystemSourceVFS::VFSEntry vfsEntry) : 
			FileReference(source),
			mFileEntry(vfsEntry)
		{
			mFileSize = mFileEntry.mDataSize;
		}
		~FileReferenceVFS(){}
		
		size_t Read(void* buffer, size_t typeSize, size_t numberToRead);
		size_t Write(const void* buffer, size_t typeSize, size_t numberToWrite);
		size_t Seek(size_t position);
		bool EndOfFile();
	private:
		friend class FileSystemSourceVFS;
		FileSystemSourceVFS::VFSEntry mFileEntry;
	};
}
#endif
