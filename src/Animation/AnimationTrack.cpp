#include <echo/Animation/AnimationTrack.h>
#include <echo/Animation/KeyFrame.h>
#include <echo/Animation/AnimationTimeIndex.h>

namespace Echo
{
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	AnimationTrack::AnimationTrack(Animation& animation) : mAnimation(animation), mAnimationLength(0){}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	AnimationTrack::~AnimationTrack()
	{
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void AnimationTrack::AddKeyFrame(shared_ptr<KeyFrame> keyframe)
	{
		if(keyframe->mTime > mAnimationLength)
		{
			mAnimationLength = keyframe->mTime;
			mAnimation.AnimationLengthAdjusted(mAnimationLength);
		}

		if(mKeyFrames.empty())
		{
			mKeyFrames.push_back(keyframe);
		}else
		{
			//Add the keyframe to the list in the correct position
			u32 i=0;
			while(keyframe->mTime > mKeyFrames[i]->mTime)
			{
				++i;
				if(i == mKeyFrames.size())
				{
					mKeyFrames.push_back(keyframe);
					return;
				}
			}

			//Now we need to insert the keyframe at i and shuffle all back
			//Push the last element back.
			u32 b=mKeyFrames.size()-1;
			mKeyFrames.push_back(mKeyFrames[b]);
			b--;
			//Move each element one by one until we get to our key
			while(b > i && b > 0)
			{
				mKeyFrames[b+1]=mKeyFrames[b];
				b--;
			}
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	shared_ptr<KeyFrame> AnimationTrack::CreateKeyFrame(Seconds time)
	{
		shared_ptr<KeyFrame> keyframe=_createKeyFrameInternal();
		keyframe->mTime=time;
		AddKeyFrame(keyframe);
		return keyframe;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool AnimationTrack::GetFrameAt(AnimationTimeIndex& inOutTime, KeyFrame& outKeyFrame)
	{
		if(mKeyFrames.empty())		//We can't do anything without any data
		{
			return false;
		}
		
		u32 lowerIndex=0;
		if(inOutTime.HasIndex())
		{
			lowerIndex=inOutTime.GetLowerIndex();
		}

		if(mAnimationLength==Seconds(0.) || inOutTime.GetTime() == Seconds(0.))
		{
			return InterpolateKeyFrames(0.0f, *mKeyFrames[0], *mKeyFrames[0], outKeyFrame);
		}

		if(inOutTime.GetTime() > mAnimationLength)
		{
			return InterpolateKeyFrames(0.0f, *mKeyFrames[mKeyFrames.size()-1], *mKeyFrames[mKeyFrames.size()-1], outKeyFrame);
		}

		u32 upperLimit=mKeyFrames.size()-1;
		while( lowerIndex < upperLimit )
		{
			if(mKeyFrames[lowerIndex]->mTime > inOutTime.GetTime())
			{
				break;
			}else
			{
				++lowerIndex;
			}
		}
		--lowerIndex;	//Go back one keyframe
		inOutTime.SetLowerIndex(lowerIndex);

		//Two frames are
		Seconds currentTimeBetween=inOutTime.GetTime() - mKeyFrames[lowerIndex]->mTime;
		Seconds lengthBetweenThisAndLast=mKeyFrames[lowerIndex+1]->mTime - mKeyFrames[lowerIndex]->mTime;
		f32 percentThrough=currentTimeBetween.count()/lengthBetweenThisAndLast.count();

		//Interpolate
		return InterpolateKeyFrames(percentThrough, *mKeyFrames[lowerIndex],*mKeyFrames[lowerIndex+1],outKeyFrame);
	}
}
