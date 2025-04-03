#ifndef SPRITEANIMATION_H
#define	SPRITEANIMATION_H

#include <echo/Chrono/Chrono.h>
#include <echo/Graphics/Texture.h>
#include <echo/Graphics/PrimitiveTypes.h>
#include <echo/Graphics/Node.h>
#include <echo/cpp/functional>
#include <map>

namespace Echo
{
	class Texture;
	class Sprite;
	
	class SpriteAnimation
	{
	public:
		typedef function<void(SpriteAnimation&, Sprite&, size_t) > FrameFunction;

		/**
		 * A frame has a number of properties as well as the texture to display.
		 */
		class Frame
		{
		public:
			Frame(std::string resource, Seconds seconds) :
				mFrameTime(seconds),
				mResource(resource),
				mOffset(Vector3::ZERO),
				mScale(Vector3::UNIT_SCALE),
				mOrientation(Quaternion::IDENTITY),
				mWidth(0.f),
				mHeight(0.f),
				mUV1(0.f,0.f),
				mUV2(1.f,1.f)
			{
			}
			Frame& frameTime(Seconds s){mFrameTime=s;return *this;}
			Frame& texture(shared_ptr<Texture> t){mTexture=t;return *this;}
			Frame& resource(const std::string& name) {mResource=name;return *this;}
			Frame& offset(Vector3 o)
			{
				mOffset=o;
				CheckTransform();
				return *this;
			}
			Frame& scale(f32 s)
			{
				return scale(Vector3(s,s,s));
			}
			Frame& scale(Vector3 s)
			{
				mScale=s;
				CheckTransform();
				return *this;
			}
			Frame& orientation(Quaternion o)
			{
				mOrientation=o;
				CheckTransform();
				return *this;
			}
			Frame& width(f32 w){mWidth=w;return *this;}
			Frame& height(f32 h){mHeight=h;return *this;}
			Frame& uv1(TextureUV uv){mUV1=uv;return *this;}
			Frame& uv2(TextureUV uv){mUV2=uv;return *this;}
			bool HasTransform() const {return mHasTransform;}
			
			const Seconds& frameTime() const {return mFrameTime;}
			shared_ptr<Texture> texture(){return mTexture;}
			const std::string& resource() const {return mResource;}
			const Vector3& offset() const {return mOffset;}
			const Vector3& scale() const {return mScale;}
			const Quaternion& orientation() const {return mOrientation;}
			f32 width() const {return mWidth;}
			f32 height() const {return mHeight;}
			const TextureUV& uv1() const {return mUV1;}
			const TextureUV& uv2() const {return mUV2;}
		private:
			void CheckTransform()
			{
				mHasTransform=false;
				if(mOffset!=Vector3::ZERO)
				{
					mHasTransform=true;
				}
				if(mScale!=Vector3::ZERO)
				{
					mHasTransform=true;
				}
				if(mOrientation!=Quaternion::IDENTITY)
				{
					mHasTransform=true;
				}
			}
					
			Seconds mFrameTime;
			shared_ptr<Texture> mTexture;
			std::string mResource;
			bool mHasTransform;
			Vector3 mOffset;
			Vector3 mScale;
			Quaternion mOrientation;
			f32 mWidth;
			f32 mHeight;
			TextureUV mUV1;
			TextureUV mUV2;
		};
		
		/**
		 * Animation modes.
		 */
		struct Modes
		{
			enum _
			{
				NOT_SET,					///Defaults to LOOP if not set, NOT_SET allows for overriding.
				LOOP,
				PLAY_ONCE,
				PLAY_ONCE_AND_HIDE,
				PLAY_ONCE_AND_RESTART,
				PLAY_ONCE_HIDE_AND_RESTART
			};
		};
		typedef Modes::_ Mode;
		
		SpriteAnimation(TextureManager& textureManager, const std::string& name);
		~SpriteAnimation();

		/**
		 * Add a frame of animation.
		 * @param imageName image file name or resource name.
		 * @param frameTimeInSeconds seconds this frame will be displayed.
		 * @param useTextureSize if true the texture will be pre-loaded (this overrides the last parameter), and the dimensions will be used as the frame size.
		 * @param preloadResource if true the texture will be pre-loaded.
		 * @return 
		 */
		Frame& AddFrame(const std::string& imageName, Seconds frameTimeInSeconds, bool useTextureSize = false, bool preloadResource = false);

		/**
		 * Add multiple frames to the animation using a sprite sheet image.
		 * @param imageName
		 * @param frameTimeInSeconds
		 * @param frameWidth can either be in the range of 0-1 to specify the texture coordinate size or size in pixels if >1
		 * @param frameHeight can either be in the range of 0-1 to specify the texture coordinate size or size in pixels if >1
		 */
		void AddFrames(const std::string& imageName, Seconds frameTimeInSeconds, f32 frameWidth, f32 frameHeight);


		/**
		 * Set the width of the animation in pixels.
		 * The size of the animation determines the size of a sprite if the sprite is using that animation.
		 */
		void SetWidthInPixels(f32 width);

		/**
		 * Set the height of the animation in pixels.
		 * The size of the animation determines the size of a sprite if the sprite is using that animation.
		 */
		void SetHeightInPixels(f32 height);
		
		/**
		 * Add a callback to trigger on a specific frame index.
		 * @param frameIndex base 0 frame index.
		 * @param frameFunction callback that will be triggered when the frame is displayed.
		 */
		void AddFrameFunction(size_t frameIndex, FrameFunction frameFunction);

		/**
		 * Add a callback to the end frame of the animation.
		 * The end frame may be used as a trigger for something but if you don't want to look up the number of frames the animation
		 * has you can use this method.
		 * Callbacks added with this method will always be triggered on the end frame, even if you change the frames in the animation.
		 * @param frameFunction callback that will be triggered when the end frame is displayed.
		 */
		void AddEndFunction(FrameFunction frameFunction);
		
		/**
		 * Clear all of the frames in the animation.
		 * This also clears all frame callbacks except end frame callbacks.
		 */
		void ClearFrames();
		
		/**
		 * Clear the end frame callbacks.
		 */
		void ClearEndCallbacks();
		
		/**
		 * Set the animation mode.
		 * @param mode the new animation mode.
		 */
		void SetMode(Mode mode);
		
		/**
		 * Get the animation mode.
		 * @return 
		 */
		Mode GetMode() const;
		
		/**
		 * Get the number of frames in the animation.
		 * @return 
		 */
		size_t GetNumberOfFrames() const;
	private:
		friend class Sprite;
		/**
		 * Internally used by Sprite to get the current frame.
		 * @param index
		 * @return the current frame.
		 */
		Frame& GetFrame(size_t index)
		{
			assert(index < mFrames.size());
			return mFrames[index];
		}
		TextureManager& mTextureManager;
		Mode mMode;
		std::vector<Frame> mFrames;
		std::map<size_t, std::vector<FrameFunction> > mFrameFunctions;
		std::vector<FrameFunction> mEndFunctions;
		f32 mWidthInPixels;
		f32 mHeightInPixels;
		
		void ProcessFrameCallbacks(Sprite& sprite, size_t frameIndex);
		void ProcessEndFrameCallbacks(Sprite& sprite);

		//Disabled
		SpriteAnimation(const SpriteAnimation& orig);

		shared_ptr<Texture> AcquireTexture(const std::string& resourceName);
	};
}
#endif	/* SPRITEANIMATION_H */
