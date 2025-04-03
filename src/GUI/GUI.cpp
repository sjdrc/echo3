#include <echo/GUI/GUI.h>
#include <echo/GUI/Image.h>
#include <echo/GUI/Button.h>
#include <echo/GUI/Text.h>
#include <echo/GUI/Layout.h>
#include <echo/GUI/TextBox.h>
#include <echo/GUI/Screen.h>
#include <echo/GUI/Container.h>
#include <echo/GUI/Cursor.h>

#include <echo/FileSystem/FileSystem.h>
#include <echo/Resource/MaterialManager.h>
#include <echo/Resource/FontManager.h>
#include <echo/Input/Input.h>
#include <echo/Util/FunctionBinder.h>
#include <echo/Util/Capnp.h>
#include <echo/Graphics/Viewport.h>
#include <echo/Graphics/Renderer.h>
#include <echo/Graphics/Camera.h>
#include <echo/Graphics/RenderTarget.h>

#include <echo/cpp/functional>
#include <sstream>

namespace Echo
{
	namespace GUI
	{
		const f32 GUI::DEFAULT_VIEWPORT_ASPECT_RATIO=16.f/9.f;
		
		GUI::GUI(	shared_ptr<FileSystem> fileSystem,
					shared_ptr<MaterialManager> materialManager,
					shared_ptr<FontManager> fontManager,
					shared_ptr<FunctionBinder> functionBinder) :
			mFileSystem(fileSystem),
			mMaterialManager(materialManager),
			mFontManager(fontManager),
			mFunctionBinder(new FunctionBinder())
		{
			// Each GUI object will register its own functions to its local binder and
			// use the provided function binder for default handling. This allows us to
			// have a higher level FunctionBinder with a larger list of available
			// functions yet allow local calls as well.
			// 
			// A good example of a local call in a GUI context is the SetVisibilityByName()
			// function which uses the name to look up Elements. The function would be
			// overridden if the same object was shared by multiple GUI objects, but a local
			// binder allows for local name lookups in each GUI object while still sharing
			// functions from a shared binder such as "NotifyTheMotherShip()" or something.
			mFunctionBinder->SetFallback(functionBinder);
			
			//Set up some handy functions.
			mFunctionBinder->Register("GUI.SetVisible",bind(&GUI::SetVisibilityByName,this,placeholders::_1,placeholders::_2),false,boost::fusion::vector<std::string,bool>());
		}
		
		GUI::~GUI()
		{
			//Avoid any bad calls.
			mFunctionBinder->Deregister("GUI.SetVisible");
		}
	
		shared_ptr<Element> GUI::LoadGUI(const std::string& fileName)
		{
			if(!mFileSystem)
			{
				ECHO_LOG_ERROR("Unable to load \"" << fileName << "\". No FileSystem object has been given.");
				return shared_ptr<Element>();
			}
			
			File file = mFileSystem->Open(fileName);
			if(!file.IsOpen())
			{
				ECHO_LOG_ERROR("Could not open \"" << fileName << "\"");
				return shared_ptr<Element>();
			}
			ECHO_LOG_INFO("Loading \"" << fileName << "\".");
			
			std::string wholeFile;
			file.ReadFileIntoString(wholeFile);

			try
			{
				shared_ptr<CapnpMessageHelper> message = CreateCapnpReaderFromData<Resources::GUI>(wholeFile);
				if(!message)
				{
					ECHO_LOG_ERROR("Unable to create reader from \"" << fileName << "\"");
					return nullptr;
				}
				Resources::GUI::Reader resources = message->GetReader<Resources::GUI>();

				//Determine if we need to create a container
				shared_ptr<Element> rootElement;
				Size numElements = 0;
				if(resources.hasElement())
				{
					numElements = resources.getElement().size();
				}

				if(resources.hasGuiFile())
				{
					numElements += resources.getGuiFile().size();
				}

				if(numElements > 1)
				{
					mRootElement.reset(new Container());
					if(resources.hasElement())
					{
						auto elements = resources.getElement();
						for(Size i = 0; i < elements.size(); ++i)
						{
							LoadElement(elements[i],mRootElement,fileName);
						}
					}
					if(resources.hasGuiFile())
					{
						auto guiFiles = resources.getGuiFile();
						for(Size i = 0; i < guiFiles.size(); ++i)
						{
							shared_ptr<Element> element=LoadGUI( mFileSystem->ResolveFullNameForFileWithParent(guiFiles[i].cStr(),fileName) );
							if(element)
							{
								mRootElement->AddElement(element);
							}
						}
					}
				}else
				if(resources.hasElement() && resources.getElement().size()==1)
				{
					mRootElement = LoadElement(resources.getElement()[0],shared_ptr<Element>(),fileName);
				}else
				if(resources.hasGuiFile() && resources.getGuiFile().size()==1)
				{
					mRootElement = LoadGUI( mFileSystem->ResolveFullNameForFileWithParent(resources.getGuiFile()[0],fileName) );
				}else
				{
					ECHO_LOG_ERROR("Not elements found in \"" << fileName << "\"");
					return nullptr;
				}
				return mRootElement;
			}catch(...)
			{
			}
			ECHO_LOG_ERROR("Failed to load GUI: \"" << fileName << "\"");
			return nullptr;
		}

		shared_ptr<Scene> GUI::LoadScreenGUIAndSetupScene(const std::string& fileName, shared_ptr<Renderer> renderer, InputManager& inputManager)
		{
			if(!renderer)
			{
				ECHO_LOG_ERROR("Unable to load Screen because a Renderer has not been provided.");
				return shared_ptr<Scene>();
			}
			
			shared_ptr<Scene> scene=make_shared<Scene>();
			shared_ptr<Camera> camera = scene->CreateCamera("GUI");
			camera->SetProjectionType(ProjectionTypes::ORTHOGRAPHIC);
			camera->SetOrthoWindowWidth(1);
			camera->SetPosition(0,0,100);
			renderer->SetCamera(camera);
			scene->SetUseOnlyZForDistanceCalculations(true);

			if(!LoadScreenGUI(fileName, renderer, *scene, inputManager))
			{
				//Error messages are output in the load function.
				return shared_ptr<Scene>();
			}
			return scene;
		}

		shared_ptr<Screen> GUI::LoadScreenGUI(const std::string& fileName, shared_ptr<Renderer> renderer, Scene& scene, InputManager& inputManager)
		{
			if(!renderer)
			{
				ECHO_LOG_ERROR("Unable to load Screen because a Renderer has not been provided.");
				return shared_ptr<Screen>();
			}

			if(!mFileSystem)
			{
				ECHO_LOG_ERROR("Unable to load \"" << fileName << "\". No FileSystem object has been given.");
				return shared_ptr<Screen>();
			}
			
			File file = mFileSystem->Open(fileName);
			if(!file.IsOpen())
			{
				ECHO_LOG_ERROR("Could not open \"" << fileName << "\"");
				return shared_ptr<Screen>();
			}
			ECHO_LOG_INFO("Loading \"" << fileName << "\".");
			
			std::string wholeFile;
			file.ReadFileIntoString(wholeFile);
			
			try
			{
				shared_ptr<CapnpMessageHelper> message = CreateCapnpReaderFromData<Resources::Screen>(wholeFile);
				if(!message)
				{
					ECHO_LOG_ERROR("Unable to create reader from \"" << fileName << "\"");
					return nullptr;
				}
				Resources::Screen::Reader screenData = message->GetReader<Resources::Screen>();
				shared_ptr<Screen> screen = LoadScreen(screenData, renderer, scene, inputManager, fileName);
				if(screen)
				{
					scene.AddRenderable(screen);
					if(screenData.hasConditional())
					{
						auto conditionals = screenData.getConditional();
						for(Size i = 0; i < conditionals.size(); ++i)
						{
							auto conditional = conditionals[i];
							if(conditional.hasFunction())
							{
								auto functions = conditional.getFunction();
								for(Size f = 0; f < functions.size(); ++f)
								{
									screen->AddConditionalFunction(conditional.getCondition().cStr(),functions[f].cStr());
								}
							}
						}
					}
				}
				return screen;
			}catch(...)
			{}	
			ECHO_LOG_ERROR("Failed to parse input. \"" << fileName << "\"");
			return nullptr;
		}

		shared_ptr<Screen> GUI::LoadScreen(Resources::Screen::Reader screenData, shared_ptr<Renderer> renderer,
										 Scene& scene, InputManager& inputManager, const std::string& fileNameForRelativeLoading)
		{
			assert(renderer && "GUI::LoadScreen() has been called with a null renderer. Make sure the pointer is not null before calling.");
					
			shared_ptr<Screen> screen(new Screen());
			screen->SetFunctionBinder(mFunctionBinder);
			
			if(screenData.hasElement())
			{
				auto elements = screenData.getElement();
				if(elements.size()>0)
				{
					for(Size i=0; i < elements.size(); ++i)
					{
						LoadElement(elements[i],screen, fileNameForRelativeLoading);
					}
				}
			}
			if(screenData.hasGuiFile())
			{
				auto guiFiles = screenData.getGuiFile();
				for(Size i = 0; i < guiFiles.size(); ++i)
				{
					shared_ptr<Element> element = LoadGUI( mFileSystem->ResolveFullNameForFileWithParent(guiFiles[i].cStr(), fileNameForRelativeLoading) );
					if(element)
					{
						screen->AddElement(element);
					}
				}
			}
			if(screenData.hasViewport())
			{
				shared_ptr<Viewport> viewport = LoadViewport(screenData.getViewport(), *renderer);
				renderer->SetViewport(viewport);
			} else
			{
				if(!renderer->GetViewport())
				{
					//Create a default viewport if one doesn't exist.
					renderer->SetViewport(shared_ptr<Viewport>(new Viewport()));
				}
			}

			if(screenData.hasCursor() && screenData.getCursor().hasElement())
			{
				auto cursor = screenData.getCursor();
				shared_ptr<Element> cursorElement = LoadElement(cursor.getElement(),shared_ptr<Element>(), fileNameForRelativeLoading);
				if(cursorElement)
				{
					shared_ptr<Cursor> cursorTask(new Cursor(	inputManager.GetInput<bool>(cursor.getButton().cStr()),
															inputManager.GetInput<f32>(cursor.getXAxis().cStr()),
															inputManager.GetInput<f32>(cursor.getYAxis().cStr()),
															screen,renderer,cursorElement));
					screen->AddElement(cursorElement);
					scene.AddTask(cursorTask);
				}
			}
			mRootElement = screen;
			return screen;
		}
		
		shared_ptr<Element> GUI::LoadElement(Resources::Element::Reader elementData, shared_ptr<Element> parent, const std::string& fileNameForRelativeLoading)
		{
			shared_ptr<Element> element;
			switch(elementData.getType())
			{
				case Resources::ElementType::CONTAINER:
					element.reset(new Container());
					break;
				case Resources::ElementType::IMAGE:
					if(elementData.hasImageProperties())
					{
						element = LoadImage(elementData.getImageProperties(), fileNameForRelativeLoading);
					}else
					{
						ECHO_LOG_ERROR("Unable to create image element because image_properties are missing.");
					}
					break;
				case Resources::ElementType::TEXT:
					if(elementData.hasTextProperties())
					{
						element = LoadText(elementData.getTextProperties());
					}else
					{
						ECHO_LOG_ERROR("Unable to create text element because text_properties are missing.");
					}
					break;
				case Resources::ElementType::BUTTON:
					if(elementData.hasButtonProperties())
					{
						element = LoadButton(elementData.getButtonProperties(), fileNameForRelativeLoading);
					}else
					{
						ECHO_LOG_ERROR("Unable to create button element because button_properties are missing.");
					}
					break;
				case Resources::ElementType::LAYOUT:
					if(elementData.hasLayoutProperties())
					{
						element = LoadLayout(elementData.getLayoutProperties());
					}else
					{
						ECHO_LOG_ERROR("Unable to create layout element because layout_properties are missing.");
					}
					break;
				case Resources::ElementType::TEXTBOX:
					if(elementData.hasTextboxProperties())
					{
						element = LoadTextBox(elementData.getTextboxProperties());
					}else
					{
						ECHO_LOG_ERROR("Unable to create textbox element because textbox_properties are missing.");
					}
					break;
			}
			if(parent)
			{
				parent->AddElement(element);
			}
			
			ProcessElementProperties(elementData, element);
			
			if(element)
			{
				element->SetFunctionBinder(mFunctionBinder);

				if(elementData.hasElement())
				{
					auto elements = elementData.getElement();
					for(Size i = 0; i < elements.size(); ++i)
					{
						LoadElement(elements[i],element,fileNameForRelativeLoading);
					}
				}
				if(elementData.hasGuiFile())
				{
					auto guiFiles = elementData.getGuiFile();
					for(Size i = 0; i < guiFiles.size(); ++i)
					{
						shared_ptr<Element> guiElement = LoadGUI( mFileSystem->ResolveFullNameForFileWithParent(guiFiles[i].cStr(), fileNameForRelativeLoading) );
						if(guiElement)
						{
							element->AddElement(guiElement);
						}
					}
				}
			}
			return element;
		}
		
		shared_ptr<Element> GUI::LoadButton(Resources::ButtonProperties::Reader buttonData, std::string fileNameForRelativeLoading)
		{
			if(!mMaterialManager)
			{
				ECHO_LOG_ERROR("Unable to create button because a MaterialManager has not been provided.");
				return shared_ptr<Element>();
			}
			shared_ptr<Material> idle = buttonData.hasIdleMaterial() ? mMaterialManager->GetResource(buttonData.getIdleMaterial().cStr(), fileNameForRelativeLoading) : nullptr;
			shared_ptr<Material> pressed = buttonData.hasPressedMaterial() ? mMaterialManager->GetResource(buttonData.getPressedMaterial().cStr(), fileNameForRelativeLoading) : nullptr;
			shared_ptr<Material> released = buttonData.hasReleasedMaterial() ? mMaterialManager->GetResource(buttonData.getReleasedMaterial().cStr(), fileNameForRelativeLoading) : nullptr;
			shared_ptr<Material> hover = buttonData.hasHoverMaterial() ? mMaterialManager->GetResource(buttonData.getHoverMaterial().cStr(), fileNameForRelativeLoading) : nullptr;
			shared_ptr<Button> button(new Button(idle));
			button->SetPressed(pressed);
			button->SetReleased(released);
			button->SetHover(hover);
			return button;
		}
		
		shared_ptr<Element> GUI::LoadImage(Resources::ImageProperties::Reader imageData, std::string fileNameForRelativeLoading)
		{
			if(!mMaterialManager)
			{
				ECHO_LOG_ERROR("Unable to create button because a MaterialManager has not been provided.");
				return shared_ptr<Element>();
			}
			shared_ptr<Material> material = mMaterialManager->GetResource(imageData.getMaterial().cStr(), fileNameForRelativeLoading);
			shared_ptr<Image> image(new Image(material));
			image->SetFlipHorizontally(imageData.getFlipHorizontally());
			image->SetFlipVertically(imageData.getFlipVertically());
			TextureUV minUV;
			TextureUV maxUV;
			minUV.u = imageData.getMinU();
			minUV.v = imageData.getMinV();
			maxUV.u = imageData.getMaxU();
			maxUV.v = imageData.getMaxV();
			image->SetMinUV(minUV);
			image->SetMaxUV(maxUV);
			return image;
		}
		
		shared_ptr<Element> GUI::LoadText(Resources::TextProperties::Reader textData)
		{
			if(!mFontManager)
			{
				ECHO_LOG_ERROR("Unable to create text because a FontManager has not been provided.");
				return shared_ptr<Element>();
			}
			
			if(!mFontManager->ResourceExists(textData.getFont().cStr()))
			{
				ECHO_LOG_ERROR("Unable to create text because the specified font does not exist.");
				return shared_ptr<Element>();
			}
			Scalar pointSize=textData.getPointSize();
			ECHO_LOG_INFO("Creating Text object with font ." << textData.getFont().cStr() << " : " << pointSize);
			shared_ptr<Text> text(new Text(textData.getText().cStr(), mFontManager));
			text->SetFont(textData.getFont().cStr(),pointSize);
			return text;
		}
		
		shared_ptr<Element> GUI::LoadLayout(Resources::LayoutProperties::Reader layoutData)
		{
			shared_ptr<Layout> layout(new Layout());
			Layout::Direction direction = Layout::Directions::HORIZONTAL_MIN_TO_MAX;
			switch(layoutData.getDirection())
			{
				case Resources::LayoutProperties::Direction::HORIZONTAL_MIN_TO_MAX: direction = Layout::Directions::HORIZONTAL_MIN_TO_MAX;	break;
				case Resources::LayoutProperties::Direction::HORIZONTAL_MAX_TO_MIN: direction = Layout::Directions::HORIZONTAL_MAX_TO_MIN;	break;
				case Resources::LayoutProperties::Direction::VERTICAL_MIN_TO_MAX: direction = Layout::Directions::VERTICAL_MIN_TO_MAX;		break;
				case Resources::LayoutProperties::Direction::VERTICAL_MAX_TO_MIN: direction = Layout::Directions::VERTICAL_MAX_TO_MIN;		break;
			}
			layout->SetDirection(direction);
			Layout::PlacementMode placementMode = Layout::PlacementModes::FIT_CONTENTS;
			switch(layoutData.getPlacementMode())
			{
				case Resources::LayoutProperties::PlacementMode::FIT_CONTENTS: placementMode = Layout::PlacementModes::FIT_CONTENTS;	break;
				case Resources::LayoutProperties::PlacementMode::ALLOW_OVERFLOW: placementMode = Layout::PlacementModes::ALLOW_OVERFLOW;	break;
			}
			layout->SetPlacementMode(placementMode);
			layout->SetUseOptimalChildSettings(layoutData.getUseOptimalSettings());
			return layout;
		}

		shared_ptr<Element> GUI::LoadTextBox(Resources::TextBoxProperties::Reader textboxData)
		{
			if(!mFontManager)
			{
				ECHO_LOG_ERROR("Unable to create textbox because a FontManager has not been provided.");
				return shared_ptr<Element>();
			}
			if(!mFontManager->ResourceExists(textboxData.getFont().cStr()))
			{
				ECHO_LOG_ERROR("Unable to create text because the specified font does not exist.");
				return shared_ptr<Element>();
			}

			Scalar pointSize=textboxData.getPointSize();
			shared_ptr<TextBox> textbox(new TextBox(textboxData.getText().cStr(), mFontManager));
			textbox->SetLineSize(textboxData.getLineSize());
			textbox->SetFont(textboxData.getFont().cStr(),pointSize);
			return textbox;
		}
		
		void GUI::ProcessElementProperties(Resources::Element::Reader elementData, shared_ptr<Element> targetElement)
		{
			if(!targetElement)
			{
				return;
			}
			
			f32 x = elementData.getX();
			f32 y = elementData.getY();
			f32 z = elementData.getZ();
			
			if(elementData.hasName())
			{
				targetElement->SetName(elementData.getName());
				AddNamedElement(targetElement);
			}
			
			targetElement->SetTargetable(elementData.getTargetable());
			
			// Flag when set true will cause a warning to be displayed when a size mode is used that uses
			// the specified aspect ratio but it isn't supplied.
			bool checkForMissingAspectRatio = false;
			Element::SizeMode sizeMode = Element::SizeModes::STRETCH;
			switch(elementData.getSizeMode())
			{
				case Resources::SizeMode::STRETCH: sizeMode = Element::SizeModes::STRETCH; break;
				case Resources::SizeMode::FILL: sizeMode = Element::SizeModes::FILL; break;
				case Resources::SizeMode::WIDTH_AND_CALCULATE_ASPECT_RATIO: sizeMode = Element::SizeModes::WIDTH_AND_CALCULATE_ASPECT_RATIO; break;
				case Resources::SizeMode::HEIGHT_AND_CALCULATE_ASPECT_RATIO: sizeMode = Element::SizeModes::HEIGHT_AND_CALCULATE_ASPECT_RATIO; break;
				case Resources::SizeMode::WIDTH_WITH_SET_ASPECT_RATIO:
					sizeMode = Element::SizeModes::WIDTH_WITH_SET_ASPECT_RATIO;
					checkForMissingAspectRatio = true;
				break;
				case Resources::SizeMode::HEIGHT_WITH_SET_ASPECT_RATIO:
					sizeMode = Element::SizeModes::HEIGHT_WITH_SET_ASPECT_RATIO;
					checkForMissingAspectRatio = true;
				break;
				case Resources::SizeMode::FILL_WITH_SET_ASPECT_RATIO:
					sizeMode = Element::SizeModes::FILL_WITH_SET_ASPECT_RATIO;
					checkForMissingAspectRatio = true;
					break;
				case Resources::SizeMode::MAX_FIT_WITH_SET_ASPECT_RATIO:
					sizeMode = Element::SizeModes::MAX_FIT_WITH_SET_ASPECT_RATIO;
					checkForMissingAspectRatio = true;
				break;
			}
			targetElement->SetSizeMode(sizeMode);
			
			f32 aspectRatio = elementData.getAspectRatio();
			if(aspectRatio==0)
			{
				if(checkForMissingAspectRatio)
				{
					ECHO_LOG_WARNING("The specified size mode depends on an aspect ratio but the aspect ratio was not supplied. The default of 1 will be used.");
				}
				aspectRatio = 1;
			}
			targetElement->SetAspectRatio(elementData.getAspectRatio());
			
			switch(elementData.getHAlign())
			{
				case Resources::Alignment::MINIMUM:
					targetElement->SetHorizontalAlignment(Element::Alignments::MINIMUM);
				break;
				case Resources::Alignment::MAXIMUM:
					targetElement->SetHorizontalAlignment(Element::Alignments::MAXIMUM);
				break;
				case Resources::Alignment::CENTRE:
				default:
					targetElement->SetHorizontalAlignment(Element::Alignments::CENTRE);
				break;
			}

			switch(elementData.getVAlign())
			{
				case Resources::Alignment::MINIMUM:
					targetElement->SetVerticalAlignment(Element::Alignments::MINIMUM);
				break;
				case Resources::Alignment::MAXIMUM:
					targetElement->SetVerticalAlignment(Element::Alignments::MAXIMUM);
				break;
				case Resources::Alignment::CENTRE:
				default:
					targetElement->SetVerticalAlignment(Element::Alignments::CENTRE);
				break;
			}

			Colour colour = elementData.hasColour() ? Colour(std::string(elementData.getColour().cStr())) : Colours::WHITE;
			targetElement->SetColour(colour);
			targetElement->SetInheritParentColour(elementData.getInheritParentColour());
			targetElement->SetInheritParentAlpha(elementData.getInheritParentAlpha());

			float width = elementData.getWidth()!=0 ? elementData.getWidth() : 1.f;
			float height = elementData.getHeight()!=0 ? elementData.getHeight() : 1.f;
			
			//Spit out some warnings
			if(elementData.getWidth()!=0 && elementData.hasPhysicalWidth())
			{
				ECHO_LOG_WARNING("Element data contains width and physical_width parameters. Using physical_width.");
			}
			if(elementData.getHeight()!=0 && elementData.hasPhysicalHeight())
			{
				ECHO_LOG_WARNING("Element data contains height and physical_width parameters. Using physical_height.");
			}

			if(elementData.hasPhysicalWidth())
			{
				f64 inPixels=0;
				if(GetPixelMeasurment(elementData.getPhysicalWidth().cStr(),inPixels))
				{
					targetElement->SetWidthInPixels(inPixels);
				}else
				{
					targetElement->SetPhysicalWidth(StringToLength<Metre>(elementData.getPhysicalWidth().cStr()));
				}
			} else
			{
				targetElement->SetWidth(width);
			}

			if(elementData.hasPhysicalHeight())
			{
				f64 inPixels=0;
				if(GetPixelMeasurment(elementData.getPhysicalHeight().cStr(),inPixels))
				{
					targetElement->SetHeightInPixels(inPixels);
				}else
				{
					targetElement->SetPhysicalHeight(StringToLength<Metre>(elementData.getPhysicalHeight().cStr()));
				}
			} else
			{
				targetElement->SetHeight(height);
			}

			targetElement->SetRelativePosition(x,y,z);
			targetElement->SetVisibilityFlag(elementData.getVisible());
			
			if(elementData.hasEnterFunction())
			{
				auto enterFunctions = elementData.getEnterFunction();
				for(Size i = 0; i < enterFunctions.size(); ++i)
				{
					targetElement->AddEnterFunction(enterFunctions[i].cStr());
				}
			}

			if(elementData.hasHoverFunction())
			{
				auto hoverFunctions = elementData.getHoverFunction();
				for(Size i = 0; i < hoverFunctions.size(); ++i)
				{
					targetElement->AddHoverFunction(hoverFunctions[i].cStr());
				}
			}

			if(elementData.hasLeaveFunction())
			{
				auto leaveFunctions = elementData.getLeaveFunction();
				for(Size i = 0; i < leaveFunctions.size(); ++i)
				{
					targetElement->AddLeaveFunction(leaveFunctions[i].cStr());
				}
			}

			if(elementData.hasPressedFunction())
			{
				auto pressedFunctions = elementData.getPressedFunction();
				for(Size i = 0; i < pressedFunctions.size(); ++i)
				{
					targetElement->AddPressedFunction(pressedFunctions[i].cStr());
				}
			}

			if(elementData.hasHoldFunction())
			{
				auto holdFunctions = elementData.getHoldFunction();
				for(Size i = 0; i < holdFunctions.size(); ++i)
				{
					targetElement->AddHoldFunction(holdFunctions[i].cStr());
				}
			}

			if(elementData.hasReleasedFunction())
			{
				auto releasedFunctions = elementData.getReleasedFunction();
				for(Size i = 0; i < releasedFunctions.size(); ++i)
				{
					targetElement->AddReleasedFunction(releasedFunctions[i].cStr());
				}
			}

			if(elementData.hasConditional())
			{
				auto conditionals = elementData.getConditional();
				for(Size i = 0; i < conditionals.size(); ++i)
				{
					auto conditional = conditionals[i];
					if(conditional.hasFunction())
					{
						auto functions = conditional.getFunction();
						for(Size f = 0; f < functions.size(); ++f)
						{
							targetElement->AddConditionalFunction(conditional.getCondition().cStr(),functions[f].cStr());
						}
					}
				}
			}
			
			if(elementData.hasProperty())
			{
				auto properties = elementData.getProperty();
				for(Size i = 0; i < properties.size(); ++i)
				{
					std::string property = properties[i].cStr();
					Size dividerPos = property.find_first_of(':');
					if(dividerPos!=std::string::npos)
					{
						std::string propertyName = property.substr(0,dividerPos);
						std::string propertyValue = property.substr(dividerPos+1);
						targetElement->AddProperty(propertyName,propertyValue);
					} else
					{
						targetElement->AddProperty<bool>(property,"true");
					}
				}
			}
		}
		
		shared_ptr<Viewport> GUI::LoadViewport(Resources::Viewport::Reader viewportData, Renderer& renderer)
		{
			f32 left = viewportData.getLeft();
			f32 top = viewportData.getTop();
			f32 right = viewportData.getRight();
			f32 bottom = viewportData.getBottom();
			
			float aspectRatio = DEFAULT_VIEWPORT_ASPECT_RATIO;
			if(viewportData.getDisplayAspectRatio()!=0.f)
			{
				aspectRatio = viewportData.getDisplayAspectRatio();
			}else
			{
				//Try and get it from the render target.
				if(renderer.GetRenderTarget())
				{
					aspectRatio = renderer.GetRenderTarget()->GetAspectRatio();
				}else
				if(renderer.GetCamera())
				{
					aspectRatio = renderer.GetCamera()->GetAspectRatio();
				}else
				{
					ECHO_LOG_WARNING("I could not determine an aspect ratio from the data or automatically view the render target or camera." <<
							" Using a default of "  << aspectRatio << "." <<
							" To avoid this warning in the future, make sure your viewport data contains an aspect ratio or the renderer is configured before loading a gui file.");
				}
			}
			
			if(viewportData.getMode()!=Resources::SizeMode::STRETCH)
			{
				switch(viewportData.getMode())
				{
					case Resources::SizeMode::WIDTH_WITH_SET_ASPECT_RATIO:
					{
						shared_ptr<Viewport> viewport(new Viewport());
						float verticalCentre = viewportData.getVerticalCentre();
						viewport->SetAsHorizontal(aspectRatio,left,right,verticalCentre);
						return viewport;
					}
					case Resources::SizeMode::HEIGHT_WITH_SET_ASPECT_RATIO:
					{
						shared_ptr<Viewport> viewport(new Viewport());
						float horizontalCentre = viewportData.getHorizontalCentre();
						viewport->SetAsVertical(aspectRatio,top,bottom,horizontalCentre);
						return viewport;
					}
					case Resources::SizeMode::MAX_FIT_WITH_SET_ASPECT_RATIO:
					{
						shared_ptr<Viewport> viewport(new Viewport());
						viewport->SetMaxFit(aspectRatio,left,top,right,bottom);
						return viewport;
					}
					case Resources::SizeMode::STRETCH:
						//This is the default.
						break;
					case Resources::SizeMode::FILL:
					case Resources::SizeMode::WIDTH_AND_CALCULATE_ASPECT_RATIO:
					case Resources::SizeMode::HEIGHT_AND_CALCULATE_ASPECT_RATIO:
					case Resources::SizeMode::FILL_WITH_SET_ASPECT_RATIO:
						ECHO_LOG_WARNING("Unsupported viewport mode specified.");
					break;
				}
			}else
			{
				return shared_ptr<Viewport>(new Viewport(left,top,right,bottom));
			}
			return shared_ptr<Viewport>();
		}

		void GUI::SetVisibilityByName(const std::string& elementName, bool visibility)
		{
			NamedElementsMap::iterator it = mNamedElements.find(elementName);
			if(it!=mNamedElements.end())
			{
				it->second->SetVisibilityFlag(visibility);
			}else
			{
				ECHO_LOG_WARNING("Unable to set visibility of unknown Element: " << elementName);
			}
		}

		shared_ptr<Element> GUI::FindElement(const std::string& elementName) const
		{
			NamedElementsMap::const_iterator it = mNamedElements.find(elementName);
			if(it!=mNamedElements.end())
			{
				return it->second;
			}
			return nullptr;
		}
		
		std::list< shared_ptr<Element> > GUI::FindElements(const std::string& elementName) const
		{
			if(mRootElement)
			{
				return mRootElement->FindElements(elementName);
			}
			return std::list< shared_ptr<Element> >();
		}

		void GUI::AddNamedElement(shared_ptr<Element> element)
		{
			if(element && !element->GetName().empty())
			{
				mNamedElements.insert(std::make_pair(element->GetName(),element));
			}
		}
	}
}
