#include <echo/Platforms/Qt/QtExecutionModel.h>
#include <QtCore/QCoreApplication>

namespace Echo
{
	QtExecutionModel::QtExecutionModel(shared_ptr<QCoreApplication> core) : ExecutionModel(Models::COOPERATE), mCore(core)
	{
	}
	
	QtExecutionModel::~QtExecutionModel()
	{
	}

	bool QtExecutionModel::SupportsModel(Models::ModelEnum model)
	{
		switch (model)
		{
			case Models::EXTERNAL_CONTROLLER:
			case Models::CONTROLLER:
				return false;
			case Models::COOPERATE:
			case Models::NONE:
				return true;
		}
		return false;
	}

	bool QtExecutionModel::ProcessEvents(f32 timeOutSuggestion)
	{
		mCore->processEvents();
		return true;
	}

	bool QtExecutionModel::EnterSystemEventManager()
	{
		return false;
	}

	bool QtExecutionModel::SendUpdateRequest()
	{
		return false;
	}
}
