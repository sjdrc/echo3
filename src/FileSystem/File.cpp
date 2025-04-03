#include <echo/FileSystem/File.h>
#include <echo/FileSystem/FileSystem.h>
#include <echo/FileSystem/FileSystemSource.h>
#include <echo/FileSystem/FileReference.h>
#include <cstring>

namespace Echo
{
	File::File() :
		mEndianMode(EndianModes::UNKNOWN),
		mConvertEndian(false)
	{
	}

	File::File(const std::string& requestedFileName, const std::string& actualFileName) :
		mRequestedFileName(requestedFileName),
		mActualFileName(actualFileName),
		mEndianMode(EndianModes::UNKNOWN),
		mConvertEndian(false)
	{
	}

	File::File(const std::string& requestedFileName, const std::string& actualFileName, shared_ptr<FileReference> fileReference) :
		mRequestedFileName(requestedFileName),
		mActualFileName(actualFileName),
		mFileReference(fileReference),
		mEndianMode(EndianModes::UNKNOWN),
		mConvertEndian(false)
	{
	}
	
	File::~File()
	{
	}

	bool File::Close()
	{
		bool closed = false;
		if(mFileReference)
		{
			closed = mFileReference.unique();
			mFileReference.reset();
		}
		return closed;
	}

	size_t File::Read(void* buffer, size_t typeSize, size_t numberToGet)
	{
		if(mFileReference)
		{
			size_t bytesRead=mFileReference->Read(buffer,typeSize,numberToGet);
#ifdef ECHO_BIG_ENDIAN
			//If the mode is unknown then the logic is as per the mConvertEndian flag
			if(mConvertEndian && mEndianMode!=EndianModes::BIG)
#else
			if(mConvertEndian && mEndianMode!=EndianModes::LITTLE)
#endif
			{
				u8* byteBuffer=reinterpret_cast<u8*>(buffer);
				if(typeSize&1)
				{
					//Don't touch
				}else
				if(typeSize==2)
				{
					for(size_t i=0;i<bytesRead;i+=2)
					{
						std::swap(byteBuffer[i+1],byteBuffer[i]);
					}
				}else
				{
					for(size_t i=0;i<bytesRead;i+=4)
					{
						std::swap(byteBuffer[i+3],byteBuffer[i]);
						std::swap(byteBuffer[i+2],byteBuffer[i+1]);
					}
				}
			}
			return bytesRead;
		}
		return 0;
	}

	size_t File::Write(const std::string& s)
	{
		return Write(s.c_str(),s.length());
	}
	
	size_t File::Write(const char* s)
	{
		return Write(s,strlen(s));
	}
	
	size_t File::Write(const void* buffer, size_t typeSize, size_t numberToWrite)
	{
		if(mFileReference)
		{
#ifdef ECHO_BIG_ENDIAN
			//If the mode is unknown then the logic is as per the mConvertEndian flag
			if(mConvertEndian && mEndianMode!=EndianModes::BIG)
#else
			if(mConvertEndian && mEndianMode!=EndianModes::LITTLE)
#endif
			{
				size_t numBytes=typeSize*numberToWrite;
				const u8* byteBuffer=reinterpret_cast<const u8*>(buffer);
				if(typeSize&1)
				{
					//Don't touch
				}else
					if(typeSize==2)
					{
						u8* unConstBuffer=new u8[numBytes];
						for(u32 i=0;i<numBytes;i+=2)
						{
							unConstBuffer[i]=byteBuffer[i+1];
							unConstBuffer[i+1]=byteBuffer[i];
						}
						u32 bytesWritten=mFileReference->Write(unConstBuffer,typeSize,numberToWrite);
						delete [] unConstBuffer;
						return bytesWritten;
					}else
					{
						u8* unConstBuffer=new u8[numBytes];
						for(u32 i=0;i<numBytes;i+=4)
						{
							unConstBuffer[i+3]=byteBuffer[i];
							unConstBuffer[i+2]=byteBuffer[i+1];
							unConstBuffer[i+1]=byteBuffer[i+2];
							unConstBuffer[i]=byteBuffer[i+3];
						}
						u32 bytesWritten=mFileReference->Write(unConstBuffer,typeSize,numberToWrite);
						delete [] unConstBuffer;
						return bytesWritten;
					}
			}
			return mFileReference->Write(buffer,typeSize,numberToWrite);
		}
		return 0;
	}

	size_t File::Seek(size_t position)
	{
		if(mFileReference)
		{
			return mFileReference->Seek(position);
		}
		return 0;
	}

	size_t File::GetSize()
	{
		if(mFileReference)
		{
			return mFileReference->GetFileSize();
		}
		return 0;
	}

	size_t File::GetPosition() const
	{
		if(mFileReference)
		{
			return mFileReference->GetPosition();
		}
		return 0;
	}

	File::File(const File& rhs)
	{
		mRequestedFileName = rhs.mRequestedFileName;
		mActualFileName = rhs.mActualFileName;
		mFileReference = rhs.mFileReference;
		mEndianMode = rhs.mEndianMode;
		mConvertEndian = rhs.mConvertEndian;
	}

	bool File::operator==(const File& rhs) const
	{
		return(mFileReference==rhs.mFileReference);
	}

	File& File::operator=(const File& rhs)
	{
		if (this == &rhs)
			return *this;
		
		mRequestedFileName = rhs.mRequestedFileName;
		mActualFileName = rhs.mActualFileName;
		mEndianMode = rhs.mEndianMode;
		mConvertEndian = rhs.mConvertEndian;
		mFileReference = rhs.mFileReference;
		return *this;
	}

	void File::ReadLines(std::vector<std::string>& linesOut, bool cullSpaces, std::string ignoreLinesStartingWithOneOf)
	{
		if(!mFileReference)
		{
			return;
		}
		u8 *fileBuff;
		size_t fileSize = mFileReference->GetFileSize();
		size_t i=0;
		size_t line=0;
		char tempChar=0;
		std::string tempString;

		//Open the file, set aside some memory for it then load it into memory
		fileBuff = new u8[fileSize+1];
		fileBuff[fileSize]=0;
		Seek(0);
		Read(fileBuff,1,fileSize);
		Seek(0);

		tempChar=fileBuff[i];
		++i;

		while(i<fileSize)
		{
			//Get a line from buffer
			while(tempChar!='\n' && tempChar!=13 && tempChar!=0 && i<=fileSize)
			{
				if(tempString.empty() && ignoreLinesStartingWithOneOf.find_first_of(tempChar)!=std::string::npos)
				{
					//Comment ignore this part
					while(tempChar!='\n' && tempChar!=13 && tempChar!=0 && i<fileSize)
					{
						tempChar=fileBuff[i];
						++i;
					}
				}else
				{
					if(cullSpaces)
					{
						if(tempChar!=' ' && tempChar!='\t')
						{
							tempString+=tempChar;
						}
					}else
					{
						if(tempChar!='\t')
						{
							tempString+=tempChar;
						}
					}

					tempChar=fileBuff[i];
					++i;
				}
			}

			while((tempChar=='\n' || tempChar==13 || tempChar==0) && i<=fileSize)
			{
				tempChar=fileBuff[i];
				++i;
			}

			//Add the line
			linesOut.push_back(tempString);
			++line;

			tempString="";
		}
		delete [] fileBuff;
	}

	void File::ReadFileIntoString(std::string& outString)
	{
		outString.clear();
		Size bufferSize = GetSize();
		Seek(0);
#if __cplusplus > 201402L
		outString.resize(bufferSize);
		Read(outString.data(),1,bufferSize);
#else
		outString.reserve(bufferSize);
		char* buffer = new char[bufferSize];
		Size bytesRead = Read(buffer,1,bufferSize);
		outString.append(buffer,bytesRead);
		outString.resize(bytesRead);
		delete [] buffer;
#endif
	}		

	bool File::EndOfFile()
	{
		if(mFileReference)
		{
			return mFileReference->EndOfFile();
		}
		return true;
	}
}
