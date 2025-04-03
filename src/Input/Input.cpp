#include <echo/Input/Input.h>
#include <sstream>
#include <iostream>

namespace Echo
{
	InputManager::InputInfo::InputInfo() :	mDeviceIndex(1),
										mHasDeviceName(false),
										mHasDeviceIndex(false),
										mHasInputName(false)
	{

	}

	InputManager::InputInfo::InputInfo(const std::string& inputString) :
							mDeviceIndex(1),
							mHasDeviceName(false),
							mHasDeviceIndex(false),
							mHasInputName(false)
	{
		ConvertInputInfo(inputString);
	}

	void InputManager::InputInfo::ConvertInputInfo(const std::string& inputString)
	{
		std::string workingCopy=inputString;
		//WorkingCopy will be deviceName(number):inputName (or variant)
		size_t cp=workingCopy.find_first_of(':');
		if(cp!=std::string::npos && cp<workingCopy.length()-1)
		{
			//Left is deviceName(number)
			//Right is inputName
			mHasInputName=true;
			mInputName=workingCopy.substr(cp+1);
			workingCopy=workingCopy.substr(0,cp);
		}

		size_t obp=workingCopy.find_first_of('(');
		mDeviceIndex=0;
		if(obp!=std::string::npos)
		{
			//there could be a device number
			size_t cbp=workingCopy.find_first_of(')');
			if(obp<cbp && cbp!=std::string::npos)
			{
				//Ok lets extract that number
				std::string numString=workingCopy.substr(obp+1,cbp-obp-1);
				mDeviceIndex=atoi(numString.c_str());
				if(mDeviceIndex>0)
				{
					mDeviceIndex--;
					mHasDeviceIndex=true;
					workingCopy=workingCopy.substr(0,obp);
				}
			}
		}
		if(workingCopy.length()>0)
		{
			mDeviceName=workingCopy;
			mHasDeviceName=true;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	InputManager::InputManager() : TaskGroup("InputManager"){}
	InputManager::~InputManager(){}

	void InputManager::GetDeviceList(std::vector<std::string>& outDevices)
	{
		std::map<std::string, std::vector<shared_ptr<InputDevice> > >::iterator it=mDevices.begin();
		std::map<std::string, std::vector<shared_ptr<InputDevice> > >::iterator itEnd=mDevices.end();
		while(it!=itEnd)
		{
			for(Size d=0; d<it->second.size(); ++d)
			{
				std::stringstream ss;
				ss << it->first << "(" << (d+1) << ")";
				std::string deviceName=ss.str();
				outDevices.push_back(deviceName);
			}
			it++;
		}
	}

	shared_ptr<InputDevice> InputManager::GetDevice(const std::string& deviceName)
	{
		InputInfo inputInfo(deviceName);

		if(!inputInfo.HasDeviceName())
		{
			return shared_ptr<InputDevice>();
		}

		std::map<std::string, std::vector<shared_ptr<InputDevice> > >::iterator it=mDevices.find(inputInfo.GetDeviceName());
		if(it==mDevices.end())
		{
			return shared_ptr<InputDevice>();
		}

		if(inputInfo.HasDeviceIndex() && inputInfo.GetDeviceIndex()>=it->second.size())
		{
			return shared_ptr<InputDevice>();
		}
		//DeviceIndex defaults to 0 anyway
		return it->second[inputInfo.GetDeviceIndex()];
	}

	void InputManager::GetAllDevices(std::vector<std::pair<std::string, shared_ptr<InputDevice> > >& outDevices)
	{
		std::map<std::string, std::vector<shared_ptr<InputDevice> > >::iterator it=mDevices.begin();
		std::map<std::string, std::vector<shared_ptr<InputDevice> > >::iterator itEnd=mDevices.end();
		while(it!=itEnd)
		{
			for(Size d=0; d<it->second.size(); ++d)
			{
				std::stringstream ss;
				ss << it->first << "(" << (d+1) << ")";
				std::string deviceName=ss.str();
				outDevices.push_back(std::pair<std::string,shared_ptr<InputDevice> >(deviceName, it->second[d]));
			}
			it++;
		}
	}

	void InputManager::GetCompleteInputList(std::vector<std::string>& outAllDeviceInputs)
	{
		std::map<std::string, std::vector<shared_ptr<InputDevice> > >::iterator it=mDevices.begin();
		std::map<std::string, std::vector<shared_ptr<InputDevice> > >::iterator itEnd=mDevices.end();
		while(it!=itEnd)
		{
			for(Size d=0; d<it->second.size(); ++d)
			{
				std::stringstream ss;
				ss << it->first << "(" << (d+1) << ")";
				std::vector<std::string> inputs;
				it->second[d]->GetInputs(inputs);
				for(Size i=0;i<inputs.size();++i)
				{
					std::string inputName=ss.str();
					inputName+=":";
					inputName+=inputs[i];
					outAllDeviceInputs.push_back(inputName);
				}
				inputs.clear();
			}
			it++;
		}
	}

	Size InputManager::InstallDevice(const std::string& deviceName, shared_ptr<InputDevice> device)
	{
		//If the device is a Task then add it to the task manager for updating.		
		Task* deviceTask = dynamic_cast<Task*>(device.get());
		if(deviceTask && deviceTask->GetNumberOfTaskManagers()==0)
		{
			//Using the raw pointer is fine because Task cleans up with managers properly.
			AddTask(deviceTask);
		}
		
		std::map<std::string, std::vector<shared_ptr<InputDevice> > >::iterator it=mDevices.find(deviceName);
		if(it!=mDevices.end())
		{
			it->second.push_back(device);
			return it->second.size();
		}
		mDevices[deviceName].push_back(device);
		return 1;
	}

	//Uninstalls an input device. DeviceName[(number])
	bool InputManager::UninstallDevice(const std::string& deviceName)
	{
		InputInfo inputInfo;
		if(!inputInfo.HasDeviceName())
			return false;

		std::map<std::string, std::vector<shared_ptr<InputDevice> > >::iterator it=mDevices.find(inputInfo.GetDeviceName());
		if(it==mDevices.end())
			return false;

		if(inputInfo.HasDeviceIndex())
			if(inputInfo.GetDeviceIndex()>=it->second.size())
				return false;

		//move all devices down in list
		if(it->second.size()>1)
		{
			for(Size d=inputInfo.GetDeviceIndex();d<it->second.size()-1;++d)
			{
				it->second[d]=it->second[d+1];
			}
			it->second.pop_back();
		}else
		{
			//remove the device entry
			mDevices.erase(it);
		}
		return true;
	}

	Size InputManager::GetNumberOfDevices()
	{
		std::map<std::string, std::vector<shared_ptr<InputDevice> > >::iterator it=mDevices.begin();
		std::map<std::string, std::vector<shared_ptr<InputDevice> > >::iterator itEnd=mDevices.end();
		Size ds=0;
		while(it!=itEnd)
		{
			std::vector<shared_ptr<InputDevice> >& deviceSet=it->second;
			ds+=deviceSet.size();
			it++;
		}
		return ds;
	}
}
