#ifndef _ECHOANIMATIONTRACK_H_
#define _ECHOANIMATIONTRACK_H_

#include <echo/Types.h>
#include <echo/Animation/Animation.h>
#include <vector>

namespace Echo
{
	class KeyFrame;
	class AnimationTimeIndex;

	/**
	 * AnimationTrack is an abstract base representing a time track of key frames.
	 * AnimationTrack objects store key frame data of a specific type and provide a mechanism
	 * for the interpolation of key frame data.
	 * 
	 * This base class is stored in an Animation instance which also processes key frames
	 * generically. Derived AnimationTrack classes must match the track they are designed
	 * for (i.e. both process the same key frame) otherwise it will result in an incorrect
	 * downcast of the key frame type.
	 * 
	 * An example match is SkeletonAnimation matched with a	BoneAnimationTrack. The
	 * derived Animation class must create the correct AnimationTrack through the
	 * virtual method _createAnimationTrack(). Similarly, the derived AnimationTrack
	 * class should create the correct KeyFrame class.
	 */
	class AnimationTrack
	{
	public:
		/**
		 * Constructor
		 * @param animation that this track will belong to.
		 */
		AnimationTrack(Animation& animation);

		/**
		 * Destructor.
		 */
		virtual ~AnimationTrack();

		/**
		 * Add a key frame to the animation track, transferring ownership.
		 */
		void AddKeyFrame(shared_ptr<KeyFrame> keyframe);

		/**
		 * Create a key frame compatible with this track.
		 * The derived key frame type is returned as a pointer to the base, ownership of
		 * the key frame is maintained by the AnimationTrack.
		 */
		shared_ptr<KeyFrame> CreateKeyFrame(Seconds time);

		/**
		 * This function returns the interpolated frame at the time specified.
		 * inOutTime will be updated to include the last lower key frame index used. This
		 * means the next time the same time index is passed into this method the method
		 * can look up the correct key frames faster.
		 * An interpolation will not be possible if there are insufficient key frames or for
		 * another reason determined by the derived class' InterpolateKeyFrames() method.
		 * @param inOutTime
		 * @param outKeyFrame	Must be a key frame object that is compatible with the animation track,
		 *						There is no guarantee that InterpolateKeyFrames() will do any type checking.
		 * @return true of a interpolation was possible.
		 */
		bool GetFrameAt(AnimationTimeIndex& inOutTime, KeyFrame& outKeyFrame);

		/**
		 * Get the length of the animation track.
		 * @return the animation track length.
		 */
		const Seconds& GetAnimationLength() const { return mAnimationLength; }

		/**
		 * Interpolates two KeyFrames of the derived key frame type.
		 * The method can safely assume that the key frames will be the same type as what is
		 * returned by _createKeyFrameInternal(), so type checking is not required.
		 * @return true if the interpolation was possible.
		 */
		virtual bool InterpolateKeyFrames(f32 percent, KeyFrame& keyframe01, KeyFrame& keyframe02, KeyFrame& outKeyFrame)=0;
	private:
		/**
		 * Returns a new KeyFrame of the derived type.
		 */
		friend class Animation;
		virtual shared_ptr<KeyFrame> _createKeyFrameInternal()=0;
	protected:
		Animation& mAnimation;
		std::vector< shared_ptr<KeyFrame> > mKeyFrames;
		Seconds mAnimationLength;
	};
}
#endif 
