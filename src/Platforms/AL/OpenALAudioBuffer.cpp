#include <echo/Platforms/AL/OpenALAudioBuffer.h>
#include <echo/Audio/AudioSource.h>
#include <iostream>

namespace Echo
{
	OpenALAudioBuffer::OpenALAudioBuffer(Audio& audio, u32 numSamples, u32 frequency, u32 sampleBitDepth, u32 numChannels, bool disableLoop) :
		AudioBuffer(audio,numSamples, frequency, sampleBitDepth, numChannels),
		mPlaying(false),
		mPaused(false),
		mLoop(false),
		mDisableLoop(disableLoop),
		mVolume(1.0f),
		mWritePosition(0),
		mALSource(0),
		mALFormat(0)
	{
		ALenum errorCode;
		alGetError();
		alGenSources(1,&mALSource);
		if((errorCode = alGetError()) != AL_NO_ERROR)
		{
			_DisplayALError("alGenSources:", errorCode);
			assert(mALSource && "Could not generate AL source.");
		}
	}

	OpenALAudioBuffer::~OpenALAudioBuffer()
	{
		_CleanUpBuffers();
		_CleanUpSource();
	}

	void OpenALAudioBuffer::_CleanUpSource()
	{
		ALenum errorCode;
		alGetError();
		alDeleteSources(1,&mALSource);	//delete source
		if((errorCode = alGetError()) != AL_NO_ERROR)
		{
			_DisplayALError("alDeleteSources:", errorCode);
			assert(mALSource && "Could not delete AL source.");
		}
	}

	void OpenALAudioBuffer::_CleanUpBuffers()
	{
		ALenum errorCode;
		alGetError();

		alSourcei(mALSource,AL_BUFFER,0);	//detach all buffers
		if((errorCode = alGetError()) != AL_NO_ERROR)
		{
			_DisplayALError("alSourcei :", errorCode);
			assert(mALSource && "Could not detach all buffers from source.");
		}

		//Delete all buffers.
		while(!mAllBuffers.empty())
		{
			ALuint b=mAllBuffers.front();
			alDeleteBuffers(1,&b);
			mAllBuffers.pop_front();
		}
		mUnusedBuffers.clear();
		mBufferSizes.clear();
	}

	void OpenALAudioBuffer::_DisplayALError(const std::string& tag, ALenum errorCode)
	{
		switch(errorCode)
		{
			case AL_ILLEGAL_COMMAND:	ECHO_LOG_ERROR(tag << ":AL_ILLEGAL_COMMAND || AL_INVALID_OPERATION");		break;
			case AL_OUT_OF_MEMORY:		ECHO_LOG_ERROR(tag << ":AL_OUT_OF_MEMORY");		break;
			case AL_ILLEGAL_ENUM:		ECHO_LOG_ERROR(tag << ":AL_ILLEGAL_ENUM || AL_INVALID_ENUM");			break;
			case AL_INVALID_NAME:		ECHO_LOG_ERROR(tag << ":AL_INVALID_NAME");			break;
			case AL_INVALID_VALUE:		ECHO_LOG_ERROR(tag << ":AL_INVALID_VALUE");		break;
			default: break;
		}
	}

	bool OpenALAudioBuffer::IsLooping()
	{
		return mLoop;
	}

	bool OpenALAudioBuffer::IsPlaying()
	{
		return mPlaying;
	}

	bool OpenALAudioBuffer::IsALSourcePlaying()
	{
		ALenum errorCode=0;
		ALint ss=0;
		alGetSourcei(mALSource,AL_SOURCE_STATE,&ss);
		if((errorCode = alGetError()) != AL_NO_ERROR)
		{
			_DisplayALError("alGetSourcei AL_SOURCE_STATE:", errorCode);
		}
		return(ss==AL_PLAYING);
	}

	ALenum OpenALAudioBuffer::GetALFormat()
	{
		if(mALFormat!=0)
			return mALFormat;

		if(GetNumChannels()==1)
		{
			if(GetBitsPerSample()==8)
				mALFormat=AL_FORMAT_MONO16;
			else
				mALFormat=AL_FORMAT_MONO16;
		}else
			if(GetBitsPerSample()==8)
				mALFormat=AL_FORMAT_STEREO8;
			else
				mALFormat=AL_FORMAT_STEREO16;
		return mALFormat;
	}

	u32 OpenALAudioBuffer::Append(void* data, u32 dataSize)
	{
		ALenum errorCode;

		//Find out how many buffers that have been processed by the source so far.
		ALint buffersProcessed=0;
		alGetSourcei(mALSource,AL_BUFFERS_PROCESSED,&buffersProcessed);

		//For each processed buffer find the recorded size and increase the play position.
		while(buffersProcessed>0)
		{
			ALuint bufferRemoved=0;
			alSourceUnqueueBuffers(mALSource,1,&bufferRemoved);
			if((errorCode = alGetError()) != AL_NO_ERROR)
			{
				_DisplayALError("alSourceQueueBuffers :", errorCode);
			}
			std::map<ALuint, u32>::iterator bit=mBufferSizes.find(bufferRemoved);
			if(bit!=mBufferSizes.end())
			{
				mPlayPosition+=bit->second;
				if(mPlayPosition > GetBufferSize())
					mPlayPosition -= GetBufferSize();
				mBufferSizes.erase(bit);
			}
			//Store the buffer reference for later use.
			//mUnusedBuffers.push_back(bufferRemoved);
			buffersProcessed--;
		}

		// We are going to append a buffer to the queue. If there isn't one
		// available in the unused list we will create one.
		if(mUnusedBuffers.empty())
		{
			ALuint buffer;
			alGenBuffers(1,&buffer);
			if((errorCode = alGetError()) != AL_NO_ERROR)
			{
				_DisplayALError("alGenBuffers (v):", errorCode);
				return 0;
			}

			mAllBuffers.push_back(buffer);
			mUnusedBuffers.push_back(buffer);
		}

		// Store the buffer size for use later use.
		mBufferSizes[mUnusedBuffers.back()]=dataSize;

		// Add the data to the AL buffer.
		alBufferData(mUnusedBuffers.back(),GetALFormat(),(const ALvoid*)data,(ALsizei)dataSize,(ALsizei)GetSampleRate());
		if((errorCode = alGetError()) != AL_NO_ERROR)
		{
			_DisplayALError("alBufferData:", errorCode);
			return 0;
		}
		
		// Queue the buffer in the source queue.
		alSourceQueueBuffers(mALSource,1,&mUnusedBuffers.back());
		if((errorCode = alGetError()) != AL_NO_ERROR)
		{
			_DisplayALError("alSourceQueueBuffers :", errorCode);
			return 0;
		}
		
		//Play the buffer it should be playing.
		if(mPlaying && !IsALSourcePlaying())
		{
			ECHO_LOG_DEBUG("Restarting buffer");
			Play();
		}
		
		// Remove the buffer from the unused list
		mUnusedBuffers.pop_back();

		// Move the write cursor.
		mWritePosition+=dataSize;
		if(mWritePosition >= GetBufferSize())
		{
			mWritePosition -= GetBufferSize();
		}
		return dataSize;
	}

	f32 OpenALAudioBuffer::GetVolume()
	{
		return mVolume;
	}

	void OpenALAudioBuffer::SetVolume(f32 v)
	{
		if(alIsSource(mALSource) == AL_FALSE)
		{
			return;
		}
		if(v>1.0f)
			v=1.0f;
		if(v<0.0f)
			v=0.0f;
		ALfloat minGain=0;
		ALfloat maxGain=0;
		ALfloat gain=0;
		alGetSourcef(mALSource,AL_MIN_GAIN,&minGain);
		alGetSourcef(mALSource,AL_MAX_GAIN,&maxGain);
		maxGain-=minGain;
		gain = mVolume*maxGain;
		gain+=minGain;
		alSourcef(mALSource,AL_GAIN,gain);
		ALenum errorCode = alGetError();
		if(errorCode != AL_NO_ERROR)
		{
			_DisplayALError("alGenSources:", errorCode);
		}
		mVolume = v;
	}

	u32 OpenALAudioBuffer::GetWritePosition()
	{
		return mWritePosition;
	}

	u32 OpenALAudioBuffer::GetPlayPosition()
	{
		ALint playOffset=0;
		ALenum errorCode=0;

		alGetSourcei(mALSource,AL_BYTE_OFFSET,&playOffset);
		if((errorCode = alGetError()) != AL_NO_ERROR)
		{
			_DisplayALError("alGetSourcei AL_BYTE_OFFSET:", errorCode);
		}
		return(mPlayPosition + (u32) playOffset) % GetBufferSize();
	}

	void OpenALAudioBuffer::SetPlayPosition(u32 pos)
	{
		ECHO_LOG_ERROR("Feature not supported");
	}

	void OpenALAudioBuffer::Pan(f32 pan)
	{
		ECHO_LOG_ERROR("Feature not supported");
	}

	void OpenALAudioBuffer::SetLoop(bool loop)
	{
		if(!mALSource)
		{
			ECHO_LOG_ERROR("OpenALAudioBuffer::SetLoop(): No source");
			return;
		}
		if(loop && !mDisableLoop)
		{
			alSourcei(mALSource,AL_LOOPING,AL_TRUE);
		}else
		{
			alSourcei(mALSource,AL_LOOPING,AL_FALSE);
		}
		mLoop = loop && !mDisableLoop;
	}

	void OpenALAudioBuffer::Play()
	{
		if(!mALSource)
		{
			ECHO_LOG_ERROR("OpenALAudioBuffer::Play(): No source");
			return;
		}
		ALenum errorCode;
		alSourcePlay(mALSource);
		if((errorCode = alGetError()) != AL_NO_ERROR)
		{
			_DisplayALError("alSourcePlay:", errorCode);
			return;
		}
		mPlaying=true;
		mPaused=false;
	}

	void OpenALAudioBuffer::Stop()
	{
		if(!mALSource)
		{
			ECHO_LOG_ERROR("OpenALAudioBuffer::Stop(): No source");
			return;
		}
		if(IsPlaying())
		{
			ALenum errorCode;
			alSourceStop(mALSource);
			if((errorCode = alGetError()) != AL_NO_ERROR)
			{
				_DisplayALError("alSourceStop:", errorCode);
				return;
			}
			mPlaying=false;
		}
	}

	void OpenALAudioBuffer::Pause()
	{
		if(!mALSource)
		{
			ECHO_LOG_ERROR("OpenALAudioBuffer::Pause(): No source");
			return;
		}
		if(IsPlaying())
		{
			ALenum errorCode;
			alSourcePause(mALSource);
			if((errorCode = alGetError()) != AL_NO_ERROR)
			{
				_DisplayALError("alSourcePause:", errorCode);
				return;
			}
			mPlaying=false;
			mPaused=true;
		}
	}

	void OpenALAudioBuffer::Reset()
	{
		_CleanUpBuffers();
	}
}
