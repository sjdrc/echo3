#include <echo/Network/DataPacket.h>
#include <echo/Network/NetworkManager.h>
#include <echo/Network/Connection.h>
#include <echo/Network/NetworkManager.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

namespace Echo
{
	DataPacketHeader::DataPacketHeader()
	{
		mData[0]=0;
		mData[1]=0;
		mData[2]=0;
		#ifdef ECHO_LITTLE_ENDIAN
		mIsBigEndian = false;
		#else
		mIsBigEndian = true;
		#endif
	}

	bool DataPacketHeader::BuildFromPacketData(const DataPacket& packet)
	{
		if(packet.GetDataSize()!=GetHeaderDataSizeInBytes())
		{
			ECHO_LOG_ERROR("Invalid packet data size when trying to build header from data. The packet size should be " << GetHeaderDataSizeInBytes() << " bytes.");
			return false;
		}

		mData[0]=packet.Get<u32>(0,0);
		mData[1]=packet.Get<u32>(1,0);
		mData[2]=packet.Get<u32>(2,0);
		mIsBigEndian = packet.IsBigEndian();
		if(mIsBigEndian)
		{
			Reorder();
		}
		return true;
	}

	void DataPacketHeader::BuildForPacket(const DataPacket& packet)
	{
		mData[0]=packet.GetPacketTypeID();
		mData[1]=packet.GetPacketID();
		mData[2]=packet.GetDataSize();
	}
	
	void DataPacketHeader::Reorder()
	{
		//The header is fine	
		u32 b1,b2,b3,b4;
		b1=mData[0] & 0xFF000000;
		b2=mData[0] & 0x00FF0000;
		b3=mData[0] & 0x0000FF00;
		b4=mData[0] & 0x000000FF;
		mData[0]=(b1>>24) | (b2>>8) | (b3<<8) | (b4<<24);

		b1=mData[1] & 0xFF000000;
		b2=mData[1] & 0x00FF0000;
		b3=mData[1] & 0x0000FF00;
		b4=mData[1] & 0x000000FF;
		mData[1]=(b1>>24) | (b2>>8) | (b3<<8) | (b4<<24);

		b1=mData[2] & 0xFF000000;
		b2=mData[2] & 0x00FF0000;
		b3=mData[2] & 0x0000FF00;
		b4=mData[2] & 0x000000FF;
		mData[2]=(b1>>24) | (b2>>8) | (b3<<8) | (b4<<24);
	}
	
	DataPacket::DataPacket()
	{
		mPacketTypeID=0;
		mPacketID=0;
		mSize=0;
		mCapacity=0;
		mReceived=0;
		mIsBigEndian=Connection::IsPlatformBigEndian();
		mData=0;
	}

	DataPacket::DataPacket(const std::string& content)
	{
		mSize=(u32)(content.length() + NUMBYTES_FOR_STRING_HEADER);
		mCapacity=mSize;
		mReceived=0;
		mPacketTypeID=0;
		mPacketID=0;
		mData=0;
		mIsBigEndian=Connection::IsPlatformBigEndian();

		mData=new u8[mSize];
		AppendString(content);
	}

	DataPacket::DataPacket(const std::vector< std::string >& content)
	{
		u32 requiredSize=0;
		for(Size i=0;i<content.size();++i)
		{
			requiredSize+=content[i].length() + NUMBYTES_FOR_STRING_HEADER;
		}
		mSize=requiredSize;
		mCapacity=mSize;
		mReceived=0;
		mPacketTypeID=0;
		mPacketID=0;
		mData=0;
		mIsBigEndian=Connection::IsPlatformBigEndian();

		if(mSize)
		{
			mData=new u8[mSize];
		}
		for(Size i=0;i<content.size();++i)
		{
			AppendString(content[i]);
		}
	}

	DataPacket::DataPacket(u32 packetTypeID, u32 size)
	{
		mSize=size;
		mCapacity=mSize;
		mReceived=0;
		mPacketTypeID=packetTypeID;
		mPacketID=0;
		mData=0;
		mIsBigEndian=Connection::IsPlatformBigEndian();

		if(mSize)
		{
			mData=new u8[mSize];
		}
	}

	DataPacket::DataPacket(const DataPacketHeader& header)
	{
		mSize=header.GetDataLength();
		mCapacity=mSize;
		mPacketTypeID=header.GetPacketTypeID();
		mPacketID=0;
		mReceived=0;
		mData=0;
		mIsBigEndian=header.IsBigEndian();
		if(mSize)
		{
			mData=new u8[mSize];
		}
	}

	DataPacket::DataPacket(const DataPacket& packet)
	{
		mPacketTypeID=packet.mPacketTypeID;
		mPacketID=packet.mPacketID;
		mSize=packet.mSize;
		mCapacity=mSize;
		mIsBigEndian=Connection::IsPlatformBigEndian();
		mReceived=0;	//We will append it in a moment
		mData=0;

		if(mSize)
		{
			mData=new u8[mSize];
			AppendData(packet.mData,mSize);
		}
	}

	DataPacket::~DataPacket()
	{
		delete [] mData;
		mData=0;
	}

	void DataPacket::Configure(const std::string& content)
	{
		SetDataSize((u32)(content.length() + NUMBYTES_FOR_STRING_HEADER),false);
		mReceived=0;
		mPacketTypeID=0;
		mIsBigEndian=Connection::IsPlatformBigEndian();
		
		AppendString(content);
	}

	void DataPacket::Configure(const std::string& label, const std::vector< std::string >& content)
	{
		u32 requiredSize=label.length() + NUMBYTES_FOR_STRING_HEADER;
		for(Size i=0;i<content.size();++i)
		{
			requiredSize+=content[i].length() + NUMBYTES_FOR_STRING_HEADER;
		}
		Configure(Connection::PacketTypes::LABELLED_PACKET,requiredSize);
		AppendString(label);
		for(Size i=0;i<content.size();++i)
		{
			AppendString(content[i]);
		}
	}
	
	void DataPacket::Configure(const std::string& label, const std::string& content)
	{
		Configure(Connection::PacketTypes::LABELLED_PACKET,label.length()+content.length()+DataPacket::NUMBYTES_FOR_STRING_HEADER*2);
		AppendString(label);
		AppendString(content);
	}
	
	void DataPacket::Configure(const std::string& label, u32 size)
	{
		Configure(Connection::PacketTypes::LABELLED_PACKET,label.length()+size+DataPacket::NUMBYTES_FOR_STRING_HEADER);
		AppendString(label);
	}

	void DataPacket::Configure(const std::vector< std::string >& content)
	{
		u32 requiredSize=0;
		for(Size i=0;i<content.size();++i)
		{
			requiredSize+=(u32)(content[i].length() + NUMBYTES_FOR_STRING_HEADER);
		}
		
		SetDataSize(requiredSize,false);
		mReceived=0;
		mPacketTypeID=0;
		mIsBigEndian=Connection::IsPlatformBigEndian();
		for(Size i=0;i<content.size();++i)
		{
			AppendString(content[i]);
		}
	}
	
	void DataPacket::Configure(u32 packetTypeID, u32 size)
	{
		SetDataSize(size,false);
		mReceived=0;
		mPacketTypeID=packetTypeID;
		mIsBigEndian=Connection::IsPlatformBigEndian();
	}

	void DataPacket::Configure(const DataPacketHeader& header)
	{
		SetDataSize(header.GetDataLength(),false);
		mPacketTypeID=header.GetPacketTypeID();
		mPacketID = header.GetPacketID();
		mReceived=0;
		mIsBigEndian=header.IsBigEndian();
	}
	
	void DataPacket::ConfigureAndTakeData(u8* dataBuffer, u32 size, u32 emptySpaceAvailable)
	{
		delete [] mData;
		mData = dataBuffer;
		mSize=size;
		mCapacity = mSize;
		mPacketTypeID=0;
		assert(emptySpaceAvailable <= mSize && "emptySpaceAvailable should not exceed the specified size");
		mReceived=mSize-emptySpaceAvailable;
		mIsBigEndian=Connection::IsPlatformBigEndian();
	}

	void DataPacket::SetDataSize(u32 newSize, bool keepExistingDataOnReallocate)
	{
		if(mCapacity<newSize)
		{
			if(keepExistingDataOnReallocate)
			{
				u8* newData = new u8[newSize];
				std::copy(mData,mData+newSize,newData);
				delete [] mData;
				mData = newData;
			}else
			{
				delete [] mData;
				mData = new u8[newSize];
			}
			mCapacity = newSize;
		}else
		{
			if(!keepExistingDataOnReallocate)
			{
				mReceived = 0;
			}
		}
		mSize = newSize;
	}
	
	void DataPacket::SetBytesReceived(u32 bytesReceived)
	{
		ECHO_ASSERT((bytesReceived <= mSize), "The number of bytes received must not exceed the size of the DataPacket buffer");
		if(bytesReceived <= mSize)
		{
			mReceived = bytesReceived;
		}else
		{
			ECHO_LOG_ERROR("The number of bytes received must not exceed the size of the DataPacket buffer: " << bytesReceived << " > " << mSize);
		}
	}
	
	u32 DataPacket::AppendData(const void* data, u32 size)		//Returns the number of bytes appended
	{
		if(size>mSize)
		{
			size=mSize-mReceived;
		}
		u32 remaining=mSize-mReceived;
		if(size>remaining)
		{
			size=remaining;
		}

		if(size==0 || !mData)
			return 0;

		//Copy the data over to our buffer
		const u8* byteData = reinterpret_cast<const u8*>(data);
		std::copy(byteData,byteData+size,&(mData[mReceived]));

		mReceived+=size;
		assert(mReceived<=mSize);
		return size;
	}

	const Size DataPacket::NUMBYTES_FOR_STRING_HEADER = sizeof(u32)*2;
	
	//Store a string from an std::string. Format will be bytesPerChar(4)length(4)content(length).
	//If there is not enough space remaining to append the string it is not appended at all and
	//false is returned. Otherwise true.
	bool DataPacket::AppendString(const std::string& s)
	{
		u32 l=(u32)s.length();

		//+NUMBYTES_FOR_STRING_HEADER for [bytes per char][length]
		if(l+NUMBYTES_FOR_STRING_HEADER>GetRemainingDataSize())
			return false;

		u32 one=1;
		AppendData(reinterpret_cast<const u8*>(&one),sizeof(u32));
		AppendData(reinterpret_cast<const u8*>(&l),sizeof(u32));
		AppendData(reinterpret_cast<const u8*>(s.c_str()),l);
		return true;
	}
	
	bool DataPacket::AppendString(const std::string& s, u8* buffer, u32 offset, u32 totalSize)
	{
		assert(offset < totalSize && "Cannot have an offset greater than the total buffer size");
		if(offset >= totalSize)
		{
			return false;
		}
		u32 l=(u32)s.length();

		//+NUMBYTES_FOR_STRING_HEADER for [bytes per char][length]
		u32 availableBytes = totalSize-offset;
		if(l+NUMBYTES_FOR_STRING_HEADER>availableBytes)
		{
			return false;
		}
		buffer+=offset;

		u32 one=1;
		const u8* onePtr = reinterpret_cast<const u8*>(&one);
		const u8* lPtr = reinterpret_cast<const u8*>(&l);
		const u8* sPtr = reinterpret_cast<const u8*>(s.c_str());
		std::copy(onePtr,onePtr+sizeof(u32),buffer);
		buffer+=sizeof(u32);
		std::copy(lPtr,lPtr+sizeof(u32),buffer);
		buffer+=sizeof(u32);
		std::copy(sPtr,sPtr+l,buffer);
		return true;
	}

	bool DataPacket::GetStringFromDataPacket(std::string& outString, Size dataOffset, Size* outDataStartOffset, bool skipLabel) const
	{
		u32 dataSize=GetDataSize();
		if(dataSize<=sizeof(u32)*2)
		{
			//Bad packet
			return false;
		}
		
		Size position=dataOffset;
		
		if(skipLabel && IsLabelledPacket())
		{
			Size labelEnd = 0;
			GetLabel(&labelEnd);
			position += labelEnd;
		}
		
		// Find out the bytes per character.
		//Internal format is as follows: bytesPerChar(4)length(4)content(length)
		
		u32 bytesPerChar=*reinterpret_cast<u32*>(&(mData[position]));
		bytesPerChar = EnsureOrder(bytesPerChar);
		
		// We only permit 1 byte per character at the moment.
		if(bytesPerChar>1)
		{
			return false;
		}
		position+=sizeof(u32);
		
		//Get the length
		u32 length=*reinterpret_cast<u32*>(&(mData[position]));
		length = EnsureOrder(length);
		
		// Check the length
		position+=sizeof(u32);
		if(length>(dataSize-position))
		{
			return false;
		}
		
		// Copy the string data.
		const char* data=reinterpret_cast<const char*>(&(mData[position]));
		outString = std::string(data,length*bytesPerChar);
		if(outDataStartOffset)
		{
			*outDataStartOffset = position+length*bytesPerChar;
		}
		return true;
	}
	
	bool DataPacket::GetStringFromDataPacket(std::vector<std::string>& outStrings, Size dataOffset, bool skipLabel) const
	{
		u32 ps=GetDataSize();
		if(ps<=sizeof(u32))
		{
			//Bad packet
			return false;
		}
		bool isLabelled = IsLabelledPacket();
		bool skippedLabel = false;
		u32 position=dataOffset;
		while(position<ps)
		{
			//bytesPerChar(4)length(4)content(length)
			u32 bytesPerChar=*reinterpret_cast<u32*>(&(mData[position]));
			bytesPerChar = EnsureOrder(bytesPerChar);
			//Reorder 
			if(bytesPerChar>1)
			{
				return false;
			}
			position+=sizeof(u32);
			u32 length=*reinterpret_cast<u32*>(&(mData[position]));
			length = EnsureOrder(length);
			position+=sizeof(u32);
			if(length<=0 || length>(mSize-position))
			{
				return false;
			}
			const char* data=reinterpret_cast<const char*>(&(mData[position]));
			std::string currentString;
			for(u32 b=0;b<length;++b)
			{
				currentString+=data[b];
				position+=bytesPerChar;
			}
			if(isLabelled)
			{
				if(skipLabel && skippedLabel)
				{
					outStrings.push_back(currentString);
				}
				skippedLabel = true;
			}else
			{
				outStrings.push_back(currentString);
			}
		}
		if(outStrings.empty())
		{
			return false;
		}
		return true;
	}
	
	bool DataPacket::IsLabelledPacket() const
	{
		return mPacketTypeID==Connection::PacketTypes::LABELLED_PACKET || mPacketTypeID==Connection::PacketTypes::LABELLED_RESPONSE_PACKET;
	}

	bool DataPacket::IsResponsePacket() const
	{
		return mPacketTypeID==Connection::PacketTypes::RESPONSE_PACKET || mPacketTypeID==Connection::PacketTypes::LABELLED_RESPONSE_PACKET;
	}

	std::string DataPacket::GetLabel(Size* labelEnd) const
	{
		std::string label;
		if(IsLabelledPacket() && GetStringFromDataPacket(label,0,labelEnd,false))
		{
			return label;
		}
		return std::string();
	}
}
