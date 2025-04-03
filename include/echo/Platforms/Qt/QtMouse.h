#ifndef _ECHOQTMOUSE_H_
#define _ECHOQTMOUSE_H_
#include <echo/Input/InputDevice.h>
#include <echo/Platforms/Qt/QtWindow.h>

namespace Echo
{
	class QtMouse : public InputDevice, public QtWindowEventListener
	{
	private:
		shared_ptr<f32> mMouseX;
		shared_ptr<f32> mMouseY;
		shared_ptr<f32> mMouseWheel;
		shared_ptr<f32> mWheelDelta;
		shared_ptr<bool> mLeftMouseButton;
		shared_ptr<bool> mMiddleMouseButton;
		shared_ptr<bool> mRightMouseButton;
		shared_ptr<QtWindow> mWindow;
	public:
		QtMouse(shared_ptr<QtWindow> window);
		~QtMouse();

		void MouseMoved(f32 newX, f32 newY);
		void LeftButtonStateChanged(f32 atX, f32 atY, bool state);
		void RightButtonStateChanged(f32 atX, f32 atY, bool state);
		void MiddleButtonStateChanged(f32 atX, f32 atY, bool state);
		void KeyStateChanged(u32 key, bool state);
	};
}
#endif
