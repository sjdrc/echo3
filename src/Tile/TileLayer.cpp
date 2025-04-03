#include <echo/Tile/TileLayer.h>

namespace Echo
{
	TileLayer::TileLayer(shared_ptr<TileLayerMesh> tileLayerMesh) :
		mTileLayer(tileLayerMesh),
		mApplyParallax(true),
		mParallax(0,0)
	{
		assert(mTileLayer && "TileMap should never create a null TileLayerMesh");
		SetMesh(tileLayerMesh);
	}
	
	TileLayer::~TileLayer(){}

	void TileLayer::SetViewWindow(AxisAlignedBox window)
	{
		if(window.IsFinite())
		{
			window.Transform(GetTransform().Inverse());
		}
		mTileLayer->SetViewWindow(window);
	}

	void TileLayer::ClearViewWindow()
	{
		mTileLayer->ClearViewWindow();
	}
	
	void TileLayer::RebuildMesh()
	{
		mTileLayer->RebuildMesh();
	}
}
