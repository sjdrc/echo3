#ifndef _ECHOGLSHADER_H_
#define _ECHOGLSHADER_H_

#include <echo/Types.h>
#include <echo/Platforms/GL/GLSupport.h>
#include <string>

namespace Echo
{
	class Shader;

	class GLShader
	{
	public:
		GLShader(GLenum shaderType);
		~GLShader();

		bool Compile(const Shader& shader);

		std::string GetErrors();
		
		GLuint GetShaderHandle() const {return mShaderHandle;}
		size_t GetVersion() const {return mVersion;}
		bool GetCompiled() {return mVersion!=0;}
	private:
		size_t mVersion;
		GLuint mShaderHandle;
	};
}
#endif
