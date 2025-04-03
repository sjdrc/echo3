#include <echo/Resource/TextureManager.h>
#include <echo/Graphics/Texture.h>
#include <echo/FileSystem/FileSystem.h>
#include <echo/Resource/TextureLoader.h>
#include <echo/Util/Utils.h>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/case_conv.hpp>

namespace Echo
{
	TextureManager::TextureManager(FileSystem& fileSystem) :
		ResourceManager<Texture>("texture"),
		mFileSystem(fileSystem),
		mTextureDelegate(ResourceDelegate<Texture>::Create()),
		mLowMemoryMode(false),
		mForcePowerOfTwoTextures(false)
	{
	}

	TextureManager::~TextureManager()
	{
	}

	void TextureManager::SetLowMemoryMode(bool lowMemoryMode)
	{
		mLowMemoryMode = lowMemoryMode;
		if(mLowMemoryMode)
		{
			RequestMemoryRelease();
		}
	}

	bool TextureManager::RegisterLoader(shared_ptr<TextureLoader> loader)
	{
		if(!loader)
		{
			ECHO_LOG_ERROR("TextureManager::RegisterLoader() loader is null.");
			return false;
		}
		std::vector<std::string> extensions = loader->GetFileExtensions();
		for(Size i=0;i < extensions.size(); ++i)
		{
			std::string extension = extensions[i];
			boost::to_lower(extension);
			if(mImageLoaders.find(extension) != mImageLoaders.end())
			{
				ECHO_LOG_ERROR("TextureManager::RegisterLoader() loader for extension \"" << extension << "\" already registered");
				return false;
			}
		
			mImageLoaders[extension] = loader;
		}
		return true;
	}

	shared_ptr<Texture> TextureManager::LoadResource(const std::string& resourceFile, const std::string& resourceName)
	{
		if (Texture* texture = _LoadResource(resourceFile, nullptr))
		{
			texture->SetName(resourceName);
			texture->SetResourceDelegate(mTextureDelegate);
			texture->SetTextureManager(*this);
			return shared_ptr<Texture>(texture);
		}
		return shared_ptr<Texture>(CreateErrorTexture());
	}

	bool TextureManager::LoadIntoResource(const std::string& resourceNameOrFile, Texture& textureToLoadInto)
	{
		return (_LoadResource(resourceNameOrFile,&textureToLoadInto)!=nullptr);
	}
	
	Texture* TextureManager::_LoadResource(const std::string& resourceNameOrFile, Texture* textureToLoadInto)
	{
		//We need to find the resource file name.
		std::string resourceFileName = GetResourceFileName(resourceNameOrFile);
		if(resourceFileName.empty())
		{
			resourceFileName=resourceNameOrFile;
		}
		
		size_t lastDot = 0;
		std::string extension;
		lastDot = resourceFileName.find_last_of(".");
		if(lastDot!=std::string::npos)
		{
			lastDot++; //Want the first pos past the dot
			if(lastDot<resourceFileName.size())
			{
				size_t extensionLength = resourceFileName.size() - lastDot;
				extension = resourceFileName.substr(lastDot, extensionLength);
				boost::to_lower(extension);
			}
		}
		if(extension.empty())
		{
			ECHO_LOG_ERROR("Could not determine file extension for \"" << resourceFileName << "\". Cannot determine which image loader to use.");
			return nullptr;
		}

		//Must load
		std::map< std::string, shared_ptr<TextureLoader> >::iterator eit = mImageLoaders.find(extension);
		if(eit == mImageLoaders.end())
		{
			ECHO_LOG_ERROR("TextureManager::LoadResource() No Loader found for extension \"" << extension << "\" for file \"" << resourceFileName << "\"");
			return nullptr;
		}
		
		//Attempt to open the file.
		File file = mFileSystem.Open(resourceFileName);
		return eit->second->LoadTexture(file, mForcePowerOfTwoTextures, textureToLoadInto);
	}
	
	shared_ptr<Texture> TextureManager::CreateTexture(const std::string& resourceName, u32 width, u32 height, Texture::Format format)
	{
		if(width==0 || height==0 || format==Texture::Formats::UNKNOWN)
		{
			ECHO_LOG_ERROR("Invalid parameters for TextureManager::CreateTexture() width: " << width << " height: " << height << " format: " << format);
		}
		shared_ptr<Texture> texture(new Texture(width,height,format));
		ResourceManager<Texture>::AddResource(resourceName, texture);
		return ResourceManager<Texture>::GetResource(resourceName);
	}

	shared_ptr<Texture> TextureManager::CreateErrorTexture() const
	{
		const u32 textureWidth = 8;
		const u32 textureHeight = 8;
		u32 textureData[textureWidth*textureHeight] = {
			0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
			0xFFFFFFFF,0xFF0000FF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF0000FF,0xFFFFFFFF,
			0xFFFFFFFF,0xFFFFFFFF,0xFF0000FF,0xFFFFFFFF,0xFFFFFFFF,0xFF0000FF,0xFFFFFFFF,0xFFFFFFFF,
			0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF0000FF,0xFF0000FF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
			0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF0000FF,0xFF0000FF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
			0xFFFFFFFF,0xFFFFFFFF,0xFF0000FF,0xFFFFFFFF,0xFFFFFFFF,0xFF0000FF,0xFFFFFFFF,0xFFFFFFFF,
			0xFFFFFFFF,0xFF0000FF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF0000FF,0xFFFFFFFF,
			0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF
		};
		const u32 width = 8;
		const u32 height = 8;
		shared_ptr<Texture> texture = shared_ptr<Texture>(new Texture(width,height, Texture::Formats::R8G8B8A8));
		shared_ptr<u8> buffer = texture->GetBuffer();
		u32* buffer32 = reinterpret_cast<u32*>(buffer.get());
		for(u32 y = 0; y < height; ++y)
		{
			for(u32 x = 0; x < width; ++x)
			{
				f32 ppx = static_cast<f32>(x)/static_cast<f32>(width);
				f32 ppy = static_cast<f32>(y)/static_cast<f32>(height);;
				u32 px = static_cast<u32>(ppx * static_cast<f32>(textureWidth));
				u32 py = static_cast<u32>(ppy * static_cast<f32>(textureHeight));
				u32 i = width*y + x;
			buffer32[i] = textureData[px + py*textureWidth];
			}
		}
		return texture;
	}

	void TextureManager::GraphicsContextLost()
	{
		typedef std::pair< const std::string, ResourceProfile > Resource;
		BOOST_FOREACH(Resource& resource, mResources)
		{
			if(resource.second.mResource)
			{
				resource.second.mResource->RequestMemoryRelease();
			}
		}
	}

	void TextureManager::ReloadResources()
	{
		typedef std::pair< const std::string, ResourceProfile > Resource;
		BOOST_FOREACH(Resource& resource, mResources)
		{
			if(resource.second.mResource)
			{
				resource.second.mResource->Reload();
			}
		}
	}
	
	void TextureManager::UnloadResources()
	{
		typedef std::pair< const std::string, ResourceProfile > Resource;
		BOOST_FOREACH(Resource& resource, mResources)
		{
			if(resource.second.mResource)
			{
				resource.second.mResource->Unload();
			}
		}
	}
	

	size_t TextureManager::RequestMemoryRelease()
	{
		typedef std::pair< const std::string, ResourceProfile > Resource;
		size_t bytesReleased = 0;
		BOOST_FOREACH(Resource& resource, mResources)
		{
			if(resource.second.mResource)
			{
				bytesReleased+=resource.second.mResource->RequestMemoryRelease();
			}
		}
		return bytesReleased;
	}

	FileSystem* TextureManager::GetFileSystem() const
	{
		return &mFileSystem;
	}

	shared_ptr<Texture> TextureManager::GetErrorResource() const
	{
		return CreateErrorTexture();
	}
}
