#ifndef _ECHOOGGAUDIOSOURCE_H_
#define _ECHOOGGAUDIOSOURCE_H_

#include <echo/Types.h>
#include <echo/Audio/AudioSource.h>
#include <echo/FileSystem/File.h>
#include <vorbis/vorbisfile.h>
#include <echo/Kernel/Mutex.h>
#include <string>

namespace Echo
{
	/**
	 * Ogg file format audio source.
	 */
	class OggAudioSource : public AudioSource
	{
	private:
		ov_callbacks mCallBacks;
		OggVorbis_File mOggVorbisFile;
		File mFile;
		u32 mLastNotAppended;
		u32 mLastReadAmount;
		u32 mDataSize;
		char* mDataBuffer;
		int mCurrentSection;
		Mutex mOggMutex;
		int mBytesPerSample;
		int mSignedSamples;
	public:
		OggAudioSource(File oggFile);
		~OggAudioSource();
		void SetFile(File oggFile);
		void Restart();
		Seconds GetLength();
		u32 GetNumChannels();
		u32 UpdateBuffer(AudioBuffer& buffer, u32 numBytes);
		u32 GetBitsPerSample();
		u32 GetSampleRate();
	};
}
#endif
