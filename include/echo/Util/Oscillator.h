#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include <echo/Kernel/Task.h>
#include <echo/cpp/functional>

namespace Echo
{
	template<typename ConvertType>
	class Oscillator : public Task
	{
	public:
		typedef function<void(ConvertType,ConvertType)> SetFunction;

		Oscillator(SetFunction setFunction, f32 magnitudeMultiplier = 1.f, f32 speedMultiplier = 1.f, f32 startAngle = 0.f) :
			mSetFunction(setFunction),
			mAngle(startAngle),
			mSpeedMultiplier(speedMultiplier),
			mMagnitudeMultiplier(magnitudeMultiplier)
		{

		}

		void Update(Seconds lastFrameTime)
		{
			mAngle += static_cast<f32>(lastFrameTime.count()) * mSpeedMultiplier;
			mSetFunction(ConvertType(Maths::Sin(mAngle,false) * mMagnitudeMultiplier),ConvertType(Maths::Cos(mAngle,false) * mMagnitudeMultiplier));
		}

		void SetMagnitudeMultiplier(f32 magnitudeMultiplier)
		{
			mMagnitudeMultiplier = magnitudeMultiplier;
		}

		f32 GetMagnitudeMultiplier() const
		{
			return mMagnitudeMultiplier;
		}

		void SetSpeedMultiplier(f32 speedMultiplier)
		{
			mSpeedMultiplier = speedMultiplier;
		}

		f32 GetSpeedMultiplier() const
		{
			return mSpeedMultiplier;
		}

	private:
		SetFunction mSetFunction;
		f32 mAngle;
		f32 mSpeedMultiplier;
		f32 mMagnitudeMultiplier;
	};
	
	template<>
	class Oscillator<f32> : public Task
	{
	public:
		typedef function<void(f32,f32)> SetFunction;

		Oscillator(SetFunction setFunction, f32 magnitudeMultiplier = 1.f, f32 speedMultiplier = 1.f, f32 startAngle = 0.f) :
			mSetFunction(setFunction),
			mAngle(startAngle),
			mSpeedMultiplier(speedMultiplier),
			mMagnitudeMultiplier(magnitudeMultiplier)
		{

		}

		void Update(Seconds lastFrameTime)
		{
			mAngle += static_cast<f32>(lastFrameTime.count()) * mSpeedMultiplier;
			mSetFunction(Maths::Sin(mAngle,false) * mMagnitudeMultiplier,Maths::Cos(mAngle,false) * mMagnitudeMultiplier);
		}

		void SetMagnitudeMultiplier(f32 magnitudeMultiplier)
		{
			mMagnitudeMultiplier = magnitudeMultiplier;
		}

		f32 GetMagnitudeMultiplier() const
		{
			return mMagnitudeMultiplier;
		}

		void SetSpeedMultiplier(f32 speedMultiplier)
		{
			mSpeedMultiplier = speedMultiplier;
		}

		f32 GetSpeedMultiplier() const
		{
			return mSpeedMultiplier;
		}

	private:
		SetFunction mSetFunction;
		f32 mAngle;
		f32 mSpeedMultiplier;
		f32 mMagnitudeMultiplier;
	};	
}

#endif
