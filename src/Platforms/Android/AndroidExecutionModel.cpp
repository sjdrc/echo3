#include <echo/Platforms/Android/AndroidExecutionModel.h>

namespace Echo
{
	AndroidExecutionModel::AndroidExecutionModel() : ExecutionModel(Models::EXTERNAL_CONTROLLER)
	{
	}
	
	AndroidExecutionModel::~AndroidExecutionModel()
	{
	}

	bool AndroidExecutionModel::SupportsModel(Models::ModelEnum model)
	{
		switch (model)
		{
			case Models::CONTROLLER:
			case Models::COOPERATE:
			case Models::NONE:
				return false;
			case Models::EXTERNAL_CONTROLLER:
			return true;
		}
		return false;
	}

	bool AndroidExecutionModel::ProcessEvents(f32 timeOutSuggestion)
	{
		return false;
	}

	bool AndroidExecutionModel::EnterSystemEventManager()
	{
		return false;
	}

	bool AndroidExecutionModel::SendUpdateRequest()
	{
		return true;
	}
}
