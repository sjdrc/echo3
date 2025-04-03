#ifndef _ECHO_GTKEXECUTIONMODEL_H_
#define _ECHO_GTKEXECUTIONMODEL_H_

#include <echo/Kernel/ExecutionModel.h>
#include <gtkmm/main.h>

namespace Echo
{
	class GTKExecutionModel : public ExecutionModel
	{
	public:
		GTKExecutionModel(shared_ptr<Gtk::Main> gtkMain);
		virtual ~GTKExecutionModel();

		bool SupportsModel(Model model);

		//Models::COOPERATE
		// If the platform supports event processing in a "Peek, process and proceed" way
		// it should be implemented within this function.
		//
		// "Peek, process and proceed" means that Echo relies on this function to CHECK
		// if there are events to process and doing so them before proceeding. If there
		// are no events to process then the function should return.
		//
		// This function should return as soon as possible.
		//
		// The timeOutSuggestion parameter is used as a suggestion for how long to wait
		// for, or process events before returning. This parameter is not strict and can
		// be ignored if the platform does not support processing time outs.
		//
		// - Echo calls this function AFTER every frame. That is an update and render.
		// - No manual calls to update Echo are required.
		//
		// This function should return true if everything went to plan and return false
		// if the COOPERATE model is unavailable.
		bool ProcessEvents(f32 timeOutSuggestion);

		//Models::CONTROLLER
		// If the platform supports a controlled event model then it should be implemented
		// in the following functions.
		//
		// Typically if a platform implements this model there will be additional functions
		// for the handling of messages. It may be a single function to receive events or
		// numerous functions accessed through registered functors. However events are
		// handled Echo expects to be updated and rendered manually by calls to
		// gEchoKernel().ProcessFrame()
		//
		// This function should return true if everything went to plan and return false
		// if the Model::CONTROLLER model is unavailable.
		//
		// IMPORTANT: Because some systems may not be able to receive events before the
		// main event loop it is important that you process any initialisation events
		// and make a call to SendUpdateRequest() (see below) in order to trigger the
		// first update. You DO NOT need to continue calling SendUpdateRequest() when
		// an update event has been received. Echo will do this at the appropriate time
		// before returning from gEchoKernel().ProcessFrame() (see note below about
		// sending event tricks).
		//
		bool EnterSystemEventManager();

		//At the end of Kernel::UpdateFrame() Echo will call SendUpdateRequest() to
		//request to be updated again.
		//
		// This function should return true if the event was successfully sent or false
		// if Echo should call again. Return false if the CONTROLLER model is not available.
		// Echo will not call this if a call to SupportsModel(Models::CONTROLLER) returns
		// false.
		//
		// This function SHOULD NOT block.
		//
		// NOTE: If your system does not support the sending of events in a non-blocking
		// way (a way to post a message to a queue to be processed later) then you can
		// perform a little trick to manage the sending of the update request in your own
		// way by making SendUpdateRequest() do nothing but returning true. Then after you
		// process the update event (and call Kernel::UpdateFrame()) you can handle
		// the sending of the event in your own way.
		bool SendUpdateRequest();
	private:
		shared_ptr<Gtk::Main> mGTKMain;
	};
}

#endif 
