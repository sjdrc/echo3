#ifndef _ECHOREGEXUTIL_H_
#define _ECHOREGEXUTIL_H_
#include <echo/Types.h>
#include <string>
#include <boost/lexical_cast.hpp>
#include <echo/cpp/functional>
#include <boost/regex.hpp>
#include <iostream>

namespace Echo
{
	namespace Utils
	{
		namespace RegEx
		{
			/**
			 * Search a string using a regular expression that contains a capture and convert the capture before calling a function with the converted value.
			 * @param content The content to search.
			 * @param expression The regular expression containing a capture, note ^ and $ do not work for line beginning and end, if
			 * you want to match up until the end of a line try using [^\n]* (anything that is not end of line).
			 * @param function The function to call with the current match count and value.
			 */
			template<typename T>
			void ProcessRegExCapture(const std::string& content, const std::string& expression, function<void(size_t, const T&)> functionToCall)
			{
				if(!functionToCall || content.empty())
				{
					return;
				}
				boost::regex regEx(expression);
				//Split up the line into function and parameters. content[content.length()] is the null byte
				boost::cregex_iterator it(&content[0], &content[content.length()], regEx);
				boost::cregex_iterator itEnd;
				size_t matchCount = 0;
				while(it!=itEnd)
				{
					// (*it)[0] contains the whole string 
					// (*it)[1] contains the value
					std::string value((*it)[1].first, (*it)[1].second);
					try
					{
						functionToCall(matchCount,boost::lexical_cast<T>(value));
					}catch(...)
					{
						ECHO_LOG_WARNING("Failed to convert " << value << " to " << typeid(T).name());
					}
					matchCount++;
					++it;
				}
			}

			/**
			 * Specialised version of ProcessRegExCapture<T>() for boolean where the capture can be either "yes", "1" or "true" rather than
			 * just "1" or "true".
			 * @see ProcessRegEx<t>()
			 */
			void ProcessRegExCaptureBool(const std::string& content, const std::string& expression, function<void(size_t, bool)> functionToCall);
		}
	}
}
#endif
