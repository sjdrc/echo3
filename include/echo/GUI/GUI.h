#ifndef _ECHOGUI_H_
#define _ECHOGUI_H_

#include <echo/Types.h>
#include <echo/Graphics/Scene.h>
#include <echo/Schema/GUI.capnp.h>
#include <map>

namespace Echo
{
	class FileSystem;
	class MaterialManager;
	class FontManager;
	class InputManager;
	class Renderer;
	class Viewport;
	class FunctionBinder;
	
	namespace GUI
	{
		class Element;
		class Screen;
		
		/**
		 * GUI is designed to make it easy to build GUIs by loading definitions in from .gui files.
		 * The GUI object will take a FunctionBinder to use as a fallback for the one it creates.
		 * The following functions are bound to the binder as:
		 *		GUI.SetVisible			SetVisibilityByName
		 */
		class GUI
		{
		public:
			typedef std::pair< std::string, shared_ptr<Element> > NamedElementPair;
			typedef std::map< std::string, shared_ptr<Element> > NamedElementsMap;
			
			GUI(shared_ptr<FileSystem> fileSystem,
				shared_ptr<MaterialManager> materialManager,
				shared_ptr<FontManager> fontManager,
				shared_ptr<FunctionBinder> functionBinder = shared_ptr<FunctionBinder>());
			virtual ~GUI();
			
			/**
			 * Loads a GUI file containing elements.
			 * @note The loaded elements use the function binder provided by the GUI object.
			 * @param fileName The name of the GUI file that you want to load.
			 * @return If loading was successful the element returned is the root element, otherwise a null
			 * pointer, see the log output for details.
			 */
			shared_ptr<Element> LoadGUI(const std::string& fileName);
			
			/**
			 * Load a screen from a gui file.
			 * Screen is a specialised element that requires a renderer and a scene. It usually specifies
			 * a viewport and cursor. The viewport will be created and used as the Renderer's viewport.
			 * This method does not create a camera for the renderer so one needs to be created.
			 * @param fileName The name of the .gui file containing the screen element.
			 * @param renderer The renderer that will receive the viewport, cannot be null.
			 * @param scene The scene the screen will be added to for rendering.
			 * @param inputManager The input manager to provide input to the cursor.
			 * @return the new screen on success, otherwise a null pointer, see the log output for details.
			 */
			shared_ptr<Screen> LoadScreenGUI(const std::string& fileName, shared_ptr<Renderer> renderer, Scene& scene, InputManager& inputManager);

			/**
			 * Load a Screen gui file and set up a new scene.
			 * This is method creates a new Scene object and camera. The camera is assigned to the renderer and then these parameters
			 * are used when calling LoadScreenGUI() to get the final result.
			 * @see LoadScreenGUI()
			 * @param fileName The name of the .gui file containing the screen element.
			 * @param renderer The renderer that will receive the camera and viewport, cannot be null.
			 * @param inputManager The input manager to provide input to the cursor.
			 * @return 
			 */
			shared_ptr<Scene> LoadScreenGUIAndSetupScene(const std::string& fileName, shared_ptr<Renderer> renderer, InputManager& inputManager);

			/**
			 * Set the visibility of an object by name.
			 * This method is bound to the internal function binder.
			 * @param elementName The name of the GUI element. If the element is not found a message logged.
			 * @param visibility Whether or not to make the element visible.
			 */
			void SetVisibilityByName(const std::string& elementName, bool visibility);

			/**
			 * Get an element by name.
			 * @param elementName The name of the element you want to find.
			 * @return If the element was found, the Element whose name is elementName.
			 */
			shared_ptr<Element> FindElement(const std::string& elementName) const;

			/**
			 * Get all elements matching the specified name.
			 * @param elementName The name of the element you want to find.
			 * @return A list of all elements with the matching name.
			 */
			std::list< shared_ptr<Element> > FindElements(const std::string& elementName) const;
			
			/**
			 * Find an Element and attempt to convert the pointer to a specialised type.
			 * @param elementName
			 * @return 
			 */
			template< class ElementType >
			shared_ptr<ElementType> FindElement(const std::string& elementName) const
			{
				NamedElementsMap::const_iterator it = mNamedElements.find(elementName);
				if(it!=mNamedElements.end())
				{
					return dynamic_pointer_cast<ElementType>(it->second);
				}
				return nullptr;
			}

			shared_ptr<Element> GetRootElement() const {return mRootElement;}
			const NamedElementsMap& GetNamedElementsMap() const {return mNamedElements;}
		private:
			shared_ptr<Screen> LoadScreen(Resources::Screen::Reader screenData, shared_ptr<Renderer> renderer,
											Scene& scene, InputManager& inputManager, const std::string& fileNameForRelativeLoading);
			shared_ptr<Element> LoadElement(Resources::Element::Reader elementData, shared_ptr<Element> parent,
											const std::string& fileNameForRelativeLoading);
			
			shared_ptr<Element> LoadButton(Resources::ButtonProperties::Reader buttonData, std::string fileNameForRelativeLoading);
			shared_ptr<Element> LoadImage(Resources::ImageProperties::Reader imageData, std::string fileNameForRelativeLoading);
			shared_ptr<Element> LoadText(Resources::TextProperties::Reader textData);
			shared_ptr<Element> LoadLayout(Resources::LayoutProperties::Reader layoutData);
			shared_ptr<Element> LoadTextBox(Resources::TextBoxProperties::Reader textboxData);
			void ProcessElementProperties(Resources::Element::Reader elementData, shared_ptr<Element> targetElement);
			shared_ptr<Viewport> LoadViewport(Resources::Viewport::Reader viewportData, Renderer& renderer);
			void AddNamedElement(shared_ptr<Element> element);

			static const f32 DEFAULT_VIEWPORT_ASPECT_RATIO;		/// This default is used when a viewport message doesn't contain an aspect ratio.
			shared_ptr<FileSystem> mFileSystem;
			shared_ptr<MaterialManager> mMaterialManager;
			shared_ptr<FontManager> mFontManager;
			shared_ptr<FunctionBinder> mFunctionBinder;
			NamedElementsMap mNamedElements;
			shared_ptr< Element > mRootElement;
		};
	}
}
#endif 
