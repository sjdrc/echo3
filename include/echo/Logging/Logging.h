#ifndef _ECHO_LOGGING_H_
#define _ECHO_LOGGING_H_

#include <echo/Types.h>
#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#undef ERROR

namespace Echo
{
	class Mutex;

	/**
	 * Logger that writes to multiple output streams.
	 * The default output stream is added with SetLogOutputStream("stdout",&std::cout).
	 */
	class Logger
	{
	public:
		struct LogLevels
		{
			enum _
			{
				NONE = 0,
				ERROR = 1,
				WARNING = 2,
				INFO = 4,
				DEBUG = 8
			};
		};
		typedef LogLevels::_ LogLevel;
		typedef u64 LogMask;
		
		Logger();
		
		/**
		 * Set the log format.
		 * Internally we are using boost format. Refer to the documentation for full formatter options.
		 * Parameters are passed to the formatter in this order:
		 *		timestamp,level,file,line,message
		 * You can change the order by specifying positions.
		 * The default format is "%1$-23s : %2$=8s : %3$40.40s : %4$=4s : %5$s"
		 */
		void SetFormat(std::string format);
		
		const std::string& GetFormat() const;

		/**
		 * Log a message at the given log level.
		 */
		void Log(LogLevel level, const char* file, int line, const std::string& message) const;

		/**
		 * Log a message with the level specified by label.
		 */
		void Log(const std::string& label, const char* file, int line, const std::string& message) const;

		/**
		 * Set the log mask.
		 * @param mask
		 */
		void SetLogMask(LogMask mask);

		/**
		 * Set the log mask using labels.
		 * @note The labels need to be registered before this is called.
		 * @note Unknown labels are ignored.
		 * @param labels | separated string of labels
		 */
		void SetLogMask(const std::string& labelMask);
		void EnableLogLevel(LogLevel level);
		void DisableLogLevel(LogLevel level);
		LogMask GetLogMask() const;
		LogMask RegisterCustomLogLevel(std::string name);
		void RegisterCustomLogLevel(std::string name, LogMask bitMask);
		
		/**
		 * Set whether or not to remove the path component of the file input when displaying.
		 * This can be useful if you use absolute paths that would otherwise make the file section long.
		 */
		void SetRemoveFilePath(bool removeFilePath);
		
		/**
		 * Get whether or not file path removal is enabled.
		 */
		bool GetRemoveFilePath() const;

		/**
		 * Set the logging output stream.
		 * The order that streams are logged to is the order that they are set. If you remove a stream by name then set using the
		 * same name then it will be logged to last.
		 * @note Since this method does not manage ownership at all the caller is responsible for ensuring that the stream is removed
		 * appropriately to ensure the stream is not used after destruction. This exists for non-application managed objects such as
		 * std::cout. You are otherwise encouraged to use the managed overload of this method.
		 * @param name The name of this output stream. This can be used later to remove.
		 * @param outputStream the new output stream, if this is null no action will be taken.
		 */
		void SetLogOutputStream(std::string name, std::ostream* outputStream);

		/**
		 * Set the logging output stream.
		 * @param name The name of this output stream. This can be used later to remove the stream.
		 * @param outputStream the new output stream, if this is null no action will be taken.
		 */
		void SetLogOutputStream(std::string name, shared_ptr<std::ostream> outputStream);

		/**
		 * Get the output stream.
		 * @return std::ostream* 
		 */
		std::ostream* GetLogOutputStream(Size index = 0) const;

		/**
		 * Get the output stream by name
		 * @return std::ostream* 
		 */
		std::ostream* GetLogOutputStream(const std::string& name) const;

		/**
		 * Get shared output stream.
		 * @return std::ostream 
		 */
		shared_ptr<std::ostream> GetSharedLogOutputStream(Size index = 0) const;

		/**
		 * Get shared output stream by name
		 * @return std::ostream
		 */
		shared_ptr<std::ostream> GetSharedLogOutputStream(const std::string& name) const;

		/**
		 * Remove an output stream
		 * @param name The name of the stream.
		 */
		bool RemoveLogOutputStream(const std::string& name);

		/**
		 * Get the log output stream labels
		 * @return std::vector< std::string > Labels, these can be used to remove.
		 */
		std::vector< std::string > GetLogOutputStreamLabels() const;

		std::string GetLogLevelLabel(const LogLevel& logLevel) const;
		LogMask ConvertLabelToLogMask(const std::string& label) const;
	private:
		inline void LocklessLog(const std::string& label, const char* file, int line, const std::string& message) const;
		inline std::string LocklessGetLogLevelLabel(const LogLevel& logLevel) const;
		LogMask mLogMask;
		std::vector< std::ostream* > mOutputStreams;
		std::map< u64, std::string > mLogLevelLabels;
		std::map< std::string, u64 > mLogLevels;
		typedef std::pair<std::ostream*, shared_ptr<std::ostream> > OutputStreamPair;
		std::map< std::string, OutputStreamPair > mNamedOutputStreams;
		u64 mNextInternalLogLevel;
		bool mRemoveFilePath;
		// Using the opaque pointer idiom so we don't pull in lots of files for formatting
		class Formatter;
		shared_ptr<Formatter> mFormatter;
		std::string mFormat;
		mutable unique_ptr<Mutex> mMutex;
	};
	
	extern Logger gDefaultLogger;

#define ECHO_LOG_(level,file,line,message)\
	do{\
		std::stringstream ss;\
		ss << message;\
		Echo::gDefaultLogger.Log(level,file,line,ss.str());\
	}while(false)

#define ECHO_LOG_DEBUG(message) ECHO_LOG_(Echo::Logger::LogLevels::DEBUG, __FILE__, __LINE__, message)
#define ECHO_LOG_ERROR(message) ECHO_LOG_(Echo::Logger::LogLevels::ERROR, __FILE__, __LINE__, message)
#define ECHO_LOG_WARNING(message) ECHO_LOG_(Echo::Logger::LogLevels::WARNING, __FILE__, __LINE__, message)
#define ECHO_LOG_INFO(message) ECHO_LOG_(Echo::Logger::LogLevels::INFO, __FILE__, __LINE__, message)
#define ECHO_LOG(label,message) ECHO_LOG_(std::string(label), __FILE__, __LINE__, message)
}

#endif
