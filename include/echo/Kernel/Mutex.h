#ifndef _ECHO_MUTEX_H_
#define _ECHO_MUTEX_H_
#include <echo/Types.h>

namespace Echo
{
	class Mutex
	{
	private:
		class Implementation;
		shared_ptr<Implementation> mImplementation;
	public:
		Mutex();
		~Mutex();

		bool Lock();
		bool AttemptLock();
		bool Unlock();
	};
	
	
}
#endif
