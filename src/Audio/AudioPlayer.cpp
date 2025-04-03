#include <echo/Audio/AudioPlayer.h>
#include <echo/Audio/AudioBuffer.h>
#include <echo/Audio/AudioSource.h>
#include <echo/Audio/AudioStream.h>
#include <echo/Audio/Audio.h>
#include <echo/Resource/WavAudioSource.h>
#include <echo/Resource/OggAudioSource.h>
#include <echo/Util/StringUtils.h>
#include <echo/FileSystem/FileSystem.h>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/case_conv.hpp>

namespace Echo
{
	AudioPlayer::AudioPlayer(shared_ptr<Audio> audio, shared_ptr<FileSystem> fileSystem) :
		Task("AudioPlayer"),
		mAudio(audio),
		mFileSystem(fileSystem),
		mMusicVolume(1.f),
		mMinMusicVolume(0.f),
		mSoundVolume(1.f)
	{
		CreateLoader<WavAudioSource>("wav");
		CreateLoader<OggAudioSource>("ogg");
	}

	AudioPlayer::~AudioPlayer()
	{
	}

	void AudioPlayer::OnResume(bool)
	{
		//Resume any resumed tracks.
	}

	void AudioPlayer::OnPause(bool)
	{
		//Pause any paused tracks.
	}

	void AudioPlayer::LoadAudioList(const std::string& trackListFile)
	{
		File f = mFileSystem->Open(trackListFile);
		if(!f.IsOpen())
		{
			ECHO_LOG_ERROR("File not open. Unable to load track list: " << trackListFile);
			return;
		}

		std::vector< std::string > lines;
		f.ReadLines(lines, false);
		f.Close();

		BOOST_FOREACH(std::string& line, lines)
		{
			std::vector<std::string> params;
			std::string token=Utils::String::SplitLine(line,"=",",",params);
			if(params.empty())
				continue;

			std::string resolvedFileName;
			resolvedFileName = mFileSystem->ResolveFullNameForFileWithParent(params[0],trackListFile);
			
			if(token=="music")
			{
				if(params.size()<2)
					continue;
				//params[0]	- FileName
				//params[1] - TrackReferenceName
				AddMusic(params[1],resolvedFileName);
				continue;
			}
			if(token=="sound")
			{
				if(params.size()<2)
					continue;
				//params[0]	- FileName
				//params[1] - SoundReferenceName
				//params[2] - delayLoad=default is false
				bool delayLoad=(params[2]=="true" || params[2]=="1");
				AddSound(params[1],resolvedFileName, delayLoad);
				continue;
			}
		}
	}

	shared_ptr<AudioStream> AudioPlayer::PlayMusic(	const std::string& name,
										CurrentTrackOption currentTrackOption,
										PlayOption inOptions)
	{
		shared_ptr<AudioStream> stream;
		
		//Look for it in the Loaded tracks - if it's not there then open a stream to it
		std::map< std::string , shared_ptr<AudioStream> >::iterator it=mLoadedTracks.find(name);
		if(it==mLoadedTracks.end())
		{
			std::map< std::string, std::string >::iterator it2=mTrackList.find(name);
			if(it2!=mTrackList.end())
			{
				shared_ptr<AudioSource> source=CreateSource(it2->second);
				if(source)
				{
					stream=mAudio->OpenStream(source);
					if(stream)
					{
						mLoadedTracks[name]=stream;
					}
				}
			}else
			{
				return shared_ptr<AudioStream>();
			}
		}else
		{
			stream = it->second;
		}

		if(stream)
		{
			//Stop the current track using the options - if the current track
			//is in a fade out mode then we can safely remove it from the
			//fade and play lists then they will be added again below.
			if(currentTrackOption!=CurrentTrackOptions::STOP_LAST || currentTrackOption!=CurrentTrackOptions::STOP_ALL)
			{
				StopMusic(currentTrackOption);
			}
			
			if(stream->IsPlaying())
			{
				mFadeInBuffers.push_back(stream);
				mFadeOutBuffers.remove(stream);
				mCurrentTracks.remove(stream);
				mCurrentTracks.push_back(stream);
				return shared_ptr<AudioStream>();
			}
			mCurrentTracks.push_back(stream);
			stream->Play(true);
			
			switch(inOptions)
			{
				case PlayOptions::FADE_IN:
					stream->SetVolume(mMinMusicVolume);
					mFadeInBuffers.push_back(stream);
				break;
				case PlayOptions::NOTHING:
				default:
					stream->SetVolume(mMusicVolume);
					break;
			}
		}
		return stream;
	}

	void AudioPlayer::AddMusic(const std::string& referenceName, const std::string& fileName)
	{
		if(mTrackList.find(referenceName)==mTrackList.end())
		{
			mTrackList[referenceName]=fileName;
		}
	}

	void AudioPlayer::StopMusic(CurrentTrackOption option)
	{
		if(!mCurrentTracks.empty())
		{
			switch(option)
			{
				case CurrentTrackOptions::FADE_ALL:
				{
					while(!mCurrentTracks.empty())
					{
						shared_ptr<AudioStream> lastBuffer=mCurrentTracks.back();
						mFadeInBuffers.remove(lastBuffer);
						mFadeOutBuffers.push_back(lastBuffer);
						mCurrentTracks.pop_back();
					}
				}
				break;
				case CurrentTrackOptions::FADE_LAST:
				{
					shared_ptr<AudioStream> lastBuffer=mCurrentTracks.back();
					mFadeInBuffers.remove(lastBuffer);
					mFadeOutBuffers.push_back(lastBuffer);
					mCurrentTracks.pop_back();
				}
				break;
				case CurrentTrackOptions::STOP_LAST:
				{
					shared_ptr<AudioStream> lastBuffer=mCurrentTracks.back();
					lastBuffer->Stop();
					mFadeInBuffers.remove(lastBuffer);
					mCurrentTracks.pop_back();
				}
				break;
				case CurrentTrackOptions::STOP_ALL:
				{
					while(!mCurrentTracks.empty())
					{
						shared_ptr<AudioStream> lastBuffer=mCurrentTracks.back();
						lastBuffer->Stop();
						mFadeInBuffers.remove(lastBuffer);
						mCurrentTracks.pop_back();
					}
				}
				break;
				case CurrentTrackOptions::DO_NOTHING:
				break;
			}
		}
	}

	shared_ptr<AudioStream> AudioPlayer::GetMusic(const std::string& name)
	{
		std::map< std::string , shared_ptr<AudioStream> >::iterator it=mLoadedTracks.find(name);
		if(it!=mLoadedTracks.end())
		{
			return it->second;
		}
		return shared_ptr<AudioStream>();
	}

	void AudioPlayer::AddSound(const std::string& referenceName, std::string fileName, bool delayLoad)
	{
		if(mSoundList.find(referenceName)==mSoundList.end())
		{
			mSoundList[referenceName]=fileName;
			if(!delayLoad)
			{
				LoadSound(referenceName);
			}
		}
	}

	shared_ptr<AudioBuffer> AudioPlayer::LoadSound(const std::string& referenceName)
	{
		std::map< std::string, shared_ptr<AudioBuffer> >::iterator lit=mLoadedSounds.find(referenceName);
		if(lit!=mLoadedSounds.end())
		{
			//Don't load it more than once!
			return lit->second;
		}

		std::map< std::string, std::string >::iterator it=mSoundList.find(referenceName);
		if(it!=mSoundList.end())
		{
			shared_ptr<AudioSource> source=CreateSource(it->second);
			if(source)
			{
				shared_ptr<AudioBuffer> buffer=mAudio->LoadSample(source);
				mLoadedSounds[referenceName]=buffer;
				return buffer;
			}
		}
		return shared_ptr<AudioBuffer>();
	}

	shared_ptr<AudioBuffer> AudioPlayer::PlaySound(const std::string& name)
	{
		//Load sound only allows a sample to be loaded once.
		shared_ptr<AudioBuffer> buffer = LoadSound(name);
		
		if(buffer)
		{
			buffer->SetVolume(mSoundVolume);
			buffer->Play();
		}
		return buffer;
	}

	void AudioPlayer::SetDefaultMusicVolume(f32 volume)
	{
		mMusicVolume=volume;
	}

	void AudioPlayer::SetDefaultSoundVolume(f32 volume)
	{
		mSoundVolume=volume;
	}

	void AudioPlayer::Update(Seconds lastFrameTime)
	{
	}

	void AudioPlayer::AddLoader(AudioSourceLoader loader, const std::string& extension)
	{
		mAudioSourceLoaders[extension]=loader;
	}

	void AudioPlayer::RemoveLoader(const std::string& extension)
	{
		std::map< std::string, AudioSourceLoader >::iterator it=mAudioSourceLoaders.find(extension);
		if(it!=mAudioSourceLoaders.end())
		{
			mAudioSourceLoaders.erase(it);
		}
	}

	void AudioPlayer::RemoveAllLoaders()
	{
		mAudioSourceLoaders.clear();
	}
	
	shared_ptr<AudioSource> AudioPlayer::CreateSource(const std::string& fileName)
	{
		File file = mFileSystem->Open(fileName);
		if(!file.IsOpen())
			return shared_ptr<AudioSource>();
		std::string extension =	Utils::String::GetFileExtension(fileName);
		boost::to_lower(extension);
		std::map< std::string, AudioSourceLoader >::iterator it = mAudioSourceLoaders.find(extension);

		if(it==mAudioSourceLoaders.end())
		{
			return shared_ptr<AudioSource>();
		}
		return it->second(file);
	}
	
}
