#include <echo/FileSystem/File.h>
#include <echo/FileSystem/FileSystem.h>
#include <echo/Util/Configuration.h>
#include <echo/Util/Capnp.h>
#include <echo/Input/Input.h>
#include <echo/Input/MappedInputDevice.h>
#include <echo/Schema/MappedInputDevice.capnp.h>

namespace Echo
{
	namespace MappedInputDeviceLoader
	{
		shared_ptr<MappedInputDevice> LoadDevice(File mappingFile, InputManager& inputManager)
		{
			if(!mappingFile.IsOpen())
			{
				ECHO_LOG_ERROR("File not open \"" << mappingFile.GetActualFileName() << "\"");
				return nullptr;
			}
			std::string wholeFile;
			mappingFile.ReadFileIntoString(wholeFile);

			try
			{
				shared_ptr<CapnpMessageHelper> message = CreateCapnpReaderFromData<Resources::MappedInputDevice>(wholeFile);
				if(!message)
				{
					ECHO_LOG_ERROR("Unable to create reader for file " << mappingFile.GetRequestedFileName());
					return nullptr;
				}

				Resources::MappedInputDevice::Reader mappedData = message->GetReader<Resources::MappedInputDevice>();

				if(!mappedData.hasName())
				{
					ECHO_LOG_ERROR("Device needs a name " << mappingFile.GetRequestedFileName());
					return nullptr;
				}
				
				//Set device name
				shared_ptr<MappedInputDevice> targetMappedDevice(new MappedInputDevice(mappedData.getName().cStr(),inputManager));
				u32 deviceNumber = inputManager.InstallDevice(mappedData.getName().cStr(),targetMappedDevice);
				std::stringstream deviceID;
				deviceID << mappedData.getName().cStr() << "(" << deviceNumber << ")";
				
				//Analogue inputs
				if(mappedData.hasAnalogue())
				{
					auto analogue = mappedData.getAnalogue();
					for(Size i = 0; i < analogue.size(); ++i)
					{
						Resources::MappedInputDevice::AnalogueInput::Reader anagloueMapping = analogue[i];
						std::vector<std::string> inputSourceNames;
						std::vector<f32> analogValues;
						if(!anagloueMapping.hasName())
						{
							ECHO_LOG_ERROR("Analogue input mapping needs a name in " << mappingFile.GetRequestedFileName());
							return nullptr;
						}
						if(anagloueMapping.hasSource())
						{
							ReadCapnpTextList(inputSourceNames,anagloueMapping.getSource());
						}
						if(anagloueMapping.hasValue())
						{
							ReadCapnpList(analogValues,anagloueMapping.getValue());
						}
						if(!targetMappedDevice->CreateAnalogInput(anagloueMapping.getName().cStr(),inputSourceNames,analogValues))
						{
							ECHO_LOG_ERROR("Failed to create analogue input mapping " << anagloueMapping.getName().cStr());
							inputManager.UninstallDevice(deviceID.str());
							return nullptr;
						}
					}
				}
				
				//Digital inputs
				if(mappedData.hasDigital())
				{
					auto digital = mappedData.getDigital();
					for(Size i = 0; i < digital.size(); ++i)
					{
						const Echo::Resources::MappedInputDevice::DigitalInput::Reader digitalMapping = digital[i];
						if(!digitalMapping.hasName())
						{
							ECHO_LOG_ERROR("Digital input mapping needs a name in " << mappingFile.GetRequestedFileName());
							return nullptr;
						}
						if(!digitalMapping.hasSource())
						{
							ECHO_LOG_ERROR("Digital input mapping needs a source in " << mappingFile.GetRequestedFileName());
							return nullptr;
						}
						f32 threshold = 0.5f;
						if(digitalMapping.getThreshold()!=0.f)
						{
							threshold=digitalMapping.getThreshold();
						}
						if(!targetMappedDevice->CreateDigitalInput(digitalMapping.getName().cStr(),digitalMapping.getSource().cStr(),threshold))
						{
							ECHO_LOG_ERROR("Failed to create digital input mapping " << digitalMapping.getName().cStr());
							inputManager.UninstallDevice(deviceID.str());
							return nullptr;
						}
					}
				}
				return targetMappedDevice;
			}catch(...)
			{
				ECHO_LOG_ERROR("Failed to load mapped device from " << mappingFile.GetRequestedFileName());
				return nullptr;
			}
		}

		Size LoadMappedDevices(Configuration& configuration, InputManager& inputManager, FileSystem& fileSystem, std::string optionName)
		{
			Size devicesLoaded=0;
			Size numberOfOptions = configuration.GetNumberOfOptionsNamed(optionName);
			for(Size i=0;i<numberOfOptions;++i)
			{
				std::string deviceFile = configuration.GetAtIndex<std::string>(optionName,"",i);
				if(!deviceFile.empty())
				{
					shared_ptr<MappedInputDevice> device=MappedInputDeviceLoader::LoadDevice(fileSystem.Open(deviceFile),inputManager);
					if(device)
					{
						ECHO_LOG_INFO("Loaded mapped input device: " << deviceFile);
						++devicesLoaded;
					}
				}
			}
			return devicesLoaded;
		}
	}
}
