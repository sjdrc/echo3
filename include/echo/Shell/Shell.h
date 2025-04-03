#ifndef _ECHOSHELL_H_
#define _ECHOSHELL_H_
#include <echo/Kernel/Thread.h>
#include <echo/cpp/functional>
#include <boost/circular_buffer.hpp>
#include <vector>

namespace Echo
{
	/**
	 * Shell provides access to a command shell.
	 * To create a platform specific shell, use Platform::CreateShell();
	 * @note Implementations should call KillProcess() within the destructor to ensure the child
	 * process is stopped before the shell is cleaned up. Shell destructor will wait on the
	 * background thread to complete if it is running in the background.
	 */
	class Shell : public enable_shared_from_this<Shell>
	{
	public:
		//Callback receives the shell associated with the line or chunk.
		typedef function < void(Shell&, const std::string&) > OutputCallback;
		//Callback receives the shell assosicated with the process, the error code and stdout buffer.
		typedef function < void(Shell&, int, boost::circular_buffer< std::string >) > ProcessFinishedCallback;
		
		/**
		 * Destructor
		 */
		virtual ~Shell();

		/**
		 * Execute a shell command.
		 * @note Calling Execute clears the stdout buffer.
		 * @param runOnThread if true the process will be executed and managed on another thread, otherwise the
		 * method will block until the process has finished. Keep in mind that if the process runs on a separate
		 * thread then all callbacks will be called on that thread.
		 * @return 0 if the command is executed on another thread, inwhich case use the process finished callback
		 * to get thre return result. Otherwise this method returns the return value from issuing the command.
		 */
		int Execute(const std::string& command, bool runOnThread);
		
		/**
		 * Set the callback for lines or chunk output to stdout.
		 * The callback will be invoked for each line that is read from stdout.
		 */
		void SetStdOutCallback(OutputCallback stdOutCallback);

		/**
		 * Set the callback for completion of a process.
		 * The callback will be invoked when a process completes.
		 */
		void SetProcessFinishedCallback(ProcessFinishedCallback finishedCallback);
		
		/**
		 * Write to stdin of the running process.
		 * If a process is not running this command returns.
		 * @param data
		 */
		void WriteToStdIn(const std::string& data);
		
		/**
		 * Write to stdin of the running process.
		 * If a process is not running this command returns.
		 * @param data
		 */
		void WriteToStdIn(const char* data, size_t dataLength);
		
		/**
		 * Get the buffer for the stdout.
		 * @note the buffer may be modified if the process is still running and another thread was used
		 * to launch the process. This may affect lines that you are trying to read. If you're interested
		 * in performing analysis of stdout over multiple lines while running multiple threads consider
		 * using a stdout callback and maintaining your own list so you can make it thread safe.
		 * @return the circular buffer of std out.
		 */
		const boost::circular_buffer< std::string >& GetStdOut() const;

		/**
		 * Tell the Shell object to capture the output from stdout.
		 * Default is to capture stdout.
		 * @param store
		 */
		void SetCaptureStdOut(bool capture);
		
		/**
		 * Set the number of lines to store for the stdout buffer.
		 * The default is 2048.
		 * @note Calling this method will clear th
		 * @param the maximum number of lines from stdout to store.
		 */
		void SetStdOutLineBufferSize(size_t numberOfLines);

		/**
		 * Set the stdout chunk Size.
		 * If this is 0 then std out call back will return for each new line. Otherwise every chunkSize bytes
		 * that are output will cause the callback to be returned.
		 * Any bytes in the buffer that do not make up a full chunk will be returned when the process ends.
		 * @param chunkSize The number of bytes to buffer from stdout before calling the callback.
		 */
		void SetStdOutChunkSize(Size chunkSize);

		/**
		 * Set the working directory for the process when executed.
		 * @param workingDirectory
		 */
		void SetWorkingDirectory(const std::string& workingDirectory);
		
		/**
		 * Clear the environment strings.
		 */
		void ClearEnvironment();

		/**
		 * Add an environment string to use when executing.
		 * @param environment string, usually containing a string "VARIABLE=value".
		 */
		void AddEnvironmentString(const std::string& environmentString);

		/**
		 * Set the environment vector.
		 * Useful if you have a vector already and don't want to iterate over it calling AddEnvironmentString().
		 */
		void SetEnvironmentStrings(const std::vector<std::string>& environment);
		
		/**
		 * Set whether or not the child process should inherit the parent's environment.
		 * @param inherit
		 */
		void SetInheritEnvironment(bool inherit);
		
		/**
		 * Set whether or not to output the environment to stdout on execute.
		 * Default is not to display.
		 */
		void SetOutputEnvironmentOnExecute(bool output) {mOutputEnvironmentOnExecute=output;}

		/**
		 * Get whether or not to output the environment to stdout on execute.
		 */
		bool GetOutputEnvironmentOnExecute() const {return mOutputEnvironmentOnExecute;}

		/**
		 * Turn quiet on to surpress startup output
		 */
		void SetQuiet(bool quiet) {mQuiet=quiet;}

		/**
		 * Get whether or not additional output will be enabled.
		 */
		bool GetQuiet() const {return mQuiet;}
		
		/**
		 * Kill the child process if it is running.
		 */
		virtual void KillProcess() = 0;
		
		/**
		 * Join the sub process as though it was a thread.
		 * @note This is only effective if the sub process is running on a background thread since
		 * we internally join on the background thread and not the process.
		 */
		void Join();

		/**
		 * Stop the background thread. This calls Thread::Terminate(waitForThread)
		 */
		void StopThread(bool waitForThread);
	protected:
		/**
		 * Platform implementers should call this method when data is available for stdout line.
		 * @note This method is designed to make managing the data coming in simpler for platform
		 * implementations. Detection of lines and calling of line callbacks is managed by the
		 * base class.
		 * @note This method does not read past the end of the buffer, so you don't need to null
		 * terminate it.
		 * @param buffer the new available data.
		 * @param bufferLength the length of the new data.
		 */
		void NewStdOutData(const char* buffer, size_t bufferLength);
		
		/**
		 * Constructor.
		 */
		Shell();
	private:
		boost::circular_buffer< std::string > mStdOut;
		bool mCaptureStdOut;
		bool mStdOutAsLines;
		bool mInheritEnvironment;
		bool mOutputEnvironmentOnExecute;
		bool mQuiet;
		bool mWaitOnDelete;		/// Used to prevent a deadlock when the last external reference is released in the end callback
		std::string mWorkingDirectory;
		std::vector<std::string> mEnvironment;
		OutputCallback mStdOutCallback;
		Size mStdOutChunkSize;
		ProcessFinishedCallback mProcessCompleteCallback;
		Thread mProcessThread;
		
		/**
		 * Performs setup of environment, calls execute and processes final callbacks.
		 * @param command The command to execute.
		 * @return the return value of the command. If there is an error before _Execute() can be called or if
		 * the thread is interrupted the method will return a value of 1.
		 */
		int DoExecute(const std::string& command);
		
		/**
		 * Perform final buffer handling and calls the end of process callback.
		 * @param returnValue the return value returned by _Execute();
		 */
		void EndOfProcess(int returnValue);
		
		/**
		 * Internal method to execute a process.
		 * @note This method should block. Running in a separate thread is done in the shared implementation.
		 * @note A platform should only support running one command at a time. The user should use multiple
		 * Shell objects to run processes concurrently.
		 * @note Call NewStdOutData() when data becomes available from stdout.
		 * @param command the command to execute.
		 * @param workingDirectory the working directory to execute the command from.
		 * @return The return code of the command, if the command cannot execute as best to conform
		 * with http://www.tldp.org/LDP/abs/html/exitcodes.html, 127 is command not found.
		 */
		virtual int _Execute(const std::string& workingDirectory, const std::string& command)=0;

		/**
		 * Internal method to set the environment before _Execute() is called.
		 * @note This method is called before _Execute().
		 * @note Some platforms may require the environment to be set during the _Execute() method, in which case
		 * the implementation should copy the vector to use later.
		 * @note If this method returns false _Execute() will not be called.
		 * @param environmentKeyvalues environment vector.
		 * @return true if the environment was set (or will be set later), false if this is unsupported.
		 */
		virtual bool SetEnvironment(const std::vector<std::string>& environmentKeyvalues) = 0;
		
		/**
		 * Get all the all environment variables and place them in the environment out vector.
		 * The implementation should add the environment as VARIABLE=value each as a separate entry.
		 * @note The implementation need not clear the vector.
		 * @note This method is called called before SetEnvironment() if mInheritEnvironment is true.
		 * @note This method needs to query the system for environment variables and should not return values
		 * from mEnvironment, as mEnvironment is likely to be the parameter passed in.
		 * @return true if the environmentOut vector contains the environment, false if this is unsupported.
		 */
		virtual bool GetEnvironment(std::vector<std::string>& environmentOut)=0;

		/**
		 * Internal method to write data to stdin of the running process.
		 * @param buffer the buffer containing the data to write.
		 * @param length the length, in bytes of the buffer
		 */
		virtual void _WriteToStdIn(const char* buffer, size_t length)=0;
	};
}
#endif
