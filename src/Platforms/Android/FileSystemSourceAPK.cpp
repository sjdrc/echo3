#include <echo/Platforms/Android/FileSystemSourceAPK.h>
#include <echo/Platforms/Android/FileReferenceAPK.h>
#include <echo/Util/StringUtils.h>
#include <iostream>

namespace Echo
{
	FileSystemSourceAPK::FileSystemSourceAPK(const std::string& identifier, AAssetManager* assetManager) :
		FileSystemSource(identifier),
		mAssetManager(assetManager)
	{

	}

	FileSystemSourceAPK::~FileSystemSourceAPK()
	{
	}

	File FileSystemSourceAPK::_Open(const std::string& originalFileName, const std::string& modifiedFileName, File::OpenMode openMode)
	{
		// Reduce the file name since we found that the AssetManager doesn't resolve files
		// with .. in the path. See {T530}
		std::string fileName=Utils::String::ReducePath(modifiedFileName);

		if(!mAssetManager)
		{
			ECHO_LOG_ERROR("FileSystemSourceAPK::mAssetManager is null");
			return CreateFile(originalFileName,modifiedFileName);
		}

		//Available modes are the following. We don't have any useful information for detecting
		//what mode to open as at the moment so we will use UNKNOWN.
		//	AASSET_MODE_UNKNOWN
		//	AASSET_MODE_RANDOM
		//	AASSET_MODE_STREAMING
		//	AASSET_MODE_BUFFER
		int mode=AASSET_MODE_UNKNOWN;

		if(openMode!=File::OpenModes::READ)
		{
			//Cannot open APK files for writing.
			return CreateFile(originalFileName,modifiedFileName);
		}
		
		AAsset* asset = AAssetManager_open(mAssetManager, fileName.c_str(), mode);
		if(!asset)
		{
			ECHO_LOG_ERROR("Could not open \"" << fileName << "\"");
			return CreateFile(originalFileName,modifiedFileName);
		}
		shared_ptr<FileReferenceAPK> fileReference(new FileReferenceAPK(*this,asset));
		return CreateFile(originalFileName,modifiedFileName,fileReference);
	}
}
