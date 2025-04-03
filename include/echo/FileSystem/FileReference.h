#ifndef _ECHOFILEREFERENCE_H_
#define _ECHOFILEREFERENCE_H_
#include <echo/Types.h>

#include <echo/FileSystem/FileSystemSource.h>

namespace Echo
{
	class FileSystemSource;
	
	class FileReference
	{
	public:	
		FileReference(FileSystemSource* source) : mPosition(0), mFileSize(0), mSource(source)
		{
			if(mSource)
			{
				mSource->FileOpened(*this);
			}
		}
		virtual ~FileReference()
		{
			if(mSource)
			{
				mSource->FileClosed(*this);
			}
		}
		
		virtual size_t Read(void* buffer, size_t typeSize, size_t numberToRead) = 0;
		virtual size_t Write(const void* buffer, size_t typeSize, size_t numberToWrite) = 0;
		virtual size_t Seek(size_t position) = 0;
		virtual bool EndOfFile() = 0;

		const size_t& GetPosition() const {return mPosition;}
		const size_t& GetFileSize() const {return mFileSize;}
		
		FileSystemSource* GetSource() {return mSource;}
	protected:
		size_t mPosition;
		size_t mFileSize;
		FileSystemSource* mSource;
	};
}

#endif
