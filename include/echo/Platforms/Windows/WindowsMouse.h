#ifndef _WINDOWSMOUSE_H_
#define _WINDOWSMOUSE_H_
#include <echo/Input/InputDevice.h>
#include <windows.h>
#undef ERROR

namespace Echo
{
	class SWindow;
	class RenderTarget;

	class WindowsMouse : public InputDevice
	{
	private:
		shared_ptr<f32> mMouseX;
		shared_ptr<f32> mMouseY;
		shared_ptr<f32> mMouseWheel;
		shared_ptr<f32> mWheelDelta;
		shared_ptr<bool> mLeftMouseButton;
		shared_ptr<bool> mMiddleMouseButton;
		shared_ptr<bool> mRightMouseButton;
		shared_ptr<RenderTarget> mPrimaryRenderTarget;
		shared_ptr<SWindow> mWindow;
		void Setup(shared_ptr<SWindow> window);
	public:
		explicit WindowsMouse(shared_ptr<SWindow> window, shared_ptr<RenderTarget> primaryRenderTarget);
		explicit WindowsMouse(HWND window, shared_ptr<RenderTarget> primaryRenderTarget);
		~WindowsMouse();

		void SetWheelDelta(f32 delta){*mWheelDelta=delta;}
		f32 GetWheelDelta() const {return *mWheelDelta;}

		int MouseMove(UINT message, u32 param01, u32 param02);
		int MouseLeftDown(UINT message, u32 param01, u32 param02);
		int MouseLeftUp(UINT message, u32 param01, u32 param02);
		int MouseRightDown(UINT message, u32 param01, u32 param02);
		int MouseRightUp(UINT message, u32 param01, u32 param02);
		int MouseMiddleDown(UINT message, u32 param01, u32 param02);
		int MouseMiddleUp(UINT message, u32 param01, u32 param02);
		int MouseWheelRotate(UINT message, u32 param01, u32 param02);	
	};
}
#endif
