#include <echo/Graphics/Shader.h>

namespace Echo
{
	Shader::Shader(const std::string& shaderType, const std::string& name) : Resource<Shader>(true),
		mType(shaderType)
	{
		SetName(name);
	}

	Shader::~Shader()
	{
	}
	
	const std::string& Shader::GetType() const
	{
		return mType;
	}

	const std::string& Shader::GetSource() const
	{
		return mSource;
	}

	void Shader::SetSource(const std::string& source)
	{
		mSource = source;
		IncrementVersion();
	}

	Size Shader::OnRequestMemoryRelease()
	{
		Size memorySaved = mSource.length();
		mSource.clear();
		return memorySaved;
	}

	bool Shader::_Unload()
	{
		mSource.clear();
		return true;
	}
}
