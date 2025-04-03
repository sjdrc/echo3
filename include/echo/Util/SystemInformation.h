#ifndef _ECHOSYSTEMINFORMATION_H_
#define _ECHOSYSTEMINFORMATION_H_

#include <echo/Platform.h>
#include <boost/algorithm/string/find.hpp>
#include <vector>

namespace Echo
{
	/**
	 * Use the functions here to get system information.
	 * Keep in mind that not all platforms will support all functions here.
	 * 
	 * Some functions will return hard coded values because the hardware they are targeting
	 * shouldn't change. This is important to mention because you cannot rely on all of
	 * these functions for determining hardware limits. For example, CPU info might be hard
	 * coded for a console but the developer hardware for that console might have more
	 * hardware capabilities for debugging purposes. As such each memeber in the structures
	 * that are returned by the functions include an "Accuracy" member for each memeber. For
	 * example, the CPU::Processor::mFrequency has a complementary
	 * CPU::Processor::mFrequencyAccuracy which can be used to determine how accurate the
	 * mFrequency member information is.
	 */
	namespace SystemInformation
	{
		struct Accuracies
		{
			enum _
			{
				UNKNOWN,			//!< The information is unknown.
				GUESS,				//!< For some reason a guess has been made, check the platform
									//	 specific implementation
				HARD_CODED,			//!< The information is hardcoded, probably because the target
									//   is a console or some other specific device.
				ACCURATE			//!< The information is accurate.
			};
		};
		typedef Accuracies::_ Accuracy;
		
		struct Processor
		{
			Processor() : 
				mID(UNKNOWN_ID),
				mIDAccuracy(Accuracies::UNKNOWN),
				mCacheSize(0),
				mCacheSizeAccuracy(Accuracies::UNKNOWN),
				mFrequency(0),
				mFrequencyAccuracy(Accuracies::UNKNOWN),
				mVendorAccuracy(Accuracies::UNKNOWN),
				mModelNameAccuracy(Accuracies::UNKNOWN),
				mHasFPU(false),
				mHasFPUAccuracy(Accuracies::UNKNOWN),
				mFlagsAccuracy(Accuracies::UNKNOWN)
			{}
			~Processor()
			{
			}
			static const size_t UNKNOWN_ID = 0xFFFFFFFF;	//!< Although not 64bit max, even as a 32bit max this is a pretty massive number.
			size_t mID;					//!< The ID given by the system, used to identify the CPU, UNKNOWN_ID if unknown.
			Accuracy mIDAccuracy;		//!< The accuracy flag for mID.
			size_t mCacheSize;			//!< The cache size in KiBi. 0 if unknown.
			Accuracy mCacheSizeAccuracy;//!< The accuracy flag for mCacheSize.
			size_t mFrequency;			//!< In Khz, 1Ghz will be 1000KHz. 0 if unknown.
			Accuracy mFrequencyAccuracy;//!< The accuracy flag for mFrequency.
			std::string mVendor;		//!< Empty if unknown.
			Accuracy mVendorAccuracy;	//!< The accuracy flag for mVendor.
			std::string mModelName;		//!< Empty if unknown.
			Accuracy mModelNameAccuracy;//!< The accuracy flag for mModelName.
			bool mHasFPU;				//!< True if the processor has a FPU, otherwise undefined.
			Accuracy mHasFPUAccuracy;	//!< The accuracy flag for mHasFPU.
			std::string mFlags;			//!< The flags indicating feature support for this processor.
			Accuracy mFlagsAccuracy;	//!< The accuracy flag for mFlags.

			/**
			 * Determine if the CPU supports a feature via the flags string.
			 * This method searches mFlags to a flag that matches the provided flag.
			 * @param flag The flag to look up.
			 * @return true if the flag is supported, otherwise false.
			 */
			inline bool HasFlag(const std::string& flag) const
			{
				return (boost::find_first(mFlags, flag));
			}
			std::vector< Processor > mProcessors;
		};

		/**
		 * Get CPU information.
		 * @return vector containing all processor CPU information, empty vector if unable to determine.
		 */
		std::vector< Processor > GetCPUInformation();

		/**
		 * Memory information.
		 */

		/**
		 * Get how much memory the system has.
		 * @return How much memory is in use in bytes, otherwise 0 if it is undetermined.
		 */
		size_t GetTotalMemeory();
		/**
		 * Get how much memory is in use.
		 * @return How much memory is in use in bytes, otherwise 0 if it is undetermined.
		 */
		size_t GetMemeoryInUse();
		
		/**
		 * Operating system information.
		 */
		struct OS
		{
			OS() :
				mNameAccuracy(Accuracies::UNKNOWN),
				mDistributionAccuracy(Accuracies::UNKNOWN),
				mKernelAccuracy(Accuracies::UNKNOWN),
				mVersionAccuracy(Accuracies::UNKNOWN)
			{
			}
			std::string mName;			//!< The name of the OS, e.g. Windows, Linux, OSX, but not limited to these. Not
										//	 all versions of an OS will necessarily report the same name. For example, some
										//	 systems might include the distribution name and release name or version in
										//	 this string. This string is probably more useful for display purposes rather
										//	 than using it to determine which OS you're running on. Use GetPlatform() for
										//	 an Echo specific platform check.
			Accuracy mNameAccuracy;		//!< The accuracy for mAccuracy.
			std::string mDistribution;	//!< For Linux platforms, the distribution name.
			Accuracy mDistributionAccuracy;//!< The accuracy for mDistribution.
			std::string mKernel;		//!< Kernel version information.
			Accuracy mKernelAccuracy;	//!< The accuracy for mKernel.
			std::string mVersion;		//!< Version.
			Accuracy mVersionAccuracy;	//!< The accuracy for mVersion.
		};
		
		/**
		 * Get OS information.
		 * @return structure containing OS information.
		 */
		OS GetOSInformation();

		/**
		 * Get platform name.
		 * The return value for this should always be accurate. The value will be hard coded by the platform developer.
		 * @note This is just a wrapper for Platform::GetName()
		 * @return The platform name.
		 */
		inline std::string GetPlatform()
		{
			return Platform::GetName();
		}
		
		inline std::ostream& operator <<(std::ostream& o, const Accuracy& accuracy)
		{
			switch(accuracy)
			{
				case Accuracies::UNKNOWN:	o << "UNKNOWN";		break;
				case Accuracies::GUESS:		o << "GUESS";		break;
				case Accuracies::HARD_CODED:o << "HARD_CODED";	break;
				case Accuracies::ACCURATE:	o << "ACCURATE";	break;
			}
			return o;
		}
		
		inline std::ostream& operator <<(std::ostream& o, const Processor& processor)
		{
			o << "Processor" << std::endl;
			o << "   ID: " << processor.mID  << " - " << processor.mIDAccuracy << std::endl;
			o << "   Vendor: " << processor.mVendor << " - " << processor.mVendorAccuracy << std::endl;
			o << "   Model: " << processor.mModelName << " - " << processor.mModelNameAccuracy << std::endl;
			o << "   Cache: " << processor.mCacheSize << " KiBi - " << processor.mCacheSizeAccuracy << std::endl;
			o << "   Frequency: " << processor.mFrequency << " KHz - " << processor.mFrequencyAccuracy << std::endl;
			o << "   FPU: " << processor.mHasFPU << " - " << processor.mHasFPUAccuracy << std::endl;
			o << "   Flags: " << processor.mFlags << " - " << processor.mFlagsAccuracy << std::endl;
			return o;
		}
		
		inline std::ostream& operator <<(std::ostream& o, const OS& os)
		{
			o << "OS" << std::endl;
			o << "   ID: " << os.mName  << " - " << os.mNameAccuracy << std::endl;
			o << "   Distribution: " << os.mDistribution << " - " << os.mDistributionAccuracy << std::endl;
			o << "   Kernel: " << os.mKernel << " - " << os.mKernelAccuracy << std::endl;
			o << "   Version: " << os.mVersion << " - " << os.mVersionAccuracy << std::endl;
			return o;
		}
	};
}

#endif
