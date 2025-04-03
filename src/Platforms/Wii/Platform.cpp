#include <echo/Platform.h>
#include <echo/FileSystem/FileSystem.h>
#include <echo/FileSystem/FileSystemSourceFile.h>
#include <echo/FileSystem/FileSystemSourceMemory.h>
#include <echo/FileSystem/FileSystemSourceVFS.h>
#include <echo/Util/FunctionBinder.h>
#include <echo/Input/Input.h>
#include <echo/Input/MappedInputDevice.h>

#include <boost/make_shared.hpp>

#include <echo/Platforms/Wii/WiiExecutionModel.h>
#include <echo/Platforms/Wii/GXRenderTarget.h>
#include <echo/Platforms/Wii/WiiSystemInput.h>
#include <echo/Platforms/Wii/Wiimote.h>
#include <echo/Platforms/Wii/SDFileSystemSource.h>
#include <echo/Platforms/Wii/USBFileSystemSource.h>

//This is a bit experimental
extern const Echo::u8 _binary_echo_vfs_start[];
extern const Echo::u8 _binary_echo_vfs_end[];

namespace Echo
{
	namespace Platform
	{
		std::string GetName()
		{
			return "Wii";
		}

		shared_ptr<ExecutionModel> CreateExecutionModel()
		{
			return shared_ptr<ExecutionModel>(new WiiExecutionModel());
		}
		
		shared_ptr<RenderTarget> CreateRenderTarget(const std::string& type, const std::string& name, Kernel& kernel, u32 width, u32 height, u8 bpp, bool fullScreen)
		{
			if(type=="Window")
			{
				//TODO: Make sure this is all working when GXRenderTarget is complete.
				shared_ptr<GXRenderTarget> renderTarget(new GXRenderTarget(kernel));
				renderTarget->SetSize(width,height);
				renderTarget->SetFullScreen(true);
				renderTarget->SetName(name);
				return renderTarget;
			}
			return shared_ptr<RenderTarget>();
		}
		
		shared_ptr<FileSystem> CreateDefaultFileSystem(const std::string& applicationName)
		{
			shared_ptr<FileSystem> fileSystem = make_shared<FileSystem>();
			shared_ptr<FileSystemSource> persistentSource;
			
			//Find a default source - SD card is first, then USB device.
			bool isDefaultSource=true;
			// shared_ptr<SDFileSystemSource> sdSource(new SDFileSystemSource("sd"));
			// if(sdSource->GetMounted())
			// {
			// 	sdSource->SetCurrentDirectory(applicationName);
			// 	if(fileSystem->InstallSource(sdSource,isDefaultSource))
			// 	{
			// 		isDefaultSource=false;
			// 		persistentSource = shared_ptr<SDFileSystemSource>(new SDFileSystemSource("sd"));
					
			// 	}
				
			// }
			// shared_ptr<USBFileSystemSource> usbSource(new USBFileSystemSource("usb"));
			// if(usbSource->GetMounted())
			// {
			// 	usbSource->SetCurrentDirectory(applicationName);
			// 	if(fileSystem->InstallSource(usbSource,isDefaultSource))
			// 	{
			// 		isDefaultSource=false;
			// 	}
			// 	if(!persistentSource)
			// 	{
			// 		persistentSource = shared_ptr<USBFileSystemSource>(new USBFileSystemSource("usb"));
			// 	}
			// }

			shared_ptr<FileSystemSourceMemory> memorySource(new FileSystemSourceMemory());
			fileSystem->InstallSource(memorySource,false);

			Size echoVFSSize = Size(_binary_echo_vfs_end) - Size(_binary_echo_vfs_start);
			if(echoVFSSize > 0)
			{
				shared_ptr<FileSystemSourceVFS> vfsSource(new FileSystemSourceVFS(*fileSystem));
				ECHO_LOG_INFO("VFS Start: " << Size(_binary_echo_vfs_start));
				ECHO_LOG_INFO("VFS End: " << Size(_binary_echo_vfs_start) + Size(_binary_echo_vfs_end));
				ECHO_LOG_INFO("VFS Size: " << echoVFSSize <<  " (" << ((echoVFSSize/1024.f)/1024.f) << " MiB)");
				ECHO_LOG_INFO("VFS Memory name is: " << FileSystemSourceMemory::MakeMemoryFileName(_binary_echo_vfs_start,echoVFSSize));
				ECHO_LOG_INFO("VFS Memory name is: " << FileSystemSourceMemory::MakeMemoryFileName(_binary_echo_vfs_start,echoVFSSize));
				ECHO_LOG_INFO("VFS Memory should be: " << Size(_binary_echo_vfs_start) << ":" << echoVFSSize);
				std::stringstream addressNotCooked; addressNotCooked << Size(_binary_echo_vfs_start) << ":" << echoVFSSize;
				ECHO_LOG_INFO("VFS Memory addressNotCooked: " << addressNotCooked.str());
				if(vfsSource->LoadVFS("memory://"+addressNotCooked.str()))//FileSystemSourceMemory::MakeMemoryFileName(_binary_echo_vfs_start,echoVFSSize)))
				{
					if(fileSystem->InstallSource(vfsSource,isDefaultSource))
					{
						isDefaultSource=false;
					}
				}else
				{
					ECHO_LOG_ERROR("Failed to load VFS from binary");
				}
			}
			
			//Create a persistent data storage location. This should be used for save files etc.
			if(persistentSource)
			{
				std::string persistentDirectory = applicationName + "/persistent";
				persistentSource->SetCurrentDirectory(persistentDirectory);
				fileSystem->InstallSource("persistent", persistentSource, false);
			}
			return fileSystem;
		}
		
		shared_ptr<Audio> CreateDefaultAudioSystem()
		{
			return shared_ptr<Audio>();
		}

		shared_ptr<InputManager> CreateDefaultInputManager(shared_ptr<RenderTarget> target)
		{
			Kernel* kernel=0;
			GXRenderTarget* renderTargetGX=dynamic_cast<GXRenderTarget*>(target.get());
			if(renderTargetGX)
			{
				kernel = &renderTargetGX->GetKernel();
			}

			shared_ptr<InputManager> inputManager = make_shared<InputManager>();
			
			shared_ptr<WiiSystemInput> systemInput(new WiiSystemInput(kernel));
			inputManager->InstallDevice("System",systemInput);

			const size_t NUMBER_OF_WIIMOTES = 4;
			for(size_t i=0; i<NUMBER_OF_WIIMOTES;++i)
			{
				shared_ptr<Wiimote> wiimote(new Wiimote(i));
				inputManager->InstallDevice("Wiimote",wiimote);
			}

			shared_ptr<MappedInputDevice> mappedMouseInput(new MappedInputDevice("Mouse",*inputManager));
			mappedMouseInput->CreateDigitalInput("Left","Wiimote:A");
			mappedMouseInput->CreateDigitalInput("Right","Wiimote:B");
			mappedMouseInput->CreateAnalogInput("X","Wiimote:X");
			mappedMouseInput->CreateAnalogInput("Y","Wiimote:Y");
			inputManager->InstallDevice("Mouse",mappedMouseInput);
			
			return inputManager;
		}
		
		shared_ptr<Shell> CreateShell()
		{
			return shared_ptr<Shell>();
		}
		
		void RegisterRenderTargets(FunctionBinder& functionBinder, Kernel& kernel)
		{
		}

		void RegisterAudioSystems(FunctionBinder& functionBinder)
		{
		}
	}
}
