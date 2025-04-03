#ifndef _ECHORESOURCEDELEGATE_H_
#define _ECHORESOURCEDELEGATE_H_
#include <echo/Types.h>
#include <echo/Kernel/ScopedLock.h>
#include <boost/foreach.hpp>
#include <list>
#include <set>
#include <algorithm>

namespace Echo
{
	template < typename ResourceType >
	class ResourceListener;
	
	/**
	 * A resource delegate will keep a list of objects that are interested in notifications of resource related events.
	 * A delegate is used to notify listeners so resources don't have to maintain their own list.
	 * The standard usage is as follows:
	 * 
	 *		Resource <-> ResourceDelegate <-> Listener
	 * 
	 * Resources maintain a pointer a resource delegate which are notified when the resource is destroyed. Delegate
	 * objects are typically created and assigned by resource managers and are usually the same for resources that are
	 * managed by the same manager. But because resources don't necessarily need to be managed by a manager, it may not
	 * necessarily have a delegate assigned. This means you should check that a resource's delegate is valid before using
	 * it.
	 * 
	 * If a resource's delegate is not valid you can create one and assign it yourself. For example:
	 * 
	 * ResourceDelegate<ResourceType> delegate = someResource->GetResourceDelegate();
	 * if(!delegate)
	 * {
	 *		delegate = ResourceDelegate<ResourceType>::Create();	// * see note below
	 *		someResource->SetResourceDelegate(delegate);
	 *		
	 * }
	 * //Use delegate. Typically:
	 * delegate->AddListener(listenerObject);
	 * 
	 * * This example could result in a lot of unnecessary delegates.
	 * 
	 * -Delegate sharing-
	 * ResourceDelegates essentially just pass on notification events to registered listeners. This means that the same
	 * delegate can be used for all resources of the same type. If you have special requirements you may decide to
	 * create delegates for specific groups of listeners instead.
	 */
	template< typename ResourceType >
	class ResourceDelegate
	{
	public:
		static shared_ptr<ResourceDelegate> Create()
		{
			return shared_ptr<ResourceDelegate>(new ResourceDelegate<ResourceType>());
		}
		
		/**
		 * The implementation is defined after the listener class since it uses listener.
		 */
		virtual ~ResourceDelegate();

		/**
		 * Add a resource listener to receive notifications.
		 * @note It is very important that you remove the listener when the listener no longer wants notifications. Listeners
		 * will automatically clean up with any delegates they have been added to.
		 * @param listener The listener, must not be null.
		 */
		void AddListener(ResourceListener<ResourceType>* listener);

		/**
		 * Remove a listener.
		 */
		void RemoveListener(ResourceListener<ResourceType>* listener);
		
		void NotifyOfDestruction(ResourceType* resource);
		void NotifyOfLoad(ResourceType* resource);
		void NotifyOfReload(ResourceType* resource);
		void NotifyOfUnload(ResourceType* resource);
		bool NotifyResourceMemoryReleaseRequest(ResourceType* resource);
		void NotifyResourceVersionChanged(ResourceType* resource);
	private:
		ResourceDelegate(){}
		Mutex mListenerMutex;
		std::list< ResourceListener<ResourceType>* > mListeners;
	};

	/**
	 * Resource listeners are registered with delegates when they need to know when a resource is destroyed.
	 * This listener base class will automatically remove the listener instance from any delegates it is registered with.
	 */
	template < typename ResourceType >
	class ResourceListener
	{
	public:
		ResourceListener(){}

		virtual ~ResourceListener()
		{
			//Move so when RemoveListener is called it it is essentially a no-op and we don't invalidate the iterator.
			mDelegatesMutex.Lock();
			std::set< ResourceDelegate<ResourceType>* > delegates = std::move(mDelegates);
			mDelegatesMutex.Unlock();
			for(ResourceDelegate<ResourceType>* delegate : delegates)
			{
				delegate->RemoveListener(this);
			}
		}
		
		/**
		 * When a resource is destroyed it will notify the delegate which will notify any listeners.
		 * @param resource
		 */
		virtual void OnResourceDestroyed(ResourceType* resource){}

		/**
		 * When a resource is loaded it will notify the delegate which will notify any listeners.
		 * @note Typically this won't be availale for the first load since the resource probably won't exist until after
		 * it is loaded for listeners to register for. This will be called when Load() is called and succeeds after the
		 * resource has been in the unloaded state via a call to Unload().
		 * @param resource
		 */
		virtual void OnResourceLoaded(ResourceType* resource){}

		/**
		 * When a resource is reloaded it will notify the delegate which will notify any listeners.
		 * @note When reloading occurs this callback will be called after the _Unload()/_Load() process has completed.
		 * @note OnResourceUnloaded() method won't be called in a Reload.
		 * @param resource
		 */
		virtual void OnResourceReloaded(ResourceType* resource){}

		/**
		 * When a resource is unloaded it will notify the delegate which will notify any listeners.
		 * @param resource
		 */
		virtual void OnResourceUnloaded(ResourceType* resource){}
		
		/**
		 * When memory has been requested to be released resources will first check with their delegate that they can be
		 * unloaded before attempting to. If listener needs the resource to remain in memory because the resource's
		 * memory is directly used then it can return false from this method.
		 */
		virtual bool OnResourceMemoryReleaseRequest(ResourceType* resource)
		{
			return true;
		}

		/**
		 * When the version of a resource changes this method will be called.
		 */
		virtual void OnResourceVersionChanged(ResourceType* resource)
		{
		}
	private:
		friend class ResourceDelegate<ResourceType>;
		void AddDelegate(ResourceDelegate<ResourceType>* delegate)
		{
			ScopedLock lock(mDelegatesMutex);
			mDelegates.insert(delegate);
		}
		void RemoveDelegate(ResourceDelegate<ResourceType>* delegate)
		{
			ScopedLock lock(mDelegatesMutex);
			mDelegates.erase(delegate);
		}
		Mutex mDelegatesMutex;
		std::set< ResourceDelegate<ResourceType>* > mDelegates;
	};

	//Definitions for ResourceDelegate

	template< typename ResourceType >
	void ResourceDelegate<ResourceType>::AddListener(ResourceListener<ResourceType>* listener)
	{
		assert(listener && "listener should not be null");
		if(listener)
		{
			ScopedLock lock(mListenerMutex);
			//Only add once.
			if(std::find(mListeners.begin(),mListeners.end(),listener)==mListeners.end())
			{
				listener->AddDelegate(this);
				mListeners.push_back(listener);
			}
		}
	}

	template< typename ResourceType >
	void ResourceDelegate<ResourceType>::RemoveListener(ResourceListener<ResourceType>* listener)
	{
		assert(listener && "listener should not be null");
		if(listener)
		{
			ScopedLock lock(mListenerMutex);
			listener->RemoveDelegate(this);
			mListeners.remove(listener);
		}
	}
	
	template< typename ResourceType >
	void ResourceDelegate<ResourceType>::NotifyOfDestruction(ResourceType* resource)
	{
		mListenerMutex.Lock();
		// Copy the list because the listeners may be modified in the callback.
		std::list<ResourceListener<ResourceType>*> listeners = mListeners;
		mListenerMutex.Unlock();
		for(ResourceListener<ResourceType>* listener : listeners)
		{
			listener->OnResourceDestroyed(resource);
		}
	}

	template< typename ResourceType >
	void ResourceDelegate<ResourceType>::NotifyOfLoad(ResourceType* resource)
	{
		mListenerMutex.Lock();
		// Copy the list because the listeners may be modified in the callback.
		std::list<ResourceListener<ResourceType>*> listeners = mListeners;
		mListenerMutex.Unlock();
		for(ResourceListener<ResourceType>* listener : listeners)
		{
			listener->OnResourceLoaded(resource);
		}
	}

	template< typename ResourceType >
	void ResourceDelegate<ResourceType>::NotifyOfReload(ResourceType* resource)
	{
		mListenerMutex.Lock();
		// Copy the list because the listeners may be modified in the callback.
		std::list<ResourceListener<ResourceType>*> listeners = mListeners;
		mListenerMutex.Unlock();
		for(ResourceListener<ResourceType>* listener : listeners)
		{
			listener->OnResourceReloaded(resource);
		}
	}

	template< typename ResourceType >
	void ResourceDelegate<ResourceType>::NotifyOfUnload(ResourceType* resource)
	{
		mListenerMutex.Lock();
		// Copy the list because the listeners may be modified in the callback.
		std::list<ResourceListener<ResourceType>*> listeners = mListeners;
		mListenerMutex.Unlock();
		for(ResourceListener<ResourceType>* listener : listeners)
		{
			listener->OnResourceUnloaded(resource);
		}
	}

	template< typename ResourceType >
	bool ResourceDelegate<ResourceType>::NotifyResourceMemoryReleaseRequest(ResourceType* resource)
	{
		bool unload=true;
		mListenerMutex.Lock();
		// Copy the list because the listeners may be modified in the callback.
		std::list<ResourceListener<ResourceType>*> listeners = mListeners;
		mListenerMutex.Unlock();
		for(ResourceListener<ResourceType>* listener : listeners)
		{
			if(!listener->OnResourceMemoryReleaseRequest(resource))
			{
				unload = false;
			}
		}
		return unload;
	}

	template< typename ResourceType >
	void ResourceDelegate<ResourceType>::NotifyResourceVersionChanged(ResourceType* resource)
	{
		mListenerMutex.Lock();
		// Copy the list because the listeners may be modified in the callback.
		std::list<ResourceListener<ResourceType>*> listeners = mListeners;
		mListenerMutex.Unlock();
		for(ResourceListener<ResourceType>* listener : listeners)
		{
			listener->OnResourceVersionChanged(resource);
		}
	}

	template< typename ResourceType >
	ResourceDelegate<ResourceType>::~ResourceDelegate()
	{
		ScopedLock lock(mListenerMutex);
		for(ResourceListener<ResourceType>* listener : mListeners)
		{
			listener->RemoveDelegate(this);
		}
	}
}
#endif
