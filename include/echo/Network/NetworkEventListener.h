#ifndef _NETWORKEVENTLISTENER_H_
#define _NETWORKEVENTLISTENER_H_
namespace Echo
{
	class Connection;

	/**
	 * Added to a NetworkManager to respond to network events.
	 * When a network event occurs Network will call OnNetworkEvent().
	 */
	class NetworkEventListener
	{
	public:
		struct NetworkEventTypes
		{
			enum _
			{
				UNKNOWN = 0,
				INCOMING_CONNECTION = 1,
				PACKET_RECEIVED = 2,
				DISCONNECTED = 4,
				ESTABLISHED = 8			
			};
		};
		typedef NetworkEventTypes::_ NetworkEventType;

		NetworkEventListener(){}
		virtual ~NetworkEventListener(){}

		//The call of OnNetworkEvent() may occur on any thread.
		virtual void OnNetworkEvent(NetworkEventType eventType) = 0;
	};
}
#endif
