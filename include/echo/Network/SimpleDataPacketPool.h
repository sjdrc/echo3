#ifndef ECHO_SIMPLEDATAPACKETPOOL_H
#define ECHO_SIMPLEDATAPACKETPOOL_H

#include <echo/Network/DataPacketFactory.h>
#include <echo/Kernel/Mutex.h>
#include <vector>

namespace Echo
{
	/**
	 * A simple DataPacket pool.
	 * The pool will allocate new packets as necessary.
	 * Each packet will be setup with a destructor to release back into the pool.
	 */
	class SimpleDataPacketPool : public DataPacketFactory
	{
	public:
		SimpleDataPacketPool(Size initialSize, Size initialPacketBufferSize);
		virtual ~SimpleDataPacketPool();
		virtual shared_ptr<DataPacket> NewDataPacket() override;
		virtual Size GetPreallocationPoolSize() const override;
	private:
		mutable Mutex mMutex;
		std::vector< DataPacket* > mAvailable;
		
		// Number of packets this pool has created (not the number of times NewDataPacket() has been called)
		Size mNumberOfPacketsAllocated;
		Size mInitialDataPacketBufferSize;

		DataPacket* CreatePacket(Size initialPacketBufferSize);
	};
}

#endif

