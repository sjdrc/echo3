#ifndef _ECHOINPUT_H_
#define _ECHOINPUT_H_
#include <vector>
#include <map>
#include <echo/Types.h>
#include <echo/Kernel/TaskGroup.h>
#include <echo/Input/InputDevice.h>

namespace Echo
{
	class InputManager : public TaskGroup
	{
	private:
		std::map<std::string, std::vector< shared_ptr<InputDevice> > > mDevices;
	public:
		class InputInfo
		{
		private:
			Size mDeviceIndex;		//If passed (1) device index is 0
			std::string mDeviceName;
			std::string mInputName;
			bool mHasDeviceName;
			bool mHasDeviceIndex;
			bool mHasInputName;
		public:
			InputInfo();
			InputInfo(const std::string& inputString);
			void ConvertInputInfo(const std::string& inputString);
			const std::string& GetDeviceName() const {return mDeviceName;}
			const std::string& GetInputName() const {return mInputName;}
			const Size& GetDeviceIndex() const {return mDeviceIndex;}
			bool HasDeviceName() const {return mHasDeviceName;}
			bool HasInputName() const {return mHasInputName;}
			bool HasDeviceIndex() const {return mHasDeviceIndex;}
		};
		InputManager();
		~InputManager();

		const std::map<std::string, std::vector<shared_ptr<InputDevice> > >& GetDeviceMap() const {return mDevices;}

		void GetDeviceList(std::vector<std::string>& outDevices);
		shared_ptr<InputDevice> GetDevice(const std::string& deviceName);
		void GetAllDevices(std::vector<std::pair<std::string, shared_ptr<InputDevice> > >& outDevices);
		//The format of each entry passed out is the standard input map (see above)
		void GetCompleteInputList(std::vector<std::string>& outAllDeviceInputs);

		/**
		 * Installs an InputDevice into the manager.
		 * Installing a device into a manager makes that device available to things that know about the manager.
		 * If the device inherits from Task then installing the device will also add it to the InputManager for
		 * updating (InputManager is a TaskManager via TaskGroup). It is only added for updating if the task is
		 * not already managed by a task manager. This means if the device needs to update at a time other than
		 * when the InputManager is updated then ensure it is added to the appropriate TaskManager before it is
		 * installed.
		 * @param deviceName
		 * @param device
		 * @return the base 1 device number.
		 */
		Size InstallDevice(const std::string& deviceName, shared_ptr<InputDevice> device);
		
		//Uninstalls an input device. DeviceName[(number])
		bool UninstallDevice(const std::string& deviceName);

		template < typename T >
		bool ResolveInputName(const Input<T>& input, std::string& outName)
		{
			std::map<std::string, std::vector<shared_ptr<InputDevice> > >::iterator it=mDevices.begin();
			std::map<std::string, std::vector<shared_ptr<InputDevice> > >::iterator itEnd=mDevices.end();
			while(it!=itEnd)
			{
				Size numDevices=it->second.size();
				for(Size d=0;d<numDevices;++d)
				{
					std::string inputName;
					if(it->second[d]->ResolveInputName(input,inputName))
					{
						std::stringstream ss;
						ss << it->first << "(" << (d+1) << "):" << inputName;
						outName=ss.str();
						return true;
					}
				}
				++it;
			}
			return false;
		}

		/**
		 * Get an input object.
		 * The standard mapping for inputs takes the following format:
		 *
		 *	DeviceName[(DeviceNumber)]:InputName
		 *
		 * If there is more than one device with the same name (eg
		 * GCNControl) then you can specify which specific device by the
		 * device number. If the device number is not specified the first
		 * device is returned.
		 * 
		 * @param inputName
		 * @return 
		 */
		template< typename T >
		shared_ptr< Input<T> > GetInput(const std::string& inputName)
		{
			InputInfo inputInfo(inputName);
			if(!inputInfo.HasDeviceName() || !inputInfo.HasInputName())
				return shared_ptr< Input<T> >();

			shared_ptr<InputDevice> device=GetDevice(inputName);
			if(!device)
			{
				ECHO_LOG_ERROR("Input not found: " << inputName);
				return shared_ptr< Input<T> >();
			}
			return device->GetInput<T>(inputInfo.GetInputName());
		}

		Size GetNumberOfDevices();
	};
}
#endif
