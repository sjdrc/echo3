#ifndef _ECHOWIIMOTE_H_
#define _ECHOWIIMOTE_H_
#include <echo/Input/InputDevice.h>
#include <echo/Maths/Quaternion.h>
#include <echo/Maths/Vector2.h>
#include <echo/Maths/Vector3.h>
#include <gccore.h>
#include <wiiuse/wpad.h>

namespace Echo
{
	class Wiimote : public InputDevice
	{
	public:
		Wiimote(u32 controlIndex);
		~Wiimote();

		void Update(Seconds lastFrameTime);
	private:
		shared_ptr<bool> mA;
		shared_ptr<bool> mB;
		shared_ptr<bool> mHome;
		shared_ptr<bool> mPlus;
		shared_ptr<bool> mMinus;
		shared_ptr<bool> mOne;
		shared_ptr<bool> mTwo;
		shared_ptr<bool> mUp;
		shared_ptr<bool> mDown;
		shared_ptr<bool> mLeft;
		shared_ptr<bool> mRight;
		shared_ptr<bool> mCursorValid;
		shared_ptr<bool> mControlConnected;
		shared_ptr<Vector2> mCursor;
		shared_ptr<f32> mCursorX;
		shared_ptr<f32> mCursorY;
		shared_ptr<f32> mAngle;
		shared_ptr<f32> mDistance;
		shared_ptr<Vector3> mAccelerometer;
		shared_ptr<f32> mRoll;
		shared_ptr<f32> mPitch;
		shared_ptr<f32> mYaw;
		shared_ptr<Quaternion> mOrientation;
		shared_ptr<Vector3> mGForce;
		u32 mControlIndex;
		s32 mState;
		u32 mExtension;
		WPADData* mWPadData;
	};
}

#endif
