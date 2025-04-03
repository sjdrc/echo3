#ifndef _ECHOFILESYSTEMSOURCEVFS_H_
#define _ECHOFILESYSTEMSOURCEVFS_H_
#include <echo/Types.h>
#include <string>
#include <map>
#include <list>
#include <echo/FileSystem/File.h>
#include <echo/FileSystem/FileSystemSource.h>

namespace Echo
{
	class FileSystemEntry;

	class FileSystemSourceVFS : public FileSystemSource
	{
	public:
		struct VFSEndianModes
		{
			enum _{DEFAULT,BIG,LITTLE};
		};
		typedef VFSEndianModes::_ VFSEndianMode;

		struct VFSEntryNameModes
		{
			enum _{FULL=1,FILE_NAME=2,DUAL=3};
		};
		typedef VFSEntryNameModes::_ VFSEntryNameMode;

		struct VFSEntry
		{
			VFSEntry() : mMainOffset(0), mDataSize(0){}
			size_t mMainOffset;		//Main offset from start of VFS file
			size_t mDataSize;
		};
		const u32 EVFS_VERSION;
		const u32 EVFS_VERSION_SWITCH;
	private:
		struct VFSOutEntry
		{
			VFSOutEntry() : mMainOffset(0), mDataSize(0){}
			std::string mTargetFileName;
			std::string mFileIncSource;
			size_t mMainOffset;		//Main offset from start of data
			size_t mDataSize;
		};
		std::map< std::string, VFSEntry > mDirectory;
		std::map< std::string, std::string > mOutDirectory;
		//Mutex mFileMutex;
		friend class FileReferenceVFS;
		File mVFSFile;
		u32 mVFSVersion;
		FileSystem& mVFSLoaderFileSystem;
	public:
		FileSystemSourceVFS(FileSystem& vfsLoaderFileSystem);
		virtual ~FileSystemSourceVFS();

		//////////////////////////////////////////////////////////////////////////
		//VFS Source functionality
		bool LoadVFS(const std::string& vfsFile);
		void UnloadVFS();

		/**
		 * Add a file to the VFS out directory list for saving.
		 * @note This method does not make the file available in the source.
		 * @param fileName The filename. The file is later resolved when calling SaveVFS() and is resolved via the FileSystem object
		 * this VFS source is installed into.
		 * @return true if the entry was added.
		 */
		bool AddFile(const std::string& fileName);

		/**
		 * Add a file to the VFS out directory list for saving allowing you to specify the targetName which will be used when reading
		 * the file from the VFS.
		 * @note This method does not make the file available in the source.
		 * @param targetName The name of the file that will be referenced when loading it from the VFS after it has been saved.
		 * @param fileName The filename. The file is later resolved when calling SaveVFS() and is resolved via the FileSystem object
		 * this VFS source is installed into.
		 * @return true if the entry was added.
		 */
		bool AddFile(std::string targetName, const std::string& source);

		/**
		 * Clears the current list of files that will be written when calling SaveVFS()
		 */
		void ClearOutDirectory();

		/**
		 * @brief 
		 * 
		 * @param fileName 
		 * @param endianMode 
		 * @param entryNameMode 
		 * @return * Exports 
		 */
		bool SaveVFS(const std::string& fileName, VFSEndianMode endianMode, VFSEntryNameMode entryNameMode = VFSEntryNameModes::FULL);

		const std::map< std::string, VFSEntry >& GetDirectory() const {return mDirectory;}
		const std::string& GetVFSFileName() const {return mVFSFile.GetActualFileName();}
		u32 GetVFSVersion() const {return mVFSVersion;}
		Size GetVFSSize() {return mVFSFile.GetSize(); }

		//////////////////////////////////////////////////////////////////////////
		//FileSystemSource interfaces
		File _Open(const std::string& originalFileName, const std::string& modifiedFileName, File::OpenMode openMode = File::OpenModes::READ) override;
	};
}
#endif
