#ifndef ECHO_DATAPACKETFACTORY_H
#define ECHO_DATAPACKETFACTORY_H
#include <echo/Types.h>

namespace Echo
{
	class DataPacket;
	
	class DataPacketFactory
	{
	public:
		DataPacketFactory();
		virtual ~DataPacketFactory();
		virtual shared_ptr<DataPacket> NewDataPacket() = 0;
		virtual Size GetPreallocationPoolSize() const = 0;
	};
}

#endif

