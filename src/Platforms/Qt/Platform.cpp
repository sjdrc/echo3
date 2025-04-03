#include <echo/Platform.h>
#include <echo/Platforms/GL/GLRenderTarget.h>
#include <echo/Platforms/Qt/QtWindow.h>
#include <echo/Platforms/Qt/QtExecutionModel.h>
#include <echo/Platforms/Qt/QtMouse.h>
#include <echo/Platforms/Qt/QtKeyboard.h>
#include <echo/Util/FunctionBinder.h>
#include <echo/Input/Input.h>
#include <echo/cpp/functional>
#include <QtGui/QGuiApplication>

#include <X11/Xlib.h>

namespace Echo
{
	namespace Platform
	{
		shared_ptr<QGuiApplication> GetQtGuiApplication()
		{
			struct CoreWithFakeArgs
			{
				shared_ptr<QGuiApplication> core;
				std::string mFakeName;
				int argc;
				char** args;
			};
			static CoreWithFakeArgs* coreWithFakeArgs = nullptr;
			if(!coreWithFakeArgs)
			{
				XInitThreads();
				coreWithFakeArgs = new CoreWithFakeArgs;
				coreWithFakeArgs->argc=1;
				coreWithFakeArgs->mFakeName="Echo";
				coreWithFakeArgs->args=new char*[1];
				coreWithFakeArgs->args[0]=(char*)coreWithFakeArgs->mFakeName.c_str();
				coreWithFakeArgs->core.reset(new QGuiApplication(coreWithFakeArgs->argc,coreWithFakeArgs->args));
			}
			return coreWithFakeArgs->core;
		}
		
		shared_ptr<ExecutionModel> CreateExecutionModel()
		{
			return make_shared<QtExecutionModel>(GetQtGuiApplication());
		}
		
		shared_ptr<RenderTarget> CreateRenderTarget(const std::string& type, const std::string& name, Kernel& kernel, u32 width, u32 height, u8 bpp, bool fullScreen)
		{
			if(type=="Window")
			{
				shared_ptr<GLContext> context(new GLContext());
				return shared_ptr<QtWindow>(new QtWindow(context, kernel,name,width,height,bpp,fullScreen,GetQtGuiApplication()));
			}
			return shared_ptr<RenderTarget>();
		}

		shared_ptr<InputManager> CreateDefaultInputManager(shared_ptr<RenderTarget> target)
		{
			shared_ptr<InputManager> inputManager = make_shared<InputManager>();
			if(target)
			{
				shared_ptr<QtWindow> window = dynamic_pointer_cast<QtWindow,RenderTarget>(target);
				if(window)
				{
					shared_ptr<QtMouse> mouse = make_shared<QtMouse>(window);
					if(mouse)
					{
						inputManager->InstallDevice("Mouse",mouse);
					}

					shared_ptr<QtKeyboard> keyboard = make_shared<QtKeyboard>(window);
					if(keyboard)
					{
						inputManager->InstallDevice("Keyboard",keyboard);
					}
				}else
				{
					ECHO_LOG_WARNING("Unable to create QtMouse and QtKeyboard input devices as the provided RenderTarget does not appear to be a QtWindow");
				}
			}
			return inputManager;
		}
	}
}
