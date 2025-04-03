#include <echo/Tile/TileLayerMesh.h>
#include <echo/Tile/TileSet.h>
#include <echo/Graphics/SubMesh.h>
#include <echo/Kernel/ScopedLock.h>

namespace Echo
{
	TileLayerMesh::TileLayerMesh(TileMap& tileMap, size_t widthInTiles, size_t heightInTiles, shared_ptr<TileSet> tileSet, Scalar tileWidth, Scalar tileHeight) :
		mTileMap(tileMap),
		mWidthInTiles(widthInTiles),
		mHeightInTiles(heightInTiles),
		mTileWidth(tileWidth),
		mTileHeight(tileHeight),
		mTileSet(tileSet),
		mMeshOutOfDate(true),
		mViewWindow(AxisAlignedBox::BOX_NULL)
	{
		shared_ptr<SubMesh> subMesh = CreateCommonSubMesh();
		mVertexBuffer = subMesh->GetVertexBuffer();
		//4 vertices per tile
		mVertexBuffer->Allocate(widthInTiles*heightInTiles*4);
		
		mVertices = mVertexBuffer->GetAccessor<Vector3>("Position");
		mNormals = mVertexBuffer->GetAccessor<Vector3>("Normal");
		mColours = mVertexBuffer->GetAccessor<VertexColour>("Colour");
		mTextureCoordinates = mVertexBuffer->GetAccessor<TextureUV>("UV0");
		
		mTileLayerData.resize(widthInTiles*heightInTiles,Tile());
		SetMaterial(tileSet->GetMaterial());
	}
	
	TileLayerMesh::~TileLayerMesh()
	{
		
	}

	const AxisAlignedBox& TileLayerMesh::GetAxisAlignedBox() const
	{
		UpdateMesh();
		return Mesh::GetAxisAlignedBox();
	}
	
	void TileLayerMesh::Clear()
	{
		for(Size i=0; i<mTileLayerData.size(); ++i)
		{
			mTileLayerData[i]=Tile();
		}
		mMeshOutOfDate=true;
	}

	void TileLayerMesh::Render(RenderContext& renderContext, const Matrix4& world, const Matrix4& worldView, Colour compoundDiffuse)
	{
		UpdateMesh();
		Mesh::Render(renderContext,world,worldView,compoundDiffuse);
	}
	
	void TileLayerMesh::SetViewWindow(AxisAlignedBox window)
	{
		if(mViewWindow!=window)
		{
			mViewWindow = window;
			mMeshOutOfDate = true;
			UpdateMesh();
		}
	}
	
	void TileLayerMesh::RebuildMesh()
	{
		mMeshOutOfDate = true;
		UpdateMesh();
	}
	
	void TileLayerMesh::UpdateMesh() const
	{
		ScopedLock lock(mMeshOutOfDateMutex);
		if(!mMeshOutOfDate)
		{
			return;
		}
		shared_ptr<SubMesh> tileMesh = GetSubMesh(0);
		
		Size numberOfTiles;

		Size currentVertexBase=0;
		
		s32 txStart;
		s32 tyStart;
		s32 txMax;
		s32 tyMax;
		if(mViewWindow.IsFinite())
		{
			txStart = (mViewWindow.GetMinimum().x / mTileWidth)-1;
			tyStart = (mViewWindow.GetMinimum().y / mTileHeight)-1;
			txStart = std::max(txStart,0);
			tyStart = std::max(tyStart,0);
			txMax = (mViewWindow.GetMaximum().x / mTileWidth)+1;
			tyMax = (mViewWindow.GetMaximum().y / mTileHeight)+1;
			txMax = std::min(txMax,static_cast<s32>(mWidthInTiles));
			tyMax = std::min(tyMax,static_cast<s32>(mHeightInTiles));
			
			if(txMax < txStart || tyMax < tyStart)
			{
				mVertexBuffer->SetNumberOfElements(0);
				return;
			}
			numberOfTiles = (txMax-txStart)*(tyMax-tyStart);
		}else
		{
			txStart=0;
			tyStart=0;
			txMax = mWidthInTiles;
			tyMax = mHeightInTiles;
			numberOfTiles = mTileLayerData.size();
		}

		mVertexBuffer->SetNumberOfElements(numberOfTiles);
		if(numberOfTiles==0)
		{
			return;
		}

		const Size TRIANGLES_PER_TILE=2;
		shared_ptr<ElementBuffer> elementBuffer = tileMesh->GetElementBuffer();
		if(!elementBuffer)
		{
			if(!tileMesh->SetElementBuffer(ElementBuffer::Types::DYNAMIC, ElementBuffer::IndexTypes::UNSIGNED_32BIT, ElementBuffer::ElementTypes::TRIANGLE, numberOfTiles*TRIANGLES_PER_TILE))
			{
				ECHO_LOG_ERROR("Unable to create element buffer for TileMesh");
				return;
			}
			elementBuffer=tileMesh->GetElementBuffer();
		}else
		{
			elementBuffer->Allocate(numberOfTiles*TRIANGLES_PER_TILE);
		}
		
		auto triangles = elementBuffer->GetAccessor<ElementBuffer::Triangle<u32> >();
		Size triangleIndex=0;
		
		for(s32 ty=tyStart; ty<tyMax; ++ty)
		{
			for(s32 tx=txStart; tx<txMax; ++tx)
			{
				size_t index = tx + ty*mWidthInTiles;
				const Tile& tile=mTileLayerData[index];
				if(tile.mBlank)
				{
					continue;
				}
				Size tileSetIndex = tile.mIndex;

				std::pair<TextureUV,TextureUV> tileTextureCoordinates = mTileSet->CalculateUVs(tileSetIndex);

				Vector3 v1 = Vector3(tx*mTileWidth,(ty+1)*mTileHeight,0);
				Vector3 v2 = Vector3((tx+1)*mTileWidth,(ty+1)*mTileHeight,0);
				Vector3 v3 = Vector3(tx*mTileWidth,ty*mTileHeight,0);
				Vector3 v4 = Vector3((tx+1)*mTileWidth,ty*mTileHeight,0);

				//Check rotate setting
				if(tile.mRotate90Degrees)
				{
					Vector3 t=v1;
					v1=v3;
					v3=v4;
					v4=v2;
					v2=t;
				}
				
				//Check flip settings
				if(tile.mFlipHorrizontally)
				{
					std::swap(tileTextureCoordinates.first.u,tileTextureCoordinates.second.u);
				}
				
				if(tile.mFlipVertically)
				{
					std::swap(tileTextureCoordinates.first.v,tileTextureCoordinates.second.v);
				}
				
				mNormals[currentVertexBase] = mVertices[currentVertexBase] = v1;
				mNormals[currentVertexBase+1] = mVertices[currentVertexBase+1] = v2;
				mNormals[currentVertexBase+2] = mVertices[currentVertexBase+2] = v3;
				mNormals[currentVertexBase+3] = mVertices[currentVertexBase+3] = v4;

				mColours[currentVertexBase] = tile.mColour;
				mColours[currentVertexBase+1] = tile.mColour;
				mColours[currentVertexBase+2] = tile.mColour;
				mColours[currentVertexBase+3] = tile.mColour;

				mTextureCoordinates[currentVertexBase] = tileTextureCoordinates.first;
				mTextureCoordinates[currentVertexBase+1] = TextureUV(tileTextureCoordinates.second.u,tileTextureCoordinates.first.v);
				mTextureCoordinates[currentVertexBase+2] = TextureUV(tileTextureCoordinates.first.u,tileTextureCoordinates.second.v);
				mTextureCoordinates[currentVertexBase+3] = tileTextureCoordinates.second;

				{
					auto& triangle = triangles[triangleIndex];
					triangle.mA = currentVertexBase;
					triangle.mB = currentVertexBase+2;
					triangle.mC = currentVertexBase+3;
					triangleIndex++;
				}
				{
					auto& triangle = triangles[triangleIndex];
					triangle.mA = currentVertexBase;
					triangle.mB = currentVertexBase+3;
					triangle.mC = currentVertexBase+1;
					triangleIndex++;
				}
				currentVertexBase+=4;
			}
		}
		elementBuffer->SetNumberOfElements(triangleIndex);
		mVertexBuffer->SetNumberOfElements(numberOfTiles*4);
		
		tileMesh->Finalise();
		mMeshOutOfDate = false;
	}
}
