#include <echo/Platforms/GL/GLShader.h>
#include <echo/Graphics/Shader.h>

#ifdef ECHO_GL_SUPPORTS_VERTEX_SHADER

namespace Echo
{
	GLShader::GLShader(GLenum shaderType) : mVersion(0)
	{
		mShaderHandle = glCreateShader(shaderType);
	}

	GLShader::~GLShader()
	{
		glDeleteShader(mShaderHandle);
	}

	bool GLShader::Compile(const Shader& shader)
	{
		if (shader.GetSource().empty())
			return false;

		const char* source = shader.GetSource().c_str();
		glShaderSource(mShaderHandle, 1, (const GLchar**)&source, 0);
		glCompileShader(mShaderHandle);

		int compiled = 0;
		glGetObjectParameterivARB(mShaderHandle, GL_OBJECT_COMPILE_STATUS_ARB, &compiled);
		if (compiled) 
		{
			mVersion=shader.GetVersion();
			return true;
		}else
		{
			mVersion=0;
		}
		return false;
	}

	std::string GLShader::GetErrors()
	{
		if (mShaderHandle==0)
			return "";

		GLint logLength = 0;
		glGetObjectParameterivARB(mShaderHandle, GL_OBJECT_INFO_LOG_LENGTH_ARB , &logLength);
		if (logLength > 1)
		{
			char* compilerLog = new char[logLength];
			GLint outLogLength = 0;
			glGetInfoLogARB(mShaderHandle, logLength, &outLogLength, compilerLog);
			std::string logAsString(compilerLog);
			delete [] compilerLog;
			return logAsString; 
		}
		return "";
	}
}
#endif
