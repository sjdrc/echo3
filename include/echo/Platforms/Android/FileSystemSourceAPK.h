#ifndef _ECHOFILESYSTEMSOURCEAPK_H_
#define _ECHOFILESYSTEMSOURCEAPK_H_
#include <echo/FileSystem/FileSystemSource.h>
#include <android/asset_manager.h>

namespace Echo
{
	class FileSystemSourceAPK : public FileSystemSource
	{
	public:
		FileSystemSourceAPK(const std::string& identifier, AAssetManager* assetManager);
		virtual ~FileSystemSourceAPK();
	protected:
		virtual File _Open(const std::string& originalFileName, const std::string& modifiedFileName, File::OpenMode openMode = File::OpenModes::READ) override;
	private:
		AAssetManager* mAssetManager;
	};
}
#endif
