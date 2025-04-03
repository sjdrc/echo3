#ifndef _ECHOMAPPEDINPUTDEVICELOADER_H_
#define _ECHOMAPPEDINPUTDEVICELOADER_H_

#include <echo/FileSystem/File.h>

namespace Echo
{
	class MappedInputDevice;
	class Configuration;
	class InputManager;
	class FileSystem;
	namespace MappedInputDeviceLoader
	{
		/**
		 * Create a device and load the input mappings from the specified input file.
		 * The mappings input file needs to specify devices/inputs in the passed in inputManager.
		 * The input manager will be the source of inputs referenced in the file as well as be used to install the returned device.
		 * The mappings can create mappings to inputs within the device as long as the source inputs are defined before they are used.
		 * @see proto/MappedInputDevice.proto for the file format.
		 * @param mappingFile An open .input file.
		 * @param inputManager The input manager with the inputs that the mapping file will reference and the device will be installed in.
		 * @return If no errors occurred a MappedInputDevice, otherwise a null pointer.
		 */
		shared_ptr<MappedInputDevice> LoadDevice(File mappingFile, InputManager& inputManager);

		/**
		 * Load mapped indevice files specified in a configuration object.
		 * @param configuration The configuration object that has the list of optionName options that you want to load.
		 * @param inputManager The InputManager to load the devices into.
		 * @param fileSystem File system to access the files pointed to by optionName.
		 * @param optionName The option name to indicate a mapped input device, usually you don't need to change this.
		 */
		size_t LoadMappedDevices(Configuration& configuration, InputManager& inputManager, FileSystem& fileSystem, std::string optionName = "mapped.input.device");
	}
}
#endif 
