#include <echo/Platform.h>
#include <echo/Platforms/GL/GLRenderTexture.h>
#include <echo/Platforms/GTK/GTKWindow.h>
#include <echo/Platforms/GTK/GTKExecutionModel.h>
#include <echo/Platforms/GTK/GTKMouse.h>
#include <echo/Platforms/GTK/GTKKeyboard.h>
#include <echo/Input/Input.h>

#include <X11/Xlib.h>

namespace Echo
{
	namespace Platform
	{
		//Get the global shared_ptr to Gtk::Main
		shared_ptr<Gtk::Main> GetGTKMain()
		{
			static shared_ptr<Gtk::Main> gtkMain;
			if(!gtkMain)
			{
				XInitThreads();
				gtkMain.reset(new Gtk::Main(0,0));
			}
			return gtkMain;
		}
		
		shared_ptr<ExecutionModel> CreateExecutionModel()
		{
			return make_shared<GTKExecutionModel>(GetGTKMain());
		}
		
		shared_ptr<RenderTarget> CreateRenderTarget(const std::string& type, const std::string& name, Kernel& kernel, u32 width, u32 height, u8 bpp, bool fullScreen)
		{
			if(type=="Window")
			{
				shared_ptr<GLContext> context(new GLContext());
				return shared_ptr<GTKWindow>(new GTKWindow(context, kernel,name,width,height,bpp,fullScreen,GetGTKMain()));
			}
			if(type=="Texture")
			{
				shared_ptr<GLContext> context(new GLContext());
				return shared_ptr<GLRenderTexture>(new GLRenderTexture(context,name,width,height,Texture::Formats::R8G8B8A8));
			}
			return shared_ptr<RenderTarget>();
		}

		shared_ptr<InputManager> CreateDefaultInputManager(shared_ptr<RenderTarget> target)
		{
			shared_ptr<InputManager> inputManager = make_shared<InputManager>();
			if(target)
			{
				shared_ptr<GTKWindow> window = dynamic_pointer_cast<GTKWindow,RenderTarget>(target);
				if(window)
				{
					shared_ptr<GTKMouse> mouse = make_shared<GTKMouse>(window);
					if(mouse)
					{
						inputManager->InstallDevice("Mouse",mouse);
					}

					shared_ptr<GTKKeyboard> keyboard = make_shared<GTKKeyboard>(window);
					if(keyboard)
					{
						inputManager->InstallDevice("Keyboard",keyboard);
					}
				}else
				{
					ECHO_LOG_WARNING("Unable to create GTKMouse and GTKKeyboard input devices as the provided RenderTarget does not appear to be a GTKWindow");
				}
			}
			return inputManager;
		}
	}
}
