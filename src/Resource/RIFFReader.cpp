#include <echo/Resource/RIFFReader.h>

namespace Echo
{
	RIFFReader::RIFFReader()
	{
		AddChunkTypeProcessor(0x46464952,this,&RIFFReader::RIFFCheck);		//RIFF type
		mCurrentChunkSize=0;
		mCurrentChunkAbsoluteOffset=0;
		mCurrentChunkID=0;
		mFormat=0;
		mRIFFOk=false;
	}

	RIFFReader::~RIFFReader()
	{
		Close();
		std::map< u32, RIFFChunkProcessorFunctor* >::iterator it=mChunkProcessors.begin();
		std::map< u32, RIFFChunkProcessorFunctor* >::iterator itEnd=mChunkProcessors.end();
		while(it!=itEnd)
		{
			delete it->second;
			++it;
		}
	}

	//This needs to be called first
	bool RIFFReader::RIFFCheck(RIFFReader* reader)
	{
		mRIFFOk=true;
		mFileSize=mCurrentChunkSize;
		//PIFF chunk is
		//4 bytes for format followed by sub chunk
		mFile.Read(&mFormat,1,4);	//Get the format
		//Forces sub chunk read
		//Move the cursor 4 bytes forward and set the chunk size to 0
		//that is effectively an advanced cursor move.
		mCurrentChunkSize=0;
		mCurrentChunkAbsoluteOffset=4;
		return true;
	}

	//Chunk only chunks are chunks that contain chunks. These chunks' sub chunks are
	//automatically traversed. Which means that if one of these chunk ids is found 
	//on a call to ProcessNext the sub chunk will automatically be processed.
	//Returns true on success. false indicates a processor has been defined for chunk
	bool RIFFReader::AddChunkType(u32 chunkID)
	{
		std::map< u32, RIFFChunkProcessorFunctor* >::iterator it=mChunkProcessors.find(chunkID);
		if(it!=mChunkProcessors.end())
			return false;
		mChunkProcessors[chunkID]=0;
		return true;
	}

	bool RIFFReader::SetFile(File file)
	{
		mFile = file;
		if(!mFile.IsOpen())
		{
			return false;
		}
		mFile.Seek(0);
		return ProcessNext();		//First process should be RIFF header
	}

	bool RIFFReader::ProcessAll()
	{
		if(!mFile.IsOpen())
		{
			return false;
		}

		mCurrentChunkID=0;
		//Start at beginning
		mCurrentChunkAbsoluteOffset=0;
		mCurrentChunkSize=0;
		while(!mFile.EndOfFile())
		{
			if(!ProcessNext())
				return false;
		}
		return true;
	}

	bool RIFFReader::ProcessNext()
	{
		if(mCurrentChunkSize&1)				//Chunks should be even in size
			++mCurrentChunkSize;
		if(mCurrentChunkAbsoluteOffset>0)	//If its not the start of the file we'll move the cursor
			mCurrentChunkAbsoluteOffset+=mCurrentChunkSize+8;//8 is the size of the chunk header

		mFile.Seek(mCurrentChunkAbsoluteOffset);	//The state of the file cursor is
													//unknown after processing so ensure
													//the next chunk start
		if(mFile.Read(&mCurrentChunkID,1,4)!=4)		//Get chunk id
			return false;
		if(mFile.Read(&mCurrentChunkSize,1,4)!=4)	//Get chunk size
			return false;

		std::map< u32, RIFFChunkProcessorFunctor* >::iterator it=mChunkProcessors.find(mCurrentChunkID);
		if(it!=mChunkProcessors.end())
		{
			if(!(it->second))						//This chunk contains sub chunks
			{
				//Change the chunk size to reflect skip to sub chunk
				mCurrentChunkSize=0;			//Two u32's
				return ProcessNext();
			}else
			{
				//Call the processor
				if(!((*it->second)(this)))
				{
					//If the processor returned false we end processing
					Close();
					return false;
				}
				if(!mRIFFOk)
					return false;
			}
		}
		return true;
	}

	u32 RIFFReader::GetCurrentChunkSize()
	{
		return mCurrentChunkSize;
	}

	u32 RIFFReader::GetSaidFileSize()
	{
		return mFileSize;
	}

	u32 RIFFReader::GetActualFileSize()
	{
		return mFile.GetSize();
	}

	void RIFFReader::ReturnToChunkStart()
	{
		mFile.Seek(mCurrentChunkAbsoluteOffset+8);	//The state of the file cursor is
	}

	u32 RIFFReader::BookmarkCurrentChunk(u32 bookMarkID)
	{
		if(bookMarkID==0)
			bookMarkID=mCurrentChunkID;
		mSavedChunkLocations[bookMarkID]=mCurrentChunkAbsoluteOffset;
		return bookMarkID;
	}

	bool RIFFReader::ReturnToBookmarkedChunk(u32 bookMarkID)
	{
		std::map< u32, u32 >::iterator it=mSavedChunkLocations.find(bookMarkID);
		if(it!=mSavedChunkLocations.end())
		{
			mFile.Seek(it->second);
			mCurrentChunkAbsoluteOffset=it->second;
			if(mFile.Read(&mCurrentChunkID,1,4)!=4)		//Get chunk id
				return false;
			if(mFile.Read(&mCurrentChunkSize,1,4)!=4)	//Get chunk size
				return false;
			return true;
		}
		return false;
	}


	u32 RIFFReader::Read(void* buffer, u32 typeSize, u32 numToGet)
	{
		u32 bytesRemaining=mFile.GetPosition()-(mCurrentChunkAbsoluteOffset+8);
		bytesRemaining=mCurrentChunkSize-bytesRemaining;
		u32 bytesTotal=typeSize*numToGet;
		if(bytesTotal>bytesRemaining)
		{
			if(typeSize==1)
				numToGet=bytesRemaining;
			else
			{
				u32 numLess=bytesRemaining/typeSize;
				numToGet-=numLess;
			}
		}
		return mFile.Read(buffer,typeSize,numToGet);
	}

	bool RIFFReader::IsReadAtEndOfChunk()
	{
		return (mFile.GetPosition()>=(mCurrentChunkAbsoluteOffset+8+mCurrentChunkSize));
	}

	void RIFFReader::Close()
	{
		mFile.Close();
	}
}
