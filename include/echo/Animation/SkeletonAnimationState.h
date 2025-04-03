#ifndef _ECHOSKELETONANIMATIONSTATE_H_
#define _ECHOSKELETONANIMATIONSTATE_H_

#include <echo/Animation/AnimationTimeIndex.h>
#include <echo/Animation/AnimationState.h>

namespace Echo
{
	class Skeleton;
	class SkeletonAnimation;

	class SkeletonAnimationState : public AnimationState
	{
	public:
		~SkeletonAnimationState();

		Skeleton& GetSkeleton() const { return mSkeleton; }
	private:
		friend class Skeleton;
		Skeleton& mSkeleton;
		SkeletonAnimationState(const std::string& name, shared_ptr<SkeletonAnimation> animation, Skeleton& skeleton);
		
		/**
		 * AnimationState override for when the time of the animation is updated.
		 * SkeletonAnimationState will notify the Skeleton that transforms need updating before being used. 
		 */
		virtual void OnTimeChanged() override;
	};
}
#endif 
