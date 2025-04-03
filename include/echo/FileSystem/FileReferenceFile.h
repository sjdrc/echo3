#ifndef _ECHOFILEREFERENCEFILE_H_
#define _ECHOFILEREFERENCEFILE_H_
#include <echo/FileSystem/FileReference.h>
#include <fstream>

namespace Echo
{
	class FileSystemSourceFile;

	class FileReferenceFile : public FileReference
	{
	public:	
		FileReferenceFile(FileSystemSource* source,
						 const char* fileName, std::ios_base::openmode mode) : FileReference(source)
		{
			mFile.open(fileName,mode);
			if(mFile.is_open())
			{
				std::streampos oldPos = mFile.tellg();
				mFile.seekg(0,std::ios_base::end);
				std::streampos length = mFile.tellg();
				mFile.seekg(oldPos,std::ios_base::beg);
			
				if(mFile.good())
				{
					mFileSize = length;
				}else
				{
					mFile.clear();
					mFileSize = 0;
				}
			}
		}
		~FileReferenceFile()
		{
			mFile.close();
		}

		size_t Read(void* buffer, size_t typeSize, size_t numberToRead);
		size_t Write(const void* buffer, size_t typeSize, size_t numberToWrite);
		size_t Seek(size_t position);
		bool EndOfFile();
		
	private:
		friend class FileSystemSourceFile;
		std::fstream mFile;
	};
}

#endif
