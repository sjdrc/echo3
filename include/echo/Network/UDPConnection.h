#ifndef _UDPCONNECTION_H_
#define _UDPCONNECTION_H_
#include <echo/Network/Connection.h>
#include <echo/Network/NetRedefinitions.h>

namespace Echo
{
	class SocketNetworkSystem;

	class UDPConnection : public Connection
	{
	protected:
		friend class SocketNetworkSystem;
		SocketAddressIn mFromSockname;
		SocketAddressIn mToSockName;
		Socket mSocket;
		SocketNetworkSystem& mManager;
		void SetSocket(Socket sock);
		void MakeToMatchFrom();

		//You don't technically connect a UDP port, you just open it.
		bool _Connect() override;
		bool _Disconnect() override;
	public:
		UDPConnection(	SocketNetworkSystem& manager );
		~UDPConnection();

		////////////////////////
		//GetLastSender
		//	Should only be used in a EUDPConnectionOwner::ReceivedUDPPacket() callback
		//	upon return the outFromSockName will contain the contents of fromSockname
		//	which after a recv from contains the from data. This should be copied if
		//	it is to be preserved as fromSockname is used in the next recvfrom call
		void GetLastSender(SocketAddressIn *outFromSockName);
		u16 GetLastPort() const;
		std::string GetLastSender();

		SendResult Send(const u8 * buffer, int numberOfBytesToSend) override;
		ReceiveResult Receive(u8* buffer, int bufferSizeInBytes) override;
		virtual bool _HandleError(int code);

		void EnableBroadcast();
		void DisableBroadcast();
		void SetTo(u32 address);
		void SetTo(std::string address);
		void SetPort_(u16 port);			//Crap name because Windows API is just crap
		void SetAddressFamily(u16 addressFamily);

		void SendDataPacket(shared_ptr<DataPacket> packet, u32 address=0);
		void SendDataPacket(shared_ptr<DataPacket> packet, std::string address);
		void SendData(u8* data, u32 dataSize, u32 packetType, std::string address);
		void SendData(u8* data, u32 dataSize, u32 packetType, u32 address=0);
	};
}
#endif	//_EUDPCONNECTION_H_
