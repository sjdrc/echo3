#ifndef _ECHOMESHMANAGER_H_
#define _ECHOMESHMANAGER_H_

#include <echo/Resource/ResourceManager.h>
#include <echo/cpp/functional>

namespace Echo
{
	class FileSystem;
	class Mesh;
	class File;
	class MaterialManager;
	class SkeletonManager;
	
	class MeshManager : public ResourceManager<Mesh>
	{
	public:
		typedef function< bool(const std::string&, FileSystem&, Mesh&) > MeshLoaderFunc;

		MeshManager(FileSystem& fileSystem,	MaterialManager& materialManager, SkeletonManager& skeletonManager);
		~MeshManager();

		static bool Load3DS(const std::string& resourceFile, FileSystem& fileSystem, Mesh& mesh);
		static bool LoadMesh(const std::string& resourceFile, FileSystem& fileSystem,MaterialManager& materialManager, SkeletonManager& skeletonManager, Mesh& mesh);

		/**
		 * Register file loader.
		 * The file loader should use the FileSystem object passed to open the file.
		 * @param fileExtension The file extension the loader is for, e.g. "3ds"
		 * @param loaderFunction
		 * @return 
		 */
		bool RegisterLoader(const std::string& fileExtension, MeshLoaderFunc loaderFunction);
		
		/**
		 * Create a mesh resrouce to be handled by the MeshManager.
		 * The resource will be available to other objects through GetResource().
		 * @param resourceName The name the resource will be referenced by.
		 * @return A pointer to an empty mesh object.
		 */
		shared_ptr<Mesh> CreateMesh(const std::string& resourceName);
		
		/**
		 * Load Mesh Resource implementation.
		 * @note Providing resourceName does not make the resource available from GetResource(), use AddResource()
		 * followed by GetResource() instead and this method will be called automatically.
		 * @param resourceFile The file to load
		 * @param resourceName The name of the resource once loaded. This name is given to the resource.
		 * @return If there is an error the pointer returned will be null.
		 */
		shared_ptr<Mesh> LoadResource(const std::string& resourceFile, const std::string& resourceName) override;

		bool LoadIntoResource(const std::string& resourceName, Mesh& mesh) override;
		
		FileSystem* GetFileSystem() const override;
	private:
		FileSystem& mFileSystem;
		MaterialManager& mMaterialManager;
		SkeletonManager& mSkeletonManager;
		std::map< std::string, MeshLoaderFunc > mMeshLoaders;
		Size mMeshNames;
	};
}
#endif 
