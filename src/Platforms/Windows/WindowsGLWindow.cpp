#include <echo/Platforms/Windows/WindowsGLWindow.h>
#include <echo/Kernel/Kernel.h>
#include <windows.h>
#undef ERROR
#include <tchar.h>

namespace Echo
{
	bool WindowsGLWindow::mConsoleVisible=false;

	WindowsGLWindow::WindowsGLWindow(	shared_ptr<GLContext> context,
										Kernel& kernel,
										const std::string& title,
										u32 width,
										u32 height,
										u8 bits,
										bool fullscreenflag,
										HWND parent) :
		GLRenderTarget(context),
		mKernel(kernel)
	{
		assert(width>=0 && height>=0 && "Cannot create window with a width or height of 0.");
		mWidth=width;
		mHeight=height;
		mBytesPerPixel=bits/8;
		mIsFullScreen=fullscreenflag;
		mShowCursor=true;
		mGLRenderingContext=NULL;

		mName=title;

		//Setup message handlers
		AddMessageHandler(WM_NCHITTEST,this,&WindowsGLWindow::HitTest);
		AddMessageHandler(WM_SIZE,this,&WindowsGLWindow::Resize);
		AddMessageHandler(WM_DESTROY,this,&WindowsGLWindow::Destroy);
		AddMessageHandler(WM_ENTERMENULOOP,this,&WindowsGLWindow::PauseKernel);
		AddMessageHandler(WM_ENTERSIZEMOVE,this,&WindowsGLWindow::PauseKernel);
		AddMessageHandler(WM_EXITMENULOOP,this,&WindowsGLWindow::ResumeKernel);
		AddMessageHandler(WM_EXITSIZEMOVE,this,&WindowsGLWindow::ResumeKernel);
		AddMessageHandler(WM_ACTIVATEAPP,this,&WindowsGLWindow::PauseResumeCheck);
		AddMessageHandler(WM_NCACTIVATE,this,&WindowsGLWindow::PauseResumeCheck);

		//Create the window
		_CreateWindow(parent);

		DetermineCapabilities();

		glewInit();
	#ifdef _DEBUG
		ShowConsoleWindow();
	#endif
	}

	WindowsGLWindow::~WindowsGLWindow()
	{
		HideConsoleWindow();
	}

	void WindowsGLWindow::ShowConsoleWindow()
	{
		if(!mConsoleVisible)
		{
			mConsoleVisible=true;
			AllocConsole(); 
			freopen("CONOUT$", "wb", stdout); 
		}
	}

	void WindowsGLWindow::HideConsoleWindow()
	{
		if(mConsoleVisible)
		{
			mConsoleVisible=false;
			fclose(stdout); 
			FreeConsole();
		}
	}


	bool WindowsGLWindow::Activate()
	{
		if(wglGetCurrentContext()!=mGLRenderingContext)
			wglMakeCurrent(mGDIDeviceContext,mGLRenderingContext);
		GLRenderTarget::Activate();
		return true;
	}

	void WindowsGLWindow::Deactivate()
	{
		GLRenderTarget::Deactivate();
	}

	void WindowsGLWindow::EnableVSync()
	{
		typedef bool (APIENTRY *PFNWGLSWAPINTERVALFARPROC)(int);
		PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress("wglSwapIntervalEXT");
		if(wglSwapIntervalEXT)
		{
			wglSwapIntervalEXT(1);
		}
	}

	void WindowsGLWindow::DisableVSync()
	{
		typedef bool (APIENTRY *PFNWGLSWAPINTERVALFARPROC)(int);
		PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress("wglSwapIntervalEXT");
		if(wglSwapIntervalEXT)
		{
			wglSwapIntervalEXT(0);
		}
	}

	void WindowsGLWindow::SwapBuffers()
	{
		::SwapBuffers(mGDIDeviceContext);
	}
	
	void WindowsGLWindow::SetFullScreen(bool fullscreen)
	{
		s32 x;
		s32 y;
		if(fullscreen)
		{
			x=GetSystemMetrics(SM_CXSCREEN);
			y=GetSystemMetrics(SM_CYSCREEN);
		}else
		{
			x=GetWidth();
			y=GetHeight();
		}
		SetSize(x, y);
	}

	void WindowsGLWindow::AlwaysShowCursor(bool show)
	{
		mShowCursor=show;
		ShowExternalCursors(show);
	}

	void WindowsGLWindow::ShowExternalCursors(bool show)
	{
		if(show)
			ShowCursor(TRUE);
		else
			ShowCursor(FALSE);
		mShowCursor=show;
	}

	//Message command handlers
	int WindowsGLWindow::Destroy(UINT message, u32 wParam, u32 lParam)
	{
		mKernel.RemoveAllTasks();
		return 0;
	}

	//WM_NCHITTEST
	int WindowsGLWindow::HitTest(UINT message, u32 wParam, u32 lParam)
	{
		//static bool hidden=false;
		//ECHO_LOG_DEBUG("WM_NCHITTEST: ");
		LRESULT res=DefWindowProc(GetWindow(),WM_NCHITTEST,wParam,lParam);
		if(res==HTCLIENT)
		{
			//ECHO_LOG_DEBUG("CA");
			if(!mShowCursor)
			{
				ShowCursor(FALSE);
			}
		}else
		{
			//ECHO_LOG_DEBUG("NCA");
			ShowCursor(TRUE);
		}
		return (int)res;
	}

	//WM_SIZE
	int WindowsGLWindow::Resize(UINT message, u32 wParam, u32 lParam)
	{
		switch(wParam)
		{
			case SIZE_RESTORED:
				mKernel.ResumeKernel(true);
				break;
			case SIZE_MINIMIZED:
				mKernel.PauseKernel(true);
				break;
		}
		mWidth=LOWORD(lParam);
		mHeight=HIWORD(lParam);
		return (int)DefWindowProc(GetWindow(),message,wParam,lParam);
	}

	//WM_ENTERMENULOOP
	//WM_ENTERSIZEMOVE
	int WindowsGLWindow::PauseKernel(UINT message, u32 wParam, u32 lParam)
	{
		mKernel.PauseKernel(true);
		return (int)DefWindowProc(GetWindow(),message,wParam,lParam);
	}

	//WM_EXITMENULOOP
	//WM_EXITSIZEMOVE
	int WindowsGLWindow::ResumeKernel(UINT message, u32 wParam, u32 lParam)
	{
		mKernel.ResumeKernel(true);
		return (int)DefWindowProc(GetWindow(),message,wParam,lParam);
	}

	//WM_ACTIVATEAPP
	//WM_NCACTIVATE
	int WindowsGLWindow::PauseResumeCheck(UINT message, u32 wParam, u32 lParam)
	{
		if(wParam)
		{
			mKernel.ResumeKernel(true);
		}else
		{
			mKernel.PauseKernel(true);
		}
		return (int)DefWindowProc(GetWindow(),message,wParam,lParam);
	}

	void WindowsGLWindow::_SubscribeExtraMouseMessages()
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize=sizeof(TRACKMOUSEEVENT);
	#pragma message("TME_NONCLIENT was removed for initial build. Update as needed.")
		tme.dwFlags=TME_LEAVE;// | TME_NONCLIENT;
		tme.hwndTrack=GetWindow();
		tme.dwHoverTime=0;
		TrackMouseEvent(&tme);
	}

	void WindowsGLWindow::_DestroyWindow()
	{
		if (IsFullScreen())										// Are We In Fullscreen Mode?
		{
			ShowCursor(true);
			if (!ChangeDisplaySettings(NULL,CDS_TEST))
			{ 													// If The Shortcut Doesn't Work ( NEW )
				ChangeDisplaySettings(NULL,CDS_RESET);			// Do It Anyway (To Get The Values Out Of The Registry) ( NEW )
				ChangeDisplaySettings(&mDMSavedState,CDS_RESET);	// Change It To The Saved Settings ( NEW )
			} else
			{
				ChangeDisplaySettings(NULL,CDS_RESET);			// If It Works, Go Right Ahead ( NEW )
			}
		}

		if (mGLRenderingContext)											// Do We Have A Rendering Context?
		{
			if (!wglMakeCurrent(NULL,NULL))						// Are We Able To Release The DC And RC Contexts?
			{
				ECHO_LOG_ERROR(mName << " SHUTDOWN ERROR: Release Of DC And RC Failed.");
				//MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
			}

			if (!wglDeleteContext(mGLRenderingContext))						// Are We Able To Delete The RC?
			{
				ECHO_LOG_ERROR(mName << " SHUTDOWN ERROR: Release Rendering Context Failed.");
				//MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
			}
			mGLRenderingContext=NULL;										// Set RC To NULL
		}

		if (mGDIDeviceContext && !ReleaseDC(GetWindow(),mGDIDeviceContext))					// Are We Able To Release The DC
		{
			ECHO_LOG_ERROR(mName << " SHUTDOWN ERROR: Release Device Context Failed.");
			//MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
			mGDIDeviceContext=NULL;										// Set DC To NULL
		}

		if (GetWindow() && !DestroyWindow(GetWindow()))					// Are We Able To Destroy The Window?
		{
			ECHO_LOG_ERROR(mName << " SHUTDOWN ERROR: Could Not Release window handle.");
			//MessageBox(NULL,"Could Not Release handle.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
			SetWindow(0);
		}

	#ifdef _UNICODE
		std::wstringstream wNameSS;
		wNameSS<<mName.c_str();
		std::wstring className=wNameSS.str();
	#else
		std::string& className=mName;
	#endif
		if (!UnregisterClass(className.c_str(),mHInstance))			// Are We Able To Unregister Class
		{
			ECHO_LOG_DEBUG(mName << " SHUTDOWN ERROR: Could Not Unregister Class.");
			//MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
			mHInstance=NULL;									// Set hInstance To NULL
		}
	}

	bool WindowsGLWindow::_CreateWindow(HWND parent)
	{
		GLuint		PixelFormat;			// Holds The Results After Searching For A Match
		WNDCLASS	wc;						// Windows Class Structure
		DWORD		dwExStyle;				// Window Extended Style
		DWORD		dwStyle;				// Window Style
		RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values

		WindowRect.left=(long)0;			// Set Left Value To 0
		WindowRect.right=(long)GetWidth();	// Set Right Value To Requested Width
		WindowRect.top=(long)0;				// Set Top Value To 0
		WindowRect.bottom=(long)GetHeight();// Set Bottom Value To Requested Height

	#ifdef _UNICODE
		std::wstringstream wNameSS;
		wNameSS<<mName.c_str();
		std::wstring className=wNameSS.str();
	#else
		std::string& className=mName;
	#endif

		mHInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window
		wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;	// Redraw On Size, And Own DC For Window.
		wc.lpfnWndProc		= (WNDPROC)SWindow::WinProc;			// WndProc Handles Messages
		wc.cbClsExtra		= 0;									// No Extra Window Data
		wc.cbWndExtra		= 0;									// No Extra Window Data
		wc.hInstance		= mHInstance;							// Set The Instance
		wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
		wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
		wc.hbrBackground	= NULL;									// No Background Required For GL
		wc.lpszMenuName		= NULL;									// We Don't Want A Menu
		wc.lpszClassName	= className.c_str();			// Set The Class Name

		if (!RegisterClass(&wc))									// Attempt To Register The Window Class
		{
			ECHO_LOG_ERROR("Failed To Register The Window Class. Error Code: " << std::hex << GetLastError() << std::dec);
			//MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
			return FALSE;											// Return FALSE
		}

		EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &mDMSavedState);	// Save The Current Display State ( NEW )

		if (IsFullScreen())											// Attempt Fullscreen Mode?
		{
			DEVMODE dmScreenSettings;								// Device Mode
			memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
			dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
			dmScreenSettings.dmPelsWidth	= GetWidth();	// Selected Screen Width
			dmScreenSettings.dmPelsHeight	= GetHeight();	// Selected Screen Height
			dmScreenSettings.dmBitsPerPel	= GetBytesPerPixel()*8;					// Selected Bits Per Pixel
			dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

			// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
			if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
			{
				// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
				if (MessageBox(NULL,_T("The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?"),_T("Echo"),MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
				{
					mIsFullScreen=false;		// Windowed Mode Selected.  Fullscreen = FALSE
				}else
				{
					// Pop Up A Message Box Letting User Know The Program Is Closing.
					//MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
					ECHO_LOG_ERROR("No Window.");
					return FALSE;									// Return FALSE
				}
			}
		}

		if (IsFullScreen())											// Are We Still In Fullscreen Mode?
		{
			dwExStyle=WS_EX_APPWINDOW;								// Window Extended Style
			dwStyle=WS_POPUP;										// Windows Style
			//ShowCursor(FALSE);										// Hide Mouse Pointer
		}
		else
		{
			dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
			dwStyle=WS_OVERLAPPEDWINDOW;							// Windows Style
		}
		if(parent)
		{
			dwExStyle=0;
			dwStyle=WS_CHILD;
		}

		AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

		// Create The Window
		mWindowHwnd=0;
		mWindowHwnd=CreateWindowEx(		dwExStyle,							// Extended Style For The Window
										className.c_str(),			// Class Name
										className.c_str(),			// Window Title
										dwStyle |							// Defined Window Style
										WS_CLIPSIBLINGS |					// Required Window Style
										WS_CLIPCHILDREN,					// Required Window Style
										CW_USEDEFAULT, CW_USEDEFAULT,		// Window Position
										WindowRect.right-WindowRect.left,	// Calculate Window Width
										WindowRect.bottom-WindowRect.top,	// Calculate Window Height
										NULL,								// No Parent Window
										NULL,								// No Menu
										mHInstance,							// Instance
										NULL);								// Dont Pass Anything To WM_CREATE
		if (!(mWindowHwnd))
		{
			ECHO_LOG_ERROR("CreateWindowEx failed: Error code: " << std::hex << GetLastError() << std::dec);
			_DestroyWindow();							// Reset The Display
			//MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
			return FALSE;								// Return FALSE
		}

		//Set the SWindow window
		SetWindow(mWindowHwnd);

		_SubscribeExtraMouseMessages();

		static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
		{
			sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
			1,											// Version Number
			PFD_DRAW_TO_WINDOW |						// Format Must Support Window
			PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
			PFD_DOUBLEBUFFER,							// Must Support Double Buffering
			PFD_TYPE_RGBA,								// Request An RGBA Format
			(BYTE)(GetBytesPerPixel()*8),					// Select Our Color Depth
			0, 0, 0, 0, 0, 0,							// Color Bits Ignored
			0,											// No Alpha Buffer
			0,											// Shift Bit Ignored
			0,											// No Accumulation Buffer
			0, 0, 0, 0,									// Accumulation Bits Ignored
			16,											// 16Bit Z-Buffer (Depth Buffer)  
			0,											// No Stencil Buffer
			0,											// No Auxiliary Buffer
			PFD_MAIN_PLANE,								// Main Drawing Layer
			0,											// Reserved
			0, 0, 0										// Layer Masks Ignored
		};

		if (!(mGDIDeviceContext=GetDC(GetWindow())))				// Did We Get A Device Context?
		{
			_DestroyWindow();							// Reset The Display
			MessageBox(NULL,_T("Can't Create A GL Device Context."),_T("ERROR"),MB_OK|MB_ICONEXCLAMATION);
			return FALSE;								// Return FALSE
		}

		if (!(PixelFormat=ChoosePixelFormat(mGDIDeviceContext,&pfd)))	// Did Windows Find A Matching Pixel Format?
		{
			_DestroyWindow();								// Reset The Display
			ECHO_LOG_ERROR("Can't Find A Suitable PixelFormat. Error Code: " << std::hex << GetLastError() << std::dec);
			//MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
			return FALSE;								// Return FALSE
		}

		if(!SetPixelFormat(mGDIDeviceContext,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
		{
			_DestroyWindow();								// Reset The Display
			ECHO_LOG_ERROR("Can't set the PixelFormat. Error Code: " << std::hex << GetLastError() << std::dec);
			//MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
			return FALSE;								// Return FALSE
		}

		if (!(mGLRenderingContext=wglCreateContext(mGDIDeviceContext)))				// Are We Able To Get A Rendering Context?
		{
			_DestroyWindow();								// Reset The Display
			ECHO_LOG_ERROR("Can't create a GL rendering context. Error Code: " << std::hex << GetLastError() << std::dec);
			//MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
			return FALSE;								// Return FALSE
		}

		if(!wglMakeCurrent(mGDIDeviceContext,mGLRenderingContext))					// Try To Activate The Rendering Context
		{
			_DestroyWindow();								// Reset The Display
			ECHO_LOG_ERROR("Can't activate the gl rendering context. Error Code: " << std::hex << GetLastError() << std::dec);
			//MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
			return FALSE;								// Return FALSE
		}
		
		//Try and determine the display DPI
		#ifdef GetDpiForMonitor
		// At the time of writing this wasn't tested, but I've left it here as a hint for when we actually develop
		// Windows 8.1+
		UINT dpiX=0;
		UINT dpiY=0;
		GetDpiForMonitor(MonitorFromWindow(hwnd,MONITOR_DEFAULTTOPRIMARY),MDT_EFFECTIVE_DPI,&dpiX,&dpiY);
		SetDisplayXDPI(dpiX);
		SetDisplayYDPI(dpiY);
		#else
		// Set up a reasonable default (this reasonable default comes from the Windows documentation, the documentation
		// says that 72dpi is used for rendering fonts when the DPI is unknown.)
		SetDisplayXDPI(72);
		SetDisplayYDPI(72);
		#endif
		

		Show();
		SetForegroundWindow(GetWindow());						// Slightly Higher Priority
		SetFocus(GetWindow());									// Sets Keyboard Focus To The Window

		//Share resources
//		WindowsGLWindow* primaryWindow=dynamic_cast<WindowsGLWindow*>(gGraphics().GetPrimaryRenderTarget());
//		if(primaryWindow)
//		{
//			if(!wglShareLists(mGLRenderingContext,primaryWindow->mGLRenderingContext))
//				ECHO_LOG_ERROR("Unable to share primary rendering target's GL rendering context. This window ("<< mName <<") will display incorrectly.");
//		}
		return TRUE;									// Success
	}

	void WindowsGLWindow::SetSize(u32 width, u32 height)
	{
//		DWORD dwExStyle;			// Window Extended Style
//		DWORD dwStyle;				// Window Style
//		RECT WindowRect;			// Grabs Rectangle Upper Left / Lower Right Values
//		u32 ow, oh, ob;
//
//		//Check to see if it's attempting to create the same screen mode!
//		if(width==GetActualWidth() && height==GetActualHeight() && bpp==GetColourDepth() && ( (fullscreen && IsFullScreen()) || (!fullscreen && IsFullScreen())))
//			return;
//
//		WindowRect.left=0;
//		WindowRect.right=width;
//		WindowRect.top=0;
//		WindowRect.bottom=height;
//
//		if(!fullscreen && IsFullScreen())	//If we were fullscreen and going windowed
//		{
//			ShowExternalCursors(true);
//			if (!ChangeDisplaySettings(NULL,CDS_TEST))		 		// If The Shortcut Doesn't Work ( NEW )
//			{
//				ChangeDisplaySettings(NULL,CDS_RESET);				// Do It Anyway (To Get The Values Out Of The Registry) ( NEW )
//				ChangeDisplaySettings(&mDMSavedState,CDS_RESET);	// Change It To The Saved Settings ( NEW )
//			}else
//			{
//				ChangeDisplaySettings(NULL,CDS_RESET);		// If It Works, Go Right Ahead ( NEW )
//			}
//		}
//
//		if(fullscreen)												// Are We Still In Fullscreen Mode?
//		{
//			DEVMODE dmScreenSettings;								// Device Mode
//			memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
//			dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
//			dmScreenSettings.dmPelsWidth	= width;					// Selected Screen Width
//			dmScreenSettings.dmPelsHeight	= height;					// Selected Screen Height
//			dmScreenSettings.dmBitsPerPel	= bpp;				// Selected Bits Per Pixel
//			dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;
//
//			ow = (u32)GetActualWidth();
//			oh = (u32)GetActualHeight();
//			ob = (u32)GetColourDepth();
//
//			// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
//			if(ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
//			{
//				if(IsFullScreen()) //Was full screened before??
//				{
//					dmScreenSettings.dmPelsWidth = ow;
//					dmScreenSettings.dmPelsWidth = oh;
//					dmScreenSettings.dmBitsPerPel = bpp;
//					if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
//					{
//						fullscreen = false;
//					}
//				}
//			}
//
//			dwExStyle=WS_EX_APPWINDOW;								// Window Extended Style
//			dwStyle=WS_POPUP;										// Windows Style
//
//			if(IsFullScreen() && fullscreen)						//Weren't already full-screen?
//				ShowExternalCursors(false);
//		}
//
//		if(!fullscreen)
//		{
//			dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
//			dwStyle=WS_OVERLAPPEDWINDOW;							// Windows Style
//		}
//
//		//Set the new styles just incase they changed
//		if( (IsFullScreen() && !fullscreen) || (IsFullScreen() && fullscreen)) //dwStyle needs changing?
//		{
//			SetWindowLong(GetWindow(), GWL_STYLE, dwStyle|WS_CLIPCHILDREN|WS_CLIPSIBLINGS);
//			SetWindowLong(GetWindow(),GWL_EXSTYLE,dwExStyle);
//			ShowWindow(GetWindow(),SW_SHOW);
//		}
//
//		AdjustWindowRectEx(&WindowRect, dwStyle, false, dwExStyle);	// Adjust Window To True Requested Size
//
//		MoveWindow(GetWindow(),0,0,WindowRect.right-WindowRect.left,WindowRect.bottom-WindowRect.top,1);
//		mIsFullScreen=fullscreen;
	}

	void WindowsGLWindow::SetName(const std::string& name)
	{
		mName = name;
		if(mWindowHwnd)
		{
			SetWindowText(mWindowHwnd,static_cast<LPCTSTR>(name.c_str()));
		}
	}
	
	std::string WindowsGLWindow::GetName()
	{
		return mName;
	}
}
