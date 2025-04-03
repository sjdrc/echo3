#ifndef _CONNECTION_H_
#define _CONNECTION_H_
#include <echo/Types.h>
#include <echo/Network/ConnectionDetails.h>
#include <list>
#include <vector>
#include <map>
#include <echo/Kernel/Mutex.h>
#include <echo/Chrono/CountDownTimer.h>
#include <echo/Util/PseudoAtomicMap.h>
#include <echo/Util/PseudoAtomicSet.h>
#include <echo/cpp/functional>
#include <boost/lexical_cast.hpp>

namespace Echo
{
	class DataPacket;
	class DataPacketHeader;
	class Connection;
	class ConnectionOwner;
	class NetworkManager;
	
	class Connection : public enable_shared_from_this<Connection>
	{
	public:
		struct States
		{
			enum _
			{
				DISCONNECTED,
				CONNECTING,
				CONNECTED
			};
		};
		typedef States::_ State;
		
		struct SendStatuses
		{
			enum _
			{
				WAIT,					// Writing failed, but might succeed next time. The connection will enter a wait
										// state until the network system signals that it can send again.
				SUCCESS,				// Send was successful.
				DISCONNECT,				// The connection has disconnected, this triggers internal house keeping to occur.
				DISCONNECT_REQUESTED,	// A packet was sent and it was to be the last packet to send before disconnecting,
										// this is requested when calling SendDataPacket*.
				INTERNAL_FAILURE,		// When the Connection determines that it cannot perform a send, in this situation
										// the network system might need to schedule a retry.
			};
		};
		typedef SendStatuses::_ SendStatus;
		
		struct ReceiveStatuses
		{
			enum _
			{
				WAIT,					// Receiving failed, but might succeed next time. The connection will enter a wait
										// state until the network system signals that it can receive again.
				SUCCESS,				// Receive was successful.
				DISCONNECT,				// The connection has disconnected, this triggers internal house keeping to occur.
				INTERNAL_FAILURE,		// When the Connection determines that it cannot perform a receive, in this situation
										// the network system might need to schedule a retry.
			};
		};
		typedef ReceiveStatuses::_ ReceiveStatus;
				
		struct SendResult
		{
			Size mBytesSent;
			SendStatus mStatus;
		};

		struct ReceiveResult
		{
			Size mBytesReceived;
			ReceiveStatus mStatus;
		};
		
		/**
		 * PacketCallback
		 * @param first parameter is the connection.
		 * @param second parameter is the incoming DataPacket.
		 */
		typedef function<void(shared_ptr<Connection>, shared_ptr<DataPacket>)> PacketCallback;

		/**
		 * LabelledPacketCallback
		 * @param first parameter is the connection.
		 * @param second parameter is the data packet that contained the labelled data.
		 * @param third is a pointer to the data in the data packet starting at the first byte after the label. The memory
		 * is owned by the DataPacket. This is read only and should not be assumed to exist after your callback returns
		 * unless you also make a copy of the DataPacket shared_ptr.
		 * @param fourth the length of the data in bytes from the (the DataPacket data size minus the label+meta info size).
		 */
		typedef function<void(shared_ptr<Connection>, shared_ptr<DataPacket>, const u8*, Size)> LabelledPacketCallback;

		/**
		 * DisconnectCallback
		 * @param first parameter is the connection.
		 */
		typedef function<void(shared_ptr<Connection>)> DisconnectCallback;

		/**
		 * ConnectCallback
		 * @param first parameter is the connection.
		 */
		typedef function<void(shared_ptr<Connection>)> ConnectCallback;

		/**
		 * BacklogCallback
		 * @param first parameter is the connection.
		 * @param second parameter is the number of bytes backlogged
		 * @param third parameter is the number of bytes that is set to trigger the callback
		 * @param fourth parameter is the current packet that is to be queued.
		 * @return The callback should return true to keep the packet or false to discard the packet.
		 */
		typedef function<bool(shared_ptr<Connection>, Size, Size, shared_ptr<DataPacket> packet)> BacklogCallback;

		//These packets are used internally for
		struct PacketTypes
		{
			enum Value
			{
				LABELLED_PACKET				=0xFFFFFFFF,
				REMOTE_DETAILS				=0xFEFEFEFE,			// Remote details packet, sent internally to determine
																	// communication details with the remote host, such as
																	// protocol version and endian format. The ID needs to
																	// read the same in big and little endian.
				LABELLED_RESPONSE_PACKET	=0xF0000002,			// Labelled response packets are given this type so
																	// there isn't confusion when ID conflicts occur.
				RESPONSE_PACKET				=0xF0000001,			// Response packets are given this type so there isn't
																	// confusion when ID conflicts occur.
				MINIMUM_RESERVED_PACKET_ID	=0xF0000000				// Packets IDs after this value are reserved
			};
		};
		typedef PacketTypes::Value PacketType;

		struct PacketPoolResizeActions
		{
			enum Value{					RESIZE_ENTIRE_POOL=1,		//Re-allocates enough memory for the
																	//pool size.
										AFFECT_PACKET_COUNT_ONLY=2,	//Attempts to avoid a reallocation
																	//and instead just affect the number
																	//of available packets.
										};
		};
		typedef PacketPoolResizeActions::Value PacketPoolResizeAction;

		static void SetPlatformBigEndian(bool isBigEndian) {mPlatformBigEndian=isBigEndian;}
		static bool IsPlatformBigEndian()					{return mPlatformBigEndian;}
		
		NetworkManager& GetNetworkManager() {return mNetworkManager;}

		bool Connect();
		bool Disconnect();
		void SetOwner(ConnectionOwner* owner);
		ConnectionOwner* GetOwner() const				{return mOwner;}
		std::string GetFriendlyIdentifier();
		std::string GetLocalFriendlyIdentifier();

		shared_ptr<DataPacket> NewDataPacket();
		shared_ptr<DataPacket> NewDataPacket(u32 packetTypeID, u32 size);
		shared_ptr<DataPacket> NewDataPacket(std::string label, u32 size);

		/**
		 * Process received packet callbacks.
		 * This is called by NetworkManager to process packet call backs. Usually on a different thread
		 * to receiving data to avoid packet processing affecting receive speed.
		 */
		void NotifyAnyReceivedPackets();
		size_t GetNumReceviedPackets() const {return mReceviedPackets.size();}

		/**
		 * Send a DataPacket
		 * @param packet the packet
		 * @param prioritise If true the packet will be sent before of any packets currently queued.
		 */
		void SendDataPacket(shared_ptr<DataPacket> packet, PacketCallback responseCallback = PacketCallback(), bool prioritise = false, bool disconnectAfterSend = false, bool isResponsePacket = false);

		/**
		 * Helper method to send data.
		 * This method builds a DataPacket for you.
		 * @param data The data
		 * @param dataSize The number of bytes to send
		 * @param packetTypeID The packet ID to send the data as.
		 * @param prioritise If true the packet will be sent before of any packets currently queued.
		 */
		void SendData(const u8* data, u32 dataSize, u32 packetTypeID, PacketCallback responseCallback = PacketCallback(), bool prioritise = false);

		/**
		 * Helper method to send a string.
		 * @param message the message to send.
		 * @param packetTypeID the packet ID to send the message as.
		 * @param prioritise If true the packet will be sent before of any packets currently queued.
		 */
		void SendMessage(const std::string& message, u32 packetTypeID, PacketCallback responseCallback = PacketCallback(), bool prioritise = false);
		
		/**
		 * Send a control packet.
		 * A control packet has an ID and no data. Control packets are designed to be used for notification.
		 * @note You cannot send a control packet as response because the ID will be set to the response ID.
		 * @param packetTypeID the ID of the packet you want to send.
		 * @param prioritise If true the packet will be sent before of any packets currently queued.
		 */
		void SendControlPacket(u32 packetTypeID, PacketCallback responseCallback = PacketCallback(), bool prioritise = false);

		/**
		 * Helper method to send a labelled DataPacket.
		 * @param prioritise If true the packet will be sent before of any packets currently queued.
		 */
		void SendLabelledPacket(const std::string& label, const u8* data, u32 dataSize, PacketCallback responseCallback = PacketCallback(), bool prioritise = false);

		/**
		 * Helper method to send a labeled DataPacket.
		 * @param prioritise If true the packet will be sent before of any packets currently queued.
		 */
		template< typename T>
		void SendLabelledPacket(const std::string& label, const T& content, PacketCallback responseCallback = PacketCallback(), bool prioritise = false)
		{
			//TODO: try and catch.
			std::string contentAsString=boost::lexical_cast<std::string>(content);
			SendLabelledPacket(label,reinterpret_cast<const u8*>(contentAsString.c_str()),contentAsString.length(),responseCallback,prioritise);
		}

		/**
		 * Helper method to send a labeled DataPacket with a string as the content.
		 * @param prioritise If true the packet will be sent before of any packets currently queued.
		 */
		void SendLabelledPacket(const std::string& label, const std::string& content, PacketCallback responseCallback = PacketCallback(), bool prioritise = false);

		/**
		 * Helper method to send a labeled DataPacket with multiple strings as the content.
		 * @param prioritise If true the packet will be sent before of any packets currently queued.
		 */
		void SendLabelledPacket(const std::string& label, const std::vector<std::string>& content, PacketCallback responseCallback = PacketCallback(), bool prioritise = false);

		/**
		 * Send a response DataPacket
		 * @note The PacketType is overridden to allow the to be tracked as a response correctly.
		 * @param packetRespondingTo The packet you are responding to.
		 * @param packet The packet you want to send
		 * @param prioritise If true the packet will be sent before of any packets currently queued.
		 * @param disconnectAfterSend If true disconnect the Connection after the packet is sent.
		 */
		void SendDataPacketResponse(shared_ptr<DataPacket> packetRespondingTo, shared_ptr<DataPacket> packet, bool prioritise = false, bool disconnectAfterSend = false);

		/**
		 * Send data as a response to a DataPacket.
		 * @param packetRespondingTo The packet you are responding to.
		 * @param data The data
		 * @param dataSize The number of bytes to send
		 * @param prioritise If true the packet will be sent before of any packets currently queued.
		 * @param disconnectAfterSend If true disconnect the Connection after the packet is sent.
		 */
		void SendDataResponse(shared_ptr<DataPacket> packetRespondingTo, const u8* data, u32 dataSize, bool prioritise = false, bool disconnectAfterSend = false);

		/**
		 * Send a message as a response to a DataPacket
		 * @param packetRespondingTo The packet you are responding to.
		 * @param message The message to send
		 * @param prioritise If true the packet will be sent before of any packets currently queued.
		 * @param disconnectAfterSend If true disconnect the Connection after the packet is sent.
		 */
		void SendMessageResponse(shared_ptr<DataPacket> packetRespondingTo, const std::string& message, bool prioritise = false, bool disconnectAfterSend = false);
		
		/**
		 * Build and send a labelled packet as a response to a DataPacket.
		 * @param packetRespondingTo The packet you are responding to.
		 * @param label The label of the packet to send.
		 * @param data The data
		 * @param dataSize The number of bytes to send
		 * @param prioritise If true the packet will be sent before of any packets currently queued.
		 * @param disconnectAfterSend If true disconnect the Connection after the packet is sent.
		 */
		void SendLabelledPacketResponse(shared_ptr<DataPacket> packetRespondingTo, const std::string& label, const u8* data, u32 dataSize, bool prioritise = false, bool disconnectAfterSend = false);
		
		/**
		 * Build and send a labelled packet with content converted to a string as a response to a DataPacket.
		 * @param packetRespondingTo The packet you are responding to.
		 * @param label The label of the packet to send.
		 * @param content The content to convert to a string then send.
		 * @param prioritise If true the packet will be sent before of any packets currently queued.
		 * @param disconnectAfterSend If true disconnect the Connection after the packet is sent.
		 * @return true if the content could be converted to a string and it was queued to send. false indicates the conversion failed.
		 */
		template< typename T>
		bool SendLabelledPacketResponse(shared_ptr<DataPacket> packetRespondingTo, const std::string& label, const T& content, bool prioritise = false, bool disconnectAfterSend = false)
		{
			try
			{
				std::string contentAsString=boost::lexical_cast<std::string>(content);
				SendLabelledPacketResponse(packetRespondingTo, label,reinterpret_cast<const u8*>(contentAsString.c_str()),contentAsString.length(),prioritise,disconnectAfterSend);
				return true;
			}catch(...)
			{
			}
			return false;
		}

		/**
		 * Build and send a labelled packet with a string included in the data as a response to a DataPacket.
		 * This is similar to responding with a message, except the packet is sent as a labelled packet response so
		 * the label is preserved.
		 * @param packetRespondingTo The packet you are responding to.
		 * @param label The label of the packet to send.
		 * @param content The string content to send.
		 * @param prioritise If true the packet will be sent before of any packets currently queued.
		 * @param disconnectAfterSend If true disconnect the Connection after the packet is sent.
		 */
		void SendLabelledPacketResponse(shared_ptr<DataPacket> packetRespondingTo, const std::string& label, const std::string& content, bool prioritise = false, bool disconnectAfterSend = false);

		/**
		 * Build and send a labelled packet with multiple strings included in the data as a response to a DataPacket.
		 * @param packetRespondingTo The packet you are responding to.
		 * @param label The label of the packet to send.
		 * @param content The strings to send.
		 * @param prioritise If true the packet will be sent before of any packets currently queued.
		 * @param disconnectAfterSend If true disconnect the Connection after the packet is sent.
		 */
		void SendLabelledPacketResponse(shared_ptr<DataPacket> packetRespondingTo, const std::string& label, const std::vector<std::string>& content, bool prioritise = false, bool disconnectAfterSend = false);
		
		/**
		 * Attempts to receive data.
		 * Data received will be used to build DataPacket objects. When a full packet
		 * is received it will be added to the list of received packets. The number of
		 * packets available will be returned.
		 * This method will call Receive().
		 * @note You do not normally need to call this method manually. The NetworkSystem
		 * managing the connection.
		 */
		void UpdateReceive();
		
		/**
		 * Attempts to send data.
		 * This method will call Send().
		 * @note You do not normally need to call this method manually.
		 */
		void UpdateSend(bool reenable);

		/**
		 * Used to change the state of the connection flag.
		 * @note This is used by network systems and should not be used in attempt to connect
		 * or disconnect. Use the dedicated methods for that which will perform the desired
		 * action.
		 * @parm state The new state, if it is not connected the can send flag is set to false.
		 */
		void SetState(State state);
		
		bool IsConnected() const {return (mState==States::CONNECTED);}
		
		/**
		 * Get whether the connection is in a connecting state or not.
		 */
		bool GetConnecting() const {return (mState==States::CONNECTING);}
		
		const ConnectionDetails& GetConnectionDetails() const { return mConnectionDetails; }
		void SetConnectionDetails(const ConnectionDetails& val) { mConnectionDetails = val; }
		const ConnectionDetails& GetLocalConnectionDetails() const { return mLocalConnectionDetails; }
		void SetLocalConnectionDetails(const ConnectionDetails& val) { mLocalConnectionDetails = val; }

		/**
		 * Register a Packet Callback method for a labelled packet.
		 * Callbacks can be registered for incoming packets which will be called when the
		 * incoming packet is identified as a labelled packet.
		 * @param label The label in which to match a labelled incoming packet.
		 * @param callback The callback to call if the label matches the packet label.
		 */
		void RegisterLabelledPacketCallback(const std::string& label, LabelledPacketCallback callback);

		/**
		 * Register a Packet Callback method for a packet.
		 * Callbacks can be registered for incoming packets which will be called when the
		 * incoming packet has the specified ID. This allows you to register callbacks for
		 * packets with different ids and not have to handle the use of the packet in
		 * ReceivedPacket.
		 * @note Some packet IDs are reserved, see PacketTypes for more information.
		 * @param packetTypeID The id in which to match with the id of an incoming packet.
		 * @param callback The callback to call if the id matches the packet id.
		 */
		void RegisterPacketCallback(u32 packetTypeID, PacketCallback callback);
		
		/**
		 * Register a "Connect" Callback method with an identifier.
		 * @note The callback will only be called if the callback is registered before the connection event.
		 * @note To remove the callback you need to reference the identifier. This is because boost functions cannot be compared.
		 * @param identifier The identifier in which to reference the registration.
		 * @param callback The callback to call when the connection connects.
		 */
		void RegisterConnectCallback(const std::string& identifier, ConnectCallback callback);

		/**
		 * Register a "Disconnect" Callback method with an identifier.
		 * @note To remove the callback you need to reference the identifier. This is because boost functions cannot be compared.
		 * @param identifier The identifier in which to reference the registration.
		 * @param callback The callback to call when the connection connects.
		 */
		void RegisterDisconnectCallback(const std::string& identifier, DisconnectCallback callback);
		
		/**
		 * Clear all labelled packet callbacks with corresponding label.
		 * @param label
		 */
		void ClearLabelledPacketCallbacks(const std::string& label);

		/**
		 * Clear all labelled packet callbacks.
		 */
		void ClearAllLabelledPacketCallbacks();

		/**
		 * Clear all packet callbacks for corresponding packet ID.
		 * @param packetTypeID
		 */
		void ClearPacketIDCallbacks(u32 packetTypeID);

		/**
		 * Clear all packet id callbacks that were registered with RegisterPacketCallback(u32,PacketCallback)
		 */
		void ClearAllPacketIDCallbacks();

		/**
		 * Clear all labelled and packet ID callbacks.
		 */
		void ClearAllPacketCallbacks();
	
		/**
		 * Clear connect callbacks corresponding to identifier.
		 * @param identifier
		 */
		void ClearConnectCallbacks(const std::string& identifier);
		
		/**
		 * Clear all of the connect callbacks.
		 */
		void ClearAllConnectCallbacks();

		/**
		 * Clear disconnect callbacks corresponding to identifier.
		 * @param identifier
		 */
		void ClearDisconnectCallbacks(const std::string& identifier);
		
		/**
		 * Clear all of the disconnect callbacks.
		 * @param attemptReconnet
		 */
		void ClearAllDisconnectCallbacks();

		/**
		 * Clear all callbacks.
		 */
		inline void ClearAllCallbacks()
		{
			ClearAllPacketCallbacks();
			ClearAllConnectCallbacks();
			ClearAllDisconnectCallbacks();
		}
		
		/**
		 * Set whether this connection should automatically reconnect.
		 * @see SetAutoAttemptReconnectTime() for information on making this work for non-regular Kernel updates.
		 * @param attemptReconnet true if this Connection should attempt to reconnect after the reconnect time.
		 */
		void SetAutoAttemptReconnect(bool attemptReconnet){mAutoAttemptReconnect=attemptReconnet;}
		bool GetAutoAttemptReconnect() const {return mAutoAttemptReconnect;}
		
		/**
		 * Set the auto reconnect time.
		 * @note The Connection will configure a CountDownTimer to process the reconnect. CountDownTimers
		 * are Tasks so they are updated during Kernel updates. This means that if an ExecutionModel that
		 * does not update regularly is in use (such as the NetworkExecutionModel) the auto reconnect time
		 * may turn out to be longer than what is configured in the Connection.
		 * @param seconds how long to wait before attempting to reconnect if the connection has disconnected.
		 */
		void SetAutoAttemptReconnectTime(Seconds seconds);
		
		/**
		 * This method will be called internally or by a network system to disconnect the connection.
		 * If the connection is connected then this method at the very minimum should:
		 *  - Set the connection into the disconnected state using SetConnected(false) then;
		 *  - call mManager.UpdateConnect(shared_from_this()) to notify the manager to process
		 *    the dropped connection.
		 * @return true if the connection changed state to disconnect otherwise false the connection
		 * is already disconnected or if something prevents the connection from disconnecting.
		 */
		virtual bool _Disconnect()=0;
		
		/**
		 * Check whether or not the remote host is big endian.
		 * @return true if the remote host is big endian, otherwise false to indicate little endian.
		 */
		bool IsRemoteBigEndian() const {return mIsRemoteBigEndian;}
		
		/**
		 * Set whether or not to discard the data packet queue upon disconnect.
		 * If false (default), any packets queued will be sent when the connection reestablishes.
		 * If true, upon disconnect the data packet queue will be discarded.
		 * If true, packets are discarded after the disconnect callbacks are called.
		 */
		void SetDiscardDataPacketQueueOnDisconnect(bool discard){mDiscardDataPacketQueueOnDisconnect=discard;}

		/**
		 * Get whether or not to discard the data packet queue upon disconnect.
		 */
		bool GetDiscardDataPacketQueueOnDisconnect() const {return mDiscardDataPacketQueueOnDisconnect;}
		
		/**
		 * Set whether or not to queue DataPackets if the connection is not connected.
		 * By default, DataPackets are added to the send queue and are sent when the connection is established.
		 * If you disable this, calling one of the Send* methods will silently discard the data.
		 */
		void SetQueueDataPacketsIfNotConnected(bool queue){mQueueDataPacketsIfNotConnected=queue;}

		/**
		 * Get whether or not to queue DataPackets if the connection isn't connected.
		 */
		bool GetQueueDataPacketsIfNotConnected() const {return mQueueDataPacketsIfNotConnected;}
		
		/**
		 * Set the temporary buffer size.
		 * This method reallocates the temporary buffer to the specified size.
		 * When a connection is created the temporary buffer takes the size determined by
		 * NetworkManager::GetNewConnectionBufferSize(). In some situations the temporary buffer
		 * size might need to be resized. The temp buffer size doesn't restrict the DataPacket
		 * size, instead it determines how much data can be processed in one block.
		 * @param size The new size in bytes.
		 */
		void SetTempBufferSize(Size sizeInBytes);

		/**
		 * Add an allowed packet type.
		 * If there haven't been any explicitly set packet types then all packet types are allowed.
		 * @note No assumptions are made by the implementation for allowed packet types, with the
		 * exception of PacketTypes::REMOTE_DETAILS which is used internally to determine how to
		 * communicate. This means that responses and labelled packets won't be enabled by default
		 * if you add at least one packet to this list. This to give finer control. For example,
		 * if you do not want connections to respond you do not need to include the response packet
		 * types. If you want to allow built in packet types you can use the
		 * AddBuiltInPacketTypesToAllowList()
		 */
		void AddAllowedPacketType(PacketType packetType);
		void RemoveAllowedPacketType(PacketType packetType);
		std::set<PacketType> GetAllowedPacketTypes() const;

		/**
		 * Add internal packet types to the allowed packet list.
		 */
		void AddBuiltInPacketTypesToAllowList();

		/**
		 * Add a labelled packet type.
		 * If there haven't been any explicitly set packet labels then all labels are allowed.
		 */
		void AddAllowedLablledPacket(std::string label);
		void RemoveAllowedLablledPacket(const std::string& label);
		std::set<std::string> GetAllowedLabelledPackets() const;

		/**
		 * Get the number of bytes that are queued to send.
		 */
		inline Size GetBytesQueuedToSend() const
		{
			return mBytesQueuedToSend;
		}

		/**
		 * Get the number of bytes this connection has sent.
		 */
		inline Size GetBytesSent() const
		{
			return mBytesSent;
		}

		/**
		 * Get the number of bytes this connection has received.
		 */
		inline Size GetBytesReceived() const
		{
			return mBytesReceived;
		}

		/**
		 * The backlog callback will be called if too much data has been buffered. The callback
		 * can decide whether or not to keep the packet that triggered the callback. It will be
		 * fired for every new packet that is to be sent while the total bytes to send exceeds
		 * the trigger threshold.
		 * @see BacklogCallback
		 * @note The callback is not proteced by a mutex and is used in SendDataPacket() so make
		 * sure you aren't calling SendDataPacket() at the same time as modifying the callback.
		 */
		void SetBacklogCallback(BacklogCallback backlogCallback)
		{
			mBacklogCallback = backlogCallback;
		}

		/**
		 * Clear the backlog callback.
		 * @note The callback is not proteced by a mutex and is used in SendDataPacket() so make
		 * sure you aren't calling SendDataPacket() at the same time as modifying the callback.
		 */
		void ClearBacklogCallback()
		{
			mBacklogCallback = BacklogCallback();
		}

		/**
		 * Set the threshold of bytes to send that the backlog callback will be called.
		 * If you set the threshold to 0 the backlog callback will be disabled.
		 */
		void SetBacklogCallbackTriggerThreshold(Size backlogCallbackTriggerThreshold)
		{
			mBacklogCallbackTriggerThreshold = backlogCallbackTriggerThreshold;
		}

		Size GetBacklogCallbackTriggerThreshold() const
		{
			return mBacklogCallbackTriggerThreshold;
		}
	protected:
		static bool mPlatformBigEndian;
		NetworkManager& mNetworkManager;
		bool mDiscardDataPacketQueueOnDisconnect;
		bool mQueueDataPacketsIfNotConnected;
		u32 mNextPacketID;

		//Only the NetworkManager can clean up a connection
		friend class NetworkManager;
		Connection(NetworkManager& manager);
		virtual ~Connection();
		
		/**
		 * Called by NetworkManager after receiving remote details.
		 */
		void SetRemoteBigEndian(bool isBigEndian) {mIsRemoteBigEndian = isBigEndian;}

		/**
		 * Process the remote details packet after initial connection.
		 */
		void OnRemoteDetails(shared_ptr<Connection> connection, shared_ptr<DataPacket> dataPacket);
		
		virtual ReceiveStatus ReceivePackets();
		virtual SendStatus SendPackets(bool reenable);
		void SendHostDetails();

		shared_ptr<DataPacket> mHeaderPacket;
		shared_ptr<DataPacket> mCurrentPacket;
		
		ConnectionOwner* mOwner;

		unique_ptr<u8[]> mTempBuffer;
		Size mTempBufferSize;
		
		State mState;
		bool mCanSend;
		bool mAutoAttemptReconnect;						/// If true will attempt to automatically reconnect if the connection drops.
		bool mManualDisconnect;							/// Used to determine whether we should automatically reconnect.
		Mutex mQueuedPacketsMutex;
		Mutex mReceviedPacketsMutex;
		Mutex mResponseCallbackMutex;
		std::list< std::pair< shared_ptr<DataPacket>, bool > > mQueuedPackets;
		std::pair< shared_ptr<DataPacket>, bool > mCurrentSendPacket;
		std::list< shared_ptr<DataPacket> > mReceviedPackets;
		Size mHeaderBytesSent;
		bool mHeaderSent;								//Flag: concerned with mCurrentSendPacket
		bool mIsRemoteBigEndian;

		/**
		 * Normally the implementation is just a wrapper around a send method.
		 * The implementation should not block.
		 * The implementation should perform error checking.
		 * @param buffer The buffer to send
		 * @param bufferSize The number of bytes to try and send. If only part of
		 * the buffer can be sent then that should be reflected in the return value.
		 * @return A SendResult that includes the status and number of bytes sent.
		 */
		virtual SendResult Send(const u8 * buffer, int numberOfBytesToSend) = 0;

		/**
		 * Normally the implementation is just a wrapper around a receive method.
		 * The method should be non-blocking.
		 * @param buffer The buffer to write to
		 * @param bufferSize The maximum number of bytes that can be written.
		 * @return A ReadResult that includes the status and number of bytes read.
		 */
		virtual ReceiveResult Receive(u8* buffer, int bufferSizeInBytes) = 0;
		
		virtual void _notifyOwner(shared_ptr<DataPacket> packet);
		
		/**
		 * Called to process dropped connections and notify the connection owner.
		 * @note This method is called by the NetworkManager.
		 */
		virtual void _dropped();
		
		/**
		 * Called to process dropped connections and notify the connection owner.
		 * @note This method is called by the NetworkManager when a connection is established.
		 */
		virtual void _established();
		
		/**
		 * Called to start a connection attempt.
		 * @return false if there were problems, otherwise true.
		 */
		virtual bool _Connect()=0;

		/**
		 * Process the packet through this connection.
		 * This will trigger appropriate callbacks registered with this connection.
		 * @param packet The packet to process.
		 */
		void ProcessReceivedPacket(shared_ptr<DataPacket> packet);

		ConnectionDetails mConnectionDetails;
		ConnectionDetails mLocalConnectionDetails;
		CountDownTimer mReconnectTimer;
		static const u32 mUnreasonableDataSize;			//Data size we consider to be unreasonable.

		Mutex mAccountingMutex;
		Size mBytesQueuedToSend;
		Size mBytesSent;
		Size mBytesReceived;
	private:
		Mutex mResponseCallbacksMutex;
		std::map< u32, PacketCallback > mResponseCallbacks;
		PseudoAtomicMappedContainer< u32, std::vector<PacketCallback> > mPacketCallbacks;
		PseudoAtomicMappedContainer< std::string, std::vector<LabelledPacketCallback> > mLabelledPacketCallbacks;
		PseudoAtomicMappedContainer< std::string, std::vector<ConnectCallback> > mConnectCallbacks;
		PseudoAtomicMappedContainer< std::string, std::vector<DisconnectCallback> > mDisconnectCallbacks;
		PseudoAtomicSet< Connection::PacketType > mAllowedPacketTypes;
		PseudoAtomicSet< std::string > mAllowedLabelledPackets;
		Size mBacklogCallbackTriggerThreshold;
		BacklogCallback mBacklogCallback;
		
		void ProcessLabelledPacket(shared_ptr<Connection> connection, shared_ptr<DataPacket> packet);
	};
}
#endif
