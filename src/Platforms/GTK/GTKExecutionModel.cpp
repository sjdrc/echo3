#include <echo/Platforms/GTK/GTKExecutionModel.h>
#include <gtkmm/main.h>
#include <gtkmm/window.h>
#include <gtkglmm.h>

namespace Echo
{
	GTKExecutionModel::GTKExecutionModel(shared_ptr<Gtk::Main> gtkMain) : ExecutionModel(Models::COOPERATE), mGTKMain(gtkMain)
	{
	}
	
	GTKExecutionModel::~GTKExecutionModel()
	{
	}

	bool GTKExecutionModel::SupportsModel(Models::ModelEnum model)
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

	bool GTKExecutionModel::ProcessEvents(f32 timeOutSuggestion)
	{
		while(mGTKMain->events_pending())
		{
			mGTKMain->iteration(false);
		}
		return true;
	}

	bool GTKExecutionModel::EnterSystemEventManager()
	{
		return false;
	}

	bool GTKExecutionModel::SendUpdateRequest()
	{
		return false;
	}
}
