#ifndef _ECHO_TILESET_H_
#define _ECHO_TILESET_H_

#include <echo/Graphics/PrimitiveTypes.h>

namespace Echo
{
	class Material;
	
	/**
	 * A tile set is an image that contains tiles on a grid.
	 * This class makes calculating texture cordinates for tiles easy.
	 * Although a texture is provided to the object, information about the image size which are used for texture
	 * coordinate instead. This allows the texture resolution to change and stil have the texture coordinates calculate
	 * correctly.
 	 */
	class TileSet
	{
	public:
		/**
		 * Constructor.
		 * @param material The material that has at least one texture unit with an image.
		 * @param tileWidthInImage The width in pixels of each tile in the image.
		 * @param tileHeightInImage The height in pixels of each tile in the image.
		 * @param imageWidth The width of the image for texture coordinate calculations.
		 * @param imageHeight The width of the image for texture coordinate calculations.
		 */
		TileSet(shared_ptr<Material> material, size_t tileWidthInImage, size_t tileHeightInImage, size_t imageWidth, size_t imageHeight);
		
		/**
		 * Calculate a pair of TextureUVs for the tile at the specified index.
		 * The first in the pair is the top left on the texture.
		 * The second in the pair is the bottom right on the texture.
		 * @param index The index within the tileset. If the index exceeds the number of tiles values are still
		 * calculated and will give v coordinates >1.0.
		 * @return A pair of texture coordinates for the tile.
		 */
		std::pair<TextureUV,TextureUV> CalculateUVs(size_t index);
		
		/**
		 * Get the material that we're using.
		 */
		shared_ptr<Material> GetMaterial() {return mMaterial;}
		size_t GetTileWidth() const {return mTileWidth;}
		size_t GetTileHeight() const {return mTileHeight;}
	private:
		shared_ptr<Material> mMaterial;
		size_t mTileWidth;				/// Tile width in pixels
		size_t mTileHeight;				/// Tile height in pixels
		size_t mImageWidth;				/// The image width that the tile set was originally specified for.
		size_t mImageHeight;			/// The image height that the tile set was originally specified for.
		size_t mNumberOfColumns;		/// Number of columns of tiles in the tile set.
		size_t mNumberOfRows;			/// Number of rows of tiles in the tile set.
	};
}
#endif 
