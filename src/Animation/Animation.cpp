#include <echo/Animation/AnimationTrack.h>
#include <echo/Animation/Animation.h>
#include <echo/Maths/Vector3.h>
#include <echo/Maths/Quaternion.h>
#include <echo/Animation/Skeleton.h>
#include <echo/Animation/BoneKeyFrame.h>
#include <echo/Animation/Bone.h>

#include <set>

namespace Echo
{
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Animation::Animation(const std::string& name) : mName(name), mLongestTrackTime(0), mTempKeyFrame()
	{
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Animation::~Animation()
	{
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void Animation::AnimationLengthAdjusted(Seconds time)
	{
		if(time > mLongestTrackTime)
		{
			mLongestTrackTime = time;
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	shared_ptr<AnimationTrack> Animation::CreateAnimationTrack( size_t forIndex )
	{	
		if(mTracks.find(forIndex)!=mTracks.end())
		{
			return shared_ptr<AnimationTrack>();
		}
		shared_ptr<AnimationTrack> animationTrack=_createAnimationTrack(forIndex);
		if(animationTrack)
		{
			if(!mTempKeyFrame)
			{
				mTempKeyFrame=animationTrack->_createKeyFrameInternal();
			}
			mTracks[forIndex]=animationTrack;
		}
		return animationTrack;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	shared_ptr<AnimationTrack> Animation::GetAnimationTrack( size_t forIndex )
	{
		std::map<size_t, shared_ptr<AnimationTrack> >::iterator it=mTracks.find(forIndex);
		if(it!=mTracks.end())
		{
			return (*it).second;
		}
		return shared_ptr<AnimationTrack>();
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void Animation::Apply(AnimationTimeIndex& timeIndex, std::map<size_t, AnimationTimeIndex>& trackTimeIndexes, f32 weight)
	{
		std::map<size_t, shared_ptr<AnimationTrack> >::iterator it=mTracks.begin();
		std::map<size_t, shared_ptr<AnimationTrack> >::iterator itEnd=mTracks.end();
		while( it != itEnd )
		{
			trackTimeIndexes[it->first].SetTime(timeIndex.GetTime());
			if(it->second->GetFrameAt(trackTimeIndexes[it->first],*mTempKeyFrame))
			{
				ApplyKeyFrame(*it->second, *mTempKeyFrame, weight);
			}
			++it;
		}
	}
}
