#include <echo/Kernel/Mutex.h>
#include <ogc/mutex.h>
#include <iostream>

namespace Echo
{
	class Mutex::Implementation
	{
	public:	
		mutex_t mMutex;
		Implementation()
		{
			LWP_MutexInit(&mMutex,false);
		}
		~Implementation()
		{
			LWP_MutexDestroy(mMutex);
		}
	};
	
	Mutex::Mutex()
	{
		mImplementation.reset(new Implementation());
		
	}
	
	Mutex::~Mutex()
	{
	}

	bool Mutex::Lock()
	{
		s32 r=LWP_MutexLock(mImplementation->mMutex);
		if(r<0)
		{
			ECHO_LOG_ERROR("Failed to lock mutex: " << r);
			return false;
		}
		return true;
	}
	
	bool Mutex::AttemptLock()
	{
		if(LWP_MutexTryLock(mImplementation->mMutex)==0)
		{
			return Lock();
		}
		return false;
	}
	
	bool Mutex::Unlock()
	{
		s32 r=LWP_MutexUnlock(mImplementation->mMutex);
		if(r<0)
		{
			ECHO_LOG_ERROR("Failed to unlock mutex: " << r);
			return false;
		}
		return true;
	}
}
