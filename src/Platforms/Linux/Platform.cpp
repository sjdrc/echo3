#include <echo/Platform.h>
#include <echo/FileSystem/FileSystem.h>
#include <echo/FileSystem/FileSystemSourceFile.h>
#include <echo/FileSystem/FileSystemSourceMemory.h>
#include <echo/Platforms/Linux/FileSystemSourceCURL.h>
#include <echo/Platforms/POSIX/POSIXShell.h>
#include <echo/Util/FunctionBinder.h>
#include <echo/Input/Input.h>
#include <echo/cpp/functional>

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <dlfcn.h>


namespace Echo
{
	namespace Platform
	{
		std::string GetName()
		{
			return "Linux";
		}

		//!\brief Creates a file system object configured with default data paths.
		//!\details This method will create a FileSystem object will the default data paths for Echo standard access.
		//!				Default data paths include:
		//!					- A default data source for data access.
		//!					- "Persistent://" for saving data
		shared_ptr<FileSystem> CreateDefaultFileSystem(const std::string& applicationName)
		{
			shared_ptr<FileSystem> fileSystem = make_shared<FileSystem>();
			
			//Install a default file source
			fileSystem->InstallSource<FileSystemSourceFile>("file",true);
			fileSystem->InstallSource<FileSystemSourceMemory>("memory",false);
			
			//Create a persistent data storage location. This should be used for save files etc.
			struct passwd *pw = getpwuid(getuid());
			std::string homedir;
			if(pw)
			{
				homedir = pw->pw_dir;
				homedir+="/";
			}else
			{
				ECHO_LOG_ERROR("Failed to get home directory using getpwuid(): " << errno << ". \"persistent\" storage source will be relative to the the working directory as \".echo/" << applicationName << "\"");
			}

			shared_ptr<FileSystemSourceFile> persistent = make_shared<FileSystemSourceFile>("persistent");
			std::string persistentDirectory = homedir + std::string(".echo/") + applicationName;
			persistent->SetCurrentDirectory(persistentDirectory);
			fileSystem->InstallSource(persistent);
			
			fileSystem->InstallSource(make_shared<FileSystemSourceCURL>(*fileSystem, "http", "cache"));
			fileSystem->InstallSource(make_shared<FileSystemSourceCURL>(*fileSystem, "https", "cache"));
			return fileSystem;
		}

		shared_ptr<Shell> CreateShell()
		{
			return POSIXShell::Create();
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
			if(functionBinder.Register("Texture",bind(&CreateRenderTarget,"Texture",placeholders::_1, boost::ref(kernel),placeholders::_2,placeholders::_3,placeholders::_4,placeholders::_5),	//GTKWindows need a Kernel.
												true,	//Allows defaults
												boost::fusion::vector<std::string,u32,u32,u8,bool>("Texture",512,512,32,false)))	//Parameters and defaults
			{
				ECHO_LOG_INFO("RenderTarget Texture registered.");
			}else
			{
				ECHO_LOG_ERROR("Failed to register RenderTarget Texture.");
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
		}
		
		LibraryHandle LoadLibrary(std::string pathToLibrary)
		{
			if(pathToLibrary.length() > 2 && pathToLibrary.substr(0,2)!="./")
			{
				// Try a local directory path search first
				std::string pathToTry = "./" + pathToLibrary;
				LibraryHandle handle = dlopen(pathToTry.c_str(),RTLD_NOW);
				if (handle)
				{ 
					return handle;
				}
			}
			LibraryHandle handle = dlopen(pathToLibrary.c_str(),RTLD_NOW);
			if (!handle)
			{
				ECHO_LOG_ERROR("Unable to load " << pathToLibrary << ":" << dlerror());
			}
			return handle;
		}
		
		LibraryFunction LoadLibraryFunction(LibraryHandle libraryHandle, std::string functionName)
		{
			dlerror();//Clear the error code.
			char* error;
			LibraryFunction handle = dlsym(libraryHandle,functionName.c_str());
			if ((error = dlerror()) != NULL)
			{
				ECHO_LOG_ERROR("Unable to load " << functionName << ":" << error);
			}
			return handle;
		}
		
		bool CloseLibrary(LibraryHandle libraryHandle)
		{
			int code = dlclose(libraryHandle);
			// Zero on success.
			if(code==0)
			{
				return true;
			}
			ECHO_LOG_ERROR("Failed with code " << code);
			return false;
		}
	}
}
