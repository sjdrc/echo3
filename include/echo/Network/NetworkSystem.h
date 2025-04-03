#ifndef _NETWORKSYSTEM_H_
#define _NETWORKSYSTEM_H_
#include <vector>
#include <echo/Network/Connection.h>

namespace Echo
{
	class Connection;
	class IncomingConnectionListener;

	//NetworkSystem
	//
	// An NetworkSystem implements the management of network connections
	// for a specific networking type. For example Berkley sockets,
	// BlueTooth or perhaps a serial interface of some sort.
	//
	// The implementation must provide the system specific implementation
	// of Connection and update the connection when appropriate.
	// 
	// The implementation must be asynchronous, that is they must not
	// block at all.
	//

	class NetworkSystem
	{
	private:
		std::string mName;
		NetworkManager& mNetworkManager;
	public:

		NetworkSystem(const std::string& name, NetworkManager& network) : mName(name), mNetworkManager(network) {}
		virtual ~NetworkSystem(){}

		const std::string& GetName() const {return mName;}

		NetworkManager& GetNetworkManager() {return mNetworkManager;}

		//Initialise the Network System. Perform initialisation/allocations here
		virtual bool Initialise()=0;

		//Start is called when Network begins execution. This is a good
		//point to execute any threads.
		virtual bool Start()=0;

		//Stop is called when Network ends execution.
		//The system should clean up, disconnect any connections and terminate any threads.
		virtual void CleanUp()=0;

		virtual void DisconnectAll()=0;

		//This method should fill outSupportedConnectionTypes with any connection types this system supports.
		//Common connection types include, Passive and Direct
		virtual void GetSupportedConnectionTypes( std::vector< std::string >& outSupportedConnectionTypes )=0;

		//This method should fill outAdditionalInformation with any additional information required or optional
		//for each of the supported connection types. Each entry in this vector should correlate with each
		//corresponding element in the vector in GetSupportedConnectionTypes().
		//Each entry should take the format
		//
		//	requiredParameter[:optionalParameter]
		//
		//For final string format see NetworkManager::Connect()
		virtual void GetAdditionalConnectionInformationFormat( std::vector< std::string >& outAdditionalInformation )=0;

		//Perform an Asynchronous Listen. IMPORTANT: This function should not block.
		virtual bool Listen( IncomingConnectionListener* listener, const ConnectionDetails& connectionDetails )=0;

		//Perform an Asynchronous Connect. IMPORTANT: This function should not block.
		virtual shared_ptr<Connection> Connect( const ConnectionDetails& connectionDetails, Connection::ConnectCallback connectionCallback, Connection::DisconnectCallback disconnectCallback)=0;

		void UpdateIncoming(shared_ptr<Connection> connection, IncomingConnectionListener* listener);
		void UpdateConnect(shared_ptr<Connection> connection);
		void UpdateReceive(shared_ptr<Connection> connection);
		void UpdateSend(shared_ptr<Connection> connection);
	};
}
#endif
