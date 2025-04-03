#include <echo/Input/InputDevice.h>
#include <boost/foreach.hpp>

namespace Echo
{
	InputDevice::InputDevice(const std::string& deviceName) : mName(deviceName)
	{

	}

	InputDevice::~InputDevice()
	{
		ClearAllInputs();
	}

	void InputDevice::ClearAllInputs()
	{
		mInputs.clear();
	}
	
	void InputDevice::Update(Seconds)
	{
		typedef std::pair< const std::string, shared_ptr< InputBase > > NamedInput;
		BOOST_FOREACH(NamedInput& input,mInputs)
		{
			input.second->Update();
		}
		BOOST_FOREACH(NamedInput& input,mInputs)
		{
			input.second->CheckAndFireCallbacks();
		}
	}

	void InputDevice::Vibrate(f32 time, f32 strength){}
	bool InputDevice::IsVibrating(){return false;}
	void InputDevice::StopVibration(){}

	bool InputDevice::RemoveInput(const std::string& inputName)
	{
		std::map< std::string, shared_ptr< InputBase > >::iterator it=mInputs.find(inputName);
		if(it!=mInputs.end())
		{
			mInputs.erase(it);
			return true;
		}
		return false;
	}

	bool InputDevice::HasInput(const std::string& inputName) const
	{
		return (mInputs.find(inputName)!=mInputs.end());
	}

	void InputDevice::GetInputs(std::vector<std::string>& inputsOut) const
	{
		typedef std::pair< const std::string, shared_ptr< InputBase > > NamedInput;
		BOOST_FOREACH(const NamedInput& input, mInputs)
		{
			inputsOut.push_back(input.first);
		}
	}
	
	std::vector<std::string> InputDevice::GetInputs() const
	{
		std::vector<std::string> inputs;
		GetInputs(inputs);
		return inputs;
	}
	
}

