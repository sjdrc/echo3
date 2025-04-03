#ifndef _GENERAL_ERROR_H_
#define _GENERAL_ERROR_H_

#include <echo/Types.h>
namespace Echo
{
	class GeneralError
	{
	protected:
		std::string mOutput;
	public:
		GeneralError(){}
		GeneralError(std::string output){}
		virtual ~GeneralError(){}
		virtual void OutputError(){ ECHO_LOG_ERROR(mOutput);}
	};
}
#endif
