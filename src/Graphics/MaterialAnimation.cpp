/* 
 * File:   MaterialAnimation.cpp
 * Author: DiamondLawrence
 * 
 * Created on 10 April 2015, 11:36 PM
 */

#include <echo/Graphics/MaterialAnimation.h>
#include <echo/Graphics/SubMesh.h>

#include <boost/foreach.hpp>

namespace Echo
{
	MaterialAnimation::Frame::Frame(Seconds duration,
									shared_ptr<Texture> texture,
									TextureUV textureCoordinateTopLeft,
									TextureUV textureCoordinateBottomRight)
		:
		duration(duration),
		texture(texture),
		textureUV1(textureCoordinateTopLeft),
		textureUV2(textureCoordinateBottomRight)
	{
	}
	
	MaterialAnimation::FilmStrip::FilmStrip(Seconds durationPerFrame,
											shared_ptr<Texture> texture,
											TextureUV sizePerFrame,
											bool horizontalFlip,
											optional<u32> numberOfFrames,
											TextureUV origin)
		:
		frames()
	{
		if (!texture || sizePerFrame.u <= 0.f || sizePerFrame.v <= 0.f)
		{
			return;
		}
		const u32 numberOfColumns = static_cast<u32>(1.f / sizePerFrame.u);
		const u32 numberOfRows = static_cast<u32>(1.f / sizePerFrame.v);
		const u32 maximumFrames = numberOfColumns * numberOfRows;
		
		if (!numberOfFrames || *numberOfFrames > maximumFrames)
		{
			numberOfFrames = maximumFrames;
		}
		for (u32 frameNumber = 0; frameNumber < *numberOfFrames; ++frameNumber)
		{
			const u32 column = frameNumber % numberOfColumns;
			const u32 row = frameNumber / numberOfColumns;
			const TextureUV topLeft(origin.u + column * sizePerFrame.u, origin.v + row * sizePerFrame.v);
			const TextureUV bottomRight(origin.u + (column+1) * sizePerFrame.u, origin.v + (row+1) * sizePerFrame.v);
			if (horizontalFlip)
			{
				frames.push_back(Frame(durationPerFrame, texture, TextureUV(bottomRight.u, topLeft.v), TextureUV(topLeft.u, bottomRight.v)));
			}
			else
			{
				frames.push_back(Frame(durationPerFrame, texture, topLeft, bottomRight));
			}
		}
	}
	
	MaterialAnimation::MaterialAnimation(void)
		: Material(), Task(),
		mTimeIntoAnimation(0.0),
		mFrames(),
		mActiveFrame(mFrames.end())
	{
	}
	
	MaterialAnimation::MaterialAnimation(const std::string& taskName, const std::vector<Frame>& frames, u32 priority)
		: Material(), Task(taskName, priority),
		mTimeIntoAnimation(0.0),
		mFrames(frames),
		mActiveFrame(mFrames.end())
	{
		if (frames.size() > 0)
		{
			mActiveFrame = mFrames.begin();
			SetTexture(frames.front().texture);
		}
	}
	
	MaterialAnimation::MaterialAnimation(const MaterialAnimation& other)
		: Material(other), Task(other),
		mTimeIntoAnimation(other.mTimeIntoAnimation),
		mFrames(other.mFrames),
		mActiveFrame(other.mActiveFrame)
	{
	}
	
	MaterialAnimation::~MaterialAnimation(void)
	{
	}
	
	void MaterialAnimation::SetFrames(const std::vector<Frame>& frames)
	{
		mFrames = frames;
	}
	
	Seconds MaterialAnimation::GetTotalDuration(void) const
	{
		Seconds animationDuration(0);
		BOOST_FOREACH(const Frame& frame, mFrames)
		{
			animationDuration += frame.duration;
		}
		return animationDuration;
	}
	
	void MaterialAnimation::Update(Seconds lastFrameTime)
	{
		mTimeIntoAnimation += lastFrameTime;
		
		const Seconds animationDuration = GetTotalDuration();
		while (animationDuration > Seconds(0) && mTimeIntoAnimation > animationDuration)
		{
			mTimeIntoAnimation -= animationDuration;
		}
		Seconds durationToFrameStart(0.0);
		mActiveFrame = mFrames.end();
		for (std::vector<Frame>::const_iterator frame = mFrames.begin();
			 frame != mFrames.end(); ++frame)
		{
			if (mTimeIntoAnimation >= durationToFrameStart)
			{
				mActiveFrame = frame;
			}
			durationToFrameStart += frame->duration;
		}
		if (mActiveFrame != mFrames.end())
		{
			SetTexture(mActiveFrame->texture);
		}
	}
	
	void MaterialAnimation::ApplyAndRender(RenderContext& renderContext, MultipassRenderable& renderable, const Matrix4& world, const Matrix4& worldView, Colour compoundDiffuse)
	{
		if (mActiveFrame != mFrames.end())
		{
			if (SubMesh* subMesh = dynamic_cast<SubMesh*>(&renderable))
			{
				VertexBuffer::Accessor<TextureUV> textureUVs = subMesh->GetComponents<TextureUV>("UV0");
				textureUVs[0] = TextureUV(mActiveFrame->textureUV1.u, mActiveFrame->textureUV1.v);
				textureUVs[1] = TextureUV(mActiveFrame->textureUV2.u, mActiveFrame->textureUV1.v);
				textureUVs[2] = TextureUV(mActiveFrame->textureUV1.u, mActiveFrame->textureUV2.v);
				textureUVs[3] = TextureUV(mActiveFrame->textureUV2.u, mActiveFrame->textureUV2.v);
				subMesh->Finalise();
			}
		}
		return Material::ApplyAndRender(renderContext, renderable, world, worldView, compoundDiffuse);
	}
}
