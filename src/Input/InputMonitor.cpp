#include <echo/Input/InputMonitor.h>
#include <echo/Input/InputMonitorListener.h>
#include <echo/Input/InputDevice.h>
#include <algorithm>
namespace Echo
{
	InputMonitor::InputMonitor(shared_ptr<InputManager> inputManager) : mInputManager(inputManager)
	{
	}

	InputMonitor::~InputMonitor()
	{
		while(!mNextInputListeners.empty())
		{
			RemoveListener(mNextInputListeners.back());
		}
	}

	void InputMonitor::_NotifyDigitalInput(const std::string& deviceName, const std::string& inputName, shared_ptr< Input<bool> > input )
	{
		auto it=mNextInputListeners.begin();
		auto itEnd=mNextInputListeners.end();
		while(it!=itEnd)
		{
			(*it)->OnDigitalInput(deviceName,inputName,input);
			++it;
		}
	}

	void InputMonitor::_NotifyAnalogInput(const std::string& deviceName, const std::string& inputName, shared_ptr< Input<f32> > input )
	{
		auto it=mNextInputListeners.begin();
		auto itEnd=mNextInputListeners.end();
		while(it!=itEnd)
		{
			(*it)->OnAnalogInput(deviceName,inputName,input);
			++it;
		}
	}


	void InputMonitor::AddDeviceToIgnoreList(const std::string& deviceName)
	{
		mDevicesToIgnore.insert(deviceName);
	}

	void InputMonitor::AddListener(shared_ptr<InputMonitorListener> listener)
	{
		if(listener)
		{
			mNextInputListeners.push_back(listener);
			listener->_SetMonitor(dynamic_pointer_cast<InputMonitor>(shared_from_this()));
		}
	}

	void InputMonitor::RemoveListener(shared_ptr<InputMonitorListener> listener)
	{
		if(listener)
		{
			mNextInputListeners.remove(listener);
			listener->_SetMonitor(nullptr);
		}
	}

	void InputMonitor::Update(Seconds lastFrameTime)
	{
		const std::map<std::string, std::vector< shared_ptr<InputDevice> > >& devices=mInputManager->GetDeviceMap();

		std::map<std::string, std::vector< shared_ptr<InputDevice> > >::const_iterator it=devices.begin();
		std::map<std::string, std::vector< shared_ptr<InputDevice> > >::const_iterator itEnd=devices.end();
		while(it!=itEnd)
		{
			if(mDevicesToIgnore.find(it->first) != mDevicesToIgnore.end())
			{
				++it;
				continue;
			}

			Size numDevices=it->second.size();
			for(Size d=0;d<numDevices;++d)
			{
				const std::map< std::string, shared_ptr< InputBase > >& inputs=it->second[d]->GetAllInputs();
				std::map< std::string, shared_ptr< InputBase > >::const_iterator iit=inputs.begin();
				std::map< std::string, shared_ptr< InputBase > >::const_iterator iitEnd=inputs.end();
				while(iit!=iitEnd)
				{
					auto digital = dynamic_pointer_cast< Input<bool> >(iit->second);
					if(digital && *digital->mData)
					{
						_NotifyDigitalInput(it->first, iit->first, digital);
						++iit;
						continue;
					}
					auto analog = dynamic_pointer_cast< Input<f32> >(iit->second);
					if(analog && *analog->mData!=0.f)
					{
						_NotifyAnalogInput(it->first, iit->first, analog);
						++iit;
						continue;
					}
					++iit;
				}
			}
			++it;
		}
	}
}
