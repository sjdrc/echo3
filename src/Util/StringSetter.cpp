#include <echo/Util/StringSetter.h>
#include <echo/Util/StringUtils.h>

namespace Echo
{
	StringSetter::StringSetter(const std::string& text, Increment increment, Scalar incrementsPerSecond, SetFunction setFunction) : Task("StringSetter"),
		mTimeSinceLastIncrement(0),
		mSetFunction(setFunction),
		mCurrentIncrement(0),
		mOriginalText(text),
		mLoop(true)
	{
		SetIncrementsPerSecond(incrementsPerSecond);
		SetIncrement(increment);
		mCurrentText.reserve(mOriginalText.length()+1);
	}
	
	void StringSetter::SetIncrement(Increment increment)
	{
		if(increment==Increments::WORD)
		{
			Utils::String::Split(mOriginalText," ",mWords);
		}else
		{
			//Increments::CHARACTER - We don't need the words.
			mWords.clear();
		}
		mCurrentIncrement=0;
		mTimeSinceLastIncrement=Seconds(0);
		mIncrement=increment;
	}
	
	void StringSetter::SetLoop(bool loop, bool autoStart)
	{
		mLoop=loop;
		if(mLoop && autoStart && GetPaused())
		{
			Resume();
		}
	}
	
	void StringSetter::SetIncrementsPerSecond(Scalar incrementsPerSecond)
	{
		//We can't have a negative increment.
		if(incrementsPerSecond<=0)
		{
			incrementsPerSecond=1;
		}
		mSecondsPerIncrement=Seconds(1./incrementsPerSecond);
	}
	
	void StringSetter::Update(Seconds lastFrameTime)
	{
		mTimeSinceLastIncrement+=lastFrameTime;
		if(mTimeSinceLastIncrement>mSecondsPerIncrement)
		{
			Size numberOfIncrements = mTimeSinceLastIncrement.count() / mSecondsPerIncrement.count();
			ChangeContent(numberOfIncrements);
			// Calculate the character time passed otherwise we won't get the actual requested speed.
			mTimeSinceLastIncrement-=mSecondsPerIncrement*numberOfIncrements;
		}
	}
	
	void StringSetter::ChangeContent(Size numberOfIncrements)
	{
		if(mOriginalText.empty())
		{
			return;
		}
		while(numberOfIncrements>0)
		{
			numberOfIncrements--;
			mCurrentIncrement++;
			if(mIncrement==Increments::WORD)
			{
				if(mCurrentIncrement>=mWords.size())
				{
					mCurrentIncrement=0;
					if(!mLoop)
					{
						Pause();
						return;
					}
					mCurrentText="";
				}
				mCurrentText+=mWords[mCurrentIncrement] + " ";
			}else
			{
				//Increments::CHARACTER
				if(mCurrentIncrement>=mOriginalText.length())
				{
					mCurrentIncrement=0;
					if(!mLoop)
					{
						Pause();
						return;
					}
					mCurrentText="";
				}
				mCurrentText+=mOriginalText[mCurrentIncrement];
			}
		}
		if(mSetFunction)
		{
			mSetFunction(mCurrentText);
		}
	}
}
