#include <echo/Platforms/Wii/Wiimote.h>

namespace Echo
{
	//The resolution here is used to calculate the cursor position. We set the controls
	//to have their own resolution and calculate a range between 0 and 1 for both X and Y
	// For X: left is 0, right is 1
	// For Y: top is 0, bottom is 1
	const u32 SCREEN_XRES=1000;
	const u32 SCREEN_YRES=1000;
	
	void wpadreadcallback(s32 chan, const WPADData *data)
	{
	}

	Wiimote::Wiimote(u32 controlIndex) : InputDevice("Wiimote")
	{
		static bool WPADInitialised=false;
		if(!WPADInitialised)
		{
			WPAD_Init();
			while(WPAD_GetStatus() != WPAD_STATE_ENABLED)
			{};
			WPADInitialised=true;
		}
		mCursor=make_shared<Vector2>(Vector2::ZERO);
		mCursorX=make_shared<f32>(0);
		mCursorY=make_shared<f32>(0);
		mCursorValid=make_shared<bool>(false);
		mA=make_shared<bool>(false);
		mB=make_shared<bool>(false);
		mHome=make_shared<bool>(false);
		mOne=make_shared<bool>(false);
		mTwo=make_shared<bool>(false);
		mUp=make_shared<bool>(false);
		mDown=make_shared<bool>(false);
		mLeft=make_shared<bool>(false);
		mRight=make_shared<bool>(false);
		mPlus=make_shared<bool>(false);
		mMinus=make_shared<bool>(false);
		mControlConnected=make_shared<bool>(false);
		mRoll=make_shared<f32>(0.f);
		mPitch=make_shared<f32>(0.f);
		mYaw=make_shared<f32>(0.f);
		mOrientation=make_shared<Quaternion>(Quaternion::IDENTITY);
		mDistance=make_shared<f32>(0.f);
		mAngle=make_shared<f32>(0.f);
		mAccelerometer=make_shared<Vector3>(Vector3::ZERO);
		mGForce=make_shared<Vector3>(Vector3::ZERO);
		mControlIndex=controlIndex;
		mState=WPAD_ERR_NO_CONTROLLER;

		AddInput<bool>("A",mA, false);
		AddInput<bool>("B",mB, false);
		AddInput<bool>("Home",mHome, false);
		AddInput<bool>("Plus",mPlus, false);
		AddInput<bool>("Minus",mMinus, false);
		AddInput<bool>("One",mOne, false);
		AddInput<bool>("Two",mTwo, false);
		AddInput<bool>("Up",mUp, false);
		AddInput<bool>("Down",mDown, false);
		AddInput<bool>("Left",mLeft, false);
		AddInput<bool>("Right",mRight, false);
		AddInput<bool>("CursorValid",mCursorValid, false);
		AddInput<bool>("Connected",mControlConnected, false);

		AddInput<Vector2>("Cursor",mCursor,Vector2::ZERO);
		AddInput<f32>("X",mCursorX,0.f);
		AddInput<f32>("Y",mCursorY,0.f);
		AddInput<f32>("Angle",mAngle,0.f);
		AddInput<f32>("Distance",mDistance,0.f);
		AddInput<Vector3>("Accelerometer",mAccelerometer,Vector3::ZERO);
		AddInput<f32>("Roll",mRoll,0.f);
		AddInput<f32>("Pitch",mPitch,0.f);
		AddInput<f32>("Yaw",mYaw,0.f);
		AddInput<Quaternion>("Orientation",mOrientation,Quaternion::ZERO);
		AddInput<Vector3>("GForce",mGForce,Vector3::ZERO);
	}

	Wiimote::~Wiimote()
	{

	}

	void Wiimote::Update(Seconds lastFrameTime)
	{
		//The first controller can update the states.
		if(mControlIndex==0)
		{
			WPAD_ScanPads();
		}
		u32 extensionType;
		WPAD_ReadPending(mControlIndex,wpadreadcallback);
		s32 state=WPAD_Probe(mControlIndex,&extensionType);

		if(state!=mState && mState==WPAD_ERR_NO_CONTROLLER)
		{
			*mControlConnected=true;
			//Wiimote/WPAD won't set the format on control reconnect
			WPAD_SetDataFormat(mControlIndex, WPAD_FMT_BTNS_ACC_IR);
			WPAD_SetVRes(mControlIndex,SCREEN_XRES,SCREEN_YRES);
		}else
		if(state==WPAD_ERR_NO_CONTROLLER)
		{
			*mControlConnected=false;
			mState=WPAD_ERR_NO_CONTROLLER;
			return;
		}

		mState=state;
		mExtension=extensionType;

		mWPadData=WPAD_Data(mControlIndex);

		WPAD_IR(mControlIndex, &mWPadData->ir);
		WPAD_Orientation(mControlIndex, &mWPadData->orient);
		WPAD_GForce(mControlIndex, &mWPadData->gforce);
		WPAD_Accel(mControlIndex, &mWPadData->accel);
		WPAD_Expansion(mControlIndex, &mWPadData->exp);

		// WPAD_ButtonsDown tells us which buttons were pressed in this loop
		// this is a "one shot" state which will not fire again until the button has been released
		u32 wiiMotePressed = WPAD_ButtonsHeld(mControlIndex);

		*mLeft=(wiiMotePressed & WPAD_BUTTON_LEFT);
		*mRight=(wiiMotePressed & WPAD_BUTTON_RIGHT);
		*mUp=(wiiMotePressed & WPAD_BUTTON_UP);
		*mDown=(wiiMotePressed & WPAD_BUTTON_DOWN);
		*mMinus=(wiiMotePressed & WPAD_BUTTON_MINUS);
		*mPlus=(wiiMotePressed & WPAD_BUTTON_PLUS);
		*mA=(wiiMotePressed & WPAD_BUTTON_A);
		*mB=(wiiMotePressed & WPAD_BUTTON_B);
		*mHome=(wiiMotePressed & WPAD_BUTTON_HOME);
		*mOne=(wiiMotePressed & WPAD_BUTTON_1);
		*mTwo=(wiiMotePressed & WPAD_BUTTON_2);

		//Now check for Pointer position
		if(mWPadData->ir.valid)
		{
			*mCursorValid=true;
			*mCursorX = mCursor->x=(mWPadData->ir.x/static_cast<f32>(SCREEN_XRES));
			*mCursorY = mCursor->y=(mWPadData->ir.y/static_cast<f32>(SCREEN_YRES));
			*mAngle=mWPadData->ir.angle;
			*mDistance=mWPadData->ir.distance;
		}else
		{
			*mCursorValid=false;
		}

		mAccelerometer->x=mWPadData->accel.x;
		mAccelerometer->y=mWPadData->accel.y;
		mAccelerometer->z=mWPadData->accel.z;

		*mRoll=mWPadData->orient.roll;
		*mPitch=mWPadData->orient.pitch;
		*mYaw=mWPadData->orient.yaw;
		mOrientation->SetEuler(Radian(mWPadData->orient.yaw),Radian(mWPadData->orient.pitch),Radian(mWPadData->orient.roll));

		mGForce->x=mWPadData->gforce.x;
		mGForce->y=mWPadData->gforce.y;
		mGForce->z=mWPadData->gforce.z;
		
		//Update the expansion data
		switch(mWPadData->exp.type)
		{
			case EXP_NONE:
				break;
			case EXP_NUNCHUK:
				//mWPadData->exp.nunchuk;
//				struct accel_t accel_calib;		/**< nunchuk accelerometer calibration		*/
//				struct joystick_t js;			/**< joystick calibration					*/
//
//				int* flags;						/**< options flag (points to wiimote_t.flags) */
//
//				ubyte btns;						/**< what buttons have just been pressed	*/
//				ubyte btns_last;				/**< what buttons have just been pressed	*/
//				ubyte btns_held;				/**< what buttons are being held down		*/
//				ubyte btns_released;			/**< what buttons were just released this	*/
//
//				struct vec3w_t accel;			/**< current raw acceleration data			*/
//				struct orient_t orient;			/**< current orientation on each axis		*/
//				struct gforce_t gforce;			/**< current gravity forces on each axis	*/
				break;
			case EXP_CLASSIC:
				//mWPadData->exp.classic;
//				short btns;						/**< what buttons have just been pressed	*/
//				short btns_last;				/**< what buttons have just been pressed	*/
//				short btns_held;				/**< what buttons are being held down		*/
//				short btns_released;			/**< what buttons were just released this	*/
//
//				ubyte rs_raw;
//				ubyte ls_raw;
//
//				float r_shoulder;				/**< right shoulder button (range 0-1)		*/
//				float l_shoulder;				/**< left shoulder button (range 0-1)		*/
//
//				struct joystick_t ljs;			/**< left joystick calibration				*/
//				struct joystick_t rjs;			/**< right joystick calibration				*/
				break;
			case EXP_GUITAR_HERO_3:
				//mWPadData->exp.gh3;
//				short btns;						/**< what buttons have just been pressed	*/
//				short btns_last;				/**< what buttons have just been pressed	*/
//				short btns_held;				/**< what buttons are being held down		*/
//				short btns_released;			/**< what buttons were just released this	*/
//
//				ubyte wb_raw;
//				float whammy_bar;				/**< whammy bar (range 0-1)					*/
//
//				ubyte tb_raw;
//				int touch_bar;					/**< touch bar								*/
//
//				struct joystick_t js;			/**< joystick calibration					*/
				break;
			case EXP_WII_BOARD:
				//mWPadData->exp.wb;
//				float tl;  /* Interpolated */
//				float tr;
//				float bl;
//				float br;  /* End interp */
//				short rtl; /* RAW */
//				short rtr;
//				short rbl;
//				short rbr; /* /RAW */
//				short ctl[3]; /* Calibration */
//				short ctr[3];
//				short cbl[3];
//				short cbr[3]; /* /Calibration */
//				float x;
//				float y;
				break;
			case EXP_MOTION_PLUS:
				//mWPadData->exp.mp;
			//	short rx, ry, rz;
			//	ubyte status;
			//	ubyte ext;
				break;
		}
		InputDevice::Update(lastFrameTime);
	}
}