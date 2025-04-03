#include <echo/FileSystem/FileSystemSourceFile.h>
#include <echo/FileSystem/FileReferenceFile.h>

#if !defined(ECHO_PLATFORM_ANDROID) && !defined(ECHO_PLATFORM_NINTENDO_GAMECUBE_OR_WII)
#include <boost/filesystem.hpp>
#endif
#include <iostream>

namespace Echo
{
	FileSystemSourceFile::FileSystemSourceFile(const std::string& identifier) : FileSystemSource(identifier)
	{

	}

	FileSystemSourceFile::~FileSystemSourceFile()
	{
	}

	File FileSystemSourceFile::_Open(const std::string& originalFileName, const std::string& modifiedFileName, File::OpenMode openMode)
	{
		std::ios_base::openmode mode=std::ios_base::binary;
		//app	(append) Set the stream's position indicator to the end of the stream before each output operation.
		//ate	(at end) Set the stream's position indicator to the end of the stream on opening.
		//binary	(binary) Consider stream as binary rather than text.
		//in	(input) Allow input operations on the stream.
		//out	(output) Allow output operations on the stream.
		//trunc	(truncate) Any current content is discarded, assuming a length of zero on opening.

		if(openMode!=File::OpenModes::READ)
		{
			// Android std file system support exists but doesn't support boost
			#if !defined(ECHO_PLATFORM_ANDROID) && !defined(ECHO_PLATFORM_NINTENDO_GAMECUBE) && !defined(ECHO_PLATFORM_NINTENDO_WII)
				using namespace boost;
				try
				{
					filesystem::path p;
					p/=modifiedFileName.c_str();
					std::string parentPath = p.parent_path().c_str();
					if(!parentPath.empty() && !filesystem::exists(parentPath) && !CreateDirectories(parentPath))
					{
						ECHO_LOG_ERROR("Failed to create directories for writing " << modifiedFileName);
						return File();
					}
				}
				catch(boost::filesystem::filesystem_error&)
				{
					ECHO_LOG_ERROR("Failed to create directories for writing " << modifiedFileName);
					return File();
				}
			#endif
		}

		switch(openMode)
		{
			case File::OpenModes::READ:
				mode|=std::ios_base::in;
			break;
			case File::OpenModes::WRITE:
				mode|=std::ios_base::out | std::ios_base::trunc;
			break;
			case File::OpenModes::APPEND:
				mode|=std::ios_base::out | std::ios_base::app;
			break;
			case File::OpenModes::READ_WRITE:
				mode|=std::ios_base::in | std::ios_base::out;
			break;
			case File::OpenModes::READ_APPEND:
				mode|=std::ios_base::in | std::ios_base::app;
			break;
		}
		
		shared_ptr<FileReferenceFile> fileReference(new FileReferenceFile(this, modifiedFileName.c_str(), mode));

		if(!fileReference->mFile.good())
		{
			return CreateFile(originalFileName,modifiedFileName);
		}
		return CreateFile(originalFileName,modifiedFileName,fileReference);
	}
	
	bool FileSystemSourceFile::_DeleteFile(const std::string& requestedFileName, const std::string& modifiedFileName)
	{
		// Android std file system support exists but doesn't support boost.
		#if !defined(ECHO_PLATFORM_ANDROID) && !defined(ECHO_PLATFORM_NINTENDO_GAMECUBE) && !defined(ECHO_PLATFORM_NINTENDO_WII)
			using namespace boost;
			try
			{
				filesystem::path p;
				p/=modifiedFileName.c_str();
				filesystem::remove(p);
				return true;
			}
			catch(boost::filesystem::filesystem_error& e)
			{
				ECHO_LOG_ERROR("Failed to delete file: '" << modifiedFileName << "' reason: " << e.what());
			}
		#endif
		return false;
	}

	bool FileSystemSourceFile::_FileExists(const std::string& requestedFileName, const std::string& modifiedFileName)
	{
	#ifdef ECHO_PLATFORM_LINUX
		using namespace boost;
		try
		{
			filesystem::path p;
			p/=modifiedFileName.c_str();
			return filesystem::exists(p);
		}
		catch(boost::filesystem::filesystem_error& e)
		{
			ECHO_LOG_ERROR("Failed to find file: '" << modifiedFileName << "' reason: " << e.what());
		}
	#endif
		// Fall back to attempt to open to detect
		return FileSystemSource::_FileExists(requestedFileName,modifiedFileName);
	}

	bool FileSystemSourceFile::_CreateDirectories(const std::string&, const std::string& modifiedFileName)
	{
#ifdef ECHO_PLATFORM_LINUX
		using namespace boost;
		try
		{
			if(!filesystem::create_directories(modifiedFileName))
			{
				ECHO_LOG_ERROR("Failed to create directories " << modifiedFileName);
				return false;
			}
			return true;
		}
		catch(boost::filesystem::filesystem_error&)
		{
			ECHO_LOG_ERROR("Failed to create directories " << modifiedFileName);
		}
#endif
		return false;
	}
}
