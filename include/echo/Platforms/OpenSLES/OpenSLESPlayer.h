#ifndef _ECHOOPENSLESPLAYER_H_
#define _ECHOOPENSLESPLAYER_H_
#include <echo/Platforms/OpenSLES/OpenSLESAudioBuffer.h>

namespace Echo
{
	class OpenSLESPlayer
	{
	public:
		OpenSLESPlayer(u32 frequency, u32 sampleBitDepth, u32 numChannels, SLEngineItf engineInterface, SLObjectItf outputMixObject, OpenSLESAudio& audioManager);
		~OpenSLESPlayer();

		//Put dataSize bytes into the buffer at the write position
		bool IsLooping();
		bool IsPlaying();
		void SetPlayPosition(u32 pos);
		f32 GetVolume();

		//Control functions
		//Enable or disable looping. Don't forget to set mLoop
		void SetLoop(bool loop);
		//Set buffer volume
		void SetVolume(f32 v);
		//Set Pan
		void Pan(f32 pan); //0 full min, 0.5 centre, 1.0 full max
		//Play buffer
		void Play();
		//Stop buffer
		void Stop();
		//Pause buffer
		void Pause();
		u32 GetNumberOfBufferSlotsAvailable() const
		{
			return mNumberOfBufferSlotsAvailable;
		}
		const SLDataFormat_PCM& GetDataFormat() const
		{
			return mDataFormat;
		}
		void SetCurrentBuffer(OpenSLESAudioBuffer* buffer)
		{
			mCurrentBuffer = buffer;
		}
		OpenSLESAudioBuffer* GetCurrentBuffer() const
		{
			return mCurrentBuffer;
		}
		u32 GetPositionInMilliseconds();
		u32 Enqueue(void* buffer, u32 dataSize);
	private:
		void AcquireSLInterfaces();
		bool mPlaying;
		bool mPaused;
		bool mLoop;
		bool mVolume;
		u32 mWritePosition;
		u32 mNumberOfBufferSlotsAvailable;
		void NotifyBufferSlotAvailable()
		{
			++mNumberOfBufferSlotsAvailable;
		}
		void CheckError(SLresult result);

		typedef struct CallbackContext_
		{
			OpenSLESPlayer* mPlayer;
		} CallbackContext;

		SLEngineItf mEngineInterface;
		SLObjectItf mOutputMixObject;
		SLObjectItf mPlayerObject;
		SLPlayItf mPlayInterface;
		SLSeekItf mSeekInterface;
		SLBufferQueueItf mBufferQueueInterface;
		SLDataLocator_BufferQueue mBufferQueue;
		//SLDataLocator_Address mDataLocator;
		SLDataFormat_PCM mDataFormat;
		SLDataSource mDataSource;
		SLVolumeItf mVolumeInterface;
		SLDataLocator_OutputMix mOutputMixLocator;
		SLDataSink mDataSink;
		CallbackContext mCallbackContext;
		OpenSLESAudioBuffer* mCurrentBuffer;
		u32 mFrequency;
		u32 mSampleBitDepth;
		u32 mNumberOfChannels;
		OpenSLESAudio& mAudioManager;

		static void BufferQueueCallback(SLBufferQueueItf queueItf, void *context);

	};
}
#endif
