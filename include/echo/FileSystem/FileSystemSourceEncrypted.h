#ifndef _ECHOFILESYSTEMSOURCEENCRYPTED_H_
#define _ECHOFILESYSTEMSOURCEENCRYPTED_H_
#include <echo/Types.h>
#include <echo/FileSystem/File.h>
#include <echo/FileSystem/FileSystemSource.h>

namespace Echo
{
	class FileSystemSourceEncrypted : public FileSystemSource
	{
	public:
		FileSystemSourceEncrypted(const std::string& identifier="encrypted");
		virtual ~FileSystemSourceEncrypted();

	protected:
		File _Open(const std::string& originalFileName, const std::string& modifiedFileName, File::OpenMode openMode = File::OpenModes::READ) override;
	};
}
#endif
