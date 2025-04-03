#ifndef _ECHOTEXTURE_H_
#define _ECHOTEXTURE_H_

#include <echo/Resource/Resource.h>
#include <echo/Maths/Vector2.h>
#include <assert.h>
#include <string>

namespace Echo
{
	class TextureManager;
	
	//!\brief Texture encapsulates texture data.
	//!\details This class encapsulates texture data that is platform independent.
	//!			Normally a texture object is passed to a render target which will then allocate any resources
	//!			that the render target may need.
	class Texture : public Resource<Texture>
	{
	public:
		struct Formats
		{
			enum _
			{
				//If this enumeration changes make sure you update, GetBytesPerPixel(), GetNumberOfChannels() and HasAlpha().
				UNKNOWN,
				R8G8B8X8,
				R8G8B8A8,
				R8G8B8,
				R5G6B5,
				R5G5B5A1,
				R4G4B4A4,
				LUMINANCE8,
				LUMINANCE8_ALPHA8,
				RGB_F32,
				RGBA_F32,
				LUMINANCE32,
				LUMINANCE32_ALPHA32,
				B8G8R8,
				B8G8R8A8,
				GREYSCALE8,
				GREYSCALE16,
				DEPTH16,
				DEPTH24,
				DEPTH32,
				DEPTH24_STENCIL8,
				R10G10B10X2,
				R10G10B10A2
			};
			static size_t GetBytesPerPixel(Formats::_ format)
			{
				switch(format)
				{
					case Formats::R8G8B8A8:
					case Formats::R8G8B8X8:
					case Formats::B8G8R8A8:
						return 4;
					case Formats::R8G8B8:
					case Formats::B8G8R8:
						return 3;
					case Formats::R5G6B5:
					case Formats::R5G5B5A1:
					case Formats::R4G4B4A4:
					case Formats::LUMINANCE8_ALPHA8:
					case Formats::GREYSCALE16:
					case Formats::DEPTH16:
						return 2;
					case Formats::LUMINANCE8:
					case Formats::GREYSCALE8:
						return 1;
					case Formats::DEPTH24:
						return 3;
					case Formats::DEPTH32:
					case Formats::DEPTH24_STENCIL8:
					case Formats::R10G10B10X2:
					case Formats::R10G10B10A2:
						return 4;
					case Formats::RGB_F32:
						return sizeof(f32) * 3;
					case Formats::RGBA_F32:
						return sizeof(f32) * 4;
					case Formats::LUMINANCE32_ALPHA32:
						return sizeof(f32) * 2;
					case Formats::LUMINANCE32:
						return sizeof(f32);
					case Formats::UNKNOWN:
					default:
						break;
				}
				return 0;
			}
			static size_t GetNumberOfChannels(Formats::_ format)
			{
				switch(format)
				{
					case Formats::R8G8B8A8:
					case Formats::R8G8B8X8:
					case Formats::R5G5B5A1:
					case Formats::R4G4B4A4:
					case Formats::RGBA_F32:
					case Formats::B8G8R8A8:
					case Formats::R10G10B10X2:
					case Formats::R10G10B10A2:
						return 4;
					case Formats::R8G8B8:
					case Formats::R5G6B5:
					case Formats::RGB_F32:
					case Formats::B8G8R8:
						return 3;
					case Formats::LUMINANCE8_ALPHA8:
					case Formats::LUMINANCE32_ALPHA32:
					case Formats::DEPTH24_STENCIL8:
						return 2;
					case Formats::LUMINANCE8:
					case Formats::LUMINANCE32:
					case Formats::GREYSCALE8:
					case Formats::GREYSCALE16:
					case Formats::DEPTH16:
					case Formats::DEPTH24:
					case Formats::DEPTH32:
						return 1;
					case Formats::UNKNOWN:

					default:
						break;
				}
				return 0;
			}
			/**
			 * Determine if a format has an alpha channel
			 */
			static bool HasAlpha(Formats::_ format)
			{
				switch(format)
				{
					case Formats::R8G8B8A8:
					case Formats::R5G5B5A1:
					case Formats::R4G4B4A4:
					case Formats::RGBA_F32:
					case Formats::LUMINANCE8_ALPHA8:
					case Formats::LUMINANCE32_ALPHA32:
					case Formats::B8G8R8A8:
					case Formats::R10G10B10A2:
						return true;
					// Other formats do not have alpha
					default:
						break;
				}
				return false;
			}
		};
		typedef Formats::_ Format;

		/**
		 * Buffer operations determine how the buffer is used upon creation.
		 */
		struct BufferOptions
		{
			enum _
			{
				TAKE_OWNERSHIP,	// Texture will take ownership of the buffer, technically ownership of
								// the buffer is that of the shared pointer it is encapsulated in,
								// this option sets up the shared pointer to array delete the buffer.
				COPY,			// An internal buffer will be allocated and the data will be copied.
				WRAP,			// Texture wraps the buffer with a faux-deleter smart pointer.
								// @note if you wrap the buffer you need to ensure that the buffer
								// is around for the life of Texture.
				NOT_SET			// If no buffer option needed to be specified this will used internally
			};
		};
		typedef BufferOptions::_ BufferOption;
		
		/**
		 * Get an Accessor to the pixel buffer in the type specified.
		 * @note The only sanity check performed on the type is a comparison of the size of the specified type
		 * and the pixel size. You need to you make sure you specify the type that matches the pixel data
		 * format otherwise your buffer results may not be as you expect.
		 * @return An Accessor if the size is valid, otherwise a null Accessor.
		 */
		template<typename T>
		class Accessor
		{
		public:
			Accessor() : mBase(nullptr), mFormat(Formats::UNKNOWN), mBytesPerPixel(0), mWidth(0), mHeight(0){}
			Accessor(shared_ptr<u8> base, Format format, Size width, Size height) : mBase(base), mFormat(format), mWidth(width), mHeight(height)
			{
				mBytesPerPixel = Texture::Formats::GetBytesPerPixel(format);
			}
			Accessor(const Accessor<T>& rhs)
			{
				mBytesPerPixel = rhs.mBytesPerPixel;
				mBase = rhs.mBase;
				mFormat = rhs.mFormat;
				mWidth = rhs.mWidth;
				mHeight = rhs.mHeight;
			}
			Accessor<T>& operator=(const Accessor<T>& rhs)
			{
				mBytesPerPixel = rhs.mBytesPerPixel;
				mBase = rhs.mBase;
				mFormat = rhs.mFormat;
				mWidth = rhs.mWidth;
				mHeight = rhs.mHeight;
				return *this;
			}
			T& operator[](Size index)
			{
				return *reinterpret_cast<T*>(&(mBase.get()[index*mBytesPerPixel]));
			}
			
			const T& operator[](Size index) const
			{
				return *reinterpret_cast<T*>(&(mBase.get()[index*mBytesPerPixel]));
			}
			
			inline operator bool() const
			{
				return (mBase!=nullptr);
			}
			inline Format GetFormat() const {return mFormat;}
			inline Size GetWidth() const {return mWidth;}
			inline Size GetHeight() const {return mHeight;}
			inline Size GetSize() const {return mWidth*mHeight;}
		private:
			shared_ptr<u8> mBase;
			Format mFormat;
			Size mBytesPerPixel;
			Size mWidth;
			Size mHeight;
		};
		
		Texture();
		Texture(u32 width, u32 height, Format format);
		
		/**
		 * Create a texture from a buffer.
		 * @param buffer The buffer data. The size of this buffer must be:
		 *			width*height*Formats::GetBytesPerPixel(format)
		 * @param width The width of the texture.
		 * @param height The height of the texture.
		 * @param format The format of the texture.
		 * @paran bufferOption How to treat the buffer, @see BufferOptions.
		 */
		Texture(u8* buffer, u32 width, u32 height, Format format, BufferOption bufferOption = BufferOptions::COPY);

		/**
		 * Create a texture from a shared buffer.
		 * @param buffer The buffer data. The size of this buffer must be:
		 *			width*height*Formats::GetBytesPerPixel(format)
		 * @param width The width of the texture.
		 * @param height The height of the texture.
		 * @param format The format of the texture.
		 * @note It is expected that the buffer has a custom deleter to clean up the array properly. No additional
		 * management for this is performed since it is expected that the shared pointer be set up appropriately.
		 */
		Texture(shared_ptr<u8> buffer, u32 width, u32 height, Format format);

		/**
		 * Create a texture from a buffer with a custom data size.
		 * This sets the format to UNKNOWN and allows you to pass an arbitrarily sized buffer. Textures of this type
		 * typically won't be renderable but may be useful as a transport for compressed image data.
		 */
		Texture(Size bufferSize, u8* buffer, u32 width, u32 height, BufferOption bufferOption = BufferOptions::COPY);
		
		/**
		 * Create a texture from a shared buffer with a custom data size.
		 * This sets the format to UNKNOWN and allows you to pass an arbitrarily sized buffer. Textures of this type
		 * typically won't be renderable but may be useful as a transport for compressed image data.
		 */
		Texture(Size bufferSize, shared_ptr<u8> buffer, u32 width, u32 height);

		Texture(TextureManager& manager, const std::string& resourceName);
		virtual ~Texture();

		void SetTextureManager(TextureManager& textureManager){mTextureManager=&textureManager;}

		/**
		 * Copy constructor.
		 * This performs a shallow copy, the buffer is shared between the copies.
		 */
		Texture(const Texture& rhs);

		/**
		 * Shallow copy the texture.
		 * The buffer is shared between the copies.
		 * An operation that causes a reallocation of the buffer
		 */
		Texture& operator=(const Texture& rhs);

		//!\brief Get the width of the texture buffer.
		u32 GetWidth() const {return mWidth;}
		
		//!\brief Get the height of the texture buffer.
		u32 GetHeight() const {return mHeight;}

		//!\brief Get a pointer to the texture byte buffer.
		shared_ptr<u8> GetBuffer() const {return mBuffer;}
		
		/**
		 * Get an accessor to the buffer
		 */
		template<typename T>
		Accessor<T> GetAccessor()
		{
			if(!mBuffer)
			{
				return Accessor<T>();
			}
			if(GetBytesPerPixel()!=sizeof(T))
			{
				ECHO_LOG_ERROR("The specified type does not match the pixel size: " << GetBytesPerPixel() << "!=" << sizeof(T));
				return Accessor<T>();
			}
			return Accessor<T>(mBuffer, mFormat, GetWidth(), GetHeight());
		}

		template<typename T>
		const Accessor<T> GetAccessor() const
		{
			if(!mBuffer)
			{
				return Accessor<T>();
			}
			if(GetBytesPerPixel()!=sizeof(T))
			{
				ECHO_LOG_ERROR("The specified type does not match the pixel size: " << GetBytesPerPixel() << "!=" << sizeof(T));
				return Accessor<T>();
			}
			return Accessor<T>(mBuffer, mFormat, GetWidth(), GetHeight());
		}
		
		Format GetFormat() const {return mFormat;}
		
		/**
		 * Attempt to create a Texture from a portion of this Texture
		 * @note This method will use the min and max of each component of pointA and pointB to
		 * determine the bounds so you can specify min or max in any order for each component.
		 * @param pointA The first point in the texture, inclusive, typically the minimum point.
		 * @param pointB The second point in the texture, inclusive, typically the maximum point.
		 * @return A texture with a copy of the data from this texture if the points are within
		 * this Texture's bound. The format will be the same.
		 */
		shared_ptr<Texture> GetSubTexture(Vector2Generic<Size> pointA, Vector2Generic<Size> pointB);

		/**
		 * Attempts to free the texture data to save memory.
		 * This method will attempt to free the texture memory but it will first check with the delegate (any hence any
		 * delegate listeners) if the memory can safely be freed.
		 * The method will also fail to free memory if it does not have a valid TextureManager and resource name. Without
		 * these the resource will not be able to reload when next needed.
		 * @return The number of bytes freed.
		 */
		Size OnRequestMemoryRelease() override;
		
		//!\brief Get the number of bytes per pixel.
		Size GetBytesPerPixel() const;
		
		/**
		 * Get the number of bytes the data takes up.
		 * This is width*height*bytesPerPixel
		 */
		Size GetDataSize() const;
		
		/**
		 * Get whether this texture has an alpha component or not.
		 * @return true if the texture has an alpha component.
		 */
		bool HasAlpha() const;

		/**
		 * Get the numebr of channels this texture has.
		 */
		Size GetNumberOfChannels() const;
		
		/**
		 * Clones the texture.
		 * This performs a deep copy. The buffer contents are copied to a new buffer.
		 */
		shared_ptr<Texture> Clone() const;
	private:
		/**
		 * Frees the buffer if the texture will be loadable again.
		 */
		bool _Unload() override;

		TextureManager* mTextureManager;
		Format mFormat;
		u32 mWidth;
		u32 mHeight;
		shared_ptr<u8> mBuffer;
		Size mBufferSize;
		BufferOption mBufferOption;
		bool SetupBuffer();
		
		friend class TextureLoader;
	};
	
	std::ostream& operator<<(std::ostream& o, const Texture::Format& format);
	std::istream& operator>>(std::istream& i, Texture::Format& format);
}
#endif
