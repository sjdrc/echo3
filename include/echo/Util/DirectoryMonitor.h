#ifndef ECHO_UTIL_DIRECTORY_MONITOR_H
#define ECHO_UTIL_DIRECTORY_MONITOR_H

#include <map>
#include <efsw/efsw.hpp>
#include <echo/Kernel/Thread.h>
#include <echo/Kernel/ScopedLock.h>
#include <boost/filesystem.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/deadline_timer.hpp>

namespace Echo
{
	/**
	 * A class to use efsw file watcher using callbacks.
	 * @note You need to link libefsw with your project if you use this class as we do not include
	 * it in Echo's build settings. This may change in the future.
	 * @note This class is header only since it is not portable if the target platform does not
	 * have an implementation (i.e. there would be no library to link against).
	 */
	class DirectoryMonitor : public efsw::FileWatchListener
	{
	public:
		typedef function<void(std::string, std::string)> DirectoryModifiedCallback;
		typedef function<void(std::string, std::string, std::string)> FileMovedCallback;
		
		DirectoryMonitor() : mIsWatching(false), mCallbackDelay(500), mWork(mCallbackService)
		{
			mCallbackThread = make_shared<Thread>("DirectoryMonitor",[this](){mCallbackService.run();});
			mCallbackThread->Execute();
		}
		
		~DirectoryMonitor()
		{
			for(auto it : mDirectoryWatches)
			{
				mFileWatcher.removeWatch(it.second);
			}
			for(auto it : mRecursiveDirectoryWatches)
			{
				mFileWatcher.removeWatch(it.second);
			}
			mCallbackService.stop();
			mCallbackThread->Terminate(true);
		}

		void SetCallbackDelay(Milliseconds callbackDelay)
		{
			mCallbackDelay = callbackDelay;
		}
		
		void RegisterFileModifiedCallback(std::string filenameOrDirectory, DirectoryModifiedCallback callback)
		{
			ScopedLock lock(mMapMutex);
			mModifiedCallbacks[filenameOrDirectory] = callback;
			UpdateWatch(filenameOrDirectory, false);
		}

		void RegisterFileAddedCallback(std::string filenameOrDirectory, DirectoryModifiedCallback callback)
		{
			ScopedLock lock(mMapMutex);
			mAddedCallbacks[filenameOrDirectory] = callback;
			UpdateWatch(filenameOrDirectory, false);
		}

		void RegisterFileDeletedCallback(std::string filenameOrDirectory, DirectoryModifiedCallback callback)
		{
			ScopedLock lock(mMapMutex);
			mDeletedCallbacks[filenameOrDirectory] = callback;
			UpdateWatch(filenameOrDirectory, false);
		}

		void RegisterMovedCallback(std::string filenameOrDirectory, FileMovedCallback callback)
		{
			ScopedLock lock(mMapMutex);
			mMovedCallbacks[filenameOrDirectory] = callback;
			UpdateWatch(filenameOrDirectory, false);
		}

		void handleFileAction( efsw::WatchID watchid, const std::string& dir, const std::string& filename, efsw::Action action, std::string oldFilename = "" )
		{
			switch( action )
			{
			case efsw::Actions::Add:
				PostCallbacks(dir, filename, mAddedCallbacks);
				break;
			case efsw::Actions::Delete:
				PostCallbacks(dir, filename, mDeletedCallbacks);
				break;
			case efsw::Actions::Modified:
				PostCallbacks(dir, filename, mModifiedCallbacks);
				break;
			case efsw::Actions::Moved:
				ProcessCallbacks(dir, oldFilename, filename, mMovedCallbacks);
				break;
			default:
				ECHO_LOG_ERROR("Received invalid EFSW action");
			}
		}
	private:
		typedef std::map< std::string, DirectoryModifiedCallback > CallbackMap;
		typedef std::map< std::string, FileMovedCallback > FileMovedCallbackMap;

		void PostCallbacks(std::string directory, std::string filename, CallbackMap& callbackMap)
		{
			ScopedLock lock(mMapMutex);
			auto it = callbackMap.find(filename);
			if(it==callbackMap.end())
			{
				it = callbackMap.find(directory+filename);
			}
			if(it==callbackMap.end())
			{
				// No callback registered for this file (could happen since the monitor works on directories)
				return;
			}

			// Get or create a timer for this
			shared_ptr<	boost::asio::deadline_timer >& callbackTimer = mCallbackTimers[directory+filename];

			if(!callbackTimer)
			{
				callbackTimer = make_shared<boost::asio::deadline_timer>(mCallbackService);
			}

			callbackTimer->expires_from_now(boost::posix_time::milliseconds(static_cast<s64>(mCallbackDelay.count())));

			auto callbackMapRef = std::ref(callbackMap);
			callbackTimer->async_wait([this,directory,filename,callbackMapRef](const boost::system::error_code& error)
			{
			  	if (!error)
				{
					ProcessCallbacks(directory,filename,callbackMapRef);
				}
			});
		}


		void ProcessCallbacks(std::string directory, std::string filename, CallbackMap& callbackMap)
		{
			ScopedLock lock(mMapMutex);
			bool fileNameFound = false;
			auto it = callbackMap.find(filename);
			if(it!=callbackMap.end())
			{
				it->second(directory, filename);
				fileNameFound = true;
			}
			
			it = callbackMap.find(directory+filename);
			if(it!=callbackMap.end())
			{
				it->second(directory, filename);
				fileNameFound = true;
			}
			
			// It could be a funky name like filename.x12345, where the extra extension includes the
			// PID. I saw this behaviour on Linux and it seems to be coming directly from the system
			// not from efsw. I did not find any documentation on this naming behaviour.
			if(!fileNameFound)
			{
				size_t lastPeriod = filename.find_last_of(".");
				if(lastPeriod!=std::string::npos)
				{
					filename = filename.substr(0,lastPeriod);
					it = callbackMap.find(filename);
					if(it!=callbackMap.end())
					{
						it->second(directory, filename);
					}

					it = callbackMap.find(directory+filename);
					if(it!=callbackMap.end())
					{
						it->second(directory, filename);
					}
				}
			}
		}
		
		void ProcessCallbacks(std::string directory, std::string oldFilename, std::string newFilename, FileMovedCallbackMap& callbackMap)
		{
			ScopedLock lock(mMapMutex);
			auto it = callbackMap.find(oldFilename);
			if(it!=callbackMap.end())
			{
				it->second(directory, oldFilename, newFilename);
			}
			
			it = callbackMap.find(directory+oldFilename);
			if(it!=callbackMap.end())
			{
				it->second(directory, oldFilename, newFilename);
			}
		}
		
		void UpdateWatch(std::string filename, bool recursive)
		{
			// See if we need to create a watch for this filename since we can only monitor
			// directories we need to figure out the directory first.
			
			using boost::filesystem::path;
			path filepath(filename);
			
			// if(!filepath.is_absolute())
			// {
			// 	filepath = boost::filesystem::current_path() / filepath;
			// }
			boost::system::error_code ec;
			std::string directory;
			if(boost::filesystem::is_directory(filepath, ec))
			{
				directory = filepath.string();
			}else
			{
				directory = filepath.parent_path().string();
			}
			if(recursive)
			{
				if(mRecursiveDirectoryWatches.find(directory)==mRecursiveDirectoryWatches.end())
				{
					mRecursiveDirectoryWatches[directory] = mFileWatcher.addWatch(directory,this,true);
				}
			}else
			{
				if(mDirectoryWatches.find(directory)==mDirectoryWatches.end())
				{
					mDirectoryWatches[directory] = mFileWatcher.addWatch(directory,this,false);
				}
			}
			if(!mIsWatching)
			{
				mIsWatching = true;
				mFileWatcher.watch();
			}
		}
		Mutex mMapMutex;
		CallbackMap mModifiedCallbacks;
		CallbackMap mAddedCallbacks;
		CallbackMap mDeletedCallbacks;
		FileMovedCallbackMap mMovedCallbacks;
		std::map< std::string, efsw::WatchID > mDirectoryWatches;
		std::map< std::string, efsw::WatchID > mRecursiveDirectoryWatches;
		efsw::FileWatcher mFileWatcher;
		bool mIsWatching;
		std::map< std::string, shared_ptr<boost::asio::deadline_timer> > mCallbackTimers;
		boost::asio::io_service mCallbackService;
		boost::asio::io_service::work mWork;
		Milliseconds mCallbackDelay;
		shared_ptr<Thread> mCallbackThread;
	};
}

#endif
