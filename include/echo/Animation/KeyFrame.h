#ifndef _ECHOKEYFRAME_H_
#define _ECHOKEYFRAME_H_

#include <echo/Types.h>

namespace Echo
{
	class KeyFrame
	{
	public:
		Seconds mTime;
		KeyFrame() : mTime(0.){}
		virtual ~KeyFrame(){}
	};
}
#endif 
