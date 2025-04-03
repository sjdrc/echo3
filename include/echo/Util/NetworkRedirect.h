#ifndef _ECHONETWORKREDIRECT_H_
#define _ECHONETWORKREDIRECT_H_

#include <echo/Network/Connection.h>
#include <echo/Kernel/Mutex.h>

namespace Echo
{
	class RedirectBuffer : public std::streambuf
	{
	public:
		RedirectBuffer(){}
		virtual ~RedirectBuffer(){}
		virtual void SetFlushChar(char c) = 0;
	};
	
	/**
	 * When used as a redirect buffer this class sends network messages labelled "networkstream" at every flush character.
	 */
	class NetworkStreamBuffer : public RedirectBuffer
	{
	public:
		/**
		 * Constructor
		 * @param connection the connection to send the networkstream packets to.
		 */
		NetworkStreamBuffer(shared_ptr<Connection> connection) :
			mFlushChar('\n'),
			mSending(false),
			mConnection(connection),
			mSecondaryOut(nullptr),
			mOriginalOut(nullptr)
		{}
		~NetworkStreamBuffer()
		{
			RevertToOriginalOut();
		}

		/**
		 * Set the character that the buffer will flush.
		 * When c is detected in the stream the buffer will flush.
		 * If c==-127 a network flush occurs every character. This is not recommended as it increases network overhead.
		 */
		void SetFlushChar(char c)
		{
			mFlushChar=c;
		}
		void SetSecondaryOut(std::streambuf* secondaryOutputStream)
		{
			mSecondaryOut=secondaryOutputStream;
		}
		void SetOriginalOut(std::streambuf* originalOutputStream)
		{
			mOriginalOut=originalOutputStream;
		}
		void RevertToOriginalOut()
		{
			if(mOriginalOut)
			{
				std::cout.rdbuf(mOriginalOut);
			}
		}
	private:
		typedef	std::basic_streambuf< char, std::char_traits< char > >::int_type int_type;
		typedef std::char_traits< char > traits_t;
		
		int_type overflow( int_type c )
		{
			if(mSecondaryOut)
			{
				mSecondaryOut->sputc(traits_t::to_char_type(c));
			}
			if(mSending)
			{
				mPendingPacket+=traits_t::to_char_type(c);
			}else
			{
				mCurrentPacket+=traits_t::to_char_type(c);
				if(c==(int_type)mFlushChar || mFlushChar==-127)
				{
					mSendingMutex.Lock();
					mSending=true;
					mConnection->SendLabelledPacket("networkstream",mCurrentPacket);
					mCurrentPacket=mPendingPacket;
					mPendingPacket.clear();
					mSending=false;
					mSendingMutex.Unlock();
				}
			}
			return c;
		}
		char mFlushChar;
		bool mSending;
		std::string mCurrentPacket;
		std::string mPendingPacket;
		shared_ptr<Connection> mConnection;
		Mutex mSendingMutex;
		std::streambuf* mSecondaryOut;
		std::streambuf* mOriginalOut;
	};
}

#endif
