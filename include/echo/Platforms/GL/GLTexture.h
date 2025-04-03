#ifndef _ECHOGLTEXTURE_H_
#define _ECHOGLTEXTURE_H_

#include <echo/Platforms/GL/GLSupport.h>

#ifdef ECHO_RENDERER_GL

namespace Echo
{
	class Texture;
	class GLTexture
	{
	public:
		//!\brief Constructor.
		GLTexture(Texture& texture);
		~GLTexture();
		
		const GLuint& GetGLReference() const {return mTextureReference;}
		
		static GLint GetGLTextureFormat(Texture::Format format);
		static GLint GetGLTexelType(Texture::Format format);
		static GLint GetGLTexelFormat(Texture::Format format);
		Size GetVersion() const {return mVersion;}

		/**
		 * Setting the version is an advanced feature it is not recommended to do it manually.
		 * You should know how these objects 
		 */
		void SetVersion(Size version)
		{
			mVersion = version;
		}

		/**
		 * Get the texture data.
		 * This method tries to get the data and put it into the texture. The texture needs to be configured and the buffer allocated with the expected format.
		 * @param outTexture The texture to write data into.
		 * @return true on success, false indicates there was an error with the input texture or getting the data from the GL buffer, check the log for GL
		 * errors and refer to the manual.
		 */
		bool GetData(Texture& outTexture);

	private:
		GLuint mTextureReference;
		Size mVersion;
	};
}
#endif

#endif
