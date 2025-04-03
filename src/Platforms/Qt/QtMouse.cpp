#include <echo/Platforms/Qt/QtMouse.h>
#include <iostream>

namespace Echo
{
	QtMouse::QtMouse(shared_ptr<QtWindow> window) : InputDevice("Mouse"), mWindow(window)
	{
		mMouseX = make_shared<f32>(0.f);
		mMouseY = make_shared<f32>(0.f);
		mMouseWheel = make_shared<f32>(0.f);
		mWheelDelta = make_shared<f32>(1.f);
		mLeftMouseButton = make_shared<bool>(false);
		mMiddleMouseButton = make_shared<bool>(false);
		mRightMouseButton = make_shared<bool>(false);
		mWindow->AddListener(this);

		AddInput<bool>("Left", mLeftMouseButton, false);
		AddInput<bool>("Right", mRightMouseButton, false);
		AddInput<bool>("Middle", mMiddleMouseButton, false);
		AddInput<f32>("X", mMouseX, 0.f);
		AddInput<f32>("Y", mMouseY, 0.f);
		AddInput<f32>("Z", mMouseWheel, 0.f);
	}

	QtMouse::~QtMouse()
	{
		mWindow->RemoveListener(this);
	}

	void QtMouse::MouseMoved(f32 newX, f32 newY)
	{
		*mMouseX = newX / mWindow->GetWidth();
		*mMouseY = newY / mWindow->GetHeight();
	}

	void QtMouse::LeftButtonStateChanged(f32 atX, f32 atY, bool state)
	{
		*mLeftMouseButton = state;
	}

	void QtMouse::RightButtonStateChanged(f32 atX, f32 atY, bool state)
	{
		*mRightMouseButton = state;
	}

	void QtMouse::MiddleButtonStateChanged(f32 atX, f32 atY, bool state)
	{
		*mMiddleMouseButton = state;
	}

	void QtMouse::KeyStateChanged(u32, bool)
	{

	}
}
