#ifndef _ECHOSKELETONANIMATION_H_
#define _ECHOSKELETONANIMATION_H_

#include <echo/Types.h>
#include <echo/Animation/Animation.h>
#include <vector>

namespace Echo
{
	class Skeleton;
	class AnimationTimeIndex;
	class BoneAnimationTrack;
	class Bone;

	/**
	 * SkeletonAnimation.
	 * A SkeletonAnimation has BoneAnimationTracks that create and use BoneKeyFrames for
	 * each animated bone in the skeleton.
	 * 
	 * 
	 */
	class SkeletonAnimation : public Animation
	{
	private:
		friend class Skeleton;
		Skeleton& mSkeleton;

		SkeletonAnimation(const std::string& name, Skeleton& skeleton);
	
		shared_ptr<AnimationTrack> _createAnimationTrack(size_t index);
	public:
		~SkeletonAnimation();
		
		shared_ptr<BoneAnimationTrack> CreateAnimationTrack( shared_ptr<Bone> forBone );
		
		shared_ptr<SkeletonAnimation> Clone(Skeleton& cloneParentSkeleton) const;

		shared_ptr<BoneAnimationTrack> GetAnimationTrack( shared_ptr<Bone> forBone );

		void ApplyKeyFrame(AnimationTrack& track, KeyFrame& keyFrame, f32 weight);
	};
}
#endif 
