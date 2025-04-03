#ifdef ECHO_SOCKET_SUPPORT
#include <echo/Network/SocketNetworkSystem.h>
#endif
#ifdef ECHO_WEBSOCKET_SUPPORT
#include <echo/Network/WebSocketNetworkSystem.h>
#endif
#include <echo/Network/NetworkManager.h>
#include <echo/Network/NetworkManagerUpdater.h>
#include <echo/Application.h>
#include <echo/Audio/AudioPlayer.h>
#include <echo/Platform.h>
#include <echo/Kernel/Kernel.h>
#include <echo/Input/Input.h>
#include <echo/Graphics/MultiRenderer.h>
#include <echo/Graphics/RenderTarget.h>
#include <echo/Resource/MeshManager.h>
#include <echo/Util/Utils.h>
#include <echo/FileSystem/FileSystem.h>
#include <echo/Resource/TextureManager.h>
#include <echo/Resource/FontManager.h>
#include <echo/Resource/MaterialManager.h>
#include <echo/Resource/SkeletonManager.h>
#include <echo/Resource/ShaderManager.h>
#include <echo/Resource/BitmapLoader.h>
#include <echo/Input/MappedInputDeviceLoader.h>
#include <echo/Util/NetworkRedirect.h>
#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>

#ifdef ECHO_JPEG_SUPPORT_ENABLED
#include <echo/Resource/JPEGLoader.h>
#endif
#ifdef ECHO_PNG_SUPPORT_ENABLED
#include <echo/Resource/PNGLoader.h>
#endif

namespace Echo
{
	Application::Application(){}
	Application::~Application(){}
	
	bool Application::Initialise(const std::string& name, std::string configurationFile)
	{
		if(!InitialiseFileSystem(name))
		{
			return false;
		}
		
		//Without a file system object LoadFile() will fail.
		mConfiguration.SetFileSystem(GetFileSystem());
		mConfiguration.Set("PLATFORM",Platform::GetName());
		
		if(!configurationFile.empty() && !mConfiguration.LoadFile(configurationFile))
		{
			ECHO_LOG_ERROR("Could not load configuration file: " << configurationFile);
			return false;
		}
		
		if(!Initialise(name,mConfiguration))
		{
			return false;
		}
		
		return true;
	}

	void Application::LoadResourceListsFromConfiguration(ResourceManagerBase& resourceManager)
	{
		std::string listOptionName = resourceManager.GetResourceTypeName() + ".list";
		size_t numberOfOptions = GetConfiguration().GetNumberOfOptionsNamed(listOptionName);
		for(size_t i=0;i<numberOfOptions;++i)
		{
			std::string listFileName = GetConfiguration().GetAtIndex<std::string>(listOptionName,"",i);
			resourceManager.LoadList(GetFileSystem()->Open(listFileName));
		}
	}

	void Application::LoadAudioPlayerListsFromConfiguration()
	{
		std::string listOptionName = "audio.player.list";
		size_t numberOfOptions = GetConfiguration().GetNumberOfOptionsNamed(listOptionName);
		for(size_t i=0;i<numberOfOptions;++i)
		{
			std::string listFileName = GetConfiguration().GetAtIndex<std::string>(listOptionName,"",i);
			if(GetAudioPlayer())
			{
				GetAudioPlayer()->LoadAudioList(listFileName);
			}
		}
	}
	
	bool Application::InitialiseFileSystem(const std::string& name)
	{
		if(!mFileSystem)
		{
			mFileSystem = Platform::CreateDefaultFileSystem(name);
			if(!mFileSystem)
			{
				ECHO_LOG_ERROR("Platform has incomplete implementation. Unable to create a default FileSystem object.");
				return false;
			}
		}
		return true;
	}
	
	bool Application::Initialise(const std::string& name, Configuration& configuration)
	{
		//Copy the configuration.
		mConfiguration = configuration;

		if(mConfiguration.HasOption("log.detail"))
		{
			Logger::LogMask logMask = gDefaultLogger.ConvertLabelToLogMask(mConfiguration.Get("log.detail","INFO|WARNING|ERROR"));
			gDefaultLogger.SetLogMask(logMask);
		}
		gDefaultLogger.SetFormat(mConfiguration.Get("log.format",gDefaultLogger.GetFormat()));
		
		const bool networkManager = GetConfiguration().Get("network", false);
		if(networkManager && !GetNetworkManager())
		{
			InitialiseNetworkManager("");
			const std::string& redirectStdOutServer = GetConfiguration().Get("network.redirect.cout.server", "direct:127.0.0.1:20154");
			if(GetConfiguration().Get("network.redirect.cout",false) && !redirectStdOutServer.empty())
			{
				Seconds reconnectTime = GetConfiguration().Get("network.redirect.cout.reconnect-time", Seconds(5.));
				bool alsoLocalOutput = GetConfiguration().Get("network.redirect.cout.dual-local", true);
				RedirectStdOutToNetwork(redirectStdOutServer,reconnectTime,alsoLocalOutput);
			}
		}
		
		//If the execution model has already been set then we don't need to use the default.
		shared_ptr<ExecutionModel> executionModel = GetExecutionModel();
		if(!executionModel)
		{
			executionModel = Platform::CreateExecutionModel();
		}
		//Still not set?
		if(!executionModel)
		{
			ECHO_LOG_ERROR("Platform has incomplete implementation. Unable to create an execution model.");
			return false;
		}
		
		SetExecutionModel(executionModel);
		
		
		//Set the frame time limits.
		SetFrameLimitsFromConfiguration();

		SetPauseHint(GetConfiguration().Get("kernel.pause-hint",true));

		if(!InitialiseFileSystem(name))
		{
			return false;
		}
		
		//If configuration file system hasn't been specified, do so.
		if(!mConfiguration.GetFileSystem())
		{
			mConfiguration.SetFileSystem(GetFileSystem());
		}
		
		//Set up our resource managers

		//The texture manager manages texture resources.
		//We are only setting it up with a PNGLoader for this test but you can also add a BitmapLoader.
		mTextureManager = shared_ptr<TextureManager>(new TextureManager(*mFileSystem));
		#ifdef ECHO_JPEG_SUPPORT_ENABLED
		mTextureManager->RegisterLoader(make_shared<JPEGLoader>());
		#endif
		#ifdef ECHO_PNG_SUPPORT_ENABLED
		mTextureManager->RegisterLoader(make_shared<PNGLoader>());
		#endif
		mTextureManager->RegisterLoader(make_shared<BitmapLoader>());

		mGeometryShaderManager = shared_ptr<ShaderManager>(new ShaderManager("geometry",*mFileSystem));
		mVertexShaderManager = shared_ptr<ShaderManager>(new ShaderManager("vertex",*mFileSystem));
		mFragmentShaderManager = shared_ptr<ShaderManager>(new ShaderManager("fragment",*mFileSystem));
		mMaterialManager = shared_ptr<MaterialManager>(new MaterialManager(*mFileSystem, *mTextureManager, *mGeometryShaderManager, *mVertexShaderManager, *mFragmentShaderManager));
		mSkeletonManager = shared_ptr<SkeletonManager>(new SkeletonManager(*mFileSystem));
		mMeshManager = shared_ptr<MeshManager>(new MeshManager(*mFileSystem,*mMaterialManager,*mSkeletonManager));

		//Register platform creator functions.
		Platform::RegisterRenderTargets(mCreatorFactory, *this);
		Platform::RegisterAudioSystems(mCreatorFactory);

		//A multi-renderer managers back buffer switching for multiple renderers. You can use two Renderer objects
		//and set their flags manually but the MultiRenderer makes it a bit easier.
		mRenderer = shared_ptr<MultiRenderer>(new MultiRenderer("Default Renderer"));
		
		//Create render targets and add them to the renderer.
		std::list< shared_ptr<RenderTarget> > renderTargets = CreateObjects< shared_ptr<RenderTarget> >("render.target");
		if(!renderTargets.empty())
		{
			BOOST_FOREACH(shared_ptr<RenderTarget> renderTarget, renderTargets)
			{
				std::string uniqueName = GenerateUniqueName(renderTarget->GetName(),mRenderTargets);
				mRenderTargets.insert(std::make_pair(uniqueName,renderTarget));
				mRenderer->AddRenderTarget(renderTarget);
			}
		}
		
		//Create a window render target. Using the platform interface will create the default one for the target
		shared_ptr<RenderTarget> primaryRenderTarget;
		u32 width = mConfiguration.Get("resolution.x", 0u);
		u32 height = mConfiguration.Get("resolution.y", 0u);
		if(width!=0 && height!=0)
		{
			const bool fullScreenHint = mConfiguration.Get("fullscreen", false);
			std::string uniqueRenderTargetName = GenerateUniqueName("Default",mRenderTargets);
			shared_ptr<RenderTarget> renderTarget =
				Platform::CreateRenderTarget("Window",mConfiguration.Get("window.title", name), *this, width,height, 32, fullScreenHint);
			if(!renderTarget)
			{
				ECHO_LOG_ERROR("Platform has incomplete implementation. Unable to create a \"Window\" render target.");
				return false;
			}
			renderTarget->SetPrimary(true);
			mRenderer->AddRenderTarget(renderTarget);
			mRenderTargets.insert(std::make_pair(uniqueRenderTargetName,renderTarget));
			primaryRenderTarget=renderTarget;
		}

		Size defaultXDPI = 300;
		Size defaultYDPI = 300;
		if(primaryRenderTarget)
		{
			defaultXDPI = primaryRenderTarget->GetDisplayXDPI();
			defaultYDPI = primaryRenderTarget->GetDisplayYDPI();
		}
		mFontManager = shared_ptr<FontManager>(new FontManager(*mFileSystem,*mTextureManager,*mMaterialManager, defaultXDPI, defaultYDPI));
		
		mInputManager = Platform::CreateDefaultInputManager(primaryRenderTarget);
		if(!mInputManager)
		{
			ECHO_LOG_ERROR("Platform has incomplete implementation. Unable to create a default input manager.");
			return false;
		}
		AddTask(*mInputManager);

		//If applications want to disable audio they can do it via configuration option "audio.enabled=0"
		if(GetConfiguration().Get<bool>("audio.enabled",true))
		{
			mAudio = CreateObject< shared_ptr<Audio> >("audio");
			if(!mAudio)
			{
				mAudio = Platform::CreateDefaultAudioSystem();
			}
			if(!mAudio)
			{
				ECHO_LOG_ERROR("Platform has incomplete implementation. Unable to create an Audio system.");
				//return false;
			}else
			{
				LoadAudioPlayerListsFromConfiguration();
			}
		}
		
		//Initialise resources
		LoadResourceListsFromConfiguration(*mTextureManager);
		LoadResourceListsFromConfiguration(*mMaterialManager);
		LoadResourceListsFromConfiguration(*mFontManager);
		LoadResourceListsFromConfiguration(*mSkeletonManager);
		LoadResourceListsFromConfiguration(*mMeshManager);
		LoadResourceListsFromConfiguration(*mGeometryShaderManager);
		LoadResourceListsFromConfiguration(*mVertexShaderManager);
		LoadResourceListsFromConfiguration(*mFragmentShaderManager);

		//Attempt to load mapped input devices from files.
		if(mInputManager && mFileSystem)
		{
			size_t numberOfDevices = MappedInputDeviceLoader::LoadMappedDevices(mConfiguration,*mInputManager,*mFileSystem);
			if(numberOfDevices)
			{
				ECHO_LOG_INFO(numberOfDevices << " mapped input devices loaded.");
			}
		}
		return true;
	}
	
	shared_ptr<RenderTarget> Application::GetRenderTarget(std::string name) const
	{
		std::map< std::string, shared_ptr<RenderTarget> >::const_iterator it = mRenderTargets.find(name);
		if(it!=mRenderTargets.end())
		{
			return it->second;
		}
		return shared_ptr<RenderTarget>();
	}
	
	bool Application::InitialiseNetworkManager(std::string updateMode)
	{
		if(updateMode.empty())
		{
			updateMode = GetConfiguration().Get("network.update-mode", "kernel");
			if(updateMode.empty())
			{
				updateMode = "kernel";
			}
		}
		mNetworkManager.reset(new NetworkManager());
		#ifdef ECHO_SOCKET_SUPPORT
		shared_ptr<SocketNetworkSystem> socketSystem(new SocketNetworkSystem(*mNetworkManager));
		if(!mNetworkManager->InstallSystem(socketSystem, true))
		{
			return false;
		}
		#endif
		#ifdef ECHO_WEBSOCKET_SUPPORT
		shared_ptr<WebSocketNetworkSystem> webSocketSystem(new WebSocketNetworkSystem(*mNetworkManager));
		if(!mNetworkManager->InstallSystem(webSocketSystem, true))
		{
			return false;
		}
		#endif
		if(updateMode=="kernel")
		{
			AddTask(*mNetworkManager);
		}else
		if(updateMode=="network.events")
		{
			mNetworkManager->AddNetworkEventListener(shared_ptr<NetworkManagerUpdater>(new NetworkManagerUpdater(*mNetworkManager)));
		}
		return true;
	}
	
	shared_ptr<AudioPlayer> Application::GetAudioPlayer(bool createIfRequired)
	{
		if(!mAudioPlayer && createIfRequired)
		{
			if(!GetAudio())
			{
				ECHO_LOG_ERROR("Unable to create an AudioPlayer. Audio object is null."); 
			}else
			if(!GetFileSystem())
			{
				ECHO_LOG_ERROR("Unable to create an AudioPlayer. FileSystem object is null.");
			}else
			{
				mAudioPlayer.reset(new AudioPlayer(GetAudio(),GetFileSystem()));
			}
		}
		return mAudioPlayer;
	}
	
	bool Application::RedirectStdOutToNetwork(const std::string& server, Seconds reconnectTime, bool alsoLocalOutput)
	{
		if(!mNetworkManager)
		{
			if(!InitialiseNetworkManager(""))
			{
				return false;
			}
		}
		
		//Attempt to connect to the stream server.
		shared_ptr<Connection> connection = mNetworkManager->Connect(server);
		if(!connection)
		{
			ECHO_LOG_ERROR("Could not create a connection to " << server << " for output redirection");
			return false;
		}
		if(reconnectTime>Seconds(0.))
		{
			connection->SetAutoAttemptReconnect(true);
			connection->SetAutoAttemptReconnectTime(reconnectTime);
		}
		mNetworkRedirect.reset(new NetworkStreamBuffer(connection));

		//Redirect cout to our network buffer.
		std::streambuf* original=std::cout.rdbuf(mNetworkRedirect.get());
		if(alsoLocalOutput)
		{
			//Tell the redirect buffer to output to our original cout as well, so we get local and remote output.	
			mNetworkRedirect->SetSecondaryOut(original);
		}
		mNetworkRedirect->SetOriginalOut(original);
		return true;
	}
	
	void Application::SetFrameLimitsFromConfiguration()
	{
		Configuration& configuration = GetConfiguration();
		FrameRateLimiter& limiter = GetFrameRateLimiter();
		
		// The minimum frame time can be set with two parameters. kernel.frame-time.min and
		// kernel.frame-rate.fps with the former taking priority. We can essentially the
		// priority by using defaults with Configuration rather than explicitly checking for
		// the parameters.
		Size fps = configuration.Get<Size>("kernel.frame-rate.fps", 60);
		if(fps==0)
		{
			ECHO_LOG_WARNING("0 is not a valid value for kernel.frame-rate.fps. Defaulting to 60.");
			fps=60;
		}
		const Seconds minFrameTime = configuration.Get<Seconds>("kernel.frame-time.min", Seconds(1./Scalar(fps)));
		limiter.SetMinFrameTime(minFrameTime);
		
		const Seconds maxFrameTime = GetConfiguration().Get<Seconds>("kernel.frame-time.max", Seconds(1.));
		limiter.SetMaxFrameTime(maxFrameTime);
	}

}
