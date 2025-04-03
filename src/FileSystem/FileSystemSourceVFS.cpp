#include <echo/FileSystem/FileSystemSourceVFS.h>
#include <echo/FileSystem/FileReferenceVFS.h>
#include <echo/FileSystem/FileSystem.h>
#include <boost/foreach.hpp>
#include <algorithm>
#include <iostream>

namespace Echo
{
	#define SWAP_ENDIAN_4(x) ( ((x&0xFF000000)>>24) | ((x&0x00FF0000)>>8) | ((x&0x0000FF00)<<8) | ((x&0x000000FF)<<24))

	FileSystemSourceVFS::FileSystemSourceVFS(FileSystem& vfsLoaderFileSystem) : FileSystemSource("vfs"),
			EVFS_VERSION(0xEC400002),
			EVFS_VERSION_SWITCH(SWAP_ENDIAN_4(EVFS_VERSION)),
			mVFSLoaderFileSystem(vfsLoaderFileSystem)
	{
	}

	FileSystemSourceVFS::~FileSystemSourceVFS()
	{
		UnloadVFS();
	}

	//////////////////////////////////////////////////////////////////////////
	//VFS Source functionality
	bool FileSystemSourceVFS::LoadVFS(const std::string& vfsFile)
	{
		mVFSFile = mVFSLoaderFileSystem.Open(vfsFile);
		if(!mVFSFile.IsOpen())
		{
			ECHO_LOG_ERROR("VFS Failed to open: " << vfsFile);
			return false;
		}

		mVFSFile.Read(&mVFSVersion,4,1);

		if(mVFSVersion==EVFS_VERSION)
		{
			ECHO_LOG_INFO("VFS Version: 0x" << std::hex << mVFSVersion << std::dec << " Native to build" );
		}else
		if(mVFSVersion==EVFS_VERSION_SWITCH)
		{
			ECHO_LOG_INFO("VFS Version: 0x" << std::hex << mVFSVersion << std::dec << " Opposite Endian format. Enabling endian conversion.");
			mVFSFile.SetEndianConversionEnabled(true);
		}else
		{
			ECHO_LOG_ERROR("Wrong VFS version: 0x" << std::hex << mVFSVersion << std::dec);
			return 0;
		}

		u32 vfsHeaderSize=0;
		mVFSFile.Read(&vfsHeaderSize,4,1);

		ECHO_LOG_DEBUG("VFS Header Size: " << vfsHeaderSize);

		//-------- Header ---------------
		//Check a file with the same name as the new file does not already exist in the vfs
		if(vfsHeaderSize>8)
		{		
			size_t entrySearchPosition=8;
			ECHO_LOG_DEBUG("-= Entries =-");
			vfsHeaderSize-=8;
			for(entrySearchPosition=0; entrySearchPosition < vfsHeaderSize; )
			{
				u32 entryNameSize=0;

				VFSEntry fse;
				mVFSFile.Read(&fse.mMainOffset,4,1);
				mVFSFile.Read(&fse.mDataSize,4,1);
				mVFSFile.Read(&entryNameSize,4,1);
				u8* nameBuffer=new u8[entryNameSize+1];
				mVFSFile.Read(nameBuffer,1,entryNameSize);
				nameBuffer[entryNameSize]=0;
				std::string entryName;
				entryName=(char*)nameBuffer;

				//12 bytes before name + name length
				entrySearchPosition+=12+entryNameSize;
				ECHO_LOG_DEBUG("Entry: " << entryName << " " << fse.mDataSize << " bytes at " << std::hex << fse.mMainOffset << std::dec);
				mDirectory[entryName]=fse;
			}
			//delete [] vfsHeader;
			mVFSFile.SetEndianConversionEnabled(false);
			return true;
		}
		mVFSFile.SetEndianConversionEnabled(false);
		return false;
	}

	void FileSystemSourceVFS::UnloadVFS()
	{
		mVFSFile.Close();
		mDirectory.clear();
	}

	bool FileSystemSourceVFS::AddFile(std::string targetName, const std::string& source)
	{
		if(mOutDirectory.find(targetName)==mOutDirectory.end())
		{
			mOutDirectory[targetName] = source;
			return true;
		}
		return false;
	}

	bool FileSystemSourceVFS::AddFile(const std::string& f)
	{
		return AddFile(f,f);
	}

	void FileSystemSourceVFS::ClearOutDirectory()
	{
		mOutDirectory.clear();
	}

	bool FileSystemSourceVFS::SaveVFS(const std::string& vfsFile, VFSEndianMode endianMode, VFSEntryNameMode entryNameMode)
	{
		if(!GetFileSystem())
		{
			ECHO_LOG_ERROR("VFS source must be installed to a FileSystem before it can write. The FileSystem is used as the source of files when writing.");
			return false;
		}
		//Open each file. Create a VFSOutEntry for each file in the the directory.
		std::list< VFSOutEntry > entries;
		u32 dataSize=0;
		u32 numErrors=0;
		u32 headerSize=8;

		{
			for(auto& it : mOutDirectory)
			{
				const std::string& targetName = it.first;
				const std::string& sourceName = it.second;
				File touch = mVFSLoaderFileSystem.Open(sourceName);
				if(touch.IsOpen())
				{
					VFSOutEntry entry;
					entry.mFileIncSource=sourceName;
					std::string resolvedName;
					if(targetName!=sourceName)
					{
						entry.mTargetFileName = targetName;
					}else
					{
						// The source and target are the same so we have to make sure we remove "source://"
						GetFileSystem()->ResolveSource(sourceName,entry.mTargetFileName);
					}
					if(entryNameMode&VFSEntryNameModes::FULL)
					{
						headerSize+=(u32)entry.mTargetFileName.length();
						headerSize+=12;
					}
					if(entryNameMode&VFSEntryNameModes::FILE_NAME)
					{
						size_t i=entry.mTargetFileName.find_last_of('/');
						if (i!=std::string::npos)
						{
							headerSize+=entry.mTargetFileName.substr(i+1, entry.mTargetFileName.size()-i-1).length();
						}else
						{
							headerSize+=entry.mTargetFileName.length();
						}
						headerSize+=12;
					}
					entry.mMainOffset=dataSize;
					entry.mDataSize=touch.GetSize();
					dataSize+=entry.mDataSize;
					entries.push_back(entry);
					touch.Close();
				}else
				{
					ECHO_LOG_ERROR("Cannot open " << sourceName);
					numErrors++;
				}
			}
		}

		//Now we need to write everything
		File vfsOut = mVFSLoaderFileSystem.Open(vfsFile, File::OpenModes::WRITE);
		if(!vfsOut.IsOpen())
		{
			ECHO_LOG_ERROR("Unable to open " << vfsFile);
			return false;
		}
		switch(endianMode)
		{
			case VFSEndianModes::DEFAULT:
			case VFSEndianModes::LITTLE:
	#ifdef ECHO_BIG_ENDIAN
				vfsOut.SetEndianConversionEnabled(true);
	#endif
			break;
			case VFSEndianModes::BIG:
	#ifdef ECHO_LITTLE_ENDIAN
				vfsOut.SetEndianConversionEnabled(true);
	#endif
			break;
		}

		//Write the version
		u32 version=EVFS_VERSION;
		vfsOut.Write(&version,4,1);
		//Write the header size
		vfsOut.Write(&headerSize,4,1);

		//Write the header
		{
			std::list< VFSOutEntry >::iterator it=entries.begin();
			std::list< VFSOutEntry >::iterator itEnd=entries.end();
			while(it!=itEnd)
			{
				VFSOutEntry& e=(*it);
				e.mMainOffset+=headerSize;			
				u32 entryNameSize=0;
				if(entryNameMode&VFSEntryNameModes::FULL)
				{
					entryNameSize=(u32)it->mTargetFileName.length();
					if(vfsOut.Write(&(it->mMainOffset),4,1)!=4)
					{
						numErrors++;
					}
					if(vfsOut.Write(&(it->mDataSize),4,1)!=4)
					{
						numErrors++;
					}
					if(vfsOut.Write(&entryNameSize,4,1)!=4)
					{
						numErrors++;
					}
					if(vfsOut.Write(reinterpret_cast<const void*>(it->mTargetFileName.c_str()),1,(u32)it->mTargetFileName.length())!=(u32)it->mTargetFileName.length())
					{
							numErrors++;
					}
				}
				if(entryNameMode&VFSEntryNameModes::FILE_NAME)
				{
					std::string name=it->mTargetFileName;
					size_t i=name.find_last_of('/');
					if (i!=std::string::npos)
					{
						name=name.substr(i+1, name.size()-i-1);
					}
					entryNameSize=(u32)name.length();
					if(vfsOut.Write(&(it->mMainOffset),4,1)!=4)
					{
						numErrors++;
					}
					if(vfsOut.Write(&(it->mDataSize),4,1)!=4)
					{
						numErrors++;
					}
					if(vfsOut.Write(&entryNameSize,4,1)!=4)
					{
						numErrors++;
					}
					if(vfsOut.Write(reinterpret_cast<const void*>(name.c_str()),1,(u32)name.length())!=(u32)name.length())
					{
						numErrors++;
					}
				}

				++it;
			}
		}

		//Write the file data
		{
			u32 bufferSize=2097152;	//2 Meg buffer
			u8* buffer=new u8[bufferSize];
			bool goneInvalid=false;
			std::list< VFSOutEntry >::iterator it=entries.begin();
			std::list< VFSOutEntry >::iterator itEnd=entries.end();
			while(it!=itEnd)
			{
				ECHO_LOG_DEBUG("\"" << it->mFileIncSource << "\"");
				if(goneInvalid)
				{
					ECHO_LOG_ERROR("\"" << it->mFileIncSource << "\" not added.");
					numErrors++;
				}else
				{
					File dataFile = mVFSLoaderFileSystem.Open(it->mFileIncSource);
					if(dataFile.IsOpen())
					{
						u32 segments=it->mDataSize/bufferSize;
						u32 remainder=it->mDataSize%bufferSize;
						for(u32 s=0;s<segments;++s)
						{
							u32 bytesRead=dataFile.Read(buffer,1,bufferSize);
							if(bytesRead<bufferSize)
							{
								ECHO_LOG_ERROR(bytesRead <<" bytes read of " << bufferSize << " ( "<< it->mDataSize <<" total) in chunk " << (s+1) << " of " << (segments+1) << " before write. " << it->mFileIncSource << " will be invalid.");
								numErrors++;
							}
							if(vfsOut.Write(buffer,1,bufferSize)!=bufferSize)
							{
								numErrors++;
							}
						}
						if(remainder>0)
						{
							u32 bytesRead=dataFile.Read(buffer,1,remainder);
							if(bytesRead<remainder)
							{
								numErrors++;
								ECHO_LOG_ERROR(bytesRead << " bytes read of " << remainder << " ( "<< it->mDataSize <<" total) in chunk " << (segments+1) << " of " << (segments+1) << " before write. " << it->mFileIncSource << " will be invalid");
							}
							if(vfsOut.Write(buffer,1,remainder)!=remainder)
								numErrors++;
						}
						dataFile.Close();
					}else
					{
						goneInvalid=true;
						numErrors++;
						ECHO_LOG_ERROR("Unable to open \"" << it->mFileIncSource << "\"");
						ECHO_LOG_ERROR("The following files are indicated in the header but are not valid.");
						ECHO_LOG_ERROR("\"" << it->mFileIncSource << "\" not added.");
					}
				}
				++it;
			}
			delete [] buffer;
			vfsOut.Close();
		}
		if(entries.size() < mOutDirectory.size())
		{
			ECHO_LOG_ERROR("Not all files were added. " << entries.size() << " of " << mOutDirectory.size());
			numErrors++;
		}else
		{
			ECHO_LOG_DEBUG("Added " << entries.size() << " files ");
		}
		ECHO_LOG_DEBUG("VFS Creation complete");
		if(numErrors)
		{
			ECHO_LOG_ERROR("VFS has "<< numErrors << " error" << ((numErrors>1) ? "s" : ""));
		}
		return true;
	}

	File FileSystemSourceVFS::_Open(const std::string& originalFileName, const std::string& modifiedFileName, File::OpenMode openMode)
	{
		if(!mVFSFile.IsOpen() || openMode!=File::OpenModes::READ)
		{
			return CreateFile(originalFileName,modifiedFileName);
		}

		std::map< std::string, VFSEntry >::iterator it=mDirectory.find(modifiedFileName);
		if(it==mDirectory.end())
		{
			return CreateFile(originalFileName,modifiedFileName);
		}
		return CreateFile(originalFileName,modifiedFileName,shared_ptr<FileReferenceVFS>(new FileReferenceVFS(this,it->second)));
	}
}
