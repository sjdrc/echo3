#ifndef _ECHOANIMATION_H_
#define _ECHOANIMATION_H_

#include <echo/Types.h>
#include <echo/Animation/AnimationTimeIndex.h>
#include <string>
#include <map>

namespace Echo
{
	class AnimationTrack;
	class KeyFrame;

	/**
	 * Animation base class.
	 * Animations are made up of AnimationTracks that represent components of an animation.
	 * AnimationTracks hold key frame data that is used in an interpolation function for the
	 * specific type of animation.
	 * 
	 * Animation objects can blend animation tracks
	 */
	class Animation
	{
	protected:
		std::string mName;
		std::map<size_t, shared_ptr<AnimationTrack> > mTracks;
		Seconds mLongestTrackTime;
		shared_ptr<KeyFrame> mTempKeyFrame;
		
		Animation(const std::string& name);
		virtual ~Animation();

		/**
		* @brief	Implemented in specialised animation classes to create an animation track specific to that animation type.
		* @return   A new EAnimationTrack of the subtype
		* @param	u32 index
		*/
		virtual shared_ptr<AnimationTrack> _createAnimationTrack(size_t index)=0;
	public:
		/**
		* @brief	public method for creating an animation track.
		* @details	This method internally calls _createAnimationTrack().
		* @return   0 if a track already exists for the index.
		* @param	u32 forIndex			Index indicator as track reference. The actual index allocations should be managed by
		*									the using class.
		*/
		shared_ptr<AnimationTrack> CreateAnimationTrack( size_t forIndex );

		/**
		* @brief	Get the name of the animation.
		* @return   Name of the animation as an EHashToken.
		*/
		const std::string& GetName() const { return mName; }

		/**
		* @brief	When the animation length changes this method should be called.
		* @details	This method is called by EAnimationTrack when the animation length changes from the addition of a KeyFrame.
		*			The calling class does not need to know if the time passed is the longest time or not, that is managed
		*			internally. The calling class should be passing its longest known time. In the case of a EAnimationTrack, it
		*			reports when it's length grows.
		* @param	time				New longest known time in seconds.
		*/
		void AnimationLengthAdjusted(Seconds time);

		/**
		* @brief	Get length of the animation.
		* @return   Animation time in seconds.
		*/
		const Seconds& GetLength() const { return mLongestTrackTime; }

		/**
		* @brief	Gets an EAnimationTrack for a given index reference.
		* @return   If there is an EAnimationTrack* for the given index then a valid EAnimationTrack* is returned, otherwise false.
		* @param	u32 forIndex
		*/
		shared_ptr<AnimationTrack> GetAnimationTrack( size_t forIndex );

		/**
		* @brief	Apply the animation at a given time index.
		* @param	AnimationTimeIndex & timeIndex
		* @param	std::map<size_t, AnimationTimeIndex>& trackTimeIndexes
		* @param	f32 weight
		*/
		void Apply(AnimationTimeIndex& timeIndex, std::map<size_t, AnimationTimeIndex>& trackTimeIndexes, f32 weight);

		/**
		* @brief	ApplyKeyFrame
		* @param	AnimationTrack* track
		* @param	KeyFrame* keyframe
		* @param	f32 weight
		*/
		virtual void ApplyKeyFrame(AnimationTrack& track, KeyFrame& keyframe, f32 weight)=0;
	};
}
#endif 
