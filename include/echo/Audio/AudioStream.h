#ifndef _ECHOAUDIOSTREAM_H_
#define _ECHOAUDIOSTREAM_H_
#include <echo/Types.h>
#include <echo/Kernel/Task.h>

namespace Echo
{
	class Audio;
	class AudioSource;
	class AudioBuffer;

	class AudioStream : public Task, public InheritableEnableSharedFromThis<AudioStream>
	{
	protected:
		friend class Audio;
		/**
		 * Constructor - accessible through the Audio interface.
		 * @param audio - The owning audio interface.
		 * @param source the source audio data will come from while streaming, this will be queried
		 * periodically for more audio data.
		 * @param bufferLength Length of the buffer, normally the default is fine.
		 * @param bufferSegments Number of segments to split the buffer into, normally the default is
		 * fine.
		 * 
		 * AudioStream will attempt to keep the AudioBuffer filled such that no more than one
		 * entire segment is empty. i.e. as a buffer plays audio data is written to the buffer as each
		 * segment of the buffer becomes empty. So a 4 second buffer with 4 segment means that 1 second of
		 * audio should be written approximately every second. It is recommended that there are at least 3
		 * segments (as though it is triple buffered) in case there is a delay in updating one of the
		 * segments.
		 * 
		 * AudioStream will detect any drift of the updates from floating point precision errors while
		 * tracking time and perform an extra update as needed.
		 */
		AudioStream(Audio& audio, shared_ptr<AudioSource> source, Seconds bufferLength = Seconds(4), Size bufferSegments = 4);
	public:
		virtual ~AudioStream();

		void Play(bool loop);

		shared_ptr<AudioSource> GetSource() const {return mSource;}
		
		//If the AudioBuffer is in Stream mode Restart will
		// - stop then rewind the buffer
		// - restart the source
		// - resume playing
		void Restart();

		void Update(Seconds lastFrameTime);

		//Implementation specific functions

		//Put dataSize bytes into the buffer at the write position
		bool IsLooping();
		bool IsPlaying();
		Seconds GetPlayPosition();
		void SetPlayPosition(Seconds pos);

		//Control functions
		//Enable or disable looping.
		//Implementations should set mLoop
		void SetLoop(bool loop);

		//Set buffer volume
		void SetVolume(f32 v);
		f32 GetVolume();

		//Set Pan
		void Pan(f32 panX);		//0 full left, 0.5 centre, 1.0 full right
		//Play
		void Play();
		//Stop
		void Stop();
		//Pause
		void Pause();
	private:
		Audio& mAudio;
		shared_ptr<AudioSource> mSource;
		shared_ptr<AudioBuffer> mBuffer;
		Seconds mSecondsLeftUntilUpdate;
		Size mSegmentSizeInBytes;
		Size mNumberOfSegments;
		bool mLoop;
	};
}
#endif
