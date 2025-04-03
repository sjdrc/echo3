#ifndef _ECHORIFFREADER_H_
#define _ECHORIFFREADER_H_
#include <echo/Types.h>
#include <echo/FileSystem/File.h>
#include <map>

namespace Echo
{
	//RIFFReader is a RIFF file reader utility.
	//It is designed to remove the tedious processing of chunks by traversing
	//a file and calling processing functions for chunk types.

	//////////////////////////////////////////////////////////////////////////
	class RIFFReader;

	//RIFF chunk processing functor
	class RIFFChunkProcessorFunctor
	{
	public:
		RIFFChunkProcessorFunctor(){}
		virtual ~RIFFChunkProcessorFunctor(){}
		virtual bool operator()(RIFFReader* reader)=0;
	};

	template<typename T>
	class RIFFChunkProcessor : public RIFFChunkProcessorFunctor
	{
	public:
		typedef bool (T::*ChunkProcessorFunc)(RIFFReader* reader);
	private:
		T* mInstance;
		ChunkProcessorFunc mProcessor;
	public:
		RIFFChunkProcessor(T* instance, typename RIFFChunkProcessor::ChunkProcessorFunc processorFunc) : mInstance(instance), mProcessor(processorFunc){}
		~RIFFChunkProcessor(){}
		bool operator()(RIFFReader* reader)
		{
			return (*mInstance.*mProcessor)(reader);
		}
	};

	//////////////////////////////////////////////////////////////////////////
	//RIFFReader
	class RIFFReader
	{
	public:
	private:
		File mFile;
		u32 mCurrentChunkID;
		u32 mCurrentChunkSize;
		u32 mCurrentChunkAbsoluteOffset;
		u32 mFormat;
		u32 mFileSize;
		bool mRIFFOk;
		std::map< u32, RIFFChunkProcessorFunctor* > mChunkProcessors;
		std::map< u32, u32 > mSavedChunkLocations;
	public:
		RIFFReader();
		~RIFFReader();

		bool RIFFCheck(RIFFReader* reader);

		//Returns true on success. False indicates processor for chunk already exists
		template< typename T>
		bool AddChunkTypeProcessor(u32 chunkID, T* object, typename RIFFChunkProcessor<T>::ChunkProcessorFunc processorFunc)
		{
			std::map< u32, RIFFChunkProcessorFunctor* >::iterator it=mChunkProcessors.find(chunkID);
			if(it!=mChunkProcessors.end())
				return false;

			mChunkProcessors[chunkID]=new RIFFChunkProcessor<T>(object,processorFunc);
			return true;
		}

		//Chunk only chunks are chunks that contain chunks. These chunks' sub chunks are
		//automatically traversed. Which means that if one of these chunk ids is found 
		//on a call to ProcessNext the sub chunk will automatically be processed.
		//Returns true on success. false indicates a processor has been defined for chunk
		bool AddChunkType(u32 chunkID);

		bool SetFile(File file);
		//ProcessAll processes all chunks non-stop
		bool ProcessAll();
		//ProcessNext gives you control over the chunk processing.
		bool ProcessNext();

		u32 GetCurrentChunkSize();
		u32 GetSaidFileSize();
		u32 GetActualFileSize();

		//BookmarkCurrentChunk allows you to book mark this chunk and return to it later
		//returns the bookmark is. If bookMarkID is 0 then the current chunk ID is used.
		//NOTE: book marks are overwritten, so if bookMarkID is 0 on a chunk with the
		//same id as a previously bookmarked one, the previous bookmark will be
		//overwritten.
		u32 BookmarkCurrentChunk(u32 bookMarkID=0);
		//ReturnToBookmarkedChunk returns false if the bookmark can not be found or
		//if there was a read error while reading the chunk size or id
		bool ReturnToBookmarkedChunk(u32 bookMarkID);

		void ReturnToChunkStart();
		u32 Read(void* buffer, u32 typeSize, u32 numToGet);
		bool IsReadAtEndOfChunk();

		void Close();
	};
}
#endif
