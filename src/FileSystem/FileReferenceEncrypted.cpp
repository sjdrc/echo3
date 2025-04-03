#include <echo/FileSystem/FileReferenceEncrypted.h>

namespace Echo
{
	size_t FileReferenceEncrypted::Read(void* buffer, size_t typeSize, size_t numberToRead)
	{
		size_t numberOfBytesRead = mFile.Read(buffer,typeSize,numberToRead);
		u8* buff=reinterpret_cast<u8*>(buffer);
		for(size_t i=0;i<numberOfBytesRead;++i)		//Decrypt
		{
			u8 b=~(buff[i]);
			u8 ln=(b>>4)&0xF;
			u8 un=(b&0xF)<<4;
			buff[i]=ln|un;
		}
		mPosition+=numberOfBytesRead;
		return numberOfBytesRead;
	}

	size_t FileReferenceEncrypted::Write(const void* buffer, size_t typeSize, size_t numberToWrite)
	{
		//Copy the buffer to encrypt it
		size_t dataSize = typeSize * numberToWrite;
		u8* tempBuffer=new u8[dataSize];

		const u8* buff=reinterpret_cast<const u8*>(buffer);
		for(size_t i=0;i<dataSize;++i)		//Encrypt
		{
			u8 b=~(buff[i]);
			u8 ln=(b>>4)&0xF;
			u8 un=(b&0xF)<<4;
			tempBuffer[i]=ln|un;
		}

		size_t numberOfBytesWritten=mFile.Write(tempBuffer,typeSize,numberToWrite);
		mFileSize+=numberOfBytesWritten;
		mPosition+=numberOfBytesWritten;
		delete [] tempBuffer;
		return numberOfBytesWritten;
	}

	size_t FileReferenceEncrypted::Seek(size_t position)
	{
		return mFile.Seek(position);
	}

	bool FileReferenceEncrypted::EndOfFile()
	{
		return mFile.EndOfFile();
	}
}
