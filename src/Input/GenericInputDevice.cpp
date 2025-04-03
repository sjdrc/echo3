#include <echo/Input/GenericInputDevice.h>

namespace Echo
{

	GenericInputDevice::GenericInputDevice(const std::string& deviceName) : InputDevice(deviceName)
	{
	}

	GenericInputDevice::~GenericInputDevice()
	{
	}

	void GenericInputDevice::AddButton(const std::string& buttonName)
	{
		std::map<std::string, shared_ptr<bool> >::iterator it=mButtons.find(buttonName);
		if(it==mButtons.end())
		{
			shared_ptr<bool> input = make_shared<bool>(false);
			mButtons[buttonName]=input;
			AddInput<bool>(buttonName,input,false);
		}else
		{
			ECHO_LOG_ERROR("Button \'" << buttonName << "\' already exists in device " << GetName());
		}
	}

	void GenericInputDevice::AddButton(const std::string& buttonName, shared_ptr<bool> state)
	{
		if(!state)
		{
			ECHO_LOG_ERROR("Input state parameter is null.");
			return;
		}

		std::map<std::string, shared_ptr<bool> >::iterator it=mButtons.find(buttonName);
		if(it==mButtons.end())
		{
			mButtons[buttonName]=state;
			AddInput<bool>(buttonName,state,false);
		}else
		{
			ECHO_LOG_ERROR("Button \'" << buttonName << "\' already exists in device " << GetName());
		}
	}

	void GenericInputDevice::SetButtonState(const std::string& buttonName, bool state)
	{
		std::map<std::string, shared_ptr<bool> >::iterator it=mButtons.find(buttonName);
		if(it!=mButtons.end())
		{
			*it->second=state;
		}
	}

	bool GenericInputDevice::GetButtonState(const std::string& buttonName)
	{
		std::map<std::string, shared_ptr<bool> >::iterator it=mButtons.find(buttonName);
		if(it!=mButtons.end())
		{
			return *it->second;
		}
		return false;
	}

	void GenericInputDevice::AddAxis(const std::string& axisName)
	{
		std::map<std::string, shared_ptr<f32> >::iterator it=mAxis.find(axisName);
		if(it==mAxis.end())
		{
			shared_ptr<f32> input = make_shared<f32>(0.f);
			mAxis[axisName]=input;
			AddInput<f32>(axisName,input, 0.f);
		}else
		{
			ECHO_LOG_ERROR("Axis \'" << axisName << "\' already exists in device " << GetName());
		}
	}

	void GenericInputDevice::AddAxis(const std::string& axisName, shared_ptr<f32> state)
	{
		if(!state)
		{
			ECHO_LOG_ERROR("Input state parameter is null.");
			return;
		}

		std::map<std::string, shared_ptr<f32> >::iterator it=mAxis.find(axisName);
		if(it==mAxis.end())
		{
			mAxis[axisName]=state;
			AddInput<f32>(axisName,state, 0.f);
		}else
		{
			ECHO_LOG_ERROR("Axis \'" << axisName << "\' already exists in device " << GetName());
		}
	}

	void GenericInputDevice::SetAxisState(const std::string& axisName, f32 state)
	{
		std::map<std::string, shared_ptr<f32> >::iterator it=mAxis.find(axisName);
		if(it!=mAxis.end())
		{
			*it->second=state;
		}
	}

	f32 GenericInputDevice::GetAxisState(const std::string& axisName)
	{
		std::map<std::string, shared_ptr<f32> >::iterator it=mAxis.find(axisName);
		if(it!=mAxis.end())
		{
			return *it->second;
		}
		return 0.f;
	}
}
