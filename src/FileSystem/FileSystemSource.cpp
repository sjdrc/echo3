#include <echo/FileSystem/FileSystemSource.h>
#include <echo/FileSystem/FileSystem.h>

namespace Echo
{
	FileSystemSource::FileSystemSource(const std::string& identifier) : mFileSystem(0), mIdentifier(identifier)
	{
	}

	FileSystemSource::~FileSystemSource()
	{
	}
	
	File FileSystemSource::Open(const std::string& fileName, File::OpenMode openMode)
	{
		std::string fullName=GetCurrentDirectory()+fileName;
		return _Open(fileName, fullName, openMode);
	}

	bool FileSystemSource::DeleteFile(const std::string& fileName)
	{
		std::string fullName=GetCurrentDirectory()+fileName;
		return _DeleteFile(fileName,fullName);
	}
	
	bool FileSystemSource::FileExists(const std::string& fileName)
	{
		std::string fullName=GetCurrentDirectory()+fileName;
		return _FileExists(fileName,fullName);
	}

	bool FileSystemSource::CreateDirectories(const std::string& directories)
	{
		std::string fullName=GetCurrentDirectory()+directories;
		return _CreateDirectories(directories,fullName);
	}

	void FileSystemSource::SetCurrentDirectory(std::string directory)
	{
		if(!directory.empty() && (directory[directory.length()-1]!='/'))
		{
			directory+='/';
		}
		mCurrentDirectory=directory;
	}

	std::string FileSystemSource::GetCurrentDirectory() const
	{
		return mCurrentDirectory;
	}

	bool FileSystemSource::_DeleteFile(const std::string&, const std::string&)
	{
		//Default implementation does not support file deletion.
		return false;
	}
	
	bool FileSystemSource::_FileExists(const std::string& requestedFileName, const std::string&)
	{
		// We'll try opening the file to determine if it exists. We ignore the modified name since
		// the name is also modified in Open().
		File file = Open(requestedFileName);
		return (file.IsOpen());
	}

	bool FileSystemSource::_CreateDirectories(const std::string& requestedFileName, const std::string& modifiedFileName)
	{
		return false;
	}

	File FileSystemSource::CreateFile(const std::string& requestedFileName, const std::string& actualFileName, shared_ptr<FileReference> fileReference)
	{
		return File(requestedFileName,actualFileName,fileReference);
	}

	File FileSystemSource::CreateFile(const std::string& requestedFileName, const std::string& actualFileName)
	{
		return File(requestedFileName,actualFileName);
	}

	void FileSystemSource::FileOpened(FileReference& reference)
	{
		if(mFileSystem)
		{
			mFileSystem->FileOpened(reference);
		}
	}
	void FileSystemSource::FileClosed(FileReference& reference)
	{
		if(mFileSystem)
		{
			mFileSystem->FileClosed(reference);
		}
	}

}
