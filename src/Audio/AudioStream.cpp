#include <echo/Audio/Audio.h>
#include <echo/Audio/AudioStream.h>
#include <echo/Audio/AudioBuffer.h>
#include <echo/Audio/AudioSource.h>

#include <iostream>

namespace Echo
{
	AudioStream::AudioStream(Audio& audio, shared_ptr<AudioSource> source, Seconds bufferLength, Size bufferSegments) : mAudio(audio), mSource(source)
	{
		mLoop=true;
		mSecondsLeftUntilUpdate=Seconds(0);
		mNumberOfSegments = bufferSegments;
		mSegmentSizeInBytes = 0;
		
		assert(mSource && "AudioStream(): source must be valid");
		assert((mNumberOfSegments>=2) && "There must be at least 2 segments for streaming to work.");
		if(mNumberOfSegments<2)
		{
			mNumberOfSegments = 2;
		}
		
		u32 numSamples=mSource->ConvertSecondsToSamples(bufferLength);
		if(numSamples==0)
		{
			ECHO_LOG_ERROR("AudioStream() Provided AudioSource reports it needs " << numSamples << " for a buffer length of " << bufferLength.count() << " seconds which is invalid.");
		}else
		{
			mBuffer=mAudio.CreateStreamBuffer(numSamples,mSource->GetSampleRate(),mSource->GetBitsPerSample(),mSource->GetNumChannels());
			assert(mBuffer && "Audio system failed to create a buffer. This is usually a result of an incomplete implementation.");
			if(mBuffer)
			{
				mSegmentSizeInBytes = mBuffer->ConvertSamplesToBytes(numSamples / mNumberOfSegments);
				mBuffer->SetLoop(true);
				//Perform the first full update.
				mSource->UpdateBuffer(*mBuffer,mBuffer->GetBufferSize());
			}else
			{
				ECHO_LOG_ERROR("AudioStream() Unable to create buffer for AudioStream.");
			}
		}
	}

	AudioStream::~AudioStream()
	{
		if(mSource)
		{
			mAudio.StreamClosed(this);
		}
	}

	void AudioStream::Play(bool loop)
	{
		SetLoop(loop);
		Play();
	}

	void AudioStream::Restart()
	{
		assert(mBuffer && "AudioStream should have a valid buffer!");
		if(!mBuffer)
		{
			return;
		}
		if(mSource)
		{
			mSource->Restart();
		}
		mSecondsLeftUntilUpdate=Seconds(0);
		bool wasPlaying=IsPlaying();
		Stop();	
		mBuffer->SetPlayPosition(mBuffer->GetWritePosition());
		Update(Seconds(0));
		if(wasPlaying)
		{
			Play();
		}
	}
	
	void AudioStream::SetLoop(bool loop)
	{
		mLoop = loop;
	}

	void AudioStream::SetVolume(f32 v)
	{
		assert(mBuffer && "AudioStream should have a valid buffer!");
		if(mBuffer)
		{
			mBuffer->SetVolume(v);
		}
	}
	
	f32 AudioStream::GetVolume()
	{
		assert(mBuffer && "AudioStream should have a valid buffer!");
		if(mBuffer)
		{
			return mBuffer->GetVolume();
		}
		return 0.f;
	}
	
	void AudioStream::Pan(f32 panX)
	{
		assert(mBuffer && "AudioStream should have a valid buffer!");
		if(mBuffer)
		{
			mBuffer->Pan(panX);
		}
	}
	
	//Play
	void AudioStream::Play()
	{
		assert(mBuffer && "AudioStream should have a valid buffer!");
		if(mBuffer)
		{
			mBuffer->Play();
		}
	}
	
	void AudioStream::Stop()
	{
		assert(mBuffer && "AudioStream should have a valid buffer!");
		if(mBuffer)
		{
			mBuffer->Stop();
		}
	}
	
	void AudioStream::Pause()
	{
		assert(mBuffer && "AudioStream should have a valid buffer!");
		if(mBuffer)
		{
			mBuffer->Pause();
		}
	}

	void AudioStream::Update(Seconds lastFrameTime)
	{
		if(mSource && mBuffer && mSegmentSizeInBytes!=0)
		{
			mSecondsLeftUntilUpdate-=lastFrameTime;
			if(mSecondsLeftUntilUpdate<=Seconds(0))
			{
				Size writeLength = mBuffer->GetWriteSpaceInBytes();
				Size updates = writeLength / mSegmentSizeInBytes;
				u32 written=0;
				while(updates>0)
				{
					written += mSource->UpdateBuffer(*mBuffer,mSegmentSizeInBytes);
					updates--;
				}
				mSecondsLeftUntilUpdate += mBuffer->ConvertBytesToSeconds(mSegmentSizeInBytes);
			}
		}
	}
	
	bool AudioStream::IsLooping()
	{
		return mLoop;
	}
	bool AudioStream::IsPlaying()
	{
		if(mBuffer)
		{
			return mBuffer->IsPlaying();
		}
		return false;
	}
	
	Seconds AudioStream::GetPlayPosition()
	{
		return Seconds(0.f);
	}
	
	void SetPlayPosition(Seconds pos)
	{
		//Not yet
	}
	
}
