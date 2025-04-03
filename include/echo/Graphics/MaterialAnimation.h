/* 
 * File:   MaterialAnimation.h
 * Author: DiamondLawrence
 *
 * Created on 10 April 2015, 11:36 PM
 */

#ifndef ECHO_MATERIALANIMATION_H
#define ECHO_MATERIALANIMATION_H

#include <echo/Graphics/Material.h>
#include <echo/Graphics/PrimitiveTypes.h>
#include <echo/Kernel/Task.h>
#include <boost/optional.hpp>

namespace Echo
{
	//! \brief   An animation sequence applied as a material, allowing meshes to have an animated material or texture.
	//! \details This is a Task so it needs to be added to a parent process in order to animate.
	class MaterialAnimation : public Material, public Task
	{
	public:
		//! \brief An individual frame of a MaterialAnimation sequence.
		struct Frame
		{
			//! \brief Constructs a MaterialAnimation::Frame.
			//! \param duration					 The duration of the frame, in seconds.
			//! \param texture					  The texture to use for this frame, which can be shared with other frames.
			//! \param textureCoordinateTopLeft	 The top-left corner of the part of the texture to be rendered for this frame, specified in texture coordinates.
			//! \param textureCoordinateBottomRight The bottom-right corner of the part of the texture to be rendered for this frame, specified in texture coordinates.
			Frame(Seconds duration,
				  shared_ptr<Texture> texture,
				  TextureUV textureCoordinateTopLeft = TextureUV(0.f, 0.f),
				  TextureUV textureCoordinateBottomRight = TextureUV(1.f, 1.f));
			
			Seconds duration;			//!< The duration of the frame, in seconds.
			shared_ptr<Texture> texture; //!< The texture to use for this frame, which can be shared with other frames.
			TextureUV textureUV1;		//!< The top-left corner of the part of the texture to be rendered for this frame, specified in texture coordinates.
			TextureUV textureUV2;		//!< The bottom-right corner of the part of the texture to be rendered for this frame, specified in texture coordinates.
		};
		
		//! \brief   A "filmstrip" or frame set, used to construct a Frame sequence from a single Texture.
		//! \details The provided Texture is treated as a "filmstrip" in containing separate sequential non-overlapping sub-Textures to treat as Frames.
		struct FilmStrip
		{
			//! \brief Constructs a FilmStrip, aka Frame sequence.
			//! \param durationPerFrame The duration, in seconds, of each Frame in the sequence to generate.
			//! \param texture		  The Texture to subdivide into various Frames.
			//! \param sizePerFrame	 The size, in texture coordinates, of the sub-Texture each Frame will have.
			//! \param horizontalFlip   true to flip the texture horizontally, false otherwise.
			//! \param numberOfFrames   If specified, the number of frames to extract from the Texture.
			//!						 If not specified, will retrieve the maximum number of Frames that the Texture can be divided into.
			//! \param origin		   The origin point to begin extracting Frames from the Texture: the top-left corner of the very first Frame.
			FilmStrip(Seconds durationPerFrame,
					  shared_ptr<Texture> texture,
					  TextureUV sizePerFrame,
					  bool horizontalFlip = false,
					  optional<u32> numberOfFrames = none,
					  TextureUV origin = TextureUV(0.f, 0.f));
			
			//! \brief A FilmStrip will implicitly convert into a Frame sequence.
			operator std::vector<Frame>(void) const
			{
				return frames;
			}
			
			std::vector<Frame> frames; //!< The Frame sequence calculated from constructor parameters.
		};
		
		//! \brief Constructs an empty MaterialAnimation.
		//! \note  Call SetTaskName() and SetFrames() to fully initialise this animation sequence.
		MaterialAnimation(void);
		//! \brief Constructs a MaterialAnimation from the provided textureFrames.
		//! \param taskName The name of this animation task.
		//! \param frames   The ordered sequence of frames to use for this animation, in order of index.
		//! \param priority The priority of this MaterialAnimation Task, as per Task::GetPriority().
		MaterialAnimation(const std::string& taskName, const std::vector<Frame>& frames, u32 priority = 5000);
		MaterialAnimation(const MaterialAnimation& other);
		~MaterialAnimation(void) override;
		
		//! \brief Sets the sequence of frames that this MaterialAnimation will use.
		//! \param frames   The ordered sequence of frames to use for this animation, in order of index.
		//! \see   Refer also to MaterialAnimation::FilmStrip which can be used to easily initialise this.
		void SetFrames(const std::vector<Frame>& frames);
		
		//! \brief  Gets the total duration, in seconds, of the animation (i.e. the accumulation of the duration for each Frame in the sequence).
		//! \return The total duration of the animation sequence.
		Seconds GetTotalDuration(void) const;
		
		void Update(Seconds lastFrameTime) override;
		void ApplyAndRender(RenderContext& renderContext, MultipassRenderable& renderable, const Matrix4& world, const Matrix4& worldView, Colour compoundDiffuse) override;
		
	private:
		Seconds mTimeIntoAnimation;					  //!< The time into the animation sequence: the amount of time that has passed while the animation has been active.
														 //!  This loops based on the total duration of the frame sequence.
		std::vector<Frame> mFrames;					  //!< The ordered sequence of frames to use for this animation, in order of index.
		std::vector<Frame>::const_iterator mActiveFrame; //!< The iterator that points to the current active frame in mFrames.
	};
}

#endif
