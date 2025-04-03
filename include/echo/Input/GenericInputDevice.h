#ifndef _ECHOGENERICINPUTDEVICE_H_
#define _ECHOGENERICINPUTDEVICE_H_
#include <echo/Input/InputDevice.h>
#include <typeinfo>

namespace Echo
{
	/**
	 * GenericInputDevice is essentially a storage device.
	 * Devices that do not require more than a simple state change from an external API
	 * (through a callback for example) can be installed using a GenericInputDevice then
	 * the device instance is simply updated using the Set*State() methods.
	 */
	class GenericInputDevice : public InputDevice
	{
	private:
		std::map<std::string, shared_ptr<bool> > mButtons;
		std::map<std::string, shared_ptr<f32> > mAxis;
	public:
		GenericInputDevice(const std::string& deviceName);
		~GenericInputDevice();
		
		void AddButton(const std::string& buttonName);

		void AddButton(const std::string& buttonName, shared_ptr<bool> state);

		
		void SetButtonState(const std::string& buttonName, bool state);


		bool GetButtonState(const std::string& buttonName);


		void AddAxis(const std::string& axisName);

		void AddAxis(const std::string& axisName, shared_ptr<f32> state);
		void SetAxisState(const std::string& axisName, f32 state);
		
		f32 GetAxisState(const std::string& axisName);
	};
	
	template<typename T>
	class OmniInputDevice : public InputDevice
	{
	public:
		OmniInputDevice(const std::string& deviceName, const std::string& inputName, T neutral) : InputDevice(deviceName)
		{
			mInput = make_shared<T>();
			AddInput<T>(inputName, mInput, neutral);
		}
		~OmniInputDevice(){}
		
		void Set(T v)
		{
			*mInput = v;
		}
	private:
		shared_ptr<T> mInput;
	};
}
#endif
