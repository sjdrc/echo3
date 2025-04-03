#include <echo/Platform.h>
#include <echo/FileSystem/FileSystemSourceVFS.h>
#include <echo/FileSystem/FileSystemSourceMemory.h>
#include <echo/FileSystem/FileSystem.h>
#include <boost/filesystem.hpp>

using namespace Echo;

int main(int argc, const char* args[])
{
	// Set the format to only display the message
	gDefaultLogger.SetLogMask("");
	shared_ptr<FileSystem> fileSystem = Platform::CreateDefaultFileSystem("evfsc");
	shared_ptr<FileSystemSourceVFS> vfs = make_shared<FileSystemSourceVFS>(*fileSystem);
	fileSystem->InstallSource("vfs",vfs);
	gDefaultLogger.SetLogMask("INFO|ERROR|WARNING");
	gDefaultLogger.SetFormat("%5$s");
	if(argc < 2)
	{
		ECHO_LOG_INFO("Usage 1 - list contents of vfs file: " << args[0] << " vfsFile");
		ECHO_LOG_INFO("Usage 2 - build vfs file from files or directory: " << args[0] << " vfsOutput fileOrDirectory [...fileOrDirectory]");
		return 0;
	}
	if(argc == 2)
	{
		if(!vfs->LoadVFS(args[1]))
		{
			ECHO_LOG_ERROR("Unable to load VFS file");
			return 1;
		}
		ECHO_LOG_INFO("Size: " << vfs->GetVFSSize());
		const std::map< std::string, FileSystemSourceVFS::VFSEntry >& directory = vfs->GetDirectory();
		for(auto& it : directory)
		{
			ECHO_LOG_INFO(it.first);
		}
		return 1;
	}
	
	// Build VFS file from input
	for(Size i = 2; i < argc; ++i)
	{
		boost::filesystem::recursive_directory_iterator it(args[i]);
		while(it!=boost::filesystem::recursive_directory_iterator())
		{
			if(it->status().type()!=boost::filesystem::regular_file)
			{
				++it;
				continue;
			}
			ECHO_LOG_INFO("Adding " << *it);
			if(!vfs->AddFile(it->path().string()))
			{
				ECHO_LOG_INFO("Failed to add " << *it << " to processing list");
			}
			it++;
		}
	}

	// We'll make options available later
	if(!vfs->SaveVFS(args[1],FileSystemSourceVFS::VFSEndianModes::DEFAULT,FileSystemSourceVFS::VFSEntryNameModes::FULL))
	{
		ECHO_LOG_ERROR("Failed to save VFS.");
		return 1;
	}
	return 0;
}
