#ifndef _ECHOAUDIOBUFFER_H_
#define _ECHOAUDIOBUFFER_H_
#include <echo/Types.h>
#include <echo/Kernel/Task.h>

namespace Echo
{
	class Audio;
	class AudioSource;

	/**
	 * An AudioBuffer is an objec that can play audio.
	 * This is a base class for API specific implementations.
	 * To create an AudioBuffer use an Audio object.
	 */
	class AudioBuffer
	{
	private:
		u32 mBufferSize;
		u32 mSampleRate;
		u32 mNumChannels;
		u32 mBitsPerSample;
		Audio& mAudio;

	protected:
		friend class Audio;
		AudioBuffer(Audio& audio, u32 numSamples, u32 frequency, u32 sampleBitDepth, u32 numChannels);
	public:
		virtual ~AudioBuffer();

		/**
		 * Get the number of channels.
		 * @return the number of channels.
		 */
		u32 GetNumChannels() const
		{
			return mNumChannels;
		}
		
		/**
		 * Get the buffer size in bytes.
		 * This should be equal to GetSampleRate() * GetBytesPerSample() * GetNumChannels()
		 * @return Size of the buffer in bytes.
		 */
		u32 GetBufferSize() const
		{
			return mBufferSize;
		}
		
		/**
		 * Get the sample rate in hertz.
		 * @return the sample rate in hertz.
		 */
		inline u32 GetSampleRate() const
		{
			return mSampleRate;
		}
		
		/**
		 * Get the number of bits per sample of a single channel.
		 * @return the number of bits per sample.
		 */
		inline u32 GetBitsPerSample() const
		{
			return mBitsPerSample;
		}

		/**
		 * Get the number of bytes per sample
		 * @return the number of bytes per sample.
		 */
		inline u32 GetBytesPerSample() const
		{
			return GetBitsPerSample()/8;
		}

		/**
		 * Play the buffer.
		 * @param loop Whether the buffer should loop or not.
		 */
		void Play(bool loop);
		
		/**
		 * Restart the buffer.
		 * This Stops the buffer, sets the play position to 0. If the buffer was playing then it will play from the beginning.
		 */
		void Restart();

		/**
		 * Append dataSize bytes into the buffer at the write position.
		 * @note The amount of data should be in multiples of a complete sample, i.e. GetBytesPerSample().
		 */
		virtual u32 Append(void* data, u32 dataSize)=0;
		
		/**
		 * Return whether the buffer is looping.
		 * @return true if the buffer is set to loop, otherwise false.
		 */
		virtual bool IsLooping()=0;
		
		/**
		 * Return whether the buffer is playing.
		 * @return true if the buffer is playing, otherwise false.
		 */
		virtual bool IsPlaying()=0;
		
		/**
		 * Get the write position of the buffer.
		 * The write position is where data will be written to when Append() is next called.
		 * This method should always return as though the buffer wraps. If an implementation
		 * does not wrap the buffer internally then it should track the write position and
		 * return the trackedWritePosition % bufferSize. i.e. it should emulate that it wraps.
		 * @return The write position in bytes as though the buffer wraps.
		 */
		virtual u32 GetWritePosition()=0;

		/**
		 * Get the play position of the buffer as though the buffer wraps.
		 * If an implementation does not wrap the buffer internally then it should track the
		 * play position and return the trackedPlaypositionPosition % bufferSize.
		 * @return the play position in bytes as though the buffer wraps.
		 */
		virtual u32 GetPlayPosition()=0;
		
		/**
		 * Get how much space there is available to write.
		 * This essentially return the distance between the play and write positions accounting
		 * for wrapping.
		 * @return the number of bytes available for writing.
		 */
		Size GetWriteSpaceInBytes();
		
		/**
		 * Set the play position.
		 * @param pos The position within the buffer in bytes that the play cursor should be moved to.
		 * @note Fixed size buffers should be able to have their position set without issue but buffers
		 * that are being used for audio streaming isn't necessarily trivial if the buffer is not a
		 * wrapping buffer. We are yet to define the behaviour in the later case.
		 */
		virtual void SetPlayPosition(u32 pos)=0;
		
		/**
		 * Set whether this buffer should loop or not.
		 * @param loop whether this buffer should loop or not.
		 */
		virtual void SetLoop(bool loop)=0;

		/**
		 * Set the buffer volume.
		 * @param v The volume, between 0 and 1 inclusive.
		 */
		virtual void SetVolume(f32 v)=0;
		
		/**
		 * Get the buffer volume.
		 * @return The volume as a value between 0 and 1 inclusive.
		 */
		virtual f32 GetVolume()=0;

		/**
		 * If the buffer is a two channel buffer set the pan amount between the two channels.
		 * @param panX 0 full left, 0.5 centre, 1.0 full right.
		 */
		virtual void Pan(f32 panX)=0;

		/**
		 * Play the buffer.
		 */
		virtual void Play()=0;

		/**
		 * Stop the buffer.
		 */
		virtual void Stop()=0;

		/**
		 * Pause the buffer.
		 * Call play again to resume playback.
		 */
		virtual void Pause()=0;
		
		/**
		 * Reset.
		 * This method is called when the buffer needs to be reset for fresh data.
		 * Some implementations might need to some house keeping before clearing for reuse.
		 * @note this is not the same as restarting the buffer.
		 */
		virtual void Reset()=0;
		
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
