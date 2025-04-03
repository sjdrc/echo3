#ifndef _ECHOFILESYSTEMSOURCE_H_
#define _ECHOFILESYSTEMSOURCE_H_
#include <echo/Types.h>
#include <echo/FileSystem/File.h>

namespace Echo
{
	class FileReference;

	class FileSystemSource
	{
	public:
		FileSystemSource(const std::string& identifier);
		virtual ~FileSystemSource();
		
		/**
		 * Opens a file.
		 * This method will attempt to open a file specified by file name.
		 * The current directory set with SetCurrentDirectory(), will be prefixed to the fileName.
		 * For example:
		 *		source->SetCurrentDirectory("MyFolder");
		 *		File file=source->Open("myFile.dat");
		 * Would attempt to open the file "MyFolder/myFile.dat".
		 * @note Generally you won't call Open() on a FileSystemSource directly. You should, instead,
		 * call FileSystem::Open() through the FileSystem object that has the source set up.
		 * The FileSystem object will resolve the source based on the file name. See the FileSystem
		 * documentation for more information.
		 * @see FileSystem
		 * @param fileName The name of the file.
		 * @param openMode The mode to open the file.
		 * @return 
		 */
		File Open(const std::string& fileName, File::OpenMode openMode = File::OpenModes::READ);
		
		/**
		 * Get the Current Directory.
		 * This method returns the value of the current directory member variable. A source may choose to
		 * ignore this value but it is recommended that the current directory be used for consistency.
		 * @return The stored current directory.
		 */
		std::string GetCurrentDirectory() const;

		/**
		 * Set the Current Directory.
		 * This method only sets the internal current directory member variable and does not notify any
		 * implementation of the call. It is expected that implementations call GetCurrentDirectory() or
		 * rely on the interfaces provided by this class to prepend the current directory.
		 * The method detects whether or not directory includes "/" at the end and will ensure that the
		 * stored value includes a trailing slash.
		 * @param directory The new current directory
		 */
		void SetCurrentDirectory(std::string directory);
		
		virtual const std::string& GetIdentifier() const{return mIdentifier;}
		
		/**
		 * Attempt to delete a file.
		 * Implementations can either implement this method or _DeleteFile() which will be provided both the
		 * requested filename and the currentDirectory + filename by the default implementation.
		 * @param fileName The file to delete.
		 * @return true if the file was deleted, otherwise false.
		 */
		virtual bool DeleteFile(const std::string& fileName);

		/**
		 * Check if a file exists.
		 * The default implementation will attempt to determine existence by opening the file then closing it.
		 * If there is a more optimal way to check if a file exists then the implementation can opt to do so.
		 * Implementations can either implement this method or _FileExists() which will be provided both the
		 * requested filename and the currentDirectory + filename by the default implementation.
		 * @param fileName The file to test for existence.
		 * @return true if the file exists, otherwise false.
		 */
		virtual bool FileExists(const std::string& fileName);

		/**
		 * Create directories as per FileSystem::CreateDirectories().
		 * The default implementation includes the current directory and calls the implementation method
		 * _CreateDirectories().
		 * @note There is no set behaviour around whether or not partial directories 
		 * @return The implementation should return true on success, otherwise false.
		 */
		virtual bool CreateDirectories(const std::string& directories);
	private:
		virtual File _Open(const std::string& originalFileName, const std::string& modifiedFileName, File::OpenMode openMode = File::OpenModes::READ) = 0;
		FileSystem* mFileSystem;
		std::string mIdentifier;
		std::string mCurrentDirectory;

		friend class FileSystem;
		void SetFileSystem(FileSystem* fileSystem)
		{
			mFileSystem = fileSystem;
		}
	protected:
		FileSystem* GetFileSystem() const {return mFileSystem;}
		/**
		 * Check whether or not a file exists
		 * The default implementation is to return false.
		 * @param requestedFileName The filename passed into FileExists() (default implementation)
		 * @param modifiedFileName  The modified filename passed into FileExists() (default implementation is
		 * currentDirectory+requestedFileName)
		 * @return true if the file was deleted, otherwise false.
		 */
		virtual bool _DeleteFile(const std::string& requestedFileName, const std::string& modifiedFileName);

		/**
		 * Check whether or not a file exists
		 * The default implementation attempts to open the file, if the file can be opened it is resolved to exist.
		 * If there is a more optimal way to check if a file exists then the implementation can opt to do so.
		 * @param requestedFileName The filename passed into FileExists() (default implementation)
		 * @param modifiedFileName  The modified filename passed into FileExists() (default implementation is
		 * currentDirectory+requestedFileName)
		 * @return true is the file exists, otherwise false.
		 */
		virtual bool _FileExists(const std::string& requestedFileName, const std::string& modifiedFileName);

		/**
		 * Create directories as per FileSystem::CreateDirectories().
		 * The default implementation returns false.
		 * @param requestedFileName The path passed into CreateDirectories() (default implementation)
		 * @param modifiedFileName  The modified path passed into CreateDirectories() (default implementation is
		 * currentDirectory+requestedFileName)
		 * @return true if the directories were created, otherwise false.
		 */
		virtual bool _CreateDirectories(const std::string& requestedFileName, const std::string& modifiedFileName);
	protected:
		File CreateFile(const std::string& requestedFileName, const std::string& actualFileName, shared_ptr<FileReference> fileReference);
		File CreateFile(const std::string& requestedFileName, const std::string& actualFileName);
	private:
		friend class FileReference;
		void FileOpened(FileReference& reference);
		void FileClosed(FileReference& reference);

	};
}
#endif
