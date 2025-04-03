#ifndef _BOOSTTCPCONNECTION_H_
#define _BOOSTTCPCONNECTION_H_
#include <echo/Network/Connection.h>
#include <echo/Network/DataPacket.h>
#include <echo/Network/NetRedefinitions.h>
#include <boost/asio/io_context.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <mutex>
#include <condition_variable>

namespace Echo
{
	class BoostASIONetworkSystem;

	class BoostTCPConnection : public Connection
	{
	protected:
		friend class BoostASIONetworkSystem;
		BoostASIONetworkSystem& mManager;

		DataPacketHeader mReceiveHeader;
		DataPacketHeader mSendHeader;
		typedef boost::asio::ip::tcp::socket TCPSocket;
		typedef boost::asio::ip::tcp::resolver Resolver;

		std::mutex mRunMutex;
		std::condition_variable mRunWaiter;
		boost::asio::io_context& mIOContext;
		shared_ptr<TCPSocket> mSocket;
		shared_ptr<Resolver> mResolver;
		bool mInSendLoop;
		bool mAsyncWriting;
		bool mAsyncReading;

		void WritePacket(const boost::system::error_code& ec, std::size_t bytesTransferred, shared_ptr<DataPacket> packet, bool disconnectAfterWrite)
		{
			if (ec)
			{
				ECHO_LOG_ERROR(this << " Failed to write. Only sent "<< bytesTransferred << " of " << mSendHeader.GetHeaderDataSizeInBytes() << ": " << ec.category().name() << ":" << ec.message());
				_Disconnect();
				std::unique_lock< std::mutex > lock(mRunMutex);
				// This is the async write loop, the async read loop should handle the read wait flag
				mAsyncWriting = false;
				mRunWaiter.notify_all();
				return;
			}

			mNetworkManager.ReportSentData(bytesTransferred);
			{
				ScopedLock accountingLock(mAccountingMutex);
				mBytesQueuedToSend-=bytesTransferred;
				mBytesSent+=bytesTransferred;
			}

			if(packet->SendHeaderOnly())
			{
				ECHO_LOG_DEBUG(this << " No Write Packet - headeronly");
				if(disconnectAfterWrite)
				{
					Disconnect();
					std::unique_lock< std::mutex > lock(mRunMutex);
					// This is the async write loop, the async read loop should handle the read wait flag
					mAsyncWriting = false;
					mRunWaiter.notify_all();
				}else
				{
					ScopedLock lock(mQueuedPacketsMutex);
					SendNextPacket();
				}
				return;
			}

			ECHO_LOG_DEBUG(this << " Write packet: " << packet->GetReceivedDataSize());
			boost::asio::async_write(*mSocket,
				boost::asio::buffer(packet->GetData(),packet->GetReceivedDataSize()),
				bind(&BoostTCPConnection::PostWritePacket,this,placeholders::_1,placeholders::_2,packet,disconnectAfterWrite));
		}

		void PostWritePacket(const boost::system::error_code& ec, std::size_t bytesTransferred, shared_ptr<DataPacket> packet, bool disconnectAfterWrite)
		{
			if(ec)
			{
				ECHO_LOG_ERROR(this << " Failed to write. Only sent "<< bytesTransferred << " of " << packet->GetReceivedDataSize() << ": " << ec.category().name() << ":" << ec.message());
				_Disconnect();
				std::unique_lock< std::mutex > lock(mRunMutex);
				// This is the async write loop, the async read loop should handle the read wait flag
				mAsyncWriting = false;
				mRunWaiter.notify_all();
				return;
			}

			mNetworkManager.ReportSentData(bytesTransferred);
			{
				ScopedLock accountingLock(mAccountingMutex);
				mBytesQueuedToSend-=bytesTransferred;
				mBytesSent+=bytesTransferred;
			}

			if(disconnectAfterWrite)
			{
				Disconnect();
				std::unique_lock< std::mutex > lock(mRunMutex);
				// This is the async write loop, the async read loop should handle the read wait flag
				mAsyncWriting = false;
				mRunWaiter.notify_all();
			}else
			{
				ScopedLock lock(mQueuedPacketsMutex);
				SendNextPacket();
			}
		}

		void ReadHeader()
		{
			ECHO_LOG_DEBUG(this << " Read header");
			boost::asio::async_read(*mSocket,boost::asio::mutable_buffer(mReceiveHeader.GetHeaderData(),mReceiveHeader.GetHeaderDataSizeInBytes()), bind(&BoostTCPConnection::ProcessHeader,this,placeholders::_1));
		}

		void ProcessHeader(const boost::system::error_code& ec)
		{
			ECHO_LOG_DEBUG(this << " Process header");
			if (ec)
			{
				ECHO_LOG_ERROR(this << " Failed to build header: " << ec.category().name() << ":" << ec.message());
				_Disconnect();
				std::unique_lock< std::mutex > lock(mRunMutex);
				// This is the async read loop, the async write loop should handle the write wait flag
				mAsyncReading = false;
				mRunWaiter.notify_all();
				return;
			}

			if(mReceiveHeader.GetDataLength() > Connection::mUnreasonableDataSize)
			{
				ECHO_LOG_ERROR(this << " DataPacket length was too large: " << mReceiveHeader.GetDataLength() << " when maximum is " << mUnreasonableDataSize << ":" << ec.category().name() << ":" << ec.message());
				_Disconnect();
				return;
			}
			mNetworkManager.ReportReceivedData(mReceiveHeader.GetHeaderDataSizeInBytes());
			mBytesReceived+=mReceiveHeader.GetHeaderDataSizeInBytes();
			shared_ptr<DataPacket> packet = NewDataPacket();
			packet->Configure(mReceiveHeader);
			if(packet->GetDataSize()==0)
			{
				ECHO_LOG_DEBUG(this << " Header only packet");
				// It might be a header only packet
				mNetworkManager.ConnectionPacketReceived(shared_from_this(), packet);
				ReadHeader();
			}else
			{
				ECHO_LOG_DEBUG(this << " Read packet");
				boost::asio::async_read(*mSocket,boost::asio::mutable_buffer(packet->GetData(),packet->GetDataSize()), bind(&BoostTCPConnection::ProcessPacket,this,placeholders::_1,packet));
			}
		}

		void ProcessPacket(const boost::system::error_code& ec, shared_ptr<DataPacket> packet)
		{
			ECHO_LOG_DEBUG(this << " Process packet");
			if (ec)
			{
				ECHO_LOG_ERROR(this << " Failed to build packet: " << ec.category().name() << ":" << ec.message());
				_Disconnect();
				std::unique_lock< std::mutex > lock(mRunMutex);
				// This is the async read loop, the async write loop should handle the write wait flag
				mAsyncReading = false;
				mRunWaiter.notify_all();
				return;
			}
			// Prepare the packet as though we appended data to it
			mNetworkManager.ReportReceivedData(packet->GetDataSize());
			mBytesReceived+=packet->GetDataSize();
			packet->SetBytesReceived(packet->GetDataSize());
			mNetworkManager.ConnectionPacketReceived(shared_from_this(), packet);
			ReadHeader();
		}

		void ConnectHandler(const boost::system::error_code& ec)
		{
			ECHO_LOG_DEBUG(this << " connect handler");
			if (!ec)
			{
				SetState(Connection::States::CONNECTED);

				mManager.UpdateConnect(shared_from_this());

				// Start the async header+packet loops
				ReadHeader();
				ScopedLock lock(mQueuedPacketsMutex);
				SendNextPacket();
			}else
			{
				ECHO_LOG_ERROR(this << " Failed to connect: " << ec.category().name() << ":" << ec.message());
				_Disconnect();
				std::unique_lock< std::mutex > lock(mRunMutex);
				// We weren't reading or writing at this stage so disable waits on both
				mAsyncReading = false;
				mAsyncWriting = false;
				mRunWaiter.notify_all();
			}
		}

		void ResolveHandler(const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::iterator it)
		{
			if (ec)
			{
				ECHO_LOG_ERROR(this << " Failed to resolve. " << ec.category().name() << ":" << ec.message());
				_Disconnect();
				std::unique_lock< std::mutex > lock(mRunMutex);
				// We weren't reading or writing at this stage so disable waits on both
				mAsyncReading = false;
				mAsyncWriting = false;
				mRunWaiter.notify_all();
				return;
			}
			mSocket->async_connect(*it, bind(&BoostTCPConnection::ConnectHandler,this,placeholders::_1));
		}
	
		bool _Connect() override
		{
			if(IsConnected())
			{
				return false;
			}
			if(!mResolver)
			{
				// This connection was not established, it was received.
				ECHO_LOG_ERROR(this << " Connection attempt made on Connection established by accepting. Unable to re-establish connection.");
				return false;
			}
			std::unique_lock< std::mutex > lock(mRunMutex);
			mAsyncReading = true;
			mAsyncWriting = true;
			SetState(Connection::States::CONNECTING);
			std::string port = mConnectionDetails.GetAdditionalInfoWithIndexFallback<std::string>("port",0,"");
			boost::asio::ip::tcp::resolver::query q{mConnectionDetails.GetAddress(), port};
			mResolver->async_resolve(q, bind(&BoostTCPConnection::ResolveHandler,this,placeholders::_1,placeholders::_2));
			return true;
		}

		bool _Disconnect() override
		{
			if(mState!=States::DISCONNECTED)
			{
				ECHO_LOG_INFO(this << " Disconnected: " << mConnectionDetails.ToString() << "... ");
				SetState(States::DISCONNECTED);
				if(mSocket->is_open())
				{
					// Documentation says to call shutdown then close
					// https://www.boost.org/doc/libs/1_82_0/doc/html/boost_asio/reference/basic_stream_socket/close/overload1.html
					boost::system::error_code ec;
					mSocket->shutdown(boost::asio::ip::tcp::socket::shutdown_both,ec);
					mSocket->close(ec);
				}

				mManager.UpdateConnect(shared_from_this());
				return true;
			}
			return false;
		}

	public:
		/**
		 * Constructor for connections to Connect();
		 */
		BoostTCPConnection(BoostASIONetworkSystem& manager, boost::asio::io_context& ioContext, const ConnectionDetails& connectionDetails) :
			Connection(manager.GetNetworkManager()),
			mManager(manager),
			mIOContext(ioContext),
			mSocket(new TCPSocket(mIOContext)),
			mResolver(new Resolver(mIOContext)),
			mInSendLoop(false),
			mAsyncWriting(false),
			mAsyncReading(false)
		{
			SetConnectionDetails(connectionDetails);
		}

		/**
		 * Constructor for sockets that have been established;
		 */
		BoostTCPConnection(BoostASIONetworkSystem& manager, boost::asio::io_context& ioContext) :
			Connection(manager.GetNetworkManager()),
			mManager(manager),
			mIOContext(ioContext),
			mInSendLoop(false),
			mAsyncWriting(false),
			mAsyncReading(false)
		{
		}

		~BoostTCPConnection()
		{
			_Disconnect();
			mManager._RemoveConnection(this);

			std::unique_lock< std::mutex > lock(mRunMutex);
			while(mAsyncReading || mAsyncWriting)
			{
				mRunWaiter.wait(lock,[this](){return (!mAsyncReading && !mAsyncReading);});
			}
		}

		/**
		 * Accept a connection and return a new connection for it.
		 * @note This will always return null on non-listen connections.
		 * @return Accepted connection ready to send or receive data.
		 */
		void Accept(shared_ptr<TCPSocket> socket)
		{
			ECHO_ASSERT(socket!=nullptr,"Socket must not be null");
			ECHO_LOG_DEBUG(this << " accepted");
			mSocket = socket;
			SetState(States::CONNECTED);
			mManager.UpdateConnect(shared_from_this());

			{
				std::unique_lock< std::mutex > syncLock(mRunMutex);
				mAsyncReading = true;
				mAsyncWriting = true;
			}
			// Start the async header+packet loops
			ReadHeader();
			ScopedLock lock(mQueuedPacketsMutex);
			SendNextPacket();
		}

		/**
		 * Send the next packet
		 * @note mQueuedPacketsMutex needs to be locked before calling this.
		 * This method should only be called as part of the send loop or to initiate it if the send loop is not currently active.
		 */
		void SendNextPacket()
		{
			mInSendLoop=true;
			if(!(mQueuedPackets.empty()))
			{
				auto& front = mQueuedPackets.front();
				// Write the header then write the packet
				mSendHeader.BuildForPacket(*front.first);
				ECHO_LOG_DEBUG(this << " Write header: " << front.first->GetPacketTypeID() << ":" << front.first->GetPacketID() << ":" << front.first->GetDataSize());
				boost::asio::async_write(*mSocket,
					boost::asio::buffer(mSendHeader.GetHeaderData(),mSendHeader.GetHeaderDataSizeInBytes()),
					bind(&BoostTCPConnection::WritePacket,this,placeholders::_1,placeholders::_2,front.first,front.second));
				mQueuedPackets.pop_front();
			}else
			{
				// If there aren't packets we can have the send triggered by the next call to SendPackets()
				mInSendLoop = false;

				std::unique_lock< std::mutex > lock(mRunMutex);
				// This is the async write loop, the async read loop should handle the read wait flag
				mAsyncWriting = false;
				mRunWaiter.notify_all();
			}
		}

		virtual SendStatus SendPackets(bool reenable) override
		{
			ScopedLock lock(mQueuedPacketsMutex);
			if(mInSendLoop)
			{
				return SendStatuses::WAIT;
			}
			SendNextPacket();
			return SendStatuses::SUCCESS;
		}

		SendResult Send(const u8* buffer, int) override
		{
			ECHO_LOG_ERROR(this << " Invalid use of BoostTCPConnection. BoostTCPConnection overrides SendPackets() to allow for easier queue management.");
			return SendResult{0,SendStatus::INTERNAL_FAILURE};
		}

		ReceiveResult Receive(u8* buffer, int bufferSizeInBytes) override
		{
			ECHO_LOG_ERROR(this << " Invalid use of BoostTCPConnection. BoostTCPConnection builds packets internally and passed them out to the system.");
			return ReceiveResult{0,ReceiveStatus::INTERNAL_FAILURE};
		}
	};
}
#endif
