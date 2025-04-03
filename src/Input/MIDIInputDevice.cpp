#include <echo/Input/MIDIInputDevice.h>
#include <echo/Kernel/Thread.h>

namespace Echo
{
	MIDIInputDevice::MIDIInputDevice(const std::string& name, File device) : InputDevice(name), mDevice(device), mRunning(false)
	{
		// Add inputs
		for(Size channel = 0; channel < 16; ++channel)
		{
			for(Size pitch = 0; pitch < 128; ++pitch)
			{
				std::stringstream inputName;
				inputName << channel << ":" << pitch;
				mState[channel][pitch] = shared_ptr<f32>(new f32());
				*mState[channel][pitch] = 0;
				AddInput(inputName.str(),mState[channel][pitch],0.f);
			}
		}
		mReadThread.reset(new Thread("MIDIInputDevice",bind(&MIDIInputDevice::ReadThread,this)));
		mReadThread->Execute();
	}

	MIDIInputDevice::~MIDIInputDevice()
	{
		mRunning = false;
		mReadThread->Join();
	}

	void MIDIInputDevice::ReadThread()
	{
		ECHO_LOG_DEBUG("Starting MIDI read " << mDevice.GetActualFileName());
		bool isOpen = mDevice.IsOpen();
		mRunning = true;
		while(isOpen && mRunning)
		{
			u8 midiData[3];
			mDevice.Read(midiData,3);
			Size channel = midiData[0] & 0x0F;
			Size pitch = midiData[1];
			if(channel<16 && pitch<128)
			{
				*mState[channel][pitch] = midiData[2] / 127.f;
				ECHO_LOG_DEBUG("MIDI: " << channel << ":" << pitch << " " << *mState[channel][pitch]);
			}
			isOpen = mDevice.IsOpen();
		}
		mDevice.Close();
		ECHO_LOG_DEBUG("Finished MIDI read " << mDevice.GetActualFileName());
	}
}
