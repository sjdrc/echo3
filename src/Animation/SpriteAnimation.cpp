#include <echo/Animation/SpriteAnimation.h>
#include <echo/Resource/TextureManager.h>
#include <echo/Util/Utils.h>
#include <boost/foreach.hpp>

namespace Echo
{
	
	SpriteAnimation::SpriteAnimation(TextureManager& textureManager, const std::string& name) :
		mTextureManager(textureManager),
		mMode(Modes::LOOP),
		mWidthInPixels(0.f),
		mHeightInPixels(0.f)
	{
	}

	SpriteAnimation::~SpriteAnimation()
	{

	}

	SpriteAnimation::Frame& SpriteAnimation::AddFrame(const std::string& resourceName, Seconds frameTimeInSeconds, bool useTextureSize, bool preloadResource)
	{
		mFrames.push_back(Frame(resourceName,frameTimeInSeconds));
		Frame& frame = mFrames.back();
		
		if(preloadResource || useTextureSize)
		{
			frame.texture(AcquireTexture(resourceName));
		}
		if(frame.texture() && useTextureSize)
		{
			frame.width(frame.texture()->GetWidth());
			frame.height(frame.texture()->GetHeight());
		}
		return frame;
	}

	void SpriteAnimation::AddFrames(const std::string& imageName, Seconds frameTimeInSeconds,f32 frameWidth, f32 frameHeight)
	{
		shared_ptr<Texture> texture = AcquireTexture(imageName);
		if(!texture || frameWidth == 0.f || frameHeight == 0.f)
		{
			return;
		}
		
		if(frameWidth<=1.f)
		{
			frameWidth = texture->GetWidth() * frameWidth;
		}

		if(frameHeight<=1.f)
		{
			frameHeight = texture->GetHeight() * frameHeight;
		}
		
		u16 numCols = static_cast<u16>(texture->GetWidth() / frameWidth);
		u16 numRows = static_cast<u16>(texture->GetHeight() / frameHeight);

		f32 u1 = 0.f;
		f32 v1 = 0.f;
		f32 uSize = frameWidth / static_cast<f32>(texture->GetWidth());
		f32 vSize = frameHeight / static_cast<f32>(texture->GetHeight());
		for(u16 row = 0; row < numRows; row++)
		{
			for(u16 col = 0; col < numCols; col++)
			{
				AddFrame(imageName, frameTimeInSeconds)
						.width(frameWidth)
						.height(frameHeight)
						.uv1(TextureUV(u1, v1))
						.uv2(TextureUV(u1 + uSize, v1 + vSize));
				u1 += uSize;
			}
			v1 += vSize;
			u1 = 0.f;
		}
	}

	void SpriteAnimation::SetWidthInPixels(f32 width)
	{
		mWidthInPixels = width;
	}

	void SpriteAnimation::SetHeightInPixels(f32 height)
	{
		mHeightInPixels = height;
	}

	void SpriteAnimation::AddFrameFunction(size_t frameIndex, FrameFunction frameFunction)
	{
		mFrameFunctions[frameIndex].push_back(frameFunction);
	}
	
	void SpriteAnimation::AddEndFunction(FrameFunction frameFunction)
	{
		mEndFunctions.push_back(frameFunction);
	}

	void SpriteAnimation::ClearFrames()
	{
		mFrames.clear();
		mFrameFunctions.clear();
	}
	
	void SpriteAnimation::ClearEndCallbacks()
	{
		mEndFunctions.clear();
	}

	void SpriteAnimation::SetMode(Mode mode)
	{
		mMode = mode;
	}

	SpriteAnimation::Mode SpriteAnimation::GetMode() const
	{
		return mMode;
	}

	shared_ptr<Texture> SpriteAnimation::AcquireTexture(const std::string& resourceName)
	{
		return mTextureManager.GetResource(resourceName);
	}

	size_t SpriteAnimation::GetNumberOfFrames() const
	{
		return mFrames.size();
	}
	
	void SpriteAnimation::ProcessFrameCallbacks(Sprite& sprite, size_t frameIndex)
	{
		std::map<size_t, std::vector<FrameFunction> >::iterator it =mFrameFunctions.find(frameIndex);
		if(it==mFrameFunctions.end())
		{
			return;
		}
		BOOST_FOREACH(FrameFunction& funct,it->second)
		{
			funct(*this,sprite,frameIndex);
		}
	}
	
	void SpriteAnimation::ProcessEndFrameCallbacks(Sprite& sprite)
	{
		BOOST_FOREACH(FrameFunction& funct,mEndFunctions)
		{
			funct(*this,sprite,mFrames.size()-1);
		}
	}
	
}
