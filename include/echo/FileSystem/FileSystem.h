#ifndef _ECHOFILESYSTEM_H_
#define _ECHOFILESYSTEM_H_
#include <echo/Types.h>
#include <echo/FileSystem/File.h>

#include <map>
#include <list>

namespace Echo
{
	class FileReference;
	class FileSystemSource;

	/**
	 * File System interface.
	 * 
	 * File access has been abstracted away from standard file I/O in order to provide a simpler interface for implementing new
	 * data sources compared to the standard library.
	 * 
	 * Additional features are also included such as the redirection of file from an input request for example, in the case of a
	 * file download from a http server the request would include the full name but the opened file may be a cached file on disk.
	 * This can be detected from the File object.
	 * 
	 * FileSystem is simply a registry of FileSystemSource objects that are installed into an instance and implement most of the
	 * logic.
	 * 
	 * By using a FileSystem instance, especially the default one for the runtime platform, suitable default file sources can be
	 * configured without any code or data path changes. For example, when building for GCN or Wii the default file system may be
	 * either an SD card or USB drive, depending on which is availble. Similarly, file system sources can be mapped to different
	 * aliases for more logical access. The "persistent" source is an example of this, it is intended to be used as a "save file"
	 * location without worrying about details, the default directory for saving data will be configured per platform and should
	 * be writable.
	 * 
	 * You can configure your own FileSystem objects if the default configuration is not to your liking. You can also extend any
	 * default instances you decide to create.
	 * 
	 * FileSystemSource objects are designed to provide file access for a named source that at minimum follows the scheme
	 * component of a Uniform Resource Identifer. The scheme component is used to identify the source to use to open the file.
	 * If there is no scheme specified the default source is used.
	 * 
	 * Sources are not provided the identifier when opening a file in order for it to be possible to re-map them. The actual
	 * scheme can be changed by the source. For example, you may have a source that supports https and install it as http which
	 * would essentially force all http requests to be https.
	 * 
	 * It is a recommendation that a platform implementation provide at least a default source and a "peristent" source. A source
	 * is specified as default when InstallSource() is called and a source can be installed with the alias "persistent" to allow
	 * resolution of "persistent://SomeFileToSave.important". The default source does not require a source specifier in order to
	 * access, i.e. can be used as Open("SomeFileAtDefaultSource.needToRead")
	 */
	class FileSystem
	{
	public:
		FileSystem();
		~FileSystem();
		
		/**
		 * Install a file source.
		 * @param source the source.
		 * @param defaultSource whether this should be the default or not.
		 * @return true if the source was installed, false if the source could not be installed because of an identifier conflict.
		 */
		bool InstallSource(shared_ptr<FileSystemSource> source, bool defaultSource=false);

		/**
		 * Install a source but use a different identifier.
		 * With this method you can install the same source under different identifiers.
		 * @note The source will always have one identifier.
		 * @param newIdentifier The identifier to use for this source.
		 * @param source The source
		 * @param defaultSource whether this source should be the default or not.
		 * @return true if the source was installed, false if the identifier is already in use.
		 */
		bool InstallSource(const std::string& newIdentifier, shared_ptr<FileSystemSource> source, bool defaultSource=false);
		
		/**
		 * Uninstall a srouce by identifier.
		 * @param identifier the identifier, e.g. "persistent".
		 * @return true if the source was uninstalled, false if the identifier wasn't found.
		 */
		bool UninstallSource(const std::string& identifier);
		
		/**
		 * Helper method to create a source.
		 * @param identifier the source identifier, the source needs to have a constructor with a string parameter for the identifier.
		 * @param defaultSource whether this source should be the default or not.
		 * @return true if the source was installed, false if there was an identifier conflict. 
		 */
		template<class T>
		bool InstallSource(const std::string& identifier,bool defaultSource=false)
		{
			return InstallSource(make_shared<T>(identifier),defaultSource);
		}

		/**
		 * Get the available source identifiers
		 * @return list of identifiers. These do not include "://", for example "file" and "vfs" may included in the list.
		 */
		std::list<std::string> GetAvailableSources() const;

		/**
		 * Get the FileSystemSource installed with the specified identifier
		 * @param identifier The identifier of the source.
		 * @return The source that can be identified by the identifier or null if one is not installed that matches.
		 */
		shared_ptr<FileSystemSource> GetSource(const std::string& identifier) const;

		/**
		 * Set the default FileSystemSource by identifier
		 * @param sourceName The name of the source, for example "file" or "vfs"
		 * @return true If the source exists, otherwise false.
		 */
		bool SetDefaultSource(const std::string& sourceName);

		/**
		 * Get the default FileSystemSource.
		 * @note The source may report a different identifier to the one that was used for installation.
		 * @return The default filesystem source or null if no default source is installed.
		 */
		shared_ptr<FileSystemSource> GetDefaultSource() const {return mDefaultSource;}

		/**
		 * Open a file.
		 * The fileName can optionally include a source specified as "source://" before the file in question. Typically implementations
		 * will honour the current directory expectation which will resolve  --to "source://currentDirectory/file..." but in some
		 * situations where there is no such concept and it cannot be emulated (MemoryFileSystemSource for example, which is designed
		 * to map a file in memory) it may only use the filename directly.
		 * 
		 * Source identifiers are just an alias and can to another source. i.e. It is possible that a source a:// be removed and
		 * something else be put in place.
		 * 
		 * The original design of this interface was to provide a consistent way to acces files without worrying about if data was
		 * compiled in as an array, on disk in the current directory or a storage device at the first available location (e.g. GCN or
		 * Wii as SD card and USB). The usage expectation is that you open files as Open("MyFile") and the location of that file is
		 * determined by the application/platform configuration.
		 * 
		 * All files are opened in binary mode. Text mode can produce unexpected results so this interface expects you're interested
		 * in accessing data rather than having an intermediate processing layer. As such the File object returned will primarily be an
		 * interface to read and write bytes.
		 * @param fileName  The file in which to access.
		 * @param openMode The open mode. Most platforms have read access. Not all platforms have write access.
		 * @return File which can be checked if the file is opened with File::IsOpen().
		 */
		File Open(const std::string& fileName, File::OpenMode openMode = File::OpenModes::READ);

		/**
		 * Overload of Open() that allows specifing the source separate to the filename.
		 * @see Open(const std::string&, File::OpenMode)
		 */
		File Open(const std::string& fileName, const std::string& sourceIdentifier, File::OpenMode openMode = File::OpenModes::READ);

		/**
		 * Attempts to resolve the source from the input filename.
		 * A source is resolved if the source identifier matches an installed source.
		 * For example:
		 * 	file://someFile.txt
		 * - Will return the source identified by "file".
		 * - Name of the request is returned in outFileNameNoSource as "someFile.txt"
		 * If no source is specified the default source is returned, which may be null.
		 * @param fileName Name of the file that may or may not include the source identifier.
		 * @param outFileNameNoSource If a source identifier is detected then upon return contains the file name. This may resolve even
		 * if the source does not exist.
		 * @return Either the file system source identified by the string, the default source or null.
		 */
		shared_ptr<FileSystemSource> ResolveSource(const std::string& fileName, std::string& outFileNameNoSource);
		
		/**
		 * Attempt to delete a file.
		 * @note This will return false if file deletion is not supported.
		 * @param fileName The name of the file or folder to delete.
		 * @return true if the file was deleted.
		 */
		bool DeleteFile(const std::string& fileName);

		/**
		 * Try to determine if a file exists.
		 * @note If a source does not directly implement this then the file will try to be opened to determine
		 * if the file exists. This may not be the most optimal way to test.
		 * @param fileName name of the file or folder to test for.
		 * @return true if the file exists, false if the file doesn't exist.
		 */
		bool FileExists(const std::string& fileName);
		
		/**
		 * Resolve the full path and name of a file that might be relative to another file.
		 * The string returned will either be the fileName or the parentPath extracted from the parentFileName + the
		 * fileName. If the string returned is not equal to the provided fileName then the file exists relative to
		 * the parent path. Otherwise if the fileName is the same there is no guarantee about whether the file exists
		 * or not. The method will just "select" the parent path + fileName if a file exists and if not it defaults
		 * to returning the fileName.
		 * @param fileName	The file that you want to look for.
		 * @param parentFileName The parent file name, including the full path.
		 * @return If the file does not exist but it exists relative to the parent path then the
		 * parentPath + fileName is returned, otherwise the fileName is returned.
		 */
		std::string ResolveFullNameForFileWithParent(const std::string& fileName, const std::string& parentFileName);

		/**
		 * Attempts to create directories in a path.
		 * Implementations should create all directories required in the path. I.e. if the directoryPath contains multiple
		 * directories each directory that needs to be created should be created to make the path resolvable. For example:
		 * Given:
		 *  - Current directory contains level1
		 *  - directoryPath is level1/level2/level3
		 * Calling CreateDirectories("level1/level2/level3") should result in the following structure:
		 *
		 *  level1
		 *    level2
		 *      level3
		 *
		 * If only some directories can be created for some reason then the method should return false.
		 * @note The current directory of the file system source should be taken into account. The caller may specify the
		 * file system source type as per normal but the current directory will determine where the first directory should
		 * be created.
		 * @note Not all all file system sources are writable so this method may never succeed in some circumstances.
		 * @param directoryPath The directory path, it should not contain a filename.
		 * @return true If all directories were created or already existed, or false on any failure.
		 */
		bool CreateDirectories(const std::string& directoryPath);
	private:
		std::map< std::string, shared_ptr<FileSystemSource> > mSources;
		std::list< FileReference* > mOpenFiles;
		shared_ptr<FileSystemSource> mDefaultSource;
	private:
		friend class FileSystemSource;
		void FileClosed(FileReference& fileReference);
		void FileOpened(FileReference& fileReference);
	};
}
#endif
