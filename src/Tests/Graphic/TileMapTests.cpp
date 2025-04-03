#include <echo/Graphics/SceneEntity.h>
#include <echo/Kernel/Task.h>
#include <echo/Application.h>
#include <echo/Graphics/Scene.h>
#include <echo/Graphics/Renderer.h>
#include <echo/Resource/MaterialManager.h>
#include <echo/Resource/TextureManager.h>
#include <echo/Graphics/MultiRenderer.h>
#include <echo/Graphics/Viewport.h>
#include <echo/Graphics/Camera.h>
#include <echo/FileSystem/FileSystem.h>
#include <echo/Tile/TileMap.h>
#include <echo/Tile/TileLayer.h>
#include <echo/Tile/TileSet.h>
#include <echo/Tile/TMXLoader.h>
#include <echo/Input/Input.h>
#include <echo/cpp/functional>

#include <doctest/doctest.h>

using namespace Echo;

namespace TileMapTests
{
	class TileMapWindow : public Task
	{
	public:
		TileMapWindow(shared_ptr< Input<bool> > button,
					 shared_ptr< Input<f32> > xInput,
					 shared_ptr< Input<f32> > yInput,
					 shared_ptr< TileMap > tileMap,
					 AxisAlignedBox window,
					 shared_ptr<Renderer> renderer) :
			mTileMap(tileMap),
			mRenderer(renderer),
			mWindow(window),
			mButton(button),
			mX(xInput),
			mY(yInput)
		{}

		bool OnStart() override
		{
			//Prevent starting this task if we have any null required parameters.
			return (mRenderer && mTileMap && mButton && mX && mY);
		}

		void Update(Seconds) override
		{
			optional<Ray> rayIntoScene = mRenderer->GetRayIntoScene(*mX,*mY);
			if(rayIntoScene)
			{
				const Ray& ray = rayIntoScene.value();
				Plane plane(Vector3::UNIT_Z,0.f);
				std::pair<bool, f32> intersectResult = ray.Intersects(plane);
				if(intersectResult.first)
				{
					Vector3 positionInSpace = ray.GetPoint(intersectResult.second);
					if(*mButton)
					{
						AxisAlignedBox window = mWindow;
						Matrix4 t;
						t.MakeTransform(positionInSpace,Vector3::UNIT_SCALE,Quaternion::IDENTITY);
						window.Transform(t);
						mTileMap->SetViewWindow(window);
					}else
					{
						mTileMap->ClearViewWindow();
					}
				}
			}
		}
	private:
		shared_ptr<TileMap> mTileMap;
		shared_ptr<Renderer> mRenderer;
		AxisAlignedBox mWindow;
		shared_ptr< Input<bool> > mButton;
		shared_ptr< Input<f32> > mX;
		shared_ptr< Input<f32> > mY;
	};

	class TileMapTestApplication : public Application
	{
	public:
		class Spinner : public Task
		{
		public:
			typedef function<void(f32,f32)> SetFunction;

			Spinner(SetFunction setFunction, Vector2 magnitudeMultiplier, f32 speedMultiplier, f32 startAngle) :
				mSetFunction(setFunction),
				mAngle(startAngle),
				mSpeedMultiplier(speedMultiplier),
				mMagnitudeMultiplier(magnitudeMultiplier)
			{

			}

			void Update(Seconds lastFrameTime)
			{
				mAngle += static_cast<f32>(lastFrameTime.count()) * mSpeedMultiplier;
				mSetFunction(Maths::Sin(mAngle,false) * mMagnitudeMultiplier.x,Maths::Cos(mAngle,false) * mMagnitudeMultiplier.y);
			}
			SetFunction mSetFunction;
			f32 mAngle;
			f32 mSpeedMultiplier;
			Vector2 mMagnitudeMultiplier;
		};

		bool Initialise()
		{
			//Initialise the application from a config file.
			if (!Application::Initialise("TileMapTest", "data/TileMapTest.config"))
			{
				return false;
			}

			shared_ptr<Viewport> leftViewport(new Viewport(0,0,1,1));
			shared_ptr<Camera> camera1 = mScene.CreateCamera();
			camera1->UpdateAspectForViewport(*leftViewport, *GetRenderTarget());
			camera1->SetPosition(Vector3(0,0,70));

			shared_ptr<Renderer> renderer = GetRenderer()->CreateRenderer(leftViewport, camera1);

			mScene.SetUseOnlyZForDistanceCalculations(true);
			AddTask(mScene);

			//Create a TileMap
			shared_ptr<TileMap> tileMap(new TileMap());
			
			//Create a TileLayer. This one will be a reconstruction of the Echo logo with an extra row of flipped and rotated tiles.
			size_t tileWidth = 32;
			size_t tileHeight = 32;
			shared_ptr<Material> material = GetMaterialManager()->GetResource("Echo");
			material->GetPass(0)->SetVertexColouringEnabled(true);
			shared_ptr<Texture> texture = material->GetTexture();
			size_t tileSetIndex = tileMap->AddTileSet(TileSet(material,tileWidth,tileHeight,texture->GetWidth(),texture->GetHeight()));
			size_t width=texture->GetWidth()/tileWidth;
			size_t height=texture->GetHeight()/tileHeight;
			s32 tilePhysicalWidth = 4;
			s32 tilePhysicalHeight = 4;
			shared_ptr<TileLayer> layer = tileMap->CreateTileLayer("1", width,height + 1,tileSetIndex,tilePhysicalWidth,tilePhysicalHeight);
			tileMap->SetPosition(Vector3(0,0,0));
			layer->SetPosition(Vector3(-(tilePhysicalWidth*((s32)width)*0.5),-tilePhysicalHeight*((s32)height)*0.5,0));

			for(size_t y=0;y<height+1;++y)
			for(size_t x=0;x<width;++x)
			{
				TileLayer& tl=*layer;
				Tile& tile=tl[x][y];
				tile.mIndex=y*width + x;

				//Flip the edges, except for the very last tile which we will rotate.
				if(y==height)
				{
					tile.mIndex=(width-1)*width + height-1;
					switch(x)
					{
						case 0: /*no transform*/ break;					//0 degrees rotation
						case 1:	tile.mFlipHorrizontally=true; break;
						case 2:	tile.mFlipVertically=true; break;
						case 3:											//180 degrees rotation
							tile.mFlipVertically=true;
							tile.mFlipHorrizontally=true;
						break;
						case 4:	tile.mRotate90Degrees=true; break;		//90 degrees rotation
						case 5:											
							tile.mRotate90Degrees=true;
							tile.mFlipHorrizontally=true;
						break;
						case 6:
							tile.mRotate90Degrees=true;
							tile.mFlipVertically=true;
						break;
						case 7:											//270 degree rotation
							tile.mRotate90Degrees=true;
							tile.mFlipVertically=true;
							tile.mFlipHorrizontally=true;
						break;
					}
				}else
				{
					if(x==width-1 || x==0)
					{
						tile.mFlipHorrizontally=true;
						tile.mColour.mRed=0.5;
					}
					if(y==height-1 || y==0)
					{
						tile.mFlipVertically=true;
						tile.mColour.mBlue=0.5;
					}
				}

			}

			//Create a second larger layer at z=1 with smaller tiles and randomise the tiles.
			material = GetMaterialManager()->GetResource("MultiStageEcho");
			material->GetPass(0)->SetDiffuse(Colour(1,1,1,0.25));
			material->GetPass(0)->SetBlendMode(BlendModes::TRANSPARENT);
			size_t newWidth = width * 4;
			size_t newHeight = height * 2.5;
			tileSetIndex = tileMap->AddTileSet(TileSet(material,tileWidth,tileHeight,texture->GetWidth(),texture->GetHeight()));
			layer = tileMap->CreateTileLayer("2", newWidth,newHeight,tileSetIndex,tilePhysicalWidth,tilePhysicalHeight);
			layer->SetPosition(Vector3(-(newWidth*tilePhysicalWidth*0.25),-(newHeight*tilePhysicalHeight*0.25),1));
			layer->SetTileSize(tilePhysicalWidth*0.5,tilePhysicalHeight*0.5);
			for(size_t y=0;y<newHeight;++y)
			for(size_t x=0;x<newWidth;++x)
			{
				TileLayer& tl=*layer;
				tl[x][y].mIndex=Maths::Random(0,width*height);
			}

			mScene.AddRenderable(tileMap);

			//Load a TileMap from a TMX file.
			shared_ptr<TileMap> tmxMap = LoadTMXFile(GetFileSystem()->Open("data/TileMap.tmx"),*GetMaterialManager());
			if(tmxMap)
			{
				Scalar scale = 1./16.;
				tmxMap->SetUniformScale(scale);
				Vector2 mapSize = (tmxMap->GetLayerTileSize() * Vector2(tmxMap->GetLayerWidthInTiles(),tmxMap->GetLayerHeightInTiles())) * scale;
				
				// Create a Node for an offset. I used a SceneEntity here because you can't add Node's to a Scene directly
				// and I did not want to make the node a member object.
				shared_ptr<SceneEntity> tileMapOffset(new SceneEntity(Vector3(-mapSize.x/2,-mapSize.y/2,5)));		//Magic offset.
				tileMapOffset->AddChild(tmxMap);
				mScene.AddRenderable(tileMapOffset);
				
				// Move the tilemap in a circle. If there is layer parallax configured you'll see it in action.
				shared_ptr<Spinner> spinner(new Spinner(bind(static_cast<void(TileMap::*)(f32,f32,f32)>(&TileMap::SetPosition),tmxMap,placeholders::_1,placeholders::_2,0),Vector2(-10.f,-10.f),1.0f,0.f));
				AddTask(spinner);
			
				tmxMap->SetUpdateMeshForCameras(false);
				shared_ptr<TileMapWindow> window(new TileMapWindow(
										GetInputManager()->GetInput<bool>("Mouse:Left"),
										GetInputManager()->GetInput<f32>("Mouse:X"),
										GetInputManager()->GetInput<f32>("Mouse:Y"),
										tmxMap,
										AxisAlignedBox(Vector3(-5,-5,-1),Vector3(5,5,1)),renderer));
				AddTask(window);
			}

			
			AddTask(GetRenderer());
			return true;
		}

		Scene mScene;
	};
	TEST_CASE("TileMap")
	{
		gDefaultLogger.SetLogMask(Echo::Logger::LogLevels::NONE);
		TileMapTestApplication tileMapTestApplication;
		REQUIRE(tileMapTestApplication.Initialise());
		tileMapTestApplication.Execute();
	}
}

