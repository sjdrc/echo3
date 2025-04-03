#ifndef _ECHO_TILELAYERMESH_H_
#define _ECHO_TILELAYERMESH_H_

#include <string>
#include <vector>
#include <echo/Types.h>
#include <echo/Tile/Tile.h>
#include <echo/Maths/Vector2.h>
#include <echo/Graphics/Mesh.h>
#include <echo/Graphics/PrimitiveTypes.h>

namespace Echo
{
	class TileMap;
	class TileSet;
	class TileLayer;
	class TileLayerMesh;
	
	class TileLayerColumn
	{
	public:
		~TileLayerColumn(){}
		inline Tile& operator[](size_t row);

		inline size_t GetNumberOfRows() const;
	private:
		friend class TileLayerMesh;
		friend class TileLayer;
		TileLayerColumn(TileLayerMesh& layer, size_t index) : mLayer(layer), mIndex(index){}
		TileLayerMesh& mLayer;
		size_t mIndex;
	};

	class TileLayerMesh : public Mesh
	{
	public:
		virtual ~TileLayerMesh();

		inline TileLayerColumn operator[](size_t index)
		{
			assert(index<GetWidthInTiles() && "index must be less than the width of the layer.");
			return TileLayerColumn(*this,index);
		}

		/**
		 * Set the visual tile size.
		 * @param width The width.
		 * @param height The height.
		 */
		inline void SetTileSize(Scalar width, Scalar height)
		{
			mTileWidth = width;
			mTileHeight = height;
			mMeshOutOfDate = true;
		}

		/**
		 * Get the tile size.
		 * @return the tile size.
		 */
		inline Vector2 GetTileSize() const
		{
			return Vector2(mTileWidth, mTileHeight);
		}

		size_t GetWidthInTiles() const {return mWidthInTiles;}
		size_t GetHeightInTiles() const {return mHeightInTiles;}
		
		/**
		 * Get the tile at the specified position.
		 * @param column The column index (base 0) that you wish to look up.
		 * @param row The row index (base 0) that you wish to look up.
		 * @return The tile at the column and row, or a nullptr if any one of the specified parameters are out of range.
		 */
		inline Tile* GetTileAt(size_t column, size_t row)
		{
			assert(column<mWidthInTiles && row<mHeightInTiles && "row or column out of range.");
			if(column<mWidthInTiles && row<mHeightInTiles)
			{
				return &mTileLayerData[column+row*mWidthInTiles];
			}
			return 0;
		}

		const AxisAlignedBox& GetAxisAlignedBox() const override;

		/**
		 * Clear all tiles.
		 */
		void Clear();

		virtual void Render(RenderContext& renderContext, const Matrix4& world, const Matrix4& worldView, Colour compoundDiffuse) override;
		
		TileMap& GetTileMap() {return mTileMap;}

		/**
		 * Set the a "window" in which the overlapping parts of the mesh will be visible.
		 * The view window restricts the tile's Mesh to only generate to be within specified AABB.
		 * The window defines the tile range and will always be filled which means the window may be
		 * larger by up to a tile width on the X axis and height on the Y axis. The window is always
		 * filled except on tile layer edges where there are no more tiles.
		 * The box provided will in the Mesh space. Use TileLayer::SetViewWindow() if you want to
		 * specify a world space box which is converted to the local space for you.
		 * @note This method regenerates the mesh.
		 * @param window Local space AABB that is transformed into local space to use as a "window".
		 */		
		void SetViewWindow(AxisAlignedBox window);

		/**
		 * Clear the view window.
		 */
		void ClearViewWindow()
		{
			SetViewWindow(AxisAlignedBox::BOX_NULL);
		}

		/**
		 * Force a mesh rebuild.
		 */
		void RebuildMesh();
	private:
		TileMap& mTileMap;
		Size mWidthInTiles;
		Size mHeightInTiles;
		Scalar mTileWidth;
		Scalar mTileHeight;
		shared_ptr<TileSet> mTileSet;
		mutable Mutex mMeshOutOfDateMutex;
		mutable bool mMeshOutOfDate;
		std::vector< Tile > mTileLayerData;
		AxisAlignedBox mViewWindow;
		shared_ptr<VertexBuffer> mVertexBuffer;
		mutable VertexBuffer::Accessor<Vector3> mVertices;
		mutable VertexBuffer::Accessor<Vector3> mNormals;
		mutable VertexBuffer::Accessor<TextureUV> mTextureCoordinates;
		mutable VertexBuffer::Accessor<VertexColour> mColours;

		friend class TileMap;
		TileLayerMesh(TileMap& tileMap, Size widthInTiles, Size heightInTiles, shared_ptr<TileSet> tileSet, Scalar tileWidth, Scalar tileHeight);

	protected:
		/**
		 * Update the mesh.
		 * This method is const because it 
		 */
		void UpdateMesh() const;
	};
		
	inline Tile& TileLayerColumn::operator[](size_t row)
	{
		Tile* tile = mLayer.GetTileAt(mIndex,row);
		assert(tile && "Invalid row specified.");
		return *tile;
	}

	inline size_t TileLayerColumn::GetNumberOfRows() const
	{
		return mLayer.GetHeightInTiles();
	}
}

#endif
