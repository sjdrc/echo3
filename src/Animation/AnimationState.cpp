#include <echo/Animation/AnimationState.h>
#include <echo/Animation/Animation.h>
#include <echo/Maths/EchoMaths.h>
#include <echo/cpp/functional>
#include <boost/foreach.hpp>

namespace Echo
{
	AnimationState::AnimationState(const std::string& name, shared_ptr<Animation> animation)
		: mName(name),
		mAnimation(animation),
		mWeight(1.f),
		mWeightInterpolator(1.,	bind(&AnimationState::OnWeightInterpolationValue,this,placeholders::_1),
								bind(&AnimationState::OnWeightInterpolationComplete,this)),
		mDisableOnInterpolationCompletion(false),
		mTimeMultiplier(1.f),
		mLoop(true),
		mEnded(false),
		mEnabled(true)
	{

	}

	AnimationState::~AnimationState()
	{
	}

	Seconds AnimationState::GetTimePosition()
	{
		return mTimeIndex.GetTime();
	}

	Seconds AnimationState::GetLength() const
	{
		return mAnimation->GetLength();
	}

	void AnimationState::SetTimePosition(const Seconds& time)
	{
		if(mLoop)
		{
			while( time > mAnimation->GetLength() )
			{
				mTimeIndex.SetTime(mTimeIndex.GetTime() - mAnimation->GetLength());
			}
			mEnded=false;
		}else
		{
			if( time > mAnimation->GetLength() )
			{
				mEnded=true;
				mTimeIndex.SetTime(mAnimation->GetLength());
			}else
			{
				mTimeIndex.SetTime(time);
			}
		}
		OnTimeChanged();
	}

	bool AnimationState::HasEnded()
	{
		return mEnded;
	}

	void AnimationState::AddTime(Seconds seconds)
	{
		mTimeIndex.AddTime(seconds * mTimeMultiplier);

		if(mAnimation->GetLength()==Seconds(0.))
		{
			mTimeIndex.SetTime(Seconds(0.));
		}else
		{
			if(mLoop)
			{
				while( mTimeIndex.GetTime() > mAnimation->GetLength() )
				{
					mTimeIndex.SetTime(mTimeIndex.GetTime() - mAnimation->GetLength());
				}
			}else
			{
				if( mTimeIndex.GetTime() > mAnimation->GetLength() )
				{
					mEnded=true;
					mTimeIndex.SetTime(mAnimation->GetLength());
				}
			}
		}
		
		if(!mWeightInterpolator.IsComplete())
		{
			//Callbacks will be called during AddTime().
			mWeightInterpolator.AddTime(seconds);
		}
		
		OnTimeChanged();
	}

	void AnimationState::Apply()
	{
		if(mEnabled)
		{
			mAnimation->Apply(mTimeIndex, mTrackTimeIndexes, mWeight);
		}
	}

	f32 AnimationState::GetWeightTarget() const
	{
		return mWeightInterpolator.GetTarget();
	}

	void AnimationState::SetWeightTarget(f32 weightTarget, Seconds timeToTarget, bool disableAnimationOnComplete)
	{
		mWeightInterpolator.SetTarget(weightTarget,timeToTarget);
		mDisableOnInterpolationCompletion = disableAnimationOnComplete;
	}

	void AnimationState::OnWeightInterpolationValue(f32 newWeight )
	{
		mWeight = newWeight;
	}
	
	void AnimationState::OnWeightInterpolationComplete()
	{
		typedef std::pair< const std::string, std::vector<WeightInterpolationEndFunction> > IdentifierFunctionPair;
		BOOST_FOREACH(IdentifierFunctionPair& ifp, mWeightInterpolationEndFunctions)
		{
			BOOST_FOREACH(WeightInterpolationEndFunction& callback, ifp.second)
			{
				callback(*this, mWeight);
			}
		}
		if(mDisableOnInterpolationCompletion)
		{
			mEnabled = false;
		}
	}

	void AnimationState::Update(Seconds lastFrameTime)
	{
		AddTime(lastFrameTime);
	}
	
	void AnimationState::AddWeightInterpolationEndFunction(const std::string& identifer, WeightInterpolationEndFunction weightInterpolationEndFunction)
	{
		mWeightInterpolationEndFunctions[identifer].push_back(weightInterpolationEndFunction);
	}

	void AnimationState::RemoveWeightInterpolationEndFunctions(const std::string& identifer)
	{
		NamedWeightInterpolationEndFunctionsMap::iterator it = mWeightInterpolationEndFunctions.find(identifer);
		if(it!=mWeightInterpolationEndFunctions.end())
		{
			mWeightInterpolationEndFunctions.erase(it);
		}
	}
}
