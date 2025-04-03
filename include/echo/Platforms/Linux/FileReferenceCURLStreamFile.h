#ifndef _ECHOFILEREFERENCECURLSTREAMFILE_H_
#define _ECHOFILEREFERENCECURLSTREAMFILE_H_
#include <echo/FileSystem/FileReference.h>
#include <fstream>
#include <condition_variable>
#include <queue>
#include <mutex>
#include <boost/compute/detail/lru_cache.hpp>
#include <boost/circular_buffer.hpp>

namespace boost
{
	namespace urls
	{
		class url;
	}
}
namespace Echo
{
	class FileSystemSourceFile;
	class DataPacket;
	class Thread;
	class Shell;

	class FileReferenceCURLStreamFile : public FileReference
	{
	public:	
		FileReferenceCURLStreamFile(FileSystemSource* source, std::string url, std::string extraCurlOptions, std::string socketPath, Size chunkSize, Size maxChunks, bool quiet, Size fallbackFileSize);
		~FileReferenceCURLStreamFile();

		size_t Read(void* buffer, size_t typeSize, size_t numberToRead);
		size_t Write(const void* buffer, size_t typeSize, size_t numberToWrite);
		size_t Seek(size_t position);
		bool EndOfFile();

		operator bool() const
		{
			return mInitialised;
		}
	private:
		void QueueFutureChunks(Size fromChunkIndex);
		bool OpenSocketTunnel();
		bool SetupCurlShell();
		bool RefreshFileSize(Size fallbackFileSize);
		void ReadThread();
		void OnChunkData(const std::string& buffer);
		Size Read(u8* buffer, Size bytes);
		void OnTunnelProcessComplete(int exitCode, boost::circular_buffer< std::string > stdOut);

		/**
		 * Key is the chunk index which is calculated by mPosition / chunkSize
		 */
		boost::compute::detail::lru_cache< Size, shared_ptr<DataPacket> > mChunks;
		shared_ptr<DataPacket> mCurrentChunk;
		unique_ptr<boost::urls::url> mURL;
		std::string mExtraCurlOptions;
		std::string mSocketPath;
		std::string mSocketOption;
		Size mCurrentReadChunk;
		Size mChunkSize;
		bool mQuiet;
		bool mStop;

		unique_ptr<Thread> mStreamThread;
		shared_ptr<Shell> mShell;
		shared_ptr<Shell> mTunnelShell;
		std::list< Size > mQueuedReadChunks;
		std::mutex mStreamReadMutex;
		std::condition_variable mNextChunkDataCondition;
		std::condition_variable mChunksReady;
		bool mInitialised;
	};
}

#endif
