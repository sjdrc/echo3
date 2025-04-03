#include <echo/Platforms/Wii/USBFileSystemSource.h>
#include <fat.h>
#include <gccore.h>
#include <ogc/usbstorage.h>

namespace Echo
{
	USBFileSystemSource::USBFileSystemSource(const std::string& identifier) : FileSystemSourceFile(identifier)
	{
		fatInitDefault();
		mMounted=fatMountSimple("usb",&__io_usbstorage);
	}

	USBFileSystemSource::~USBFileSystemSource()
	{
		if(mMounted)
		{
			fatUnmount("usb");
		}
	}

	File USBFileSystemSource::_Open(const std::string& originalFileName, const std::string& modifiedFileName, File::OpenMode openMode)
	{
		std::string fileNameWithPrefix="usb:/" + modifiedFileName;
		return CreateFile(originalFileName,fileNameWithPrefix,FileSystemSourceFile::_Open(originalFileName,fileNameWithPrefix,openMode).GetReference());
	}
}
