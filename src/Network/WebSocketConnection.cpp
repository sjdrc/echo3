#include <echo/Network/NetRedefinitions.h>
#include <echo/Network/WebSocketConnection.h>
#include <echo/Network/DataPacket.h>
#include <echo/Util/StringUtils.h>
#include <boost/lexical_cast.hpp>

namespace Echo
{
	WebSocketConnectionBase::WebSocketConnectionBase(WebSocketNetworkSystem& manager) : Connection(manager.GetNetworkManager()), mManager(manager)
	{
	}

	WebSocketConnectionBase::~WebSocketConnectionBase()
	{
	}

	Connection::ReceiveResult WebSocketConnectionBase::Receive(u8* buffer, int bufferSizeInBytes)
	{
		size_t requestedLength = bufferSizeInBytes;
		size_t available = std::min(mBufferedData.size(),requestedLength);
		if(available)
		{
			for(size_t i=0; i < available; ++i)
			{
				char front = mBufferedData.front();
				buffer[i] = *reinterpret_cast<u8*>(&front);
				mBufferedData.pop();
			}
		}
		// Read disconnect handling is done through the callbacks so we don't need to worry about checking for it here
		return ReceiveResult{available,ReceiveStatuses::SUCCESS};
	}
}
