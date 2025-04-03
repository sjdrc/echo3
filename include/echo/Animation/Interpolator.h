#ifndef _ECHOINTERPOLATOR_H_
#define _ECHOINTERPOLATOR_H_

#include <echo/Types.h>
#include <echo/Kernel/Task.h>
#include <echo/Maths/EchoMaths.h>
#include <echo/cpp/functional>

namespace Echo
{
	/**
	 * A class that manages interpolation between two values over a specified time.
	 * Each time you set the target the interpolator will be activated. It will interpolate to the
	 * target in the specified amount of time.
	 * A callback is called whenever the value changes. When interpolation completes a
	 * callback will be called if provided.
	 * Time needs to added via AddTime() during which call backs will be fired.
	 */
	template< typename T >
	class Interpolator
	{
	public:
		typedef function<void(void)> TargetReachedCallback;
		typedef function<void(const T&)> ValueUpdateCallback;
		Interpolator(T initial, ValueUpdateCallback valueUpdateCallback, TargetReachedCallback completeCallback = TargetReachedCallback()) :
			mCurrent(initial),
			mTarget(initial),
			mOrigin(initial),
			mComplete(true),
			mProgressTimeMultiplier(1.f),
			mProgress(1.f),
			mValueUpdateCallback(valueUpdateCallback),
			mCompleteCallback(completeCallback)
		{
		}
		Interpolator(T initial, T target, ValueUpdateCallback valueUpdateCallback, TargetReachedCallback completeCallback = TargetReachedCallback()) :
			mCurrent(initial),
			mTarget(target),
			mOrigin(initial),
			mComplete(false),
			mProgressTimeMultiplier(1.f),
			mProgress(1.f),
			mValueUpdateCallback(valueUpdateCallback),
			mCompleteCallback(completeCallback)
		{
		}
		virtual ~Interpolator(){}

		/**
		 * Set a target to interpolate to.
		 * Interpolation occurs during AddTime() calls.
		 * Changing the target while the interpolation is in progress will reset the progress to 0% and
		 * the current value will become the origin.
		 * @param target The target you want to interpolate to.
		 * @param timeToTarget The amount of time it should take to interpolate to the target.
		 */
		void SetTarget(const T& target, Seconds timeToTarget)
		{
			mTarget = target;
			mOrigin = mCurrent;
			mComplete = false;
			mProgressTimeMultiplier = 1.f / timeToTarget.count();
			mProgress = 0.f;
		}

		/**
		 * Set the origin to interpolate from.
		 * Interpolation occurs during AddTime() calls.
		 * Changing the origin while the interpolation is in progress will reset the progress to 0% and
		 * the current value will become the origin.
		 * @param origin The origin you want to interpolate to.
		 * @param timeToTarget The amount of time it should take to interpolate to the target.
		 */
		void SetOrigin(const T& origin, Seconds timeToTarget)
		{
			mCurrent = mOrigin;
			mComplete = false;
			mProgressTimeMultiplier = 1.f / timeToTarget.count();
			mProgress = 0.f;
			
			//Since the value changed, update it.
			if(mValueUpdateCallback)
			{
				mValueUpdateCallback(mCurrent);
			}
		}

		/**
		 * Set the interpolator origin and target to interpolate through.
		 * Interpolation occurs during AddTime() calls.
		 * Changing the target while the interpolation is in progress will reset the progress to 0%.
		 * @param origin The origin the interpolation should start from.
		 * @param target The target you want to interpolate to.
		 * @param timeToTarget The amount of time it should take to interpolate to the target.
		 */
		void Set(const T& origin, const T& target, Seconds timeToTarget)
		{
			mTarget = target;
			mOrigin = origin;
			mCurrent = mOrigin;
			mComplete = false;
			mProgressTimeMultiplier = 1.f / timeToTarget.count();
			mProgress = 0.f;
			
			//Since the value changed, update it.
			if(mValueUpdateCallback)
			{
				mValueUpdateCallback(mCurrent);
			}
		}
		
		/**
		 * Add time to the interpolation progress.
		 * @note Calling this method if the object is in the complete state will return immediately. To
		 * activate call SetTarget().
		 * @param seconds How much time to progress the interpolation by. The value can be anything, including
		 * a negative value, to allow for progressing backwards if desired. Progress will be clamped between
		 * 0% and 100% inclusive however there is only a callback for forward time to 100% as 0% is assumed
		 * to be the origin.
		 */
		void AddTime(Seconds seconds)
		{
			if(mComplete)
			{
				return;
			}
			if(seconds.count() > 0.f)
			{
				mProgress += mProgressTimeMultiplier * seconds.count();
				if(mProgress>=1.f)
				{
					mProgress = 1.f;
					mComplete = true;
				}
			}else
			{
				mProgress += mProgressTimeMultiplier * seconds.count();
				if(mProgress<0.f)
				{
					mProgress = 0.f;
				}
			}
			mCurrent = Maths::LinearInterpolate(mOrigin,mTarget,mProgress);
			if(mValueUpdateCallback)
			{
				mValueUpdateCallback(mCurrent);
			}
			
			//Did we finish the interpolation?
			if(mComplete)
			{
				if(mCompleteCallback)
				{
					mCompleteCallback();
				}
			}
		}

		/**
		 * Return whether the interpolation is complete or not.
		 * @return true if interpolation is complete, otherwise false.
		 */
		bool IsComplete() const {return mComplete;}
		
		/**
		 * Get the current interpolation value.
		 * @return the current interpolation value.
		 */
		const T& GetCurrent() const {return mCurrent;}

		/**
		 * Get the target.
		 * @return The target value.
		 */
		const T& GetTarget() const {return mTarget;}

		/**
		 * Get the interpolation origin.
		 * @return The origin value.
		 */
		const T& GetOrigin() const {return mOrigin;}
		
		/**
		 * Get the interpolation progress.
		 * @return 
		 */
		f32 GetProgress() const {return mProgress;}
	private:
		T mCurrent;
		T mTarget;					/// The interpolation target.
		T mOrigin;					/// The interpolation origin.
		bool mComplete;				/// Flag whether the transition is complete.
		f32 mProgressTimeMultiplier;/// The multiplier applied to the passing time to give the percent progress step.
		f32 mProgress;				/// The progress of the interpolation, 0-1.
		ValueUpdateCallback mValueUpdateCallback;
		TargetReachedCallback mCompleteCallback;
	};

	/**
	 * An Interpolator in Task form so it can update as a Task.
	 */
	template< typename T >
	class InterpolatorTask : public Interpolator<T>, public Task
	{
	public:
		typedef typename Interpolator<T>::TargetReachedCallback TargetReachedCallback;
		typedef typename Interpolator<T>::ValueUpdateCallback ValueUpdateCallback;
		InterpolatorTask(T initial, ValueUpdateCallback valueUpdateCallback, TargetReachedCallback completeCallback = TargetReachedCallback()) :
			Interpolator<T>(initial,valueUpdateCallback,completeCallback)
		{
		}
		
		InterpolatorTask(T initial, T target, ValueUpdateCallback valueUpdateCallback, TargetReachedCallback completeCallback = TargetReachedCallback()) :
			Interpolator<T>(initial,target,valueUpdateCallback,completeCallback)
		{
		}
		
		virtual void Update(Seconds lastFrameTime) override
		{
			Interpolator<T>::AddTime(lastFrameTime);
		}
	};
	
}
#endif 
