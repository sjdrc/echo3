#ifdef ECHO_PLATFORM_WINDOWS
#ifndef _EWINDOWSGLWINDOW_H_
#define _EWINDOWSGLWINDOW_H_

//I don't like this!
#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.
#define _WIN32_WINNT 0x0500
#endif
#include <echo/Platforms/GL/GLShaderSupport.h>
#include <echo/Platforms/GL/GLRenderTarget.h>
#include <echo/Platforms/Windows/SWindow.h>
#pragma comment (lib,"OpenGL32.lib")

namespace Echo
{
	class Kernel;
	
	class WindowsGLWindow : public GLRenderTarget, public SWindow
	{
	private:
		Kernel& mKernel;
		std::string mName;
		HDC mGDIDeviceContext;
		HWND mWindowHwnd;
		HGLRC mGLRenderingContext;
		DEVMODE mDMSavedState;
		HINSTANCE mHInstance;
		static bool mConsoleVisible;
		bool mShowCursor;
		void _SubscribeExtraMouseMessages();
		bool _CreateWindow(HWND parent);
		void _DestroyWindow();
	public:
		WindowsGLWindow(shared_ptr<GLContext> context, Kernel& kernel, const std::string& title, u32 width, u32 height, u8 bits=32, bool fullscreenflag=false, HWND parent=0);
		virtual ~WindowsGLWindow();

		void ShowConsoleWindow();
		void HideConsoleWindow();

		virtual bool Activate();
		//We will process our windows messages on Deactivate
		virtual void Deactivate();

		virtual void EnableVSync();
		virtual void DisableVSync();
	
		void SwapBuffers();

		virtual void SetSize(u32 width, u32 height);
		virtual void SetFullScreen(bool fullscreen);
		virtual void ShowExternalCursors(bool show);
		void AlwaysShowCursor(bool show);

		//Message command handlers
		virtual int Destroy(UINT message, u32 wParam, u32 lParam);

		//WM_NCHITTEST
		virtual int HitTest(UINT message, u32 wParam, u32 lParam);

		//WM_SIZE
		virtual int Resize(UINT message, u32 wParam, u32 lParam);

		//WM_ENTERMENULOOP
		//WM_ENTERSIZEMOVE
		virtual int PauseKernel(UINT message, u32 wParam, u32 lParam);

		//WM_EXITMENULOOP
		//WM_EXITSIZEMOVE
		virtual int ResumeKernel(UINT message, u32 wParam, u32 lParam);

		//WM_ACTIVATEAPP
		//WM_NCACTIVATE
		virtual int PauseResumeCheck(UINT message, u32 wParam, u32 lParam);
		void SetName(const std::string& name) override;
		std::string GetName() override;
	};

}

#endif 
#endif //ECHO_PLATFORM_WINDOWS
