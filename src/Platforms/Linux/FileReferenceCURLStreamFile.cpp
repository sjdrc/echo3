#include <echo/Platforms/Linux/FileReferenceCURLStreamFile.h>
#ifndef ECHO_PLATFORM_ANDROID
#include <boost/filesystem.hpp>
#endif
#include <iostream>
#include <echo/Assert.h>
#include <boost/algorithm/string.hpp>
#include <echo/Shell/Shell.h>
#include <echo/Network/DataPacket.h>
#include <echo/Platform.h>
#include <boost/url/src.hpp>
#include <echo/Util/StringUtils.h>
#include <echo/Util/RegEx.h>

namespace Echo
{
	// Thanks to this
	// https://unix.stackexchange.com/questions/393640/how-to-access-http-server-from-bash-script-with-existing-tcp-connection
		// -N, --no-buffer
		// 		Disables  the  buffering  of the output stream. In normal work situations, curl will use a standard buffered output stream that
		// 		will have the effect that it will output the data in chunks, not necessarily exactly when the data arrives.  Using this  option
		// 		will disable that buffering.

		// --retry <num>
		// 		If  a  transient  error is returned when curl tries to perform a transfer, it will retry this number of times before giving up.
		// 		Setting the number to 0 makes curl do no retries (which is the default). Transient error means either: a timeout,  an  FTP  4xx
		// 		response code or an HTTP 408, 429, 500, 502, 503 or 504 response code.

		// 		When  curl  is about to retry a transfer, it will first wait one second and then for all forthcoming retries it will double the
		// 		waiting time until it reaches 10 minutes which then will be the delay between the rest of the retries. By  using  --retry-delay
		// --retry-max-time <seconds>
		// 		The retry timer is reset before the first transfer attempt. Retries will be done as usual (see --retry) as long  as  the  timer
		// 		has  not  reached this given limit. Notice that if the timer has not reached the limit, the request will be made and while per‐
		// 		forming, it may take longer than this given time period. To limit a single request's maximum time, use -m, --max-time. Set this
		// 		option to zero to not timeout retries.

		// 		If this option is used several times, the last one will be used.
		//socat TCP:10.5.1.1:80 UNIX-LISTEN:/tmp/curl.sock,fork

	FileReferenceCURLStreamFile::FileReferenceCURLStreamFile(FileSystemSource* source, std::string url, std::string extraCurlOptions,
		std::string socketPath, Size chunkSize, Size maxChunks, bool quiet, Size fallbackFileSize) :
		FileReference(source),
		mURL(new boost::urls::url(url)),
		mExtraCurlOptions(extraCurlOptions),
		mSocketPath(socketPath),
		mCurrentReadChunk(0),
		mChunkSize(chunkSize),
		mChunks(maxChunks),
		mQuiet(quiet),
		mStop(false)
	{
		ECHO_LOG_DEBUG("chunkSize:" << chunkSize << " maxChunks:" << maxChunks << " socketPath: " << socketPath << " curl: " << extraCurlOptions);
		ECHO_ASSERT(chunkSize > 0, "Chunk size must be greater than 0");
		mInitialised = RefreshFileSize(fallbackFileSize) && SetupCurlShell();
		if(!mSocketPath.empty())
		{
			mInitialised = mInitialised && OpenSocketTunnel();
		}
	}

	FileReferenceCURLStreamFile::~FileReferenceCURLStreamFile()
	{
		mStop = true;
		mNextChunkDataCondition.notify_all();
		mChunksReady.notify_all();
		if(mTunnelShell)
		{
			mTunnelShell->KillProcess();
			mTunnelShell->StopThread(true);
		}
		if(mShell)
		{
			mShell->StopThread(true);
		}
		if(mStreamThread)
		{
			mStreamThread->Terminate(true);
		}
	}

	bool FileReferenceCURLStreamFile::OpenSocketTunnel()
	{
		mTunnelShell = Platform::CreateShell();
		if(!mTunnelShell)
		{
			ECHO_LOG_ERROR("Unable to create shell to run socat to download file.");
			return false;
		}

		// Open a socat tunnel on another thread
		std::string defaultPort = (mURL->scheme()=="https") ? "443": "80";
		mTunnelShell->SetQuiet(mQuiet);
		mTunnelShell->Execute("socat UNIX-LISTEN:" + mSocketPath + ",fork,reuseaddr TCP:" + mURL->host() + ":" + (mURL->has_port() ? std::string(mURL->port().begin(),mURL->port().end()) : defaultPort), true);
		mTunnelShell->SetProcessFinishedCallback(bind(&FileReferenceCURLStreamFile::OnTunnelProcessComplete,this,placeholders::_2,placeholders::_3));
		mSocketOption="--unix-socket " + mSocketPath;

		return true;
	}

	void FileReferenceCURLStreamFile::OnTunnelProcessComplete(int exitCode, boost::circular_buffer< std::string > stdOut)
	{
		// From man socat:
		// On exit, socat gives status 0 if it terminated due to EOF or inactivity timeout, with a positive value on error, and with
		// a negative value on fatal error.
		if(exitCode!=0)
		{
			ECHO_LOG_ERROR("socat tunnel shutdown with exit code: " << exitCode << " tunnel " << std::string(mURL->host().begin(),mURL->host().end()) << " via " << mSocketPath);
		}else
		{
			ECHO_LOG_DEBUG("socat tunnel shutdown successfully: " << std::string(mURL->host().begin(),mURL->host().end()) << " via " << mSocketPath);
		}
	}

	bool FileReferenceCURLStreamFile::SetupCurlShell()
	{
		using placeholders::_1;
		using placeholders::_2;
		mShell = Platform::CreateShell();
		if(!mShell)
		{
			ECHO_LOG_ERROR("Unable to create shell to run curl to download file.");
			return false;
		}

		mShell->SetCaptureStdOut(true);
		mShell->SetQuiet(mQuiet);
		mShell->SetStdOutChunkSize(1024*32);	// Copy out of shell to our chunks in 32 KiB fragments. This is just arbitrary.
		mShell->SetStdOutCallback(bind(&FileReferenceCURLStreamFile::OnChunkData,this,_2));

		mStreamThread.reset(new Thread("CurlStream",[this](){ReadThread();}));
		mStreamThread->Execute();
		return true;
	}

	bool FileReferenceCURLStreamFile::RefreshFileSize(Size fallbackFileSize)
	{
		mFileSize = fallbackFileSize;

		//https://stackoverflow.com/questions/4497759/how-to-get-remote-file-size-from-a-shell-script
		// We can try and get the size with curl
		//curl -sIL https://www.emblem.net.au/arduino-1.0.1-Marlin-EmblemPrusai2.zip | grep content-length | awk '{print $2}'
		
		shared_ptr<Shell> shell = Platform::CreateShell();
		if(!shell)
		{
			ECHO_LOG_ERROR("Unable to create shell to run curl to download file.");
			return false;
		}
		shell->SetQuiet(mQuiet);
		shell->SetCaptureStdOut(true);

		struct SizeParamsAndHeader
		{
			std::string mCurlParameters;
			std::string mValueFromHeaderRegex;
		};

		SizeParamsAndHeader headersAndParametersToTrySearchFor[] = {
			// -i is include the HTTP response headers in the output -  this is different to -I to make a HEAD request
			// -r is get range
			{"-isfL -r 0-1",".*content-range:.*\\/([0-9]*)"},	// In a couple of experiements, including using a presigned URL, the range was returned with `content-range: from-to/total`
			// -s = silent
			// -f = fail to report an exit code on http error (see man curl)
			// -I = Fetch headers only
			// -L = follow location changes/redirects
			{"-sfIL","content-length:\\s*([0-9]*)"}				// This uses a HTTP HEAD request. Presigned S3 URLs have issues if they are only permitted GET requests.
		};

		for(const SizeParamsAndHeader& searchParameters : headersAndParametersToTrySearchFor)
		{
			shell->SetStdOutCallback([this,searchParameters](Shell&, const std::string& line)
			{
				std::string lowerLine = boost::algorithm::to_lower_copy(line);
				ECHO_LOG_DEBUG(lowerLine);
				Utils::RegEx::ProcessRegExCapture<Size>(lowerLine,searchParameters.mValueFromHeaderRegex,[this](Size /*matches*/, const Size& fileSize){
					mFileSize = fileSize;
				});
			});

			ECHO_LOG_DEBUG("Attempting curl with " << searchParameters.mCurlParameters << " for file size probe");
			ECHO_LOG_INFO("curl " << searchParameters.mCurlParameters << " \"" << std::string(mURL->buffer().begin(),mURL->buffer().end()) << "\" " << mExtraCurlOptions);
			int returnValue = shell->Execute("curl " + searchParameters.mCurlParameters + " \"" + std::string(mURL->buffer().begin(),mURL->buffer().end()) + "\" " + mExtraCurlOptions, false);
			if(returnValue!=0)
			{
				boost::urls::url wihoutUserInfo = *mURL;
				wihoutUserInfo.remove_userinfo();
				if(returnValue==22)
				{
					// HTTP  page  not retrieved. The requested url was not found or returned another error with the HTTP error code being 400 or above.
					// This return code only appears if -f, --fail is used.
					ECHO_LOG_ERROR("HTTP 400+ error received. File cannot be opened: " << wihoutUserInfo.buffer());
				}else
				{
					// NOTE: Outputting stdout won't be useful here because we are specifying the -f command tells curl to fail silently so the
					// return code should tell us more. The success output is redirected to stdout for parsing.
					ECHO_LOG_ERROR("Failed to get filesize. Error:" << returnValue << " for " << wihoutUserInfo.buffer());
				}
				continue;
			}
			ECHO_LOG_DEBUG("Got file size: " << mFileSize);
			return true;
		}
		if(fallbackFileSize>0)
		{
			ECHO_LOG_DEBUG("Using fallback file size");
			return true;
		}
		ECHO_LOG_DEBUG("Could not find file size");
		return false;
	}

	void FileReferenceCURLStreamFile::ReadThread()
	{
		while(mPosition < mFileSize && !mStop)
		{
			Size chunkIndex;
			// We interact with the read chunk queue and the chunks map with the same mutex
			{
				std::unique_lock< std::mutex > lock(mStreamReadMutex);

				if(mQueuedReadChunks.empty())
				{
					// Wait for the next chunk to be needed
					mNextChunkDataCondition.wait(lock,[this](){return (!mQueuedReadChunks.empty() || mStop);});
				}
				if(mStop)
				{
					ECHO_LOG_INFO("Immediate stop reading more chunks");
					return;
				}
				chunkIndex = mQueuedReadChunks.front();
			}

			// mCurrentChunk is modified in this loop only (or during clean up)- the shell command is blocking and the read callback is where it is written to
			mCurrentChunk.reset(new DataPacket(0,mChunkSize));
			Size chunkStart = chunkIndex*mChunkSize;
			if(chunkStart >= mFileSize)
			{
				// Remove the chunk we just downloaded
				std::unique_lock< std::mutex > lock(mStreamReadMutex);
				ECHO_LOG_DEBUG("Chunk past end requested, discarding request. This is normal behaviour as the reader just requests N chunks past the current read chunk regardless of position.");
				mQueuedReadChunks.pop_front();
				continue;
			}
			Size chunkEnd = chunkStart+mChunkSize-1;
			if(mFileSize>0 && chunkEnd > mFileSize-1)
			{
				chunkEnd = mFileSize-1;
			}
			mCurrentReadChunk = chunkIndex;
			ECHO_LOG_DEBUG("Requesting  " << chunkStart << " to " << chunkEnd << " for chunk " << mCurrentReadChunk);
			// -s for silent
			// -S show errors
			// -N to output to stdout
			// -L = follow location changes/redirects
			int returnValue = mShell->Execute("curl -sSNL \"" + std::string(mURL->buffer().begin(),mURL->buffer().end()) + "\" " +  mSocketOption + " " + mExtraCurlOptions + " -r " + std::to_string(chunkStart) + "-" + std::to_string(chunkEnd), false);
			{
				// Remove the chunk we just downloaded
				std::unique_lock< std::mutex > lock(mStreamReadMutex);
				mQueuedReadChunks.pop_front();
			}

			if(returnValue!=0)
			{
				boost::urls::url wihoutUserInfo = *mURL;
				wihoutUserInfo.remove_userinfo();
				if(returnValue==33)
				{
					// Specifically: HTTP range error. The range "command" didn't work.
					ECHO_LOG_ERROR("HTTP range error. The range command didn't work when requesting chunk for " << wihoutUserInfo.buffer());
				}else
				{
					// NOTE: Outputting stdout won't be useful here because it could contain garbage since we're asking curl to output to stdout
					// instead the -f command tells curl to fail silently so the return code should tell us more.
					ECHO_LOG_ERROR("Error running curl to get range. Error:" << returnValue << " for " << wihoutUserInfo.buffer());
				}

				mStop = true;
			}

		}
	}

	void FileReferenceCURLStreamFile::OnChunkData(const std::string& buffer)
	{
		Size bytesLeft = buffer.size();
		Size appended = mCurrentChunk->AppendData(buffer.data(),buffer.size());
		//ECHO_LOG_INFO("Got  " << bytesLeft << " bytes for chunk " << mCurrentReadChunk << " appended " << appended);
		while(appended < bytesLeft)
		{
			bytesLeft-=appended;

			// Add the chunk to the cache
			{
				mChunks.insert(mCurrentReadChunk,mCurrentChunk);
				mCurrentChunk.reset(new DataPacket(0,mChunkSize));
				mChunksReady.notify_all();
			}
	
			++mCurrentReadChunk;

			appended=mCurrentChunk->AppendData(buffer.data(),bytesLeft);
			ECHO_LOG_DEBUG("\tNext chunk " << bytesLeft << " bytes for chunk " << mCurrentReadChunk << " appended " << appended);
		}
		// See if this is the last chunk

		if(mCurrentChunk->GetRemainingDataSize()==0 || mCurrentChunk->GetReceivedDataSize()==(mFileSize - (mCurrentReadChunk * mChunkSize)))
		{
			// Add the chunk to the cache
			mChunks.insert(mCurrentReadChunk,mCurrentChunk);
			mCurrentChunk.reset(new DataPacket(0,mChunkSize));
			ECHO_LOG_DEBUG("Notify chunk available  " << mCurrentReadChunk);
			++mCurrentReadChunk;
			mChunksReady.notify_all();
		}

	}

	void FileReferenceCURLStreamFile::QueueFutureChunks(Size fromChunkIndex)
	{
		// Make sure the next N chunks are available or queued in
		Size lastFutureChunkIndex = fromChunkIndex + mChunks.capacity();
		for(Size futureChunkIndex = fromChunkIndex; futureChunkIndex < lastFutureChunkIndex; ++futureChunkIndex)
		{
			if(!mChunks.contains(futureChunkIndex))
			{
				// Need to request a chunk
				if(std::find(mQueuedReadChunks.begin(), mQueuedReadChunks.end(), futureChunkIndex)==mQueuedReadChunks.end())
				{
					mQueuedReadChunks.push_back(futureChunkIndex);
					mNextChunkDataCondition.notify_all();
				}
			}
		}
	}

	Size FileReferenceCURLStreamFile::Read(u8* buffer, Size bytes)
	{
		Size chunkIndex = mPosition / mChunkSize;
		std::unique_lock< std::mutex > lock(mStreamReadMutex);
		
		boost::optional< shared_ptr<DataPacket> > chunk = mChunks.get(chunkIndex);
		if(!chunk)
		{
			// Need to request a chunk
			QueueFutureChunks(chunkIndex);

			ECHO_LOG_DEBUG("Queued " << chunkIndex << " and friends. Waiting for available" );

			// Wait on read
			mChunksReady.wait(lock,[this,chunkIndex](){return (mChunks.contains(chunkIndex) || mStop);});
			if(mStop)
			{
				ECHO_LOG_WARNING("Read cancelled");
				return 0;
			}
			chunk = mChunks.get(chunkIndex);
			ECHO_ASSERT(chunk,"Logic error. Chunk should be valid.");
		}

		DataPacket& chunkPacket = (**chunk);
		Size positionInChunk = mPosition - (chunkIndex*mChunkSize);
		Size bytesCanRead = std::min(Size(chunkPacket.GetReceivedDataSize())-positionInChunk,bytes);
		ECHO_LOG_DEBUG("Reading " << bytesCanRead << " bytes from chunk " << chunkIndex);
		const u8* from = &(chunkPacket.GetData()[positionInChunk]);
		std::copy(from,from+bytesCanRead,buffer);

		// Request the next chunk if we are at the end of this chunk
		if(positionInChunk+bytesCanRead >=chunkPacket.GetReceivedDataSize() )
		{
			ECHO_LOG_DEBUG("Hit end of chunk, queing future next");
			QueueFutureChunks(chunkIndex+1);
		}

		return bytesCanRead;
	}

	size_t FileReferenceCURLStreamFile::Read(void* buffer, size_t typeSize, size_t numToRead)
	{
		if(mStop)
		{
			boost::urls::url wihoutUserInfo = *mURL;
			wihoutUserInfo.remove_userinfo();
			ECHO_LOG_ERROR("The stream has failed for " << wihoutUserInfo.buffer());
			return 0;
		}
		u8* byteBuffer = reinterpret_cast<u8*>(buffer);
		Size totalBytesRead = 0;
		Size bytesRead = 0;
		Size bytesLeft=typeSize*numToRead;
		do
		{
			bytesRead = Read(byteBuffer,bytesLeft);
			byteBuffer+=bytesRead;
			bytesLeft-=bytesRead;
			totalBytesRead+=bytesRead;
			mPosition+=bytesRead;
			
		}while(bytesLeft > 0 && bytesRead!=0 && !mStop);
		return totalBytesRead;
	}

	size_t FileReferenceCURLStreamFile::Write(const void* buffer, size_t typeSize, size_t numberToWrite)
	{
		ECHO_LOG_ERROR("Write is not supported");
		return 0;
	}

	size_t FileReferenceCURLStreamFile::Seek(size_t position)
	{
		mPosition=position;
		return mPosition;
	}


	bool FileReferenceCURLStreamFile::EndOfFile()
	{
		return (mFileSize==mPosition);
	}
}
