#include <echo/Input/PseudoMouse.h>
#include <boost/make_shared.hpp>

namespace Echo
{
	PseudoMouse::PseudoMouse(const std::string& deviceName) : InputDevice(deviceName)
	{
		mMouseX=make_shared<f32>(0.f);
		mMouseY=make_shared<f32>(0.f);
		mMouseWheel=make_shared<f32>(0.f);
		mWheelDelta=make_shared<f32>(1.f);
		mLeftMouseButton=make_shared<bool>(false);
		mMiddleMouseButton=make_shared<bool>(false);
		mRightMouseButton=make_shared<bool>(false);

		AddInput<bool>("Left",mLeftMouseButton,false);
		AddInput<bool>("Right",mRightMouseButton,false);
		AddInput<bool>("Middle",mMiddleMouseButton,false);
		AddInput<f32>("X",mMouseX, 0.f);
		AddInput<f32>("Y",mMouseY, 0.f);
		AddInput<f32>("Z",mMouseWheel, 0.f);
	}

	PseudoMouse::~PseudoMouse()
	{
	}

	void PseudoMouse::MouseMove(f32 newX, f32 newY)
	{
		*mMouseX=newX;
		*mMouseY=newY;
	}

	void PseudoMouse::SetLeft(bool down)
	{
		*mLeftMouseButton=down;
	}

	void PseudoMouse::SetRight(bool down)
	{
		*mRightMouseButton=down;
	}

	void PseudoMouse::SetMiddle(bool down)
	{
		*mMiddleMouseButton=down;
	}

	void PseudoMouse::MouseWheelRotate(f32 delta)
	{
		*mMouseWheel=*mWheelDelta*delta;
	}
}
