#ifndef _ECHOSHADERPROGRAM_H_
#define _ECHOSHADERPROGRAM_H_

#include <echo/Graphics/Shader.h>
#include <map>

namespace Echo
{
	//An internal structure for managing the shader resource
	class ShaderProgramVariable;

	/**
	 * ShaderProgram is composed of a number of Shader objects and provides methods for setting script variables.
	 * A ShaderProgram is not a specific type, it can have Shader objects added as far as the target API and shading
	 * language will allow.
	 * ShaderProgram is simply a list of Shaders in addition to shader variables that are mapped by name. The
	 * available variables depend on the Shader scrips. You can acquire variables using the methods in this class.
	 * 
	 * GetUniformVariable()		gets a shared pointer to a uniform variable. A variable that has the same value for
	 *							the execution of the program (it can change per execution).
	 * GetAttributeVariable()	gets a shared pointer to a pointer, where the pointer value is initially null and can
	 *							be set so that every vertex accesses a different element of the array that the pointer
	 *							points to.
	 * 
	 * Alternatively you can use the Set*() methods to set values. These are for convenience but if you are doing a
	 * lot of modifications of variables consider acquiring the shared pointers so that you don't have to perform a
	 * map lookup each time (internally through Set*()) you want to modify the value.
	 * 
	 * The ShaderProgram is populated with available variables after a script is compiled and linked via the target
	 * API. Often this can be too late for the first time you use the script as you will want the variable available
	 * before rendering, not as you render.
	 * 
	 * Because you will likely be interacting with scripts with which you know its variables. You can acquire a pointer
	 * before the ShaderProgram is populated. This is done by passing true as the second parameter when you acquire or
	 * set values. It basically allows you to add the variable to the ShaderProgram which the platform implementation
	 * will use instead of creating a new one. By default the parameter is false (to not create) so you explicitly have
	 * to say that you want to create a new internal variable.
	 * 
	 * You can create ShaderProgram objects yourself and use RenderTarget::ActivateProgram() to activate the program.
	 * Typically though, you'll be using a ShaderProgram as part of a rendering process in which there will be other
	 * material properties that you want to set. MaterialManager will create ShaderPrograms for Materials if it is
	 * specified during loading (alternatively you can create a ShaderProgram and add it to a Material). Use the Shader
	 * resource managers to acquire the shaders to add to ShaderPrograms.
	 * 
	 * NOTE: Not all platforms support shaders. For these platforms you should have an alternative implementation.
	 */
	class ShaderProgram : public ResourceListener<Shader>
	{
	private:
		/**
		 * Base for the following classes so we can store them in a list.
		 */
		class Variable
		{
		public:
			Variable(){}
			virtual ~Variable(){}
		};

		/**
		 * Container for a uniform shared variable.
		 */
		template < typename T >
		class UniformVariableContainer : public Variable
		{
		public:
			UniformVariableContainer() : mVariable(make_shared<T>()){}
			~UniformVariableContainer(){}
			shared_ptr<T> mVariable;
		};
	public:
		ShaderProgram();
		~ShaderProgram();

		// Adds a shader to the program.
		void AddShader(shared_ptr<Shader> shader);

		void IncrementVersion()
		{
			++mVersion;
		}

		const std::list< shared_ptr<Shader> >& GetShaders() const {return mShaders;}
		
		//These methods are used by a render system to define what variables are available.
		template <typename T>
		shared_ptr<T> GetUniformVariable( const std::string& name, bool createIfNotFound = false)
		{
			std::map < std::string, shared_ptr<Variable> >::iterator it = mUniformProgramVariables.find(name);
			if(it!=mUniformProgramVariables.end())
			{
				//Check the types match
				shared_ptr<UniformVariableContainer<T> > variable=dynamic_pointer_cast< UniformVariableContainer<T> >(it->second);
				if(!variable)
				{
					ECHO_LOG_ERROR("ShaderProgram::_GetUniformProgramVariables: Tried to get " << name << " but it already exists but is of different type.");
					return shared_ptr<T>();
				}
				return variable->mVariable;
			}
			
			if(createIfNotFound)
			{
				shared_ptr<UniformVariableContainer<T> > variable(new UniformVariableContainer<T>());
				mUniformProgramVariables[name] = variable;
				return variable->mVariable;
			}
			return shared_ptr<T>();
		}
		
		/**
		 * This version of SetUniformVariable() takes a string as the value and attempts to convert it
		 * to the specified type, on failure it returns false. On failure to convert the method does
		 * not create the uniform variable.
		 * @param name The name of the variable. This should match the name in the shader.
		 * @param stringValue The value as a string to be converted to the specified type.
		 * @param createIfNotFound If true the uniform variable will be created for the program, it is not
		 * used if the program does not include the variable. Usually you would set this to true only if
		 * the program has not been compiled.
		 * @return true if the variable was converted, acquired and set.
		 */
		template<typename T>
		bool SetUniformVariable( const std::string& name, const std::string& stringValue, bool createIfNotFound = false)
		{
			T value;
			std::stringstream ss(stringValue);
			ss >> value;
			if(ss.fail())
			{
				return false;
			}
			
			shared_ptr<T> uniformVariable=GetUniformVariable<T>(name,createIfNotFound);
			if(uniformVariable)
			{
				*uniformVariable=value;
				return true;
			}
			return false;
		}
		
		/**
		 * Set a uniform variable in the shader program.
		 * A uniform variable is like a static variable for all executions of the program for a render call.
		 * It can be modified between render calls.
		 * @param name The name of the variable. This should match the name in the shader.
		 * @param value The value to set the variable to.
		 * @param createIfNotFound If true the uniform variable will be created for the program, it is not
		 * used if the program does not include the variable. Usually you would set this to true only if
		 * the program has not been compiled.
		 * @return true if the variable was acquired and set.
		 */
		template<typename T>
		bool SetUniformVariable( const std::string& name, const T& value, bool createIfNotFound = false)
		{
			shared_ptr<T> uniformVariable=GetUniformVariable<T>(name,createIfNotFound);
			if(uniformVariable)
			{
				*uniformVariable=value;
				return true;
			}
			return false;
		}
		
		void ClearAllVariables();

		Size GetVersion() const
		{
			return mVersion;
		}

	private:
		virtual void OnResourceVersionChanged(Shader* resource) override;

		Size mVersion;
		std::list< shared_ptr<Shader> > mShaders;
		std::map < std::string, shared_ptr<Variable> > mUniformProgramVariables;
	};
}
#endif
