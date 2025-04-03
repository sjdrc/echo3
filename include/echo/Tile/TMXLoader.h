#ifndef _ECHO_TMXMAPLOADER_H_
#define _ECHO_TMXMAPLOADER_H_

#include <echo/FileSystem/File.h>
#include <echo/Util/Configuration.h>

namespace Echo
{
	class TileMap;
	class MaterialManager;
	
	class TileMapObjectFactory
	{
	public:
		/**
		 * Parameters passed to object create methods.
		 */
		struct SceneEntityParameters
		{
			SceneEntityParameters() :
				mName(),
				mMaterial(),
				mTextureCoordinates(TextureUV(0,0),TextureUV(1,1)),
				mSize(0,0),
				mPosition(0,0,0)
			{}
			std::string mName;						/// The name of the object, if available.
			shared_ptr<Material> mMaterial;			/// The material for the entity, usually a default material is created
													/// for any image specified, unless there is a property to explicity
													/// define the material.
			TextureUVPair mTextureCoordinates;		/// The texture coordinates for the entity.
			Vector2 mSize;							/// The size of the object.  In a TMX file this is usually in pixels
													/// since the TileMaps are assumed to render at 1 unit=1 pixel. 
													/// The size on screen depends on how you set up a Camera and Viewport.
			Vector3 mPosition;						/// The position of the object relative to the layer it is on. In a
													/// TMX file this is usually in pixels since the TileMaps are assumed
													/// to render at 1 unit=1 pixel. As with size, on screen position
													/// depends on how you set up a Camera and Viewport. The position
													/// doesn't need specific unit information if all you are doing is
													/// just setting your entity position to this value.
			Configuration mConfiguration;			/// Contains extra data, this is a copy of the Tmx::PropertySet data
													/// in the form of a Configuration object which has more features.
		};
		virtual ~TileMapObjectFactory();
		
		/**
		 * Create an object for an object layer.
		 * The default implementation of this method will return a Sprite.
		 * @param parameters The parameters used for creation.
		 * @return a SceneEntity to be added to a layer, or null if you the object should be ignored.
		 */
		virtual shared_ptr<SceneEntity> CreateObject(const SceneEntityParameters& parameters);
	};
	
	/**
	 * Load a TMX tile map file.
	 * TMX files are produced by the Tiled map editor which can be found here http://www.mapeditor.org
	 * Tile layers can have the following properties:
	 *	parallax.x	-	The parallax value for the X axis. @see TileLayer::SetParallax
	 *	parallax.y	-	The parallax value for the Y axis. @see TileLayer::SetParallax
	 * @see http://doc.mapeditor.org/reference/tmx-map-format for information on the tmx format.
	 * @param file An open tmx file read to be read. The function will seek to the start before reading.
	 * @param materialManager A material manager that will contain the materials for the TileSet.
	 * @param tileMapObjectFactory a factory object to create the objects within the map.
	 * @return A valid TileMap with the loaded layers.
	 * previous layer. If there is an error it will be output to stdout and the function will return a
	 * null pointer.
	 */
	shared_ptr<TileMap> LoadTMXFile(File file, MaterialManager& materialManager,
	                                shared_ptr<TileMapObjectFactory> tileMapObjectFactory = shared_ptr<TileMapObjectFactory>());
}
#endif 

