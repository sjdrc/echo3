#include <echo/Platforms/Windows/WindowsExecutionModel.h>
#include <windows.h>
#undef ERROR

namespace Echo
{
	WindowsExecutionModel::WindowsExecutionModel() : ExecutionModel(Models::COOPERATE)
	{
	}
	
	WindowsExecutionModel::~WindowsExecutionModel()
	{
	}
	
	bool WindowsExecutionModel::SupportsModel(Models::ModelEnum model)
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

	bool WindowsExecutionModel::ProcessEvents(f32 timeOutSuggestion)
	{
		//Handle any window stuff
		MSG msg;
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message!=WM_QUIT)
			{
				//translate any accelerator keys
				TranslateMessage(&msg);
				//send the message to the window proc
				DispatchMessage(&msg);
			}
		}
		return true;
	}

	bool WindowsExecutionModel::EnterSystemEventManager()
	{
		return false;
	}

	bool WindowsExecutionModel::SendUpdateRequest()
	{
		return false;
	}
}
