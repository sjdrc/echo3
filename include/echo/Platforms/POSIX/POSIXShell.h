#ifndef _ECHOPOSIXSHELL_H_
#define _ECHOPOSIXSHELL_H_
#include <echo/Shell/Shell.h>
#include <echo/Kernel/Mutex.h>
#include <stdio.h>

namespace Echo
{
	/**
	 * A POSIX compatible command shell.
	 */
	class POSIXShell : public Shell
	{
	public:
		static shared_ptr<POSIXShell> Create();

		~POSIXShell();
	private:
		POSIXShell();
		
		int _Execute(const std::string& workingDirectory, const std::string& command) override;
		void _WriteToStdIn(const char* buffer, size_t length) override;
		void KillProcess() override;
		bool SetEnvironment(const std::vector<std::string>& environmentKeyvalues) override;
		bool GetEnvironment(std::vector<std::string>& environmentOut) override;
		
		pid_t mPID;
		int mPipeToProcess[2];
		int mPipeFromProcess[2];
		std::vector<std::string> mEnvironment;
		Mutex mStdInMutex;
		std::string mPendingStdIn;

		/**
		 * Allocates a new buffer with arguments from the string
		 * Parses argsString, replacing separators with null characters and returns a vector of pointers within the string.
		 * If quotes are within the string simple open/close quote tracking is performed to allow spaces within quotes.
		 * @note This method overwrites characters in argsString.
		 * @return a vector containing pointers to each arguement (within the argString).
		 */
		std::vector<char*> ParseArgsString(std::string& argsString);
		
		std::vector<char*> BuildEnvironmentArray(const std::vector<std::string>& environment);
		
		bool ChangeDirectory(const std::string& directory);
	};
}
#endif
