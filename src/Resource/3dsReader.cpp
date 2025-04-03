#include <echo/Resource/3dsReader.h>
#include <echo/FileSystem/File.h>
#include <echo/FileSystem/FileSystem.h>

namespace Echo
{
	////////////////////////////////////////////////////////////////////////////
	// Chunk3DS class
	////////////////////////////////////////////////////////////////////////////
	
	//Returns a pointer to the Next Chunk.
	//If the next chunk is not known it is found
	Chunk3DS* Chunk3DS::NextChunk()
	{
		if(!pNextChunk)			//If we don't already have the next chunk
								//If we do we can return the next chunk
		{
			if(GetNext()==0)	//If zero end?
				return(0);

			Chunk3DS* ptempChunk;
			u16 rid=0;
			u32 rnext=0;

			mFile.Seek(offset+GetNext());	//Move to next Chunk start
			ptempChunk=new Chunk3DS(mFile,lengthsSize);		//Create a new Chunk Object

			mFile.Read((char*)&rid,2,1);		//Read the ID
			mFile.Read((char*)&rnext,lengthsSize,1);	//Read the length till next

			ptempChunk->SetID(rid);			//Set the ID
			ptempChunk->SetNext(rnext);		//Set the length till Next
			ptempChunk->SetOffset(offset+GetNext());	//Set the offset of the chunk
			ptempChunk->SetLastChunk(this);

			pNextChunk=ptempChunk;
		}
		return (pNextChunk);
	};

	//Returns a pointer to the Next Sub Chunk.
	//If the next chunk is not known it is found
	//The difference between a sub chunk and a chunk
	//is a subchunk exists within a chunk's data.
	//The only difference between this Function
	//and NextChunk() is the seek to before the read.
	//
	//Basically NextChunk finds the Next chunk based
	//on the length until next data, and NextSubChunk
	//Gets the next chunk directly after the current
	//chunk - too bad if its data this function wont
	//check for a bogas value for the next chunk.
	Chunk3DS* Chunk3DS::NextSubChunk(u32 skipBytes)
	{
		if(!pNextChunk)			//If we don't already have the next chunk
								//If we do we can return the next chunk
		{
			if(GetNext()==0)	//If zero end?
				return(0);

			Chunk3DS* ptempChunk;
			u16 rid=0;
			u32 rnext=0;

			u32 to=offset+lengthsSize+2+skipBytes;
			u32 coff=mFile.Seek(to);	//Move to next Chunk start
			if(coff!=to)
			{
				ECHO_LOG_ERROR("Seek failed: " << coff << " instead of " << to);
			}
			ptempChunk=new Chunk3DS(mFile,lengthsSize);		//Create a new Chunk Object

			mFile.Read((char*)&rid,2,1);		//Read the ID
			mFile.Read((char*)&rnext,lengthsSize,1);	//Read the length till next

			ptempChunk->SetID(rid);			//Set the ID
			ptempChunk->SetNext(rnext);		//Set the length till Next
			ptempChunk->SetOffset(offset+lengthsSize+2+skipBytes);	//Set the offset of the chunk
												//Offset is current chunk offset + current chunk header
			ptempChunk->SetLastChunk(this);
			pNextChunk=ptempChunk;
		}

		return (pNextChunk);
	};

	Chunk3DS::Chunk3DS(File& afile, u16 ilengthsSize) : mFile(afile)
	{
		lengthsSize=ilengthsSize;
		pNextChunk=0;
		pLastChunk=0;
		offset=0;
	}

	void Chunk3DS::ProcessFirstChunk()
	{
		mFile.Seek(0);	//Seek to start of file
		mFile.Read((char*)&cid,2,1);
		mFile.Read((char*)&next,lengthsSize,1);
	}

	//Recursive - Scary
	Chunk3DS* Walk(u32 toid,u8 direction,bool removeWhileWalking,Chunk3DS* chunk)
	{
		Chunk3DS *previousChunk=0;//, *nextChunk=0;

		if(direction==CHUNK_WALK_BACK)
		{
			previousChunk=chunk->GetLastChunk();
			if(removeWhileWalking)
				delete chunk;		//chunk is now invalid - DO NOT TOUCH!
			if(previousChunk->GetID()==toid)
			{
				previousChunk->SetNextChunk(0);
				return(previousChunk);
			}else
				return(Walk(toid,CHUNK_WALK_BACK,removeWhileWalking,previousChunk));
		}else
		if(direction==CHUNK_WALK_FORWARD)//Can't walk forward just yet ;)
		{
			return(0);
		}else
			return(0);
	}
	
	////////////////////////////////////////////////////////////////////////////
	// End Chunk3DS class
	////////////////////////////////////////////////////////////////////////////
	
	bool Read3DSFile(File& inFile, std::vector<Vector3> &vertices, std::vector<u16> &indices, std::vector < std::vector<TextureUV> > &textureCoordinates)
	{
		//3DS Files are stored in Little endian modes
		inFile.SetEndianMode(File::EndianModes::LITTLE);
	#ifdef ECHO_BIG_ENDIAN
		inFile.SetEndianConversionEnabled(true);
	#endif

		//Create the first Chunk (will be the main file chunk)
		//and check for error
		Chunk3DS* currentChunk = new Chunk3DS(inFile);
		currentChunk->ProcessFirstChunk();
		Chunk3DS* currentChunkTemp = 0;

		//Search through chunks in this order
		/*
		by id												defined
			0x4d4d											MAIN3DS
				|->0x3d3d									EDIT3DS
					 |->0x4000								EDIT_OBJECT
						  |->0x4100							OBJ_TRIMESH
							   //Read Vertex data 4110		TRI_VERTEXL
							   //Read Face Data 4120		TRI_FACEL1
						  |<---|
					 |<---|
					 |->0x4000
					   //Until no more 0x4000
			//End Read

			This requires a walk backwards in the list
		to find the chunk with id 0x3d3d after the
		Vertex and Face read. As the walk
		progresses each chunk is deleted from memory.
		 */

		//Make sure the first chunk is the MAIN3DS chunk
		if(!(currentChunk->GetID() == MAIN3DS))
		{
			//If not then report an error
			ECHO_LOG_ERROR("Error Read3DSFile:File is not a valid 3ds file.");
			return false;
		}

		//Display Chunks
		while(currentChunk)
		{
			//I only want to see 3D Edit chunk, main chunk and Objects
			switch(currentChunk->GetID())
			{
			case MAIN3DS:
				ECHO_LOG_DEBUG("Main Chunk found");
				currentChunk = currentChunk->NextSubChunk(0); //Get the Next Chunk
				break;
			case EDIT3DS:
				ECHO_LOG_DEBUG("Edit3DS Chunk found");
				currentChunk = currentChunk->NextSubChunk(0); //Get the Next Chunk
				break;
			case EDIT_OBJECT:
			{
				//Read past the object name
				char tempChar;
				u32 nameLength;
				nameLength = 0;
				tempChar = 1;
				inFile.Seek(currentChunk->GetOffset() + 6);
				std::string name;
				while(tempChar != 0) //Prints the name and tracks how long it is
				{
					inFile.Read(&tempChar, 1, 1);
					name+=tempChar;
					nameLength++;
				}
				ECHO_LOG_DEBUG("Object Found: " << name);
				currentChunk = currentChunk->NextSubChunk(nameLength); //Get the Next Chunk
			}
				break;
			case OBJ_TRIMESH:
				ECHO_LOG_DEBUG("Object Mesh Found");
				currentChunk = currentChunk->NextSubChunk(0); //Get the Next Chunk
				break;
			case TRI_VERTEXL:
			{
				ECHO_LOG_DEBUG("Mesh Vertex List Found");
				//Read the file and extract the vertex info
				u16 numberVertices;

				inFile.Read((char*)&numberVertices, 2, 1);
				ECHO_LOG_DEBUG("Number of Vertices: " << numberVertices);
				vertices.reserve(numberVertices);
				for(int i = 0; i < numberVertices; i++)
				{
					f32 xf, yf, zf;
					inFile.Read((char*)&xf, 4, 1);
					inFile.Read((char*)&yf, 4, 1);
					inFile.Read((char*)&zf, 4, 1);
					//Add the vertex, intentionally flip z and y.
					vertices.push_back(Vector3(xf,zf,yf));
				}

				//Now find face data.
				currentChunk = currentChunk->NextChunk(); //Get the Next Chunk
			}
				break;
			case TRI_FACEL1:
			{
				ECHO_LOG_DEBUG("Mesh Face List Found");
				//Read the file and extract the vertex info
				u16 numberFaces;
				u16 v1, v2, v3, faceOptions;

				inFile.Read((char*)&numberFaces, 2, 1);
				ECHO_LOG_DEBUG("Number of Faces: " << numberFaces);
				indices.reserve(numberFaces*3);
				for(int i = 0; i < numberFaces; i++)
				{
					inFile.Read((char*)&v1, 2, 1);
					inFile.Read((char*)&v2, 2, 1);
					inFile.Read((char*)&v3, 2, 1);
					inFile.Read((char*)&faceOptions, 2, 1);

					indices.push_back(v1);
					indices.push_back(v2);
					indices.push_back(v3);
				}

				//Walk back through list until EDIT_OBJECT then find next Chunk
				//currentChunk=Walk(EDIT_OBJECT,CHUNK_WALK_BACK,true,currentChunk);
				//currentChunk is now the last EDIT_OBJECT chunk
				currentChunk = currentChunk->NextChunk(); //Get the Next Chunk
			}
				break;
			case TRI_MAPPING:
			{
				ECHO_LOG_DEBUG("Mesh Vertex Mapping List Found");
				//Read the file and extract the vertex info
				u16 numberVerts;
				//u32 s,t;
				float sf, tf;
				TextureUV currentMappingCo(0,0);

				inFile.Read((char*)&numberVerts, 2, 1);
				ECHO_LOG_DEBUG("Number of Vertices: " << numberVerts);
				textureCoordinates.push_back(std::vector<TextureUV>());
				textureCoordinates.back().reserve(numberVerts);
				for(int i = 0; i < numberVerts; i++)
				{
					inFile.Read((char*)&sf, 4, 1);
					inFile.Read((char*)&tf, 4, 1);
					currentMappingCo.u = sf;
					currentMappingCo.v = tf;
					textureCoordinates.back().push_back(currentMappingCo);
				}

				//Walk back through list until EDIT_OBJECT then find next Chunk
				//currentChunk=Walk(EDIT_OBJECT,CHUNK_WALK_BACK,true,currentChunk);
				//currentChunk is now the last EDIT_OBJECT chunk
				currentChunk = currentChunk->NextChunk(); //Get the Next Chunk
			}
				break;
				//mappingCoordinates
			default:
				currentChunkTemp = currentChunk; //This is for cleanup later - The escape
				//out of this loop occurs when the file
				//can't be read anymore and currentChunk
				//is NULL, so we need to preserve currentChunk
				//for when it's NextChunk is NULL.
				currentChunk = currentChunk->NextChunk(); //Get the Next Chunk
				break;
			}
		}
		//Finish up and clean up the chunks
		if(currentChunkTemp)
		{
			//Clean up all our chunks in memory       \del/
			currentChunk = Walk(MAIN3DS, CHUNK_WALK_BACK, true, currentChunkTemp);
			delete currentChunk;
		}
		return true;
	}
}
