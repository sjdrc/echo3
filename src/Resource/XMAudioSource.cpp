#include <echo/Resource/XMAudioSource.h>
#include <echo/Audio/AudioBuffer.h>
#include <iostream>

namespace Echo
{
	XMAudioSource::XMAudioSource(File xmFile) : mModFile(0), mModFileMem(0), mSampleData(0), mSampleDataSize(0)
	{
		mLastNotAppeneded=0;
		if(xmFile.IsOpen())
		{
			u32 size=xmFile.GetSize();
			mModFileMem=new u8[size];
			xmFile.Read(mModFileMem,1,size);
			mSampleDataSize = (44100*2*2)*5; //44100 samples a second, 2 channels, 16 bits a sample * seconds.
			mSampleData=new u16[mSampleDataSize];
			mModFile=ModPlug_Load(mModFileMem, size);
		}else
		{
			ECHO_LOG_ERROR("Cannot read XM track. " << xmFile.GetActualFileName() << " is not open");
		}
	}

	XMAudioSource::~XMAudioSource()
	{
		ModPlug_Unload(mModFile);
		delete [] mModFileMem;
		delete [] mSampleData;
	}

	void XMAudioSource::Restart()
	{
		if(mModFile)
		{
			ModPlug_Seek(mModFile,0);
		}
	}

	u32 XMAudioSource::GetSampleRate()
	{
		if(mModFile)
		{
			ModPlug_Settings settings;
			ModPlug_GetSettings(&settings);
			return settings.mFrequency;
		}
		return 0;
	}

	u32 XMAudioSource::GetBitsPerSample()
	{
		if(mModFile)
		{
			ModPlug_Settings settings;
			ModPlug_GetSettings(&settings);
			return settings.mBits;
		}
		return 0;
	}

	Seconds XMAudioSource::GetLength()
	{
		f32 l=0;
		if(mModFile)
		{
			//Get the length of the mod, in milliseconds
			l=static_cast<f32>(ModPlug_GetLength(mModFile));
			//Convert the length to seconds.
			l/=1000.0f;
		}
		return Seconds(l);
	}

	u32 XMAudioSource::GetNumChannels()
	{
		if(mModFile)
		{
			ModPlug_Settings settings;
			ModPlug_GetSettings(&settings);
			return settings.mChannels;		
		}
		return 1;
	}
		
	u32 XMAudioSource::UpdateBuffer(AudioBuffer& buffer, u32 numBytesToRead)
	{
		if(mModFile)
		{
			u32 readSize = std::min(numBytesToRead, mSampleDataSize);
			//Normal stream operation.
			ModPlug_Read(mModFile, &(mSampleData[mLastNotAppeneded]), readSize-mLastNotAppeneded);
			u32 bytesRead = buffer.Append(mSampleData,readSize);
			//The number of bytes not appended is the requested amount minus the amount read.
			mLastNotAppeneded=numBytesToRead-bytesRead;
			return bytesRead;
		}
		return 0;
	}
}
