#ifndef _INCOMINGCONNECTIONLISTENER_H_
#define _INCOMINGCONNECTIONLISTENER_H_
namespace Echo
{
	class Connection;

	/**
	 * IncomingConnectionListener.
	 * When the network manager detects an incoming connection on port onPort it will establish
	 * a connection then call IncomingConnection(). The Listener can then decide what to do with
	 * the connection.
	 */
	class IncomingConnectionListener
	{
	public:
		IncomingConnectionListener(){}
		virtual ~IncomingConnectionListener(){}
		virtual void IncomingConnection(shared_ptr<Connection> connection)=0;
	};
}
#endif
