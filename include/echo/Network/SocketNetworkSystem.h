#ifndef _SOCKETNETWORKSYSTEM_H_
#define _SOCKETNETWORKSYSTEM_H_
#include <echo/Network/NetRedefinitions.h>
#include <echo/Network/NetworkManager.h>
#include <echo/Network/NetworkSystem.h>
#include <echo/Kernel/TaskThread.h>
#include <echo/Kernel/Mutex.h>
#include <list>
#include <map>

namespace Echo
{
	class Connection;
	class ConnectionDetails;
	class IncomingConnectionListener;
	struct SocketNetworkPlatformDetail;

	class SocketNetworkSystem : public NetworkSystem
	{
	protected:
		class SocketThreadTask;
		Mutex mConnectionsMutex;
		std::map< Socket, shared_ptr<Connection> > mConnections;
		std::map< Socket, IncomingConnectionListener* > mIncomingConnectionListeners;

		struct ThreadTaskPair
		{
			shared_ptr<TaskThread> mThread;
			shared_ptr<SocketThreadTask> mSocketThreadTask;
		};
		std::list< shared_ptr<ThreadTaskPair> > mSocketThreadTasks;
		std::list< shared_ptr<ThreadTaskPair> > mThreadTasksToCleanup;
		std::map< Socket, shared_ptr<ThreadTaskPair> > mSocketThreadTaskConnectionLookup;
		bool mStarted;
		Size mMaximumSocketsPerThread;
		int mPollTimeInMS;
		Mutex mSocketThreadTasksMutex;

		// A few platforms require some extra initialisation and clean up. It is currently maintained within the implementation.
		// This will eventually need to be refactroed to a better design. That will likely occur when the networking internals
		// are refactored.
		// SocketNetworkSystem will call SocketNetworkPlatformDetail::Create() to create a shared platform state.
		shared_ptr<SocketNetworkPlatformDetail> mPlatformDetail;

		//Read set for listen and read data
		//Write set for connect and write data
		//except for connect failed

		//Each SocketThreadTask can monitor up to FD_SETSIZE
		class SocketThreadTask : public Task
		{
		public:
			SocketThreadTask(SocketNetworkSystem& s, Size maximumSockets, int pollTimeInMS);
			~SocketThreadTask();

			bool AddSocket(Socket s);
			bool RemoveSocket(Socket s);

			void EnableWriteCheck(Socket s);
			void DisableWriteCheck(Socket s);

			void Update(Seconds lastFrameTime);
			Size GetSocketCount() const
			{
				return mCount;
			}
			bool HasCapacity() const
			{
				return (mCount<mMaximumSockets);
			}
		private:
			Mutex mPendingSocketsMutex;
			std::set< Socket > mPendingAddSockets;
			std::set< Socket > mPendingRemovalSockets;
			std::set< Socket > mPendingWriteDisableSockets;
			std::set< Socket > mPendingWriteEnableSockets;
			std::vector< pollfd > mPollFDs;
			std::map< Socket, Size > mSocketToPollFDMap;
			// These internal methods should only be called when the pending mutex is locked
			void InternalAddSocket(Socket s);
			void InternalRemoveSocket(Socket s);
			void InternalWriteDisable(Socket s);
			void InternalWriteEnable(Socket s);
			std::atomic<bool> mPollUpdateRequired;
			Size mCount;
			Size mMaximumSockets;
			int mPollTimeInMS;
			SocketNetworkSystem& mSystem;
		};

	private:
		friend class SocketThreadTask;
		void CheckForTimeout(Socket s);
		void ReadNotify(Socket s);
		void WriteNotify(Socket s);
		void ExceptNotify(Socket s);
		bool IsSocketConnected(Socket s);
		bool AssignThreadTask(Socket s);

		/**
		 * Mutex must be locked
		 * @return a ThreadTaskPair if one has capacity, otherwise false.
		 */
		shared_ptr<ThreadTaskPair> FindAvailableSocketThreadTask();
		
		/**
		 * Attempts to accept a connection.
		 * @returns false if the socket is not a listening socket, otherwise true.
		 */
		bool AcceptConnection(Socket s);

		void OnThreadTaskComplete(shared_ptr<ThreadTaskPair> taskPair);
	public:

		struct InterfaceInformation
		{
			std::string mHostname;
			std::string mIPAddress;
			std::string mSubnetMask;
			std::string mGateway;
			std::string mDNS1;
			std::string mDNS2;
			std::string mMacAddress;
		};

		SocketNetworkSystem(NetworkManager& networkManager, Size maximumSocketsPerThread = 0, int pollTimeInMS = 2000);
		~SocketNetworkSystem();

		//Initialise the Network System. Perform initialisation/allocations here
		bool Initialise();

		//Start is called when ENetwork begins execution. This is a good
		//point to execute any threads.
		bool Start();

		void CleanUp();

		void DisconnectAll();

		//This method should fill outSupportedConnectionTypes with any connection types this system supports.
		//Common connection types include, Passive and Direct
		void GetSupportedConnectionTypes( std::vector< std::string >& outSupportedConnectionTypes );

		//This method should fill outAdditionalInformation with any additional information required or optional
		//for each of the supported connection types. Each entry in this vector should correlate with each
		//corresponding element in the vector in GetSupportedConnectionTypes().
		void GetAdditionalConnectionInformationFormat( std::vector< std::string >& outAdditionalInformation );

		static bool GetInterfaceInformation(std::vector<InterfaceInformation>& outInterfaceInformation);

		//Perform an Asynchronous Listen. IMPORTANT: This function should not block
		bool Listen( IncomingConnectionListener* listener, const ConnectionDetails& connectionDetails );

		//Perform an Asynchronous Connect. IMPORTANT: This function should not block
		shared_ptr<Connection> Connect( const ConnectionDetails& connectionDetails, Connection::ConnectCallback connectionCallback, Connection::DisconnectCallback disconnectCallback);

		bool UpdateSocket(Socket s, shared_ptr<Connection> connection);

		void EnableSocketWriteCheck(Socket s);
		void DisableSocketWriteCheck(Socket s);

		//Returns true on success, or false if there was an error
		bool SetSocketBlockingEnabled(int fd, bool blocking);
		void CleanSocket(Socket s);
		/**
		 * Attempts to get the host IP by name or string IP.
		 * @param hostNameOrIP The hostname or IP address as a string, e.g. www.emblem.net.au or 192.168.1.1.
		 * @param output The IPv4 address will be written to this value upon success. The value is not modified
		 * if there is an error.
		 * @return false if the DNS lookup failed.
		 */
		static bool GetHostByName(const std::string& hostnameOrIP, u32& output);

		static bool HandleError(int code);
	};
}
#endif
