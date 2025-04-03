#ifndef _ECHOPLATFORM_H_
#define _ECHOPLATFORM_H_
#include <string>
#include <echo/Kernel/ExecutionModel.h>

namespace Echo
{
	class RenderTarget;
	class FileSystem;
	class Kernel;
	class Audio;
	class InputManager;
	class FunctionBinder;
	class Shell;
	class Plugin;

	namespace Platform
	{
		
		/**
		 * Get the platform name.
		 * See each platform specific implementation or documentation for what value to expect.
		 * Here are some examples though:
		 *	- Linux
		 *	- Windows
		 *	- Wii
		 * @return The platform name.
		 */
		std::string GetName();
	 
		//!\brief Creates an execution model object for the target platform.
		//!\details An execution model object is used by Kernel objects to allow them to to update. The execution
		//!			environment may differ on platform to platform and these objects provide the Kernel with the
		//!			mechanism to work with the system, sometimes in multiple ways.
		//!			The execution model returned by this method is different depending on the platform.
		//!\return The platform's execution model.
		shared_ptr<ExecutionModel> CreateExecutionModel();
		
		//!\brief Creates a render target.
		//!\details This factory method will create a render target of the specified type. If the type is not specified
		//!			then the default render target for the platform will be created. Common types are:
		//!				(empty string) - Default
		//!				Texture - Texture render target
		//!				Window - Window render target
		//!\param type The render target type to create.
		//!\retrun A shared pointer to the requested render target. May be a null pointer if the type is not available.
		shared_ptr<RenderTarget> CreateRenderTarget(const std::string& type, const std::string& name, Kernel& kernel, u32 width, u32 height, u8 bpp, bool fullScreen);
		
		//!\brief Creates a file system object configured with default data paths.
		//!\details This method will create a FileSystem object will the default data paths for Echo standard access.
		//!				Default data paths include:
		//!					- A default data source for data access.
		//!					- "persistent://" for saving data
		//!\param applicationName The name of the application. This may be used by the platform to create persistent data storage.
		shared_ptr<FileSystem> CreateDefaultFileSystem(const std::string& applicationName);

		shared_ptr<Audio> CreateDefaultAudioSystem();
		
		/**
		 * Create a default input manager.
		 * The default input manager will set up input devices for the platform.
		 * Be sure to check that the input requested is valid before attempting to use it.
		 * @note Some platforms require window or similar object in order to set up a mouse-like device.
		 * @param target A render target used as the coordinate space for pointing devices.
		 */
		shared_ptr<InputManager> CreateDefaultInputManager(shared_ptr<RenderTarget> target);
		
		/**
		 * Create a shell object.
		 * @return a shell object for the platform, otherwise a null pointer if a shell isn't available.
		 */
		shared_ptr<Shell> CreateShell();

		/**
		 * Register available render target creation functions.
		 * Prototype: shared_ptr<RenderTarget> Type(std::string name, u32 width, u32 height, u8 bpp, bool fullScreen)
		 * A kernel is passed because some render targets are responsible for pause and resume events, in such cases register
		 * the function and pass the kernel directly to the bind method.
		 * @param functionBinder the function binder to register functions with.
		 * @param kernel the kernel to be paused/resumed if the render target is responsible for such events.
		 */
		void RegisterRenderTargets(FunctionBinder& functionBinder, Kernel& kernel);

		/**
		 * Register available audio systems creation functions.
		 * Prototype: shared_ptr<Audio> Type()
		 * @param functionBinder the function binder to register functions with.
		 */
		void RegisterAudioSystems(FunctionBinder& functionBinder);

		/**
		 * LibraryHandle is just a void* and the platform implementation will need to cast to and from the corresponding
		 * internal handle type.
		 */
		typedef void* LibraryHandle;

		/**
		 * LibraryFunction is just a void* and needs to be cast to the corresponding function pointer type in order to call
		 * the function.
		 */
		typedef void* LibraryFunction;

		/**
		 * Attempt to load a dynamic library.
		 * @param pathToLibrary the file name of the library to attempt to load.
		 * @return A library handle, this is the void* version of the platform specific handle (which might be a void* anyway)
		 */
		LibraryHandle LoadLibrary(std::string pathToLibrary);
		
		/**
		 * Attempt to load a function from a library.
		 * @param libraryHandle the library to load from.
		 * @param functionName the name of the function to attempt to load, your function usually needs to be declared as
		 * extern "C" in order to get around the C++ name mangling rules so your function name matches the name in code.
		 * @return A handle to the function.
		 */
		LibraryFunction LoadLibraryFunction(LibraryHandle libraryHandle, std::string functionName);
		
		/**
		 * Attempt to close library.
		 * @param libraryHandle the library to close.
		 * @return true if the library was closed.
		 */
		bool CloseLibrary(LibraryHandle libraryHandle);
	}
}
#endif
