#ifndef _ECHOSDFILESYSTEMSOURCE_H_
#define _ECHOSDFILESYSTEMSOURCE_H_
#include <echo/FileSystem/FileSystemSourceFile.h>

namespace Echo
{
	/**
	 * FileSystemSource to access SD Card storage.
	 */
	class SDFileSystemSource : public FileSystemSourceFile
	{
	public:
		SDFileSystemSource(const std::string& identifier);
		virtual ~SDFileSystemSource();
		bool GetMounted() const {return mMounted;}
	protected:
		virtual File _Open(const std::string& originalFileName, const std::string& modifiedFileName, File::OpenMode openMode = File::OpenModes::READ) override;
		bool mMounted;
	};
}
#endif
