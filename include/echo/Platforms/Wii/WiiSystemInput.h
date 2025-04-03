#ifndef _ECHOWIISYSTEMINPUT_H_
#define _ECHOWIISYSTEMINPUT_H_
#include <echo/Input/InputDevice.h>

namespace Echo
{
	class Kernel;
	
	class WiiSystemInput : public InputDevice
	{
	public:
		WiiSystemInput(Kernel* kernel);
		~WiiSystemInput();
		void PowerPressed();
		void ResetPressed();
		void CheckForKernelShutdown();
		void Update(Seconds lastFrameTime) override;
	private:
		shared_ptr<bool> mPowerButton;
		shared_ptr<bool> mResetButton;
		Kernel* mKernel;
		bool mKillOnPowerOrReset;
		size_t mFramesSinceResetButton;
		size_t mFramesSincePowerButton;
	};
}

#endif
