#ifndef _ECHOGLCUBEMAPTEXTURE_H_
#define _ECHOGLCUBEMAPTEXTURE_H_

#include <echo/Platforms/GL/GLSupport.h>

#ifdef ECHO_RENDERER_GL

namespace Echo
{
	class CubeMapTexture;
	class Texture;

	class GLCubeMapTexture
	{
	public:
		//!\brief Constructor.
		GLCubeMapTexture(CubeMapTexture& cubeMap);
		~GLCubeMapTexture();
		
		void CheckVersion();

		const GLuint& GetGLReference() const {return mTextureReference;}
		
		static GLint GetGLTextureFormat(Texture::Format format);
		static GLint GetGLTexelType(Texture::Format format);
		static GLint GetGLTexelFormat(Texture::Format format);
		size_t GetVersion() const {return mVersion;}

	private:
		bool LoadTexture(Texture* texture, GLint textureSlot);
		GLuint mTextureReference;
		size_t mVersion;
	};
}
#endif

#endif
