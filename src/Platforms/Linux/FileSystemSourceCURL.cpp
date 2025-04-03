#include <echo/Platforms/Linux/FileSystemSourceCURL.h>
#include <echo/Platforms/Linux/FileReferenceCURLStreamFile.h>
#include <echo/FileSystem/FileSystem.h>
#include <echo/Platform.h>
#include <echo/Shell/Shell.h>
#include <echo/Util/Configuration.h>
#include <echo/Maths/EchoMaths.h>
#include <echo/Util/StringUtils.h>
#include <echo/Chrono/Chrono.h>

namespace Echo
{

	FileSystemSourceCURL::FileSystemSourceCURL(FileSystem& cacheFileSystem, std::string identifier, std::string cachePath) :
		FileSystemSource(identifier), mCacheFileSystem(cacheFileSystem), mIdentifier(identifier)
	{
		if(!cachePath.empty())
		{
			mCachePath = cachePath;
			if(mCachePath.back()!='/')
			{
				mCachePath += "/";
			}
		}
	}
	
	FileSystemSourceCURL::~FileSystemSourceCURL()
	{
	}

	optional<Configuration> FileSystemSourceCURL::GetOptions(std::string urlWithOptions) const
	{
		Configuration options;
		std::vector< std::string > optionLines;
		Utils::String::Split(urlWithOptions,"|",optionLines);
		if(optionLines.empty() || !options.ParseLines(optionLines))
		{
			ECHO_LOG_ERROR("There was a problem parsing one or more of the options.");
			return none;
		}

		// The first should be the URL
		options.Set("url",optionLines.front());
		return options;
	}

	File FileSystemSourceCURL::_Open(const std::string& originalFileName, const std::string& modifiedFileName, File::OpenMode openMode)
	{
		// Check to see if there are additional options.
		// From the spec https://www.rfc-editor.org/rfc/rfc3986#section-2 the characters are listed through a number of sections.
		// Someone kindly listed them out for someone else here https://stackoverflow.com/a/1547940
		// ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=

		// We're using a nonstandard
		optional<Configuration> options = GetOptions(originalFileName);
		if(!options)
		{
			ECHO_LOG_ERROR("Failed to create Configuration from input URL");
			return File();
		}

		// The file name is stripped of the protocol, e.g. https:// because FileSystem preprocesses and uses this to determine the
		// FileSystem source. Since we need to install one of these objects per protocol (http and https) we'll use them as the
		// protocol identifiers to build the full URL.

		std::string url;
		options->Get<std::string>("url",url,"");
		url = mIdentifier+"://"+url;

		if(openMode!=File::OpenModes::READ)
		{
			ECHO_LOG_ERROR("Unable to open " << Utils::String::RemoveURLSchemeAndCreds(url) << ". Write mode is not supported");
			return File();
		}
		
		// We ignore the modified name because we want the original request. We modify it to be file system safe.
		std::string cachedFile = mCachePath + Utils::String::MakeURLDirectorySafe(url);

		bool stream = options->Get("stream",false);
		if(stream)
		{
			// Open as a stream - 
			// We may need to use --unix-socket option to re-use the address
			std::string extraCurlOptions;
			options->Get("curl",extraCurlOptions,"",0,true);

			// If empty a socket file won't be used
			std::string socketPath;
			if(options->Get("useSocketFile",false))
			{
				options->Set("DATETIME",Chrono::GetIOS8601PathSafe());
				options->Set("RANDOM", Maths::Random());
				options->Set("DEFAULT_SOCKET_PATH","efrcsftun$DATETIME$RANDOM.sock");
				socketPath = options->Get("socketPath",options->Get("DEFAULT_SOCKET_PATH",""));
			}
			Size chunkSize = options->Get("chunkSize",1024*1024);	// 1MiB
			Size maxChunks = options->Get("maxChunks",10);
			Size fallbackFileSize = options->Get("fallbackFileSize",0);
			bool quiet = options->Get("quiet",true);
			
			shared_ptr<FileReferenceCURLStreamFile> fileReference(new FileReferenceCURLStreamFile(this,url,extraCurlOptions,socketPath,chunkSize,maxChunks,quiet,fallbackFileSize));
			if(*fileReference)
			{
				return File(url,url,fileReference);
			}
			ECHO_LOG_ERROR("Unable to open file for streaming");
			return File();
		}
		if(!mCacheFileSystem.FileExists(cachedFile))
		{
			// download the file, save it to cache then open it.
			shared_ptr<Shell> shell = Platform::CreateShell();
			if(!shell)
			{
				ECHO_LOG_ERROR("Unable to create shell to run curl to download file.");
				return File(url,cachedFile);
			}
			
			ECHO_LOG_INFO("Cache File does not exist \"" << cachedFile <<"\" : attempting to download...");
			
			std::string curlOptions;
			options->Get<std::string>("curl",curlOptions,"",0,true);
			int returnValue = shell->Execute("curl \"" + url + "\" " + curlOptions + " --create-dirs -o \"" + cachedFile + "\"", false);
			if(returnValue!=0)
			{
				const boost::circular_buffer< std::string >& stdOut = shell->GetStdOut();
				ECHO_LOG_ERROR("Failed to download file. Output: " << returnValue);
				for( auto& line : stdOut )
				{
					ECHO_LOG_ERROR(line);
				}
				return File(url,cachedFile);
			}
			ECHO_LOG_INFO("Downloaded to: \"" + cachedFile + "\"");
		}
		return mCacheFileSystem.Open(cachedFile, openMode);
	}
}
