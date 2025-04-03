#include <echo/Platform.h>
#include <echo/FileSystem/FileSystem.h>
#include <echo/FileSystem/FileSystemSourceFile.h>
#include <echo/FileSystem/FileSystemSourceMemory.h>
#include <echo/FileSystem/FileSystemSourceEncrypted.h>
#include <echo/Graphics/RenderTargetNotifier.h>
#include <echo/Platforms/Android/AndroidSupport.h>
#include <echo/Platforms/Android/FileSystemSourceAPK.h>
#include <echo/Platforms/Android/AndroidWindow.h>
#include <echo/Platforms/Android/AndroidExecutionModel.h>
#include <echo/Platforms/OpenSLES/OpenSLESAudio.h>
#include <echo/Platforms/AL/OpenALAudio.h>
#include <echo/Input/PseudoMouse.h>
#include <echo/Input/PseudoKeyboard.h>
#include <echo/Input/Input.h>
#include <echo/Util/FunctionBinder.h>
#include <echo/cpp/functional>
#include <boost/foreach.hpp>

namespace Echo
{
	AndroidSupport* androidSupport = 0;
		
	namespace Platform
	{
		std::string GetName()
		{
			return "Android";
		}
		
		shared_ptr<ExecutionModel> CreateExecutionModel()
		{
			return make_shared<AndroidExecutionModel>();
		}
		
		shared_ptr<RenderTarget> CreateRenderTarget(const std::string& type, const std::string& name, Kernel& kernel, u32 width, u32 height, u8 bpp, bool fullScreen)
		{
			if(type=="Window")
			{
				RenderTargetNotifier* notifier = androidSupport->GetRenderTargetNotifier();
				//AndroidWindow sizes are determined by Android.
				width = androidSupport->GetScreenWidth();
				height = androidSupport->GetScreenHeight();
				shared_ptr<RenderTarget> window = shared_ptr<AndroidWindow>(new AndroidWindow(type,width,height,bpp,fullScreen));
				notifier->AddRenderTarget(type,window);
				return window;
			}
			return shared_ptr<RenderTarget>();
		}
		
		//!\brief Creates a file system object configured with default data paths.
		//!\details This method will create a FileSystem object will the default data paths for Echo standard access.
		//!				Default data paths include:
		//!					- A default data source for data access.
		//!					- "persistent://" for saving data
		shared_ptr<FileSystem> CreateDefaultFileSystem(const std::string& applicationName)
		{
			shared_ptr<FileSystem> fileSystem = make_shared<FileSystem>();
			
			//Install FileSystemSources
			bool defaultSourceIsAPK = androidSupport->GetAssetManager()!=0;
			shared_ptr<FileSystemSourceAPK> apkSource(new FileSystemSourceAPK("APK",androidSupport->GetAssetManager()));
			fileSystem->InstallSource(apkSource,defaultSourceIsAPK);
			fileSystem->InstallSource<FileSystemSourceFile>("file",!defaultSourceIsAPK);
			fileSystem->InstallSource<FileSystemSourceMemory>("memory",false);
			fileSystem->InstallSource<FileSystemSourceEncrypted>("encrypted",false);
	
			//Create a persistent data storage location. This should be used for save files etc.
			if(!androidSupport->GetInternalApplicationDirectory().empty())
			{
				shared_ptr<FileSystemSourceFile> persistent = make_shared<FileSystemSourceFile>("persistent");
				persistent->SetCurrentDirectory(androidSupport->GetInternalApplicationDirectory());
				fileSystem->InstallSource(persistent);
			}
			return fileSystem;
		}

		shared_ptr<Audio> CreateDefaultAudioSystem()
		{
			OpenALAudio* alAudio = new OpenALAudio();
			alAudio->Initialise(true);
			return shared_ptr<Audio>(alAudio);
		}
		
		shared_ptr<InputManager> CreateDefaultInputManager(shared_ptr<RenderTarget>)
		{
			shared_ptr<InputManager> inputManager = make_shared<InputManager>();
			BOOST_FOREACH(shared_ptr<InputDevice> device, androidSupport->GetInputDevices())
			{
				inputManager->InstallDevice(device->GetName(),device);
			}
			return inputManager;
		}

		shared_ptr<Shell> CreateShell()
		{
			ECHO_LOG_ERROR("Shell is not available on Android.");
			return shared_ptr<Shell>();
		}
		
		void RegisterRenderTargets(FunctionBinder& functionBinder, Kernel& kernel)
		{
			if(functionBinder.Register("Window",bind(&CreateRenderTarget,"Window",placeholders::_1, boost::ref(kernel),placeholders::_2,placeholders::_3,placeholders::_4,placeholders::_5),	//GTKWindows need a Kernel.
												true,	//Allows defaults
												boost::fusion::vector<std::string,u32,u32,u8,bool>("Window",1024,768,32,false)))	//Parameters and defaults
			{
				ECHO_LOG_INFO("RenderTarget Window registered.");
			}else
			{
				ECHO_LOG_ERROR("Failed to register RenderTarget Window.");
			}
		}

		void RegisterAudioSystems(FunctionBinder& functionBinder)
		{
			if(functionBinder.RegisterVoid("Default",bind(&CreateDefaultAudioSystem)))
			{
				ECHO_LOG_INFO("Audio system Default registered.");
			}else
			{
				ECHO_LOG_ERROR("Failed to register audio system Default.");
			}
			
			if(functionBinder.RegisterVoid("OpenSLES",bind(&CreateDefaultAudioSystem)))
			{
				ECHO_LOG_INFO("Audio system OpenSLES registered.");
			}else
			{
				ECHO_LOG_ERROR("Failed to register audio system OpenSLES.");
			}
		}	
		
	}	
}
