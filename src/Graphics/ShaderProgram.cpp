#include <echo/Graphics/ShaderProgram.h>

namespace Echo
{
	ShaderProgram::ShaderProgram() : mVersion(0)
	{
	}

	ShaderProgram::~ShaderProgram()
	{
	}

	void ShaderProgram::AddShader(shared_ptr<Shader> shader)
	{
		shared_ptr< ResourceDelegate<Shader> > delegate = shader->GetOrCreateResourceDelegate();
		delegate->AddListener(this);
		mShaders.push_back(shader);
	}

	void ShaderProgram::OnResourceVersionChanged(Shader*)
	{
		mVersion++;
	}

	void ShaderProgram::ClearAllVariables()
	{
		mUniformProgramVariables.clear();
	}
}
