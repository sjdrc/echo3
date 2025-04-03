#ifndef _ECHOOPENSLESAUDIO_H_
#define _ECHOOPENSLESAUDIO_H_

#include <echo/Audio/Audio.h>
#include <SLES/OpenSLES.h>

namespace Echo
{
	class AudioBuffer;
	class OpenSLESPlayer;
	class OpenSLESAudioBuffer;

	/**
	 * An OpenSLES implementation of Audio.
	 */
	class OpenSLESAudio : public Audio
	{
	public:
		OpenSLESAudio();
		~OpenSLESAudio();

		shared_ptr<AudioBuffer> CreateBuffer(u32 numSamples, u32 frequency, u32 sampleBitDepth, u32 numChannels);
		shared_ptr<AudioBuffer> CreateStreamBuffer(u32 numSamples, u32 frequency, u32 sampleBitDepth, u32 numChannels);
	private:
		friend class OpenSLESPlayer;
		void PlayerFinished(OpenSLESPlayer* player);
		void PlayerDestroyed(OpenSLESPlayer* player);
	private:
		friend class OpenSLESAudioBuffer;

		//Get a player object compatible with the given buffer.
		//Returns a null pointer if a player is not available.
		// The number of player objects available at one time, will depend on the platform implementation.
		// If truncateEarlierSounds is true it will
		OpenSLESPlayer* GetPlayer(OpenSLESAudioBuffer& forBuffer, bool truncateEarlierSounds);

		class BufferFormat
		{
		public:
			BufferFormat(const BufferFormat& rhs);
			BufferFormat(u32 sampleRate, u32 bitDepth, u32 channels);
			BufferFormat& operator=(const BufferFormat& rhs);
			BufferFormat(const SLDataFormat_PCM& dataFormat);
			BufferFormat(const AudioBuffer& buffer);
			u32 mSampleRate;
			u32 mBitDepth;
			u32 mChannels;
			bool operator==(const BufferFormat& rhs);
		};
		bool IsThreadSafe() const
		{
			return mThreadSafe;
		}
	private:
		bool _Initialise();
		bool CheckCapabilities();
		void CheckError(SLresult result);

		OpenSLESPlayer* FindPlayerMatch(const BufferFormat& format, const std::list< OpenSLESPlayer* >& players) const;
		OpenSLESPlayer* NewPlayer(const BufferFormat& format);

		SLObjectItf mEngineObject;
		SLEngineItf mEngineInterface;
		SLEngineCapabilitiesItf mEngineCapabilitiesIterface;

		SLint16 mMaximumNumber2DVoices;
		SLint16 mNumberFree2DVoices;
		SLboolean mIsMaximum2DVoicesAbsolute;
		bool mThreadSafe;

		// output mix interfaces
		SLObjectItf mOutputMixObject;
		std::list< OpenSLESPlayer* > mAvailablePlayers;
		std::list< OpenSLESPlayer* > mStreamPlayers;
		std::list< OpenSLESPlayer* > mPlayersInUse;
	};
}
#endif
