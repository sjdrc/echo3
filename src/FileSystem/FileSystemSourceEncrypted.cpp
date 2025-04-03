#include <echo/FileSystem/FileSystemSourceEncrypted.h>
#include <echo/FileSystem/FileReferenceEncrypted.h>
#include <echo/FileSystem/FileSystem.h>

namespace Echo
{
	FileSystemSourceEncrypted::FileSystemSourceEncrypted(const std::string& identifier) : FileSystemSource(identifier)
	{
	}

	FileSystemSourceEncrypted::~FileSystemSourceEncrypted()
	{

	}

	File FileSystemSourceEncrypted::_Open(const std::string& originalFileName, const std::string&, File::OpenMode openMode)
	{
		if(!GetFileSystem())
		{
			return File();
		}
		File encryptedFile = GetFileSystem()->Open(originalFileName,openMode);
		if(!encryptedFile.IsOpen())
		{
			return CreateFile(originalFileName,originalFileName);
		}
		return CreateFile(originalFileName, originalFileName, shared_ptr<FileReferenceEncrypted>(new FileReferenceEncrypted(this,encryptedFile)));
	}
}
