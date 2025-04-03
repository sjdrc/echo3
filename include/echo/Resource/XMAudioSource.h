#ifndef _ECHOXMAUDIOSOURCE_H_
#define _ECHOXMAUDIOSOURCE_H_
#include <echo/Types.h>
#include <echo/Audio/AudioSource.h>
#include <echo/FileSystem/File.h>
#include <stdint.h>
#include <libmodplug/stdafx.h>
#include <libmodplug/modplug.h>
#include <libmodplug/sndfile.h>
#undef ERROR
#undef PlaySound
#include <string>

namespace Echo
{
	struct _ModPlugFile
	{
		CSoundFile mSoundFile;
	};

	/**
	 * XM file format audio source.
	 */
	class XMAudioSource : public AudioSource
	{
	public:
		XMAudioSource(File xmFile);
		~XMAudioSource();
		void Restart();
		u32 GetSampleRate();
		u32 GetBitsPerSample();
		Seconds GetLength();
		u32 GetNumChannels(); //Mono (1) or Stereo (2)

		Seconds GetRequiredBufferLengthInSeconds()
		{
			return Seconds(0.4f);
		}

		u32 UpdateBuffer(AudioBuffer& buffer, u32 numBytes);
	private:
		ModPlugFile* mModFile;
		u8* mModFileMem;
		u16 *mSampleData;
		u32 mSampleDataSize;
		u32 mLastNotAppeneded;
	};
}
#endif
