#include <echo/Platforms/OpenSLES/OpenSLESAudio.h>
#include <echo/Platforms/OpenSLES/OpenSLESAudioBuffer.h>
#include <echo/Platforms/OpenSLES/OpenSLESPlayer.h>
#include <echo/Audio/AudioSource.h>

#include <assert.h>
#include <vector>
#include <iostream>

namespace Echo
{
	OpenSLESAudioBuffer::OpenSLESAudioBuffer(u32 numSamples, u32 frequency, u32 sampleBitDepth, u32 numChannels, OpenSLESAudio& audioManager) :
		AudioBuffer(audioManager, numSamples, frequency, sampleBitDepth, numChannels),
		mPlaying(false),
		mPaused(false),
		mLoop(false),
		mVolume(1.0f),
		mWritePosition(0),
		mPlayer(0),
		mAudioManager(audioManager)
	{
		if(GetBufferSize() > 0)
		{
			mBuffer = new u8[GetBufferSize()];
		} else
		{
			mBuffer = 0;
		}
	}

	OpenSLESAudioBuffer::~OpenSLESAudioBuffer()
	{
		//ECHO_LOG_DEBUG("Destroy OpenSLESAudioBuffer");
		Stop();
	}

	bool OpenSLESAudioBuffer::IsLooping()
	{
		return mLoop;
	}

	bool OpenSLESAudioBuffer::IsPlaying()
	{
		return mPlaying;
	}

	u32 OpenSLESAudioBuffer::Append(void* data, u32 dataSize)
	{
		// If the buffer is a stream but doesn't have a player, don't update.
		// A stream's first buffer update will be performed before the source is
		// assigned so this will appear as though it is a normal buffer.
		if(!mBuffer)
		{
			ECHO_LOG_ERROR_LOCATIONv("OpenSLESAudioBuffer - play interface or buffer null");
			return 0;
		}
		if(mPlayer && mPlayer->GetNumberOfBufferSlotsAvailable() == 0)
		{
			//No spare buffer slots available.
			return 0;
		}
		u32 positionInMilliseconds = 0;
		if(mPlayer)
		{
			positionInMilliseconds = mPlayer->GetPositionInMilliseconds();
		}
		u32 writeLength = 0;
		u32 bytesWritten = 0;
		u32 bufferPosition = (positionInMilliseconds * 1000) * GetSampleRate() * GetBytesPerSample() * GetNumChannels();

		if(mWritePosition < bufferPosition)
			writeLength = bufferPosition - mWritePosition;
		else
			writeLength = GetBufferSize() - mWritePosition; // + bufferPosition;

		if(dataSize > writeLength)
			dataSize = writeLength;

		u8* byteData = reinterpret_cast<u8*>(data);
		std::copy(byteData,byteData +dataSize,&mBuffer[mWritePosition]);
		bytesWritten += dataSize;

		//Does this need to return the number of bytes queued? It will either b 0 or the dataSize.
		// Maybe we can always successfully append, but sounds may not play?
		// Or perhaps always append for samples, otherwise this behaviour for streams?
		// Lets focus on streams first
		if(mPlayer)
		{
			 mPlayer->Enqueue(&mBuffer[mWritePosition], dataSize);
		}

		mWritePosition += bytesWritten;
		if(mWritePosition >= GetBufferSize())
		{
			mWritePosition -= GetBufferSize();
		}
		return bytesWritten;

	}

	f32 OpenSLESAudioBuffer::GetVolume()
	{
		return mVolume;
	}

	void OpenSLESAudioBuffer::SetVolume(f32 v)
	{
		if(v>1.0f)
			v=1.0f;
		if(v<0.0f)
			v=0.0f;
		mVolume = v;
		if(mPlayer)
		{
			mPlayer->SetVolume(v);
		}
	}

	u32 OpenSLESAudioBuffer::GetWritePosition()
	{
		return 0;
	}

	u32 OpenSLESAudioBuffer::GetPlayPosition()
	{
		return 0;
	}

	void OpenSLESAudioBuffer::SetPlayPosition(u32 pos)
	{
		ECHO_LOG_ERROR("Feature not supported");
	}

	void OpenSLESAudioBuffer::Pan(f32 pan)
	{
		ECHO_LOG_ERROR("Feature not supported");
	}

	void OpenSLESAudioBuffer::SetLoop(bool loop)
	{
		mLoop = loop;
		if(mPlayer)
		{
			mPlayer->SetLoop(loop);
		}
	}
	
	void OpenSLESAudioBuffer::Play()
	{
		if(mPlaying)
			return;
		//Need to resume if we were paused
		mPlaying=true;
		mPaused=false;
		//ECHO_LOG_DEBUG("Play");
		if(!mPlayer)
		{
			mPlayer = mAudioManager.GetPlayer(*this, true);
		}
		if(mPlayer)
		{
			mPlayer->SetCurrentBuffer(this);
			mPlayer->Play();
			u32 enqueSize = (mWritePosition == 0) ? GetBufferSize() : mWritePosition;
			mPlayer->Enqueue(mBuffer, enqueSize);
		}
	}

	void OpenSLESAudioBuffer::Stop()
	{
		mPlaying=false;
		//ECHO_LOG_DEBUG("Stop");
		if(mPlayer)
		{
			OpenSLESPlayer* player = mPlayer;
			mPlayer = 0;
			player->Stop();
		}
	}

	void OpenSLESAudioBuffer::Pause()
	{
		if(IsPlaying())
		{
			mPlaying=false;
			mPaused=true;
			//ECHO_LOG_DEBUG("Pause");
			if(mPlayer)
			{
				mPlayer->Pause();
			}
		}
	}
	
	void OpenSLESAudioBuffer::Reset()
	{
		
	}
}
