#ifndef _ECHOAPPLICATION_H_
#define _ECHOAPPLICATION_H_
#include <echo/Kernel/Kernel.h>
#include <echo/Util/FunctionBinder.h>
#include <echo/Util/Configuration.h>
#include <echo/Resource/ResourceManagerBase.h>

namespace Echo
{
	class FileSystem;
	class TextureManager;
	class FontManager;
	class ShaderManager;
	class MaterialManager;
	class SkeletonManager;
	class MeshManager;
	class RenderTarget;
	class MultiRenderer;
	class Audio;
	class AudioPlayer;
	class InputManager;
	class NetworkManager;
	class NetworkStreamBuffer;
	
	/**
	 * Application.
	 * This class provides the setup of common object used by most applications.
	 * For initialisation of an Application object the following will be performed:
	 *	- Initialise the File system to allow you access to the platform data sources.
	 *	- Initialise the Audio system.
	 *	- Set up common Resource managers. The manages will load lists as defined by the configuration.
	 *	- An InputManager with installed devices as determined by the platform.
	 *	- Read the configuration, if provided, for parameters to configure the default/primary render target.
	 *	- Read the configuration, if provided, for any mapped input devices.
	 *	- Read the configuration, if provided, for any additional render targets.
	 * 	- Read the configuration, if provided, for any additional render targets.
	 */
	class Application : public Kernel
	{
	public:
		Application();
		~Application();
		
		/**
		 * Initialise the application settings from a configuration object.
		 * This variation of Initialise allows you to initialise in code if you prefer not to load a file.
		 * @note Loading a configuration file into a Configuration object so you can pass it into this method
		 * requires that you initialise a file system object in order to obtain a File object to load from.
		 * Save yourself the effort and use the other version of this method which will initialise the
		 * file system for you. In either case the file system will be initialised but the other method will
		 * initialise it so the configuration can be loaded in.
		 * @see Initialise(const std::string&, std::string) for loading the Configuration
		 * object from a file.
		 * @param name application name.
		 * @param configuration The configuration to initialise with, this will be copied to the
		 * class configuration for access via GetConfiguration().
		 * @return true if initialisation was successful, otherwise false.
		 */
		bool Initialise(const std::string& name, Configuration& configuration);

		/**
		 * Initialise the application with settings from a configuration file.
		 * If you don't specify a configuration file (i.e. empty string) then no configuration
		 * is loaded and the application will not initialise a default render target. Initialisation still
		 * occurs though. This can be used to create a command line application.
		 * 
		 * This method will initialise the file system object first so it can load the configuration file.
		 * 
		 * @param name application name.
		 * @param configuration File the configuration file to load, can be an empty string.
		 * @return true if initialisation was successful, otherwise false.
		 */
		bool Initialise(const std::string& name, std::string configurationFile = "");
		
		/**
		 * Initialise the network manager.
		 * The NetworkManager will send send and receive data on another thread.
		 * 
		 * When data is received, connection callbacks and network events (such as new connection)
		 * are fired when the network manager is updated.
		 * 
		 * Typically the manager is updated by the kernel on the main thread which allows you to have
		 * network packets processed at a defined point in your application's execution. The default
		 * is with Kernel updates but you can move the network manager to a different task manager to
		 * change when the update occurs). On the other hand, you can also get the network manager to
		 * process connection callbacks on a network thread as soon as a packet arrives.
		 * 
		 * The thread that processes the packets may not be the same every time, this depends on the
		 * platform implementation though. For example there might be a limit to the number of
		 * connections that a thread can process at once, which means there might be a number of
		 * worker threads (this is often the case).
		 * 
		 * The updateMode parameter allows you to control the network manager update mode. The
		 * following modes are supported:
		 * 
		 *	kernel - The NetworkManager instance is added to the kernel for updates.
		 *  network.events - When a network event occurs the network manager will update.
		 *  none - No NetworkManager updates are made. This allows you to take control updates
		 *	yourself. Note: Failure to update the network manager can cause a backlog of packets
		 *  and incoming connections.
		 * 
		 * If updateMode is empty the value will be acquired from the network.update-mode
		 * configuration option. If no option is found then "kernel" is used.
		 * 
		 * @param updateMode How to update the network manager.
		 * @return true if the network manager is initialised 
		 */
		bool InitialiseNetworkManager(std::string updateMode = "kernel");

		shared_ptr<FileSystem> GetFileSystem() const {return mFileSystem;}
		shared_ptr<TextureManager> GetTextureManager() const {return mTextureManager;}
		shared_ptr<FontManager> GetFontManager() const {return mFontManager;}
		shared_ptr<ShaderManager> GetGeometryShaderManager() const {return mGeometryShaderManager;}
		shared_ptr<ShaderManager> GetVertexShaderManager() const {return mVertexShaderManager;}
		shared_ptr<ShaderManager> GetFragmentShaderManager() const {return mFragmentShaderManager;}
		shared_ptr<MaterialManager> GetMaterialManager() const {return mMaterialManager;}
		shared_ptr<SkeletonManager> GetSkeletonManager() const {return mSkeletonManager;}
		shared_ptr<MeshManager> GetMeshManager() const {return mMeshManager;}
		shared_ptr<RenderTarget> GetRenderTarget(std::string name="Default") const;
		shared_ptr<MultiRenderer> GetRenderer() const {return mRenderer;}
		shared_ptr<InputManager> GetInputManager() const {return mInputManager;}
		shared_ptr<NetworkManager> GetNetworkManager() const {return mNetworkManager;}
		shared_ptr<Audio> GetAudio() const {return mAudio;}
		shared_ptr<AudioPlayer> GetAudioPlayer(bool createIfRequired = true);
		Configuration& GetConfiguration() {return mConfiguration;}
		
		/**
		 * Redirect stdout to a network server.
		 * @note if the network manager is not initialised before this method is called it will be
		 * initialised. If you want more control over how the network manager is updated then call
		 * InitialiseNetworkManager() before calling this method.
		 * @param server The server that will receive output data.
		 * @param reconnectTime if greater than 0 then the time between reconnection attempts if the
		 * connection is dropped.
		 * @param alsoLocalOutput If true will also output to what would have been the the local stdout.
		 * @return true if successful. False indicates the network manager was not initialised and
		 * failed to initialise automatically or that a connection could not be created.
		 */
		bool RedirectStdOutToNetwork(const std::string& server, Seconds reconnectTime = Seconds(5.), bool alsoLocalOutput = true);
	private:
		shared_ptr<FileSystem> mFileSystem;
		shared_ptr<TextureManager> mTextureManager;
		shared_ptr<FontManager> mFontManager;
		shared_ptr<ShaderManager> mGeometryShaderManager;
		shared_ptr<ShaderManager> mVertexShaderManager;
		shared_ptr<ShaderManager> mFragmentShaderManager;
		shared_ptr<MaterialManager> mMaterialManager;
		shared_ptr<SkeletonManager> mSkeletonManager;
		shared_ptr<MeshManager> mMeshManager;
		std::map< std::string, shared_ptr<RenderTarget> > mRenderTargets;
		shared_ptr<MultiRenderer> mRenderer;
		shared_ptr<Audio> mAudio;
		shared_ptr<AudioPlayer> mAudioPlayer;
		shared_ptr<InputManager> mInputManager;
		shared_ptr<NetworkManager> mNetworkManager;
		shared_ptr<NetworkStreamBuffer> mNetworkRedirect;
		Configuration mConfiguration;
		FunctionBinder mCreatorFactory;			//Used to register functions via the platform interface.
		bool InitialiseFileSystem(const std::string& name);
		void LoadResourceListsFromConfiguration(ResourceManagerBase& resourceManager);
		void LoadAudioPlayerListsFromConfiguration();
		void SetFrameLimitsFromConfiguration();

		/**
		 * Create an object using value from the specified option in the Configuration object.
		 * The method acquires the value the configuration option and passes it into the
		 * FunctionBinder::Call(). The mCreatorFactory is the function binder used.
		 * mCreatorFactory is passed to the Platform::Register* methods which allows platforms
		 * to register functions to create objects.
		 * @note Since the method will always return an object, it is important that the default
		 * constructor can be used and that you do not need to know of a failure. Generally the
		 * method is used to create shared_ptr< X > objects so the default object is a null
		 * pointer. If you need error checking implement another method to do so.
		 * @note A warning is output on a Call() failure.
		 * @param optionName The name of the option in the configuration file.
		 * @return The constructed object, or a default-constructed object if the call failed or
		 * optionName does not exist.
		 */
		template <typename T>
		T CreateObject(const std::string& optionName)
		{
			const std::string& creationFunction = GetConfiguration().Get(optionName, "");
			if(!creationFunction.empty())
			{
				T returnedObject;
				if(mCreatorFactory.Call<T>(creationFunction, &returnedObject).mStatus == FunctionBinder::CallStatuses::SUCCESS)
				{
					return returnedObject;
				}
				ECHO_LOG_WARNING("Failed to create object for \"" << optionName << "\" using \"" << creationFunction << "\"");
			}
			return T();
		}
		
		/**
		 * Multi-object version of CreateObject().
		 * This method will attempt to create an object for each option in the Configuration
		 * object. If Call() fails a message will be output for the value but does not cause
		 * the funciton to fail. This is by design so the configuration file can have multiple
		 * values for different platforms if required.
		 * @see CreateObject()
		 * @return A list of new objects.
		 */
		template <typename T>
		std::list<T> CreateObjects(const std::string& optionName)
		{
			std::list<T> objects;
			size_t numberOfOptions = GetConfiguration().GetNumberOfOptionsNamed(optionName);
			for(size_t i=0;i<numberOfOptions;++i)
			{
				std::string creationFunction = GetConfiguration().GetAtIndex<std::string>(optionName,"",i);
				T returnedObject;
				if(mCreatorFactory.Call<T>(creationFunction, &returnedObject).mStatus == FunctionBinder::CallStatuses::SUCCESS)
				{
					objects.push_back(returnedObject);
				}else
				{
					ECHO_LOG_WARNING("Failed to create object for \"" << optionName << "\" using \"" << creationFunction << "\"");
				}
			}
			return objects;
		}
	};
}
#endif
