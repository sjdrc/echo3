#ifndef _ECHOWIIEXECUTIONMODEL_H_
#define _ECHOWIIEXECUTIONMODEL_H_
#include <echo/Kernel/ExecutionModel.h>

namespace Echo
{	
	/**
	 * The WiiExecutionModel is exclusive control of the console.
	 */
	class WiiExecutionModel : public ExecutionModel
	{
	public:
		WiiExecutionModel() : ExecutionModel(Models::NONE){}
		~WiiExecutionModel(){}
		bool SupportsModel(Model model) override {return (model==Models::NONE);}
		bool ProcessEvents(f32 timeOutSuggestion) override {return false;}
		bool EnterSystemEventManager() override {return false;}
		bool SendUpdateRequest() override {return false;}
	};
}

#endif
