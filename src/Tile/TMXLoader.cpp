#include <echo/Tile/TileMap.h>
#include <echo/Tile/TileLayer.h>
#include <echo/Tile/TileSet.h>
#include <echo/Resource/MaterialManager.h>
#include <echo/Graphics/Mesh.h>
#include <echo/Graphics/Sprite.h>
#include <echo/FileSystem/File.h>
#include <echo/Util/StringUtils.h>
#include <echo/Tile/TMXLoader.h>
#include <tmxparser/Tmx.h>

namespace Echo
{
	TileMapObjectFactory::~TileMapObjectFactory()
	{
	}

	shared_ptr<SceneEntity> TileMapObjectFactory::CreateObject(const SceneEntityParameters& parameters)
	{
		shared_ptr<Sprite> sprite(new Sprite());
		sprite->SetName(parameters.mName);
		sprite->SetMaterial(parameters.mMaterial);
		sprite->SetTextureCoordinates(parameters.mTextureCoordinates.first,parameters.mTextureCoordinates.second);
		sprite->SetPixelToWorldUnitRatio(1.f);
		sprite->SetSizeInPixels(parameters.mSize.x,parameters.mSize.y);
		sprite->SetPosition(parameters.mPosition);
		return sprite;
	}
	
	
	shared_ptr<TileMap> LoadTMXFile(File file, MaterialManager& materialManager,
	                                shared_ptr<TileMapObjectFactory> tileMapObjectFactory)
	{
		Tmx::Map map;
		if(!file.IsOpen())
		{
			ECHO_LOG_ERROR("The file is not open.");
			return shared_ptr<TileMap>();
		}
		file.Seek(0);
		std::string fileContents;
		file.ReadFileIntoString(fileContents);
		map.ParseText(fileContents);

		switch(map.GetErrorCode())
		{
			case Tmx::TMX_COULDNT_OPEN:
				ECHO_LOG_ERROR("The file could not be opened.");
				return shared_ptr<TileMap>();

			case Tmx::TMX_PARSING_ERROR:
				ECHO_LOG_ERROR("There was an error in parsing the TMX file.");
				return shared_ptr<TileMap>();
			case Tmx::TMX_INVALID_FILE_SIZE:
				ECHO_LOG_ERROR("The size of the file is invalid.");
				return shared_ptr<TileMap>();
		}
		
		if(map.GetOrientation()!=Tmx::TMX_MO_ORTHOGONAL)
		{
			ECHO_LOG_ERROR("We only support orthogonal tile maps at the moment.");
			return shared_ptr<TileMap>();
		}
		
		//TODO: Check render order
		//Tmx::MapRenderOrder renderOrder = map.GetRenderOrder();

		int numberOfTilesX = map.GetWidth();
		int numberOfTilesY = map.GetHeight();
		if(numberOfTilesX==0 || numberOfTilesY==0)
		{
			ECHO_LOG_ERROR("Cannot create a Tilemap with 0 size.");
			return shared_ptr<TileMap>();
		}

		int tileWidth=map.GetTileWidth();
		int tileHeight=map.GetTileHeight();
		if(tileWidth==0 || tileHeight==0)
		{
			ECHO_LOG_ERROR("Cannot create a Tilemap with tile size of 0.");
			return shared_ptr<TileMap>();
		}
		
		// Since Echo uses +Y as up, which is different to the TMX format, we need to convert Y coordinates
		// for images and objects as an offset from the yMax rather than 0. i.e. yMax - objectY.
		int yMax = tileHeight*map.GetHeight();
		
		//Make sure there is a factory object.
		if(!tileMapObjectFactory)
		{
			tileMapObjectFactory.reset(new TileMapObjectFactory());
		}

		std::map< int, std::pair<size_t, const Tmx::Tileset* > > tileSetGIDStartLookup;
		std::map< const Tmx::Tileset*, size_t > tileSetIndexLookup;

		shared_ptr<TileMap> tileMap(new TileMap());
		const int NULL_TILESET=-1;
		
		//For each tile set, create a resource if required. Store a temporary lookup for the Globally Unique ID (GID) start.
        const std::vector< Tmx::Tileset* >& tilesets = map.GetTilesets();
		BOOST_FOREACH(const Tmx::Tileset* tileset, tilesets)
		{
			const Tmx::Image* image=tileset->GetImage();
			if(!image)
			{
				ECHO_LOG_ERROR("Tileset \"" << tileset->GetName() << "\" missing image.");
				continue;
			}
			std::string imagePathAndName = Utils::String::GetPathFromFilename(file.GetRequestedFileName()) + image->GetSource();
			// Get or create the material if it doesn't exist.
			shared_ptr<Material> material;
			if(!materialManager.ResourceExists(imagePathAndName))
			{
				material = materialManager.CreateMaterialUsingImage(imagePathAndName);
				// Apply some settings to the material that are relevant. These are deliberately not applied
				// if the material has been defined in the manager so users can override the material settings.
				//Turn on nearest filtering. This prevents some rendering artefacts.
				material->GetPass(0)->GetTextureUnit(0)->SetFilter(TextureUnit::TextureFilters::NEAREST);
			} else
			{
				material = materialManager.GetResource(imagePathAndName);
			}
			
			// Warn if a margin has been set. This will require calculating the texture UVs differently. This is
			// something that the TileSet needs to support first.
			if(tileset->GetMargin()!=0)
			{
				ECHO_LOG_WARNING("Tile margin is currently not supported.");
			}

			// Make sure the tile set exists. If layers use the same tile set this will actually create a duplicate profile.
			// We are still only using a single material and loaded resource for both tile sets though.
			size_t tileSetIndex = tileMap->AddTileSet(TileSet(material,tileset->GetTileWidth(),tileset->GetTileHeight(),image->GetWidth(),image->GetHeight()));

			tileSetGIDStartLookup[tileset->GetFirstGid()]=std::make_pair(tileSetIndex,tileset);
			tileSetIndexLookup[tileset]=tileSetIndex;
		}
		
		//Tile layers
		int layerCount=0;
		const std::vector< Tmx::TileLayer* >& tileLayers = map.GetTileLayers();
		BOOST_FOREACH(Tmx::TileLayer* layer, tileLayers)
		{
			// Check that this layer's tilesets are all the same. We require this because we build a single mesh using
			// a single material for the tileset. Multiple tile sets would mean building a mesh for each tile set.
			// This could be supported in the future by using a submesh per tileset, but doing that may break rendering.
			int tileSetID=-1;
			for(int y=0; y< layer->GetHeight(); ++y)
			for(int x=0; x< layer->GetWidth(); ++x)
			{
				if(tileSetID==NULL_TILESET)
				{
					tileSetID = layer->GetTileTilesetIndex(x,y);
					continue;
				}
				//Verify that all tiles are using the same Tilesets.
				int id = layer->GetTileTilesetIndex(x,y);
				if(id != tileSetID && id!=NULL_TILESET && tileSetID!=NULL_TILESET)
				{
					ECHO_LOG_ERROR("Currently we only support one tileSet per layer." << layer->GetTileTilesetIndex(x,y) << "!=" << tileSetID);
					return shared_ptr<TileMap>();
				}
			}
			
			const Tmx::Tileset* tileset=map.GetTileset(tileSetID);
			
			std::string layerName = layer->GetName();
			if(layerName.empty())
			{
				//Assigned a name.
				std::stringstream ss;
				ss << layerCount;
				layerName = ss.str();
			}
			
			//Find the tileset index
			std::map< const Tmx::Tileset*, size_t >::iterator it=tileSetIndexLookup.find(tileset);
			if(it==tileSetIndexLookup.end())
			{
				ECHO_LOG_ERROR("Tile layer." << layerName << " specified an invalid tileset " << tileSetID);
				return shared_ptr<TileMap>();
			}
			
			//Create the tile layer.
			shared_ptr<TileLayer> tileLayer = tileMap->CreateTileLayer(layerName, layer->GetWidth(),layer->GetHeight(),it->second,tileWidth,tileHeight);
			
			if(!tileLayer)
			{
				ECHO_LOG_ERROR("Could not create object layer \"" << layerName << "\". Make sure your layer names are unique.");
				continue;
			}
			
			//Find the parallax property and set it to our layer.
			f32 parallaxX = layer->GetProperties().GetFloatProperty("parallax.x");
			f32 parallaxY = layer->GetProperties().GetFloatProperty("parallax.y");
			tileLayer->SetParallax(Vector2(parallaxX,parallaxY));
			
			//Set the position to the layer count.
			tileLayer->SetPosition(Vector3(layer->GetX(),layer->GetY(),layer->GetZOrder()));

			// Populate the tile map =)
			int layerHeight = layer->GetHeight();
			for(int y=0; y< layerHeight; ++y)
			for(int x=0; x< layer->GetWidth(); ++x)
			{
				TileLayer& tl=*tileLayer;
				// Our tile map follows the coordinate system where by Y is up rather than down so we invert the y index.
				Tile& tile = tl[x][layerHeight-y-1];
				tile.mIndex = layer->GetTileId(x,y);
				tile.mFlipHorrizontally = layer->IsTileFlippedHorizontally(x,y);
				tile.mFlipVertically = layer->IsTileFlippedVertically(x,y);
				tile.mRotate90Degrees = layer->IsTileFlippedDiagonally(x,y);
				tile.mBlank = (layer->GetTileTilesetIndex(x,y)==NULL_TILESET);
			}
			layerCount++;
		}
		
		//Object group layers
		size_t objectLayerCount=0;
		const std::vector< Tmx::ObjectGroup* >& objectGroups = map.GetObjectGroups();
		BOOST_FOREACH(Tmx::ObjectGroup* group, objectGroups)
		{
			if(group->GetNumObjects()==0)
			{
				ECHO_LOG_WARNING("Object group \"" << group->GetName() << "\" found without any objects. Skipped.");
				continue;
			}
			
			std::string groupName = group->GetName();
			if(groupName.empty())
			{
				//Assigned a name.
				std::stringstream ss;
				ss << objectLayerCount;
				groupName = ss.str();
			}
			//Otherwise lets create an entity.
			shared_ptr<SceneEntity> objectLayer = tileMap->CreateObjectLayer(groupName);

			//TODO: Set the colour.
			//group->GetColor() returns a string, format?
			//objectLayer->SetColour(Colour())
			objectLayer->SetPosition(group->GetX(), group->GetY(), group->GetZOrder());

			const std::vector< Tmx::Object* >& objects = group->GetObjects();
			BOOST_FOREACH(Tmx::Object* object, objects)
			{
				//If the object has a gid, then it is a tile.
				if(object->GetGid())
				{
					// The map will sort by the key which is the GID starts. We just need to reverse iterate and find the
					// first one that is less than or equal to the object gid.
					shared_ptr<TileSet> tileSet;
					int tileID=0;
					std::map< int, std::pair<size_t, const Tmx::Tileset* > >::reverse_iterator it = tileSetGIDStartLookup.rbegin();
					while(it!=tileSetGIDStartLookup.rend())
					{
						if(it->first<=object->GetGid())
						{
							tileSet=tileMap->GetTileSet(it->second.first);
							tileID=object->GetGid() - it->first;
							break;
						}
						++it;
					}

					if(tileSet)
					{
						//Use the object width if set, otherwise it is the tile size.
						f32 width = object->GetWidth();
						if(width==0)
						{
							width = tileWidth;
						}
						f32 height = object->GetHeight();
						if(height==0)
						{
							height = tileWidth;
						}
						
						std::pair<TextureUV,TextureUV> uvs=tileSet->CalculateUVs(tileID);

						// The position of the object is from 0,0 of the layer. We need to adjust the position by + halfX
						// and +halfY because the mesh is centred. Tiled is exporting the y position as the bottom of the
						// object rather than the top. I'm not sure if this is a bug or not, the TMX manual just says y
						// position but doesn't specify whether or not it is the top or bottom. It is inconsistent with
						// image layer positions which is a little annoying.
						f32 x=(width/2.f) + object->GetX();
						f32 y=yMax + (height/2.f) - object->GetY();
						
						TileMapObjectFactory::SceneEntityParameters objectParameters;
						objectParameters.mName=object->GetName();
						objectParameters.mMaterial=tileSet->GetMaterial();
						objectParameters.mTextureCoordinates=TextureUVPair(uvs.first,uvs.second);
						objectParameters.mSize=Vector2(width,height);
						objectParameters.mPosition=Vector3(x,y,0);
						objectParameters.mConfiguration.AddOptions(object->GetProperties().GetList());

						shared_ptr<SceneEntity> layerObject=tileMapObjectFactory->CreateObject(objectParameters);
						if(layerObject)
						{
							objectLayer->AddChild(layerObject);
						}
					}
				}else
				{
					//Otherwise it might be a shape, which we don't support yet.
					ECHO_LOG_WARNING("Object is not a tile object. \"" << object->GetName() << "\" will not be used because shapes aren't supported.");
				}
			}
		}

		//Create any image layers. These are background images, not objects.
		const std::vector< Tmx::ImageLayer* >& imageLayers = map.GetImageLayers();
		size_t imageLayerCount=0;
		BOOST_FOREACH(Tmx::ImageLayer* imageLayer, imageLayers)
		{
			const Tmx::Image* image = imageLayer->GetImage();
			if(!image)
			{
				ECHO_LOG_WARNING("Found image layer \"" << imageLayer->GetName() << "\" without any image. Skipped.");
				continue;
			}
			
			std::string layerName = imageLayer->GetName();
			if(layerName.empty())
			{
				//Assigned a name.
				std::stringstream ss;
				ss << imageLayerCount;
				layerName = ss.str();
			}
			
			//Otherwise lets create an entity.
			shared_ptr<SceneEntity> objectLayer = tileMap->CreateObjectLayer(layerName);
			if(!objectLayer)
			{
				ECHO_LOG_ERROR("Could not create object layer \"" << layerName << "\". Make sure your layer names are unique.");
				continue;
			}

			objectLayer->SetColour(Colour(1,1,1,imageLayer->GetOpacity()));

			std::string imagePathAndName = Utils::String::GetPathFromFilename(file.GetRequestedFileName()) + image->GetSource();
		
			// Get or create the material.
			shared_ptr<Material> material;
			if(materialManager.ResourceExists(imagePathAndName))
			{
				material = materialManager.GetResource(imagePathAndName);
			}else
			{
				material = materialManager.CreateMaterialUsingImage(imagePathAndName);
				
				// Apply some settings to the material that are relevant. These are deliberately not applied
				// if the material has been defined in the manager so users can override the material settings.
				//Turn on nearest filtering. This prevents some rendering artefacts.
				material->GetPass(0)->GetTextureUnit(0)->SetFilter(TextureUnit::TextureFilters::NEAREST);
				if(imageLayer->GetOpacity() < 1.f)
				{
					material->GetPass(0)->SetBlendMode(BlendModes::TRANSPARENT);
				}
			}

			
			shared_ptr<Mesh> mesh(new Mesh());
			
			//If the width or height of the image are 0 then it wasn't specified so we need to get the size from the texture.
			shared_ptr<Texture> texture = material->GetTexture();
			int width=image->GetWidth();
			int height=image->GetHeight();
			if(width==0)
			{
				width = texture->GetWidth();
			}
			if(height==0)
			{
				height = texture->GetWidth();
			}
			mesh->CreateQuadSubMesh(width,height);
			mesh->SetMaterial(material);
			objectLayer->SetMesh(mesh);

			// The position of the image layer is from 0,0 of the map. We need to adjust the position by + halfX
			// and halfY because the mesh is centred.
			f32 x=((f32)width)/2.f + imageLayer->GetX();
			f32 y=yMax - ((f32)height)/2.f - imageLayer->GetY();
			objectLayer->SetPosition(x,y, imageLayer->GetZOrder());
		}

		// Celebrate!
		return tileMap;
	}
}
