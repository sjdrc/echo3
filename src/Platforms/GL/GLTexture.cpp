#include <echo/Graphics/Texture.h>
#include <echo/Platforms/GL/GLSupport.h>
#include <iostream>

#include <echo/Platforms/GL/GLTexture.h>

namespace Echo
{
	GLTexture::GLTexture(Texture& texture)
	{
		glGenTextures(1, &mTextureReference);
		if(!EchoCheckOpenGLErrorInfo("glGenTextures(1, &mTextureReference)"))
		{
			return;
		}

		// Typical Texture Generation Using Data From The Bitmap
		glBindTexture(GL_TEXTURE_2D, mTextureReference);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		GLint textureFormat = GetGLTextureFormat(texture.GetFormat());
		GLint texelFormat = GetGLTexelFormat(texture.GetFormat());
		GLint texelType = GetGLTexelType(texture.GetFormat());
		glTexImage2D(GL_TEXTURE_2D, 0, textureFormat, texture.GetWidth(), texture.GetHeight(), 0, texelFormat, texelType, texture.GetBuffer().get());
		EchoCheckOpenGLErrorInfo("glTexImage2D()");
		mVersion = texture.GetVersion();
	}

	GLTexture::~GLTexture()
	{
		if(glIsTexture(mTextureReference)==GL_TRUE)
		{
			glDeleteTextures(1,&mTextureReference);
		}
		mTextureReference = 0;
	}

	bool GLTexture::GetData(Texture& outTexture)
	{
		if(!outTexture.GetBuffer())
		{
			return false;
		}
		void* buffer = outTexture.GetBuffer().get();
		int bufferSize = outTexture.GetDataSize();
		Texture::Format textureFormat = outTexture.GetFormat();

		GLint format = GetGLTexelFormat(textureFormat);
		GLint type = GetGLTexelType(textureFormat);
		if(format==GL_FALSE || type==GL_FALSE)
		{
			return false;
		}
		glGetTextureImage(mTextureReference,0,format,type,bufferSize,buffer);

		GLint e=glGetError();
		if(e!=GL_NO_ERROR)
		{
			switch(e)
			{
				case GL_INVALID_ENUM:		ECHO_LOG_ERROR("glGetTextureImage: " << e << " - GL_INVALID_ENUM");		break;
				case GL_INVALID_VALUE:		ECHO_LOG_ERROR("glGetTextureImage: " << e << " - GL_INVALID_VALUE");	break;
				case GL_INVALID_OPERATION:	ECHO_LOG_ERROR("glGetTextureImage: " << e << " - GL_INVALID_OPERATION");break;
				case GL_STACK_OVERFLOW:		ECHO_LOG_ERROR("glGetTextureImage: " << e << " - GL_STACK_OVERFLOW");	break;
				case GL_STACK_UNDERFLOW:	ECHO_LOG_ERROR("glGetTextureImage: " << e << " - GL_STACK_UNDERFLOW");	break;
				case GL_OUT_OF_MEMORY:		ECHO_LOG_ERROR("glGetTextureImage: " << e << " - GL_OUT_OF_MEMORY");	break;
				default:					ECHO_LOG_ERROR("glGetTextureImage: " << e);
			}
			return false;
		}
		return true;
	}

	GLint GLTexture::GetGLTextureFormat(Texture::Format format)
	{
		switch(format)
		{
			case Texture::Formats::R5G6B5:
				return GL_RGB565;
			case Texture::Formats::R8G8B8:
			case Texture::Formats::B8G8R8:		// BGR will be converted to RGB internally
				return GL_RGB8;
			case Texture::Formats::R4G4B4A4:
				return GL_RGBA4;
			case Texture::Formats::R5G5B5A1:
				return GL_RGB5_A1;
			case Texture::Formats::R8G8B8A8:
				return GL_RGBA8;
			case Texture::Formats::R8G8B8X8:
				return GL_RGBA8;
			case Texture::Formats::RGB_F32:
				return GL_RGB32F;
			case Texture::Formats::RGBA_F32:
				return GL_RGBA32F;
			case Texture::Formats::B8G8R8A8:	// BGRA will be converted to RGBA internally
				return GL_RGBA8;
			case Texture::Formats::R10G10B10X2:
				return GL_RGB10_A2;
			case Texture::Formats::R10G10B10A2:
				return GL_RGB10_A2;
			case Texture::Formats::LUMINANCE8:
				return GL_LUMINANCE;
			case Texture::Formats::GREYSCALE8:
				return GL_R8;
			case Texture::Formats::LUMINANCE32:
				return GL_R32F;
			case Texture::Formats::GREYSCALE16:
				return GL_RED;
			case Texture::Formats::DEPTH16:
				return GL_DEPTH_COMPONENT16;
			case Texture::Formats::DEPTH24:
				return GL_DEPTH_COMPONENT24;
			case Texture::Formats::DEPTH32:
				return GL_DEPTH_COMPONENT32;
			case Texture::Formats::DEPTH24_STENCIL8:
				return GL_DEPTH24_STENCIL8;
			case Texture::Formats::LUMINANCE8_ALPHA8:
				return GL_LUMINANCE8_ALPHA8;
			case Texture::Formats::LUMINANCE32_ALPHA32:
				return GL_RG32F;
			case Texture::Formats::UNKNOWN:
				ECHO_LOG_WARNING("GetGLTextureFormat() called with Formats::UNKNOWN.");
				return GL_FALSE;
		};
		return GL_FALSE;
	}
	
	GLint GLTexture::GetGLTexelType(Texture::Format format)
	{
		switch(format)
		{
			case Texture::Formats::R5G6B5:
				return GL_UNSIGNED_SHORT_5_6_5;
			case Texture::Formats::R5G5B5A1:
				return GL_UNSIGNED_SHORT_5_5_5_1;
			case Texture::Formats::R4G4B4A4:
				return GL_UNSIGNED_SHORT_4_4_4_4;
			case Texture::Formats::R8G8B8X8:
			case Texture::Formats::R8G8B8A8:
			case Texture::Formats::R8G8B8:
			case Texture::Formats::B8G8R8:
			case Texture::Formats::B8G8R8A8:
				break;
			case Texture::Formats::LUMINANCE8:
			case Texture::Formats::GREYSCALE8:
				return GL_UNSIGNED_BYTE;
			case Texture::Formats::DEPTH24:
			case Texture::Formats::DEPTH32:
				return GL_UNSIGNED_INT;
			case Texture::Formats::LUMINANCE8_ALPHA8:
				return GL_UNSIGNED_BYTE;
			case Texture::Formats::GREYSCALE16:
			case Texture::Formats::DEPTH16:
				return GL_UNSIGNED_SHORT;
			case Texture::Formats::DEPTH24_STENCIL8:
				return GL_UNSIGNED_INT_24_8;
			case Texture::Formats::LUMINANCE32_ALPHA32:
			case Texture::Formats::LUMINANCE32:
			case Texture::Formats::RGBA_F32:
			case Texture::Formats::RGB_F32:
				return GL_FLOAT;
			case Texture::Formats::R10G10B10X2:
			case Texture::Formats::R10G10B10A2:
				return GL_UNSIGNED_INT_10_10_10_2;

			case Texture::Formats::UNKNOWN:
				ECHO_LOG_WARNING("GetGLTexelType() called with Formats::UNKNOWN.");
				return GL_FALSE;
		};
		return GL_UNSIGNED_BYTE;
	}
	
	GLint GLTexture::GetGLTexelFormat(Texture::Format format)
	{
		switch(format)
		{
			case Texture::Formats::R5G6B5:
			case Texture::Formats::R8G8B8:
			case Texture::Formats::RGB_F32:
				return GL_RGB;
			case Texture::Formats::B8G8R8:
				return GL_BGR;
			case Texture::Formats::B8G8R8A8:
				return GL_BGRA;
			case Texture::Formats::R5G5B5A1:
			case Texture::Formats::R8G8B8A8:
			case Texture::Formats::R8G8B8X8:
			case Texture::Formats::R4G4B4A4:
			case Texture::Formats::RGBA_F32:
			case Texture::Formats::R10G10B10X2:
			case Texture::Formats::R10G10B10A2:
				break;
			case Texture::Formats::LUMINANCE8:
			case Texture::Formats::LUMINANCE32:
			case Texture::Formats::GREYSCALE8:
			case Texture::Formats::GREYSCALE16:
				return GL_RED;
			case Texture::Formats::DEPTH16:
			case Texture::Formats::DEPTH24:
			case Texture::Formats::DEPTH32:
				return GL_DEPTH_COMPONENT;
			case Texture::Formats::DEPTH24_STENCIL8:
				return GL_DEPTH_STENCIL;
			case Texture::Formats::LUMINANCE8_ALPHA8:
			case Texture::Formats::LUMINANCE32_ALPHA32:
				return GL_LUMINANCE_ALPHA;
			case Texture::Formats::UNKNOWN:
				ECHO_LOG_WARNING("GetGLTexelFormat() called with Formats::UNKNOWN.");
				break;
		};
		return GL_RGBA;
	}	
}
