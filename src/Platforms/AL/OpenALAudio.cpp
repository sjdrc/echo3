#include <echo/Platforms/AL/OpenALAudio.h>
#include <echo/Platforms/AL/OpenALAudioBuffer.h>
#include <echo/Audio/AudioSource.h>
#include <iostream>

namespace Echo
{
	OpenALAudio::OpenALAudio()
	{
	}

	OpenALAudio::~OpenALAudio()
	{	
		_CleanUpOpenAL();
	}

	void OpenALAudio::_CleanUpOpenAL()
	{
		alcMakeContextCurrent(NULL);
		if(mContext)
		{
			alcDestroyContext(mContext);
		}

		if(mDevice)
		{
			alcCloseDevice(mDevice);
		}
	}

	bool OpenALAudio::_Initialise()
	{
		mDevice = alcOpenDevice(NULL); // select the "preferred device"
		if(mDevice)
		{
			mContext=alcCreateContext(mDevice,NULL);
			alcMakeContextCurrent(mContext);
		}
		return true;
	}

	shared_ptr<AudioBuffer> OpenALAudio::CreateBuffer(u32 numSamples, u32 frequency, u32 sampleBitDepth, u32 numChannels)
	{
		shared_ptr<OpenALAudioBuffer> exabuffer(new OpenALAudioBuffer(*this, numSamples, frequency, sampleBitDepth, numChannels, false));
		exabuffer->SetVolume(1.0f);
		return exabuffer;
	}

	shared_ptr<AudioBuffer> OpenALAudio::CreateStreamBuffer(u32 numSamples, u32 frequency, u32 sampleBitDepth, u32 numChannels)
	{
		shared_ptr<OpenALAudioBuffer> exabuffer(new OpenALAudioBuffer(*this, numSamples, frequency, sampleBitDepth, numChannels, true));
		exabuffer->SetVolume(1.0f);
		return exabuffer;
	}
}
