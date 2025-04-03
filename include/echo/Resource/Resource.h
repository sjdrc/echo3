#ifndef _ECHORESOURCE_H_
#define _ECHORESOURCE_H_

#include <echo/Resource/ResourceDelegate.h>
#include <echo/Resource/ResourceLoader.h>
#include <echo/Kernel/ScopedLock.h>

namespace Echo
{
	/**
	 * Resource base class.
	 * Provides basic facilities for getting and setting the delegate object which will notify registered listeners of
	 * destruction.
	 */
	template< typename T >
	class Resource
	{
	public:
		Resource(bool loaded) : mResourceLoader(0), mVersion(0), mLoaded(loaded)
		{
		}
		
		virtual ~Resource()
		{
			ScopedLock lock(mDelegateMutex);
			if(mDelegate)
			{
				mDelegate->NotifyOfDestruction(static_cast<T*>(this));
			}
		}
		
		/**
		 * Get the resource delegate.
		 * @note This method intentionally does not create a delegate by default if the delegate is null since it would
		 * result in a lot of unnecessary delegate objects. @see ResourceDelegate for more information on delegate sharing.
		 */
		shared_ptr< ResourceDelegate<T> > GetResourceDelegate()
		{
			return mDelegate;
		}

		/**
		 * Get or create a resource delegate.
		 */
		shared_ptr< ResourceDelegate<T> > GetOrCreateResourceDelegate()
		{
			ScopedLock lock(mDelegateMutex);
			if(!mDelegate)
			{
				mDelegate = ResourceDelegate<T>::Create();
			}
			return mDelegate;
		}

		/**
		 * Get the resource delegate, assign if the delegate has not been set.
		 * @param existingDelegate an existing delegate to assign to this Resource if it has not been assigned one.
		 * @return The resource delegate, or the passed in value if the resource delegate was not set.
		 */
		shared_ptr< ResourceDelegate<T> > GetResourceDelegate(shared_ptr< ResourceDelegate<T> > existingDelegate)
		{
			ScopedLock lock(mDelegateMutex);
			if(!mDelegate)
			{
				mDelegate = existingDelegate;
			}
			return mDelegate;
		}
		
		/**
		 * Set the resource delegate.
		 */
		void SetResourceDelegate(shared_ptr< ResourceDelegate<T> > delegate)
		{
			mDelegate = delegate;
		}
				
		/**
		 * When a resource reloads or changes the version will be incremented.
		 * This is used to determine whether or not the current resource and contents matches any other resources
		 * that may have been allocated to use the data. For example RenderTargets might allocate a custom texture
		 * object for each texture as they are used and use the version numbers to determine if the custom texture
		 * is out of date.
		 * @return the current version number of the resource.
		 */
		Size GetVersion() const
		{
			return mVersion;
		}
		
		/**
		 * Force a version change.
		 */
		inline void IncrementVersion()
		{
			mVersion++;
			ScopedLock lock(mDelegateMutex);
			if(GetResourceDelegate())
			{
				GetResourceDelegate()->NotifyResourceVersionChanged(static_cast<T*>(this));
			}
		}


		void SetName(const std::string& name)
		{
			mName = name;
		}

		inline const std::string& GetName() const
		{
			return mName;
		}
		
		/**
		 * Resources sometimes use unnecessary memory, especially when the resource data is copied for a platform
		 * specific implementation. A resource manager will call this method if it has been asked to free some
		 * memory.
		 * 
		 * This method first checks with the delegate that the memory can safely be released first. If the the response
		 * from all of the resource listeners registered with the delegate is positive then the method will proceed to
		 * request a release from the implementation. The implementation may have other criteria for not releasing
		 * memory such as not being able to reload the data.
		 * @return the amount of memory that was released.
		 */
		Size RequestMemoryRelease()
		{
			if(mName.empty())
			{
				return 0;
			}

			{
				ScopedLock lock(mDelegateMutex);
				if(GetResourceDelegate())
				{
					if(!GetResourceDelegate()->NotifyResourceMemoryReleaseRequest(static_cast<T*>(this)))
					{
						return 0;
					}
				}
			}
			
			return OnRequestMemoryRelease();
		}
		
		/**
		 * Attempt to load the resource.
		 * @return return true if the load was successful. If the resource was already loaded the method will return false.
		 */
		bool Load()
		{
			if(mLoaded)
			{
				return false;
			}
			mLoaded = _Load();
			IncrementVersion();
			if(GetResourceDelegate())
			{
				GetResourceDelegate()->NotifyOfLoad(static_cast<T*>(this));
			}
			return mLoaded;
		}
		
		/**
		 * Unloads then attempts to load the resource.
		 */
		bool Reload()
		{
			if(mLoaded && !mName.empty())
			{
				bool unloadSuccess = _Unload();
				if(unloadSuccess)
				{
					mLoaded = false;	// The resource will be un the unloaded state during load
					mLoaded = _Load();
					IncrementVersion();	// Increment the version even if there is a failure. This will allow error resources to be loaded in place.
					if(GetResourceDelegate())
					{
						GetResourceDelegate()->NotifyOfReload(static_cast<T*>(this));
					}
					return mLoaded;
				}
			}
			return false;
		}

		/**
		 * Unload the resource.
		 * Unloading only occurs if the resource can be loaded again.
		 */
		bool Unload()
		{
			if(mLoaded && !mName.empty())
			{
				bool unloadSuccess = _Unload();
				if(unloadSuccess)
				{
					ScopedLock lock(mDelegateMutex);
					if(GetResourceDelegate())
					{
						GetResourceDelegate()->NotifyOfUnload(static_cast<T*>(this));
					}
					mLoaded = false;
					return true;
				}
			}
			return false;
		}
		
		/**
		 * Find out if the resource is loaded or not.
		 */
		inline bool IsLoaded() const {return mLoaded;}
		void SetResourceLoader(ResourceLoader<T>* resourceLoader){mResourceLoader=resourceLoader;}
		ResourceLoader<T>* GetResourceLoader() const {return mResourceLoader;}
	private:
		shared_ptr< ResourceDelegate<T> > mDelegate;
		ResourceLoader<T>* mResourceLoader;

		Size mVersion;
		std::string mName;
		bool mLoaded;
		Mutex mDelegateMutex;
		
		/**
		 * Attempt to load the resource.
		 */
		virtual bool _Load()
		{
			if(mResourceLoader)
			{
				return mResourceLoader->LoadIntoResource(GetName(),static_cast<T&>(*this));
			}
			return false;
		}
		/**
		 * Attempt to unload the.
		 * This method should return false if the resource cannot be loaded again by a call to Load().
		 */
		virtual bool _Unload() = 0;
	protected:
		/**
		 * The implementation should free memory in an override of this method if possible.
		 * RequestMemoryRelease() will first check that there is a resource name which should be used when attempting to
		 * reload then check with the delegate to find out if any listeners still need the resource data. If neither of
		 * these are true then this method is not called..
		 * @note It is important that you do not free memory if the implementation will not be able to reload the
		 * resource data on the next Load() call.
		 */
		virtual Size OnRequestMemoryRelease() = 0;
	};
}
#endif 
