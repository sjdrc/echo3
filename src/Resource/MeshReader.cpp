#include <echo/Platforms/Ogre/OgreMeshFileFormat.h>
#include <echo/Resource/3dsReader.h>
#include <echo/Graphics/PrimitiveTypes.h>
#include <echo/FileSystem/FileSystem.h>
#include <echo/Graphics/Mesh.h>
#include <echo/Resource/MaterialManager.h>
#include <echo/Graphics/SubMesh.h>
#include <echo/Resource/SkeletonManager.h>
#include <iostream>
#include <vector>

namespace Echo
{
	//From Ogre
	enum OperationType {
		/// A list of points, 1 vertex per point
		OT_POINT_LIST = 1,
		/// A list of lines, 2 vertices per line
		OT_LINE_LIST = 2,
		/// A strip of connected lines, 1 vertex per line plus 1 start vertex
		OT_LINE_STRIP = 3,
		/// A list of triangles, 3 vertices per triangle
		OT_TRIANGLE_LIST = 4,
		/// A strip of triangles, 3 vertices for the first triangle, and 1 per triangle after that 
		OT_TRIANGLE_STRIP = 5,
		/// A fan of triangles, 3 vertices for the first triangle, and 1 per triangle after that
		OT_TRIANGLE_FAN = 6
	};

	/// Vertex element semantics, used to identify the meaning of vertex buffer contents
	enum VertexElementSemantic {
		/// Position, 3 reals per vertex
		VES_POSITION = 1,
		/// Blending weights
		VES_BLEND_WEIGHTS = 2,
		/// Blending indices
		VES_BLEND_INDICES = 3,
		/// Normal, 3 reals per vertex
		VES_NORMAL = 4,
		/// Diffuse colours
		VES_DIFFUSE = 5,
		/// Specular colours
		VES_SPECULAR = 6,
		/// Texture coordinates
		VES_TEXTURE_COORDINATES = 7,
		/// Binormal (Y axis if normal is Z)
		VES_BINORMAL = 8,
		/// Tangent (X axis if normal is Z)
		VES_TANGENT = 9

	};

	/// Vertex element type, used to identify the base types of the vertex contents
	enum VertexElementType
	{
		VET_FLOAT1 = 0,
		VET_FLOAT2 = 1,
		VET_FLOAT3 = 2,
		VET_FLOAT4 = 3,
		/// alias to more specific colour type - use the current rendersystem's colour packing
		VET_COLOUR = 4,
		VET_SHORT1 = 5,
		VET_SHORT2 = 6,
		VET_SHORT3 = 7,
		VET_SHORT4 = 8,
		VET_UBYTE4 = 9,
		/// D3D style compact colour
		VET_COLOUR_ARGB = 10,
		/// GL style compact colour
		VET_COLOUR_ABGR = 11
	};

	struct VertexDeclaration
	{
		struct VertexElement
		{
			u16 mSource;
			u16 mType;
			u16 mSemantic;
			u16 mOffset;
			u16 mIndex;
		};
		std::vector < VertexElement > mVertexElements;
	};

	struct VertexData
	{
		VertexData() : mData(0){}
		~VertexData()
		{
			if(mData)
				delete [] mData;
		}
		u32 mVertexDataSize;
		u16 mVertexSize;
		u16 mBindIndex;
		u8* mData;
	};

	struct Geometry
	{
		VertexDeclaration mVertexDeclaration;
		std::vector< VertexData* > mVertexData;
		VertexData* GetBuffer(int index)
		{
			for(u32 i=0;i<mVertexData.size();++i)
				if(index==mVertexData[i]->mBindIndex)
					return mVertexData[i];
			return 0;
		}
		u32 mVertexCount;
	};

	struct SubMeshData
	{
		SubMeshData()
		{
			mOperationType=OT_TRIANGLE_LIST;
			mUseSharedVertices=false;
			mIndexCount=0;
			mUses32BitIndexes=false;
			m32BitIndexes=0;
			m16BitIndexes=0;
			mGeometry=0;
		}
		~SubMeshData()
		{
			if(m32BitIndexes)
				delete [] m32BitIndexes;
			if(m16BitIndexes)
				delete [] m16BitIndexes;
			if(mGeometry)
				delete mGeometry;
		}
		std::string mMaterialName;
		bool mUseSharedVertices;
		u32 mIndexCount;
		bool mUses32BitIndexes;
		u32 *m32BitIndexes;
		u16 *m16BitIndexes;
		std::map< u32, std::map< size_t, f32 > > mBoneAssignments;
		OperationType mOperationType;
		Geometry* mGeometry;
	};

	struct MeshData
	{
		MeshData() : mGeometry(0), mMin(Vector3::ZERO), mMax(Vector3::ZERO)
		{

		}
		~MeshData()
		{
			for(u32 m=0;m<mSubMeshes.size();++m)
				delete mSubMeshes[m];

			if(mGeometry)
				delete mGeometry;
		}
		std::vector< SubMeshData* > mSubMeshes;
		std::map< u32, std::map< u16, f32 > > mBoneAssignments;
		std::string mSkeletonFile;
		Geometry* mGeometry;
		u32 mIndexCount;
		bool mUses32BitIndexes;
		u32 *m32BitIndexes;
		u16 *m16BitIndexes;
		Vector3 mMin;
		Vector3 mMax;
	};

	void _ReadGeormetryVertexDeclaration(File& inFile, Geometry* geometry, Chunk3DS** inOutChunk);
	void _ReadGeormetryVertexElementDeclaration(File& inFile, Geometry* geometry, Chunk3DS** inOutChunk);
	void _ReadGeometryVertexBuffer(File& inFile, Geometry* geometry, Chunk3DS** inOutChunk);
	
	template <typename T>
	bool CopyVertexData(Geometry& geometry, VertexDeclaration::VertexElement& element, VertexBuffer& destVertexBuffer, Size attributeIndex)
	{
		VertexData* vertexData=geometry.GetBuffer(element.mSource);
		assert(vertexData && "Vertex data was not found for buffer");
		if(!vertexData)
		{
			return false;
		}
		VertexBuffer::Accessor<T> destData = destVertexBuffer.GetAccessor<T>(attributeIndex);
		Size i=0;
		for(Size v=element.mOffset; v < vertexData->mVertexDataSize; v+=vertexData->mVertexSize)
		{
			destData[i++]=*reinterpret_cast<T*>(&vertexData->mData[v]);
		}
		return true;
	}

	void ExtractMesh(Geometry* geometry, shared_ptr<SubMesh> subMesh, SubMeshData* subMeshData, MaterialManager& materialManager, std::string meshFileName)
	{	
		{
			if(subMeshData->mOperationType!=OT_TRIANGLE_LIST)
			{
				ECHO_LOG_ERROR("Echo does not support loading of non-triangle list .mesh meshes");
				return;
			}
			ElementBuffer::IndexType indexType = subMeshData->mUses32BitIndexes ? ElementBuffer::IndexTypes::UNSIGNED_32BIT : ElementBuffer::IndexTypes::UNSIGNED_16BIT;
			subMesh->SetElementBuffer(ElementBuffer::Types::STATIC,indexType,ElementBuffer::ElementTypes::TRIANGLE,subMeshData->mIndexCount/3);
			shared_ptr<ElementBuffer> elementBuffer=subMesh->GetElementBuffer();
			
			if(subMeshData->mUses32BitIndexes)
			{
				auto indexes = elementBuffer->GetIndexAccessor<u32>();
				for(u32 i=0; i< subMeshData->mIndexCount; ++i)
				{
					indexes[i] = subMeshData->m32BitIndexes[i];
				}
			}else
			{
				auto indexes = elementBuffer->GetIndexAccessor<u16>();
				for(u16 i=0; i< subMeshData->mIndexCount; ++i)
				{
					indexes[i] = subMeshData->m16BitIndexes[i];
				}
			}

			subMesh->SetMaterial(materialManager.GetResource(subMeshData->mMaterialName,meshFileName));
			
			shared_ptr<VertexBuffer> vertexBuffer = subMesh->GetVertexBuffer();

			Size textureCoordinates = 0;
			// Create the vertex attributes
			for(Size e=0;e < geometry->mVertexDeclaration.mVertexElements.size(); ++e)
			{
				VertexDeclaration::VertexElement& element=geometry->mVertexDeclaration.mVertexElements[e];
				std::stringstream tag;
				switch(element.mSemantic)
				{
					case VES_POSITION:				tag << "Position"; break;
					case VES_TEXTURE_COORDINATES:	tag << "UV" << textureCoordinates++; break;
					case VES_NORMAL:				tag << "Normal"; break;
					case VES_BINORMAL:				tag << "Binormal"; break;
					case VES_TANGENT:				tag << "Tangent"; break;
					case VES_SPECULAR:				tag << "Specular"; break;
					case VES_DIFFUSE:				tag << "Colour"; break;
				}
				switch(element.mSemantic)
				{
					case VES_POSITION:
					case VES_TEXTURE_COORDINATES:
					case VES_NORMAL:
					case VES_BINORMAL:
					case VES_TANGENT:
					{
						switch(element.mType)
						{
							case VET_FLOAT2:
								vertexBuffer->AddVertexAttribute(tag.str(), VertexAttribute(VertexAttribute::ComponentTypes::VECTOR2,1));
								break;
							case VET_FLOAT3:
								vertexBuffer->AddVertexAttribute(tag.str(), VertexAttribute(VertexAttribute::ComponentTypes::VECTOR3,1));
								break;
							case VET_FLOAT4:
								vertexBuffer->AddVertexAttribute(tag.str(), VertexAttribute(VertexAttribute::ComponentTypes::VECTOR4,1));
								break;
							default:
								ECHO_LOG_ERROR("Unsupported vertex element type");
							continue;
						}
					}
					break;
					case VES_SPECULAR:
					case VES_DIFFUSE:
					{
						if(element.mType!=VET_COLOUR_ABGR && element.mType!=VET_COLOUR_ARGB)
						{
							ECHO_LOG_ERROR("Colours: Unsupported vertex element type");
							break;
						}
						// We'll reorder when we copy
						vertexBuffer->AddVertexAttribute(tag.str(), VertexAttribute(VertexAttribute::ComponentTypes::COLOUR_8,1));
					}
					break;
					default:
						ECHO_LOG_WARNING("Unsupported element semantic (" << element.mSemantic << "). File a feature request for additional mesh features. Your mesh may look incorrect");
						break;
				}
			}			

			//Allocate buffers now we have our vertex buffer attributes set
			if(!vertexBuffer->Allocate(geometry->mVertexCount))
			{
				ECHO_LOG_ERROR("Failed to allocate for vertex buffer data");
				return;
			}
			
			
			for(Size e=0;e < geometry->mVertexDeclaration.mVertexElements.size(); ++e)
			{
				VertexDeclaration::VertexElement& element=geometry->mVertexDeclaration.mVertexElements[e];
				switch(element.mSemantic)
				{
					case VES_POSITION:
					case VES_TEXTURE_COORDINATES:
					case VES_NORMAL:
					case VES_BINORMAL:
					case VES_TANGENT:
					{
						switch(element.mType)
						{
							case VET_FLOAT2:
							{
								if(!CopyVertexData<Vector2>(*geometry, element, *vertexBuffer, e))
								{
									ECHO_LOG_ERROR("Failed to copy data");
									vertexBuffer->Deallocate();
									return;
								}
							}
							break;
							case VET_FLOAT3:
							{
								if(!CopyVertexData<Vector3>(*geometry, element, *vertexBuffer, e))
								{
									ECHO_LOG_ERROR("Failed to copy data");
									vertexBuffer->Deallocate();
									return;
								}
							}
							break;
							case VET_FLOAT4:
							{
								if(!CopyVertexData<Vector4>(*geometry, element, *vertexBuffer, e))
								{
									ECHO_LOG_ERROR("Failed to copy data");
									vertexBuffer->Deallocate();
									return;
								}
							}
							break;
							default:
								ECHO_LOG_ERROR("Unsupported vertex element type");
							continue;
						}
						
						//
					}
					break;
					case VES_DIFFUSE:
					case VES_SPECULAR:
					{
						VertexData* vertexData=geometry->GetBuffer(element.mSource);
						if(!vertexData)
						{
							ECHO_LOG_ERROR("No buffer found for vertex element.");
							break;
						}

						if(element.mType!=VET_COLOUR_ABGR && element.mType!=VET_COLOUR_ARGB)
						{
							ECHO_LOG_ERROR("Colours: Unsupported vertex element type");
							break;
						}

						VertexBuffer::Accessor<VertexColour> colours = vertexBuffer->GetAccessor<VertexColour>(e);

						Size i=0;
						for(Size v=element.mOffset; v < vertexData->mVertexDataSize; v+=vertexData->mVertexSize)
						{
							u8* colourComponents=&vertexData->mData[v];
							VertexColour vertexColour;
							if(element.mType==VET_COLOUR_ABGR)
							{
								vertexColour._RGBA.a=colourComponents[0];
								vertexColour._RGBA.b=colourComponents[1];
								vertexColour._RGBA.g=colourComponents[2];
								vertexColour._RGBA.r=colourComponents[3];
							}else
							//if(element.mType==VET_COLOUR_ARGB)
							{
								vertexColour._RGBA.a=colourComponents[0];
								vertexColour._RGBA.r=colourComponents[1];
								vertexColour._RGBA.g=colourComponents[2];
								vertexColour._RGBA.b=colourComponents[3];
							}
							colours[i++] = vertexColour;
						}
					}
					break;
					default:
						ECHO_LOG_WARNING("Unsupported element semantic. Your mesh may look incorrect");
						break;
				}
			}
		}
		subMesh->Finalise();
	}

	//////////////////////////////////////////////////////////////////////////
	//Geometry Reading
	//////////////////////////////////////////////////////////////////////////
	Geometry* _ReadGeormetry(File& inFile, Chunk3DS** inOutChunk)
	{
		Geometry* geometry=new Geometry();
		inFile.Read(&geometry->mVertexCount,4,1);
		*inOutChunk=(*inOutChunk)->NextSubChunk(4);
		bool chunkKnown=true;

		while(chunkKnown)
		{
			switch((*inOutChunk)->GetID())
			{
			case Ogre::M_GEOMETRY_VERTEX_DECLARATION:
				_ReadGeormetryVertexDeclaration(inFile,geometry,inOutChunk);
				break;
			case Ogre::M_GEOMETRY_VERTEX_BUFFER:
				_ReadGeometryVertexBuffer(inFile,geometry,inOutChunk);
				break;
			default:
				chunkKnown=false;
			}
		}
		return geometry;
	}

	void _ReadGeormetryVertexDeclaration(File& inFile, Geometry* geometry, Chunk3DS** inOutChunk)
	{
		*inOutChunk=(*inOutChunk)->NextSubChunk(0);
		while((*inOutChunk)->GetID()==Ogre::M_GEOMETRY_VERTEX_ELEMENT)
		{
			VertexDeclaration::VertexElement element;

			inFile.Read(&element.mSource,2,1);
			inFile.Read(&element.mType,2,1);
			inFile.Read(&element.mSemantic,2,1);
			inFile.Read(&element.mOffset,2,1);
			inFile.Read(&element.mIndex,2,1);
			geometry->mVertexDeclaration.mVertexElements.push_back(element);
			*inOutChunk=(*inOutChunk)->NextChunk();
		}
	}

	void _ReadGeometryVertexBuffer(File& inFile, Geometry* geometry, Chunk3DS** inOutChunk)
	{
		VertexData* vertexData=new VertexData();
		inFile.Read(&vertexData->mBindIndex,2,1);
		inFile.Read(&vertexData->mVertexSize,2,1);
		u32 totalDataSize=vertexData->mVertexSize * geometry->mVertexCount;
		vertexData->mData=new u8[totalDataSize];
		vertexData->mVertexDataSize=totalDataSize;
		*inOutChunk=(*inOutChunk)->NextSubChunk(4);
		if((*inOutChunk)->GetID()!=Ogre::M_GEOMETRY_VERTEX_BUFFER_DATA)
		{
			ECHO_LOG_ERROR("Error reading geometry Vertex Buffer: ID incorrect (" << (*inOutChunk)->GetID() << ")");
			delete vertexData;
			return;
		}
		geometry->mVertexData.push_back(vertexData);
		inFile.Read(vertexData->mData,1,totalDataSize);
		*inOutChunk=(*inOutChunk)->NextChunk();
	}
	//////////////////////////////////////////////////////////////////////////
	//SubMesh Reading
	//////////////////////////////////////////////////////////////////////////

	SubMeshData* ReadSubMesh(File& inFile, Chunk3DS** inOutChunk)
	{
		SubMeshData* subMesh=new SubMeshData();

		//Read past the object name
		char tempChar;
		u32 chunkSize=0;
		tempChar=1;
		while(tempChar!=0x0A && tempChar!=0x0)
		{
			inFile.Read(&tempChar,1,1);
			if(tempChar!=0x0A && tempChar!=0x0)
				subMesh->mMaterialName+=tempChar;
			chunkSize++;
		}
		ECHO_LOG_INFO("Submesh Found. Material:" << subMesh->mMaterialName);

		chunkSize+=inFile.Read(&subMesh->mUseSharedVertices,1,1);
		chunkSize+=inFile.Read(&subMesh->mIndexCount,4,1);
		chunkSize+=inFile.Read(&subMesh->mUses32BitIndexes,1,1);

		if(subMesh->mUses32BitIndexes)
		{
			subMesh->m32BitIndexes=new u32[subMesh->mIndexCount];
			chunkSize+=inFile.Read(subMesh->m32BitIndexes,4,subMesh->mIndexCount);
		}else
		{
			subMesh->m16BitIndexes=new u16[subMesh->mIndexCount];
			chunkSize+=inFile.Read(subMesh->m16BitIndexes,2,subMesh->mIndexCount);
		}	

		(*inOutChunk)=(*inOutChunk)->NextSubChunk(chunkSize);	//Get the Next Chunk

		bool chunkKnown=true;
		while(chunkKnown)
		{
			switch((*inOutChunk)->GetID())
			{
			case Ogre::M_SUBMESH_OPERATION:
				inFile.Read(&subMesh->mOperationType,2,1);
				(*inOutChunk)=(*inOutChunk)->NextChunk();	//Get the Next Chunk
				break;
			case Ogre::M_SUBMESH_BONE_ASSIGNMENT:
				{
					u32 vertex=0;
					u16 bone=0;
					f32 weight=0.0f;
					inFile.Read(&vertex,4,1);
					inFile.Read(&bone,2,1);
					inFile.Read(&weight,4,1);
					subMesh->mBoneAssignments[vertex][bone]=weight;
					(*inOutChunk)=(*inOutChunk)->NextChunk();	//Get the Next Chunk
				}
				break;
			case Ogre::M_GEOMETRY:
				if(subMesh->mGeometry)
					delete subMesh->mGeometry;
				subMesh->mGeometry=_ReadGeormetry(inFile,inOutChunk);
				break;
				default:
					chunkKnown=false;
			}
		}
		return subMesh;
	}

	shared_ptr< std::vector< BoneBinding* > > ExtractBoneWeights(Mesh& outMesh, Skeleton& skeleton, SubMesh& subMesh, u32 vertexCount, std::map< u32, std::map< size_t, f32 > >& boneAssignments)
	{
		std::vector< BoneBinding* >* boneWeights=new std::vector< BoneBinding* >();
		boneWeights->reserve(vertexCount);					
		for(u32 v=0; v < vertexCount; ++v)
		{
			boneWeights->push_back(0);
		}

		for(u32 v=0; v < vertexCount; ++v)
		{
			//Get the bone
			std::map< u32, std::map< size_t, f32 > >::iterator it=boneAssignments.find(v);
			if(it==boneAssignments.end())
			{
				ECHO_LOG_WARNING("Vertex " << v << " has no bone assignments");
				continue;
			}
			bool allBonesFound=true;
			std::map< size_t, f32 >::iterator bit=it->second.begin();
			while(bit!=it->second.end())
			{
				shared_ptr<Bone> bone=skeleton.GetBone(bit->first);
				if(!bone)
				{
					ECHO_LOG_WARNING("Binding information has bone index out of range " << bit->first << " not found");
					allBonesFound=false;
					it->second.erase(bit);
					bit=it->second.begin();	//crude hack but it'll be ok. This case shouldn't happen really
				}else
				{
					++bit;
				}
			}
			if(!allBonesFound)
			{
				ECHO_LOG_WARNING("Not all bone binds were found");
			}

			BoneBinding* binding=outMesh.CreateBinding(it->second);
			(*boneWeights)[v]=binding;
		}
		return shared_ptr< std::vector< BoneBinding* > >(boneWeights);
	}

	//Lets get into it
	bool ReadMeshFile(File& inFile, MaterialManager& materialManager, SkeletonManager& skeletonManager, Mesh& targetMesh)
	{

		//--------------------
		//				START EXTRACTION CODE
		// This code is all the work done for you - unless you want something more
		// from the file. The guts of this program loops and finds the relating chunks
		// to vertices, faces and mapping data and extracts it into three lists (just below)
		//
		// You can scroll all the way down until you see a nice note that its the
		// end of the extraction code, where after you can use the lists and save your own
		// format.
		//--------------------
		//3DS Files are stored in Little endian modes
	#ifdef ECHO_BIG_ENDIAN
		inFile.SetEndianConversionEnabled(true);
	#endif

		//Create the first Chunk (will be the main file chunk)
		//and check for error
		Chunk3DS* currentChunk = new Chunk3DS(inFile);
		currentChunk->ProcessFirstChunk();
		Chunk3DS* currentChunkTemp=0;

		//Make sure the first chunk is the Ogre::M_HEADER chunk
		if(!(currentChunk->GetID()==Ogre::M_HEADER))
		{//If not then report an error
			ECHO_LOG_ERROR("File is not a valid .mesh file.");
			return false;
		}

		std::map< u16, std::string > subMeshNames;
		MeshData* mesh=new MeshData();

		//Display Chunks
		//u32 dontGoDeeperThan=0;
		//u32 currentSubMeshReadAmount=0;
		while(currentChunk)
		{
			//printf("\nPos: %d",g_fileSystem->getpos(inFile.fileHandle));

			//I only want to see 3D Edit chunk, main chunk and Objects
			switch(currentChunk->GetID())
			{
				case Ogre::M_HEADER:
				{
					//This is a special chunk. It does not have a length (beats me why)
					char tempChar=1;
					u32 chunkSize=0;
					std::string name;
					inFile.Seek(2);
					while(tempChar!=0x0A && tempChar!=0x0)
					{
						inFile.Read(&tempChar,1,1);
						if(tempChar!=0x0A && tempChar!=0x0)
							name+=tempChar;
						++chunkSize;
					}
					ECHO_LOG_DEBUG(".mesh version:" << name);
					currentChunk->SetNext(chunkSize+2);
					currentChunk=currentChunk->NextChunk();
				}break;
				case Ogre::M_MESH:
					ECHO_LOG_DEBUG("Mesh Chunk found");
					currentChunk=currentChunk->NextSubChunk(1);	//Get the Next Chunk
				break;
				case Ogre::M_SUBMESH:
					{
						SubMeshData* subMesh=ReadSubMesh(inFile,&currentChunk);
						mesh->mSubMeshes.push_back(subMesh);
					}
				break;
				case Ogre::M_MESH_SKELETON_LINK:
					{
						//This is a special chunk. It does not have a length (beats me why)
						char tempChar=1;
						std::string name;
						while(tempChar!=0x0A && tempChar!=0x0)
						{
							inFile.Read(&tempChar,1,1);
							if(tempChar!=0x0A && tempChar!=0x0)
								name+=tempChar;
						}
						ECHO_LOG_DEBUG("Link:" << name);
						mesh->mSkeletonFile=name;
						currentChunk=currentChunk->NextChunk();
					}
				break;
				case Ogre::M_SUBMESH_NAME_TABLE:
					currentChunk=currentChunk->NextSubChunk(0);
					while(currentChunk->GetID()==Ogre::M_SUBMESH_NAME_TABLE_ELEMENT)
					{
						u16 index=0;

						inFile.Read(&index,2,1);

						char tempChar;
						//u32 chunkSize=0;
						std::string name;
						tempChar=1;
						while(tempChar!=0x0A && tempChar!=0x0 && !inFile.EndOfFile())
						{
							inFile.Read(&tempChar,1,1);
							if(tempChar!=0x0A && tempChar!=0x0)
								name+=tempChar;
						}
						subMeshNames[index]=name;
						currentChunk=currentChunk->NextChunk();
					}
				break;
				case Ogre::M_MESH_BOUNDS:
					{
						inFile.Read(&mesh->mMin.x,4,1);
						inFile.Read(&mesh->mMin.y,4,1);
						inFile.Read(&mesh->mMin.z,4,1);
						inFile.Read(&mesh->mMax.x,4,1);
						inFile.Read(&mesh->mMax.y,4,1);
						inFile.Read(&mesh->mMax.z,4,1);
						f32 temp;
						inFile.Read(&temp,4,1);
						currentChunk=currentChunk->NextChunk();
					}
				break;
				case Ogre::M_GEOMETRY:
					if(!mesh->mGeometry)
						delete mesh->mGeometry;
					mesh->mGeometry=_ReadGeormetry(inFile,&currentChunk);				
				break;
				case Ogre::M_MESH_BONE_ASSIGNMENT:
					{
						// Optional bone weights (repeating section)
						// unsigned int vertexIndex;
						// unsigned short boneIndex;
						// float weight;
						u32 vertexIndex=0;
						u16 boneIndex=0;
						f32 weight;
						inFile.Read(&vertexIndex,4,1);
						inFile.Read(&boneIndex,2,1);
						inFile.Read(&weight,4,1);

						mesh->mBoneAssignments[vertexIndex][boneIndex]=weight;

						currentChunk=currentChunk->NextChunk();

					}
				break;
				//mappingCoordinates
				default:
					currentChunkTemp=currentChunk;					//This is for cleanup later - The escape
																	//out of this loop occurs when the file
																	//can't be read anymore and currentChunk
																	//is NULL, so we need to preserve currentChunk
																	//for when it's NextChunk is NULL.
					currentChunk=currentChunk->NextChunk();	//Get the Next Chunk
				break;
			}
		}
		ECHO_LOG_DEBUG("End mesh file");
	//--------------------
	//				END EXTRACTION CODE
	//--------------------

		//Finish up and clean up the chunks
		if(currentChunkTemp)
		{
			//Clean up all our chunks in memory       \del/
			currentChunk=Walk(Ogre::M_HEADER,CHUNK_WALK_BACK,true,currentChunkTemp);
			delete currentChunk;
		}
		inFile.Close();


		//Now we have our mesh data lets construct an Mesh

		bool extractedShared=false;
		shared_ptr< VertexBuffer > sharedVertices;
		shared_ptr< std::vector<u16> > sharedIndices;
		shared_ptr< std::vector< BoneBinding* > > sharedBoneWeights;

		shared_ptr< Skeleton > skeleton;
		if(!mesh->mSkeletonFile.empty())
		{
			skeleton=skeletonManager.GetResource(mesh->mSkeletonFile, inFile.GetActualFileName());
			targetMesh.SetSkeleton(skeleton);
		}

		for(u16 m=0;m<mesh->mSubMeshes.size();++m)
		{
			SubMeshData* subMeshData=mesh->mSubMeshes[m];
			shared_ptr<SubMesh> subMesh;

			if(subMeshNames.find(m)!=subMeshNames.end())
				subMesh=targetMesh.CreateSubMesh(subMeshNames[m]);
			else
				subMesh=targetMesh.CreateSubMesh();

			if(subMeshData->mUseSharedVertices)
			{
				if(!extractedShared)
				{
					ExtractMesh(mesh->mGeometry, subMesh, subMeshData,materialManager, inFile.GetActualFileName());
					if(skeleton)
					{
						sharedBoneWeights=ExtractBoneWeights(targetMesh, *skeleton, *subMesh, mesh->mGeometry->mVertexCount, subMeshData->mBoneAssignments);
						subMesh->SetBoneWeights(sharedBoneWeights);
					}
					sharedVertices = subMesh->GetVertexBuffer();
				}else
				{
					subMesh->SetVertexBuffer(sharedVertices);
					subMesh->SetBoneWeights(sharedBoneWeights);
				}
			}else
			{
				ExtractMesh(subMeshData->mGeometry, subMesh, subMeshData, materialManager, inFile.GetActualFileName());
				if(skeleton)
				{
					shared_ptr< std::vector< BoneBinding* > > boneWeights=ExtractBoneWeights(targetMesh, *skeleton, *subMesh, subMeshData->mGeometry->mVertexCount, subMeshData->mBoneAssignments);
					subMesh->SetBoneWeights(boneWeights);
				}
			}
			subMesh->Finalise();
		}
		//This forces an update of the AABB.
		targetMesh.GetAxisAlignedBox();

		delete mesh;
		ECHO_LOG_DEBUG("Complete ");
		return true;
	}
}
