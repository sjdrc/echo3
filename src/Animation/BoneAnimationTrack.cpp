#include <echo/Animation/BoneAnimationTrack.h>
#include <echo/Animation/BoneKeyFrame.h>
#include <echo/Animation/Bone.h>
#include <echo/Maths/Quaternion.h>
#include <boost/foreach.hpp>

namespace Echo
{
	
	BoneAnimationTrack::BoneAnimationTrack(shared_ptr<Bone> bone, Animation& animation) : AnimationTrack(animation), mBone(bone){}
	
	BoneAnimationTrack::BoneAnimationTrack(BoneAnimationTrack& animationTrack, shared_ptr<Bone> bone, Animation& animation) : AnimationTrack(animation), mBone(bone)
	{
		BOOST_FOREACH(shared_ptr<KeyFrame>& keyFrame, animationTrack.mKeyFrames)
		{
			BoneKeyFrame* boneKeyFrame = static_cast<BoneKeyFrame*>(keyFrame.get());
			shared_ptr<BoneKeyFrame> boneKeyFrameCopy(new BoneKeyFrame(*boneKeyFrame));
			AddKeyFrame(boneKeyFrameCopy);
		}
	}

	BoneAnimationTrack::~BoneAnimationTrack(){}

	shared_ptr<KeyFrame> BoneAnimationTrack::_createKeyFrameInternal()
	{
		return shared_ptr<KeyFrame>(new BoneKeyFrame());
	}

	bool BoneAnimationTrack::InterpolateKeyFrames(f32 percent, KeyFrame& keyframe01, KeyFrame& keyframe02, KeyFrame& outKeyFrame)
	{
		BoneKeyFrame& frame01=static_cast<BoneKeyFrame&>(keyframe01);
		BoneKeyFrame& frame02=static_cast<BoneKeyFrame&>(keyframe02);
		BoneKeyFrame& outFrame=static_cast<BoneKeyFrame&>(outKeyFrame);

		if(percent==0.0f)
		{
			outFrame.mPosition=frame01.mPosition;
			outFrame.mScale=frame01.mScale;
			outFrame.mOrientation=frame01.mOrientation;
		}else
		{
			outFrame.mPosition=Maths::LinearInterpolate(frame01.mPosition,frame02.mPosition,percent);
			outFrame.mScale=Maths::LinearInterpolate(frame01.mScale,frame02.mScale,percent);
			outFrame.mOrientation=frame01.mOrientation.Nlerp(frame02.mOrientation, percent, true);
		}
		return true;
	}
}
