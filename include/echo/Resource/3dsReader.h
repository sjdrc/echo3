#ifndef _3DSREADER_H_
#define _3DSREADER_H_

#include <echo/Graphics/PrimitiveTypes.h>
#include <echo/Maths/Vector3.h>
#include <echo/Types.h>
#include <vector>
#include <iostream>

#define CHUNK_WALK_BACK		1
#define CHUNK_WALK_FORWARD	2

//>------ Primary chunk
#define MAIN3DS	   0x4D4D

//>------ Main Chunks
#define EDIT3DS			0x3D3D  // this is the start of the editor config
#define KEYF3DS			0xB000  // this is the start of the keyframer config

//>------ sub defines of EDIT3DS
#define EDIT_MATERIAL	0xAFFF
#define EDIT_CONFIG1	0x0100
#define EDIT_CONFIG2	0x3E3D
#define EDIT_VIEW_P1	0x7012
#define EDIT_VIEW_P2	0x7011
#define EDIT_VIEW_P3	0x7020
#define EDIT_VIEW1		0x7001
#define EDIT_BACKGR		0x1200
#define EDIT_AMBIENT	0x2100
#define EDIT_OBJECT		0x4000

#define EDIT_UNKNW01	0x1100
#define EDIT_UNKNW02	0x1201
#define EDIT_UNKNW03	0x1300
#define EDIT_UNKNW04	0x1400
#define EDIT_UNKNW05	0x1420
#define EDIT_UNKNW06	0x1450
#define EDIT_UNKNW07	0x1500
#define EDIT_UNKNW08	0x2200
#define EDIT_UNKNW09	0x2201
#define EDIT_UNKNW10	0x2210
#define EDIT_UNKNW11	0x2300
#define EDIT_UNKNW12	0x2302 
#define EDIT_UNKNW13	0x3000
#define EDIT_UNKNW14	0xAFFF

//>------ sub defines of EDIT_OBJECT
#define OBJ_TRIMESH		0x4100
#define OBJ_LIGHT		0x4600
#define OBJ_CAMERA		0x4700

#define OBJ_UNKNWN01	0x4010
#define OBJ_UNKNWN02	0x4012 //>>---- Could be shadow

//>------ sub defines of OBJ_CAMERA
#define CAM_UNKNWN01	0x4710 
#define CAM_UNKNWN02	0x4720 

//>------ sub defines of OBJ_LIGHT
#define LIT_OFF			0x4620
#define LIT_SPOT		0x4610
#define LIT_UNKNWN01	0x465A

//>------ sub defines of OBJ_TRIMESH
#define TRI_VERTEXL		0x4110
#define TRI_FACEL2		0x4111 
#define TRI_FACEL1		0x4120
#define TRI_SMOOTH		0x4150
#define TRI_MAPPING		0x4140
#define TRI_LOCAL		0x4160
#define TRI_VISIBLE		0x4165

//>>------ sub defs of KEYF3DS
#define KEYF_UNKNWN01	0xB009
#define KEYF_UNKNWN02	0xB00A
#define KEYF_FRAMES	0xB008
#define KEYF_OBJDES	0xB002

//>>------  these define the different colour chunk types
#define COL_RGB		0x0010
#define COL_TRU		0x0011
#define COL_UNK		0x0013

namespace Echo
{
	class File;
	
	class Chunk3DS
	{
	public:
		Chunk3DS(File& afile,u16 ilengthsSize=4);
		~Chunk3DS(){};
		//Get Chunks
		Chunk3DS* NextChunk();
		Chunk3DS* NextSubChunk(u32 skipBytes);
		//Get private member data
		u16 GetID(){ return (cid); };
		u64 GetNext(){ return (next); };
		u32 GetOffset(){ return (offset); };
		Chunk3DS* GetNextChunk(){ return(pNextChunk); };
		Chunk3DS* GetLastChunk(){ return(pLastChunk); };
		//Set private member data
		void SetID(u16 nid){ cid=nid; };
		void SetNext(u32 nnext){ next=nnext; };
		void SetOffset(u32 noffset){ offset=noffset; };
		void SetLastChunk(Chunk3DS* chunk){ pLastChunk=chunk; };
		void SetNextChunk(Chunk3DS* chunk){ pNextChunk=chunk; };
		
		void ProcessFirstChunk();

		const u16& GetLengthsSize() const { return lengthsSize; }
		void SetLengthsSize(const u16& val) { lengthsSize = val; }
	private:
		File& mFile;
		u16 lengthsSize;
		u16 cid;
		u64 next;				//How far till next chunk? - not subchunks
		u32 offset;				//Offset from start of file this chunk exists
		Chunk3DS* pNextChunk;
		Chunk3DS* pLastChunk;
	};

	//Tool functions
	Chunk3DS* Walk(u32 toid,u8 direction,bool removeWhileWalking,Chunk3DS* chunk);

	bool Read3DSFile(File& file, std::vector<Vector3> &vertices,std::vector<u16> &indices,std::vector< std::vector<TextureUV> > &textureCoordinates);
}
#endif
