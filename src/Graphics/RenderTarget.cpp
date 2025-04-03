#include <echo/Graphics/RenderTarget.h>

namespace Echo
{
	RenderTarget::RenderTarget()
	{
		mCurrentViewport = nullptr;
		mCurrentCamera = nullptr;
		mIsPrimary = false;
		mIsFullScreen = false;
		mDisplayXDPI = 0.f;		//Display X DPI, 0 if unknown
		mDisplayYDPI = 0.f;		//Display Y DPI, 0 if unknown
	}
	
	RenderTarget::~RenderTarget()
	{
	}
	
	f32 RenderTarget::GetAspectRatio() const
	{
		return (static_cast<f32>(GetWidth()) / static_cast<f32>(GetHeight()));
	}
	
	Metre RenderTarget::GetDisplayPhysicalWidth() const
	{
		u32 widthInPixels = GetWidth();
		if(mDisplayXDPI == 0.f || widthInPixels==0)
		{
			//We don't know.
			return Metre(0);
		}
		return Inch(widthInPixels / mDisplayXDPI);
	}
	
	Metre RenderTarget::GetDisplayPhysicalHeight() const
	{
		u32 heightInPixels = GetHeight();
		if(mDisplayYDPI == 0.f || heightInPixels==0)
		{
			//We don't know.
			return Metre(0);
		}
		
		return Inch(heightInPixels / mDisplayYDPI);
	}

	Metre RenderTarget::GetDisplayPhysicalDiagonalSize() const
	{
		Metre width = GetDisplayPhysicalWidth();
		Metre height = GetDisplayPhysicalHeight();
		if(width==Metre(0) || height==Metre(0))
		{
			return Metre(0);
		}
		return Metre(Maths::Hypotenuse(width.Count(),height.Count()));
	}

	void RenderTarget::SetTextureUnit(TextureUnit& textureUnit, u32 stage)
	{
		if(textureUnit.HasTexture())
		{
			SetTexture(textureUnit.GetTexture().get(), stage);
			SetTexGen(textureUnit.GetTexGenMode(), stage);
			SetTextureMatrix(textureUnit.GetTextureMatrix(), stage);
			SetWrapModeU(textureUnit.GetWrapModeU());
			SetWrapModeV(textureUnit.GetWrapModeV());
			SetMinFilter(textureUnit.GetMinFilter());
			SetMagFilter(textureUnit.GetMagFilter());
			SetTexture2DEnabled(true, stage);
			SetTextureStageBlendMode(textureUnit.GetBlendMode(), stage);
			SetTextureStageBlendMode(textureUnit.GetAlphaBlendMode(), stage);
		}else
		{
			if(textureUnit.IsCubeMap())
			{
				SetCubeMap(textureUnit.GetCubeMap().get(),stage);
			}
		}
	}

}
