#ifndef _ECHOPSEUDOMOUSE_H_
#define _ECHOPSEUDOMOUSE_H_
#include <echo/Input/InputDevice.h>

namespace Echo
{
	class PseudoMouse : public InputDevice
	{
	private:
		shared_ptr<f32> mMouseX;
		shared_ptr<f32> mMouseY;
		shared_ptr<f32> mMouseWheel;
		shared_ptr<f32> mWheelDelta;
		shared_ptr<bool> mLeftMouseButton;
		shared_ptr<bool> mMiddleMouseButton;
		shared_ptr<bool> mRightMouseButton;
	public:
		PseudoMouse(const std::string& deviceName);
		~PseudoMouse();

		void SetWheelDelta(f32 delta){*mWheelDelta=delta;}
		f32 GetWheelDelta() const {return *mWheelDelta;}

		void MouseMove(f32 newX, f32 newY);
		void SetLeft(bool down);
		void SetRight(bool down);
		void SetMiddle(bool down);
		void MouseWheelRotate(f32 delta);
	};
}
#endif
