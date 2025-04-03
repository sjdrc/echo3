#ifndef _ECHOANIMATIONTIMEINDEX_H_
#define _ECHOANIMATIONTIMEINDEX_H_

#include <echo/Types.h>
#include <echo/Chrono/Chrono.h>

namespace Echo
{
	//!\brief AnimationTimeIndex holds a current time and associated lower index of a vector of key frames.
	//!\details The idea is that the lower index acts as a bookmark for the last lower index that was used in interpolation
	//!			between two frames. By using this bookmark you avoid searching from the start of the keyframe vector each
	//!			time you interpolate at a time.
	class AnimationTimeIndex
	{
	private:
		bool mHasIndex;
		Seconds mTime;
		size_t mLowerIndex;
	public:
		AnimationTimeIndex() : mHasIndex(false), mTime(0), mLowerIndex(0){}
		AnimationTimeIndex(Seconds time) : mHasIndex(false), mTime(time), mLowerIndex(0){}
		bool HasIndex() const {return mHasIndex;}
		void SetLowerIndex(const size_t& val) { mLowerIndex = val; }
		const size_t& GetLowerIndex() const { return mLowerIndex; }
		const Seconds& GetTime() const { return mTime; }

		//Add time adds time to the current time. This
		//allows keyframe searches to perform slightly
		//faster over SetTime.
		void AddTime(const Seconds& amount) { mTime+=amount; }

		//Setting time clears the index. So a lower performance
		//search is required to find the index in the keyframe
		void SetTime(const Seconds& val) { mTime=val; mHasIndex=false;}
	};
}
#endif 
