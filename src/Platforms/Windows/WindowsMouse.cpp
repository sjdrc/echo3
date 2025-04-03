#include <echo/Platforms/Windows/WindowsMouse.h>
#include <echo/Platforms/Windows/SWindow.h>
#include <echo/Graphics/RenderTarget.h>
#include <iostream>

namespace Echo
{
	WindowsMouse::WindowsMouse(HWND window, shared_ptr<RenderTarget> primaryRenderTarget) : InputDevice("Mouse"), mPrimaryRenderTarget(primaryRenderTarget)
	{
		mWindow.reset(new SWindow());
		mWindow->TakeOverWindow(window);
		mWindow->SetCallOldProc(true);
		Setup(mWindow);
	}

	WindowsMouse::WindowsMouse(shared_ptr<SWindow> window, shared_ptr<RenderTarget> primaryRenderTarget) : InputDevice("Mouse"), mPrimaryRenderTarget(primaryRenderTarget)
	{
		Setup(window);
	}

	void WindowsMouse::Setup(shared_ptr<SWindow> window)
	{
		mMouseX.reset(new f32);
		*mMouseX=0.f;
		mMouseY.reset(new f32);
		*mMouseY=0.f;
		mMouseWheel.reset(new f32);
		*mMouseWheel=0.f;
		mWheelDelta.reset(new f32);
		*mWheelDelta=1.0f;
		mLeftMouseButton.reset(new bool);
		*mLeftMouseButton=false;
		mMiddleMouseButton.reset(new bool);
		*mMiddleMouseButton=false;
		mRightMouseButton.reset(new bool);
		*mRightMouseButton=false;	

		//TRACKMOUSEEVENT tme;
		//tme.cbSize=sizeof(TRACKMOUSEEVENT);
		//tme.dwFlags=TME_LEAVE | TME_NONCLIENT;
		//tme.hwndTrack=window->GetWindow();
		//tme.dwHoverTime=0;
		//TrackMouseEvent(&tme);

		window->AddMessageHandler(WM_MBUTTONDOWN,this,&WindowsMouse::MouseMiddleDown);
		window->AddMessageHandler(WM_LBUTTONDOWN,this,&WindowsMouse::MouseLeftDown);
		window->AddMessageHandler(WM_RBUTTONDOWN,this,&WindowsMouse::MouseRightDown);
		window->AddMessageHandler(WM_MBUTTONUP,this,&WindowsMouse::MouseMiddleUp);
		window->AddMessageHandler(WM_LBUTTONUP,this,&WindowsMouse::MouseLeftUp);
		window->AddMessageHandler(WM_RBUTTONUP,this,&WindowsMouse::MouseRightUp);
		window->AddMessageHandler(WM_MOUSEMOVE,this,&WindowsMouse::MouseMove);

		AddInput<bool>("Left",mLeftMouseButton,false);
		AddInput<bool>("Right",mRightMouseButton,false);
		AddInput<bool>("Middle",mMiddleMouseButton,false);
		AddInput<f32>("X",mMouseX,0.f);
		AddInput<f32>("Y",mMouseY,0.f);
		AddInput<f32>("Z",mMouseWheel,0.f);
	}

	WindowsMouse::~WindowsMouse()
	{
		if(mWindow)
		{
			mWindow->ReleaseTakenOverWindow();
		}
	}

	int WindowsMouse::MouseMove(UINT message, u32 param01, u32 param02)
	{
		s16 mx=(s16)( LOWORD(param02));
		s16 my=(s16)( HIWORD(param02));

		if(mPrimaryRenderTarget)
		{
			*mMouseX=static_cast<f32>(mx) / mPrimaryRenderTarget->GetWidth();
			*mMouseY=static_cast<f32>(my) / mPrimaryRenderTarget->GetHeight();
		}
		return 0;
	}

	int WindowsMouse::MouseLeftDown(UINT message, u32 param01, u32 param02)
	{
		*mLeftMouseButton=true;
		return 0;
	}

	int WindowsMouse::MouseLeftUp(UINT message, u32 param01, u32 param02)
	{
		*mLeftMouseButton=false;
		return 0;
	}

	int WindowsMouse::MouseRightDown(UINT message, u32 param01, u32 param02)
	{
		*mRightMouseButton=true;
		return 0;
	}

	int WindowsMouse::MouseRightUp(UINT message, u32 param01, u32 param02)
	{
		*mRightMouseButton=false;
		return 0;
	}

	int WindowsMouse::MouseMiddleDown(UINT message, u32 param01, u32 param02)
	{
		*mMiddleMouseButton=true;
		return 0;
	}

	int WindowsMouse::MouseMiddleUp(UINT message, u32 param01, u32 param02)
	{
		*mMiddleMouseButton=false;
		return 0;
	}

	int WindowsMouse::MouseWheelRotate(UINT message, u32 param01, u32 param02)
	{
		s32 delta=HIWORD(param01);
		*mMouseWheel=*mWheelDelta*delta;
		return 0;
	}
}
