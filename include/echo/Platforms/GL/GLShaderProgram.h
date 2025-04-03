#ifndef _ECHOGLSHADERPROGRAM_H_
#define _ECHOGLSHADERPROGRAM_H_

#include <echo/Types.h>
#include <echo/Platforms/GL/GLSupport.h>
#include <echo/Graphics/ShaderProgram.h>
#include <map>
#include <echo/cpp/functional>

namespace Echo
{
	class GLShader;
	
	/**
	 * GLShaderProgram manages the activation of a shader script and setting of any mapped variables.
	 * 
	 * Internally GLShaderProgram manages TargetVariable objects that are used to Set and Unset variables
	 * within shader scripts using GL API calls.
	 * The variables are mapped into ShaderProgram objects, so code can use ShaderProgram::GetUniformVariable()
	 * to acquire a shader pointer to a variable that will be used by these objects to set the variables in
	 * the scripts. Essentially:
	 * 
	 *  ShaderProgram		<- mapping ->		GLShaderProgram
	 *     |                                          TargetVariable
	 *     \---------------- shared_ptr<T> -----------------/
	 * 
	 * Where TargetVariable contains the logic for setting the variable in the GL API.
	 * The mapping between ShaderProgram and GLShaderProgram is managed by GLRenderTarget and corresponding
	 * GLContext. There may be more than one mapping of a ShaderProgram to GLShaderProgram objects.
	 * In the case of multiple mappings there is still only one shared_ptr<T> shared across all, that means
	 * that setting the ShaderProgram variable in code affects all GLShaderPrograms when they are used.
	 */
	class GLShaderProgram
	{
	public:
		GLShaderProgram();
		~GLShaderProgram();

		//Links the program
		bool CompileAndLink(ShaderProgram& shaderProgram);

		//Setting active assigns all of the variables.
		void Activate();
		void Deactivate();

		std::string GetErrors();
		Size GetVersion() const
		{
			return mVersion;
		}
	private:
		bool mLinked;
		GLuint mProgramHandle;
		Size mVersion;
		
		class TargetVariable
		{
		public:
			TargetVariable(){}
			virtual ~TargetVariable(){}
			virtual void Set()=0;
			virtual void Unset(){};
		};

		template < typename T >
		class TargetUniformVariable : public TargetVariable
		{
		public:
			typedef function < void(GLint, T) > SetFunction;
			TargetUniformVariable(GLint location, shared_ptr<T> variable, SetFunction func) :
				mLocation(location),
				mVariable(variable),
				mSetFunction(func)
			{
				assert((mLocation!=-1) && "TargetUniformVariable mLocation cannot be -1");
				assert(mVariable && "TargetUniformVariable mVariable cannot be null");
				assert(mSetFunction && "TargetUniformVariable mSetFunction cannot be null");
			}
			void Set()
			{
				mSetFunction(mLocation, *(mVariable));
			}
		private:
			GLint mLocation;
			shared_ptr<T> mVariable;
			SetFunction mSetFunction;
		};

		template < typename T >
		class TargetUniformVariableFloatVector : public TargetVariable
		{
		public:
			typedef function < void(GLint, GLfloat*) > SetFunction;
			TargetUniformVariableFloatVector(GLint location, shared_ptr<T> variable, SetFunction func) :
				mLocation(location),
				mVariable(variable),
				mSetFunction(func)
			{
				assert((mLocation!=-1) && "TargetUniformVariableFloatVector mLocation cannot be -1");
				assert(mVariable && "TargetUniformVariableFloatVector mVariable cannot be null");
				assert(mSetFunction && "TargetUniformVariableFloatVector mSetFunction cannot be null");
			}
			void Set()
			{
				mSetFunction(mLocation, (GLfloat*)mVariable.get());
			}
		private:
			GLint mLocation;
			shared_ptr<T> mVariable;
			SetFunction mSetFunction;
		};
		
		std::list < shared_ptr<TargetVariable> > mTargetVariables;
		
		std::map< const Shader*, shared_ptr<GLShader> > mShaderLookup;
		
		shared_ptr<GLShader> CompileShader(const Shader* shader);
	};
}
#endif
