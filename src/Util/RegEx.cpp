
#include <echo/Util/RegEx.h>
namespace Echo
{
	namespace Utils
	{
		namespace RegEx
		{
			void ProcessRegExCaptureBool(const std::string& content, const std::string& expression, function<void(size_t, bool)> functionToCall)
			{
				if(!functionToCall)
				{
					return;
				}
				boost::regex regEx(expression);
				//Split up the line into function and parameters.
				boost::cregex_iterator it(&content[0], &content[content.length()-1], regEx);
				boost::cregex_iterator itEnd;
				size_t matchCount = 0;
				while(it!=itEnd)
				{
					// (*it)[0] contains the whole string 
					// (*it)[1] contains the value
					std::string value((*it)[1].first, (*it)[1].second);
					bool isTrue = (value=="yes") || (value=="1") || (value=="true");
					functionToCall(matchCount,isTrue);
					matchCount++;
					++it;
				}
			}
		}
	}
}
