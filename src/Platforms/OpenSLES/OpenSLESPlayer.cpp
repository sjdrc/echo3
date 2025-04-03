#include <echo/Platforms/OpenSLES/OpenSLESAudio.h>
#include <echo/Platforms/OpenSLES/OpenSLESAudioBuffer.h>
#include <echo/Platforms/OpenSLES/OpenSLESPlayer.h>

#include <iostream>

namespace Echo
{
	OpenSLESPlayer::OpenSLESPlayer(u32 frequency, u32 sampleBitDepth, u32 numChannels,
									 SLEngineItf engineInterface, SLObjectItf outputMixObject,
									 OpenSLESAudio& audioManager) :
	mPlaying(false),
	mPaused(false),
	mLoop(false),
	mVolume(1.0f),
	mWritePosition(0),
	mEngineInterface(engineInterface),
	mOutputMixObject(outputMixObject),
	mPlayerObject(0),
	mPlayInterface(0),
	mSeekInterface(0),
	mVolumeInterface(),
	mCurrentBuffer(0),
	mFrequency(frequency),
	mSampleBitDepth(sampleBitDepth),
	mNumberOfChannels(numChannels),
	mAudioManager(audioManager)
	{
		AcquireSLInterfaces();
	}

	OpenSLESPlayer::~OpenSLESPlayer()
	{
		//ECHO_LOG_DEBUG("Destroy OpenSLESPlayer");
		//Prevent callbacks that might get fired when a buffer stops from calling any methods.
		mCallbackContext.mPlayer = 0;
		if(mCurrentBuffer)
		{
			mCurrentBuffer->Stop();
		} else
		{
			Stop();
		}
		if(mPlayerObject)
		{
			// Destroy the player
			(*mPlayerObject)->Destroy(mPlayerObject);
		}
		mAudioManager.PlayerDestroyed(this);
	}

	void OpenSLESPlayer::AcquireSLInterfaces()
	{
		//ECHO_LOG_DEBUG("OpenSLESPlayer::AcquireSLInterfaces()");
		if(!mEngineInterface)
		{
			ECHO_LOG_ERROR("OpenSLESAudioBuffer not initialised properly. mEngineInterface is null");
			return;
		}
		if(!mOutputMixObject)
		{
			ECHO_LOG_ERROR_LOCATION"OpenSLESAudioBuffer not initialised properly. mOutputMixObject is null");
			return;
		}

		// configure audio source
		//mDataLocator.locatorType = SL_DATALOCATOR_ADDRESS;
		//mDataLocator.pAddress = mBuffer;
		//mDataLocator.length = GetBufferSize();

		mBufferQueue.locatorType = SL_DATALOCATOR_BUFFERQUEUE;
		mBufferQueue.numBuffers = 4; //GetBufferSize() / GetBytesPerUpdate();
		mNumberOfBufferSlotsAvailable = 4;

		mDataFormat.formatType = SL_DATAFORMAT_PCM;
		mDataFormat.numChannels = mNumberOfChannels;
		mDataFormat.samplesPerSec = mFrequency*1000; // (milliHertz), common rates can be found by searching for SL_SAMPLINGRATE_8
		mDataFormat.bitsPerSample = (SLuint16) mSampleBitDepth;
		mDataFormat.containerSize = (SLuint16) mSampleBitDepth;
		mDataFormat.channelMask = mNumberOfChannels == 1 ? SL_SPEAKER_FRONT_CENTER : 0;
		mDataFormat.endianness = SL_BYTEORDER_LITTLEENDIAN;

		mDataSource.pLocator = &mBufferQueue;
		mDataSource.pFormat = &mDataFormat;

		// configure audio sink
		mOutputMixLocator.locatorType = SL_DATALOCATOR_OUTPUTMIX;
		mOutputMixLocator.outputMix = mOutputMixObject;
		mDataSink.pLocator = &mOutputMixLocator;
		mDataSink.pFormat = NULL;

		// create audio player
		const SLInterfaceID ids[2] = {SL_IID_BUFFERQUEUE, /*SL_IID_SEEK, */SL_IID_VOLUME};
		const SLboolean req[2] = {SL_BOOLEAN_TRUE, /*SL_BOOLEAN_TRUE, */SL_BOOLEAN_TRUE};

		//ECHO_LOG_DEBUG("OpenSLESAudioBuffer - CreateAudioPlayer()");

		CheckError((*mEngineInterface)->CreateAudioPlayer(mEngineInterface, &mPlayerObject, &mDataSource, &mDataSink, 2, ids, req));

		if(!mPlayerObject)
		{
			ECHO_LOG_ERROR("OpenSLESPlayer - could not create player.");
			return;
		}

		// realize the player
		//ECHO_LOG_DEBUG("OpenSLESPlayer - Realize player");

		CheckError((*mPlayerObject)->Realize(mPlayerObject, SL_BOOLEAN_FALSE));

		// get the play interface
		//ECHO_LOG_DEBUG("OpenSLESPlayer - Get play interface");
		CheckError((*mPlayerObject)->GetInterface(mPlayerObject, SL_IID_PLAY, &mPlayInterface));


		// get the buffer queue interface
		CheckError((*mPlayerObject)->GetInterface(mPlayerObject, SL_IID_BUFFERQUEUE, &mBufferQueueInterface));

		mCallbackContext.mPlayer = this;

		CheckError((*mBufferQueueInterface)->RegisterCallback(mBufferQueueInterface, &OpenSLESPlayer::BufferQueueCallback, &mCallbackContext));

		// register callback on the buffer queue
		//result = (*bqPlayerBufferQueue)->RegisterCallback(bqPlayerBufferQueue, bqPlayerCallback, NULL);

		// get the effect send interface
		//result = (*mPlayerObject)->GetInterface(mPlayerObject, SL_IID_EFFECTSEND, &bqPlayerEffectSend);

		// get the volume interface
		//ECHO_LOG_DEBUG("OpenSLESPlayer - Get volume interface");
		CheckError((*mPlayerObject)->GetInterface(mPlayerObject, SL_IID_VOLUME, &mVolumeInterface));

		//ECHO_LOG_DEBUG("OpenSLESAudioBuffer - Get seek interface");
		//	CheckError((*mPlayerObject)->GetInterface(mPlayerObject, SL_IID_SEEK, &mSeekInterface));

		//SetLoop(mLoop);

	}

	bool OpenSLESPlayer::IsLooping()
	{
		return mLoop;
	}

	bool OpenSLESPlayer::IsPlaying()
	{
		return mPlaying;
	}

	f32 OpenSLESPlayer::GetVolume()
	{
		return mVolume;
	}

	void OpenSLESPlayer::SetVolume(f32 v)
	{
		if(v > 1.0f)
			v = 1.0f;
		if(v < 0.0f)
			v = 0.0f;
		mVolume = v;
		//SLresult result = (*mVolumeInterface)->SetVolumeLevel(mVolumeInterface, -300);
		//assert(SL_RESULT_SUCCESS == result);
		//(void) result;
	}

	void OpenSLESPlayer::SetPlayPosition(u32 pos)
	{
		ECHO_LOG_ERROR("Feature not supported");
	}

	u32 OpenSLESPlayer::GetPositionInMilliseconds()
	{
		SLmillisecond positionInMilliseconds = 0;
		if(mPlayInterface)
		{
			CheckError((*mPlayInterface)->GetPosition(mPlayInterface, &positionInMilliseconds));
		}
		return static_cast<u32> (positionInMilliseconds);
	}

	void OpenSLESPlayer::Pan(f32 pan)
	{
		ECHO_LOG_ERROR("Feature not supported");
	}

	void OpenSLESPlayer::SetLoop(bool loop)
	{
		mLoop = loop;
		if(mSeekInterface)
		{
			CheckError((*mSeekInterface)->SetLoop(mSeekInterface, mLoop ? 1 : 0, 0, SL_TIME_UNKNOWN));
		}
	}

	void OpenSLESPlayer::Play()
	{
		mPlaying = true;
		mPaused = false;
		//ECHO_LOG_DEBUG("Play");
		if(mPlayInterface)
		{
			CheckError((*mPlayInterface)->SetPlayState(mPlayInterface, SL_PLAYSTATE_PLAYING));
		}
	}

	void OpenSLESPlayer::Stop()
	{
		mPlaying = false;
		//ECHO_LOG_DEBUG("Stop");
		if(mPlayInterface)
		{
			mCurrentBuffer = 0;
			CheckError((*mPlayInterface)->SetPlayState(mPlayInterface, SL_PLAYSTATE_STOPPED));
			mAudioManager.PlayerFinished(this);
		}
	}

	void OpenSLESPlayer::Pause()
	{
		if(IsPlaying())
		{
			mPlaying = false;
			mPaused = true;
			//ECHO_LOG_DEBUG("Pause");
			CheckError((*mPlayInterface)->SetPlayState(mPlayInterface, SL_PLAYSTATE_PAUSED));
		}
	}

	void OpenSLESPlayer::CheckError(SLresult result)
	{
		assert(SL_RESULT_SUCCESS == result);
		(void) result;
	}

	u32 OpenSLESPlayer::Enqueue(void* buffer, u32 dataSize)
	{
		if(GetNumberOfBufferSlotsAvailable() == 0)
		{
			return 0;
		}
		//ECHO_LOG_DEBUG("Enqueue: " << dataSize);
		mNumberOfBufferSlotsAvailable--;
		CheckError((*mBufferQueueInterface)->Enqueue(mBufferQueueInterface, buffer, dataSize));
		return dataSize;
	}


	void OpenSLESPlayer::BufferQueueCallback(SLBufferQueueItf queueItf, void *context)
	{
		CallbackContext* cc = reinterpret_cast<CallbackContext*> (context);
		OpenSLESPlayer* player = cc->mPlayer;
		if(player)
		{
			player->mNumberOfBufferSlotsAvailable++;
			OpenSLESAudioBuffer* buffer = player->GetCurrentBuffer();
			if(buffer && (player->mBufferQueue.numBuffers == player->mNumberOfBufferSlotsAvailable))
			{
				//The buffer for this player hasn't got any more data so we'll stop it.
				buffer->Stop();
			}
		}
	}
}
