#ifndef _EGLSHADERSUPPORT_H_
#define _EGLSHADERSUPPORT_H_

#include <echo/Types.h>
#include <echo/Platforms/GL/GLSupport.h>
#ifndef ECHO_GL_SUPPORTS_SHADER
//This is a little hack to get this to build on non-shader supported systems until we sort out the shader system.
typedef Echo::u32 GLuint;
typedef Echo::s32 GLint;
#endif
namespace Echo
{
	struct ShaderDataTypes
	{
		enum _
		{
			//Unfortunately due to the generic names of built in types naming "FLOAT"
			//is likely to conflict with an external library.
			SHADER_NONE,
			SHADER_FLOAT,
			SHADER_INT,
			SHADER_BOOL,
			SHADER_FLOAT_VEC2,
			SHADER_FLOAT_VEC3,
			SHADER_FLOAT_VEC4,
			SHADER_FLOAT_MAT3,
			SHADER_FLOAT_MAT4
		};
	};
	typedef ShaderDataTypes::_ ShaderDataType;
}
#endif
