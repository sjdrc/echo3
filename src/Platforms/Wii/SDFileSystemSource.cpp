#include <echo/Platforms/Wii/SDFileSystemSource.h>
#include <fat.h>
#include <sdcard/wiisd_io.h>
#include <iostream>

namespace Echo
{
	SDFileSystemSource::SDFileSystemSource(const std::string& identifier) : FileSystemSourceFile(identifier)
	{
		fatInitDefault();
		mMounted=fatMountSimple("sd",&__io_wiisd);
	}

	SDFileSystemSource::~SDFileSystemSource()
	{
		if(mMounted)
		{
			fatUnmount("sd");
		}
	}

	File SDFileSystemSource::_Open(const std::string& originalFileName, const std::string& modifiedFileName, File::OpenMode openMode)
	{
		std::string fileNameWithPrefix="sd:/" + modifiedFileName;
		return CreateFile(originalFileName,fileNameWithPrefix,FileSystemSourceFile::_Open(originalFileName,fileNameWithPrefix,openMode).GetReference());
	}
}
