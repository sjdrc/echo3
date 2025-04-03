#include <boost/thread/mutex.hpp>
#include <echo/Kernel/Mutex.h>

namespace Echo
{
	class Mutex::Implementation
	{
	public:
		boost::mutex mMutex;
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
		try
		{
			mImplementation->mMutex.lock();
		}
		catch(boost::lock_error& e)
		{
			ECHO_LOG_ERROR("Failed to lock mutex: " << e.what());
			return false;
		}
		return true;
	}
	
	bool Mutex::AttemptLock()
	{
		return mImplementation->mMutex.try_lock();
	}
	
	bool Mutex::Unlock()
	{
		try
		{
			mImplementation->mMutex.unlock();
		}
		catch(boost::lock_error& e)
		{
			ECHO_LOG_ERROR("Failed to unlock mutex: " << e.what());
			return false;
		}
		return true;
	}
}
