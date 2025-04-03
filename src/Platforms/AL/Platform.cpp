#include <echo/Platforms/AL/OpenALAudio.h>

namespace Echo
{
	namespace Platform
	{
		shared_ptr<Audio> CreateDefaultAudioSystem()
		{
			OpenALAudio* alAudio = new OpenALAudio();
			alAudio->Initialise(true);
			return shared_ptr<Audio>(alAudio);
		}
	}
}
