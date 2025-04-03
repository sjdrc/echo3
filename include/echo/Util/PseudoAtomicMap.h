#ifndef ECHO_PSEUDOATOMICMAP_H
#define ECHO_PSEUDOATOMICMAP_H

#include <echo/Kernel/ScopedLock.h>
#include <map>
#include <vector>
#include <atomic>

namespace Echo
{
	/**
	 * PseudoAtomicMap is designed to provide a level of thread safety for insert and erasure of elements in a map
	 * without locking a mutex every time the map is accessed. This is accomplished by delayed insert or removal to
	 * when the container is to be used. It is useful if you're going to be adding or removing to the map while
	 * iterating over it but don't want the processing to block the add or erase.
	 * The one strict requirement of this class is that access to the map via GetMap() is from one thread or
	 * protected by an external mutex since the GetMap() call will perform management of the container before
	 * returning.
	 */
	template< class _KeyType, typename _ValueType >
	class PseudoAtomicMap
	{
	public:
		typedef _KeyType KeyType;
		typedef _ValueType ValueType;
		typedef std::map<KeyType,ValueType> MapType;
		
		PseudoAtomicMap() : mManagementTime(false), mClear(false)
		{
		}
		
		/**
		 * Get the map
		 * @note IMPORTANT: This should only ever be called from the one thread or access to this object is protected.
		 * @return a reference to the map.
		 */
		MapType& GetMap()
		{
			if(mManagementTime)
			{
				ScopedLock locky(mMutex);

				if(mClear)
				{
					mContainer.clear();
				}
				mClear = false;

				for(ItemModification& modification : mItemsToModify)
				{
					switch(modification.mModification)
					{
						case Modifications::ERASE:
							mContainer.erase(modification.mKey);
						break;
						case Modifications::INSERT:
							if(modification.mValue)
							{
								mContainer.emplace(std::pair<KeyType,ValueType>(modification.mKey,*modification.mValue));
							}else
							{
								ECHO_LOG_ERROR("Value is not set for insert action");
							}
						break;
						case Modifications::SET:
							if(modification.mValue)
							{
								mContainer[modification.mKey] = *modification.mValue;
							}else
							{
								ECHO_LOG_ERROR("Value is not set for set action");
							}
						break;
					}
				}
				mItemsToModify.resize(0);

				mManagementTime = false;
			}
			return mContainer;
		}

		/**
		 * In stead of map::insert()
		 */
		inline void Insert(KeyType key, ValueType value)
		{
			ScopedLock locky(mMutex);
			mItemsToModify.emplace_back(ItemModification{key,Modifications::INSERT,value});
			mManagementTime = true;
		}

		/**
		 * Instead of map::operator[]
		 */
		inline void Set(KeyType key, ValueType value)
		{
			ScopedLock locky(mMutex);
			mItemsToModify.emplace_back(ItemModification{key,Modifications::SET,value});
			mManagementTime = true;
		}

		/**
		 * Instead of map::erase()
		 */
		void Erase(KeyType key)
		{
			ScopedLock locky(mMutex);
			mItemsToModify.emplace_back(ItemModification{key,Modifications::ERASE,none});
			mManagementTime = true;
		}

		inline void Clear()
		{
			ScopedLock locky(mMutex);
			// Clear the work list first
			mItemsToModify.clear();
			mClear = true;
			mManagementTime = true;
		}
	private:
		struct Modifications
		{
			enum _
			{
				ERASE,
				INSERT,
				SET
			};
		};
		typedef typename Modifications::_ Modification;

		struct ItemModification
		{
			KeyType mKey;
			Modification mModification;
			optional<ValueType> mValue;
		};

		MapType mContainer;
		std::vector< ItemModification > mItemsToModify;
		Mutex mMutex;
		std::atomic<bool> mManagementTime;
		bool mClear;
	};
	
	/**
	 * Variant of PseudoAtomicMap whose value is a container of vectors
	 * @see PseudoAtomicMap
	 */
	template< class _KeyType, typename _ValueContainerType >
	class PseudoAtomicMappedContainer
	{
	public:
		typedef _KeyType KeyType;
		typedef _ValueContainerType ValueContainerType;
		typedef typename ValueContainerType::value_type ValueType;
		typedef std::map<KeyType, ValueContainerType > MapType;
		
		PseudoAtomicMappedContainer() : mManagementTime(false), mClear(false)
		{
		}
		
		/**
		 * Get the map
		 * @note IMPORTANT: This should only ever be called from the one thread or access to this object is protected.
		 * @return a reference to the map.
		 */
		inline MapType& GetMap()
		{
			if(mManagementTime)
			{
				ScopedLock locky(mMutex);

				if(mClear)
				{
					mContainer.clear();
				}
				mClear = false;

				for(ItemModification& modification : mItemsToModify)
				{
					switch(modification.mModification)
					{
						case Modifications::REMOVE_KEY:
							mContainer.erase(modification.mKey);
						break;
						case Modifications::PUSH_BACK:
							if(modification.mValue)
							{
								mContainer[modification.mKey].emplace_back(*modification.mValue);;
							}else
							{
								ECHO_LOG_ERROR("Value is not set for push back action");
							}
						break;
					}
				}
				mItemsToModify.resize(0);
				mManagementTime = false;
			}
			return mContainer;
		}
		inline void Add(KeyType key, ValueType value)
		{
			ScopedLock locky(mMutex);
			mItemsToModify.emplace_back(ItemModification{key,Modifications::PUSH_BACK,value});
			mManagementTime = true;
		}
		inline void Remove(KeyType key)
		{
			ScopedLock locky(mMutex);
			mItemsToModify.emplace_back(ItemModification{key,Modifications::REMOVE_KEY,none});
			mManagementTime = true;
		}
		inline void Clear()
		{
			ScopedLock locky(mMutex);
			// Clear the work list first
			mItemsToModify.clear();
			mClear = true;
			mManagementTime = true;
		}
	private:
		struct Modifications
		{
			enum _
			{
				REMOVE_KEY,
				PUSH_BACK
			};
		};
		typedef typename Modifications::_ Modification;

		struct ItemModification
		{
			KeyType mKey;
			Modification mModification;
			optional<ValueType> mValue;
		};

		MapType mContainer;
		std::vector< ItemModification > mItemsToModify;
		Mutex mMutex;
		std::atomic<bool> mManagementTime;
		bool mClear;
	};
}

#endif

