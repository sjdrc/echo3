#include <echo/Animation/SkeletonAnimation.h>
#include <echo/Animation/BoneAnimationTrack.h>
#include <echo/Animation/AnimationTimeIndex.h>
#include <echo/Animation/BoneAnimationTrack.h>
#include <echo/Animation/BoneKeyFrame.h>
#include <echo/Animation/Bone.h>
#include <echo/Animation/Skeleton.h>
#include <boost/foreach.hpp>

namespace Echo
{
	SkeletonAnimation::SkeletonAnimation(const std::string& name, Skeleton& skeleton) : Animation(name), mSkeleton(skeleton)
	{
		mTempKeyFrame.reset(new BoneKeyFrame());
	}

	SkeletonAnimation::~SkeletonAnimation()
	{
	}

	shared_ptr<AnimationTrack> SkeletonAnimation::_createAnimationTrack(size_t index)
	{
		shared_ptr<Bone> bone=mSkeleton.GetBone(index);
		if(!bone)
		{
			return shared_ptr<AnimationTrack>();
		}
		return shared_ptr<AnimationTrack>(new BoneAnimationTrack(bone, *this));
	}
	
	shared_ptr<BoneAnimationTrack> SkeletonAnimation::CreateAnimationTrack( shared_ptr<Bone> forBone )
	{
		shared_ptr<BoneAnimationTrack> track = dynamic_pointer_cast<BoneAnimationTrack>(Animation::CreateAnimationTrack(forBone->GetIndex()));
		return track;
	}
	
	shared_ptr<SkeletonAnimation> SkeletonAnimation::Clone(Skeleton& cloneParentSkeleton) const
	{
		shared_ptr<SkeletonAnimation> clonedAnimation(new SkeletonAnimation(GetName(), cloneParentSkeleton));

		typedef std::pair<const size_t, shared_ptr<AnimationTrack> > IndexTrackPair;
		BOOST_FOREACH(const IndexTrackPair& indexTrackPair,mTracks)
		{
			BoneAnimationTrack* boneTrack = static_cast<BoneAnimationTrack*>(indexTrackPair.second.get());
			shared_ptr<Bone> cloneBone = cloneParentSkeleton.GetBone(boneTrack->GetBone()->GetName());
			if(cloneBone)
			{
				clonedAnimation->mTracks[cloneBone->GetIndex()] = shared_ptr<BoneAnimationTrack>(new BoneAnimationTrack(*boneTrack,cloneBone,*clonedAnimation));
			}
		}
		return clonedAnimation;
	}

	shared_ptr<BoneAnimationTrack> SkeletonAnimation::GetAnimationTrack( shared_ptr<Bone> forBone )
	{
		return dynamic_pointer_cast<BoneAnimationTrack>(Animation::GetAnimationTrack(forBone->GetIndex()));
	}

	void SkeletonAnimation::ApplyKeyFrame(AnimationTrack& track, KeyFrame& keyFrame, f32 weight)
	{
		BoneAnimationTrack& bat=static_cast<BoneAnimationTrack&>(track);
		BoneKeyFrame& kf=static_cast<BoneKeyFrame&>(keyFrame);

		Vector3 pos=Maths::LinearInterpolate(Vector3(0,0,0),kf.mPosition,weight);
		//Vector3 scale=EUtil::LinearInterpolate(Vector3(1,1,1),kf->mScale,0.0f);
		Quaternion orientation=Quaternion::IDENTITY.Nlerp(kf.mOrientation, weight, true);
		shared_ptr<Bone> bone=bat.GetBone();

		//bone->Scale(scale);
		bone->Rotate(orientation);
		bone->Translate(pos);
		mSkeleton.MarkTransformsOutOfDate();
	}
}
