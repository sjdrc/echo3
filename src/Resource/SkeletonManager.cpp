#include <echo/Resource/SkeletonManager.h>
#include <echo/Resource/SkeletonReader.h>
#include <echo/Util/StringUtils.h>
#include <echo/FileSystem/FileSystem.h>

namespace Echo
{
	SkeletonManager::SkeletonManager(FileSystem& fileSystem) : 
		ResourceManager<Skeleton>("skeleton"),
		mFileSystem(fileSystem),
		mSkeletonNames(0)
	{
	}

	SkeletonManager::~SkeletonManager()
	{

	}

	shared_ptr<Skeleton> SkeletonManager::LoadResource(const std::string& resourceFile, const std::string& resourceName)
	{
		shared_ptr<Skeleton> skeleton(new Skeleton());
		if(LoadSkeleton(resourceFile, *skeleton))
		{
			skeleton->SetName(resourceName);
		}
		return skeleton;
	}

	bool SkeletonManager::LoadIntoResource(const std::string& resourceName, Skeleton& skeleton)
	{
		//We need to find the resource file name.
		std::string resourceFile = GetResourceFileName(resourceName);
		if(resourceFile.empty())
		{
			return false;
		}
		return LoadSkeleton(resourceName, skeleton);
	}


	shared_ptr<Skeleton> SkeletonManager::CreateSkeleton(const std::string& resourceName)
	{
		shared_ptr<Skeleton> skeleton(new Skeleton());
		skeleton->SetName(resourceName);
		ResourceManager<Skeleton>::AddResource(resourceName,skeleton);
		return ResourceManager<Skeleton>::GetResource(resourceName);
	}

	bool SkeletonManager::LoadSkeleton( const std::string& resourceFile, Skeleton& skeleton)
	{
		File file = mFileSystem.Open(resourceFile);
		if(!file.IsOpen())
		{
			ECHO_LOG_ERROR("Could not open skeleton file " << resourceFile);
			return false;
		}
		return ReadSkeletonFile(file, skeleton);
	}

	FileSystem* SkeletonManager::GetFileSystem() const
	{
		return &mFileSystem;
	}
}
