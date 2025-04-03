#ifndef _ECHO_TILEMAP_H_
#define _ECHO_TILEMAP_H_

#include <echo/Types.h>
#include <echo/Graphics/SceneEntity.h>
#include <vector>

namespace Echo
{
	class TileLayer;
	class Texture;
	class TileSet;
	
	class TileMap : public SceneEntity
	{
	public:
		typedef std::map< std::string, shared_ptr<TileLayer> > NamedLayers;
		typedef std::pair< std::string, shared_ptr<TileLayer> > NamedLayer;
		
		TileMap();
		~TileMap();
		
		/**
		 * Get a layer's width in tiles.
		 * @param layerName The name of the layer, if empty the first layer created will be used.
		 * @return The layer's width in tiles, or if the layer isn't found 0.
		 */
		size_t GetLayerWidthInTiles(std::string layerName = "") const;
	
		/**
		 * Get a layer's height in tiles.
		 * @param layerName The name of the layer, if empty the first layer created will be used.
		 * @return The layer's height in tiles, or if the layer isn't found 0.
		 */
		size_t GetLayerHeightInTiles(std::string layerName = "") const;
		
		/**
		 * Get a layer's tile size.
		 * @note This is different to the TileSet tile size. Since the tile size in a TileSet is the size
		 * of the tile in the image but this method returns the size of the tile in space.
		 * @param layerName The name of the layer, if empty the first layer created will be used.
		 * @return The layer's tile size, or if the layer isn't found Vector2::ZERO.
		 */
		Vector2 GetLayerTileSize(std::string layerName = "") const;
		
		/**
		 * Add a TileSet.
		 * @param tileSet the tile set to add.
		 * @return The assigned ID for the TileSet.
		 */
		size_t AddTileSet(const TileSet& tileSet);

		/**
		 * Get a TileSet by id.
		 * @return The TileSet associated with the provided ID or a null pointer if the ID is not found.
		 */
		shared_ptr<TileSet> GetTileSet(size_t tileSetID);
		
		/**
		 * Get a tile layer by name.
		 * @param name The name of the tile layer.
		 * @return the tile layer, or a null pointer if the tile layer doesn't exist.
		 */
		shared_ptr<TileLayer> GetTileLayer(const std::string& name) const;
		
		NamedLayers GetTileLayers(void);
		
		/**
		 * Create a TileLayer.
		 * @param name The name of the layer. This will be used to reference the layer in the future.
		 * @param widthTiles The number of tiles horizontally of this layer. Must be >0.
		 * @param heightTiles The number of tiles vertically of this layer. Must be >0.
		 * @param tileSetIndex The tileSet index, must be less than the number of tile sets that have been added.
		 * @param tileWidth The width of the tiles used for building the layer (this is the physical width and can
		 * be different to the tile width in the image), <=0 indicates to use the tile widthfrom the image.
		 * @param tileHeight The width of the tiles used for building the layer (this is the physical height and can be
		 * different to the tile height in the image), <=0 indicates to use the tile height from the image.
		 * @return The new layer or a null pointer if any of the parameters are bad.
		 */
		shared_ptr<TileLayer> CreateTileLayer(const std::string& name, size_t widthTiles, size_t heightTiles, size_t tileSetIndex, Scalar tileWidth=-1.f, Scalar tileHeight=-1.f);

		/**
		 * Get an object layer by name.
		 * @param name The name of the tile layer.
		 * @return the object layer, or a null pointer if the tile layer doesn't exist.
		 */
		shared_ptr<SceneEntity> GetObjectLayer(const std::string& name) const;
	
		/**
		 * Create an ObjectLayer.
		 * @param name The name of the layer. This will be used to reference the layer in the future.
		 * @return A SceneEntity representing the layer.
		 */
		shared_ptr<SceneEntity> CreateObjectLayer(const std::string& name);


		/**
		 * Overrides for Node::SetPosition.
		 * These apply the parallax for the TileLayers.
		 * @note If you want to reposition the TileMap to some initial position but don't want it to apply the parallax,
		 * create a Node or SceneEntity and add the TileMap to it. You can then reposition this parent node without the
		 * parallax being applied. The parallax will be applied when the TileMap is positioned.
		 * @note If you use Node::SetPosition directly (forced) then the parallax won't be applied.
		 * @note You can override the layer position after calling SetPosition or disable parallax on the layer with
		 * TileLayer::SetApplyParallax().
		 */
		virtual void SetPosition(const Vector3& val) override;
		virtual void SetPosition(f32 x, f32 y, f32 z) override;
		
		/**
		 * Set the view window for the tile layers.
		 * @see TileLayerMesh::SetViewWindow()
		 * @param window World space AABB that is transformed into local space to use as a "window".
		 */
		void SetViewWindow(AxisAlignedBox window);

		/**
		 * Clear the view window.
		 * Each layer's mesh will be generated.
		 */
		void ClearViewWindow()
		{
			SetViewWindow(AxisAlignedBox::BOX_NULL);
		}
		/**
		 * Update all TileLayer meshes.
		 */
		void UpdateLayerMeshes();
				
		void Accept(SceneRenderableVisitor& visitor) override;
		
		/**
		 * Get whether or not to update layers to only fill the camera view while rendering.
		 * Each time the TIleMap is rendered and this flag is true the TileMap will calculate a window to use to update
		 * the layer meshes.
		 * The default is true.
		 * @return whether or not to update layers for cameras.
		 */
		bool GetUpdateMeshForCameras() const {return mUpdateMeshForCameras;}
		
		/**
		 * Set whether or not to update layers to only fill the camera view while rendering.
		 * @param update true to update layers for cameras.
		 */
		void SetUpdateMeshForCameras(bool update){mUpdateMeshForCameras=update;}
	protected:
		NamedLayers mTileLayers;
		std::vector< shared_ptr<TileSet> > mTileSets;
		std::map< std::string, shared_ptr<SceneEntity> > mObjectLayers;
		std::string mFirstLayerName;
		bool mUpdateMeshForCameras;
		AxisAlignedBox mViewWindow;
	};
}
#endif 
