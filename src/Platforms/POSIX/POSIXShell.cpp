#include <echo/Platforms/POSIX/POSIXShell.h>
#include <echo/Util/StringUtils.h>
#include <echo/Kernel/ScopedLock.h>
#include <sys/wait.h>
#include <wordexp.h>
#include <boost/foreach.hpp>
#ifdef ECHO_PLATFORM_LINUX
#include <sys/prctl.h>
#endif

namespace Echo
{
	const size_t READ=0;
	const size_t WRITE=1;

	shared_ptr<POSIXShell> POSIXShell::Create()
	{
		return shared_ptr<POSIXShell>(new POSIXShell());
	}
	
	POSIXShell::POSIXShell() : mPID(0)
	{
	}
	
	POSIXShell::~POSIXShell()
	{
		KillProcess();
	}
	
	void POSIXShell::_WriteToStdIn(const char* buffer, size_t length)
	{
		ScopedLock locky(mStdInMutex);
		if(mPID!=0 && buffer!=nullptr && length!=0)
		{
			//Write to the write end of the pipe to the process.
			size_t bytes = write(mPipeToProcess[WRITE],buffer,length);
			if (bytes != length)
			{
				ECHO_LOG_WARNING("Could not write all bytes to stdin");
			}
		}else
		{
			mPendingStdIn+=std::string(buffer,length);
		}
	}
	
	int POSIXShell::_Execute(const std::string& workingDirectory, const std::string& command)
	{
		if(pipe(mPipeToProcess) != 0 || pipe(mPipeFromProcess) !=0)
		{
			return -1;
		}
		
		#ifdef ECHO_PLATFORM_LINUX
		pid_t ppidBeforeFork = getpid();
		#endif
		mPID = fork();
		if(mPID < 0)
			return 1;

		//Managing file handlers - Documentation taken from http://tldp.org/LDP/lpg/node11.html
		//If the parent wants to receive data from the child, it should close fd1, and the child should close fd0.
		//If the parent wants to send data to the child, it should close fd0, and the child should close fd1.
		
		if(mPID==0)
		{
			#ifdef ECHO_PLATFORM_LINUX
			// This isn't POSIX compliant. According to comments here https://stackoverflow.com/questions/284325/how-to-make-child-process-die-after-parent-exits
			// there isn't a POSIX way to do it.
			int r = prctl(PR_SET_PDEATHSIG, SIGHUP);
			if (r == -1)
			{
				perror(0);
				exit(1);
			}
			if (getppid() != ppidBeforeFork)
			{
				exit(1);
			}
			#endif
			
			std::string arrgsString=command;
			std::vector<char*> argsStrings=ParseArgsString(arrgsString);
			if(argsStrings.empty())
			{
				ECHO_LOG_ERROR("Could not determine command in \"" << command << "\"");
				return 127;
			}

			if(!workingDirectory.empty() && !ChangeDirectory(workingDirectory))
			{
				ECHO_LOG_ERROR("Unable to change directory to \"" << workingDirectory << "\"");
				return 127;
			}
			
			//Child process
			close(mPipeToProcess[WRITE]);			//Child closes the input side.
			dup2(mPipeToProcess[READ], STDIN_FILENO);		//Redirect the output side of the pipe to get the input of stdin
			
			close(mPipeFromProcess[READ]);			//Child closes output side
			dup2(mPipeFromProcess[WRITE], STDOUT_FILENO);	//Redirect the input side of the pipe to get the output of stdout
			dup2(mPipeFromProcess[WRITE], STDERR_FILENO);	//Redirect the input side of the pipe to get the output of stderr

			argsStrings.push_back(nullptr);
			if(mEnvironment.empty())
			{
				execvp(argsStrings[0],&argsStrings[0]);
				perror("execvp");
			}else
			{
				std::vector<char*> environmentStrings=BuildEnvironmentArray(mEnvironment);
				if(environmentStrings.empty())
				{
					ECHO_LOG_ERROR("Could not build environment list.");
					return 127;
				}
				environmentStrings.push_back(nullptr);
				execvpe(argsStrings[0],&argsStrings[0],&environmentStrings[0]);
				perror("execvpe");
			}
			
			exit(errno);
		}
		
		//Parent process
		close(mPipeToProcess[READ]);			//Parent closes the output side.
		close(mPipeFromProcess[WRITE]);			//Parent closes the input side.
		
		{
			ScopedLock locky(mStdInMutex);
			//Write to the write end of the pipe to the process.
			size_t bytes = write(mPipeToProcess[WRITE],mPendingStdIn.c_str(),mPendingStdIn.length());
			if (bytes != mPendingStdIn.length())
			{
				ECHO_LOG_WARNING("Could not write all bytes to stdin");
			}
			mPendingStdIn.clear();
		}
		
		//Read and process stdoout.
		const size_t BUFFER_SIZE=128;
		char buffer[BUFFER_SIZE+1];
		std::fill(buffer,&buffer[BUFFER_SIZE],0);
		std::string result = "";
		while(1)
		{
			ssize_t count = read(mPipeFromProcess[READ], buffer, BUFFER_SIZE);
			if(count<=0)
			{
				break;
			}
			NewStdOutData(buffer,count);
		}
		
		int status=0;
		int returnValue=0;
		waitpid(mPID,&status,WNOHANG);
		if ( WIFEXITED(status) ) 
		{ 
			returnValue = WEXITSTATUS(status);
		}else
		{
			ECHO_LOG_WARNING("Child process did not exit normally.");
			// Indicate failure so "success" isn't assumed by the caller
			returnValue = 1;
		}
		
		mPID=0;
		close(mPipeToProcess[WRITE]);
		close(mPipeFromProcess[READ]);
		return returnValue;
	}

	void POSIXShell::KillProcess()
	{
		if(mPID)
		{
			kill(mPID,SIGKILL);
		}
	}
	
	bool POSIXShell::SetEnvironment(const std::vector<std::string>& environmentKeyvalues)
	{
		mEnvironment=environmentKeyvalues;
		return true;
	}
	
	bool POSIXShell::GetEnvironment(std::vector<std::string>& environmentOut)
	{
		//GNU environ is a char** of all environment variables.
		size_t i = 0;
		while(environ[i])
		{
			environmentOut.push_back(std::string(environ[i]));
			++i;
		}
		return true;
	}

	std::vector<char*> POSIXShell::ParseArgsString(std::string& argsString)
	{
		size_t length = argsString.length();
		std::vector<char*> argPositions;

		bool quoteOpen=false;
		bool lookingForCharacter=true;
		size_t argc=0;
		for(size_t i=0;i<length;++i)
		{
			if(argsString[i]=='"')
			{
				quoteOpen = !quoteOpen;
				argsString[i]=0;
				if(!quoteOpen)
				{
					lookingForCharacter=true;
				}
			}else
			if(argsString[i]==' ' || argsString[i]=='\t')
			{
				if(!quoteOpen)
				{
					lookingForCharacter=true;
					argsString[i]=0;
				}
			}else
			if(lookingForCharacter)
			{
				argPositions.push_back(&argsString[i]);
				argc++;
				lookingForCharacter=false;
			}
		}
		return argPositions;
	}
	
	std::vector<char*> POSIXShell::BuildEnvironmentArray(const std::vector<std::string>& environment)
	{
		std::vector<char*> environmentArray;
		BOOST_FOREACH(std::string& s, mEnvironment)
		{
			environmentArray.push_back(const_cast<char*>(s.c_str()));
		}
		return environmentArray;
	}
	
	bool POSIXShell::ChangeDirectory(const std::string& directory)
	{
		int r=chdir(directory.c_str());
		if(r==0)
		{
			return true;
		}
		switch(r)
		{
			case EACCES:		ECHO_LOG_ERROR("Search permission is denied for one of the components of path."); break;
			case EFAULT:		ECHO_LOG_ERROR("The path parameter points outside your accessible address space."); break;
			case EIO:			ECHO_LOG_ERROR("An I/O error occurred."); break;
			case ELOOP:			ECHO_LOG_ERROR("Too many symbolic links were encountered in resolving path."); break;
			case ENAMETOOLONG:	ECHO_LOG_ERROR("path is too long."); break;
			case ENOENT:		ECHO_LOG_ERROR("The directory does not exist."); break;
			case ENOMEM:		ECHO_LOG_ERROR("Insufficient kernel memory was available."); break;
			case ENOTDIR:		ECHO_LOG_ERROR("A component of path is not a directory."); break;
		}
		return false;
	}
}
