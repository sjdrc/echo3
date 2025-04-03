#ifndef _ECHOUSBFILESYSTEMSOURCE_H__
#define _ECHOUSBFILESYSTEMSOURCE_H__
#include <echo/FileSystem/FileSystemSourceFile.h>

namespace Echo
{
	/**
	 * FileSystemSource to access USB storage devices.
	 */
	class USBFileSystemSource : public FileSystemSourceFile
	{
	public:
		USBFileSystemSource(const std::string& identifier);
		virtual ~USBFileSystemSource();
		bool GetMounted() const {return mMounted;}
	protected:
		virtual File _Open(const std::string& originalFileName, const std::string& modifiedFileName, File::OpenMode openMode = File::OpenModes::READ) override;
		bool mMounted;
	};
}
#endif
