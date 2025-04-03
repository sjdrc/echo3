#include <echo/Shell/Shell.h>
#include <echo/cpp/functional>
#include <boost/circular_buffer.hpp>
#include <boost/foreach.hpp>
#include <echo/cpp/functional>

namespace Echo
{
	const size_t SHELL_DEFAULT_OUTPUT_BUFFER_SIZE=2048;
	Shell::Shell() :
		mStdOut(SHELL_DEFAULT_OUTPUT_BUFFER_SIZE),
		mCaptureStdOut(true),
		mStdOutAsLines(true),
		mStdOutChunkSize(0),
		mInheritEnvironment(false),
		mOutputEnvironmentOnExecute(false),
		mQuiet(false),
		mWaitOnDelete(true),
		mProcessThread("Shell thread")
	{
	}

	Shell::~Shell()
	{
		StopThread(mWaitOnDelete);
	}

	int Shell::Execute(const std::string& command, bool runOnThread)
	{
		mProcessThread.SetThreadFunction(bind(&Shell::DoExecute,this,command));
		//Clear the stdout and stderr buffers and add a blank line ready for data.
		mStdOut.clear();
		mStdOut.push_back(std::string());
		
		if(runOnThread)
		{
			if(!mProcessThread.Execute())
			{
				return 1;
			}
			return 0;

		}
		return DoExecute(command);
	}
	
	void Shell::EndOfProcess(int returnValue)
	{
		if(!mStdOut.back().empty() && mStdOutCallback)
		{
			mStdOutCallback(*this,mStdOut.back());
		}
		if(mProcessCompleteCallback)
		{
			mProcessCompleteCallback(*this,returnValue,mStdOut);
		}
	}

	void Shell::SetStdOutCallback(OutputCallback stdOutCallback)
	{
		mStdOutCallback=stdOutCallback;
	}

	void Shell::SetProcessFinishedCallback(ProcessFinishedCallback finishedCallback)
	{
		 mProcessCompleteCallback=finishedCallback;
	}

	void Shell::WriteToStdIn(const std::string& data)
	{
		WriteToStdIn(data.c_str(),data.length());
	}

	void Shell::WriteToStdIn(const char* data, size_t dataLength)
	{
		_WriteToStdIn(data,dataLength);
	}
		
	const boost::circular_buffer< std::string >& Shell::GetStdOut() const
	{
		return mStdOut;
	}

	void Shell::SetCaptureStdOut(bool capture)
	{
		mCaptureStdOut=capture;
	}

	void Shell::SetStdOutLineBufferSize(size_t numberOfLines)
	{
		mStdOut=boost::circular_buffer<std::string>(numberOfLines);
	}

	void Shell::SetStdOutChunkSize(Size chunkSize)
	{
		mStdOutChunkSize = chunkSize;
		mStdOutAsLines = (chunkSize==0);
	}
	
	void Shell::SetWorkingDirectory(const std::string& workingDirectory)
	{
		mWorkingDirectory=workingDirectory;
	}
	
	void Shell::ClearEnvironment()
	{
		mEnvironment.clear();
	}

	void Shell::AddEnvironmentString(const std::string& environmentString)
	{
		mEnvironment.push_back(environmentString);
	}

	void Shell::SetEnvironmentStrings(const std::vector<std::string>& environment)
	{
		mEnvironment=environment;
	}
	
	void Shell::SetInheritEnvironment(bool inherit)
	{
		mInheritEnvironment=inherit;
	}

	void Shell::NewStdOutData(const char* buffer, size_t bufferLength)
	{
		if(!mCaptureStdOut)
		{
			return;
		}
		std::string* currentLine = &mStdOut.back();

		if(mStdOutAsLines)
		{
			for(size_t i=0;i<bufferLength;++i)
			{
				switch(buffer[i])
				{
					case '\n':
						//Create a new line
						if(mStdOutCallback)
						{
							mStdOutCallback(*this,*currentLine);
						}
						mStdOut.push_back(std::string());
						currentLine = &mStdOut.back();
						break;
					case '\r':
						//The line has completed but going to be overwritten.
						if(mStdOutCallback)
						{
							mStdOutCallback(*this,*currentLine);
						}
						currentLine->clear();
						break;
					default:
						//Append this character to the line.
						(*currentLine)+=buffer[i];
						break;
				}
			}
		}else
		{
			Size currentSize = currentLine->size();
			Size newSize = currentSize + bufferLength;
			while(newSize >= mStdOutChunkSize)
			{
				Size thisSize = mStdOutChunkSize - currentSize;
				currentLine->append(buffer,thisSize);
				if(mStdOutCallback)
				{
					mStdOutCallback(*this,*currentLine);
				}
				currentLine->clear();
				buffer+=thisSize;
				bufferLength-=thisSize;
				newSize=bufferLength;
			}
			// At this point - we should be under a chunk size so we can copy the bufferLength
			if(bufferLength > 0)
			{
				currentLine->append(buffer,bufferLength);
			}
		}
	}
	
	void Shell::StopThread(bool waitForThread)
	{
		mProcessThread.Terminate(waitForThread);
	}
	
	void Shell::Join()
	{
		mProcessThread.Join();
	}

	int Shell::DoExecute(const std::string& command)
	{
		//Get the inherited environment if necessary.
		std::vector<std::string> inheritedEnvironment;
		if(mInheritEnvironment)
		{
			if(!GetEnvironment(inheritedEnvironment))
			{
				ECHO_LOG_ERROR("Shell::DoExecute() Unable to inherit environment. Unsupported.");
				return 1;
			}
		}
		//Combing the inherited environment with the added environment settings.
		std::vector<std::string> finalEnvironment;
		finalEnvironment.reserve(inheritedEnvironment.size() + mEnvironment.size());
		finalEnvironment.insert(finalEnvironment.end(), inheritedEnvironment.begin(), inheritedEnvironment.end());
		finalEnvironment.insert(finalEnvironment.end(),mEnvironment.begin(), mEnvironment.end());

		//Set the environment.
		if(!SetEnvironment(finalEnvironment))
		{
			ECHO_LOG_ERROR("Shell::DoExecute() Unable to set environment. Unsupported.");
			return 1;
		}

		if(!mQuiet)
		{
			ECHO_LOG_INFO("Executing: \"" << command << "\" in \"" << mWorkingDirectory << "\"");
			ECHO_LOG_INFO("Environment: " << finalEnvironment.size() << " variables.");
		}
		if(mOutputEnvironmentOnExecute)
		{
			BOOST_FOREACH(const std::string& s, finalEnvironment)
			{
				ECHO_LOG_INFO("\t" << s);
			}
		}

		// This little trick is to prevent a deadlock when the end process callback releases
		// the last reference. If the thread is the last thing to have a reference to itself
		// via this shared pointer then we won't wait on clean up.
		mWaitOnDelete = true;
		shared_ptr<Shell> myself = shared_from_this();
		
		//Execute.
		int returnValue=_Execute(mWorkingDirectory,command);

		//Process callbacks.
		EndOfProcess(returnValue);
		
		// See comment above about this pointy bit
		if (myself.use_count()==1)
		{
			mWaitOnDelete = false;
		}
		return returnValue;
	}
}
