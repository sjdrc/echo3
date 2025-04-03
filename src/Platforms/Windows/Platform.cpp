#include <echo/Platform.h>
#include <echo/FileSystem/FileSystem.h>
#include <echo/FileSystem/FileSystemSourceFile.h>
#include <echo/Platforms/GL/GLRenderTarget.h>
#include <echo/Platforms/AL/OpenALAudio.h>
#include <echo/Platforms/Windows/WindowsGLWindow.h>
#include <echo/Platforms/Windows/WindowsExecutionModel.h>
#include <echo/Platforms/Windows/WindowsMouse.h>
#include <echo/Platforms/Windows/WindowsKeyboard.h>
#include <echo/Util/FunctionBinder.h>
#include <echo/Input/Input.h>
#include <echo/cpp/functional>
#include <userenv.h>

#undef SetCurrentDirectory

namespace Echo
{
	namespace Platform
	{
		std::string GetName()
		{
			return "Windows";
		}

		shared_ptr<ExecutionModel> CreateExecutionModel()
		{
			return make_shared<WindowsExecutionModel>();
		}
		
		shared_ptr<RenderTarget> CreateRenderTarget(const std::string& type, const std::string& name, Kernel& kernel, u32 width, u32 height, u8 bpp, bool fullScreen)
		{
			if(type=="Window")
			{
				shared_ptr<GLContext> context(new GLContext());
				return shared_ptr<WindowsGLWindow>(new WindowsGLWindow(context, kernel,name,width,height,bpp,fullScreen));
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
			
			//Install a default file source
			fileSystem->InstallSource<FileSystemSourceFile>("file",true);
			
			//Create a persistent data storage location. This should be used for save files etc.
			// The following code snippet came from MSDN and has been modified as needed.

			// Get the user's profile directory
			const DWORD BUFFER_LENGTH = 512;
			char buffer[BUFFER_LENGTH];
			DWORD bufferLength = BUFFER_LENGTH;
			HANDLE hToken;
			std::string homedir;

			shared_ptr<FileSystemSourceFile> persistent = make_shared<FileSystemSourceFile>("persistent");
			if( OpenProcessToken( GetCurrentProcess(), TOKEN_READ, &hToken ))
			{
				if( !GetUserProfileDirectory( hToken, buffer, &bufferLength ))
				{
					const Size UNREASONABLY_LONG_PATH_LENGTH = 1024*1024*10;
					if(bufferLength<UNREASONABLY_LONG_PATH_LENGTH)
					{
						char* sufficentBuffer=new char[bufferLength];
						if( GetUserProfileDirectory( hToken, sufficentBuffer, &bufferLength ))
						{
							homedir=sufficentBuffer;
						}
						delete [] sufficentBuffer;
					}
				}else
				{
					homedir=buffer;
				}
				CloseHandle( hToken );			
			}
			if(!homedir.empty())
			{
				std::string persistentDirectory = homedir + "/.echo/" + applicationName;
				persistent->SetCurrentDirectory(persistentDirectory);
			}else
			{
				ECHO_LOG_ERROR("Unable to find user's profile directory to set up persistent data source. I'll use the current directory which may not be writable.");
			}
			fileSystem->InstallSource(persistent);
			return fileSystem;
		}
		
		shared_ptr<Audio> CreateDefaultAudioSystem()
		{
			OpenALAudio* alAudio = new OpenALAudio();
			alAudio->Initialise(true);
			return shared_ptr<Audio>(alAudio);
		}

		shared_ptr<InputManager> CreateDefaultInputManager(shared_ptr<RenderTarget> target)
		{
			shared_ptr<InputManager> inputManager = make_shared<InputManager>();
			if(target)
			{
				shared_ptr<WindowsMouse> mouse = make_shared<WindowsMouse>(dynamic_pointer_cast<SWindow,RenderTarget>(target),target);
				if(mouse)
				{
					inputManager->InstallDevice("Mouse",mouse);
				}

				shared_ptr<WindowsKeyboard> keyboard = make_shared<WindowsKeyboard>();
				if(keyboard)
				{
					inputManager->InstallDevice("Keyboard",keyboard);
				}
			}
			return inputManager;
		}
		
		shared_ptr<Shell> CreateShell()
		{
			ECHO_LOG_ERROR("A shell has not been implemented for Windows.");
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
			
			if(functionBinder.RegisterVoid("OpenAL",bind(&CreateDefaultAudioSystem)))
			{
				ECHO_LOG_INFO("Audio system OpenAL registered.");
			}else
			{
				ECHO_LOG_ERROR("Failed to register audio system OpenAL.");
			}
		}

		LibraryHandle LoadLibrary(std::string pathToLibrary)
		{
			ECHO_LOG_ERROR("Unable to load Library: DLL support is not implemented");
			return nullptr;
		}
		
		LibraryFunction LoadLibraryFunction(LibraryHandle libraryHandle, std::string functionName)
		{
			ECHO_LOG_ERROR("Unable to load library function: DLL support is not implemented");
			return nullptr;
		}
		
		bool CloseLibrary(LibraryHandle)
		{
			ECHO_LOG_ERROR("Unable to close library: DLL support is not implemented");
			return false;
		}
	}
}
