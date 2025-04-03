#include <echo/Logging/Logging.h>
#include <echo/Chrono/Chrono.h>
#include <echo/Util/StringUtils.h>
#include <echo/Kernel/ScopedLock.h>
#include <echo/Kernel/Mutex.h>
#include <boost/format.hpp>
#include <boost/utility/string_ref.hpp>

namespace Echo
{
	class Logger::Formatter : public boost::format
	{
	public:
		Formatter(std::string format) : boost::format(format)
		{
			
		}
	};
	
	Logger gDefaultLogger;

	Logger::Logger()
	{
		mMutex = std::unique_ptr<Mutex>(new Mutex());
		mLogMask = LogLevels::WARNING | LogLevels::ERROR | LogLevels::INFO;
		// Set to default
		SetLogOutputStream("stdout",&std::cout);
		mLogLevels["ERROR"] = LogLevels::ERROR;
		mLogLevels["INFO"] = LogLevels::INFO;
		mLogLevels["WARNING"] = LogLevels::WARNING;
		mLogLevels["DEBUG"] = LogLevels::DEBUG;
		mLogLevelLabels[LogLevels::ERROR] = "ERROR";
		mLogLevelLabels[LogLevels::INFO] = "INFO";
		mLogLevelLabels[LogLevels::WARNING] = "WARNING";
		mLogLevelLabels[LogLevels::DEBUG] = "DEBUG";
		mNextInternalLogLevel = LogLevels::DEBUG << 1;
		mRemoveFilePath = true;
		SetFormat("%1$-23s : %2$=8s : %3$30.30s : %4$=4s : %5$s");
	}
	
	void Logger::SetFormat(std::string format)
	{
		mFormat = format;
		mFormatter.reset(new Formatter(format));
	}
	
	const std::string& Logger::GetFormat() const
	{
		return mFormat;
	}
	
	void Logger::SetLogMask(Logger::LogMask logMask)
	{
		mLogMask = logMask;
	}
	
	void Logger::SetLogMask(const std::string& labelMask)
	{
		SetLogMask(ConvertLabelToLogMask(labelMask));
	}
	
	Logger::LogMask Logger::GetLogMask() const
	{
		return mLogMask;
	}
	
	void Logger::SetRemoveFilePath(bool removeFilePath)
	{
		mRemoveFilePath = removeFilePath;
	}
	
	bool Logger::GetRemoveFilePath() const
	{
		return mRemoveFilePath;
	}
	
	void Logger::EnableLogLevel(LogLevel level)
	{
		mLogMask |= level;
	}
	
	void Logger::DisableLogLevel(LogLevel level)
	{
		if(mLogMask & level)
		{
			mLogMask^=level;
		}
	}
	
	u64 Logger::RegisterCustomLogLevel(std::string name)
	{
		ScopedLock lock(*mMutex);
		u64 level = mNextInternalLogLevel;
		mLogLevels[name] = level;
		mLogLevelLabels[level] = name;
		mNextInternalLogLevel<<=1;
		return level;
	}
	
	void Logger::RegisterCustomLogLevel(std::string name, LogMask bitMask)
	{
		ScopedLock lock(*mMutex);
		mLogLevels[name] = bitMask;
		mLogLevelLabels[bitMask] = name;
	}
	
	void Logger::SetLogOutputStream(std::string name, std::ostream* outputStream)
	{
		ScopedLock lock(*mMutex);
		if(!outputStream)
		{
			return;
		}
		mNamedOutputStreams[name] = OutputStreamPair(outputStream, nullptr);
		mOutputStreams.push_back(outputStream);
	}

	void Logger::SetLogOutputStream(std::string name, shared_ptr<std::ostream> outputStream)
	{
		ScopedLock lock(*mMutex);
		mNamedOutputStreams[name] = OutputStreamPair(outputStream.get(), outputStream);
		mOutputStreams.push_back(outputStream.get());
	}

	bool Logger::RemoveLogOutputStream(const std::string& name)
	{
		ScopedLock lock(*mMutex);
		auto it = mNamedOutputStreams.find(name);
		if(it == mNamedOutputStreams.end())
		{
			return false;
		}

		// This is the target to match.
		std::ostream* target = it->second.first;

		// We're done with the named storage of it.
		mNamedOutputStreams.erase(it);

		for(auto vit = mOutputStreams.begin(); vit!=mOutputStreams.end(); ++vit)
		{
			if(*vit == target)
			{
				mOutputStreams.erase(vit);
				break;
			}
		}
		return true;
	}

	std::ostream* Logger::GetLogOutputStream(Size index) const
	{
		ScopedLock lock(*mMutex);
		if(index < mOutputStreams.size())
		{
			return mOutputStreams[index];
		}
		return nullptr;
	}

	std::ostream* Logger::GetLogOutputStream(const std::string& name) const
	{
		ScopedLock lock(*mMutex);
		auto it = mNamedOutputStreams.find(name);
		if(it!=mNamedOutputStreams.end())
		{
			return it->second.first;
		}
		return nullptr;
	}

	shared_ptr<std::ostream> Logger::GetSharedLogOutputStream(Size index) const
	{
		ScopedLock lock(*mMutex);
		if(index < mOutputStreams.size())
		{
			std::ostream* target = mOutputStreams[index];

			// This requires a long search in the map - name, < ostream*, shared_ptr<ostream*> >
			for(auto& namedPair : mNamedOutputStreams)
			{
				if(namedPair.second.first == target)
				{
					return namedPair.second.second;
				}
			}
		}
		return nullptr;
	}

	shared_ptr<std::ostream> Logger::GetSharedLogOutputStream(const std::string& name) const
	{
		ScopedLock lock(*mMutex);
		auto it = mNamedOutputStreams.find(name);
		if(it!=mNamedOutputStreams.end())
		{
			return it->second.second;
		}
		return nullptr;
	}

	void Logger::Log(LogLevel level, const char* file, int line, const std::string& message) const
	{
		if(mLogMask & level)
		{
			ScopedLock lock(*mMutex);
			LocklessLog(LocklessGetLogLevelLabel(level),file,line,message);
		}
	}

	void Logger::Log(const std::string& label, const char* file, int line, const std::string& message) const
	{
		ScopedLock lock(*mMutex);
		auto it = mLogLevels.find(label);
		if(it==mLogLevels.end())
		{
			return;
		}
		if(mLogMask & (it->second))
		{
			LocklessLog(label,file,line,message);
		}
	}

	void Logger::LocklessLog(const std::string& label, const char* file, int line, const std::string& message) const
	{
		if(mRemoveFilePath)
		{
			boost::string_ref fileRef = file;
			size_t pos = fileRef.find_last_of(boost::string_ref("/\\"));
			boost::string_ref filePartRef;
			if(pos!=boost::string_ref::npos)
			{
				filePartRef = fileRef.substr(pos+1);
			}else
			{
				filePartRef = fileRef.substr(pos);
			}
			for(std::ostream* stream : mOutputStreams)
			{
				(*stream) << (boost::format(*mFormatter) % Chrono::GetIOS8601() % label % filePartRef % line % message) << std::endl;
			}
		}else
		{
			for(std::ostream* stream : mOutputStreams)
			{
				(*stream) << (boost::format(*mFormatter) % Chrono::GetIOS8601() % label % file % line % message) << std::endl;
			}
		}
	}

	std::vector< std::string > Logger::GetLogOutputStreamLabels() const
	{
		ScopedLock lock(*mMutex);
		std::vector< std::string > labels;
		labels.reserve(mNamedOutputStreams.size());

		for(auto& namedPair : mNamedOutputStreams)
		{
			labels.push_back(namedPair.first);
		}
		return labels;
	}

	std::string Logger::GetLogLevelLabel(const LogLevel& logLevel) const
	{
		ScopedLock lock(*mMutex);
		return LocklessGetLogLevelLabel(logLevel);
	}
	
	std::string Logger::LocklessGetLogLevelLabel(const LogLevel& logLevel) const
	{
		auto it = mLogLevelLabels.find(logLevel);
		if(it != mLogLevelLabels.end())
		{
			return it->second;
		}
		return std::string();
	}

	Logger::LogMask Logger::ConvertLabelToLogMask(const std::string& label) const
	{
		ScopedLock lock(*mMutex);
		Logger::LogMask logMask = LogLevels::NONE;
		std::vector<std::string> labels;
		Utils::String::Split(label,"|",labels);
		for(auto l : labels)
		{
			auto it = mLogLevels.find(l);
			if(it != mLogLevels.end())
			{
				logMask |= static_cast<LogLevel>(it->second);
			}
		}
		return logMask;
	}
}
