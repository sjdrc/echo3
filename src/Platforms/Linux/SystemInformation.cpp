#include <echo/Util/SystemInformation.h>
#include <echo/Util/ProcessorInformationHelper.h>
#include <echo/Util/RegEx.h>
#include <echo/Util/Configuration.h>
#include <echo/cpp/functional>

#include <fstream>
#include <sys/sysinfo.h>
#include <sys/utsname.h>

namespace Echo
{
	bool ReadInFile(std::string fileName, std::string& wholeFile)
	{
		//We don't need a FileSystem object because we know we are on Linux.
		std::ifstream file;
		file.open(fileName.c_str());
		if(!file.is_open())
		{
			ECHO_LOG_ERROR("GetCPUInformation() Unable to open file.");
			return false;
		}

		const size_t LINE_SIZE=4096;
		char line[LINE_SIZE];
		do
		{
		   file.getline(line, LINE_SIZE);
		   if(!file.good())
		   {
			   break;
		   }
		   wholeFile+=line;
		   wholeFile+="\n";
		} while (!file.eof());

		return true;
	}

	namespace SystemInformation
	{
		std::vector< Processor > GetCPUInformation()
		{
			std::string wholeFile;
			if(!ReadInFile("/proc/cpuinfo",wholeFile))
			{
				return std::vector< Processor >();
			}

			ProcessorInformationHelper processors;
			using namespace Utils::RegEx;
			//The results from the file are all accurate as they come from /proc/cpuinfo.
			ProcessRegExCapture<size_t>(wholeFile, "processor\\s*:\\s*(\\d*)", bind(&ProcessorInformationHelper::SetID,&processors,placeholders::_1,placeholders::_2,Accuracies::ACCURATE));
			ProcessRegExCapture<size_t>(wholeFile, "cache size\\s*:\\s*(\\d*)", bind(&ProcessorInformationHelper::SetCacheSize,&processors,placeholders::_1,placeholders::_2,Accuracies::ACCURATE));
			ProcessRegExCapture<size_t>(wholeFile, "cpu MHz\\s*:\\s*(\\d*)\\.(\\d*)", bind(&ProcessorInformationHelper::SetFrequency,&processors,placeholders::_1,placeholders::_2,Accuracies::ACCURATE));
			ProcessRegExCaptureBool(wholeFile, "fpu\\s*:\\s*([^\n]*)", bind(&ProcessorInformationHelper::SetHasFPU,&processors,placeholders::_1,placeholders::_2,Accuracies::ACCURATE));
			ProcessRegExCapture<std::string>(wholeFile, "model name\\s*:\\s*([^\n]*)", bind(&ProcessorInformationHelper::SetModelName,&processors,placeholders::_1,placeholders::_2,Accuracies::ACCURATE));
			ProcessRegExCapture<std::string>(wholeFile, "vendor_id\\s*:\\s*([^\n]*)", bind(&ProcessorInformationHelper::SetVendor,&processors,placeholders::_1,placeholders::_2,Accuracies::ACCURATE));
			ProcessRegExCapture<std::string>(wholeFile, "flags\\s*:\\s*([^\n]*)", bind(&ProcessorInformationHelper::SetFlags,&processors,placeholders::_1,placeholders::_2,Accuracies::ACCURATE));
			
			return processors.GetProcessors();
		}

		size_t GetTotalMemeory()
		{
			struct sysinfo memInfo;
			sysinfo (&memInfo);
			long long totalVirtualMem = memInfo.totalram;
			//Split these operations up to avoid overflow
			totalVirtualMem += memInfo.totalswap;
			totalVirtualMem *= memInfo.mem_unit;
			return totalVirtualMem;
		}
			 
		size_t GetMemeoryInUse()
		{
			struct sysinfo memInfo;
			sysinfo (&memInfo);
			long long virtualMemUsed = memInfo.totalram - memInfo.freeram;
			//Split these operations up to avoid overflow
			virtualMemUsed += memInfo.totalswap - memInfo.freeswap;
			virtualMemUsed *= memInfo.mem_unit;
			return virtualMemUsed;
		}
		
		OS GetOSInformation()
		{
			OS os;
			utsname unameinfo;
			uname(&unameinfo);
			
			os.mName = unameinfo.sysname;
			os.mNameAccuracy = Accuracies::ACCURATE;

			std::string wholeFile;
			Configuration configuration;
			
			if(ReadInFile("/etc/os-release",wholeFile) && configuration.ParseString(wholeFile))
			{
				if(configuration.GetOrFail("PRETTY_NAME", os.mDistribution))
				{
					os.mDistributionAccuracy = Accuracies::ACCURATE;
				}
			}
			
			os.mKernel=unameinfo.release;
			os.mKernelAccuracy=Accuracies::ACCURATE;
			os.mVersion=unameinfo.version;
			os.mVersionAccuracy=Accuracies::ACCURATE;
			return os;
		}
	};
}
