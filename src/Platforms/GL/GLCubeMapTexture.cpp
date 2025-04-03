#include <echo/Graphics/Texture.h>
#include <echo/Graphics/CubeMapTexture.h>
#include <echo/Platforms/GL/GLSupport.h>
#include <iostream>

#include <echo/Platforms/GL/GLCubeMapTexture.h>

namespace Echo
{
	GLCubeMapTexture::GLCubeMapTexture(CubeMapTexture& cubeMap)  
	{
		glGenTextures(1, &mTextureReference);
		GLint e=glGetError();
		if(e!=GL_NO_ERROR)
		{
			ECHO_LOG_ERROR("glGenTextures(1, &mTextureReference)");
			return;
		}

		glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureReference);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		LoadTexture(cubeMap.GetTextureRight().get(), GL_TEXTURE_CUBE_MAP_POSITIVE_X);
		LoadTexture(cubeMap.GetTextureLeft().get(), GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
		LoadTexture(cubeMap.GetTextureFront().get(), GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
		LoadTexture(cubeMap.GetTextureBack().get(), GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
		LoadTexture(cubeMap.GetTextureTop().get(), GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
		LoadTexture(cubeMap.GetTextureBottom().get(), GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);

		mVersion = cubeMap.GetVersion();
	}

	GLCubeMapTexture::~GLCubeMapTexture()
	{
		if(glIsTexture(mTextureReference)==GL_TRUE)
		{
			glDeleteTextures(1,&mTextureReference);
		}
		mTextureReference = 0;
	}

	bool GLCubeMapTexture::LoadTexture(Texture* texture, GLint textureSlot)
	{
		if(!texture)
		{
			// Not logging an error because it may deliberate to exclude a cube map face
			return false;
		}

		GLint textureFormat = GetGLTextureFormat(texture->GetFormat());
		GLint texelFormat = GetGLTexelFormat(texture->GetFormat());
		GLint texelType = GetGLTexelType(texture->GetFormat());
		glTexImage2D(textureSlot, 0, textureFormat, texture->GetWidth(), texture->GetHeight(), 0, texelFormat, texelType, texture->GetBuffer().get());

		GLint e=glGetError();
		if(e!=GL_NO_ERROR)
		{
			switch(e)
			{
				case GL_INVALID_ENUM:		ECHO_LOG_ERROR("OGL Texture Generation: " << e << " - GL_INVALID_ENUM");		break;
				case GL_INVALID_VALUE:		ECHO_LOG_ERROR("OGL Texture Generation: " << e << " - GL_INVALID_VALUE");	break;
				case GL_INVALID_OPERATION:	ECHO_LOG_ERROR("OGL Texture Generation: " << e << " - GL_INVALID_OPERATION");break;
				case GL_STACK_OVERFLOW:		ECHO_LOG_ERROR("OGL Texture Generation: " << e << " - GL_STACK_OVERFLOW");	break;
				case GL_STACK_UNDERFLOW:	ECHO_LOG_ERROR("OGL Texture Generation: " << e << " - GL_STACK_UNDERFLOW");	break;
				case GL_OUT_OF_MEMORY:		ECHO_LOG_ERROR("OGL Texture Generation: " << e << " - GL_OUT_OF_MEMORY");	break;
				default:					ECHO_LOG_ERROR("OGL Texture Generation: " << e);
			}
			return false;
		}
		return true;
	}

	GLint GLCubeMapTexture::GetGLTextureFormat(Texture::Format format)
	{
		switch(format)
		{
			case Texture::Formats::R5G6B5:
			case Texture::Formats::R8G8B8:
			case Texture::Formats::RGB_F32:
			case Texture::Formats::B8G8R8:		// BGR will be converted to RGB internally
				return GL_RGB;
			case Texture::Formats::R4G4B4A4:
			case Texture::Formats::R5G5B5A1:
			case Texture::Formats::R8G8B8A8:
			case Texture::Formats::R8G8B8X8:
			case Texture::Formats::RGBA_F32:
			case Texture::Formats::B8G8R8A8:	// BGRA will be converted to RGBA internally
				break;
			case Texture::Formats::LUMINANCE8:
			case Texture::Formats::GREYSCALE8:
				return GL_LUMINANCE8;
			case Texture::Formats::LUMINANCE32:
			case Texture::Formats::GREYSCALE16:
				// Try and give a larger range
				return GL_LUMINANCE16;
			case Texture::Formats::LUMINANCE8_ALPHA8:
				return GL_LUMINANCE8_ALPHA8;
			case Texture::Formats::LUMINANCE32_ALPHA32:
				return GL_LUMINANCE16_ALPHA16;
			case Texture::Formats::UNKNOWN:
				ECHO_LOG_WARNING("GetGLTextureFormat() called with Formats::UNKNOWN.");
				return GL_FALSE;
		};
		return GL_RGBA;
	}
	
	GLint GLCubeMapTexture::GetGLTexelType(Texture::Format format)
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
			case Texture::Formats::LUMINANCE8_ALPHA8:
				return GL_UNSIGNED_BYTE;
			case Texture::Formats::GREYSCALE16:
				return GL_UNSIGNED_SHORT;
			case Texture::Formats::LUMINANCE32_ALPHA32:
			case Texture::Formats::LUMINANCE32:
			case Texture::Formats::RGBA_F32:
			case Texture::Formats::RGB_F32:
				return GL_FLOAT;
			case Texture::Formats::UNKNOWN:
				ECHO_LOG_WARNING("GetGLTexelType() called with Formats::UNKNOWN.");
				return GL_FALSE;
		};
		return GL_UNSIGNED_BYTE;
	}
	
	GLint GLCubeMapTexture::GetGLTexelFormat(Texture::Format format)
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
				break;
			case Texture::Formats::LUMINANCE8:
			case Texture::Formats::LUMINANCE32:
			case Texture::Formats::GREYSCALE8:
			case Texture::Formats::GREYSCALE16:
				return GL_LUMINANCE;
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
