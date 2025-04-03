#ifndef _ECHO_TILE_H_
#define _ECHO_TILE_H_

#include <echo/Types.h>
#include <echo/Graphics/Colour.h>

namespace Echo
{
	/**
	 * A single is a single unit in a tile layer.
	 * A tile contains an index to a tile in the tileset and other data which is used to determine how to render it.
	 */
	class Tile
	{
	public:
		u32 mIndex;						/// The index of the tile in the tile set.
		Colour mColour;					/// The colour of the tile.
		bool mFlipHorrizontally;		/// Whether or not the tile should be flipped horizontally.
		bool mFlipVertically;			/// whether or not the tile should be flipped vertically.
		bool mRotate90Degrees;			/// Whether or not the tile should be rotated 90 degrees, this is applied before
										/// any flipping. You can combine this with horizontal or vertical flipping to
										/// rotate for each 90 state:
										/// 
										/// 0 degrees: no flip, no rotation.
										/// 90 degrees: no flip, 90 degree rotation.
										/// 180 degrees: flip vertical, 90 degree rotation.
										/// 270 degrees: flip vertical, flip horizontal, 90 degree rotation.
										/// 
										/// There are a total of 8 states that the tile could be in.
		bool mBlank;					/// If set to true the tile isn't rendered.
		Tile() :
			mIndex(0),
			mFlipHorrizontally(false),
			mFlipVertically(false),
			mRotate90Degrees(false),
			mBlank(false)
		{
		}
		~Tile()
		{
		}
	};
}
#endif 

