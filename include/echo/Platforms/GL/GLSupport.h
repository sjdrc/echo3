#ifndef _EGLSUPPORT_H_
#define _EGLSUPPORT_H_
#include <echo/Logging/Logging.h>

#ifdef ECHO_PLATFORM_WINDOWS

	#if !defined( __MINGW32__ )
	#define WIN32_LEAN_AND_MEAN
	#ifndef NOMINMAX
	#define NOMINMAX // required to stop windows.h messing up std::min
	#endif
	#endif

	#include <windows.h>
	#include <wingdi.h>
	#undef ERROR
	#include <GL/glew.h>
	#include <GL/wglew.h>
	#include <GL/glu.h>
	#include <GL/gl.h>
	#undef TRANSPARENT
	#define ECHO_RENDERER_GL
	#define ECHO_GL_SUPPORTS_TEXTURE_GENERATION
	#define ECHO_GL_SUPPORTS_POINT_SPRITE

	//Disabling these macros for Windows until T323 is fixed
	//#define ECHO_GL_SUPPORTS_FRAGMENT_SHADER
	//#define ECHO_GL_SUPPORTS_VERTEX_SHADER
	//#define ECHO_GL_SUPPORTS_SHADER


#elif ECHO_PLATFORM_LINUX

	#include <GL/glew.h>
	#include <GL/glu.h>
	#define GL_GLEXT_PROTOTYPES
	#define ECHO_RENDERER_GL
	#define ECHO_GL_SUPPORTS_TEXTURE_GENERATION
	#define ECHO_GL_SUPPORTS_POINT_SPRITE
	#define ECHO_GL_SUPPORTS_FRAGMENT_SHADER
	#define ECHO_GL_SUPPORTS_VERTEX_SHADER
	#define ECHO_GL_SUPPORTS_SHADER

#elif ECHO_PLATFORM_ANDROID

#define ECHO_RENDERER_GL

#ifdef ECHO_GLES_SUPPORT
#include <GLES/gl.h>

#define GL_LUMINANCE8 GL_LUMINANCE
#define GL_LUMINANCE8_ALPHA8 GL_LUMINANCE_ALPHA

#define ECHO_RENDERER_GL
#elif defined (ECHO_GLES2_SUPPORT)
#include <GLES2/gl2.h>
#define ECHO_RENDERER_GL
	#define ECHO_GL_SUPPORTS_FRAGMENT_SHADER
	#define ECHO_GL_SUPPORTS_VERTEX_SHADER
	#define ECHO_GL_SUPPORTS_SHADER
#endif


#elif defined (ECHO_PLATFORM_MAC) || (ECHO_PLATFORM_IOS)
	#define ECHO_RENDERER_GL

	#ifdef ECHO_PLATFORM_IOS
	#include <OpenGLES/ES1/gl.h>
	#define GL_GLEXT_PROTOTYPES
	#define ECHO_RENDERER_GL
	#define ECHO_GL_SUPPORTS_TEXTURE_GENERATION
	#define ECHO_GL_SUPPORTS_POINT_SPRITE
	#else
	#include <GL/glew.h>
	#include <OpenGL/glu.h>
	#include <OpenGL/OpenGL.h>
	#define ECHO_GL_SUPPORTS_TEXTURE_GENERATION
	#define ECHO_GL_SUPPORTS_POINT_SPRITE
#endif

#endif

namespace Echo
{
	inline bool CheckOpenGLError_(const char* file, int lineNumber, std::string additional)
	{
		GLenum e = glGetError();
		if(e != GL_NO_ERROR)
		{
			switch(e)
			{
				case GL_INVALID_ENUM:		ECHO_LOG_(Echo::Logger::LogLevels::ERROR, file, lineNumber, "OpenGL: " << e << ": GL_INVALID_ENUM " << additional);		break;
				case GL_INVALID_VALUE:		ECHO_LOG_(Echo::Logger::LogLevels::ERROR, file, lineNumber, "OpenGL: " << e << ": GL_INVALID_VALUE " << additional);		break;
				case GL_INVALID_OPERATION:	ECHO_LOG_(Echo::Logger::LogLevels::ERROR, file, lineNumber, "OpenGL: " << e << ": GL_INVALID_OPERATION " << additional);	break;
				case GL_STACK_OVERFLOW:		ECHO_LOG_(Echo::Logger::LogLevels::ERROR, file, lineNumber, "OpenGL: " << e << ": GL_STACK_OVERFLOW " << additional);	break;
				case GL_STACK_UNDERFLOW:	ECHO_LOG_(Echo::Logger::LogLevels::ERROR, file, lineNumber, "OpenGL: " << e << ": GL_STACK_UNDERFLOW " << additional);	break;
				case GL_OUT_OF_MEMORY:		ECHO_LOG_(Echo::Logger::LogLevels::ERROR, file, lineNumber, "OpenGL: " << e << ": GL_OUT_OF_MEMORY " << additional);		break;
				default:					ECHO_LOG_(Echo::Logger::LogLevels::ERROR, file, lineNumber, "OpenGL: " << e << ": " << additional);						break;
			}
			return false;
		}
		return true;
	}
	#define EchoCheckOpenGLError() CheckOpenGLError_(__FILE__,__LINE__,"")
	#define EchoCheckOpenGLErrorInfo(additional) CheckOpenGLError_(__FILE__,__LINE__,additional)
}

#endif
