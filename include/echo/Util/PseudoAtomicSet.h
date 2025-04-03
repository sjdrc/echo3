#ifndef ECHO_PSEUDOATOMICSET_H
#define ECHO_PSEUDOATOMICSET_H

#include <echo/Kernel/ScopedLock.h>
#include <set>
#include <vector>
#include <atomic>

namespace Echo
{
	/**
	 * PseudoAtomicSet is designed to provide a level of thread safety for insert and erasure of elements in a set
	 * without locking a mutex every time the set is accessed. This is accomplished by delayed insert or removal to
	 * when the container is to be used. It is useful if you're going to be adding or removing to the set while
	 * iterating over it but don't want the processing to block the add or erase.
	 * The one strict requirement of this class is that access to the set via GetSet() is from one thread or
	 * protected by an external mutex since the GetSet() call will perform management of the container before
	 * returning.
	 */
	template< typename _ValueType >
	class PseudoAtomicSet
	{
	public:
		typedef _ValueType ValueType;
		typedef std::set<ValueType> SetType;
		
		PseudoAtomicSet() : mManagementTime(false), mClear(false)
		{
		}
		
		/**
		 * Get the set
		 * @note IMPORTANT: This should only ever be called from the one thread or access to this object is protected.
		 * @return a reference to the set.
		 */
		inline SetType& GetSet()
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
							mContainer.erase(modification.mValue);
						break;
						case Modifications::INSERT:
							mContainer.emplace(modification.mValue);
						break;
					}
				}
				mItemsToModify.resize(0);
				mManagementTime = false;
			}
			return mContainer;
		}
		/**
		 * Get SetCopy can be called from any thread to obtain a copy of the container.
		 * This won't perform the management of the container though, just copy the work that needs to be done.
		 */
		inline SetType GetSetCopy() const
		{
			ScopedLock locky(mMutex);
			SetType setCopy = mContainer;
			if(mClear)
			{
				setCopy.clear();
			}

			for(const ItemModification& modification : mItemsToModify)
			{
				switch(modification.mModification)
				{
					case Modifications::ERASE:
						setCopy.erase(modification.mValue);
					break;
					case Modifications::INSERT:
						setCopy.emplace(modification.mValue);
					break;
				}
			}
			return setCopy;
		}
		/**
		 * In stead of set::insert()
		 */
		inline void Insert(ValueType value)
		{
			ScopedLock locky(mMutex);
			mItemsToModify.emplace_back(ItemModification{Modifications::INSERT,value});
			mManagementTime = true;
		}

		/**
		 * Instead of erase than insert
		 */
		inline void Set(ValueType value)
		{
			ScopedLock locky(mMutex);
			mItemsToModify.emplace_back(ItemModification{Modifications::ERASE,value});
			mItemsToModify.emplace_back(ItemModification{Modifications::INSERT,value});
			mManagementTime = true;
		}

		/**
		 * Instead of set::erase()
		 */
		void Erase(ValueType value)
		{
			ScopedLock locky(mMutex);
			mItemsToModify.emplace_back(ItemModification{Modifications::ERASE,value});
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
				INSERT
			};
		};
		typedef typename Modifications::_ Modification;

		struct ItemModification
		{
			Modification mModification;
			ValueType mValue;
		};

		std::set<ValueType> mContainer;
		std::vector< ItemModification > mItemsToModify;
		mutable Mutex mMutex;
		std::atomic<bool> mManagementTime;
		bool mClear;
	};
}

#endif

