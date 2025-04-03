#include <echo/Util/SystemInformation.h>
#include <sstream>
#include <gccore.h>

#define READU32(address) (*((u32*)address))

namespace Echo
{
	namespace SystemInformation
	{
		std::vector< Processor > GetCPUInformation()
		{
			std::vector< Processor > processors;
			Processor processor;
			processor.mFrequency = READU32(0x800000FC);				//Usually 0x2B73A840 for Wii
			processor.mFrequencyAccuracy = Accuracies::ACCURATE;
			processor.mID = 0;
			processor.mFrequencyAccuracy = Accuracies::ACCURATE;
			processors.push_back(processor);
			return processors;
		}

		size_t GetTotalMemeory()
		{
			// http://wiibrew.org/wiki/Memory_Map
			
			// The GameCube has one 24MB bank of 1T SRAM that is used for all code and data, spread across two
			// external chips; there is also a chip containing 16MB of ARAM, which could be used for storing data.

			// The Wii moves all 24MB of 1T-SRAM (referred to as MEM1) inside the Hollywood package, and adds
			// an additional 64MB of GDDR3 RAM (MEM2). During normal operation, IOS reserves the upper 12-16MB
			// of MEM2 for its own use; the rest can freely be used for code or data by running PPC code. MEM1
			// is slightly faster than MEM2.
			return READU32(0x80000028);	//Typically 0x01800000
		}

		size_t GetMemeoryInUse()
		{
			//Since we're not managing memory yet we don't know the memory usage.
			return 0;
		}
		
		OS GetOSInformation()
		{
			union IOSVersion
			{
				u32 mValue;
				struct _
				{
					u16 mIOSnumber;
					u8 mMajor;
					u8 mMinor;
				}Components;
			};
			IOSVersion iosVersion;
			iosVersion.mValue = READU32(0x80003140);
			
			OS os;
#ifdef HW_RVL
			os.mName = "Wii";
#else
			os.mName = "GCN";
#endif
			os.mNameAccuracy = Accuracies::HARD_CODED;
			
			std::stringstream ss;ss << "IOS" << iosVersion.Components.mIOSnumber << " v" << iosVersion.Components.mMajor << "." << iosVersion.Components.mMinor;
			os.mVersion = ss.str();
			os.mVersionAccuracy = Accuracies::ACCURATE;
			
			os.mDistribution = "N/A";
			os.mDistributionAccuracy = Accuracies::HARD_CODED;
			os.mKernel="Echo Engine";
			os.mKernelAccuracy = Accuracies::HARD_CODED;
			return OS();
		}
	};
}
