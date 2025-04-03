#include <echo/Animation/SkeletonAnimationState.h>
#include <echo/Animation/SkeletonAnimation.h>
#include <echo/Animation/Skeleton.h>

namespace Echo
{
	SkeletonAnimationState::SkeletonAnimationState(const std::string& name, shared_ptr<SkeletonAnimation> animation, Skeleton& skeleton)
		: AnimationState(name,animation), mSkeleton(skeleton)
	{

	}

	SkeletonAnimationState::~SkeletonAnimationState()
	{
	}
	
	void SkeletonAnimationState::OnTimeChanged()
	{
		mSkeleton.MarkTransformsOutOfDate();
	}
}
