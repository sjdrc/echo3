#include <echo/Network/Connection.h>
#include <echo/Network/ConnectionOwner.h>
#include <echo/Network/DataPacket.h>
#include <echo/Network/NetworkManager.h>
#include <echo/Kernel/TaskManager.h>
#include <echo/Kernel/ScopedLock.h>
#include <echo/Maths/EchoMaths.h>
#include <echo/Chrono/Chrono.h>
#include <echo/Util/StringUtils.h>
#include <boost/scope_exit.hpp>
#include <echo/cpp/functional>
#include <iostream>

namespace Echo
{
#ifdef ECHO_BIG_ENDIAN
	bool Connection::mPlatformBigEndian=true;
#else
	bool Connection::mPlatformBigEndian=false;
#endif
	const u32 Connection::mUnreasonableDataSize=0x00A00000;	//10MiB for a packet HUGE really.

	Connection::Connection(NetworkManager& manager) : mNetworkManager(manager)
	{
		mState=States::DISCONNECTED;
		mCanSend=false;
		mAutoAttemptReconnect = false;
		mQueueDataPacketsIfNotConnected = true;
		mDiscardDataPacketQueueOnDisconnect = false;
		SetAutoAttemptReconnectTime(Seconds(5.));
		mReconnectTimer.AddTimeoutFunction(bind(&Connection::Connect,this),"Connection::Connect");
		mOwner=0;
		mHeaderSent=false;
		mHeaderBytesSent=0;
		// This is updated when a PacketTypes::REMOTE_DETAILS is received, which is queued to send first when a connection is established.
		mIsRemoteBigEndian = false;
		mTempBuffer = nullptr;
		mTempBufferSize = 0;
		mBytesQueuedToSend = 0;
		mBytesSent = 0;
		mBytesReceived = 0;
		mBacklogCallbackTriggerThreshold = 0;

		DataPacketHeader header;
		mHeaderPacket=shared_ptr<DataPacket>(new DataPacket(0,header.GetHeaderDataSizeInBytes()));
		RegisterPacketCallback(PacketTypes::LABELLED_PACKET,bind(&Connection::ProcessLabelledPacket,this,placeholders::_1,placeholders::_2));
		RegisterPacketCallback(PacketTypes::REMOTE_DETAILS, bind(&Connection::OnRemoteDetails,this,placeholders::_1,placeholders::_2));
		
		SetTempBufferSize(manager.GetNewConnectionBufferSize());
	}

	Connection::~Connection()
	{
	}
	
	void Connection::SetTempBufferSize(Size sizeInBytes)
	{
		ECHO_ASSERT_NOT_NULL(sizeInBytes);
		mTempBuffer.reset(new u8[sizeInBytes]);
		mTempBufferSize = sizeInBytes;
	}

	void Connection::SetOwner(ConnectionOwner* receiver)
	{
		mOwner=receiver;
	}

	void Connection::SetState(State state)
	{
		if(mState==state)
		{
			return;
		}
		mState=state;

		//Disable sending if we aren't connected.
		if(mState!=States::CONNECTED)
		{
			mCanSend = false;
		}else
		{
			SendHostDetails();
			UpdateSend(true);
		}
	}
	
	shared_ptr<DataPacket> Connection::NewDataPacket()
	{
		return mNetworkManager.NewDataPacket();
	}

	shared_ptr<DataPacket> Connection::NewDataPacket(u32 packetTypeID, u32 size)
	{
		shared_ptr<DataPacket> packet = mNetworkManager.NewDataPacket();
		packet->Configure(packetTypeID,size);
		return packet;
	}
	
	shared_ptr<DataPacket> Connection::NewDataPacket(std::string label, u32 size)
	{
		shared_ptr<DataPacket> packet = mNetworkManager.NewDataPacket();
		packet->Configure(label,size);
		return packet;
	}

	void Connection::_notifyOwner(shared_ptr<DataPacket> packet)
	{
		if(mOwner)
		{
			mOwner->ReceivedPacket(shared_from_this(), packet);
		}
	}

	void Connection::_dropped()
	{
		SetState(States::DISCONNECTED);
		if(mOwner)
		{
			mOwner->ConnectionDrop(shared_from_this());
		}
		
		// Always reset any current receiving/sending data
		mHeaderSent = false;
		mHeaderBytesSent=0;
		if(mHeaderPacket)
		{
			mHeaderPacket->mReceived=0;
		}
		mCurrentPacket.reset();

		shared_ptr<Connection> connection = shared_from_this();
		
		std::map< std::string, std::vector<DisconnectCallback> >& disconnectCallbacks = mDisconnectCallbacks.GetMap();
		
		typedef std::pair< const std::string, std::vector<DisconnectCallback> > IdentifierDisconnectPair;
		for(IdentifierDisconnectPair& idp : disconnectCallbacks)
		{
			for(DisconnectCallback& callback : idp.second)
			{
				callback(connection);
			}
		}
		
		if(mDiscardDataPacketQueueOnDisconnect)
		{
			ScopedLock lock(mQueuedPacketsMutex);
			mQueuedPackets.clear();
			mCurrentSendPacket.first.reset();
		}else
		{
			ScopedLock lock(mQueuedPacketsMutex);
			// Make sure the sending packet is reset for resending
			if(mCurrentSendPacket.first)
			{
				mCurrentSendPacket.first->mReceived = mCurrentSendPacket.first->mSize;
			}
		}
		
		//Attempt to reconnect
		if(mAutoAttemptReconnect)
		{
			if(!mNetworkManager.HasTask(mReconnectTimer))
			{
				mNetworkManager.AddTask(mReconnectTimer);
			}
			mReconnectTimer.Reset();
		}
	}

	void Connection::_established()
	{
		mHeaderBytesSent=0;
		if(mOwner)
		{
			mOwner->ConnectionEstablished(shared_from_this());
		}

		shared_ptr<Connection> connection = shared_from_this();
		
		std::map< std::string, std::vector<ConnectCallback> >& connectCallbacks = mConnectCallbacks.GetMap();
		
		typedef std::pair< const std::string, std::vector<ConnectCallback> > IdentifierConnectPair;
		for(IdentifierConnectPair& icp : connectCallbacks)
		{
			for(ConnectCallback& callback : icp.second)
			{
				callback(connection);
			}
		}
	}
	bool Connection::Connect()
	{
		if(!IsConnected() && !GetConnecting())
		{
			mReconnectTimer.Pause();
			mNetworkManager.RemoveTask(mReconnectTimer);
			mManualDisconnect=false;	//Reset this to allow auto connect to work.
			return _Connect();
		}
		return false;
	}
	
	bool Connection::Disconnect()
	{
		if(IsConnected())
		{
			mManualDisconnect=true;		//This was a deliberate disconnect, this stops allow auto connecting.
			return _Disconnect();
		}
		return false;
	}

	void Connection::UpdateReceive()
	{
		ReceiveStatus status = ReceivePackets();
		if(status==ReceiveStatuses::DISCONNECT)
		{
			_Disconnect();
		}
	}

	Connection::ReceiveStatus Connection::ReceivePackets()
	{
		assert(mHeaderPacket);

		//If there isn't a current packet we need to extract header data
		ReceiveResult receiveResult = Receive(mTempBuffer.get(),mTempBufferSize);
		if(receiveResult.mStatus!=ReceiveStatuses::SUCCESS)
		{
			return receiveResult.mStatus;
		}
		mNetworkManager.ReportReceivedData(receiveResult.mBytesReceived);

		// Receiving only happens on one thread so we don't need to protect writing to this.
		mBytesReceived+=receiveResult.mBytesReceived;

		//ECHO_LOG_DEBUG("0x" << std::hex << this << std::dec << "Recv: " << mTempBuffer << ":" << receiveResult.mBytesReceived);

		u8* bufferStart=mTempBuffer.get();
		while(receiveResult.mBytesReceived>0)
		{
			//ECHO_LOG_DEBUG("bufferStart: " << bufferStart << ":" << receiveResult.mBytesReceived);
			u32 headerBytes=0;
			//ECHO_LOG_DEBUG(receiveResult.mBytesReceived << " bytes");
			if(!(mHeaderPacket->HasReceivedAllData()))				//We have a valid header
			{
				//ECHO_LOG_DEBUG("I'll try and make a header for you");
				//Add the received data to the header packet
				// Avoid branching by indexing the result of bool to int conversion
				u32 minSize[] = {static_cast<u32>(receiveResult.mBytesReceived), mHeaderPacket->GetRemainingDataSize()};
				u32 headerBytes = minSize[ (static_cast<u32>(receiveResult.mBytesReceived) > mHeaderPacket->GetRemainingDataSize()) ];

				//ECHO_LOG_DEBUG("Appending " << headerBytes <<  " bytes");
				mHeaderPacket->AppendData(bufferStart,headerBytes);
				if(mHeaderPacket->HasReceivedAllData())
				{
					//ECHO_LOG_DEBUG("Found enough data for a full header...";
					DataPacketHeader header;
					if(!header.BuildFromPacketData(*mHeaderPacket))
					{
						ECHO_LOG_ERROR("Failed to build packet header from incoming data");
						mHeaderPacket->mReceived=0;
						return ReceiveStatuses::DISCONNECT;
					}
					
					//Determine if the packet size is reasonable.
					//If it is then get rid of that incoming data
					if(header.GetDataLength()<=mUnreasonableDataSize)
					{
						mCurrentPacket=NewDataPacket();
						mCurrentPacket->Configure(header);
					}else
					{
						ECHO_LOG_ERROR("DataPacket length was too large: " << header.GetDataLength() << " when maximum is " << mUnreasonableDataSize);
						mHeaderPacket->mReceived=0;
						return ReceiveStatuses::DISCONNECT;
					}
				}
				receiveResult.mBytesReceived-=headerBytes;				//For next part
				bufferStart+=headerBytes;
			}
			if(mCurrentPacket)
			{
				///ECHO_LOG_DEBUG("Constructing Packet...");
				if(receiveResult.mBytesReceived>0)
				{
					//ECHO_LOG_DEBUG("Appending " << headerBytes <<  " bytes");
					u32 bytesAppended=0;
					bytesAppended=mCurrentPacket->AppendData(bufferStart, receiveResult.mBytesReceived);
					u32 remaining=receiveResult.mBytesReceived-bytesAppended;

					//There could be another packet waiting - receiveResult.mBytesReceived tracks how much is left
					bufferStart+=bytesAppended;

					receiveResult.mBytesReceived-=bytesAppended;
				}
				if(mCurrentPacket->HasReceivedAllData())
				{
					//ECHO_LOG_DEBUG("Packet Constructed!");
					//ECHO_LOG_DEBUG("Notifying Owner...");
					mNetworkManager.ConnectionPacketReceived(shared_from_this(), mCurrentPacket);
					mCurrentPacket.reset();
					mHeaderPacket->mReceived=0;
				}
			}
		}
		return ReceiveStatuses::SUCCESS;
	}

	void Connection::UpdateSend(bool reenable)
	{
		Connection::SendStatus status = SendPackets(reenable);
		switch(status)
		{
			case Connection::SendStatuses::DISCONNECT:
				_Disconnect();
			break;
			case Connection::SendStatuses::DISCONNECT_REQUESTED:
				Disconnect();
			break;
			case Connection::SendStatuses::WAIT:
				break;
			case Connection::SendStatuses::SUCCESS:
				break;
			case Connection::SendStatuses::INTERNAL_FAILURE:
				break;
		}
	}

	Connection::SendStatus Connection::SendPackets(bool reenable)
	{
		// reenable is true when the NetworkSystem changes the connections state appropriately
		// or calls UpdateWrite(true) explicitly to update the write state.
		if(!reenable)
		{
			// If we can't send or acquire a lock, we need to bail. The latter means something else is writing.
			if(!mCanSend || !mQueuedPacketsMutex.AttemptLock())
			{
				return SendStatuses::INTERNAL_FAILURE;
			}
		}else
		{
			mQueuedPacketsMutex.Lock();
		}
		BOOST_SCOPE_EXIT(&mQueuedPacketsMutex)
		{
			mQueuedPacketsMutex.Unlock();
		} BOOST_SCOPE_EXIT_END

		mCanSend=false;
		//ECHO_LOG_DEBUG(mQueuedPackets.size());
		//if(!mSendingData)
		
		while(!(mQueuedPackets.empty()) || mCurrentSendPacket.first)
		{
			shared_ptr<DataPacket> packet = mCurrentSendPacket.first;
			if(!packet)
			{
				// This means mQueuedPackets is not empty(), otherwise we would have exited the loop
				mCurrentSendPacket = mQueuedPackets.front();
				mQueuedPackets.pop_front();
				packet = mCurrentSendPacket.first;
			}
			if(packet)
			{
				//ECHO_LOG_DEBUG(packet->GetData());
				//ECHO_LOG_DEBUG("Connection::UpdateWrite() - mCanSend==true");
				//ECHO_LOG_DEBUG("Connection::UpdateWrite():Packet:0x" << std::hex << packet->GetPacketTypeID() << std::dec);
				if(!mHeaderSent)
				{
					DataPacketHeader header;
					header.BuildForPacket(*packet);

					//ECHO_LOG_DEBUG("0x" << std::hex << this << std::dec << ": Send Header");
					SendResult sendResult=Send(header.GetHeaderData()+mHeaderBytesSent,header.GetHeaderDataSizeInBytes()-mHeaderBytesSent);
					//ECHO_LOG_DEBUG("0x" << std::hex << socketNum << std::dec);
					mNetworkManager.ReportSentData(sendResult.mBytesSent);
					{
						ScopedLock accountingLock(mAccountingMutex);
						mBytesQueuedToSend-=sendResult.mBytesSent;
						mBytesSent+=sendResult.mBytesSent;
					}

					if(sendResult.mStatus!=SendStatuses::SUCCESS)
					{
						//ECHO_LOG_DEBUG("mCanSend=false; for header");
						mHeaderBytesSent+=sendResult.mBytesSent;
						return sendResult.mStatus;
					}

					//ECHO_LOG_DEBUG("Sent: Header");
					mHeaderSent=true;
					mHeaderBytesSent=0; // Reset for the next packet
					//See if this is just a control packet - If the data packet is not the same as the header
					if(packet->SendHeaderOnly())
					{
						packet.reset();
						mHeaderSent=false;
						mCurrentSendPacket.first.reset();
						if(mCurrentSendPacket.second)
						{
							return SendStatuses::DISCONNECT_REQUESTED;
						}
						continue;
					}
				}

				const u8* data=&(packet->mData[packet->mSize-packet->mReceived]);
				//ECHO_LOG_DEBUG("0x" << std::hex << this << std::dec << ": Send Data");
				SendResult sendResult=Send(data,packet->mReceived);

				packet->mReceived-=sendResult.mBytesSent;

				mNetworkManager.ReportSentData(sendResult.mBytesSent);
				{
					ScopedLock accountingLock(mAccountingMutex);
					mBytesQueuedToSend-=sendResult.mBytesSent;
					mBytesSent+=sendResult.mBytesSent;
				}
				
				if(sendResult.mStatus!=SendStatuses::SUCCESS)
				{
					//ECHO_LOG_DEBUG("mCanSend=false; for packet");
					return sendResult.mStatus;
				}
				
				//ECHO_LOG_DEBUG("Sent: " << packet->mSize-packet->mReceived << " of " << packet->mSize << " bytes");
				if(packet->mReceived==0)			//All our data was sent
				{
					packet.reset();
					mHeaderSent=false;
					mCurrentSendPacket.first.reset();
					if(mCurrentSendPacket.second)
					{
						return SendStatuses::DISCONNECT_REQUESTED;
					}
				}
			}
		}	
		mCanSend=true;
		return SendStatuses::SUCCESS;
	}

	void Connection::SendDataPacket(shared_ptr<DataPacket> packet, PacketCallback responseCallback, bool prioritise, bool disconnectAfterSend, bool isResponsePacket)
	{
		// In the CONNECTING or CONNECTED states we should treat it as connected.

		// If we're not connected, should we silently discard?
		if(mState==States::DISCONNECTED && !mQueueDataPacketsIfNotConnected)
		{
			return;
		}
		
		//ECHO_LOG_DEBUG("SendDataPacket: " << mQueuedPackets.size());
		if(packet)
		{
			if(packet->mReceived!=packet->mSize)
			{
				packet->mReceived=packet->mSize;
			}

			{
				// I want to change the way header size is return in the header class so this header size here is just a quick fix.
				const Size HEADER_SIZE=(sizeof(u32)*3);
				ScopedLock accountingLock(mAccountingMutex);
				mBytesQueuedToSend+=(packet->mSize + HEADER_SIZE);
				if(mBytesQueuedToSend>=mBacklogCallbackTriggerThreshold && mBacklogCallbackTriggerThreshold>0 && mBacklogCallback)
				{
					if(!mBacklogCallback(shared_from_this(),mBytesQueuedToSend,mBacklogCallbackTriggerThreshold,packet))
					{
						mBytesQueuedToSend-=(packet->mSize + HEADER_SIZE);
						return;
					}
				}
			}
			
			if(!isResponsePacket)
			{
				// All packets are sent through this method.	
				packet->mPacketID = mNextPacketID++;
			}
			
			mQueuedPacketsMutex.Lock();
			if(prioritise)
			{
				mQueuedPackets.push_front( std::pair< shared_ptr<DataPacket>, bool >(packet, disconnectAfterSend) );
			}else
			{
				mQueuedPackets.push_back( std::pair< shared_ptr<DataPacket>, bool >(packet, disconnectAfterSend) );
			}
			if(responseCallback)
			{
				ScopedLock locky(mResponseCallbacksMutex);
				mResponseCallbacks[packet->mPacketID] = responseCallback;
			}
			mQueuedPacketsMutex.Unlock();
			//ECHO_LOG_DEBUG("SendDataPacket2: " << mQueuedPackets.size());
			UpdateSend(false);
		}
	}

	void Connection::SendData(const u8* data, u32 dataSize, u32 packetTypeID, PacketCallback responseCallback, bool prioritise)
	{
		shared_ptr<DataPacket> packet=NewDataPacket();
		packet->Configure(packetTypeID,dataSize);
		//Copy the data
		packet->AppendData(data,dataSize);
		SendDataPacket(packet,responseCallback,prioritise);
	}

	void Connection::SendMessage(const std::string& message, u32 packetTypeID, PacketCallback responseCallback, bool prioritise)
	{
		shared_ptr<DataPacket> packet=NewDataPacket();
		packet->Configure(message);
		packet->SetPacketTypeID(packetTypeID);
		packet->AppendString(message);
		SendDataPacket(packet,responseCallback,prioritise);
	}

	void Connection::SendControlPacket(u32 packetTypeID, PacketCallback responseCallback, bool prioritise)
	{
		shared_ptr<DataPacket> packet=NewDataPacket();
		packet->Configure(packetTypeID,0);
		SendDataPacket(packet,responseCallback,prioritise);
	}

	void Connection::SendLabelledPacket(const std::string& label, const u8* data, u32 dataSize, PacketCallback responseCallback, bool prioritise)
	{
		shared_ptr<DataPacket> packet=NewDataPacket();
		packet->Configure(PacketTypes::LABELLED_PACKET,label.length()+DataPacket::NUMBYTES_FOR_STRING_HEADER+dataSize);
		packet->AppendString(label);
		packet->AppendData(data,dataSize);
		SendDataPacket(packet,responseCallback,prioritise);
	}
	
	void Connection::SendLabelledPacket(const std::string& label, const std::string& content, PacketCallback responseCallback, bool prioritise)
	{
		shared_ptr<DataPacket> packet=NewDataPacket();
		packet->Configure(label,content);
		SendDataPacket(packet,responseCallback,prioritise);
	}
	
	void Connection::SendLabelledPacket(const std::string& label, const std::vector<std::string>& content, PacketCallback responseCallback, bool prioritise)
	{
		shared_ptr<DataPacket> packet=NewDataPacket();
		packet->Configure(label,content);
		SendDataPacket(packet,responseCallback,prioritise);
	}
	
	void Connection::SendDataPacketResponse(shared_ptr<DataPacket> packetRespondingTo, shared_ptr<DataPacket> packet, bool prioritise, bool disconnectAfterSend)
	{
		packet->SetPacketID(packetRespondingTo->GetPacketID());
		if(packet->IsLabelledPacket())
		{
			packet->SetPacketTypeID(PacketTypes::LABELLED_RESPONSE_PACKET);
		}else
		{
			packet->SetPacketTypeID(PacketTypes::RESPONSE_PACKET);
		}
		SendDataPacket(packet,PacketCallback(),prioritise,disconnectAfterSend,true);
	}
	
	void Connection::SendDataResponse(shared_ptr<DataPacket> packetRespondingTo, const u8* data, u32 dataSize, bool prioritise, bool disconnectAfterSend)
	{
		shared_ptr<DataPacket> packet=NewDataPacket();
		packet->Configure(PacketTypes::RESPONSE_PACKET,dataSize);
		//Copy the data
		packet->AppendData(data,dataSize);
		packet->SetPacketID(packetRespondingTo->GetPacketID());
		SendDataPacket(packet,PacketCallback(),prioritise,disconnectAfterSend,true);
	}

	void Connection::SendMessageResponse(shared_ptr<DataPacket> packetRespondingTo, const std::string& message, bool prioritise, bool disconnectAfterSend)
	{
		shared_ptr<DataPacket> packet=NewDataPacket();
		packet->Configure(message);
		packet->SetPacketID(packetRespondingTo->GetPacketID());
		packet->SetPacketTypeID(PacketTypes::RESPONSE_PACKET);
		SendDataPacket(packet,PacketCallback(),prioritise,disconnectAfterSend,true);
	}

	void Connection::SendLabelledPacketResponse(shared_ptr<DataPacket> packetRespondingTo, const std::string& label, const u8* data, u32 dataSize, bool prioritise, bool disconnectAfterSend)
	{
		shared_ptr<DataPacket> packet=NewDataPacket();
		packet->Configure(PacketTypes::LABELLED_RESPONSE_PACKET,label.length()+DataPacket::NUMBYTES_FOR_STRING_HEADER+dataSize);
		packet->AppendString(label);
		packet->AppendData(data,dataSize);
		packet->SetPacketID(packetRespondingTo->GetPacketID());
		SendDataPacket(packet,PacketCallback(),prioritise,disconnectAfterSend,true);
	}
	
	void Connection::SendLabelledPacketResponse(shared_ptr<DataPacket> packetRespondingTo, const std::string& label, const std::string& content, bool prioritise, bool disconnectAfterSend)
	{
		shared_ptr<DataPacket> packet=NewDataPacket();
		packet->Configure(label,content);
		packet->SetPacketID(packetRespondingTo->GetPacketID());
		packet->SetPacketTypeID(PacketTypes::LABELLED_RESPONSE_PACKET);
		SendDataPacket(packet,PacketCallback(),prioritise,disconnectAfterSend,true);
	}
	
	void Connection::SendLabelledPacketResponse(shared_ptr<DataPacket> packetRespondingTo, const std::string& label, const std::vector<std::string>& content, bool prioritise, bool disconnectAfterSend)
	{
		shared_ptr<DataPacket> packet=NewDataPacket();
		packet->Configure(label,content);
		packet->SetPacketID(packetRespondingTo->GetPacketID());
		packet->SetPacketTypeID(PacketTypes::LABELLED_RESPONSE_PACKET);
		SendDataPacket(packet,PacketCallback(),prioritise,disconnectAfterSend,true);
	}	
	
	void Connection::SendHostDetails()
	{
		#ifdef ECHO_LITTLE_ENDIAN
		std::string hostDetails="1.0:little";
		#elif defined(ECHO_BIG_ENDIAN)
		std::string hostDetails="1.0:big";
		#else
		#error "Unable to determine host endianess"
		#endif
		ECHO_LOG_DEBUG("Sending host details " << hostDetails);
		SendMessage(hostDetails,PacketTypes::REMOTE_DETAILS, PacketCallback(), true);
	}
	
	void Connection::OnRemoteDetails(shared_ptr<Connection> connection, shared_ptr<DataPacket> dataPacket)
	{	
		std::string remoteDetails;
		if(!dataPacket->GetStringFromDataPacket(remoteDetails))
		{
			connection->Disconnect();
			return;
		}
		
		//Remote details needs to be "protocolVersion:endian"
		// protocolVersion - 1.0
		// endian - "big" or "little"
		
		ECHO_LOG_DEBUG("Remote details: " << remoteDetails);
		std::vector<std::string> parameters;
		Utils::String::Split(remoteDetails,":",parameters);
		
		if(parameters.size()<2)
		{
			ECHO_LOG_WARNING("Received remote details without enough parameters - " << remoteDetails << ". Diconnecting.");
			connection->Disconnect();
			return;
		}
		
		if(parameters[0]!="1.0")
		{
			ECHO_LOG_WARNING("Received remote details specifying an unsupported protocol version - " << parameters[0] << ". Diconnecting.");
			connection->Disconnect();
			return;
		}
		bool bigEndian = true;
		if(parameters[1]=="little")
		{
			bigEndian = false;
		}else if(parameters[1]!="big")
		{
			ECHO_LOG_WARNING("Received remote details specifying an unsupported endian format - " << parameters[1] << ". Diconnecting.");
			connection->Disconnect();
			return;
		}

		connection->SetRemoteBigEndian(bigEndian);
	}

	void Connection::ProcessReceivedPacket(shared_ptr<DataPacket> packet)
	{
		std::set< Connection::PacketType >& allowedPacketTypes = mAllowedPacketTypes.GetSet();
		if(!allowedPacketTypes.empty() && packet->GetPacketTypeID()!=PacketTypes::REMOTE_DETAILS)
		{
			PacketType packetType = static_cast<PacketType>(packet->GetPacketTypeID());
			if(allowedPacketTypes.find(packetType)==allowedPacketTypes.end())
			{
				ECHO_LOG_WARNING("Received packet type (0x"<< std::hex << packetType << std::dec <<") not in allowed list. Disconnecting.");
				Disconnect();
				return;
			}
		}
		
		if(packet->IsResponsePacket())
		{
			PacketCallback callback;
			{
				// We can't call the callback with the mutex locked in case the callback sends something else with a response callback set
				ScopedLock locky(mResponseCallbacksMutex);
				std::map< u32, PacketCallback >::iterator rit=mResponseCallbacks.find(packet->GetPacketID());
				if(rit!=mResponseCallbacks.end())
				{
					callback = std::move(rit->second);
					mResponseCallbacks.erase(rit);
				}
			}
			if(callback)
			{
				callback(shared_from_this(),packet);
			}
		}else
		{
			std::map< u32, std::vector<PacketCallback> >& packetCallbacks = mPacketCallbacks.GetMap();
			std::map< u32, std::vector<PacketCallback> >::iterator it=packetCallbacks.find(packet->GetPacketTypeID());
			if(it!=packetCallbacks.end())
			{
				BOOST_FOREACH(PacketCallback& callback, it->second)
				{
					callback(shared_from_this(),packet);
				}
			}
		}
		_notifyOwner(packet);
	}
	
	void Connection::ProcessLabelledPacket(shared_ptr<Connection> connection, shared_ptr<DataPacket> packet)
	{
		Size dataOffset = 0;
		std::string label = packet->GetLabel(&dataOffset);
		if(label.empty())
		{
			ECHO_LOG_WARNING("Packet with ID LABELLED_PACKET is invalid or does not contain a label.");
			return;
		}
		
		std::set< std::string >& allowedLabelledPackets = mAllowedLabelledPackets.GetSet();
		if(!allowedLabelledPackets.empty())
		{
			if(allowedLabelledPackets.find(label)==allowedLabelledPackets.end())
			{
				ECHO_LOG_WARNING("Received labelled packet ("<< label <<")not in allowed list. Disconnecting.");
				Disconnect();
				return;
			}
		}
		
		std::map< std::string, std::vector<LabelledPacketCallback> >& labelledPacketCallbacks = mLabelledPacketCallbacks.GetMap();
		std::map< std::string, std::vector<LabelledPacketCallback> >::iterator it=labelledPacketCallbacks.find(label);
		if(it!=labelledPacketCallbacks.end())
		{
			u8* dataStart = &(packet->GetData()[dataOffset]);
			Size numberOfBytes = (packet->GetDataSize()-dataOffset);
			BOOST_FOREACH(LabelledPacketCallback& callback, it->second)
			{
				callback(connection,packet,dataStart,numberOfBytes);
			}
		}
	}

	std::string Connection::GetFriendlyIdentifier()
	{
		return mConnectionDetails.ToString();
	}
	
	std::string Connection::GetLocalFriendlyIdentifier()
	{
		return mLocalConnectionDetails.ToString();
	}

	void Connection::RegisterLabelledPacketCallback(const std::string& label, LabelledPacketCallback callback)
	{
		if(callback)
		{
			mLabelledPacketCallbacks.Add(label,callback);
		}
	}

	void Connection::RegisterPacketCallback(u32 packetTypeID, PacketCallback callback)
	{
		if(callback)
		{
			mPacketCallbacks.Add(packetTypeID,callback);
		}
	}
	
	void Connection::RegisterConnectCallback(const std::string& identifier, ConnectCallback callback)
	{
		if(callback)
		{
			mConnectCallbacks.Add(identifier,callback);
		}
	}

	void Connection::RegisterDisconnectCallback(const std::string& identifier, DisconnectCallback callback)
	{
		if(callback)
		{
			mDisconnectCallbacks.Add(identifier,callback);
		}
	}
	
	void Connection::ClearLabelledPacketCallbacks(const std::string& label)
	{
		mLabelledPacketCallbacks.Remove(label);
	}

	void Connection::ClearAllLabelledPacketCallbacks()
	{
		mLabelledPacketCallbacks.Clear();
	}

	void Connection::ClearPacketIDCallbacks(u32 packetTypeID)
	{
		mPacketCallbacks.Remove(packetTypeID);
	}
	
	void Connection::ClearAllPacketIDCallbacks()
	{
		mPacketCallbacks.Clear();
	}
	
	void Connection::ClearAllPacketCallbacks()
	{
		ClearAllPacketIDCallbacks();
		ClearAllLabelledPacketCallbacks();
	}
	
	void Connection::ClearConnectCallbacks(const std::string& identifier)
	{
		mConnectCallbacks.Remove(identifier);
	}
	
	void Connection::ClearAllConnectCallbacks()
	{
		mConnectCallbacks.Clear();
	}

	void Connection::ClearDisconnectCallbacks(const std::string& identifier)
	{
		mDisconnectCallbacks.Remove(identifier);
	}

	void Connection::ClearAllDisconnectCallbacks()
	{
		mDisconnectCallbacks.Clear();
	}
	
	void Connection::SetAutoAttemptReconnectTime(Seconds seconds)
	{
		mReconnectTimer.SetTimeout(seconds);
	}

	void Connection::AddAllowedPacketType(PacketType packetType)
	{
		if(packetType==PacketTypes::REMOTE_DETAILS)
		{
			ECHO_LOG_WARNING("Cannot add PacketTypes::REMOTE_DETAILS to allowed list directly as it is always allowed.");
			return;
		}
		mAllowedPacketTypes.Insert(packetType);
	}

	void Connection::RemoveAllowedPacketType(PacketType packetType)
	{
		if(packetType==PacketTypes::REMOTE_DETAILS)
		{
			ECHO_LOG_WARNING("Cannot remove PacketTypes::REMOTE_DETAILS from allowed packet list");
			return;
		}
		mAllowedPacketTypes.Erase(packetType);
	}
	
	std::set<Connection::PacketType> Connection::GetAllowedPacketTypes() const
	{
		return mAllowedPacketTypes.GetSetCopy();
	}
	
	void Connection::AddBuiltInPacketTypesToAllowList()
	{
		AddAllowedPacketType(PacketTypes::LABELLED_PACKET);
		AddAllowedPacketType(PacketTypes::LABELLED_RESPONSE_PACKET);
		AddAllowedPacketType(PacketTypes::RESPONSE_PACKET);
	}

	void Connection::AddAllowedLablledPacket(std::string label)
	{
		mAllowedLabelledPackets.Insert(label);
	}

	void Connection::RemoveAllowedLablledPacket(const std::string& label)
	{
		mAllowedLabelledPackets.Erase(label);
	}
	
	std::set<std::string> Connection::GetAllowedLabelledPackets() const
	{
		return mAllowedLabelledPackets.GetSetCopy();
	}
	
}
