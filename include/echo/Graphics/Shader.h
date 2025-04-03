#ifndef _ECHOSHADER_H_
#define _ECHOSHADER_H_

#include <echo/Types.h>
#include <echo/Resource/Resource.h>
#include <string>

namespace Echo
{
	/**
	 * A shader is part of a ShaderProgram pipeline.
	 * This class is just a container for the program source.
	 * RenderTarget specific resource objects will be created from Shader objects.
	 */
	class Shader : public Resource<Shader>
	{
	public:
		Shader(const std::string& shaderType, const std::string& name);
		~Shader();

		const std::string& GetType() const;

		void SetSource(const std::string& source);
		/**
		 * The version increments each time the source is set.
		 * The version will be checked by render target specific shaders to determine if they
		 * need to be recompiled.
		 */
		const std::string& GetSource() const;
	protected:
		/**
		 * The implementation should free memory in an override of this method if possible.
		 * RequestMemoryRelease() will first check that there is a resource name which should be used when attempting to
		 * reload then check with the delegate to find out if any listeners still need the resource data. If neither of
		 * these are true then this method is not called..
		 * @note It is important that you do not free memory if the implementation will not be able to reload the
		 * resource data on the next Load() call.
		 */
		virtual Size OnRequestMemoryRelease() override;
	private:
		std::string mType;
		std::string mSource;
		
		virtual bool _Unload() override;
	};
}
#endif
