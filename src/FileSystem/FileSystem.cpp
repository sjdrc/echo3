#include <algorithm>
#include <iostream>
#include <echo/FileSystem/FileSystem.h>
#include <echo/FileSystem/FileSystemSource.h>
#include <echo/FileSystem/FileReference.h>
#include <echo/Util/StringUtils.h>

namespace Echo
{
	FileSystem::FileSystem()
	{
	}
	
	FileSystem::~FileSystem()
	{
		while(!mSources.empty())
		{
			UninstallSource(mSources.begin()->first);
		}
	}

	void FileSystem::FileClosed(FileReference& fileReference)
	{
		//notification that a file has closed.
		std::list<FileReference*>::iterator it=std::find(mOpenFiles.begin(),mOpenFiles.end(),&fileReference);
		if(it==mOpenFiles.end())
		{
			if(fileReference.GetSource())
			{
				ECHO_LOG_ERROR("Unknown reference closed. This could be an independent FileSystemSource(" << fileReference.GetSource()->GetIdentifier() << ")");
			}else
			{
				ECHO_LOG_ERROR("Unknown reference closed. This could be an independent FileSystemSource(null)");
			}
		}else
		{
			mOpenFiles.erase(it);
		}
	}

	void FileSystem::FileOpened(FileReference& fileReference)
	{
		mOpenFiles.push_back(&fileReference);
	}

	bool FileSystem::InstallSource(shared_ptr<FileSystemSource> source, bool defaultSource)
	{
		return InstallSource(source->GetIdentifier(),source,defaultSource);
	}
	
	bool FileSystem::InstallSource(const std::string& identifier, shared_ptr<FileSystemSource> source, bool defaultSource)
	{
		if(!source)
		{
			ECHO_LOG_ERROR("Attempted to install a null filesystem source as \"" << identifier<< "\"");
			return false;
		}

		if(mSources.find(identifier)!=mSources.end())
		{
			ECHO_LOG_ERROR("Source Identifier '"<< identifier <<"' already exists. Multiple sources per identifier is currently unsupported.");
			return false;
		}
		mSources[identifier]=source;
		if(!source->GetFileSystem() && source->GetFileSystem()!=this)
		{
			source->SetFileSystem(this);
		}else
		{
			ECHO_LOG_WARNING("Source Identifier '"<< identifier <<"' has been installed but internally references another FileSystem object. This is allowed and often intentional but may result in files not found.");
		}

		if(defaultSource)
		{
			ECHO_LOG_INFO("Installed data source: " << identifier << " (Default)");
			mDefaultSource = source;
		}else
		{
			ECHO_LOG_INFO("Installed data source: " << identifier);
		}
		return true;
	}

	bool FileSystem::UninstallSource(const std::string& identifier)
	{
		std::map< std::string, shared_ptr<FileSystemSource> >::iterator sit=mSources.find(identifier);
		if(sit==mSources.end())
		{
			ECHO_LOG_ERROR("Source identifier '"<< identifier <<"' not found.");
			return false;
		}
		sit->second->SetFileSystem(nullptr);
		mSources.erase(sit);
		return true;
	}

	std::list<std::string> FileSystem::GetAvailableSources() const
	{
		std::list<std::string> outSources;
		std::map< std::string, shared_ptr<FileSystemSource> >::const_iterator sit=mSources.begin();
		while(sit!=mSources.end())
		{
			outSources.push_back(sit->first);
			sit++;
		}
		return outSources;
	}

	shared_ptr<FileSystemSource> FileSystem::GetSource(const std::string& identifier) const
	{
		std::map< std::string, shared_ptr<FileSystemSource> >::const_iterator iit=mSources.find(identifier);
		if(iit!=mSources.end())
		{
			return iit->second;
		}
		ECHO_LOG_ERROR("Source Identifier '"<< identifier <<"' not found.");
		return shared_ptr<FileSystemSource>();
	}

	bool FileSystem::SetDefaultSource(const std::string& identifier)
	{
		std::map< std::string, shared_ptr<FileSystemSource> >::iterator sit=mSources.find(identifier);
		if(sit!=mSources.end())
		{
			mDefaultSource = sit->second;
			return true;
		}
		ECHO_LOG_ERROR("Source Identifier '"<< identifier <<"' not found.");
		return false;
	}

	File FileSystem::Open(const std::string& fileName, File::OpenMode openMode)
	{
		std::string fileNameNoSource;
		shared_ptr<FileSystemSource> source = ResolveSource(fileName, fileNameNoSource);
		if(source)
		{
			return source->Open(fileNameNoSource,openMode);
		}
		return File();
	}

	File FileSystem::Open(const std::string& fileName, const std::string& sourceIdentifier, File::OpenMode openMode)
	{
		std::string fileNameNoSource;
		shared_ptr<FileSystemSource> sourceFound = ResolveSource(fileName,fileNameNoSource);
		shared_ptr<FileSystemSource> source = GetSource(sourceIdentifier);
		if(sourceFound && source!=sourceFound)
		{
			ECHO_LOG_WARNING("Found source in '" << fileName << "' varies from specified '" << sourceIdentifier << "' using specified");
		}
		if(source)
		{
			return source->Open(fileNameNoSource,openMode);
		}
		return File();
	}

	shared_ptr<FileSystemSource> FileSystem::ResolveSource(const std::string& fileName, std::string& outFileNameNoSource)
	{
		//Identifier format
		//source://
		//Need to find first of : then the following characters would be //
		size_t firstColon=fileName.find_first_of(':');
		if(firstColon==std::string::npos)
		{
			outFileNameNoSource=fileName;
			return mDefaultSource;
		}
		if(fileName.length()>firstColon+2)
		{
			//We could have a potential source
			if(	fileName[firstColon+1]=='/' && 
				fileName[firstColon+2]=='/')
			{
				outFileNameNoSource=fileName.substr(firstColon+3);
				return GetSource(fileName.substr(0,firstColon));
			}
		}
		outFileNameNoSource=fileName;
		return mDefaultSource;
	}
	
	bool FileSystem::DeleteFile(const std::string& fileName)
	{
		std::string fileNameNoSource;
		shared_ptr<FileSystemSource> source = ResolveSource(fileName, fileNameNoSource);
		if(source)
		{
			return source->DeleteFile(fileNameNoSource);
		}
		return false;
	}

	bool FileSystem::FileExists(const std::string& fileName)
	{
		std::string fileNameNoSource;
		shared_ptr<FileSystemSource> source = ResolveSource(fileName, fileNameNoSource);
		if(source)
		{
			return source->FileExists(fileNameNoSource);
		}
		return false;
	}

	std::string FileSystem::ResolveFullNameForFileWithParent(const std::string& fileName, const std::string& parentFileName)
	{
		// Resolve whether the file is relative or absolute.
		std::string parentPath = Utils::String::GetPathFromFilename(parentFileName);
		if(!FileExists(fileName) && FileExists(parentPath+fileName))
		{
			//It is a relative name.
			return parentPath+fileName;
		}
		return fileName;
	}

	bool FileSystem::CreateDirectories(const std::string& directoryPath)
	{
		std::string fileNameNoSource;
		shared_ptr<FileSystemSource> source = ResolveSource(directoryPath, fileNameNoSource);
		if(source)
		{
			return source->CreateDirectories(fileNameNoSource);
		}
		return false;
	}
}
