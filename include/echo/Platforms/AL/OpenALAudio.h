#ifndef _ECHOOPENALAUDIO_H_
#define _ECHOOPENALAUDIO_H_
#include <echo/Audio/Audio.h>
#include <AL/alc.h>
namespace Echo
{
	class AudioBuffer;

	/**
	 * An OpenAL Audio implementation.
	 */
	class OpenALAudio : public Audio
	{
	public:
		OpenALAudio();
		virtual ~OpenALAudio();

		virtual shared_ptr<AudioBuffer> CreateBuffer(u32 numSamples, u32 frequency, u32 sampleBitDepth, u32 numChannels) override;
		virtual shared_ptr<AudioBuffer> CreateStreamBuffer(u32 numSamples, u32 frequency, u32 sampleBitDepth, u32 numChannels) override;
	protected:
		virtual bool _Initialise() override;
	private:
		ALCdevice* mDevice;
		ALCcontext* mContext;
		void _CleanUpOpenAL();
	};
}
#endif
