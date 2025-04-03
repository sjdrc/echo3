#ifndef _ECHO_TILELAYER_H_
#define _ECHO_TILELAYER_H_

#include <string>
#include <vector>
#include <echo/Types.h>
#include <echo/Tile/Tile.h>
#include <echo/Tile/TileLayerMesh.h>
#include <echo/Graphics/SceneEntity.h>
#include <echo/Maths/Vector2.h>

namespace Echo
{
	class TileLayer : public SceneEntity
	{
	public:
		virtual ~TileLayer();
		
		inline TileLayerColumn operator[](size_t index)
		{
			return TileLayerColumn(*mTileLayer,index);
		}
	
		inline void SetTileSize(Scalar width, Scalar height)
		{
			return mTileLayer->SetTileSize(width, height);
		}

		inline Vector2 GetTileSize() const
		{
			return mTileLayer->GetTileSize();
		}
		
		size_t GetWidthInTiles() const
		{
			return mTileLayer->GetWidthInTiles();
		}
		
		size_t GetHeightInTiles() const
		{
			return mTileLayer->GetHeightInTiles();
		}

		inline Tile* GetTileAt(size_t column, size_t row)
		{
			return mTileLayer->GetTileAt(column, row);
		}

		void Clear()
		{
			mTileLayer->Clear();
		}

		TileMap& GetTileMap()
		{
			return mTileLayer->GetTileMap();
		}

		/**
		 * Set the parallax value for the x and y axis.
		 * @param amount Percentage values of the parent that will be applied.
		 */
		void SetParallax(const Vector2& amount)
		{
			mParallax=amount;
		}
		
		/**
		 * Set whether or not to apply the parallax.
		 * The default is true.
		 */
		void SetApplyParallax(bool applyParallax){mApplyParallax=applyParallax;}

		/**
		 * Set whether or not to apply the parallax.
		 * The default is true.
		 */
		bool GetApplyParallax() const {return mApplyParallax;}

		/**
		 * Get the parallax value.
		 * @see SetParallax.
		 * @return the parallax value.
		 */
		const Vector2& GetParallax() const
		{
			return mParallax;
		}

		/**
		 * Set the view window for the tile mesh.
		 * @see TileLayerMesh::SetViewWindow().
		 * @param window World space AABB that is transformed into local space to use as a "window".
		 */
		void SetViewWindow(AxisAlignedBox window = AxisAlignedBox::BOX_NULL);
		void ClearViewWindow();
		void RebuildMesh();
	private:
		friend class TileMap;
		TileLayer(shared_ptr<TileLayerMesh> tileLayerMesh);
		shared_ptr<TileLayerMesh> mTileLayer;
		bool mApplyParallax;			/// Whether or not the TileMap should apply the parallax.
		Vector2 mParallax;				/// When used in a TileMap this value determines how much extra this layer
										/// should move relative to the parent. The value is a percentage of
										/// the parent's position. So if the TileMap is at 100 and the parallax
										/// x is 0.5 then the position of this layer will be 150.
										/// @note This value is used by TileMap and setting the position of a
										/// TileLayer directly will not make use of this.s
	};
}

#endif
