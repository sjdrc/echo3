#include <echo/Resource/ShaderManager.h>
#include <echo/Resource/TextureManager.h>
#include <echo/FileSystem/FileSystem.h>

namespace Echo
{
	ShaderManager::ShaderManager(std::string shaderType, FileSystem& fileSystem) :
		ResourceManager<Shader>(shaderType+".shader"),
		mFileSystem(fileSystem),
		mShaderType(shaderType)
	{

	}

	ShaderManager::~ShaderManager()
	{

	}

	shared_ptr<Shader> ShaderManager::LoadResource(const std::string& resourceFile, const std::string& resourceName)
	{
		File f = mFileSystem.Open(resourceFile);
		if(!f.IsOpen())
		{
			ECHO_LOG_ERROR("Could not open \"" << resourceFile << "\"");
			return shared_ptr<Shader>();
		}
		std::string wholeFile;
		f.ReadFileIntoString(wholeFile);
		f.Close();

		Shader* shader = new Shader(mShaderType,resourceName);
		shader->SetSource(wholeFile);
		return shared_ptr<Shader>(shader);
	}

	bool ShaderManager::LoadIntoResource(const std::string& resourceName, Shader& shaderToLoadInto)
	{
		std::string resourceFileName = GetResourceFileName(resourceName);
		if(resourceFileName.empty())
		{
			return false;
		}

		File f = mFileSystem.Open(resourceFileName);
		if(!f.IsOpen())
		{
			ECHO_LOG_ERROR("Could not open \"" << resourceFileName << "\"");
			return false;
		}
		std::string wholeFile;
		f.ReadFileIntoString(wholeFile);
		f.Close();

		shaderToLoadInto.SetSource(wholeFile);
		return true;
	}

	FileSystem* ShaderManager::GetFileSystem() const
	{
		return &mFileSystem;
	}
}
