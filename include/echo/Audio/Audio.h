#ifndef _ECHOAUDIO_H_
#define _ECHOAUDIO_H_
#include <echo/Types.h>
#include <echo/Kernel/TaskGroup.h>
#include <echo/Kernel/Thread.h>
#include <echo/Kernel/Mutex.h>

#include <string>
#include <map>

namespace Echo
{
	class AudioBuffer;
	class AudioSource;
	class AudioStream;
	class TaskThread;

	/**
	 * The audio class is the base class for implementation of audio systems.
	 * To implement an Audio system inherit from this class and implement the pure
	 * virtual methods.
	 * 
	 * Basic audio is implemented as AudioBuffer objects that are filled with audio
	 * data then played by the implementation.
	 * 
	 * You can load entire music tracks into an audio buffer if you want but in the
	 * interest of saving memory you can also use an AudioStream which will read
	 * audio data from a source and fill an AudioBuffer periodically.
	 * 
	 * This base class will manage updating any AudioStreams.
	 * 
	 * For a higher level interface to dealing with audio you may want to consider
	 * using AudioPlayer which takes care of dealing with audio sources and manages
	 * buffers.
     */
	class Audio : public TaskGroup 
	{
	public:
		Audio();
		virtual ~Audio();

		/**
		 * Initialise the audio system.
		 * @param runOnOwnThread If true the instance will set up its own thread to process audio on.
		 * This is usually recommended because if processing is performed on the same thread as other
		 * work and something holds up the thread then audio may skip.
		 * @return true if audio initialisation was successful, otherwise false.
		 */
		bool Initialise(bool runOnOwnThread);

		/**
		 * Create an audio buffer.
		 * @param numSamples The number of samples the buffer should contain
		 * @param frequency The sampling frequency of the buffer.
		 * @param sampleBitDepth The number of bits per sample, should be a multiple of 8.
		 * @param numChannels The number of audio channels, mono=1, stereo=2, etc. 
		 * @return If the parameters are valid for the implementation then an AudioBuffer
		 * is returned, otherwise a null pointer.
		 */
		virtual shared_ptr<AudioBuffer> CreateBuffer(u32 numSamples, u32 frequency, u32 sampleBitDepth, u32 numChannels)=0;

		/**
		 * Create an audio buffer specifically designed for streaming.
		 * Some implementations may use an AudioBuffer specialisation that is optimised for streaming.
		 * The buffer will still need to be periodically written to, so to more easily do this use
		 * OpenStream() instead, which will return an AudioStream object. AudioStreams take an audio
		 * source and manage the periodic writing to an AudioBuffer.
		 * @note If you are creating a new implementation and you find one AudioBuffer implementation
		 * is sufficient for samples and streaming then you can make this method return a buffer from
		 * CreateBuffer().
		 * @note The implementation should set the buffer up correctly for streaming. Echo uses the
		 * buffer as though it is a circular buffer of fixed size. This means the AudioStream will set
		 * the buffer to loop. Some implementations may deal with audio which don't fit this model and
		 * as such may need to prevent the buffer from looping for stream buffers.
		 * @param numSamples The number of samples the buffer should contain
		 * @param frequency The sampling frequency of the buffer.
		 * @param sampleBitDepth The number of bits per sample, should be a multiple of 8.
		 * @param numChannels The number of audio channels, mono=1, stereo=2, etc. 
		 * @return If the parameters are valid for the implementation then an AudioBuffer
		 * is returned, otherwise a null pointer.
		 */
		virtual shared_ptr<AudioBuffer> CreateStreamBuffer(u32 numSamples, u32 frequency, u32 sampleBitDepth, u32 numChannels)=0;
		
		/**
		 * Load audio from a source entirely into a buffer.
		 * @param source The audio source.
		 * @return If the source is valid and can be accessed this method returns a pointer
		 * to an AudioBuffer, otherwise false.
		 */
		shared_ptr<AudioBuffer> LoadSample(shared_ptr<AudioSource> source);

		/**
		 * Open an audio stream.
		 * @param source The audio source.
		 * @param loop Whether the stream should loop, you can modify this with the returned stream as well.
		 * @return If the source is valid and can be accessed this method returns a pointer
		 * to an AudioStream, otherwise false.
		 */
		shared_ptr<AudioStream> OpenStream(shared_ptr<AudioSource> source, bool loop=true);

	protected:
		void Update(Seconds lastFrameTime) override;
		void OnResume(bool) override;
		void OnPause(bool) override;
		
		virtual bool _Initialise()=0;
	private:
		friend class AudioStream;
		void StreamClosed(AudioStream* stream);
	private:
		std::list< AudioStream* > mStreams;
		TaskThread* mStreamsThread;
		Mutex mStreamsMutex;
		void _CleanUp();
	};
}
#endif
