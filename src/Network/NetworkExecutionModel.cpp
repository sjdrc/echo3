#include <echo/Network/NetworkManager.h>
#include <echo/Network/NetworkExecutionModel.h>
#include <echo/Kernel/Kernel.h>
#include <echo/Kernel/Thread.h>
#include <echo/cpp/functional>
#include <condition_variable>
#include <thread>
#include <boost/asio/placeholders.hpp>

namespace Echo
{
	NetworkExecutionModel::NetworkExecutionModel(Kernel& kernel) : ExecutionModel(ExecutionModel::Models::CONTROLLER),
		mKernel(kernel),
		mUseCommandProcessing(false),
		mUpdateFrequency(5),
		mUpdateFrame(false)
	{
		SetDefaultCommandProcessorFunction(bind(&NetworkExecutionModel::DefaultCommandProcessor,this));
	}

	NetworkExecutionModel::~NetworkExecutionModel()
	{
	}
	
	void NetworkExecutionModel::SetUseCommandProcessing(bool useCommandProcessing)
	{
		mUseCommandProcessing = useCommandProcessing;
	}

	void NetworkExecutionModel::SetUpdateFrequency(Seconds updateFrequency)
	{
		mUpdateFrequency = updateFrequency;
		mKernel.GetFrameRateLimiter().SetMaxFrameTime(updateFrequency);
	}

	void NetworkExecutionModel::OnNetworkEvent(NetworkEventType eventType)
	{
		mUpdateFrame = true;
		mUpdateCondition.notify_all();
	}
	
	void NetworkExecutionModel::Stop()
	{
		// This removes all tasks
		mKernel.Stop();
		// Trigger a continue
		mUpdateFrame = true;
		mUpdateCondition.notify_all();
	}


	bool NetworkExecutionModel::SupportsModel(Models::ModelEnum model)
	{
		switch (model)
		{
			case Models::CONTROLLER:
				return true;
			case Models::EXTERNAL_CONTROLLER:
			case Models::COOPERATE:
			case Models::NONE:
				return false;
		}
		return false;
	}

	bool NetworkExecutionModel::ProcessEvents(f32)
	{
		return false;
	}

	bool NetworkExecutionModel::EnterSystemEventManager()
	{
		if(mUseCommandProcessing)
		{
			std::string command;
			std::cout << "Type 'quit' to quit" << std::endl;
			while(command!="quit")
			{
				std::getline(std::cin,command);
				if(!std::cin.good())
				{
					// The process might be launched with a detached stdin which means cin will be bad.
					// If that is the case we'll sleep and check back every 10 seconds (this is just arbitrary).
					Thread::Sleep(Seconds(10));
				}
				size_t space = command.find_first_of(' ');
				std::string parameters;
				if(space!=std::string::npos)
				{
					parameters = command.substr(space+1);
					command = command.substr(0,space);
				}
				ProcessorMap::iterator it = mProcessors.find(command);
				if(it!=mProcessors.end())
				{
					it->second(parameters);
				}else
				{
					mDefaultCommandProcessorFunction(command,parameters);
				}
			}
		}else
		{
			SetupAndStartUpdateTimer();
			mServiceThread=std::thread([this]{
				mIOService.run();
			});
			
			bool kernelActive =  true;
			while(kernelActive)
			{
				std::unique_lock<std::mutex> lock(mUpdateMutex);
				mUpdateCondition.wait(lock, [this](){return mUpdateFrame;});
				mUpdateFrame = false;
				kernelActive = mKernel.ProcessFrame();
			}
			
			mIOService.stop();
			mServiceThread.join();
		}
		return true;
	}

	bool NetworkExecutionModel::SendUpdateRequest()
	{
		return true;
	}

	void NetworkExecutionModel::SetCommandProcessor(const std::string& command, CommandProcessorFunction func)
	{
		mProcessors[command] = func;
	}

	void NetworkExecutionModel::SetDefaultCommandProcessorFunction(DefaultCommandProcessorFunction func)
	{
		mDefaultCommandProcessorFunction = func;
	}

	void NetworkExecutionModel::DefaultCommandProcessor()
	{
	}
	
	void NetworkExecutionModel::SetupAndStartUpdateTimer()
	{
		size_t ms = mUpdateFrequency.count() * 1000;
		mUpdateTimer.reset(new boost::asio::deadline_timer(mIOService, boost::posix_time::milliseconds(ms)));
		mUpdateTimer->async_wait(bind(&NetworkExecutionModel::OnUpdateTimer,this));
	}
	
	void NetworkExecutionModel::OnUpdateTimer()
	{
		if(mKernel.HasAtLeastOneTask())
		{
			OnNetworkEvent(NetworkEventType::UNKNOWN);
			size_t ms = mUpdateFrequency.count() * 1000;
			mUpdateTimer->expires_at(mUpdateTimer->expires_at() + boost::posix_time::milliseconds(ms));
			mUpdateTimer->async_wait(bind(&NetworkExecutionModel::OnUpdateTimer,this));
		}
	}

}
