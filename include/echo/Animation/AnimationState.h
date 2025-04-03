#ifndef _ECHOANIMATIONSTATE_H_
#define _ECHOANIMATIONSTATE_H_

#include <echo/Animation/AnimationTimeIndex.h>
#include <echo/Animation/Interpolator.h>
#include <echo/Kernel/Task.h>
#include <echo/cpp/functional>
#include <string>
#include <map>
#include <vector>

namespace Echo
{
	class AnimationTrack;
	class Animation;

	class AnimationState : public Task
	{
	public:
		typedef function<void(AnimationState&, f32) > WeightInterpolationEndFunction;

		virtual ~AnimationState();

		shared_ptr<Animation> GetAnimation() const { return mAnimation; }
		const std::string& GetName() const { return mName; }

		bool GetLoop() const { return mLoop; }
		void SetLoop(bool val) { mLoop = val; }

		Seconds GetTimePosition();
		void SetTimePosition(const Seconds& time);

		bool GetEnabled() const { return mEnabled; }
		void SetEnabled(bool val) { mEnabled = val; }

		bool HasEnded();
		Seconds GetLength() const;

		f32 GetWeight() const { return mWeight; }
		void SetWeight(f32 val) { mWeight = val; }
		
		f32 GetTimeMultiplier() const { return mTimeMultiplier; }
		void SetTimeMultiplier(f32 val) { mTimeMultiplier = val; }

		void AddTime(Seconds seconds);
		void Apply();

		f32 GetWeightTarget() const;
		
		/**
		 * Set a target weight to interpolate to.
		 * Interpolation occurs during AddTime() calls. If you are using this AnimationState as a Task
		 * then this will be done automatically when it Updates.
		 * @param targetWeigth The weight you want to interpolate to.
		 * @param timeToTarget The amount of time it should take to interpolate to the target.
		 * @param disableAnimationOnComplete If true, after the interpolation finishes the animation
		 * will be disabled, this is useful if your target is a weight of 0 and your intent is to
		 * disable the animation at that point.
		 */
		void SetWeightTarget(f32 targetWeigth, Seconds timeToTarget, bool disableAnimationOnComplete);
		
		/**
		 * Task override.
		 * Update calls AddTime() to the animation progress and, if enabled, interpolates the
		 * weight from the current weight to a target weight based on a speed.
		 */
		virtual void Update(Seconds lastFrameTime) override;
		
		/**
		 * Add a function to call when weight interpolation completes.
		 * @param identifier The identifier to use for removal later. Try and keep this unique. Multiple
		 * functions can be added using the same identifier but they can't be removed individually.
		 * @param weightInterpolationEndFunction The function to call when interpolation completes.
		 */
		void AddWeightInterpolationEndFunction(const std::string& identifer, WeightInterpolationEndFunction weightInterpolationEndFunction);

		/**
		 * Remove all functions that would be called when weight interpolation completes.
		 * @param identifier The identifier that was used when Added.
		 */
		void RemoveWeightInterpolationEndFunctions(const std::string& identifer);
	protected:
		AnimationState(const std::string& name, shared_ptr<Animation> animation);
	private:
		/**
		 * This method is called internally when the time marker changes.
		 */
		virtual void OnTimeChanged() = 0;
		std::string mName;
		shared_ptr<Animation> mAnimation;
		AnimationTimeIndex mTimeIndex;
		std::map<size_t, AnimationTimeIndex > mTrackTimeIndexes;

		f32 mWeight;
		Interpolator< f32 > mWeightInterpolator;
		void OnWeightInterpolationValue(f32 newWeight);
		void OnWeightInterpolationComplete();
		bool mDisableOnInterpolationCompletion;
		
		f32 mTimeMultiplier;
		bool mLoop;
		bool mEnded;
		bool mEnabled;
		typedef std::map< std::string, std::vector<WeightInterpolationEndFunction> > NamedWeightInterpolationEndFunctionsMap;
		NamedWeightInterpolationEndFunctionsMap mWeightInterpolationEndFunctions;
	};
}
#endif 

