#ifndef _ECHO_EXECUTIONMODEL_H_
#define _ECHO_EXECUTIONMODEL_H_

#include <echo/Types.h>

namespace Echo
{
	/**
	 * ExecutionModel interface.
	 * 
	 * This applies to platform implementors and does not concern most users.
	 *
	 * Echo supports a number of different event models that affect the way
	 * Echo updates. Kernel objects will use an ExecutionModel object upon a call to
	 * Execute() to determine how to manage frame updates.
	 *
	 * The main event models are NONE, CONTROLLER and COOPERATE.
	 *
	 * NONE is fairly obvious, there is no system event model and you can be
	 * sure the OS wont hold your Echo application up. Echo will run in real
	 * time and provide constant updates. You can think of this as loop that
	 * looks like.
	 *
	 * while(executing)
	 * {
	 *		Update();
	 * }
	 *
	 * CONTROLLER and COOPERATE are a little more complex.
	 *
	 * These two event models appear to be very similar and at the end of the day
	 * the application coder does not need to know which is being used.
	 * The fundamental difference between the event models lies in when Echo
	 * actually performs a frame update. The below graphs illustrate this.
	 * 
	 *  CONTROLLER
	 * 	Execute Kernel
	 * 		- Enter OS event management 
	 * 			- Receive update event
	 * 				- Echo Updates
	 * 				- post update event
	 * 				- return to event management
	 * 
	 *  COOPERATE
	 * 	Execute Kernel
	 * 	/->	- Echo Updates
	 * 	*	- Allow OS to process messages
	 * 	\---/
	 *
	 * Which is right for your platform?
	 * ---------------------------------
	 * At the end of the day that is dependent on the platform and ultimately
	 * how you want to implement support for it. Neither one is better than the
	 * other.
	 *
	 * At least one execution model class needs to be available for each platform
	 * and the Platform namespace should return a model valid for the other platform
	 * defaults. Follow the function documentation and implement accordingly.
	 *
	 * If your platform does not support one or more of the models simply implement
	 * empty functions that return false for each of the unsupported models.
	 */
	class ExecutionModel
	{
	public:
		struct Models
		{
			enum ModelEnum
			{
				NONE = 0,		//The OS has no event model
				CONTROLLER = 1,	//The OS maintains control over the update process.
								//Echo will send itself an update event at the end of
								//each update then will give control to the OS.
				COOPERATE = 2,	//Echo will cooperate with the event model by giving a
								//chance to process messages periodically (usually every
								//frame)
				EXTERNAL_CONTROLLER = 4//Like CONTROLLER, except the OS requires the "main"
								// invocation to return in order to get control back. The OS/runtime
								// environment must manage the updates in another way. This runtime
								// model is rarely used, it may be used on platforms where a wrapper
								// is used to run C++ code. For example, on Android we use a wrapper
								// to run Echo and the wrapper will call updates as required.
			};
		};
		typedef Models::ModelEnum Model;
		
		ExecutionModel(Model defaultModel);
		virtual ~ExecutionModel();
		
		bool SetModelToUse(Model model);
		
		Model GetCurrentModel() const
		{
			return mCurrentModel;
		}

		/**
		 * Returns true if the system supports the event model specified.
		 * Kernel objects use this function to determine whether or not it can run using
		 * the specified model. A platform can support multiple models.
		 * Echo allows the user to hint which model to use, however if the hinted model
		 * is not available the default will be used.
		 * @param model that we wish to test support for.
		 * @return true if the model is supported.
		 */
		virtual bool SupportsModel(Model model) = 0;

		/**
		 * Models::COOPERATE
		 * If the platform supports event processing in a "Peek, process and proceed" way
		 * it should be implemented within this function.
		 *
		 * "Peek, process and proceed" means that Echo relies on this function to CHECK
		 * if there are events to process and doing so them before proceeding. If there
		 * are no events to process then the function should return.
		 *
		 * This function should return as soon as possible.
		 *
		 * The timeOutSuggestion parameter is used as a suggestion for how long to wait
		 * for, or process events before returning. This parameter is not strict and can
		 * be ignored if the platform does not support processing time outs.
		 *
		 * - Kernels call this function AFTER every frame.
		 * - No manual calls to update kernels are required.
		 *
		 * This function should return true if everything went to plan and return false
		 * if the COOPERATE model is unavailable.
		*/
		virtual bool ProcessEvents(f32 timeOutSuggestion) = 0;

		/**
		 *Models::CONTROLLER
		 * If the platform supports a controller event model then it should be implemented
		 * in the following functions.
		 *
		 * Typically if a platform implements this model there will be additional functions
		 * for the handling of messages. It may be a single function to receive events or
		 * numerous functions accessed through registered functors. However events are
		 * handled, Echo requires that kernel objects are to be updated manually by calls to
		 * Kernel::ProcessFrame() for each kernel object in use.
		 *
		 * This function should return true if everything went to plan and return false
		 * if the Model::CONTROLLER model is unavailable.
		 *
		 * IMPORTANT: Because some systems may not be able to receive events before the
		 * main event loop it is important that you process any initialisation events
		 * and make a call to SendUpdateRequest() (see below) in order to trigger the
		 * first update. You DO NOT need to continue calling SendUpdateRequest() when
		 * an update event has been received. Echo will do this at the appropriate time
		 * before returning from gEchoKernel().ProcessFrame() (see note below about
		 * sending event tricks).
		 */
		virtual bool EnterSystemEventManager() = 0;

		/**
		 * At the end of Kernel::ProcessFrame() the Kernel will call SendUpdateRequest() to
		 * request to be updated again.
		 *
		 * This function should return true if the event was successfully sent or false
		 * if Echo should call again. Return false if the CONTROLLER model is not available.
		 * Echo will not call this if a call to SupportsModel(Models::CONTROLLER) returns
		 * false.
		 *
		 * This function SHOULD NOT block.
		 *
		 * NOTE: If your system does not support the sending of events in a non-blocking
		 * way (a way to post a message to a queue to be processed later) then you can
		 * perform a little trick to manage the sending of the update request in your own
		 * way by making SendUpdateRequest() do nothing but returning true. Then after you
		 * process the update event (and call Kernel::UpdateFrame()) you can handle
		 * the sending of the event in your own way.
		 */
		virtual bool SendUpdateRequest() = 0;
	private:
		Model mCurrentModel;
	};
}

#endif 
