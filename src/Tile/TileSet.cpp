#include <echo/Tile/TileSet.h>
#include <echo/Graphics/Material.h>

namespace Echo
{
	TileSet::TileSet(shared_ptr<Material> material, size_t tileWidthInImage, size_t tileHeightInImage,
					 size_t imageWidth, size_t imageHeight) :
		mMaterial(material),
		mTileWidth(tileWidthInImage),
		mTileHeight(tileHeightInImage),
		mImageWidth(imageWidth),
		mImageHeight(imageHeight)
	{
		assert(mMaterial && "material needs to be valid");
		assert(mMaterial->GetTexture() && "material needs at least one texture");
		assert(mTileWidth>0 && "tileWidthInImage must be >0");
		assert(mTileHeight>0 && "tileHeightInImage must be >0");
		assert(mImageWidth>0 && "imageWidth must be >0");
		assert(mImageHeight>0 && "imageHeight must be >0");
		if(mTileWidth>0 && mTileHeight>0 && mImageWidth>0 && mImageHeight>0 && mMaterial && mMaterial->GetTexture())
		{
			mNumberOfColumns = imageWidth / tileWidthInImage;
			mNumberOfRows = imageHeight / tileHeightInImage;
		}else
		{
			//This prevents NaN from generating in calculations later.
			mNumberOfColumns = 1;
			mNumberOfRows = 1;
			mTileWidth=1;
			mTileHeight=1;
			mImageWidth=1;
			mImageHeight=1;
		}
	}

	std::pair<TextureUV,TextureUV> TileSet::CalculateUVs(size_t index)
	{
		size_t column = index % mNumberOfColumns;
		size_t row = index / mNumberOfColumns;
		f32 imageWidth = static_cast<f32>(mImageWidth);
		f32 imageHeight = static_cast<f32>(mImageHeight);
		f32 u = static_cast<f32>(column*mTileWidth) / imageWidth;
		f32 v = static_cast<f32>(row*mTileHeight) / imageHeight;

		//Next coordinates are the next block diagonally minus one pixel.
		f32 s = static_cast<f32>(((column+1) * mTileWidth)-1) / imageWidth;
		f32 t = static_cast<f32>(((row+1) * mTileHeight)-1) / imageHeight;
		return std::make_pair(TextureUV(u,v),TextureUV(s,t));
	}
}
