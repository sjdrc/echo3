#ifndef ECHO_SIMPLEEXECUTIONMODEL_H
#define ECHO_SIMPLEEXECUTIONMODEL_H

#include <echo/Kernel/ExecutionModel.h>

namespace Echo
{
	/**
	 * A simple execution model that only supports the ExecutionModel::Models::NONE model.
	 * This can be used when no OS or other third party system needs interaction.
	 */
	class SimpleExecutionModel : public ExecutionModel
	{
	public:
		SimpleExecutionModel() : ExecutionModel(Models::NONE){}
		virtual ~SimpleExecutionModel(){}
		bool SupportsModel(Model model) override final
		{
			return (model==Models::NONE);
		}
		bool ProcessEvents(f32) override final
		{
			return true;
		}
		bool EnterSystemEventManager() override final
		{
			return false;
		}
		bool SendUpdateRequest() override final
		{
			return false;
		}
	};
}

#endif /* SIMPLEEXECUTIONMODEL_H */

