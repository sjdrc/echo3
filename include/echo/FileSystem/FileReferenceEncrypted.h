#ifndef _ECHOFILEREFERENCEENCRYPTED_H_
#define _ECHOFILEREFERENCEENCRYPTED_H_
#include <echo/FileSystem/FileReference.h>
#include <echo/FileSystem/File.h>

namespace Echo
{
	class FileSystemSourceEncrypted;

	class FileReferenceEncrypted : public FileReference
	{
	public:	
		FileReferenceEncrypted(FileSystemSource* source, File encryptedFile) : FileReference(source), mFile(encryptedFile)
		{
			mFileSize = mFile.GetSize();
		}
		~FileReferenceEncrypted()
		{
			mFile.Close();
		}

		size_t Read(void* buffer, size_t typeSize, size_t numberToRead);
		size_t Write(const void* buffer, size_t typeSize, size_t numberToWrite);
		size_t Seek(size_t position);
		bool EndOfFile();
	private:
		friend class FileSystemSourceEncrypted;
		File mFile;
	};
}
#endif
