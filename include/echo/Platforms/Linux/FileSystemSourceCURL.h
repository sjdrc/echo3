#ifndef _ECHOFILESYSTEMSOURCECURL_H_
#define _ECHOFILESYSTEMSOURCECURL_H_
#include <echo/Types.h>
#include <echo/FileSystem/File.h>
#include <echo/FileSystem/FileSystemSource.h>

namespace Echo
{
	class Thread;
	class Configuration;

	/**
	 * A File system source that supports downloading files using curl.
	 * You provide a cache file system in which the defaultCachePath determines the file cache.
	 * The source will check to see if the requested file (by path) exists and if it doesn't it
	 * will download from the given URL and save it.
	 * 
	 * For example:
	 * 
	 *	mFileSystem->Open("https://www.somedomain.com/somepath/somefile.tar.gz");
	 * 
	 * Will perform the following operaiton:
	 * 
	 *	std::string cachedFile = cachePath + "www.somedomain.com/somepath/somefile.tar.gz";
	 *	if(!mCacheFileSystem.FileExists(cachedFile))
	 *	{
	 *		// download the file, save it to cache then open it.
	 *	}
	 *	return mCacheFileSystem.Open(cachedFile);
	 * 
	 * @note The download will block the open call until it finishes. If this is a problem you should
	 * consider opening these URLs in a separate thread. In the future there will likely be a more
	 * flexible implementation.
	 * 
	 * @note This version of the class has been added to the Linux platform since it depends on the
	 * command line curl program to perform the download. In the future we will implement an
	 * integrated curl solution version but no additional dependencies are required for this variation
	 * and this is being implemented for someone that needs it immediately. Further on that note, the
	 * implementation uses boost to perform additional file and folder management, so the cache
	 * FileSystem+cachePath should really be a file source.
	 * 
	 * @note To support http and https you need to install multiple instances of this class one for
	 * each protocol. i.e in your Application code:
	 *		shared_ptr<FileSystem> fileSystem = GetFileSystem();
	 *		fileSystem->InstallSource(make_shared<FileSystemSourceCURL>(*fileSystem,"http","cache"),false);
	 *		fileSystem->InstallSource(make_shared<FileSystemSourceCURL>(*fileSystem,"https","cache"),false);
	 */
	class FileSystemSourceCURL : public FileSystemSource
	{
	public:
		/**
		 * Constructor
		 * @param cacheFileSystem - The FileSytem to use to open, write and check for cached files.
		 * @param identifier - This is the protocol, e.g. http or https.
		 * @param cachePath
		 */
		FileSystemSourceCURL(FileSystem& cacheFileSystem, std::string identifier, std::string cachePath="");
		virtual ~FileSystemSourceCURL();

		//////////////////////////////////////////////////////////////////////////
		//FileSystemSource interfaces
		File _Open(const std::string& originalFileName, const std::string& modifiedFileName, File::OpenMode openMode = File::OpenModes::READ) override;
	private:
		/**
		 * Get any options that were included in the URL
		 * @param urlWithOptions Options are appended to the URL separated with | for example
		 * emblem.net.au|option1=123|option2=abc|optionN=xyz
		 * @return optional<Configuration> On error (from calling Configuration::ParseLines() with the
		 * options) is none, otherwise a configuration object containing the options.
		 */
		optional<Configuration> GetOptions(std::string urlWithOptions) const;

		FileSystem& mCacheFileSystem;
		std::string mCachePath;
		std::string mIdentifier;
	};
}
#endif
