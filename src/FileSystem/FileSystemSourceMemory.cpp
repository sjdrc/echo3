#include <echo/FileSystem/FileSystemSourceMemory.h>
#include <echo/FileSystem/FileReferenceMemory.h>
#include <iostream>

namespace Echo
{
	FileSystemSourceMemory::FileSystemSourceMemory(const std::string& identifier) : FileSystemSource(identifier)
	{
	}

	FileSystemSourceMemory::~FileSystemSourceMemory()
	{

	}
	
	File FileSystemSourceMemory::OpenDirect(void* address, Size dataSize)
	{
		return File("","",shared_ptr<FileReferenceMemory>(new FileReferenceMemory(nullptr,address,dataSize)));
	}

	File FileSystemSourceMemory::OpenDirect(const void* address, Size dataSize)
	{
		return File("","",shared_ptr<FileReferenceMemory>(new FileReferenceMemory(nullptr,address,dataSize)));
	}

	File FileSystemSourceMemory::_Open(const std::string& originalFileName, const std::string&, File::OpenMode openMode)
	{
		if(openMode!=File::OpenModes::READ)
		{
			return File();
		}

		//format is
		//address:size
		size_t colon=originalFileName.find_first_of(':');
		size_t dataSize=0;
		//cull the size portion
		std::string fileAddress;
		if(colon!=std::string::npos)
		{
			std::string dataSizeString=originalFileName.substr(colon+1);
			dataSize=atoi(dataSizeString.c_str());
			fileAddress=originalFileName.substr(0,colon);
		}else
		{
			fileAddress=originalFileName;
		}

		std::stringstream iss(fileAddress);
		Size addr;
		if(fileAddress.substr(0,2)=="0x")
		{
			iss << std::hex;
		}
		iss >> addr;

		if(addr==0)
		{
			ECHO_LOG_ERROR("FileSystemSourceMemory: " << fileAddress << " > 0x" << std::hex << addr << ": 0x" << dataSize << std::dec << " invalid address.");
			return CreateFile(originalFileName,originalFileName);
		}
		ECHO_LOG_DEBUG("FileSystemSourceMemory: " << fileAddress << " > 0x" << std::hex << addr << ": 0x" << dataSize << std::dec);
		return CreateFile(originalFileName,originalFileName,shared_ptr<FileReferenceMemory>(new FileReferenceMemory(this,reinterpret_cast<void*>(addr),dataSize)));
	}
}
