#ifndef _ECHORESOURCEMANAGER_H_
#define _ECHORESOURCEMANAGER_H_

#include <echo/FileSystem/FileSystem.h>
#include <echo/Resource/ResourceManagerBase.h>
#include <echo/Resource/ResourceLoader.h>
#include <echo/Util/StringUtils.h>
#include <map>
#ifdef ECHO_EFSW_SUPPORT
#include <echo/Util/DirectoryMonitor.h>
#endif

namespace Echo
{
	template< typename T >
	class ResourceManager : public ResourceManagerBase, public ResourceLoader<T>
	{
	public:
		struct ResourceProfile
		{
			shared_ptr<T> mResource;
			std::string mFile;		//File to load from if the resource is not loaded
		};
		typedef typename std::map< std::string, ResourceProfile >::iterator ResourceIterator;
		
		ResourceManager(const std::string& resourceTypeName) : mResourceType(resourceTypeName)
		{
			#ifdef ECHO_EFSW_SUPPORT
			mDirectoryMonitor = make_shared<DirectoryMonitor>();
			#endif
		}
		
		virtual ~ResourceManager()
		{
			//No explicit cleanup required. Resources will be unloaded as their reference counts become 0.
		}
		
		/**
		 * Adds a resource by specifying the resource file to be loaded when needed.
		 */
		void AddResource(const std::string& name, const std::string& resourceFile)
		{
			ResourceIterator it = mResources.find(name);
			if(it != mResources.end())
			{
				ECHO_LOG_ERROR("ResourceManager(" << mResourceType << ") Resource \"" << name << "\" already added.");
				return;
			}

			ResourceProfile newResourceProfile;
			newResourceProfile.mFile = resourceFile;
			mResources[name] = newResourceProfile;
		}

		/**
		 * Add a resource to this manager.
		 * @note Adding a resource in this way means it cannot be reloaded if needed. Consider adding
		 * a resource by specifying a resource file instead.
		 * @param name name of the resource. This will be used to get the resource later.
		 * @param resource 
		 */
		void AddResource(const std::string& name, shared_ptr<T> resource)
		{
			ResourceIterator it = mResources.find(name);
			if(it != mResources.end())
			{
				ECHO_LOG_ERROR("ResourceManager(" << mResourceType << ") Resource \"" << name << "\" already added.");
				return;
			}

			ResourceProfile newResourceProfile;
			newResourceProfile.mResource = resource;
			mResources[name] = newResourceProfile;
		}

		/**
		 * Remove a resource from the manager.
		 * This method does not delete the resource, it just removes the entry. However, if the
		 * manager held the last reference to the resource it will be deleted.
		 * @param name The name of the resource.
		 * @return true if the resource was found and removed, otherwise false.
		 */
		bool RemoveResource(const std::string& name)
		{
			ResourceIterator it = mResources.find(name);
			if(it == mResources.end())
			{
				ECHO_LOG_ERROR("ResourceManager(" << mResourceType << ") Resource \"" << name << "\" does not exist.");
				return false;
			}
			mResources.erase(it);
			return true;
		}

		/**
		 * Returns the resource if it exists.
		 * If the resource needs to be loaded it will be loaded.
		 * If the resource does not exist and the resource manager has a file system object it will
		 * query to see if the file exists. If the file exists it will add the resource by file name
		 * then load it.
		 * @param nameOrFile The name or file name of the resource
		 * @param relativeToFile If specifying a file, specifying another file will perform a relative
		 * file look up.
		 * @return a value resource of the specialised type if it was found and loaded.
		 */
		shared_ptr<T> GetResource(const std::string& nameOrFile, std::string relativeToFile="")
		{
			ResourceIterator it = mResources.find(nameOrFile);
			if(it == mResources.end())
			{
				// If the resource does not exist this might be a file name.
				FileSystem* fileSystem = GetFileSystem();
				if(fileSystem)
				{
					std::string fileName = fileSystem->ResolveFullNameForFileWithParent(nameOrFile,relativeToFile);
					
					// The resource might be as a filename.
					it = mResources.find(fileName);
					if(it == mResources.end())
					{
						if(fileSystem->FileExists(fileName))
						{
							// We can't just add this resource as the original requested name since multiple paths
							// might contain a file with the same name.
							AddResource(fileName, fileName);
							shared_ptr<T> resource = GetResource(fileName);
							if(resource)
							{
								resource->SetName(fileName);
								resource->SetResourceLoader(this);
							}
							return resource;
						}
					}
				}
				// The iterator will be set above if the resource is found under a different name.
				if(it == mResources.end())
				{
					ECHO_LOG_ERROR("ResourceManager(" << mResourceType << ") Resource \"" << nameOrFile << "\" not found.");
					return GetErrorResource();
				}
			}
			if(!it->second.mResource)
			{
				it->second.mResource = LoadResource(it->second.mFile, nameOrFile);
				if(it->second.mResource)
				{
					it->second.mResource->SetResourceLoader(this);
				}

				#ifdef ECHO_EFSW_SUPPORT
				if(it->second.mResource && mDirectoryMonitor)
				{
					std::string fileToWatch = it->second.mFile;
					mDirectoryMonitor->RegisterFileModifiedCallback(fileToWatch,[this,nameOrFile,fileToWatch](std::string directory, std::string modifiedFilename){
						ResourceIterator it = mResources.find(nameOrFile);
						if(it != mResources.end())
						{
							if(!it->second.mResource->Reload())
							{
								ECHO_LOG_INFO("Resource reload failed: " << nameOrFile);
							}
						}else
						{
							ECHO_LOG_INFO("Resource not found for reload: " << nameOrFile);
						}
					});
				}
				#endif
			}
			return it->second.mResource;
		}

		/**
		 * Get the resource file name from a named resource.
		 * @param resourceName The resource name.
		 * @return The resource file if the resource exists, otherwise an empty string.
		 */
		std::string GetResourceFileName(const std::string& resourceName)
		{
			ResourceIterator it = mResources.find(resourceName);
			if(it == mResources.end())
			{
				return std::string();
			}
			return it->second.mFile;
		}
		
		/**
		 * Cleans any unused Resources from memory.
		 */
		void CleanUnusedResources()
		{
			ResourceIterator it = mResources.begin();
			ResourceIterator itEnd = mResources.end();
			while(it != itEnd)
			{
				if(it->second.mResource)
				{
					if(it->second.mResource.use_count() == 1)
					{
						ECHO_LOG_INFO("Unloading unused resource " << it->second.mFile);
						it->second.mResource.reset();
					}
				}
				++it;
			}
		}

		/**
		 * Check to see if a resource exists.
		 * This method checks the resource list, it does not check if resources are available
		 * via the FileSystem object.
		 * @return true if the resource exists, otherwise false.
		 */
		bool ResourceExists(const std::string& name)
		{
			return (mResources.find(name) != mResources.end());
		}
		
		/**
		 * Check to see if a resource is loaded.
		 * This method checks the resource list, it does not check if resources are available
		 * via the FileSystem object.
		 * @param name The name of the resource.
		 * @return true if the resource is loaded, or false if it isn't loaded or doesn't exist.
		 */
		bool IsResourceLoaded(const std::string& name)
		{
			ResourceIterator it = mResources.find(name);
			if(it == mResources.end())
			{
				ECHO_LOG_ERROR("ResourceManager(" << mResourceType << ")::IsResourceLoaded(): Resource \"" << name << "\" not found.");
				return false;
			}
			return (it->second.mResource);
		}

		/**
		 * Resource specific load method.
		 * @param resourceFile The resource file to load.
		 * @param resourceName The resource name that the file is being loaded for.
		 * @return If successful, the newly loaded resource, otherwise a null pointer.
		 */
		virtual shared_ptr<T> LoadResource(const std::string& resourceFile, const std::string& resourceName) = 0;

		/**
		 * Load a list of resources from a file using a standard format.
		 * The format is:
		 * 
		 *		resource.type.name=resourceFileName,resourceName
		 * 
		 * If only a file name is specified the file name is used as the resource name.
		 * resource.type.name can be determined by calling GetResourceTypeName(). It is the name that is defined
		 * upon construction of the ResourceManager.
		 * @note The behaviour of this method and the exact format can change depending on the resource
		 * manager type. Check the corresponding manager implementation to find out the required format.
		 * @param listFile The list file to read.
		 * @return true if the file was processed, false if the file is not open.
		 */
		virtual bool LoadList(File listFile) override
		{
			ECHO_LOG_INFO("Loading " << mResourceType << " resource list " << listFile.GetActualFileName());
			if(!listFile.IsOpen())
			{
				ECHO_LOG_ERROR("Failed to open: " << listFile.GetActualFileName());
				return false;
			}

			FileSystem* fileSystem=GetFileSystem();
			
			std::vector<std::string> lines;
			listFile.ReadLines(lines,false);
			for(u32 l=0;l<lines.size();++l)
			{
				std::vector<std::string> params;
				std::string token=Utils::String::SplitLine(lines[l],"=",",",params);
				if(params.size()<1)
					continue;
				std::string resourceFileName;

				//If there is a file system object we'll try and resolve the file.
				if(fileSystem)
				{
					resourceFileName = fileSystem->ResolveFullNameForFileWithParent(params[0],listFile.GetRequestedFileName());
				}else
				{
					resourceFileName = params[0];
				}
				
				if(token==mResourceType)
				{
					switch(params.size())
					{
						case 2:	AddResource(params[1],resourceFileName); break;
						case 1:	AddResource(params[0],resourceFileName); break;		//params[0] is the reference used.
					}
				}else
				{
					if(!ProcessListEntry(token,resourceFileName,params))
					{
						ECHO_LOG_ERROR("There was an error processing " << listFile.GetActualFileName() << ". Failed to process line " << l << ": \"" << lines[l] << "\"");
						return false;
					}
				}
			}
			return true;
		}

		/**
		 * Get the resource type name.
		 * @return 
		 */
		virtual const std::string& GetResourceTypeName() const override
		{
			return mResourceType;
		}
		
		/**
		 * Method used to try and get a file system.
		 * Resource Managers can provide a file system object which will be used to resolve
		 * file paths in LoadList(). This allows for paths relative to the list.
		 * @return a pointer to a file system object if available, otherwise a null pointer.
		 */
		virtual FileSystem* GetFileSystem() const = 0;
		
		/**
		 * Get resource associated with the error state for the resource type.
		 * Some managers may have a resource to load to indicate that the resource was not able to load.
		 * This is useful when your resource type should be presented in a particular way to make it obvious
		 * that the resource failed to load. A good example of this is when textures fail to load, they can
		 * be replaced by an error texture.
		 * Managers should override this method to provide an error resource.
		 * @return The error resource, or nullptr.
		 */
		virtual shared_ptr<T> GetErrorResource() const
		{
			return nullptr;
		}

		/**
		 * Load a resource into an existing object.
		 * This is used for resource reloading.
		 */
		virtual bool LoadIntoResource(const std::string& resourceName, T& resourceToLoadInto) override = 0;
	protected:
		/**
		 * This method will be called during LoadList() for non resource type entries.
		 * You may override this method if the entry if your manager needs to support multiple
		 * entry types. For example, material list files define materials using
		 * `material=file,name`, but you can also specify `material.bundle=file` which contains
		 * multiple materials. The override of this method handles the loading of bundles.
		 * @param key The line's key, corresponds to the left hand side of the entry, i.e. key=data.
		 * @param resourceFile The file to process, if a FileSystem object is available the argument
		 * might be a resolved filename relative to the list file, otherwise it is just the specified
		 * entry.
		 * @param params The parameters from the entry, params[0] will be the specified file
		 * (unresolved) and each other string is a another parameter from the line. For example:
		 * `key=abc.xyz,A,B,C` would result in params[0]=abc.xyz (might be different from the
		 * resourceFile argument), params[1]=A, params[2]=B, params[3]=C.
		 * @return true to indicate there weren't any errors, otherwise false to indicate an error.
		 */
		virtual bool ProcessListEntry(const std::string& key, const std::string& resourceFile, const std::vector<std::string>& params)
		{
			return true;
		}
		std::map< std::string,  ResourceProfile > mResources;
		std::string mResourceType;	//Name of the resource type.

		#ifdef ECHO_EFSW_SUPPORT
		shared_ptr<DirectoryMonitor> mDirectoryMonitor;
		#endif
	};
}
#endif 

