#include <echo/Tile/TileMap.h>
#include <echo/Tile/TileLayer.h>
#include <echo/Tile/TileSet.h>
#include <echo/Graphics/Material.h>
#include <echo/Graphics/RenderPass.h>
#include <echo/Graphics/TextureUnit.h>
#include <echo/Graphics/Camera.h>

namespace Echo
{
	TileMap::TileMap() : mUpdateMeshForCameras(true)
	{
	}

	TileMap::~TileMap()
	{
	}
	
	size_t TileMap::GetLayerWidthInTiles(std::string layerName) const
	{
		if(layerName.empty())
		{
			layerName = mFirstLayerName;
		}
		
		if(layerName.empty())
		{
			return 0u;
		}
		
		shared_ptr<TileLayer> layer = GetTileLayer(layerName);
		if(!layer)
		{
			ECHO_LOG_ERROR("Unable to find layer \"" << layerName << "\"");
			return 0u;
		}
		return layer->GetWidthInTiles();
	}
	
	size_t TileMap::GetLayerHeightInTiles(std::string layerName) const
	{
		if(layerName.empty())
		{
			layerName = mFirstLayerName;
		}
		
		if(layerName.empty())
		{
			return 0u;
		}
		
		shared_ptr<TileLayer> layer = GetTileLayer(layerName);
		if(!layer)
		{
			ECHO_LOG_ERROR("Unable to find layer \"" << layerName << "\"");
			return 0u;
		}
		return layer->GetHeightInTiles();
	}
	
	Vector2 TileMap::GetLayerTileSize(std::string layerName) const
	{
		if(layerName.empty())
		{
			layerName = mFirstLayerName;
		}
		
		if(layerName.empty())
		{
			return Vector2::ZERO;
		}
		
		shared_ptr<TileLayer> layer = GetTileLayer(layerName);
		if(!layer)
		{
			ECHO_LOG_ERROR("Unable to find layer \"" << layerName << "\"");
			return Vector2::ZERO;
		}
		return layer->GetTileSize();
	}
	
	size_t TileMap::AddTileSet(const TileSet& tileSet)
	{
		mTileSets.push_back(shared_ptr<TileSet>(new TileSet(tileSet)));
		return mTileSets.size()-1;
	}

	shared_ptr<TileSet> TileMap::GetTileSet(size_t tileSetID)
	{
		if(tileSetID >= mTileSets.size())
		{
			return shared_ptr<TileSet>();
		}
		return mTileSets[tileSetID];
	}
	
	shared_ptr<TileLayer> TileMap::GetTileLayer(const std::string& name) const
	{
		NamedLayers::const_iterator it = mTileLayers.find(name);
		if(it == mTileLayers.end())
		{
			return shared_ptr<TileLayer>();
		}
		return it->second;
	}
	
	TileMap::NamedLayers TileMap::GetTileLayers(void)
	{
		return mTileLayers;
	}
	
	shared_ptr<TileLayer> TileMap::CreateTileLayer(const std::string& name, size_t widthTiles, size_t heightTiles, size_t tileSetIndex, Scalar tileWidth, Scalar tileHeight)
	{
		assert(tileSetIndex<mTileSets.size() && "tileSetIndex invalid.");
		assert(widthTiles>0 && "widthTiles must be greater than 0");
		assert(heightTiles>0 && "heightTiles must be greater than 0");
		assert(!name.empty() && "name must not be empty");
		if(tileSetIndex>=mTileSets.size() || widthTiles==0 || heightTiles==0 || name.empty())
		{
			ECHO_LOG_ERROR("Invalid parameter with: name:" << name << " widthTiles:" << widthTiles << " heightTiles:" << heightTiles << " tileSetIndex:" << tileSetIndex << " mTileSets.size():" << mTileSets.size());
			return shared_ptr<TileLayer>();
		}

		//Check that the layer name doesn't already exist.
		if(GetTileLayer(name))
		{
			ECHO_LOG_ERROR("Cannot create layer with name \"" << name << "\". A layer with the same name already exists.");
			return shared_ptr<TileLayer>();
		}
		
		shared_ptr<TileSet> tileSet=mTileSets[tileSetIndex];

		if(tileWidth<=0.f)
		{
			tileWidth = tileSet->GetTileWidth();
		}
		
		if(tileHeight<=0.f)
		{
			tileHeight = tileSet->GetTileWidth();
		}
		
		shared_ptr<TileLayerMesh> newLayer(new TileLayerMesh(*this,widthTiles, heightTiles, tileSet, tileWidth, tileHeight));

		//Create a SceneEntity.
		shared_ptr<TileLayer> entity(new TileLayer(newLayer));
		entity->SetName(name);
		mTileLayers.insert(std::make_pair(name,entity));
		AddChild(*entity);
		
		if(mFirstLayerName.empty())
		{
			mFirstLayerName = name;
		}

		return entity;
	}

	shared_ptr<SceneEntity> TileMap::GetObjectLayer(const std::string& name) const
	{
		std::map< std::string, shared_ptr<SceneEntity> >::const_iterator it = mObjectLayers.find(name);
		if(it == mObjectLayers.end())
		{
			return shared_ptr<SceneEntity>();
		}
		return it->second;
	}
	
	shared_ptr<SceneEntity> TileMap::CreateObjectLayer(const std::string& name)
	{
		if(GetObjectLayer(name))
		{
			ECHO_LOG_ERROR("Cannot create layer with name \"" << name << "\". A layer with the same name already exists.");
			return shared_ptr<SceneEntity>();
		}
		//Create a SceneEntity.
		shared_ptr<SceneEntity> entity(new SceneEntity());
		entity->SetName(name);
		mObjectLayers.insert(std::make_pair(name,entity));
		AddChild(*entity);
		return entity;
	}
	
	void TileMap::SetPosition(const Vector3& newPosition)
	{
		Vector3 scale=GetScale();
		
		//Prevent NaN.
		if(scale.x==0.f)
		{
			scale.x=1.f;
		}
		if(scale.y==0.f)
		{
			scale.y=1.f;
		}

		std::map< std::string, shared_ptr<TileLayer> >::iterator it=mTileLayers.begin();
		std::map< std::string, shared_ptr<TileLayer> >::iterator itEnd=mTileLayers.end();
		while(it!=itEnd)
		{
			if(it->second->GetApplyParallax())
			{
				Vector3 childPosition = it->second->GetPosition();
			
				// The position of each layer is newPosition * the layer parallax by the inverse of the scale.
				// If we don't apply the inverse of the scale then the effect of the parallax is also scaled.
				childPosition.x = (newPosition.x * it->second->GetParallax().x) / scale.x;
				childPosition.y = (newPosition.y * it->second->GetParallax().y) / scale.y;
				it->second->SetPosition(childPosition);
			}
			++it;
		}
		
		//That sorts out the children. Now set this object.
		Node::SetPosition(newPosition);
	}

	void TileMap::SetPosition(f32 x, f32 y, f32 z)
	{
		SetPosition(Vector3(x,y,z));
	}
	
	void TileMap::SetViewWindow(AxisAlignedBox window)
	{
		mViewWindow = window;
		UpdateLayerMeshes();
	}

	void TileMap::UpdateLayerMeshes()
	{
		std::map< std::string, shared_ptr<TileLayer> >::iterator it=mTileLayers.begin();
		std::map< std::string, shared_ptr<TileLayer> >::iterator itEnd=mTileLayers.end();
		while(it!=itEnd)
		{
			it->second->SetViewWindow(mViewWindow);
			++it;
		}
	}

	void TileMap::Accept(SceneRenderableVisitor& visitor)
	{
		if(mUpdateMeshForCameras)
		{
			//TODO: This function is a little bit hacky. It will only work properly for cameras that look straight
			// on to the tile map. The method needs to be updated to calculate the intersect plane of the TileMap
			// and the camera's frustum then use that to calculate a AABB. I have done this quick implementation
			// for improved performance until I get around to writing this properly..
			const Camera* camera = visitor.GetCurrentCamera();

			if(camera)
			{
				std::map< std::string, shared_ptr<TileLayer> >::iterator it=mTileLayers.begin();
				std::map< std::string, shared_ptr<TileLayer> >::iterator itEnd=mTileLayers.end();
				while(it!=itEnd)
				{
					Vector2 size;
					Vector3 layerPosition = it->second->GetDerivedPosition();
					if(camera->GetProjectionType()==ProjectionTypes::PERSPECTIVE)
					{
						Scalar distance = Maths::Abs(layerPosition.z - camera->GetPosition().z);

						// Calculate so that the horizontal view takes up viewWidth units.
						Radian halfFOVx = camera->GetFOV() / 2.f;
						Scalar width = (Maths::Tan(halfFOVx.ValueRadians()) * distance) * 2.f;
						Scalar height = width / camera->GetAspectRatio();

						size=Vector2(width,height);
					}else
					{
						size = Vector2(camera->GetOrthoWindowWidth(),camera->GetOrthoWindowHeight());
					}
					
					// Half the size since the box will be from the Camera's position which
					// probably is the centre of the viewport.
					size*=0.5;

					AxisAlignedBox window(	Vector3(camera->GetPosition().x-size.x,camera->GetPosition().y-size.y,-1),
											Vector3(camera->GetPosition().x+size.x,camera->GetPosition().y+size.y,1));
					it->second->SetViewWindow(window);
					++it;
				}
			}
		}
		SceneEntity::Accept(visitor);
	}
}
