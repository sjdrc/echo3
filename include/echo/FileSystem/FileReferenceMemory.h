#ifndef _ECHOFILEREFERENCEMEMORY_H_
#define _ECHOFILEREFERENCEMEMORY_H_
#include <echo/FileSystem/FileReference.h>

namespace Echo
{
	class FileSystemSourceMemory;

	class FileReferenceMemory : public FileReference
	{
	public:	
		/**
		 * Constructor for read only memory
		 */
		FileReferenceMemory(FileSystemSource* source, const void* dataLocation, size_t dataSize);
		/**
		 * Constructor for read/write memory.
		 * @note You cannot write past the end of the data block.
		 */
		FileReferenceMemory(FileSystemSource* source, void* dataLocation, size_t dataSize);
		~FileReferenceMemory(){}
		size_t Read(void* buffer, size_t typeSize, size_t numberToRead);
		size_t Write(const void* buffer, size_t typeSize, size_t numberToWrite);
		size_t Seek(size_t position);
		bool EndOfFile();
	private:
		friend class FileSystemSourceMemory;
		const void* mConstData;
		void* mData;
	};
}
#endif
