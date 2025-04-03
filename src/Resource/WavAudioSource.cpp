#include <echo/Resource/WavAudioSource.h>
#include <echo/Resource/RIFFReader.h>
#include <echo/Audio/AudioBuffer.h>
#include <iostream>

namespace Echo
{
	WavAudioSource::WavAudioSource(File waveFile) : mDataSize(0), mDataBuffer(0), mLastNotAppeneded(0)
	{
		if(!mRIFFReader.SetFile(waveFile))
		{
			ECHO_LOG_ERROR("File " << waveFile.GetActualFileName() << " is not a RIFF file.");
			return;
		}
		mRIFFReader.AddChunkTypeProcessor(0x20746D66,this,&WavAudioSource::_ProcessFMT);	//'fmt ' chunk
		mRIFFReader.AddChunkTypeProcessor(0x61746164,this,&WavAudioSource::_ProcessDATA);	//'data' chunk
		//We want to know the 'fmt ' first this should take us there
		//Process to 'data' block
		while(mRIFFReader.ProcessNext() && mDataSize==0)
		{}
	}

	WavAudioSource::~WavAudioSource()
	{
		delete [] mDataBuffer;
	}

	bool WavAudioSource::_ProcessFMT(RIFFReader* reader)
	{
		mRIFFReader.Read(&mWaveFormat.mAudioFormat,1, mRIFFReader.GetCurrentChunkSize());

		if(mWaveFormat.mAudioFormat!=1)	//If not PCM
		{
			return false;
		}
		return true;
	}

	bool WavAudioSource::_ProcessDATA(RIFFReader* reader)
	{
		mDataSize=mRIFFReader.GetCurrentChunkSize();
		mDataBuffer = new u8[mDataSize];
		return true;
	}

	void WavAudioSource::Restart()
	{
		mRIFFReader.ReturnToChunkStart();
	}

	u32 WavAudioSource::GetSampleRate()
	{
		return mWaveFormat.mSampleRate;
	}

	u32 WavAudioSource::GetBitsPerSample()
	{
		return mWaveFormat.mBitsPerSample;
	}

	Seconds WavAudioSource::GetLength()
	{
		u32 length=mDataSize;
		length/=mWaveFormat.mNumChannels;
		length/=mWaveFormat.mBitsPerSample/8;
		f32 lengthSeconds=static_cast<f32>(length);
		lengthSeconds/=static_cast<f32>(mWaveFormat.mSampleRate);
		return Seconds(lengthSeconds);
	}

	u32 WavAudioSource::GetNumChannels()
	{
		return mWaveFormat.mNumChannels;
	}

	u32 WavAudioSource::Read(void* rawBuffer, u32 numBytes)
	{
		u32 numRead=mRIFFReader.Read(rawBuffer,1,numBytes);
		if(mRIFFReader.IsReadAtEndOfChunk())
			Restart();
		return numRead;
	}
	
	u32 WavAudioSource::UpdateBuffer(AudioBuffer& buffer, u32 numBytes)
	{
		u32 steps=0;
		u32 written=0;
		steps=(numBytes/mDataSize);
		for (u32 s=0; s< steps; ++s)
		{
			u32 numRead=mRIFFReader.Read(&(mDataBuffer[mLastNotAppeneded]),1,(mDataSize)-mLastNotAppeneded);
			if(mRIFFReader.IsReadAtEndOfChunk())
				Restart();
			u32 r=buffer.Append(mDataBuffer,numRead);
			written+=r;
			mLastNotAppeneded=(mDataSize)-r;
			if(mLastNotAppeneded)
				return written;
			if(mRIFFReader.IsReadAtEndOfChunk())
				Restart();
		}
		//Last step will be the remainder
		numBytes=numBytes%mDataSize;
		if(numBytes>0)
		{
			u32 numRead=mRIFFReader.Read(&(mDataBuffer[mLastNotAppeneded]),1, (numBytes)-mLastNotAppeneded);
			u32 r=buffer.Append(mDataBuffer,numRead);
			mLastNotAppeneded=(numBytes)-r;
			written+=r;
			if(mRIFFReader.IsReadAtEndOfChunk())
			{
				Restart();
				numBytes-=numRead;
				u32 numRead=mRIFFReader.Read(&(mDataBuffer[mLastNotAppeneded]),1, (numBytes)-mLastNotAppeneded);
				u32 r=buffer.Append(mDataBuffer,numRead);
				mLastNotAppeneded=(numBytes)-r;
				written+=r;
			}
		}
		return written;
	}
}
