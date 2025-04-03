#include <echo/Kernel/ExecutionModel.h>

namespace Echo
{
	ExecutionModel::ExecutionModel(Model defaultModel) : mCurrentModel(defaultModel)
	{
	}
	
	ExecutionModel::~ExecutionModel()
	{
	}
	
	bool ExecutionModel::SetModelToUse(Model model)
	{
		if(SupportsModel(model))
		{
			mCurrentModel = model;
			return true;
		}
		return false;
	}
}
