#ifndef _ECHOOPENSLESAUDIOBUFFER_H_
#define _ECHOOPENSLESAUDIOBUFFER_H_
#include <echo/Audio/AudioBuffer.h>
#include <SLES/OpenSLES.h>

namespace Echo
{
	class OpenSLESPlayer;

	class OpenSLESAudioBuffer : public AudioBuffer
	{
	public:
		OpenSLESAudioBuffer(u32 numSamples, u32 frequency, u32 sampleBitDepth, u32 numChannels, OpenSLESAudio& audioManager);
		~OpenSLESAudioBuffer();

		//Put dataSize bytes into the buffer at the write position
		u32 Append(void* data, u32 dataSize);
		bool IsLooping();
		bool IsPlaying();
		u32 GetWritePosition();
		u32 GetPlayPosition();
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
	private:
		bool mPlaying;
		bool mPaused;
		bool mLoop;
		f32 mVolume;
		u32 mWritePosition;

		u8* mBuffer;

		OpenSLESPlayer* mPlayer;
		OpenSLESAudio& mAudioManager;
		void Reset();
	};
}
#endif
