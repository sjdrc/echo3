#ifndef _ECHOPROCESSORINFORMATIONHELPER_H_
#define _ECHOPROCESSORINFORMATIONHELPER_H_

#include <echo/Util/SystemInformation.h>
#include <vector>

namespace Echo
{
	namespace SystemInformation
	{
		/**
		 * ProcessorInformationHelper makes sure a vector contains enough values for the specified index.
		 * The vector will be expanded for indices larger than the capacity using defaults.
		 */
		class ProcessorInformationHelper
		{
		public:
			ProcessorInformationHelper(){}
			~ProcessorInformationHelper(){}
			void SetID(size_t index, size_t id, Accuracy accuracy = Accuracies::UNKNOWN)
			{
				CheckFit(index);
				mProcessors[index].mID = id;
				mProcessors[index].mIDAccuracy = accuracy;
			}
			void SetCacheSize(size_t index, size_t cacheSize, Accuracy accuracy = Accuracies::UNKNOWN)
			{
				CheckFit(index);
				mProcessors[index].mCacheSize = cacheSize;
				mProcessors[index].mCacheSizeAccuracy = accuracy;
			}
			void SetFrequency(size_t index, size_t frequency, Accuracy accuracy = Accuracies::UNKNOWN)
			{
				CheckFit(index);
				mProcessors[index].mFrequency = frequency;
				mProcessors[index].mFrequencyAccuracy = accuracy;
			}
			void SetVendor(size_t index, const std::string& vendor, Accuracy accuracy = Accuracies::UNKNOWN)
			{
				CheckFit(index);
				mProcessors[index].mVendor = vendor;
				mProcessors[index].mVendorAccuracy = accuracy;
			}
			void SetModelName(size_t index, const std::string& modelName, Accuracy accuracy = Accuracies::UNKNOWN)
			{
				CheckFit(index);
				mProcessors[index].mModelName = modelName;
				mProcessors[index].mModelNameAccuracy = accuracy;
			}
			void SetHasFPU(size_t index, bool hasFPU, Accuracy accuracy = Accuracies::UNKNOWN)
			{
				CheckFit(index);
				mProcessors[index].mHasFPU = hasFPU;
				mProcessors[index].mHasFPUAccuracy = accuracy;
			}
			void SetFlags(size_t index, const std::string& flags, Accuracy accuracy = Accuracies::UNKNOWN)
			{
				CheckFit(index);
				mProcessors[index].mFlags = flags;
				mProcessors[index].mFlagsAccuracy = accuracy;
			}
			void CheckFit(size_t index)
			{
				while(index >= mProcessors.size())
				{
					AddProcessor();
				}
			}
			void AddProcessor()
			{
				Processor cpu;
				cpu.mID = Processor::UNKNOWN_ID;
				cpu.mIDAccuracy = Accuracies::UNKNOWN;
				cpu.mCacheSize = 0;
				cpu.mCacheSizeAccuracy = Accuracies::UNKNOWN;
				cpu.mFrequency = 0;
				cpu.mFrequencyAccuracy = Accuracies::UNKNOWN;
				cpu.mVendorAccuracy = Accuracies::UNKNOWN;
				cpu.mModelNameAccuracy = Accuracies::UNKNOWN;
				cpu.mHasFPUAccuracy = Accuracies::UNKNOWN;
				cpu.mFlagsAccuracy = Accuracies::UNKNOWN;
				mProcessors.push_back(cpu);
			}
			const std::vector<Processor>& GetProcessors() const {return mProcessors;}
		private:
			std::vector<Processor> mProcessors;
		};
	}
}

#endif
