#ifndef _ECHOSHADERMANAGER_H_
#define _ECHOSHADERMANAGER_H_

#include <echo/Resource/ResourceManager.h>
#include <echo/Graphics/Shader.h>
namespace Echo
{
	class FileSystem;
	
	/**
	 * ShaderManager is a simple resource manger for shader files.
	 * It is not specific to a single type of shader since all shaders are just source files, instead, when loading
	 * a resource the shader manager will read the file and look for a line that determines the shader type. This
	 * may or may not be important to the underlying shader system.
	 * echo.shader.type=type
	 */
	class ShaderManager : public ResourceManager<Shader>
	{
	public:
		ShaderManager(std::string shaderType, FileSystem& fileSystem);
		~ShaderManager();

		FileSystem* GetFileSystem() const override;
		
		bool LoadIntoResource(const std::string& resourceName, Shader& shaderToLoadInto) override;
	private:
		FileSystem& mFileSystem;
		std::string mShaderType;

		/**
		 * Load Vertex Resource implementation.
		 * @note Providing resourceName does not make the resource available from GetResource(), use AddResource()
		 * followed by GetResource() instead and this method will be called automatically.
		 * @param resourceFile The file to load
		 * @param resourceName The name of the resource once loaded. This name is given to the resource.
		 * @return If there is an error the pointer returned will be null.
		 */
		shared_ptr<Shader> LoadResource(const std::string& resourceFile, const std::string& resourceName) override;
	};
}
#endif 
