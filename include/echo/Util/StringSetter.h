#ifndef ECHOSTRINGSETTER_H
#define ECHOSTRINGSETTER_H

#include <echo/Kernel/Task.h>
#include <echo/cpp/functional>
#include <vector>

namespace Echo
{
	/**
	 * A Task that will call a set function periodically to build the provided string one word at a time.
	 */
	class StringSetter : public Task
	{
	public:
		typedef function<void(const std::string&)> SetFunction;
		struct Increments
		{
			enum _
			{
				WORD,			// Each update increment will add another word.
				CHARACTER		// Each update increment will add another character.
			};
		};
		typedef Increments::_ Increment;
		
		StringSetter(const std::string& text, Increment increment, Scalar incrementsPerSecond, SetFunction setFunction);
		void Update(Seconds lastFrameTime) override;

		/**
		 * Set what to increment by.
		 * @note Calling this method will reset the current progress even if the increment is the
		 * same and regardless of the loop setting.
		 * @param increment The increment, see Increments.
		 */
		void SetIncrement(Increment increment);
		
		/**
		 * Set the number of increments per second.
		 * @param incrementsPerSecond Increments per second, must be > 0 otherwise it defaults to 1.
		 */
		void SetIncrementsPerSecond(Scalar incrementsPerSecond);

		/**
		 * Set whether the setter should loop.
		 * Looping is enabled by default.
		 * @note If looping is disabled the Task is paused.
		 * @param loop true if the setter should loop.
		 * @param autoStart if true and looping is enabled the Task will resume if paused.
		 */
		void SetLoop(bool loop, bool autoStart=true);
	private:
		void ChangeContent(Size numberOfIncrements);
		
		Seconds mTimeSinceLastIncrement;
		Increment mIncrement;
		Seconds mSecondsPerIncrement;
		SetFunction mSetFunction;
		std::vector<std::string> mWords;
		size_t mCurrentIncrement;
		std::string mOriginalText;
		std::string mCurrentText;
		bool mLoop;
	};
}

#endif
