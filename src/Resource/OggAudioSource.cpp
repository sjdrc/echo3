#include <echo/Resource/OggAudioSource.h>
#include <echo/Audio/AudioBuffer.h>
#include <echo/Audio/Audio.h>
#include <boost/scope_exit.hpp>
#include <iostream>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

namespace Echo
{
	size_t OVRead(void *ptr, size_t size, size_t nmemb, void *datasource)
	{
		File* f=reinterpret_cast<File*>(datasource);
		size_t r=(size_t)f->Read(ptr,(u32)size,(u32)nmemb);
		return r;
	}

	int OVSeek(void *datasource, ogg_int64_t offset, int whence)
	{
		File* f=reinterpret_cast<File*>(datasource);
		size_t r=0;
		switch(whence)
		{
		case SEEK_SET:
			{
				r=f->Seek((u32)offset);
				r=0;
			}break;
		case SEEK_CUR:
			{
				s64 p=(s64)f->GetPosition();
				r=(size_t)f->Seek(((u32)p+offset));
				r=0;
			}break;
		case SEEK_END:
			{
				s64 s=f->GetSize();
				f->Seek((u32)(s-offset));
				r=0;
			}break;
		}

		return (int)r;
	}

	long OVTell(void *datasource)
	{
		File* f=reinterpret_cast<File*>(datasource);
		long p=(long)f->GetPosition();
		return p;
	}

	int OVClose(void *datasource)
	{
		File* f=reinterpret_cast<File*>(datasource);
		f->Close();
		return 0;
	}

	OggAudioSource::OggAudioSource(File oggFile)
	{
		mDataBuffer=0;
		mLastNotAppended=0;
		mLastReadAmount=0;
		mDataSize=0;
		mCurrentSection=0;
		
		// We use these defaults. If someone requests for support for 8 bit and/or unsigned samples then
		// we can make these configurable.
		mBytesPerSample=2;			//Typically this is 2 for 16bit samples but it could also be set to 1 for 8 bit samples
		mSignedSamples=1;			//1 for signed, 0 for unsigned.
		
		SetFile(oggFile);
	}

	OggAudioSource::~OggAudioSource()
	{
		delete [] mDataBuffer;
		mOggMutex.Lock();
		ov_clear(&mOggVorbisFile);
		mOggMutex.Unlock();
		mFile.Close();
	}

	void OggAudioSource::SetFile(File oggFile)
	{
		mCallBacks.close_func=OVClose;
		mCallBacks.read_func=OVRead;
		mCallBacks.seek_func=OVSeek;
		mCallBacks.tell_func=OVTell;
		mLastReadAmount=0;
		mLastNotAppended=0;

		if(!oggFile.IsOpen())
		{
			return;
		}
		mFile = oggFile;
		mFile.Seek(0);
		mOggMutex.Lock();
		int r=ov_open_callbacks(&mFile, &mOggVorbisFile, 0, 0, mCallBacks);
		mOggMutex.Unlock();
		if(r<0)
		{
			switch(r)
			{
			case OV_EREAD:
				ECHO_LOG_ERROR("OV_EREAD - A read from media returned an error.");
				break;
			case OV_ENOTVORBIS:
				ECHO_LOG_ERROR("OV_ENOTVORBIS - Bitstream does not contain any Vorbis data.");
				break;
			case OV_EVERSION:
				ECHO_LOG_ERROR("OV_EVERSION - Vorbis version mismatch.");
				break;
			case OV_EBADHEADER:
				ECHO_LOG_ERROR("OV_EBADHEADER - Invalid Vorbis bitstream header.");
				break;
			case OV_EFAULT:
				ECHO_LOG_ERROR("OV_EFAULT - Internal logic fault; indicates a bug or heap/stack corruption.");
				break;
			}
		}
		
		//Arbitrarily have a buffer size of 1 seconds.
		mDataSize=GetNumChannels();
		mDataSize*=GetBitsPerSample()/8;
		mDataSize*=GetSampleRate();
		
		assert(mDataSize && "OggAudioSource mDataSize is 0");
		mLastNotAppended=0;
		mCurrentSection=0;
		delete [] mDataBuffer;
		if(mDataSize!=0)
		{
			mDataBuffer=new char[mDataSize];
		}else
		{
			mDataBuffer=0;
		}
	}

	void OggAudioSource::Restart()
	{
		mOggMutex.Lock();
		File currentFile = mFile;	//Copy the file so it remains open.
		ov_clear(&mOggVorbisFile);
		mOggMutex.Unlock();
		SetFile(currentFile);
	}

	Seconds OggAudioSource::GetLength()
	{
		return Seconds(ov_time_total(&mOggVorbisFile,-1));
	}

	u32 OggAudioSource::GetNumChannels()
	{
		if(mOggVorbisFile.vi)
			return mOggVorbisFile.vi->channels;
		return 0;
	}

	u32 OggAudioSource::UpdateBuffer(AudioBuffer& buffer, u32 numBytes)
	{
		assert(mDataBuffer && "Attempt to use an invalid buffer. mDataBuffer is 0");
		if(!mDataBuffer)
		{
			return 0;
		}
		mOggMutex.Lock();
		BOOST_SCOPE_EXIT(&mOggMutex)
		{
			mOggMutex.Unlock();
		} BOOST_SCOPE_EXIT_END

		if(numBytes > mLastNotAppended)
		{
			numBytes -= mLastNotAppended;
		}

		numBytes = std::min((mDataSize-mLastNotAppended),numBytes);

		mLastReadAmount=mLastNotAppended;
		while( mLastReadAmount <numBytes)			//Try and read the full required number of bytes
		{
			//Read enough data to fill numBytes
			s32 ram=ov_read(&mOggVorbisFile, &(mDataBuffer[mLastNotAppended]), mDataSize-mLastNotAppended,0,mBytesPerSample,mSignedSamples,&mCurrentSection);
			switch(ram)
			{
			case OV_HOLE:
				ECHO_LOG_ERROR("OV_HOLE - There was an interruption in the data. (one of: garbage between pages, loss of sync followed by recapture, or a corrupt page)");
				return 0;
				break;
			case OV_EBADLINK:
				ECHO_LOG_ERROR("OV_EBADLINK - An invalid stream section was supplied to libvorbisfile, or the requested link is corrupt.");
				return 0;
				break;
			case OV_EINVAL:
				ECHO_LOG_ERROR("OV_EINVAL - The initial file headers couldn't be read or are corrupt, or that the initial open call for vf failed.");
				return 0;
				break;
			case 0:
				{
					mOggMutex.Unlock();
					Restart();
					mOggMutex.Lock();
					numBytes=mLastReadAmount;
				}
				break;
			default:
				mLastReadAmount+=ram;
				mLastNotAppended+=ram;
				break;
			}
		}

		u32 toWrite = (mLastNotAppended > numBytes) ? numBytes : mLastNotAppended;
		u32 written = buffer.Append(&(mDataBuffer[mLastReadAmount - mLastNotAppended]), toWrite);
		mLastNotAppended -= written;

		//Move the remaining data back to the start of the buffer
		u32 mi=0;
		while(mi<mLastNotAppended)
		{
			mDataBuffer[mi]=mDataBuffer[mLastReadAmount-mLastNotAppended+mi];
			++mi;
		}

		//ECHO_LOG_DEBUG("Requested: " << numBytes << " Wrote: " << written);
		return written;
	}

	u32 OggAudioSource::GetBitsPerSample()
	{
		return mBytesPerSample * 8;
	}

	u32 OggAudioSource::GetSampleRate()
	{
		if(mOggVorbisFile.vi)
		{
			return mOggVorbisFile.vi->rate;
		}
		return 0;
	}
}
