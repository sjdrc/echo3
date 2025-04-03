#include <echo/Platforms/Wii/WiiSystemInput.h>
#include <echo/Kernel/Kernel.h>
#include <ogc/system.h>

namespace Echo
{
	namespace
	{
		//We need to create an InputDevice to make the inputs available to the input system. But the callbacks
		//operate globally (i.e. we can't pass any parameters).
		//

		WiiSystemInput* lastWiiSystemInput=0;
		void ResetCallback(u32, void*)
		{
			if(lastWiiSystemInput)
			{
				lastWiiSystemInput->ResetPressed();
			}
		}

		void PowerCallback()
		{
			if(lastWiiSystemInput)
			{
				lastWiiSystemInput->PowerPressed();
			}
		}
	}

	WiiSystemInput::WiiSystemInput(Kernel* kernel) : InputDevice("System"), mKernel(kernel), mKillOnPowerOrReset(true)
	{
		mFramesSinceResetButton=0;
		mFramesSincePowerButton=0;

		SYS_SetPowerCallback(PowerCallback);
		SYS_SetResetCallback(ResetCallback);
		mResetButton=make_shared<bool>(false);
		mPowerButton=make_shared<bool>(false);
		AddInput<bool>("Reset",mResetButton,false);
		AddInput<bool>("Power",mPowerButton,false);
		lastWiiSystemInput=this;
	}
	
	WiiSystemInput::~WiiSystemInput()
	{
	}
	
	void WiiSystemInput::PowerPressed()
	{
		*mPowerButton=true;
		mFramesSincePowerButton=0;
		CheckForKernelShutdown();
	}
	
	void WiiSystemInput::ResetPressed()
	{
		*mResetButton=true;
		CheckForKernelShutdown();
		mFramesSinceResetButton=0;
	}
	
	void WiiSystemInput::CheckForKernelShutdown()
	{
		if(mKillOnPowerOrReset && mKernel)
		{
			mKernel->RemoveAllTasks();
		}
	}
	
	void WiiSystemInput::Update(Seconds lastFrameTime)
	{
		//If the power or reset buttons were pressed and we've processed at least two frames we'll reset the flags.
		//It is probably unusual to find a case where someone won't actually want the default behaviour for the
		//reset and power buttons but we'll enable support for it anyway.
		if(*mPowerButton && mFramesSincePowerButton>1)
		{
			mFramesSincePowerButton++;
			*mPowerButton = false;
		}

		if(*mResetButton && mFramesSinceResetButton>1)
		{
			mFramesSinceResetButton++;
			*mResetButton  = false;
		}
		InputDevice::Update(lastFrameTime);
	}
}
