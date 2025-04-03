#ifndef _ECHOAUDIOSOURCE_H_
#define _ECHOAUDIOSOURCE_H_
#include <echo/Types.h>
#include <echo/Chrono/Chrono.h>

namespace Echo
{
	class AudioBuffer;

	/**
	 * Abstract Base class for Audio Sources
	 */
	class AudioSource
	{
	public:
		AudioSource();
		virtual ~AudioSource();

		/**
		 * Restart the audio source.
		 * Will be called when the AudioBuffer finds that it reached the end of the
		 * source. After Reset is called. If the source is a audio file then Restart()
		 * should start reading from the start of the file's audio data again.
		 */
		virtual void Restart()=0;

		/**
		 * Get length of the buffer.
		 * @return the length of the Source in seconds.
		 */
		virtual Seconds GetLength()=0;

		/**
		 * Get the number of channels the audio source provides.
		 * @return the number of channels, e.g. 1 for mono and 2 for stereo.
		 */
		virtual u32 GetNumChannels()=0;

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		/**
		 * UpdateBuffer is called by the buffer on the source when the buffer needs to be updated with more data.
		 * The frequency is determined by the buffer and can be platform dependent. The buffer will call this
		 * more rapidly when it is nearer to being played out in an attempt to be filled with enough data to play
		 * without interruptions.
		 * This method should call AudioBuffer::Append() to append up to numBytes to the buffer.
		 * @param buffer the audio buffer to append data to.
		 * @param numBytes the requested number of bytes to append to the buffer.
		 * @return the number of bytes.
		 */
		virtual u32 UpdateBuffer(AudioBuffer& buffer, u32 numBytes)=0;

		/**
		 * Get the bit depth of a sample.
		 * @return the bit depth.
		 */
		virtual u32 GetBitsPerSample() = 0;
		
		/**
		 * Get the number of bytes per sample
		 * @return the number of bytes per sample.
		 */
		inline u32 GetBytesPerSample()
		{
			return GetBitsPerSample()/8;
		}
		
		/**
		 * Get the sample rate.
		 * This is samples per second,known as hertz.
		 * @return the sample rate.
		 */
		virtual u32 GetSampleRate() = 0;
		
		/**
		 * Helper method to convert the number of bytes to the number of samples.
		 * @param bytes the number of bytes.
		 * @return the number of samples the number of bytes would make up.
		 */
		inline Size ConvertBytesToSamples(Size bytes)
		{
			return (bytes / GetBytesPerSample());
		}
		
		/**
		 * Helper method to convert the number of bytes to seconds.
		 * @param bytes the number of bytes.
		 * @return the time in seconds that the number of bytes would make up as audio data for
		 * this buffer.
		 */
		inline Seconds ConvertBytesToSeconds(Size bytes)
		{
			return Seconds((ConvertBytesToSamples(bytes) / GetSampleRate()) / GetNumChannels());
		}

		/**
		 * Helper method to convert the number of samples to the number of bytes.
		 * @note Be sure the number of samples includes the total count for all channels for the
		 * time you expect.
		 * @param samples the number of samples.
		 * @return the number of bytes the given number of samples would require.
		 */
		inline Size ConvertSamplesToBytes(Size samples)
		{
			return (samples * GetBytesPerSample());
		}

		/**
		 * Helper method to convert the number of samples to seconds.
		 * @param totalSampleCountForAllChannels the number of samples for all channels,
		 * if the sample rate is 16000 and there are two channels then passing 16000 would
		 * mean this method returns 0.5 seconds.
		 * @return the time in seconds that the number of samples would make up if intended for
		 * this buffer.
		 */
		inline Seconds ConvertSamplesToSeconds(Size totalSampleCountForAllChannels)
		{
			return Seconds((totalSampleCountForAllChannels / GetSampleRate()) / GetNumChannels());
		}

		/**
		 * Helper method to convert the seconds to the required number of bytes.
		 * This method returns the total number of bytes required for all channels to make
		 * the specified seconds worth of audio.
		 * @param seconds the number of seconds.
		 * @return the number of bytes required for time in seconds of audio.
		 */
		inline Size ConvertSecondsToBytes(Seconds seconds)
		{
			return (ConvertSecondsToSamples(seconds) * GetBytesPerSample());
		}

		/**
		 * Helper method to convert the seconds to the required number of samples.
		 * This method returns the total number of samples required for all channels to make
		 * the specified seconds worth of audio.
		 * @param seconds the number of seconds.
		 * @return the number of samples required for time in seconds of audio.
		 */
		inline Size ConvertSecondsToSamples(Seconds seconds)
		{
			return seconds.count() * GetSampleRate() * GetNumChannels();
		}		
	};
}
#endif

