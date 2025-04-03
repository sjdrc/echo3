#ifndef _ECHOSPEECHTOTEXTRECEIVER_H_
#define _ECHOSPEECHTOTEXTRECEIVER_H_
#include <string>
#include <vector>
#include <echo/UTF8String.h>

namespace Echo
{
	class SpeechToTextReceiver
	{
	public:
		SpeechToTextReceiver(){}
		virtual ~SpeechToTextReceiver(){}
		virtual void OnSpeechPartialResults(const std::vector< std::pair<UTF8String, f32> > weightedResults) = 0;
		virtual void OnSpeechFinalResults(const std::vector< std::pair<UTF8String, f32> > weightedResults) = 0;
	};
}
#endif
