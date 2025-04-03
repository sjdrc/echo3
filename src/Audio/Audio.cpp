#include <echo/Audio/Audio.h>
#include <echo/FileSystem/FileSystem.h>
#include <echo/Kernel/TaskThread.h>
#include <echo/Audio/AudioSource.h>
#include <echo/Audio/AudioBuffer.h>
#include <echo/Audio/AudioStream.h>
#include <boost/foreach.hpp>

#include <iostream>

namespace Echo
{
	Audio::Audio() : TaskGroup("Audio")
	{
		mStreamsThread=0;
	}

	Audio::~Audio()
	{	
		_CleanUp();
	}

	bool Audio::Initialise(bool runOnOwnThread)
	{
		//We want to update audio buffers all the time
		if(!_Initialise())
		{
			ECHO_LOG_ERROR("Audio() - Audio Subsystem initialisation failed. Audio will be unavailable.");
			return false;
		}
		
		if(runOnOwnThread)
		{
			mStreamsThread=new TaskThread("Audio::Stream Thread");
			mStreamsThread->AddTask(this);
			if(!mStreamsThread->Execute())
			{
				runOnOwnThread=false;
				ECHO_LOG_WARNING("Unable to start streams update thread. Audio will run in Kernel Thread. This may cause audio glitches.");
			}
		}

		if(!runOnOwnThread)
		{
			if(mStreamsThread)
			{
				mStreamsThread->RemoveTask(this);
				delete mStreamsThread;
				mStreamsThread = 0;
			}
		}
		return true;
	}

	void Audio::_CleanUp()
	{
		if(mStreamsThread)
		{
			mStreamsThread->Terminate(true);
			delete mStreamsThread;
		}
	}

	void Audio::StreamClosed(AudioStream* stream)
	{
		mStreamsMutex.Lock();
		mStreams.remove(stream);
		mStreamsMutex.Unlock();
	}

	//////////////////////////////////////////////////////////////////////////
	shared_ptr<AudioStream> Audio::OpenStream(shared_ptr<AudioSource> source, bool loop/*=true*/)
	{
		if(!source)
		{
			return shared_ptr<AudioStream>();
		}
		shared_ptr<AudioStream> stream(new AudioStream(*this,source));
		
		mStreamsMutex.Lock();
		AddTask(stream.get());
		mStreams.push_back(stream.get());
		mStreamsMutex.Unlock();
		return stream;
	}

	shared_ptr<AudioBuffer> Audio::LoadSample(shared_ptr<AudioSource> source)
	{
		if(!source)
		{
			return shared_ptr<AudioBuffer>();
		}
		u32 numSamples=source->GetSampleRate();
		Seconds bufferLengthInSeconds=source->GetLength();
		numSamples=static_cast<u32>(bufferLengthInSeconds.count()*numSamples);
		shared_ptr<AudioBuffer> buffer=CreateBuffer(numSamples, source->GetSampleRate(), source->GetBitsPerSample(), source->GetNumChannels());
		if(buffer)
		{
			buffer->SetLoop(false);
			//Samples are only updated by sources, the buffer does not need the source once it has been filled.
			source->UpdateBuffer(*buffer, buffer->GetBufferSize());
		}
		return buffer;
	}

	void Audio::Update(Seconds lastFrameTime)
	{
		//Streams don't stop
		mStreamsMutex.Lock();
		TaskGroup::UpdateTasks(lastFrameTime);
		mStreamsMutex.Unlock();
	}

	void Audio::OnResume(bool applicationResume)
	{
		mStreamsMutex.Lock();		
		TaskGroup::OnResume(applicationResume);
		mStreamsMutex.Unlock();
	}

	void Audio::OnPause(bool applicationPause)
	{
		mStreamsMutex.Lock();
		TaskGroup::OnPause(applicationPause);
		mStreamsMutex.Unlock();		
	}
}
