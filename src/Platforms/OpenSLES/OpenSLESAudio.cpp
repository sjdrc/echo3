#include <echo/Platforms/OpenSLES/OpenSLESAudio.h>

#include <echo/Platforms/OpenSLES/OpenSLESAudioBuffer.h>
#include <echo/Platforms/OpenSLES/OpenSLESPlayer.h>
#include <echo/Audio/AudioSource.h>
#include <boost/foreach.hpp>

#include <assert.h>
#include <iostream>

namespace Echo
{
	////////////////
	//BufferFormat
	OpenSLESAudio::BufferFormat::BufferFormat(const BufferFormat& rhs) : mSampleRate(rhs.mSampleRate), mBitDepth(rhs.mBitDepth), mChannels(rhs.mChannels)
	{
	}

	OpenSLESAudio::BufferFormat::BufferFormat(u32 sampleRate, u32 bitDepth, u32 channels) : mSampleRate(sampleRate), mBitDepth(bitDepth), mChannels(channels)
	{
	}

	OpenSLESAudio::BufferFormat::BufferFormat(const SLDataFormat_PCM& dataFormat) : mSampleRate(dataFormat.samplesPerSec), mBitDepth(dataFormat.bitsPerSample), mChannels(dataFormat.numChannels)
	{
	}

	OpenSLESAudio::BufferFormat& OpenSLESAudio::BufferFormat::operator=(const BufferFormat& rhs)
	{
		if (this == &rhs)
			return *this;
		
		mSampleRate = rhs.mSampleRate;
		mBitDepth = rhs.mBitDepth;
		mChannels = rhs.mChannels;
		return *this;
	}

	OpenSLESAudio::BufferFormat::BufferFormat(const AudioBuffer& buffer) : mSampleRate(buffer.GetSampleRate()), mBitDepth(buffer.GetBitsPerSample()), mChannels(buffer.GetNumChannels())
	{
	}

	bool OpenSLESAudio::BufferFormat::operator==(const BufferFormat& rhs)
	{
		return(mSampleRate == rhs.mSampleRate && mBitDepth == rhs.mBitDepth && mChannels == rhs.mChannels);
	}

	//-----------------------------------------------------------------------
	//////////////////////////////////////////////////////////////////////////
	//------------------------------------------------------------------------
	OpenSLESAudio::OpenSLESAudio() : mEngineObject(0),
		mEngineInterface(0),
		mEngineCapabilitiesIterface(0),
		mOutputMixObject(0),
		mMaximumNumber2DVoices(0),
		mNumberFree2DVoices(0),
		mIsMaximum2DVoicesAbsolute(SL_BOOLEAN_FALSE)
	{
	}

	OpenSLESAudio::~OpenSLESAudio()
	{
		ECHO_LOG_DEBUG("Destroy OpenSLESAudio");

		// Destroy Output Mix object
		if(mOutputMixObject)
		{
			(*mOutputMixObject)->Destroy(mOutputMixObject);
		}
		// Destroy Engine object
		if(mEngineObject)
		{
			(*mEngineObject)->Destroy(mEngineObject);
		}
	}

	shared_ptr<AudioBuffer> OpenSLESAudio::CreateBuffer(u32 numSamples, u32 frequency, u32 sampleBitDepth, u32 numChannels)
	{
		shared_ptr<OpenSLESAudioBuffer> exabuffer(new OpenSLESAudioBuffer(numSamples, frequency, sampleBitDepth, numChannels, *this));
		exabuffer->SetVolume(1.0f);
		return exabuffer;
	}
	
	shared_ptr<AudioBuffer> OpenSLESAudio::CreateStreamBuffer(u32 numSamples, u32 frequency, u32 sampleBitDepth, u32 numChannels)
	{
		//Not doing anything special for stream buffers at the moment.
		return CreateBuffer(numSamples, frequency, sampleBitDepth, numChannels);
	}


	OpenSLESPlayer* OpenSLESAudio::GetPlayer(OpenSLESAudioBuffer& forBuffer, bool truncateEarlierSounds)
	{
		//If we cannot play any sounds, don't provide a player.
		if(mMaximumNumber2DVoices == 0)
		{
			return 0;
		}

		BufferFormat requiredFormat(forBuffer);

		//Try and find a compatible player
		if(!mAvailablePlayers.empty())
		{
			if(OpenSLESPlayer * player = FindPlayerMatch(requiredFormat, mAvailablePlayers))
			{
				//ECHO_LOG_DEBUG("Found player match.");
				mAvailablePlayers.remove(player);
				mPlayersInUse.push_back(player);
				return player;
			} else
			{
				//Available players are in the stopped state and should not have a buffer.
				//We already established we couldn't find a matching format.

				//ECHO_LOG_DEBUG("Destroying unused available player.");
				OpenSLESPlayer* oldPlayer = mAvailablePlayers.front();
				delete oldPlayer;
				//Destroying the player will also cause it to be removed from mAvailablePlayers through the callback.
				//ECHO_LOG_DEBUG("Creating new player.");
				OpenSLESPlayer* newPlayer = new OpenSLESPlayer(requiredFormat.mSampleRate,
																 requiredFormat.mBitDepth,
																 requiredFormat.mChannels,
																 mEngineInterface,
																 mOutputMixObject, *this);
				mPlayersInUse.push_back(newPlayer);
				return newPlayer;
			}
		}

		// No player was found, lets see if we can allocate one.

		// Have we allocated all players?
		if((mPlayersInUse.size() + mAvailablePlayers.size()) < mMaximumNumber2DVoices)
		{
			//No, allocate a new player.
			//ECHO_LOG_DEBUG("Creating new player.");
			OpenSLESPlayer* newPlayer = new OpenSLESPlayer(requiredFormat.mSampleRate,
															requiredFormat.mBitDepth,
															requiredFormat.mChannels,
															mEngineInterface,
															mOutputMixObject, *this);
			mPlayersInUse.push_back(newPlayer);
			return newPlayer;
		}

		//No available player found and we've allocated all. Truncate oldest sample.
		if(truncateEarlierSounds && !mPlayersInUse.empty())
		{
			//ECHO_LOG_DEBUG("Truncating sound.");
			OpenSLESPlayer* oldestPlayer = mPlayersInUse.front();
			mPlayersInUse.pop_front();

			//Yes, lets use this player.
			OpenSLESAudioBuffer* currentBuffer = oldestPlayer->GetCurrentBuffer();
			if(currentBuffer)
			{
				currentBuffer->Stop();
			} else
			{
				// There should always be a buffer, but just incase someone is doing things manually
				// we better stop the player.
				oldestPlayer->Stop();
				ECHO_LOG_WARNING("OpenSLESPlayer in use without a OpenSLESAudioBuffer.");
			}

			//Does the player match the required format?
			if(requiredFormat == BufferFormat(oldestPlayer->GetDataFormat()))
			{
				//Yes, just return the buffer;
				//ECHO_LOG_DEBUG("Oldest player format matches.");
				return oldestPlayer;
			}
			//ECHO_LOG_DEBUG("Destroying oldest player.");
			delete oldestPlayer;
			//ECHO_LOG_DEBUG("Creating new player.");
			OpenSLESPlayer* newPlayer = new OpenSLESPlayer(requiredFormat.mSampleRate,
															requiredFormat.mBitDepth,
															requiredFormat.mChannels,
															mEngineInterface,
															mOutputMixObject, *this);
			mPlayersInUse.push_back(newPlayer);
			return newPlayer;
		}

		return 0;
	}

	void OpenSLESAudio::PlayerFinished(OpenSLESPlayer* player)
	{
		mAvailablePlayers.push_back(player);
		mPlayersInUse.remove(player);
	}

	void OpenSLESAudio::PlayerDestroyed(OpenSLESPlayer* player)
	{
		mAvailablePlayers.remove(player);
		mPlayersInUse.remove(player);
	}

	OpenSLESPlayer* OpenSLESAudio::FindPlayerMatch(const BufferFormat& format, const std::list< OpenSLESPlayer* >& players) const
	{
		BOOST_FOREACH(OpenSLESPlayer* player, players)
		{
			const SLDataFormat_PCM& format = player->GetDataFormat();
			BufferFormat bufferFormat = BufferFormat(format.samplesPerSec / 1000, format.numChannels, format.bitsPerSample);
			if(bufferFormat == format)
			{
				return player;
			}
		}
		return 0;
	}

	bool OpenSLESAudio::_Initialise()
	{
		ECHO_LOG_INFO("Initialise OpenSLESAudio");
		//Taken from the native audio android example. Will clean up as we get things working.

		SLresult result;

		result = slCreateEngine(&mEngineObject, 0, NULL, 0, NULL, NULL);
		CheckError(result);

		// realize the engine
		result = (*mEngineObject)->Realize(mEngineObject, SL_BOOLEAN_FALSE);
		CheckError(result);

		// get the engine interface, which is needed in order to create other objects
		result = (*mEngineObject)->GetInterface(mEngineObject, SL_IID_ENGINE, &mEngineInterface);
		CheckError(result);

		// create output mix
		//SLInterfaceID iidArray[1];
		//SLboolean required[1];
		//iidArray[0] = SL_IID_VOLUME;
		//required[0] = SL_BOOLEAN_FALSE;
		result = (*mEngineInterface)->CreateOutputMix(mEngineInterface, &mOutputMixObject, 0, 0, 0); //iidArray, required);
		CheckError(result);

		// realize the output mix
		result = (*mOutputMixObject)->Realize(mOutputMixObject, SL_BOOLEAN_FALSE);
		CheckError(result);

		mMaximumNumber2DVoices = 16;
		//if(!CheckCapabilities())
		//{
		//	return false;
		//}

		//ECHO_LOG_DEBUG("Initialise OpenSLESAudio complete");
		return true;
	}

	bool OpenSLESAudio::CheckCapabilities()
	{
		if(!mEngineInterface)
		{
			ECHO_LOG_ERROR("OpenSLESAudio::CalculateCapabilities(): object not initialised.");
			return false;
		}

		SLresult result;

		/* Get the Engine Capabilities interface - an implicit interface */
		ECHO_LOG_INFO("Acquiring interface to audio engine capabilities...");
		result = (*mEngineObject)->GetInterface(mEngineObject, SL_IID_ENGINECAPABILITIES, (void*) &mEngineCapabilitiesIterface);
		CheckError(result);
		ECHO_LOG_INFO("Audio engine capabilities interface acquired!");

		ECHO_LOG_INFO("Querying API version...");
		SLint16 vMajor = 0, vMinor = 0, vStep = 0;
		result = (*mEngineCapabilitiesIterface)->QueryAPIVersion(mEngineCapabilitiesIterface, &vMajor, &vMinor, &vStep);
		CheckError(result);
		ECHO_LOG_INFO("OpenSLES version: " << vMajor << "." << vMinor << "." << vStep);

		/* Query profile support */
		SLuint16 profilesSupported = 0;
		SLboolean isPhoneProfileSupported = SL_BOOLEAN_FALSE;
		SLboolean isMusicProfileSupported = SL_BOOLEAN_FALSE;
		SLboolean isGameProfileSupported = SL_BOOLEAN_FALSE;

		ECHO_LOG_INFO("Querying supported SLES profiles...");
		result = (*mEngineCapabilitiesIterface)->QuerySupportedProfiles(mEngineCapabilitiesIterface, &profilesSupported);
		CheckError(result);

		if(profilesSupported & SL_PROFILES_PHONE)
		{
			isPhoneProfileSupported = SL_BOOLEAN_TRUE;
			ECHO_LOG_INFO("Phone profile supported.");
		}
		if(profilesSupported & SL_PROFILES_MUSIC)
		{
			isMusicProfileSupported = SL_BOOLEAN_TRUE;
			ECHO_LOG_INFO("Music profile supported.");
		}
		if(profilesSupported & SL_PROFILES_GAME)
		{
			isGameProfileSupported = SL_BOOLEAN_TRUE;
			ECHO_LOG_INFO("Game profile supported.");
		}

		if(!isPhoneProfileSupported && !isMusicProfileSupported && !isGameProfileSupported)
		{
			ECHO_LOG_ERROR("No supported profiles.");
			return false;
		}

		ECHO_LOG_INFO("Querying number of voices available...");
		//Query the number of players we can allocate
		result = (*mEngineCapabilitiesIterface)->QueryAvailableVoices(mEngineCapabilitiesIterface,
																	SL_VOICETYPE_2D_AUDIO,
																	&mMaximumNumber2DVoices,
																	&mIsMaximum2DVoicesAbsolute,
																	&mNumberFree2DVoices);
		CheckError(result);
		ECHO_LOG_INFO("Number of voices available: " << mMaximumNumber2DVoices);

		if(mMaximumNumber2DVoices == 0)
		{
			return false;
		}

		//Uncomment when ready to implement vibration support.
		//	// Query number of vibra devices present in the system
		//	SLuint32 numVibraDevices = 0, VibraDeviceID[MAX_NUMBER_VIBRA_DEVICES];
		//
		//	result = (*mEngineCapabilitiesIterface)->QueryVibraCapabilities(mEngineCapabilitiesIterface, &numVibraDevices, NULL, NULL);
		//	CheckSLESError(result);
		//
		//	// Get the capabilities of each vibra device present
		//	for(i = 0; i < numVibraDevices; i++)
		//	{
		//		// Retrieve the VibradeviceID for each of the Vibra devices found on the system
		//		result = (*mEngineCapabilitiesIterface)->QueryVibraCapabilities(mEngineCapabilitiesIterface, &i, &VibraDeviceID[i], NULL);
		//		CheckSLESError(result);
		//
		//		// Either the index i or the VibraDeviceID can be used to retrieve the capabilities of each
		//		// vibra device; we choose to use the VibraDeviceID here
		//		result = (*mEngineCapabilitiesIterface)->QueryVibraCapabilities(mEngineCapabilitiesIterface, NULL, &VibraDeviceID[i], &VibraDescriptor[i]);
		//		CheckSLESError(result);
		//	}

		ECHO_LOG_INFO("Querying threading mode...");
		SLboolean isThreadSafe = SL_BOOLEAN_FALSE;
		result = (*mEngineCapabilitiesIterface)->IsThreadSafe( mEngineCapabilitiesIterface,&isThreadSafe);
		CheckError(result);
		mThreadSafe = (isThreadSafe == SL_BOOLEAN_TRUE);
		if(mThreadSafe)
		{
			ECHO_LOG_INFO("Thread safe.");
		} else
		{
			ECHO_LOG_INFO("Not Thread safe.");
		}

		return true;
	}


	void OpenSLESAudio::CheckError(SLresult result)
	{
		assert(SL_RESULT_SUCCESS == result);
		(void) result;
	}
}
