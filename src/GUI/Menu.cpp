#include <echo/GUI/Menu.h>
#include <echo/GUI/GUI.h>
#include <echo/Util/FunctionBinder.h>
#include <echo/Util/Utils.h>
#include <echo/Util/StringUtils.h>
#include <echo/Util/Configuration.h>
#include <echo/Graphics/MultiRenderer.h>
#include <echo/FileSystem/FileSystem.h>
#include <echo/Graphics/Renderer.h>

#include <map>
#include <stack>

#include <echo/cpp/functional>

namespace Echo
{
	namespace GUI
	{
		Menu::Menu(shared_ptr<FileSystem> fileSystem,
				shared_ptr<MaterialManager> materialManager,
				shared_ptr<FontManager> fontManager,
				shared_ptr<InputManager> inputManager,
				shared_ptr<MultiRenderer> multiRenderer,
				shared_ptr<FunctionBinder> functionBinder) : TaskGroup("Menu"),
			mFileSystem(fileSystem),
			mInputManager(inputManager),
			mMaterialManager(materialManager),
			mFontManager(fontManager),
			mMultiRenderer(multiRenderer),
			mFunctionBinder(new FunctionBinder()),
			mUnloadInactiveMenus(true)
		{
			assert(mFileSystem && "mFileSystem cannot be null");
			assert(mMaterialManager && "mMultiRenderer cannot be null");
			assert(mFontManager && "mFontManager cannot be null");
			assert(mInputManager && "mInputManager cannot be null");
			assert(mMultiRenderer && "mMultiRenderer cannot be null");

			mFunctionBinder->SetFallback(functionBinder);
			RegisterFunctions();
		}
		
		Menu::~Menu()
		{
			//Avoid any bad calls.
			DeregisterFunctions();
		}
		
		bool Menu::LoadGUIList(const std::string& fileName)
		{
			Configuration menuList(mFileSystem,mFunctionBinder);
			if(!menuList.LoadFile(fileName))
			{
				return false;
			}
			std::string optionName = "menu";
			size_t numberOfOptions = menuList.GetNumberOfOptionsNamed(optionName);
			for(size_t i=0;i<numberOfOptions;++i)
			{
				std::string menuFile= menuList.GetAtIndex<std::string>(optionName,"",i);
				std::vector<std::string> options;
				Utils::String::Split(menuFile,",",options);
				
				if(options.empty())
				{
					continue;
				}
				std::string resolvedMenuFile = mFileSystem->ResolveFullNameForFileWithParent(options[0],fileName);
				
				if(options.size()>=3)
				{
					//This is a bit messy.
					bool preload = options[2]=="Preload";
					u32 priority = 5000;
					u32 priorityOption = 0;
					if(preload)
					{
						priorityOption = (options.size()>3) ? 3 : 0;
					}else
					{
						priorityOption = 2;
					}
					if(priorityOption)
					{
						try
						{
							priority = boost::lexical_cast<u32>(options[priorityOption]);
						}catch(...){} //Catch the convert exception.
					}
					AddGUI(resolvedMenuFile,options[1],preload,priority);
				}else
				if(options.size()==2)
				{
					AddGUI(resolvedMenuFile,options[1]);
				}else
				if(options.size()==1)
				{
					AddGUI(resolvedMenuFile,options[0]);
				}
			}
			
			std::string firstGUI = menuList.Get("active","");
			if(!firstGUI.empty())
			{
				Push(firstGUI);
			}
			
			return true;
		}

			
		bool Menu::AddGUI(const std::string& fileName, std::string name, bool preload, u32 rendererPriority)
		{
			if(name.empty())
			{
				name = fileName;
			}
			if(GUIExists(name))
			{
				ECHO_LOG_ERROR("GUI with name already exists: " << name);
				return false;
			}

			if(!preload)
			{
				mMenuFiles.insert(std::make_pair(name, GUISettings(fileName,rendererPriority)));
				return true;
			} else
			{
				shared_ptr<GUIScene> guiScene = LoadGUI(fileName, rendererPriority);
				if(guiScene)
				{
					mPreloadedMenus.insert(std::make_pair(name, guiScene));
					return true;
				}
				//Errors will be output during the attempt to load. No need to duplicate.
			}
			return false;
		}

		bool Menu::AddScene(shared_ptr<Scene> scene, std::string name, u32 rendererPriority)
		{
			if(scene)
			{
				//Complain, provide the name, it might help with tracking it down.
				ECHO_LOG_ERROR("null scene object passed. name: " << name);
				return false;
			}
			
			if(GUIExists(name))
			{
				//Complain, provide the name, it might help with tracking it down.
				ECHO_LOG_ERROR("GUI with name already exists: " << name);
				return false;
			}
			
			if(name.empty())
			{
				// Well, we'll have to come up with something.
				name = GenerateUniqueName("UnnamedMenu",mPreloadedMenus);
			}

			//Need to create a GUIScene object for this gui.
			shared_ptr<GUIScene> guiScene(new GUIScene());
			guiScene->mScene = scene;
			guiScene->mRenderer = CreateRenderer(rendererPriority);
			
			mPreloadedMenus.insert(std::make_pair(name, guiScene));
			return true;
		}
		
		void Menu::Push(const std::string& name)
		{
			if(SetCurrent(name))
			{
				mStack.push(name);
			}else
			{
				ECHO_LOG_ERROR("Unable to set the current menu to \"" << name << "\"");
			}
		}
		
		void Menu::Pop()
		{
			if(mStack.empty())
			{
				ECHO_LOG_WARNING("No menus left to pop.");
				return;

			}
			mStack.pop();

			if(!mStack.empty())
			{
				if(!SetCurrent(mStack.top()))
				{
					ECHO_LOG_ERROR("Failed to set last menu \"" << mStack.top() << "\".");
				}
			}else
			{
				//No more left, passing null here will clear the current menu as appropriate.
				SetCurrent(shared_ptr<GUIScene>());
			}
		}
		
		void Menu::PopTo(const std::string& name)
		{
			while(!mStack.empty())
			{
				if(mStack.top()==name)
				{
					//Time to set the menu.
					SetCurrent(name);
					return;
				}
				mStack.pop();
			}
			ECHO_LOG_ERROR("Popped all menus but \"" << name << "\" was not found.");
		}

		void Menu::ClearStack()
		{
			if(mCurrentScene)
			{
				mCurrentScene->mRenderer->Pause();
				mMultiRenderer->RemoveRenderer(mCurrentScene->mRenderer);
				mCurrentScene->mScene->Pause();
			}

			mCurrentScene.reset();
			
			while(!mStack.empty())
			{
				mStack.pop();
			}
		}

		void Menu::QueuePush(const std::string& name)
		{
			QueuePostUpdateAction(bind(&Menu::Push,this,name));
		}
		
		void Menu::QueuePop()
		{
			QueuePostUpdateAction(bind(&Menu::Pop,this));
		}
		
		void Menu::QueuePopTo(const std::string& name)
		{
			QueuePostUpdateAction(bind(&Menu::PopTo,this,name));
		}
		
		void Menu::QueueClearStack()
		{
			QueuePostUpdateAction(bind(&Menu::ClearStack,this));
		}
		
		void Menu::SetUnloadInactiveMenus(bool unloadInactiveMenus)
		{
			mUnloadInactiveMenus = unloadInactiveMenus;
		}
		
		shared_ptr<GUI> Menu::GetCurrentGUI() const
		{
			if(mCurrentScene)
			{
				return mCurrentScene->mGUI;
			}
			return nullptr;
		}
		
		bool Menu::GUIExists(const std::string& name)
		{
			return (mMenuFiles.find(name)!=mMenuFiles.end() ||
				mPreloadedMenus.find(name)!=mPreloadedMenus.end());
		}
		
		shared_ptr<Menu::GUIScene> Menu::GetOrLoadGUI(const std::string& name)
		{
			//First look for preloaded GUIs.
			NamedGUIMap::iterator git = mPreloadedMenus.find(name);
			if(git!=mPreloadedMenus.end())
			{
				return git->second;
			}
			NamedFileMap::iterator fit=mMenuFiles.find(name);
			if(fit!=mMenuFiles.end())
			{
				shared_ptr<GUIScene> guiScene = LoadGUI(fit->second.mFile,fit->second.mRendererPriority);
				if(guiScene)
				{
					if(!mUnloadInactiveMenus)
					{
						mPreloadedMenus.insert(std::make_pair(name, guiScene));
					}
				}
				return guiScene;
			}
			return shared_ptr<GUIScene>();
		}

		shared_ptr<Menu::GUIScene> Menu::LoadGUI(const std::string& fileName, u32 rendererPriority)
		{
			if(fileName.empty())
			{
				return shared_ptr<GUIScene>();
			}
			
			shared_ptr<GUIScene> guiScene(new GUIScene());
			guiScene->mGUI.reset(new GUI(mFileSystem,mMaterialManager,mFontManager,mFunctionBinder));
			guiScene->mRenderer = CreateRenderer(rendererPriority);
			guiScene->mScene = guiScene->mGUI->LoadScreenGUIAndSetupScene(fileName,guiScene->mRenderer,*mInputManager);
			if(guiScene->mScene)
			{
				//Make sure the renderer has a viewport. It might not if the gui file doesn't specify a screen.
				if(!guiScene->mRenderer->GetViewport())
				{
					guiScene->mRenderer->SetViewport(shared_ptr<Viewport>(new Viewport()));
				}
				return guiScene;
			}
			
			//GUI::LoadMenu will output error messages.
			//Errors will be output during the attempt to load. No need to duplicate.
			return shared_ptr<GUIScene>();
		}
		
		bool Menu::SetCurrent(const std::string& newCurrent)
		{
			if(!GUIExists(newCurrent))
			{
				ECHO_LOG_ERROR("Menu \"" << newCurrent << "\" does not exist.");
				ECHO_LOG_ERROR("Available menus:");
				BOOST_FOREACH(const NamedFilePair& namedFile, mMenuFiles)
				{
					ECHO_LOG_ERROR(" - " << namedFile.first);
				}
				ECHO_LOG_ERROR("Preloaded menus:");
				BOOST_FOREACH(const NamedGUIPair& namedMenu, mPreloadedMenus)
				{
					ECHO_LOG_ERROR("- " << namedMenu.first);
				}
				return false;
			}

			shared_ptr<GUIScene> gui=GetOrLoadGUI(newCurrent);
			if(!gui)
			{
				ECHO_LOG_ERROR("Failed to get menu \"" << newCurrent << "\".");
				return false;
			}

			SetCurrent(gui);
			return true;
		}
		
		void Menu::SetCurrent(shared_ptr<GUIScene> newCurrent)
		{
			if(newCurrent==mCurrentScene)
			{
				return;
			}
			
			//Pause the current menu and the renderer.
			if(mCurrentScene)
			{
				//If it is preloaded then we'll just pause it. If not then we need to remove the renderer.
				mCurrentScene->mRenderer->Pause();
				mMultiRenderer->RemoveRenderer(mCurrentScene->mRenderer);
				mCurrentScene->mScene->Pause();
				RemoveTask(mCurrentScene->mScene);
			}
			
			mCurrentScene = newCurrent;

			if(mCurrentScene)
			{
				mMultiRenderer->AddRenderer(mCurrentScene->mRenderer);
				mCurrentScene->mRenderer->Resume();
				AddTask(mCurrentScene->mScene);
				mCurrentScene->mScene->Resume();
			}
		}
		
		void Menu::RegisterFunctions()
		{
			mFunctionBinder->Register("Menu.Push",bind(&Menu::Push,this,placeholders::_1),false,boost::fusion::vector<std::string>());
			mFunctionBinder->Register("Menu.Pop",bind(&Menu::Pop,this),false,boost::fusion::vector<>());
			mFunctionBinder->Register("Menu.PopTo",bind(&Menu::PopTo,this,placeholders::_1),false,boost::fusion::vector<std::string>());
			mFunctionBinder->Register("Menu.ClearStack",bind(&Menu::ClearStack,this),false,boost::fusion::vector<std::string>());
			mFunctionBinder->Register("Menu.QueuePush",bind(&Menu::QueuePush,this,placeholders::_1),false,boost::fusion::vector<std::string>());
			mFunctionBinder->Register("Menu.QueuePop",bind(&Menu::QueuePop,this),false,boost::fusion::vector<>());
			mFunctionBinder->Register("Menu.QueuePopTo",bind(&Menu::QueuePopTo,this,placeholders::_1),false,boost::fusion::vector<std::string>());
			mFunctionBinder->Register("Menu.QueueClearStack",bind(&Menu::QueueClearStack,this),false,boost::fusion::vector<std::string>());
		}
		
		void Menu::DeregisterFunctions()
		{
			mFunctionBinder->Deregister("Menu.Push");
			mFunctionBinder->Deregister("Menu.Pop");
			mFunctionBinder->Deregister("Menu.PopTo");
			mFunctionBinder->Deregister("Menu.ClearStack");
			mFunctionBinder->Deregister("Menu.QueuePush");
			mFunctionBinder->Deregister("Menu.QueuePop");
			mFunctionBinder->Deregister("Menu.QueuePopTo");
			mFunctionBinder->Deregister("Menu.QueueClearStack");
		}
		
		bool Menu::IsPreloaded(shared_ptr<GUIScene> gui)
		{
			NamedGUIMap::iterator it = mPreloadedMenus.begin();
			NamedGUIMap::iterator itEnd = mPreloadedMenus.begin();
			while(it!=itEnd)
			{
				if(it->second==gui)
				{
					return true;
				}
				++it;
			}
			return false;
		}
		
		shared_ptr<Renderer> Menu::CreateRenderer(u32 priority)
		{
			shared_ptr<Renderer> renderer= mMultiRenderer->CreateRenderer("",priority);
			renderer->SetClear(true);
			renderer->SetClearMask(RenderTarget::ClearMaskFlags::DEPTH);

			//Remove the renderer from the active renderers, it will be added later.
			mMultiRenderer->RemoveRenderer(renderer);
			
			return renderer;
		}
		
		void Menu::OnPause(bool applicationPause)
		{
			if(mCurrentScene)
			{
				mCurrentScene->mRenderer->Pause();
				mMultiRenderer->RemoveRenderer(mCurrentScene->mRenderer);
			}
		}
		
		void Menu::OnResume(bool applicationResume)
		{
			if(mCurrentScene)
			{
				mMultiRenderer->AddRenderer(mCurrentScene->mRenderer);
				mCurrentScene->mRenderer->Resume();
			}
		}
		
	}
}
