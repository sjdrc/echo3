#ifndef ECHO_CUBEMAPTEXTURE_H
#define ECHO_CUBEMAPTEXTURE_H

#include <echo/Resource/Resource.h>

namespace Echo
{
	class TextureManager;
	class Texture;
	
	/**
	 * Cube map abstraction.
	 */
	class CubeMapTexture : public Resource<CubeMapTexture>
	{
	public:
		CubeMapTexture(const std::string& name, TextureManager& textureManager);
		~CubeMapTexture();
		
		void SetLeft(const std::string& textureName);
		void SetRight(const std::string& textureName);
		void SetFront(const std::string& textureName);
		void SetBack(const std::string& textureName);
		void SetTop(const std::string& textureName);
		void SetBottom(const std::string& textureName);

		void SetLeft(shared_ptr<Texture> texture);
		void SetRight(shared_ptr<Texture> texture);
		void SetFront(shared_ptr<Texture> texture);
		void SetBack(shared_ptr<Texture> texture);
		void SetTop(shared_ptr<Texture> texture);
		void SetBottom(shared_ptr<Texture> texture);

		shared_ptr<Texture> GetTextureLeft() const
		{
			return mTextureLeft;
		}

		shared_ptr<Texture> GetTextureRight() const
		{
			return mTextureRight;
		}

		shared_ptr<Texture> GetTextureFront() const
		{
			return mTextureFront;
		}
		
		shared_ptr<Texture> GetTextureBack() const
		{
			return mTextureBack;
		}

		shared_ptr<Texture> GetTextureTop() const
		{
			return mTextureTop;
		}

		shared_ptr<Texture> GetTextureBottom() const
		{
			return mTextureBottom;
		}
	private:
		virtual bool _Unload() override;
		virtual size_t OnRequestMemoryRelease() override;

		TextureManager& mTextureManager;
		shared_ptr<Texture> mTextureLeft;
		shared_ptr<Texture> mTextureRight;
		shared_ptr<Texture> mTextureFront;
		shared_ptr<Texture> mTextureBack;
		shared_ptr<Texture> mTextureTop;
		shared_ptr<Texture> mTextureBottom;
	};
}
#endif 
