#ifndef _ECHOSKELETONMANAGER_H_
#define _ECHOSKELETONMANAGER_H_

#include <echo/Resource/ResourceManager.h>
#include <echo/Animation/Skeleton.h>
namespace Echo
{
	class FileSystem;
	
	class SkeletonManager : public ResourceManager<Skeleton>
	{
	public:
		SkeletonManager(FileSystem& fileSystem);
		~SkeletonManager();

		bool LoadSkeleton(const std::string& resourceFile, Skeleton& targetSkeleton);
		bool LoadIntoResource(const std::string& resourceName, Skeleton& skeleton) override;

		shared_ptr<Skeleton> CreateSkeleton(const std::string& resourceName);

		FileSystem* GetFileSystem() const override;
	private:
		FileSystem& mFileSystem;
		u32 mSkeletonNames;
		
		/**
		 * Load Skeleton Resource implementation.
		 * @note Providing resourceName does not make the resource available from GetResource(), use AddResource()
		 * followed by GetResource() instead and this method will be called automatically.
		 * @param resourceFile The file to load
		 * @param resourceName The name of the resource once loaded. This name is given to the resource.
		 * @return If there is an error the pointer returned will be null.
		 */
		shared_ptr<Skeleton> LoadResource(const std::string& resourceFile, const std::string& resourceName) override;
	};
}
#endif 

