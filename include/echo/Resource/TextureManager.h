#ifndef _ECHOTEXTUREEMANAGER_H_
#define _ECHOTEXTUREEMANAGER_H_

#include <echo/Resource/ResourceManager.h>
#include <echo/Graphics/Texture.h>

namespace Echo
{
	class FileSystem;
	class TextureLoader;
	
	class TextureManager : public ResourceManager<Texture>
	{
	public:
		TextureManager(FileSystem& fileSystem);
		~TextureManager();

		/**
		 * Register a loader.
		 * @param loader
		 * @return true on success, false if a loader alread exists for the supported file type or the loader is null.
		 */
		bool RegisterLoader(shared_ptr<TextureLoader> loader);
		
		/**
		 * Create a texture resource.
		 * This method allows you to create a texture resource. You will need to fill the texture pixel data yourself.
		 * @param resourceName The name of the resource, this name can be used later by GetResource() to get the resource.
		 * @param width
		 * @param height
		 * @param format
		 * @return A texture resource if successful, otherwise a null pointer if one of the parameters is bad.
		 */
		shared_ptr<Texture> CreateTexture(const std::string& resourceName, u32 width, u32 height, Texture::Format format);

		/**
		 * Notify method to call when the graphics context is lost.
		 * This is usually called from an existing render system and does not need to be called manually.
		 */
		void GraphicsContextLost();

		/**
		 * Reload resources.
		 */
		void ReloadResources();
		
		/**
		 * Unload resources.
		 */
		void UnloadResources();
		
		/**
		 * Low memory mode is a hint to the TextureManager that we want to reduce memory usage as much as possible.
		 * The affect of this depends on the implementation.
		 * When low memory mode is turned on the TextureManager will attempt to unload any unused resources or
		 * duplicates of texture data which happens on platforms where textures are loaded into system memory and
		 * graphics memory.
		 * @note Low memory mode may require resources to be loaded from the storage device again which may impact 
		 * performance.
		 * @param lowMemoryMode
		 */
		void SetLowMemoryMode(bool lowMemoryMode);
		
		/**
		 * Get whether low memory mode is on or not.
		 * @return 
		 */
		bool GetLowMemoryMode() const {return mLowMemoryMode;}
		
		/**
		 * Request to release any unused memory.
		 * If a texture isn't used by anything then it will be unloaded. This is not just a manual management of
		 * low memory mode, 
		 * @return 
		 */
		size_t RequestMemoryRelease();
		
		/**
		 * Create the default error texture.
		 * @return 
		 */
		virtual shared_ptr<Texture> CreateErrorTexture() const;
		
		/**
		 * Set whether or not to force power of two textures.
		 * @param force
		 */
		void SetForcePowerOfTwoTextures(bool force) {mForcePowerOfTwoTextures = force;}

		FileSystem* GetFileSystem() const override;

		virtual shared_ptr<Texture> GetErrorResource() const override;

		virtual bool LoadIntoResource(const std::string& resourceNameOrFile, Texture& textureToLoadInto) override;
	private:
		/**
		 * Load a resource from the specified file.
		 * The file specifies an image file that a corresponding loader should load. If a loader is available then this loader will
		 * load the file and create a texture. This method is used internally to load an unloaded resource. If you are reading this
		 * to figure out how to load a texture you probably need GetResource().
		 * @param resourceFile	  The resource file to load.
		 * @param resourceName	  The name of the resource to load.
		 * @return A texture resource if successful otherwise a null pointer if loading failed.
		 */
		shared_ptr<Texture> LoadResource(const std::string& resourceFile, const std::string& resourceName) override;
		
		friend class Texture;
		
		/**
		 * Load a resource from a specified resource or file.
		 * Private raw pointer helper for LoadResource(). Make sure you know what you're doing with memory management here,
		 * because the returned pointer is either new memory or not depending on usage.
		 * If a resource name is passed for resourceNameOrFile the resource should be an existing resource. A lookup of the file name
		 * will be performed by using the resource name. If no resource exists then it is assumed that the parameter is a file name
		 * and the method will attempt load.
		 * @param resourceNameOrFile The name of the resource to load or the name of the resource file to load.
		 * @param textureToLoadInto if non-null, this function will load the resource into the textureToLoadInto pointer and return it;
		 *						  if null, this function will allocate a new Texture and return that.
		 * @return a raw pointer to the Texture with the loaded resource. This may or may not be new memory, depending on textureToLoadInto.
		 *		 This will be null if there was a failure.
		 */
		Texture* _LoadResource(const std::string& resourceNameOrFile, Texture* textureToLoadInto = nullptr);
		
		FileSystem& mFileSystem;
		std::map< std::string, shared_ptr<TextureLoader> > mImageLoaders;
		shared_ptr< ResourceDelegate<Texture> > mTextureDelegate;
		bool mLowMemoryMode;	//!< Determines if resources should behave in low memory mode or not, default is off, but some platforms may default to on.
		bool mForcePowerOfTwoTextures;
	};
}
#endif 
