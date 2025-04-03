#ifndef _ECHOOPENALAUDIOBUFFER_H_
#define _ECHOOPENALAUDIOBUFFER_H_
#include <echo/Audio/AudioBuffer.h>
#include <AL/al.h>
#include <string>
#include <map>
#include <list>

namespace Echo
{
	class OpenALAudioBuffer : public AudioBuffer
	{
	private:
		bool mPlaying;
		bool mPaused;
		bool mLoop;
		bool mDisableLoop;						/// Prevent loop from being set on the buffer, buffers used for
												/// streaming should have this set to true so played buffers
												/// can be reused.
		f32 mVolume;
		u32 mWritePosition;
		u32 mPlayPosition;
		std::map<ALuint, u32> mBufferSizes;
		std::list<ALuint> mUnusedBuffers;
		std::list<ALuint> mAllBuffers;
		ALuint mALSource;
		ALenum mALFormat;
		void _CleanUpBuffers();
		void _CleanUpSource();
		ALenum GetALFormat();
		bool IsALSourcePlaying();
	private:
		friend class OpenALAudio;
		static void _DisplayALError(const std::string& tag, ALenum errorCode);
	public:
		OpenALAudioBuffer(Audio& audio, u32 numSamples, u32 frequency, u32 sampleBitDepth, u32 numChannels, bool disableLoop);
		virtual ~OpenALAudioBuffer();
		//Implementation specific functions

		void SetSource(shared_ptr<AudioSource> source);

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
		
		/**
		 * Reset.
		 */
		void Reset();
	};
}
#endif
