#include <echo/Network/SimpleDataPacketPool.h>
#include <echo/Network/DataPacket.h>
#include <echo/Kernel/ScopedLock.h>

namespace Echo
{
	SimpleDataPacketPool::SimpleDataPacketPool(Size initialSize, Size initialPacketBufferSize) :
		mNumberOfPacketsAllocated(0),
		mInitialDataPacketBufferSize(initialPacketBufferSize)
	{
		if(initialSize>0)
		{
			// This could be a little more efficient if we just used an array rather than a vector,
			// or allocated into the array. But this is just a bit safer since it is easier to
			// process.
			mAvailable.reserve(initialSize);
			for(Size i=0; i < initialSize; ++i)
			{
				mAvailable.push_back(CreatePacket(mInitialDataPacketBufferSize));
			}
		}
	}
	
	SimpleDataPacketPool::~SimpleDataPacketPool()
	{
		ScopedLock locky(mMutex);
		ECHO_ASSERT(mAvailable.size()==mNumberOfPacketsAllocated, "The number of available packets in the pool is not equal to the number of packets allocated. The pool needs to live longer than the packets.");
		if(mAvailable.size()!=mNumberOfPacketsAllocated)
		{
			ECHO_LOG_ERROR("The number of available packets in the pool is not equal to the number of packets allocated. The pool needs to live longer than the packets.");
		}
		for(auto packet : mAvailable)
		{
			delete packet;
		}
	}
	
	shared_ptr<DataPacket> SimpleDataPacketPool::NewDataPacket()
	{
		ScopedLock locky(mMutex);
		if(mAvailable.empty())
		{
			// Allocate a new one
			mAvailable.push_back(CreatePacket(mInitialDataPacketBufferSize));
		}

		DataPacket* dataPacket = mAvailable.back();
		mAvailable.resize(mAvailable.size()-1);
		
		return shared_ptr<DataPacket>(dataPacket, [this](DataPacket* packet){
			ScopedLock locky(mMutex);
			mAvailable.push_back(packet);
		});
	}
	
	Size SimpleDataPacketPool::GetPreallocationPoolSize() const
	{
		return mNumberOfPacketsAllocated;
	}

	DataPacket* SimpleDataPacketPool::CreatePacket(Size initialPacketBufferSize)
	{
		DataPacket* dataPacket = new DataPacket();
		dataPacket->SetDataSize(initialPacketBufferSize,false);
		mNumberOfPacketsAllocated++;
		return dataPacket;
	}
}

