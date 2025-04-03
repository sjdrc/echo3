#ifndef ECHOSPRITE_H
#define ECHOSPRITE_H

#include <echo/Graphics/SceneEntity.h>
#include <echo/Animation/SpriteAnimation.h>
#include <echo/Kernel/Task.h>
#include <echo/Maths/Vector2.h>

namespace Echo
{
	class SpriteAnimation;
	
	//! \brief A sprite, or billboard, is a flat textured rectangle that always faces the camera (by default).
	//! \see   https://en.wikipedia.org/wiki/Sprite_%28computer_graphics%29
	class Sprite : public SceneEntity, public Task
	{
	public:
		Sprite(const Vector3& position = Vector3::ZERO,
			   const Quaternion& orientation = Quaternion::IDENTITY,
			   const Vector3& scale = Vector3::UNIT_SCALE);
		Sprite(shared_ptr<Material> material,
			   const Vector3& position = Vector3::ZERO,
			   const Quaternion& orientation = Quaternion::IDENTITY,
			   const Vector3& scale = Vector3::UNIT_SCALE);
		Sprite(const Sprite& other);
		virtual ~Sprite(void);
		
		/**
		 * Sets the Sprite to always look at whatever camera is going to render.
		 * @note This makes use of Node::SetInitialOrientation() when the feature is turned on
		 * and the initial orientation is restored when the feature is turned off.
		 * The default value is true.
		 * @param alwaysFaceCamera true for this sprite to always face the camera, false otherwise.
		 */
		void SetAlwaysFaceCamera(bool alwaysFaceCamera);
		bool GetAlwaysFaceCamera() const {return mAlwaysFaceCamera;}
		
		/**
		 * Override from SceneRenderable.
		 * This method allows the sprite to modify its orientation to face the camera if the option is turned on.
		 * @param visitor
		 */
		virtual void Accept(SceneRenderableVisitor& visitor) override;
		
		/**
		 * Set the animation for this sprite.
		 * @param animation
		 */
		void SetAnimation(shared_ptr<SpriteAnimation> animation);
		
		/**
		 * Task override that processes the first frame when the task starts.
		 * @return true.
		 */
		bool OnStart() override;
		
		/**
		 * Task override that makes sure the animation is up to date upon resumes.
		 */
		void OnResume(bool applicationResume = false) override;
		
		/**
		 * Task override that keeps an internal count of the current frame time and changes the frame as appropriate.
		 */
		void Update(Seconds lastFrameTime) override;
		
		/**
		 * Overrides SceneEntity::Render() to allow the Sprite to apply an extra transform to 
		 * @param transform
		 * @param renderTarget
		 */
		virtual void Render(RenderContext& renderContext, Colour compoundDiffuse) override;

		/**
		 * Set the size of the sprite in world units.
		 * @see SetPixelToWorldUnitRatio() for changing the pixel to world unit ratio.
		 * @expects the mesh to contain 4 vertices.
		 */
		void SetSize(f32 width, f32 height);

		/**
		 * Set the size of the sprite in pixels.
		 * @see SetPixelToWorldUnitRatio() for changing the pixel to world unit ratio.
		 * @expects the mesh to contain 4 vertices.
		 */
		void SetSizeInPixels(f32 width, f32 height);
		
		/**
		 * Specify that the Sprite should use the frame size from the animation.
		 * The default is to use the animation size (true), this will either be the frame size or if the frame contains
		 * a zero dimension the animation size for that dimension will be used instead. If this flag is set to false
		 * then the current size will remain the size of the sprite unless SetSizeInPixels() is called. This might sound
		 * a little confusing but it gives the option to use animation sizes or set the size manually
		 * @param useAnimationSize
		 */
		void SetUseAnimationSize(bool useAnimationSize);
		
		/**
		 * Sets the number of pixels that corresponds to a world unit.
		 * @param ratio pixel to world units ratio, e.g. 100 would mean 100 pixels to a single world unit.
		 */
		void SetPixelToWorldUnitRatio(f32 ratio);
		
		/**
		 * Set the animation mode.
		 * The default is SpriteAnimation::Modes::NOT_SET which uses the Mode set in the SpriteAnimation.
		 * @param mode
		 */
		void SetAnimationMode(SpriteAnimation::Mode mode){mAnimationMode=mode;}
		
		/**
		 * Flip the texture on the quad horizontally.
		 * This is accomplished by flipping the texture coordinates of frames.
		 * @param flippedHorizontally
		 */
		void SetFlippedHorizontally(bool flippedHorizontally);

		/**
		 * Flip the texture on the quad vertically.
		 * This is accomplished by flipping the texture coordinates of frames.
		 * @param flippedHorizontally
		 */
		void SetFlippedVertically(bool flippedVertically);
		
		/**
		 * Get whether the texture is flipped horizontally.
		 * @return 
		 */
		bool GetFlippedHorizontally() const {return mFlippedHorizontally;}

		/**
		 * Get whether the texture is flipped vertically.
		 * @return 
		 */
		bool GetFlippedVertically() const {return mFlippedVertically;}

		/**
		 * Set the size of the sprite.
		 * @expects the mesh to contain 4 texture coordinates.
		 */
		void SetTextureCoordinates(const TextureUV& uv1, const TextureUV& uv2);
	private:
		void CreateUnitQuad(void);
		
		/**
		 * Processes the current frame.
		 * @expects the mAnimation to be valid and to contain at least one frame, and mCurrentFrameIndex points to a valid frame.
		 */
		void ProcessFrame();
		
		/**
		 * Perform end of animation cycle actions.
		 * @expects mCurrentFrameIndex and mAnimation to be valid.
		 */
		void ProcessAnimationEnd();
		
		/**
		 * Actual implementation of setting size in pixels.
		 */
		void _SetSizeInPixels(f32 width, f32 height);

		bool mAlwaysFaceCamera;	//!< true for this sprite to always face the camera, false otherwise.
		bool mFrameHasTransform;
		bool mFlippedHorizontally;
		bool mFlippedVertically;
		bool mUseAnimationSize;
		shared_ptr<SpriteAnimation> mAnimation;
		Seconds mCurrentFrameTime;
		size_t mCurrentFrameIndex;
		f32 mPixelToWorldUnitRatio;
		Vector2 mCachedSizeInPixels;
		Node mNode;
		SpriteAnimation::Mode mAnimationMode;
	};
}

#endif
