#include <echo/Audio/Audio.h>
#include <echo/Audio/AudioBuffer.h>
#include <echo/Audio/AudioSource.h>

#include <iostream>

namespace Echo
{
	AudioBuffer::AudioBuffer(Audio& audio, u32 numSamples, u32 frequency, u32 sampleBitDepth, u32 numChannels) : mAudio(audio)
	{
		mNumChannels = numChannels;
		mBitsPerSample = sampleBitDepth;
		mSampleRate = frequency;
		mBufferSize = numSamples * (sampleBitDepth / 8) * numChannels;
	}

	AudioBuffer::~AudioBuffer()
	{
	}

	void AudioBuffer::Play(bool loop)
	{
		SetLoop(loop);
		Play();
	}

	void AudioBuffer::Restart()
	{
		bool wasPlaying=IsPlaying();
		Stop();	
		SetPlayPosition(GetWritePosition());
		if(wasPlaying)
		{
			Play();
		}
	}
	
	Size AudioBuffer::GetWriteSpaceInBytes()
	{
		u32 writePosition = GetWritePosition();
		u32 playPosition = GetPlayPosition();
		if ( writePosition < playPosition)
		{
			return (playPosition - writePosition);
		}
		return ((GetBufferSize() - writePosition) + playPosition);
	}
}
