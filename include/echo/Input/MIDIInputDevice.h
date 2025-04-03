#ifndef _ECHOMIDIINPUTDEVICE_H_
#define _ECHOMIDIINPUTDEVICE_H_
#include <echo/Input/InputDevice.h>
#include <echo/FileSystem/File.h>

namespace Echo
{
	class Thread;
	/**
	 * A general purpose MIDI device.
	 * This exposes inputs for all channels and all inputs and just makes the data available as an Echo InputDevice.
	 * Each Channel+Input has a float value of f32 representing their
	 * I referenced this page https://www.cs.cmu.edu/~music/cmsip/readings/MIDI%20tutorial%20for%20programmers.html which stated:
	 * > The pitch value determines the frequency of the note to be played. It goes from 0 to 127, with the middle C note being represented by the value of 60:
	 * The input mappings are:
	 * 	Input<f32>(Channel:Pitch)
	 * The first version of this runs the input on another thread and whatever the state of the given input when input is read is what it is. That means you
	 * won't get buffered input - i.e. button pressed then release as X then 0 (where X is the velocity), you'll just receive the last update position.
	 */
	class MIDIInputDevice : public InputDevice
	{
	public:
		MIDIInputDevice(const std::string& name, File device);
		~MIDIInputDevice();
	private:
		void ReadThread();
		unique_ptr<Thread> mReadThread;
		File mDevice;
		bool mRunning;
		shared_ptr<f32> mState[16][128];
	};
}
#endif
