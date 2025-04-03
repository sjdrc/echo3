#ifndef _ECHOWAVAUDIOSOURCE_H_
#define _ECHOWAVAUDIOSOURCE_H_
#include <echo/Audio/AudioSource.h>
#include <echo/Resource/RIFFReader.h>

namespace Echo
{
	class WavAudioSource : public AudioSource
	{
	public:
		struct WaveFormat
		{
			WaveFormat()
			{
				mAudioFormat=0;
				mNumChannels=0;
				mSampleRate=0;
				mByteRate=0;
				mBlockAlign=0;
				mBitsPerSample=0;
			}
			~WaveFormat(){}
			unsigned short  mAudioFormat;
			unsigned short  mNumChannels;
			unsigned int	mSampleRate;
			unsigned int	mByteRate;
			unsigned short  mBlockAlign;
			unsigned short  mBitsPerSample;
		};
		WaveFormat mWaveFormat;
		u32 mDataSize;
		u8* mDataBuffer;
		u32 mLastNotAppeneded;
		RIFFReader mRIFFReader;
	public:
		WavAudioSource(File waveFile);
		~WavAudioSource();
		bool _ProcessFMT(RIFFReader* reader);
		bool _ProcessDATA(RIFFReader* reader);
		void Restart();
		u32 GetSampleRate();
		u32 GetBitsPerSample();
		Seconds GetLength();
		u32 GetNumChannels();
		u32 Read(void* rawBuffer, u32 numBytes);
		u32 UpdateBuffer(AudioBuffer& buffer, u32 numBytes);
	};
}
#endif
