#include <echo/Platforms/GL/GLShaderSupport.h>
#include <echo/Platforms/GL/GLShaderProgram.h>
#include <echo/Graphics/Shader.h>
#include <echo/Platforms/GL/GLShader.h>
#include <echo/Maths/Vector2.h>
#include <echo/Maths/Matrix4.h>
#include <iostream>
#include <boost/foreach.hpp>
#include <echo/cpp/functional>

namespace Echo
{
	GLShaderProgram::GLShaderProgram() :
		mLinked(false),
		mProgramHandle(0),
		mVersion(std::numeric_limits<Size>::max())
	{
	}

	GLShaderProgram::~GLShaderProgram()
	{
	}

	bool GLShaderProgram::CompileAndLink(ShaderProgram& shaderProgram)
	{
#ifdef ECHO_GL_SUPPORTS_SHADER
		//Compile all of the shaders.
		const std::list< shared_ptr<Shader> >& shaders=shaderProgram.GetShaders();
		if(shaders.empty())
		{
			ECHO_LOG_ERROR("GLShaderProgram: no shaders.");
			return false;
		}

		std::vector< shared_ptr<GLShader> > shadersGL;
		shadersGL.reserve(shaders.size());
		
		BOOST_FOREACH(const shared_ptr<Shader>& shader, shaders)
		{
			shared_ptr<GLShader> shaderGL = CompileShader(shader.get());
			if(!shaderGL)
			{
				ECHO_LOG_ERROR("GLShaderProgram: Unable to compile shaders.");
				return false;
			}
			shadersGL.push_back(shaderGL);
		}
		
		if(mLinked)
		{
			//No more work to do.
			return true;
		}

		if(mProgramHandle)
		{
			glDeleteProgram(mProgramHandle);
			mTargetVariables.clear();
		}
		mProgramHandle = glCreateProgram();

		BOOST_FOREACH(shared_ptr<GLShader>& shader, shadersGL)
		{
			glAttachShader(mProgramHandle, shader->GetShaderHandle());
		}
		
		//All shaders should be compiled.
		glLinkProgram(mProgramHandle);

		int linked = 0;
		glGetObjectParameterivARB(mProgramHandle, GL_OBJECT_LINK_STATUS_ARB, &linked);
		if (linked) 
		{
			//Get a list of variables
			GLint numUniforms = 0;
			GLint maxNameLength = 0;
			glGetProgramiv(mProgramHandle, GL_ACTIVE_UNIFORMS, &numUniforms);
			glGetProgramiv(mProgramHandle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLength);

			char* nameBuffer = new char[maxNameLength+1];

			for(GLint v = 0; v < numUniforms; ++v)
			{
				GLint numElements = 0;
				GLenum variableType = 0;

				glGetActiveUniform(mProgramHandle, v, maxNameLength+1, 0, &numElements, &variableType, nameBuffer);
				
				std::string uniformBaseName;
				//Find the base name
				if(numElements>1)
				{
					uniformBaseName = nameBuffer;
					uniformBaseName = uniformBaseName.substr(0,uniformBaseName.find_first_of('['));
				}

				for(GLint i=0;i<numElements;++i)
				{
					std::string uniformName;
					if(numElements>1)
					{
						std::stringstream ss;
						ss << uniformBaseName << "[" << i << "]";
						uniformName = ss.str();
					}else
					{
						uniformName = nameBuffer;
					}
					GLint location = glGetUniformLocation(mProgramHandle, uniformName.c_str());

					GLenum e=glGetError();
					if(e!=GL_NO_ERROR)
					{
						switch(e)
						{
						case GL_INVALID_ENUM: ECHO_LOG_ERROR(e << " - GL_INVALID_ENUM"); break;
						case GL_INVALID_VALUE: ECHO_LOG_ERROR(e << "GL_INVALID_VALUE"); break;
						case GL_INVALID_OPERATION: ECHO_LOG_ERROR(e << "GL_INVALID_OPERATION"); break;
						case GL_STACK_OVERFLOW: ECHO_LOG_ERROR(e << "GL_STACK_OVERFLOW"); break;
						case GL_STACK_UNDERFLOW: ECHO_LOG_ERROR(e << "GL_STACK_UNDERFLOW"); break;
						case GL_OUT_OF_MEMORY: ECHO_LOG_ERROR(e << "GL_OUT_OF_MEMORY"); break;
						default: ECHO_LOG_ERROR("glGetUniformLocation: unhandled error:" << e); break;
						}
					}else
					{
						if(location==-1)
						{
							ECHO_LOG_ERROR("glGetUniformLocation returned a location index of -1 for \"" << uniformName << "\". Ignoring variable.");
							continue;
						}
						switch(variableType)
						{
							case GL_FLOAT:
							{
								shared_ptr<float> variable = shaderProgram.GetUniformVariable<float>(uniformName,true);
								if(variable)
								{
									mTargetVariables.push_back(make_shared<TargetUniformVariable< float > >(location,variable,bind(glUniform1f,placeholders::_1,placeholders::_2)));
								}
							}break;
							case GL_INT:
							{
								shared_ptr<s32> variable = shaderProgram.GetUniformVariable<s32>(uniformName,true);
								if(variable)
								{
									mTargetVariables.push_back(make_shared<TargetUniformVariable< s32> >(location,variable,bind(glUniform1i,placeholders::_1,placeholders::_2)));
								}
							}break;
							case GL_BOOL:
							{
								shared_ptr<bool> variable = shaderProgram.GetUniformVariable<bool>(uniformName,true);
								if(variable)
								{
									mTargetVariables.push_back(make_shared<TargetUniformVariable<bool> >(location,variable,bind(glUniform1i,placeholders::_1,placeholders::_2)));
								}
							}break;
							case GL_FLOAT_VEC2:
							{
								shared_ptr<Vector2> variable = shaderProgram.GetUniformVariable<Vector2>(uniformName,true);
								if(variable)
								{
									mTargetVariables.push_back(make_shared<TargetUniformVariableFloatVector< Vector2 > >(location,variable,bind(glUniform2fv,placeholders::_1,1,placeholders::_2)));
								}
							}break;
							case GL_FLOAT_VEC3:
							{
								shared_ptr<Vector3> variable = shaderProgram.GetUniformVariable<Vector3>(uniformName,true);
								if(variable)
								{
									mTargetVariables.push_back(make_shared<TargetUniformVariableFloatVector< Vector3> >(location,variable,bind(glUniform3fv,placeholders::_1,1,placeholders::_2)));
								}
							}break;
							case GL_FLOAT_VEC4:
							{
								shared_ptr<Vector4> variable = shaderProgram.GetUniformVariable<Vector4>(uniformName,true);
								if(variable)
								{
									mTargetVariables.push_back(make_shared<TargetUniformVariableFloatVector< Vector4> >(location,variable,bind(glUniform4fv,placeholders::_1,1,placeholders::_2)));
								}
							}break;
							case GL_FLOAT_MAT3:
							{
								shared_ptr<Matrix3> variable = shaderProgram.GetUniformVariable<Matrix3>(uniformName,true);
								if(variable)
								{
									mTargetVariables.push_back(make_shared<TargetUniformVariableFloatVector< Matrix3 > >(location,variable,bind(glUniformMatrix3fv,placeholders::_1,1,GL_TRUE,placeholders::_2)));
								}
							}break;
							case GL_FLOAT_MAT4:
							{
								shared_ptr<Matrix4> variable = shaderProgram.GetUniformVariable<Matrix4>(uniformName,true);
								if(variable)
								{
									mTargetVariables.push_back(make_shared<TargetUniformVariableFloatVector< Matrix4> >(location,variable,bind(glUniformMatrix4fv,placeholders::_1,1,GL_TRUE,placeholders::_2)));
								}
							}break;
							case GL_SAMPLER_2D:
							{
								shared_ptr<int> variable = shaderProgram.GetUniformVariable<int>(uniformName,true);
								if(variable)
								{
									mTargetVariables.push_back(make_shared<TargetUniformVariable< int> >(location,variable,bind(glUniform1i,placeholders::_1,placeholders::_2)));
								}
							}break;
							case GL_SAMPLER_CUBE:
							{
								shared_ptr<int> variable = shaderProgram.GetUniformVariable<int>(uniformName,true);
								if(variable)
								{
									mTargetVariables.push_back(make_shared<TargetUniformVariable<int> >(location,variable,bind(glUniform1i,placeholders::_1,placeholders::_2)));
								}
							}break;
							default:
							{
								ECHO_LOG_ERROR("Script contains uniform variable \"" << uniformName << "\" of unsupported type: " << variableType);
							}break;
						}
					}
				}
			}

			delete [] nameBuffer;

			mLinked = true;
			mVersion = shaderProgram.GetVersion();
			return true;
		}
#endif
		return false;
	}

	void GLShaderProgram::Activate()
	{
#ifdef ECHO_GL_SUPPORTS_SHADER
		if(!mProgramHandle)
		{
			ECHO_LOG_ERROR("GLShaderProgram::Activate(): Could not activate shader program. Not built.");
			return;
		}
		glUseProgram(mProgramHandle);
		if (glGetError() != GL_NO_ERROR)
		{
			return;
		}
		
		BOOST_FOREACH(shared_ptr<TargetVariable>& target, mTargetVariables)
		{
			target->Set();
			GLenum e=glGetError();
			if(e!=GL_NO_ERROR)
			{
				switch(e)
				{
				case GL_INVALID_ENUM: ECHO_LOG_ERROR(e << " - GL_INVALID_ENUM"); break;
				case GL_INVALID_VALUE: ECHO_LOG_ERROR(e << "GL_INVALID_VALUE"); break;
				case GL_INVALID_OPERATION: ECHO_LOG_ERROR(e << "GL_INVALID_OPERATION"); break;
				case GL_STACK_OVERFLOW: ECHO_LOG_ERROR(e << "GL_STACK_OVERFLOW"); break;
				case GL_STACK_UNDERFLOW: ECHO_LOG_ERROR(e << "GL_STACK_UNDERFLOW"); break;
				case GL_OUT_OF_MEMORY: ECHO_LOG_ERROR(e << "GL_OUT_OF_MEMORY"); break;
				default: ECHO_LOG_ERROR("Setting shader variable: unhandled error:" << e); break;
				}
			}
		}
#endif
	}
	
	void GLShaderProgram::Deactivate()
	{
#ifdef ECHO_GL_SUPPORTS_SHADER
		glUseProgram(0);
		GLenum e=glGetError();
		if(e!=GL_NO_ERROR)
		{
			switch(e)
			{
			case GL_INVALID_ENUM: ECHO_LOG_ERROR(e << " - GL_INVALID_ENUM"); break;
			case GL_INVALID_VALUE: ECHO_LOG_ERROR(e << "GL_INVALID_VALUE"); break;
			case GL_INVALID_OPERATION: ECHO_LOG_ERROR(e << "GL_INVALID_OPERATION"); break;
			case GL_STACK_OVERFLOW: ECHO_LOG_ERROR(e << "GL_STACK_OVERFLOW"); break;
			case GL_STACK_UNDERFLOW: ECHO_LOG_ERROR(e << "GL_STACK_UNDERFLOW"); break;
			case GL_OUT_OF_MEMORY: ECHO_LOG_ERROR(e << "GL_OUT_OF_MEMORY"); break;
			default: ECHO_LOG_ERROR("glUseProgram(0): unhandled error:" << e); break;
			}
		}
		
		BOOST_FOREACH(shared_ptr<TargetVariable>& target, mTargetVariables)
		{
			target->Unset();
			GLenum e=glGetError();
			if(e!=GL_NO_ERROR)
			{
				switch(e)
				{
				case GL_INVALID_ENUM: ECHO_LOG_ERROR(e << " - GL_INVALID_ENUM"); break;
				case GL_INVALID_VALUE: ECHO_LOG_ERROR(e << "GL_INVALID_VALUE"); break;
				case GL_INVALID_OPERATION: ECHO_LOG_ERROR(e << "GL_INVALID_OPERATION"); break;
				case GL_STACK_OVERFLOW: ECHO_LOG_ERROR(e << "GL_STACK_OVERFLOW"); break;
				case GL_STACK_UNDERFLOW: ECHO_LOG_ERROR(e << "GL_STACK_UNDERFLOW"); break;
				case GL_OUT_OF_MEMORY: ECHO_LOG_ERROR(e << "GL_OUT_OF_MEMORY"); break;
				default: ECHO_LOG_ERROR("Resetting shader variable: unhandled error:" << e); break;
				}
			}
		}
#endif
	}

	std::string GLShaderProgram::GetErrors()
	{
#ifdef ECHO_GL_SUPPORTS_SHADER 
		if (mProgramHandle==0)
			return "";

		GLint logLength = 0;
		glGetObjectParameterivARB(mProgramHandle, GL_OBJECT_INFO_LOG_LENGTH_ARB , &logLength);
		if (logLength > 1)
		{
			char* compilerLog = new char[logLength];
			GLint outLogLength = 0;
			glGetInfoLogARB(mProgramHandle, logLength, &outLogLength, compilerLog);
			std::string logAsString(compilerLog);
			delete [] compilerLog;
			return logAsString;
		}
#endif
		return "";
	}
	
	shared_ptr<GLShader> GLShaderProgram::CompileShader(const Shader* shader)
	{
#ifdef ECHO_GL_SUPPORTS_SHADER
		std::map<const Shader*, shared_ptr<GLShader> >::iterator it = mShaderLookup.find(shader);
		if(it==mShaderLookup.end())
		{
			//Determine shader type
			GLenum shaderType=0;
			if(shader->GetType()=="geometry")
			{
				shaderType=GL_GEOMETRY_SHADER;
			}else
			if(shader->GetType()=="vertex")
			{
				shaderType=GL_VERTEX_SHADER;
			}else
			if(shader->GetType()=="fragment")
			{
				shaderType=GL_FRAGMENT_SHADER;
			}else
			{
				ECHO_LOG_ERROR("Unsupported shader type");
				return nullptr;
			}
			
			//Create the shader
			shared_ptr<GLShader> shaderGL(new GLShader(shaderType));
			
			mLinked = false;
			
			//Compile it
			if(!shaderGL->Compile(*shader))
			{
				ECHO_LOG_ERROR("Failed to compile shader:");
				ECHO_LOG_ERROR(shaderGL->GetErrors());
				return shared_ptr<GLShader>();
			}
			mShaderLookup.insert(std::make_pair(shader,shaderGL));
			return shaderGL;
		}else
		{
			//Check version
			if(it->second->GetVersion()!=shader->GetVersion())
			{
				mLinked = false;
				if(!it->second->Compile(*shader))
				{
					ECHO_LOG_ERROR("Failed to compile shader:");
					ECHO_LOG_ERROR(it->second->GetErrors());
					return shared_ptr<GLShader>();
				}
			}
		}
		return it->second;
#else
		return shared_ptr<GLShader>();
#endif
	}	
}
