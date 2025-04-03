#ifndef _ECHOMENU_H_
#define _ECHOMENU_H_

#include <echo/Kernel/TaskGroup.h>
#include <map>
#include <stack>

namespace Echo
{
	class FileSystem;
	class MaterialManager;
	class FontManager;
	class InputManager;
	class MultiRenderer;
	class Renderer;
	class Viewport;
	class FunctionBinder;
	class Scene;
	
	namespace GUI
	{
		class GUI;
		
		/**
		 * Menu manages the activation of Scene objects as menu screens.
		 * Menu maintains a stack. Only one scene is active at a time.
		 * Use Push() and Pop() menus to navigate.
		 * 
		 * The Menu object will take a FunctionBinder to use as a fallback for the one it creates.
		 * The following functions are bound to the binder as:
		 *		Menu.Push				QueuePush
		 *		Menu.Pop				QueuePop
		 *		Menu.PopTo				QueuePopTo
		 *		Menu.ClearStack			QueueClearStack
		 * The queue methods are bound because pushing and popping scenes during an update can
		 * cause deletion of objects part way through their execution. The Queue methods ensure
		 * the action is called after the update is complete. @see QueuePush() for more information.
		 *
		 * There are two ways to add a menu scene:
		 * - Build your own Scene and then add it with AddScene() or
		 * - Add a GUI file which will later be loaded and a Scene will be created for it.
		 */
		class Menu : public TaskGroup
		{
		public:
			Menu(shared_ptr<FileSystem> fileSystem,
				shared_ptr<MaterialManager> materialManager,
				shared_ptr<FontManager> fontManager,
				shared_ptr<InputManager> inputManager,
				shared_ptr<MultiRenderer> multiRenderer,
				shared_ptr<FunctionBinder> functionBinder = shared_ptr<FunctionBinder>());
			virtual ~Menu();
			
			struct GUISettings
			{
				GUISettings(std::string file, u32 rendererPriority = 5000) :
					mFile(file),
					mRendererPriority(rendererPriority){}
				std::string mFile;
				u32 mRendererPriority;
			};
			typedef std::stack< std::string > GUIStack;
			typedef std::map< std::string, GUISettings > NamedFileMap;
			typedef std::pair< std::string, GUISettings > NamedFilePair;
			
			/**
			 * Load a list of GUI files and add each to the menu.
			 * The list file is just a text file where each line is:
			 *  menu=GUIFile.gui[,NameToReference[,preload]]
			 * Where the parameters correspond to each in AddGUI():
			 *  - The filename can be absolute or relative to the file.
			 *  - NameToReference, optional, is the name to use to reference.
			 *  - preload indicates whether or not to preload the menu. This parameter
			 *	is optional after NameToReference and requires NameToReference to preceed
			 *  it.
			 * The file can also contain a line specifying the active menu. Only the first
			 * line found is used.
			 *  active=Name
			 * Where Name is the NameToReference as described above or the filename if no
			 * NameToReference was given.
			 */
			bool LoadGUIList(const std::string& fileName);

			/**
			 * Add a GUI file to load in the future.
			 */
			bool AddGUI(const std::string& fileName, std::string name = "", bool preload = false, u32 rendererPriority = 5000);
			
			/**
			 * Add an existing Scene.
			 */
			bool AddScene(shared_ptr<Scene> scene, std::string name = "", u32 rendererPriority = 5000);
	
			/**
			 * Push the specified scene onto the stack.
			 * @note See QueuePush() for information on why you might need to use QueuePush().
			 * @param name the menu to push onto the stack.
			 */
			void Push(const std::string& name);
			/**
			 * Pop the current scene onto the stack.
			 * @note See QueuePop() for information on why you might need to use QueuePop().
			 * If there isn't a current scene then no action occurs.
			 */
			void Pop();
			
			/**
			 * Attempts to pop the stack until the scene with the specified name is found.
			 * @note See QueuePopTo() for information on why you might need to use QueuePopTo().
			 * @name the name of the scene to pop back to.
			 */
			void PopTo(const std::string& name);
			
			/**
			 * Clear the stack.
			 * @note See QueueClearStack() for information on why you might need to use QueueClearStack().
			 * This method deactivates the current scene and clears the stack.
			 */
			void ClearStack();

			/**
			 * Queue a push action to occur at the end of the update.
			 * @note This requests a push which will occur at the Menu's current or next update. Use this rather
			 * than taking immediate action because the action is often executed though the function binder from some
			 * Menu element's event response. Depending on whether unloadInactiveMenus is set to true or how one has
			 * structured an externally added scene, the result of pushing immediately might cause the destruction of
			 * elements part way through an update. It is generally safe to push and pop immediately as long as the
			 * Menu is not part way through updating.
			 * @param name the name of the menu to push onto the stack.
			 */
			void QueuePush(const std::string& name);

			/**
			 * Queue a pop.
			 * @see QueuePush() for more information on when to queue an action.
			 */
			void QueuePop();

			/**
			 * Queue a PopTo action.
			 * @see QueuePush() for more information on when to queue an action.
			 */
			void QueuePopTo(const std::string& name);

			/**
			 * Queue a ClearStack() action.
			 * @see QueuePush() for more information on when to queue an action.
			 */
 			void QueueClearStack();
			
			/**
			 * Set whether to unload inactive menus or not.
			 * If disabled menus are unloaded when they become inactive, but only if the menu is not in the preload list.
			 * A menu will be put into the preload list if:
			 *	- The preload flag is true in a call to AddGUI(std::string).
			 *  - The GUI object was added via AddGUI(shared_ptr<GUI>)
			 *  - This setting was false when a menu is activated.
			 */
			void SetUnloadInactiveMenus(bool unloadInactiveMenus);
			
			/**
			 * Get the current GUI object.
			 * @return The current GUI object if one is set, otherwise nullptr.
			 */
			shared_ptr<GUI> GetCurrentGUI() const;
			
			virtual void OnPause(bool applicationPause) override;
			virtual void OnResume(bool applicationResume) override;
		private:
			struct GUIScene
			{
				shared_ptr<GUI> mGUI;
				shared_ptr<Scene> mScene;
				shared_ptr<Renderer> mRenderer;
			};
			typedef std::map< std::string, shared_ptr<GUIScene> > NamedGUIMap;
			typedef std::pair< std::string, shared_ptr<GUIScene> > NamedGUIPair;
			bool GUIExists(const std::string& name);
			shared_ptr<GUIScene> GetOrLoadGUI(const std::string& name);
			shared_ptr<GUIScene> LoadGUI(const std::string& fileName, u32 rendererPriority);
			void SetCurrent(shared_ptr<GUIScene> newCurrent);
			bool SetCurrent(const std::string& newCurrent);
			
			void RegisterFunctions();
			void DeregisterFunctions();
			
			bool IsPreloaded(shared_ptr<GUIScene> gui);
			shared_ptr<Renderer> CreateRenderer(u32 priority);
			
			shared_ptr<FileSystem> mFileSystem;
			shared_ptr<InputManager> mInputManager;
			shared_ptr<MaterialManager> mMaterialManager;
			shared_ptr<FontManager> mFontManager;
			shared_ptr<MultiRenderer> mMultiRenderer;
			shared_ptr<FunctionBinder> mFunctionBinder;
			
			GUIStack mStack;						/// The menu stack.
			NamedGUIMap mPreloadedMenus;			/// Used to prevent menus added with AddGUI(ptr) from unloading.
			NamedFileMap mMenuFiles;				/// List of menu files mapped to the menu name for lookup to load.
			bool mUnloadInactiveMenus;			/// @see SetUnloadInactiveMenus().
			shared_ptr<GUIScene> mCurrentScene;
		};
	}
}
#endif 
