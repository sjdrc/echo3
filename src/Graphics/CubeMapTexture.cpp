#include <echo/Graphics/CubeMapTexture.h>
#include <echo/Resource/TextureManager.h>

namespace Echo
{
	CubeMapTexture::CubeMapTexture(const std::string& name, TextureManager& textureManager) : Resource<CubeMapTexture>(true),
		mTextureManager(textureManager)
	{
	}
	
	CubeMapTexture::~CubeMapTexture()
	{
	}

	void CubeMapTexture::SetLeft(const std::string& textureName)
	{
		mTextureLeft = mTextureManager.GetResource(textureName);
	}
	
	void CubeMapTexture::SetRight(const std::string& textureName)
	{
		mTextureRight = mTextureManager.GetResource(textureName);
	}
	
	void CubeMapTexture::SetFront(const std::string& textureName)
	{
		mTextureFront = mTextureManager.GetResource(textureName);
	}
	
	void CubeMapTexture::SetBack(const std::string& textureName)
	{
		mTextureBack = mTextureManager.GetResource(textureName);
	}
	
	void CubeMapTexture::SetTop(const std::string& textureName)
	{
		mTextureTop = mTextureManager.GetResource(textureName);
	}
	
	void CubeMapTexture::SetBottom(const std::string& textureName)
	{
		mTextureBottom = mTextureManager.GetResource(textureName);
	}

	void CubeMapTexture::SetLeft(shared_ptr<Texture> texture)
	{
		mTextureLeft = texture;
	}
	
	void CubeMapTexture::SetRight(shared_ptr<Texture> texture)
	{
		mTextureRight = texture;
	}
	
	void CubeMapTexture::SetFront(shared_ptr<Texture> texture)
	{
		mTextureFront = texture;
	}
	
	void CubeMapTexture::SetBack(shared_ptr<Texture> texture)
	{
		mTextureBack = texture;
	}
	
	void CubeMapTexture::SetTop(shared_ptr<Texture> texture)
	{
		mTextureTop = texture;
	}
	
	void CubeMapTexture::SetBottom(shared_ptr<Texture> texture)
	{
		mTextureBottom = texture;
	}

	bool CubeMapTexture::_Unload()
	{
		return false;
	}

	size_t CubeMapTexture::OnRequestMemoryRelease()
	{
		return 0;
	}
}
