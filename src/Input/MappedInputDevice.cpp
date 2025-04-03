#include <echo/Input/MappedInputDevice.h>

namespace Echo
{
	MappedInputDevice::MappedInputDevice(const std::string& name, InputManager& inputManager) : InputDevice(name), mInputManager(&inputManager)
	{
	}

	MappedInputDevice::~MappedInputDevice()
	{

	}

	void MappedInputDevice::Vibrate(f32 time, f32 strength)
	{
		std::map< std::string, std::list<shared_ptr<InputDevice> > >::iterator it = mVibratingInputs.begin();
		std::map< std::string, std::list<shared_ptr<InputDevice> > >::iterator itEnd = mVibratingInputs.end();
		while(it != itEnd)
		{
			std::list<shared_ptr<InputDevice> >::iterator dit;
			std::list<shared_ptr<InputDevice> >::iterator ditEnd;
			while(dit != ditEnd)
			{
				(*dit)->Vibrate(time, strength);
				++dit;
			}
			++it;
		}
	}

	bool MappedInputDevice::IsVibrating()
	{
		std::map< std::string, std::list<shared_ptr<InputDevice> > >::iterator it = mVibratingInputs.begin();
		std::map< std::string, std::list<shared_ptr<InputDevice> > >::iterator itEnd = mVibratingInputs.end();
		while(it != itEnd)
		{
			std::list<shared_ptr<InputDevice> >::iterator dit;
			std::list<shared_ptr<InputDevice> >::iterator ditEnd;
			while(dit != ditEnd)
			{
				if((*dit)->IsVibrating())
					return true;
				++dit;
			}
			++it;
		}
		return false;
	}

	void MappedInputDevice::StopVibration()
	{
		std::map< std::string, std::list<shared_ptr<InputDevice> > >::iterator it = mVibratingInputs.begin();
		std::map< std::string, std::list<shared_ptr<InputDevice> > >::iterator itEnd = mVibratingInputs.end();
		while(it != itEnd)
		{
			std::list<shared_ptr<InputDevice> >::iterator dit;
			std::list<shared_ptr<InputDevice> >::iterator ditEnd;
			while(dit != ditEnd)
			{
				(*dit)->StopVibration();
				++dit;
			}
			++it;
		}
	}

	shared_ptr<InputTransform> MappedInputDevice::_CreateAnalogInputTransform(const std::vector<std::string>& inputSourceNames, const std::vector<f32>& analogValues, std::list<shared_ptr<InputDevice> >& outVibratingDevices)
	{
		if(!mInputManager)
		{
			assert(mInputManager && "The Mapped Input Device that you are trying to create input for no longer has an input manager. This happens when the input manager falls out of scope before the device.");
			ECHO_LOG_WARNING("The Mapped Input Device \""<< GetName() << "\" that you are trying to create input for no longer has an input manager. This happens when the input manager falls out of scope before the device.");
			return shared_ptr<InputTransform>();
		}
		shared_ptr< Input<f32> > analogInput = mInputManager->GetInput<f32>(inputSourceNames[0]);
		if(!analogInput)
		{
			shared_ptr< Input<bool> > digitalInput = mInputManager->GetInput<bool>(inputSourceNames[0]);
			if(!digitalInput)
			{
				return shared_ptr<InputTransform>();
			}
			f32 analogValue = 1.0f;
			if(!analogValues.empty())
			{
				analogValue = analogValues[0];
			}

			shared_ptr<InputDevice> device = mInputManager->GetDevice(inputSourceNames[0]);
			if(device->SupportsVibration())
			{
				outVibratingDevices.push_back(device);
			}

			shared_ptr<DigitalToAnalogInputTransform> transform(new DigitalToAnalogInputTransform(*this, analogValue, digitalInput));
			for(u32 i = 1; i < inputSourceNames.size(); ++i)
			{
				if(i < analogValues.size())
				{
					analogValue = analogValues[i];
				}else
				{
					analogValue = 1.0f;
				}
				shared_ptr< Input<bool> > additionalDigitalInput = mInputManager->GetInput<bool>(inputSourceNames[i]);
				if(additionalDigitalInput)
				{
					shared_ptr<InputDevice> device = mInputManager->GetDevice(inputSourceNames[i]);
					if(device->SupportsVibration())
						outVibratingDevices.push_back(device);
					transform->AddSource(additionalDigitalInput, analogValue);
				}
			}
			return transform;
		}
		shared_ptr<InputDevice> device = mInputManager->GetDevice(inputSourceNames[0]);
		if(device && device->SupportsVibration())
		{
			outVibratingDevices.push_back(device);
		}
		shared_ptr<AnalogToAnalogInputTransform> transform(new AnalogToAnalogInputTransform(*this));
		transform->SetSource(analogInput);
		return transform;
	}

	shared_ptr<InputTransform> MappedInputDevice::_CreateDigitalInputTransform(const std::string& inputSourceName, f32& threshold, std::list<shared_ptr<InputDevice> >& outVibratingDevices)
	{
		if(!mInputManager)
		{
			assert(mInputManager && "The Mapped Input Device that you are trying to create an input for no longer has an input manager. This happens when the input manager falls out of scope before the device.");
			ECHO_LOG_WARNING("The Mapped Input Device \""<< GetName() << "\" that you are trying to create input for no longer has an input manager. This happens when the input manager falls out of scope before the device.");
			shared_ptr<InputTransform>();
		}
		shared_ptr< Input<bool> > digitalInput = mInputManager->GetInput<bool>(inputSourceName);
		if(!digitalInput)
		{
			shared_ptr< Input<f32> > analogInput = mInputManager->GetInput<f32>(inputSourceName);
			if(!analogInput)
				return shared_ptr<InputTransform>();

			shared_ptr<InputDevice> device = mInputManager->GetDevice(inputSourceName);
			if(device)
				if(device->SupportsVibration())
					outVibratingDevices.push_back(device);

			shared_ptr<AnalogToDigitalInputTransform> transform(new AnalogToDigitalInputTransform(*this, threshold));
			transform->SetSource(analogInput);
			return transform;
		}
		shared_ptr<InputDevice> device = mInputManager->GetDevice(inputSourceName);
		if(device && device->SupportsVibration())
		{
			outVibratingDevices.push_back(device);
		}

		shared_ptr<DigitalToDigitalInputTransform> transform(new DigitalToDigitalInputTransform(*this));
		transform->SetSource(digitalInput);
		return transform;
	}

	bool MappedInputDevice::ApplyDigitialMapping(const std::string& name, shared_ptr<InputTransform> transform)
	{
		std::map< std::string, shared_ptr<InputMapping> >::iterator it = mInputMappings.find(name);
		if(it != mInputMappings.end())
		{
			if(it->second->GetInputTargetType() != InputTransform::InputSourceType::DIGITAL)
			{
				return false;
			}
			it->second->SetTransform(transform);
			return true;
		} else
		{
			shared_ptr<DigitalInputMapping> digitalMapping(new DigitalInputMapping(transform));
			if(!AddInput<bool>(name, digitalMapping->GetDigitalValuePtr(),false))
			{
				return false;
			}
			mInputMappings[name] = digitalMapping;
			return true;
		}
		return false;
	}

	bool MappedInputDevice::ApplyAnalogMapping(const std::string& name, shared_ptr<InputTransform> transform)
	{
		std::map< std::string, shared_ptr<InputMapping> >::iterator it = mInputMappings.find(name);
		if(it != mInputMappings.end())
		{
			if(it->second->GetInputTargetType() != InputTransform::InputSourceType::ANALOG)
			{
				return false;
			}
			it->second->SetTransform(transform);
			return true;
		} else
		{
			shared_ptr<AnalogInputMapping> analogMapping(new AnalogInputMapping(transform));
			if(!AddInput<f32>(name, analogMapping->GetAnalogValuePtr(),0.f))
			{
				return false;
			}
			mInputMappings[name] = analogMapping;
			return true;
		}
		return false;
	}

	bool MappedInputDevice::CreateDigitalInput(const std::string& inputName, const std::string& inputSourceName, f32 threshold)
	{
		std::list< shared_ptr<InputDevice> >& vibratingDevs = mVibratingInputs[inputName];
		vibratingDevs.clear();
		shared_ptr<InputTransform> transform = _CreateDigitalInputTransform(inputSourceName, threshold, vibratingDevs);
		if(!transform)
		{
			return false;
		}
		if(vibratingDevs.empty())
		{
			mVibratingInputs.erase(inputName);
		}
		return ApplyDigitialMapping(inputName, transform);
	}

	bool MappedInputDevice::CreateAnalogInput(const std::string& inputName, const std::string& inputSourceName, f32 analogValue)
	{
		std::vector<std::string> inputSourceNames;
		inputSourceNames.push_back(inputSourceName);
		std::vector<f32> analogValues;
		analogValues.push_back(analogValue);
		return CreateAnalogInput(inputName, inputSourceNames, analogValues);
	}

	bool MappedInputDevice::CreateAnalogInput(const std::string& inputName, const std::vector<std::string>& inputSourceNames, const std::vector<f32>& analogValues)
	{
		std::list< shared_ptr<InputDevice> >& vibratingDevs = mVibratingInputs[inputName];
		vibratingDevs.clear();
		shared_ptr<InputTransform> transform = _CreateAnalogInputTransform(inputSourceNames, analogValues, vibratingDevs);
		if(!transform)
		{
			return false;
		}
		if(vibratingDevs.empty())
		{
			mVibratingInputs.erase(inputName);
		}
		return ApplyAnalogMapping(inputName, transform);
	}

	bool MappedInputDevice::RemoveInput(const std::string& inputName)
	{
		std::map< std::string, shared_ptr<InputMapping> >::iterator it = mInputMappings.find(inputName);
		if(it != mInputMappings.end())
		{
			std::map< std::string, std::list<shared_ptr<InputDevice> > >::iterator vit = mVibratingInputs.find(inputName);
			if(vit != mVibratingInputs.end())
			{
				mVibratingInputs.erase(vit);
			}
			InputDevice::RemoveInput(inputName);
			mInputMappings.erase(it);
			return true;
		}
		return false;
	}

	bool MappedInputDevice::ChangeDigitalInput(const std::string& inputName, const std::string& newIputSourceName, f32 threshold)
	{
		return CreateDigitalInput(inputName, newIputSourceName, threshold);
	}

	bool MappedInputDevice::ChangeAnalogInput(const std::string& inputName, const std::string& inputSourceName, f32 analogValue)
	{
		return CreateAnalogInput(inputName, inputSourceName, analogValue);
	}

	bool MappedInputDevice::ChangeAnalogInput(const std::string& inputName, const std::vector<std::string>& inputSourceNames, const std::vector<f32>& analogValues)
	{
		return CreateAnalogInput(inputName, inputSourceNames, analogValues);

	}

	shared_ptr<InputMapping> MappedInputDevice::GetInputMapping(const std::string& inputName)
	{
		std::map< std::string, shared_ptr<InputMapping> >::iterator it = mInputMappings.find(inputName);
		if(it != mInputMappings.end())
		{
			return it->second;
		}
		return shared_ptr<InputMapping>();
	}

	void MappedInputDevice::Update(Seconds lastFrameTime)
	{
		InputDevice::Update(lastFrameTime);
		typedef std::pair< const std::string, shared_ptr<MappableInputComponetCollection> > NamedMappableInput;
		BOOST_FOREACH(NamedMappableInput& nmi, mMappableInputs)
		{
			nmi.second->UpdateComponentValues();
		}
		typedef std::pair< const std::string, shared_ptr<InputMapping> > NamedInputMapping;
		BOOST_FOREACH(NamedInputMapping& nim, mInputMappings)
		{
			nim.second->Update();
		}

	}
	void MappedInputDevice::OnTaskManagerRemoved(TaskManager& manager)
	{
		// If the manager is the input manager we need to forget about it because this normally happens
		// when the input manager is deleted.
		if(dynamic_cast<InputManager*>(&manager)==mInputManager)
		{
			mInputManager = nullptr;
		}
	}

}
