#ifndef _CONNECTIONOWNER_H_
#define _CONNECTIONOWNER_H_
#include <echo/Types.h>

namespace Echo
{
	class DataPacket;
	class DataPacketHeader;
	class Connection;

	class ConnectionOwner
	{
	public:
		ConnectionOwner(){}
		virtual ~ConnectionOwner(){}

		/**
		 * Called when a packet is received.
		 * You can copy the packet pointer if you wish to use the packet later. After this method returns the
		 * Connection will remove references to the packet, so if the packet pointer was copied it will be
		 * the only copy left.
		 * @param connection
		 * @param packet
		 */
		virtual void ReceivedPacket(shared_ptr<Connection> connection, shared_ptr<DataPacket> packet){}

		/**
		 * Called when a connection drops.
		 * After this function is called the connection is no longer tracked by the system.
		 * @param connection
		 */
		virtual void ConnectionDrop(shared_ptr<Connection> connection){}

		/**
		 * Called when a connection was successfully established.
		 * @param connection
		 */
		virtual void ConnectionEstablished(shared_ptr<Connection> connection){}
	};
}
#endif
