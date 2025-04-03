#ifndef _ECHOBONEANIMATIONTRACK_H_
#define _ECHOBONEANIMATIONTRACK_H_

#include <echo/Animation/AnimationTrack.h>

namespace Echo
{
	class Bone;

	/**
	 * BoneAnimationTrack creates and interpolates BoneKeyFrames.
	 * @see AnimationTrack for more information on AnimationTracks in general.
	 */
	class BoneAnimationTrack : public AnimationTrack
	{
	private:
		shared_ptr<Bone> mBone;
	public:
		/**
		 * Construct an animation track for specified bone and animation.
		 * @param bone that the animation track is for.
		 * @param animation animation that the track is part of.
		 */
		BoneAnimationTrack(shared_ptr<Bone> bone, Animation& animation);
		
		/**
		 * Constructor to create a copy of an animation track for a different bone and animation.
		 * This constructor is used when cloning a skeleton animation to make a copy of the
		 * bone animation track for a different bone.
		 * @param animationTrack to copy.
		 * @param bone the track is for.
		 * @param animation object used by AnimationTrack base class.
		 */
		BoneAnimationTrack(BoneAnimationTrack& animationTrack, shared_ptr<Bone> bone, Animation& animation);
		
		/**
		 * Destructor.
		 */
		~BoneAnimationTrack();
		
		/**
		 * Overridden method to create BoneKeyFrame.
		 * @return 
		 */
		shared_ptr<KeyFrame> _createKeyFrameInternal();

		/**
		 * Get the bone associated with this animation track.
		 * @return The bone associated with this animation track.
		 */
		shared_ptr<Bone> GetBone() const { return mBone; }

		/**
		 * Interpolate between two bone key frames.
		 * @note This method does not do any type checking on the passed in key frame objects.
		 * @param percent The percent between the two key frames to interpolate.
		 * @param keyframe01 First key frame, needs to be a BoneKeyFrame object.
		 * @param keyframe02 Second key frame, needs to be a BoneKeyFrame object.
		 * @param outKeyFrame The key frame object to receive the interpolated result, needs to be a BoneKeyFrame object.
		 * @return true if the interpolation was successful, otherwise false.
		 */
		bool InterpolateKeyFrames(f32 percent, KeyFrame& keyframe01, KeyFrame& keyframe02, KeyFrame& outKeyFrame);
	};
}
#endif 
