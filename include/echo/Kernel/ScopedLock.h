#ifndef _ECHO_SCOPEDLOCK_H_
#define _ECHO_SCOPEDLOCK_H_
#include <echo/Kernel/Mutex.h>

namespace Echo
{
	/**
	 * Really simple ScopedLock.
	 */
	class ScopedLock
	{
	public:
		ScopedLock(Mutex& m) : mMutex(m)
		{
			mMutex.Lock();
		}
		~ScopedLock()
		{
			mMutex.Unlock();
		}
	private:
		Mutex& mMutex;
	};
	
	
}
#endif
