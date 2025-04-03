#ifndef _ECHO_NETWORKEXECUTIONMODEL_H_
#define _ECHO_NETWORKEXECUTIONMODEL_H_

#include <echo/Network/NetworkEventListener.h>
#include <echo/Kernel/ExecutionModel.h>
#include <boost/bind/arg.hpp>
#include <echo/cpp/functional>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <boost/asio/io_service.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace Echo
{
	class Kernel;

	/**
	 * A NetworkExecutionModel is designed to update a kernel on network events.
	 * While the NetworkExecutionModel is waiting for network events it will either behave in one of two ways:
	 * 
	 * 1. If configured to use command processing (default), events it will be waiting for user input on std::in.
	 * Functions can be registered with the object as named commands to allow the kernel to receive input.
	 * The execution model needs to be set in order for a Kernel to execute. In the case of Application
	 * the execution model can be set before Initialise() is called which then overrides the default
	 * execution model.
	 * 
	 * 2. If configured to not use command processing, the model will sleep the thread until the update frequency
	 * time has passed then the kernel will update a Frame. This is important for allowing connections to
	 * auto reconnect if they are configured to do so. One side effect of this is that Connection auto reconnect
	 * times may be extended to up to just short of twice what is configured. It also means that reconnect times
	 * won't be less than the update frequency.
	 * 
	 * To set up a NetworkExecutionModel:
	 *	- Create a shared pointer pointing to an instance of a NetworkExecutionModel.
	 *  - If you're using Application set the execution model with SetExecutionModel()
	 *  - If you're using Application initialise the network interface if it has not already been done,
	 *		if you're not using Application then create a NetworkManager object and initialise it.
	 *  - Add the NetworkExecutionModel to the NetworkManager's event listener list.
	 * 
	 * Example code:
	 * 
	 *		InitialiseNetworkManager();
	 *		shared_ptr<NetworkExecutionModel> executionModel(new NetworkExecutionModel(*this));
	 *		SetExecutionModel(executionModel);
	 *		GetNetworkManager()->AddNetworkEventListener(executionModel);
	 *		//Initialise application as usual.
	 *		Application::Initialise("YourApplication","resources/YourApplication.config");
	 */
	class NetworkExecutionModel : public ExecutionModel, public NetworkEventListener
	{
	public:
		typedef function<void(const std::string&)> CommandProcessorFunction;
		static boost::arg<1> CommandPlaceholder;
		typedef function<void(const std::string&, const std::string&)> DefaultCommandProcessorFunction;
		static boost::arg<2> ParametersPlaceholder;

		NetworkExecutionModel(Kernel& kernel);
		~NetworkExecutionModel();
		
		void OnNetworkEvent(NetworkEventType eventType) override;
		void Stop();

		bool SupportsModel(Models::ModelEnum model) override;
		bool ProcessEvents(f32) override;
		bool EnterSystemEventManager() override;
		bool SendUpdateRequest() override;

		void SetCommandProcessor(const std::string& command, CommandProcessorFunction func);
		void SetDefaultCommandProcessorFunction(DefaultCommandProcessorFunction func);
		void SetUseCommandProcessing(bool useCommandProcessing);
		
		/**
		 * Set the update frequency regardless of network events.
		 * This method will adjust the provided Kernel's maximum update time to the update frequency.
		 */
		void SetUpdateFrequency(Seconds updateFrequency);
	private:
		void DefaultCommandProcessor();
		void SetupAndStartUpdateTimer();
		void OnUpdateTimer();
		Kernel& mKernel;
		bool mUseCommandProcessing;
		Seconds mUpdateFrequency;
		bool mUpdateFrame;
		std::mutex mUpdateMutex;
		std::condition_variable mUpdateCondition;
		std::thread mServiceThread;
		boost::asio::io_service mIOService;
		shared_ptr<boost::asio::deadline_timer> mUpdateTimer;
		typedef std::map< std::string, CommandProcessorFunction > ProcessorMap;
		ProcessorMap mProcessors;
		DefaultCommandProcessorFunction mDefaultCommandProcessorFunction;
	};
}
#endif
