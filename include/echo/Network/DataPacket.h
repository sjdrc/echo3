#ifndef _DATAPACKET_H_
#define _DATAPACKET_H_
#include <echo/Types.h>
#include <echo/Util/Utils.h>
#include <string>
#include <vector>

namespace Echo
{
	class Connection;
	class DataPacket;

	class DataPacketHeader
	{
	private:
		friend class Connection;
		// Data is sent/received by two hosts as the header.
		//[0] - Packet Type ID Custom field for packet type
		//[1] - Packet ID
		//[2] - Length of data to come
		u32 mData[3];									// If this changes in size don't forget to update Connection::SendDataPacket().
		bool mIsBigEndian;
		void Reorder();									//Reorders the header data from one endian to the other
	public:
		DataPacketHeader();
		bool BuildFromPacketData(const DataPacket& packet);
		void BuildForPacket(const DataPacket& packet);
		const bool IsBigEndian() const					{return mIsBigEndian;}
		inline u8* GetHeaderData()						{return reinterpret_cast<u8*>(mData);}
		inline const u8* GetHeaderData() const			{return reinterpret_cast<const u8*>(mData);}
		inline u32 GetHeaderDataSizeInBytes()			{return SizeOfArray(mData)*sizeof(u32);}
		inline u32 GetPacketID() const					{return mData[1];}
		inline u32 GetPacketTypeID() const				{return mData[0];}
		inline void SetPacketTypeID(u32 id)				{mData[0]=id;}
		inline u32 GetDataLength()	const				{return mData[2];}
	};

	class DataPacket
	{
	protected:
		friend class Connection;
		u32 mPacketTypeID;								//Four bytes to specify the packet type - this is a custom field
		u32 mPacketID;									//Four bytes for the ID of the packet, this is assigned internally
														//and used for response callback processing.
		u32 mSize;										//Size of data
		u32 mReceived;									//Record of number of bytes received
		u32 mCapacity;									// The capacity of the DataPacket, this is used internally for
														// managing reallocations. In the future it might be used to allow
														// dynamically sized DataPackets.
		bool mIsBigEndian;
		u8* mData;										//The data
		
	public:
		const static Size NUMBYTES_FOR_STRING_HEADER;
		class SizeExceedsMaximumAllowedException{};
		DataPacket();
		DataPacket(u32 packetTypeID, u32 size);
		DataPacket(const std::vector< std::string >& content);
		DataPacket(const std::string& content);
		DataPacket(const DataPacketHeader& header);
		DataPacket(const DataPacket& packet);
		virtual ~DataPacket();

		/**
		 * Ensure the order of the given data.
		 * This method will compare the packet endian format against the platform format.
		 * The returned result is either the data passed in or the reordered value.
		 */
		inline u32 EnsureOrder(u32 data) const
		{
			#ifdef ECHO_LITTLE_ENDIAN
				if(mIsBigEndian)
				{
					u32 b1,b2,b3,b4;
					b1=data & 0xFF000000;
					b2=data & 0x00FF0000;
					b3=data & 0x0000FF00;
					b4=data & 0x000000FF;
					u32 reordered=(b1>>24) | (b2>>8) | (b3<<8) | (b4<<24);
					return reordered;
				}
			#else
				if(!mIsBigEndian)
				{
					u32 b1,b2,b3,b4;
					b1=data & 0xFF000000;
					b2=data & 0x00FF0000;
					b3=data & 0x0000FF00;
					b4=data & 0x000000FF;
					u32 reordered=(b1>>24) | (b2>>8) | (b3<<8) | (b4<<24);
					return reordered;
				}
			#endif
			return data;
		}
		inline u16 EnsureOrder(u16 data)
		{
			#ifdef ECHO_LITTLE_ENDIAN
				if(mIsBigEndian)
				{
					u16 b1,b2;
					b1=data & 0xFF00;
					b2=data & 0x00FF;
					u32 reordered=(b1>>8) | (b2<<8);
					return reordered;
				}
			#else
				if(!mIsBigEndian)
				{
					u16 b1,b2;
					b1=data & 0xFF00;
					b2=data & 0x00FF;
					u32 reordered=(b1>>8) | (b2<<8);
					return reordered;
				}
			#endif
			return data;
		}

		//Alternative method for setting up a DataPacket is to create
		//with the default constructor then call Configure. This is
		//designed to be used by Connection.
		void Configure(u32 packetTypeID, u32 size);
		void Configure(const std::string& content);
		void Configure(const std::vector< std::string >& content);
		void Configure(const std::string& label, const std::vector< std::string >& content);
		void Configure(const std::string& label, const std::string& content);
		void Configure(const std::string& label, u32 size);
		void Configure(const DataPacketHeader& header);

		/**
		 * Configure the data packet with a data buffer and size.
		 * @note Use SetPacketTypeID() to set the packet type ID.
		 * @note If you want your packet to be a labelled packet the labelled needs to exist at
		 * the start of the buffer. Use AppendString(std::string,u8*,u32,u32) first.
		 * @param dataBuffer The data packet that the packet will taken ownership of
		 * @param size the size of the DataPacket. This has to be less than or equal to the size of
		 * the passed buffer. The size is the amount of data that will be sent.
		 * @param emptySpaceAvailable The space available left in the buffer for writing data, if
		 * the dataBuffer is full then this value will be 0. This is used to setup internal write
		 * positions.
 		 */
		void ConfigureAndTakeData(u8* dataBuffer, u32 size, u32 emptySpaceAvailable);

		/**
		 * Check the internal buffer and ensure it has the specified dataSize.
		 * This method ensures that the internal buffer has the specified capacity. If the buffer
		 * has a capacity of the same size or larger then the buffer is unchanged.
		 * While the internal buffer will be allocated to fit the data specified by Configure(),
		 * this method can be used to set the capacity of the buffer to prevent an allocation when
		 * Configure() is called. This is useful for pools that want to avoid allocating memory
		 * frequently.
		 * @param dataSize the minimum capacity the buffer should have.
		 * @param keepExistingDataOnReallocate when true the data will be copied if the buffer
		 * needs to be resized.
		 */
		void SetDataSize(u32 dataSize, bool keepExistingDataOnReallocate);
		
		void SetBigEndian(bool isBigEndian)				{mIsBigEndian=isBigEndian;}
		bool IsBigEndian() const						{return mIsBigEndian;}

		void SetPacketTypeID(u32 packetTypeID)			{mPacketTypeID=packetTypeID;}
		u32 GetPacketTypeID() const						{return mPacketTypeID;}
		
		/**
		 * The packet ID is is used by Connection to manage packet responses.
		 * Connection will assign a packetID to a DataPacket which the remote end can use and assign
		 * to another packet which is then used to identify that packet as the response. Typically
		 * client code will not modify packet id's unless you are forwarding a packet to another
		 * destination, for which you would need to save then restore the packet ID of the packet
		 * being forwarded before you send a response to the originating connection.
		 */
		void SetPacketID(u32 packetD)					{mPacketID=packetD;}
		u32 GetPacketID() const							{return mPacketID;}

		u32 AppendData(const void* data, u32 size);		//Returns the number of bytes appended
		inline bool HasReceivedAllData() const			{return (mSize==mReceived);}
		inline u32 GetReceivedDataSize() const			{return mReceived;}
		inline u32 GetRemainingDataSize() const			{return (mSize-mReceived);}
		inline u32 GetDataSize() const					{return mSize;}
		inline u8* GetData()							{return mData;}
		inline const u8* GetData() const				{return mData;}

		/**
		 * Set the number of bytes received.
		 * @param bytesReceived The number of bytes received, must be <= the size of the packet.
		 */
		void SetBytesReceived(u32 bytesReceived);

		inline u8* GetDataAfterLabel()
		{
			if(IsLabelledPacket())
			{
				Size labelEnd = 0;
				GetLabel(&labelEnd);
				return &(mData[labelEnd]);
			}
			return mData;
		}
		
		inline const u8* GetDataAfterLabel() const
		{
			if(IsLabelledPacket())
			{
				Size labelEnd = 0;
				GetLabel(&labelEnd);
				return &(mData[labelEnd]);
			}
			return mData;
		}
		
		inline u8* GetDataAfterLabel(Size* dataLength)
		{
			if(IsLabelledPacket())
			{
				Size labelEnd = 0;
				GetLabel(&labelEnd);
				*dataLength = GetDataSize()-labelEnd;
				return &(mData[labelEnd]);
			}
			*dataLength = GetDataSize();
			return mData;
		}
		
		inline const u8* GetDataAfterLabel(Size* dataLength) const
		{
			if(IsLabelledPacket())
			{
				Size labelEnd = 0;
				GetLabel(&labelEnd);
				*dataLength = GetDataSize()-labelEnd;
				return &(mData[labelEnd]);
			}
			*dataLength = GetDataSize();
			return mData;
		}
		
		/**
		 * Get a value by accessing the packet data as though it is an array of the specified type.
		 * @note It is very important that you check that you are not going to try and access past
		 * the end of the data.
		 * @param index The index of the data as though the data was an array of the specified type,
		 * this is NOT a byte offset. To access a value at a given byte use GetAtByte<T>()
		 * @param defaultValue The default value if there isn't enough data to retreive the type.
		 * @param skipLabel If the packet is a labelled packet, whether or not to skip the label.
		 * @return The data at the index as though the data was a single array of T.
		 */
		template<typename T>
		inline T Get(Size index, T defaultValue, bool skipLabel=true) const
		{
			if(skipLabel && IsLabelledPacket())
			{
				return GetAtByte<T>(index * sizeof(T), skipLabel);
			}
			
			if(index < GetDataSize()/sizeof(T))
			{
				return reinterpret_cast<T*>(mData)[index];
			}
			return defaultValue;
		}

		/**
		 * Get a value at the specified byte.
		 * @note It is very important that you check that you are not going to try and access past
		 * the end of the data.
		 * @return The data at the specified byte interpreted as T.
		 */
		template<typename T>
		inline T GetAtByte(Size byteOffset, T defaultValue, bool skipLabel=true) const
		{
			if(skipLabel && IsLabelledPacket())
			{
				Size labelEnd = 0;
				GetLabel(&labelEnd);
				byteOffset+=labelEnd;
			}
			if(byteOffset < (GetDataSize() - sizeof(T)))
			{
				return *reinterpret_cast<T*>(&mData[byteOffset]);
			}
			return defaultValue;
		}
		
		inline u8* GetCurrentWritePointer()				{return &mData[mReceived];}
		inline bool SendHeaderOnly() const				{return (mData==0) || (mReceived==0);}

		//Store a string from an std::string. Format will be bytesPerChar(2)length(2)content(length).
		//If there is not enough space remaining to append the string it is not appended at all and
		//false is returned. Otherwise true.
		bool AppendString(const std::string& s);

		/**
		 * Append a string to a buffer using the internal send format.
		 * @param s the string to append.
		 * @param buffer The buffer to append the string to.
		 * @param offset The position in the buffer to put the string
		 * @param totalSize The total size of the buffer.
		 * @return false if there is not enough space available to append the string, otherwise true.
		 */
		static bool AppendString(const std::string& s, u8* buffer, u32 offset, u32 totalSize);

		/**
		 * Get a string from a data packet using the built in format
		 * @param outString A string to hold the value on success, it will be empty on failure.
		 * @param dataOffset An offset in the DataPacket to read from. If the packet is a labelled packet
		 * the offset is past the label.
		 * @param outDataStartOffset If set and the method succeeds, will contain the offset in the DataPacket
		 * that points to after the string.
		 * @param skipLabel If true and the packet is PacketTypes::LABELLED_PACKET the label will be skipped.
		 * @return true on success, false if there was a problem processing the data (e.g. incorrect format)
		 */
		bool GetStringFromDataPacket(std::string& outString, Size dataOffset=0, Size* outDataStartOffset=nullptr, bool skipLabel=true) const;
		
		/**
		 * Get std::strings from a data packet using the built in format
		 * If the packet is PacketTypes::LABELLED_PACKET the label will be skipped.
		 * @param outStrings The vector to write to, the container has all strings added to it but is not cleared before
		 * the method begins. Strings may be added to the vector even if the method returns false, if there is an error
		 * processing one of the strings in a multi-string packet.
		 * @param dataOffset The offset in the packet.
		 * @param skipLabel If true and the packet is PacketTypes::LABELLED_PACKET the label will be skipped.
		 * @return false if there were problems processing the content or there weren't any strings.
		 */
		bool GetStringFromDataPacket(std::vector<std::string>& outStrings, Size dataOffset=0, bool skipLabel=true) const;

		/**
		 * Determine whether the DataPacket a labelled packet or not.
		 */
		bool IsLabelledPacket() const;

		/**
		 * Determine whether the DataPacket a response packet.
		 * Response packets are assigned a response PacketType and not processed through registered callbacks.
		 */
		bool IsResponsePacket() const;
		
		/**
		 * Get the packet label.
		 * @param labelEnd if not null will have the position of the next byte after the label.
		 * @return the label or if the packet is not a labelled type the string will be empty.
		 */
		std::string GetLabel(Size* labelEnd = nullptr) const;
	};
}
#endif
