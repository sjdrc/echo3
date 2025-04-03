#ifndef _ECHOFILESYSTEMSOURCEFILE_H_
#define _ECHOFILESYSTEMSOURCEFILE_H_
#include <echo/FileSystem/FileSystemSource.h>

namespace Echo
{
	class FileSystemSourceFile : public FileSystemSource
	{
	public:
		FileSystemSourceFile(const std::string& identifier="file");
		virtual ~FileSystemSourceFile();
	protected:
		virtual File _Open(const std::string& originalFileName, const std::string& modifiedFileName, File::OpenMode openMode = File::OpenModes::READ) override;
		virtual bool _DeleteFile(const std::string& requestedFileName, const std::string& modifiedFileName) override;
		virtual bool _FileExists(const std::string& requestedFileName, const std::string& modifiedFileName) override;
		virtual bool _CreateDirectories(const std::string& requestedFileName, const std::string& modifiedFileName) override;

	};
}
#endif
