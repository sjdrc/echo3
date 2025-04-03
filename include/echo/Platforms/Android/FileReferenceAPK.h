#ifndef _ECHOFILEREFERENCEAPK_H_
#define _ECHOFILEREFERENCEAPK_H_
#include <echo/FileSystem/FileReference.h>

struct AAsset;

namespace Echo
{
	class FileReferenceAPK : public FileReference
	{
	public:	
		FileReferenceAPK(FileSystemSource* source,AAsset* asset);
		~FileReferenceAPK();

		size_t Read(void* buffer, size_t typeSize, size_t numberToRead);
		size_t Write(const void* buffer, size_t typeSize, size_t numberToWrite);
		size_t Seek(size_t position);
		bool EndOfFile();
		
	private:
		AAsset* mFile;
	};
}
#endif
