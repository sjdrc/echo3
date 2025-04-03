#include <echo/Graphics/Texture.h>
#include <echo/Resource/TextureManager.h>
#include <echo/Util/Utils.h>

namespace Echo
{
	Texture::Texture(u32 width, u32 height, Format format) : Resource<Texture>(true),
		mTextureManager(0),
		mFormat(format),
		mWidth(width),
		mHeight(height),
		mBufferSize(0),
		mBufferOption(BufferOptions::NOT_SET)
	{
		(void)SetupBuffer();	//The error message is output in the call.
	}
	
	Texture::Texture(u8* buffer, u32 width, u32 height, Format format, BufferOption bufferOption) : Resource<Texture>(true),
		mTextureManager(0),
		mFormat(format),
		mWidth(width),
		mHeight(height),
		mBufferSize(0),
		mBufferOption(bufferOption)
	{
		switch(mBufferOption)
		{
			case BufferOptions::TAKE_OWNERSHIP:
				mBuffer = shared_ptr<u8>(buffer, ArrayDeleter<u8>());
				// This is a bit of an assumption, but maintains the same behaviour before mBufferSize was introduced
				mBufferSize = mWidth*mHeight*GetBytesPerPixel();
			break;
			case BufferOptions::COPY:
				if(SetupBuffer())
				{
					Size numberOfBytes = GetDataSize();

					u8* destBuffer = mBuffer.get();
					// Copy the data
					for(Size i =0 ; i < numberOfBytes; ++i)
					{
						destBuffer[i] = buffer[i];
					}
				}
			break;
			case BufferOptions::WRAP:
				// Fake delete
				mBuffer = shared_ptr<u8>(buffer, [](u8 const* p){});
				mBufferSize = mWidth*mHeight*GetBytesPerPixel();
			break;
			case BufferOptions::NOT_SET:
			default:
				assert(false && "Unknown buffer handling option");
				ECHO_LOG_ERROR("Unknown buffer handling option: " << bufferOption);
			break;
		}
	}
	
	Texture::Texture(shared_ptr<u8> buffer, u32 width, u32 height, Format format) : Resource<Texture>(true),
		mTextureManager(0),
		mFormat(format),
		mWidth(width),
		mHeight(height),
		mBuffer(buffer),
		mBufferSize(0),
		mBufferOption(BufferOptions::NOT_SET)
	{
		// This is a bit of an assumption, but maintains the same behaviour before mBufferSize was introduced
		mBufferSize = mWidth*mHeight*GetBytesPerPixel();
	}
	
	Texture::Texture(Size bufferSize, shared_ptr<u8> buffer, u32 width, u32 height) : Resource<Texture>(true),
		mTextureManager(0),
		mFormat(Formats::UNKNOWN),
		mWidth(width),
		mHeight(height),
		mBuffer(buffer),
		mBufferSize(bufferSize),
		mBufferOption(BufferOptions::NOT_SET)
	{
	}

	Texture::Texture(Size bufferSize, u8* buffer, u32 width, u32 height, BufferOption bufferOption) : Resource<Texture>(true),
		mTextureManager(0),
		mFormat(Formats::UNKNOWN),
		mWidth(width),
		mHeight(height),
		mBufferSize(bufferSize),
		mBufferOption(bufferOption)
	{
		switch(mBufferOption)
		{
			case BufferOptions::TAKE_OWNERSHIP:
				mBuffer = shared_ptr<u8>(buffer, ArrayDeleter<u8>());
			break;
			case BufferOptions::COPY:
			{
				mBuffer = shared_ptr<u8>(new u8[bufferSize], ArrayDeleter<u8>());
				u8* destBuffer = mBuffer.get();
				// Copy the data
				for(Size i =0 ; i < bufferSize; ++i)
				{
					destBuffer[i] = buffer[i];
				}
			}
			break;
			case BufferOptions::WRAP:
				// Fake delete
				mBuffer = shared_ptr<u8>(buffer, [](u8 const* p){});
			break;
			case BufferOptions::NOT_SET:
			default:
				assert(false && "Unknown buffer handling option");
				ECHO_LOG_ERROR("Unknown buffer handling option: " << bufferOption);
			break;
		}
	}
	
	Texture::Texture() : Resource<Texture>(true),
		mTextureManager(nullptr),
		mFormat(Formats::UNKNOWN),
		mWidth(0),
		mHeight(0),
		mBufferSize(0),
		mBufferOption(BufferOptions::NOT_SET)
	{
	}

	Texture::Texture(TextureManager& manager, const std::string& resourceName) : Resource<Texture>(false),
		mFormat(Formats::UNKNOWN),
		mWidth(0),
		mHeight(0),
		mBufferSize(0),
		mBufferOption(BufferOptions::NOT_SET)
	{
		mTextureManager = &manager;
		SetName(resourceName);
		Load();
	}

	Size Texture::GetBytesPerPixel() const
	{
		return Formats::GetBytesPerPixel(mFormat);
	}
	
	Size Texture::GetDataSize() const
	{
		return mBufferSize;
	}
	
	Texture::~Texture()
	{
	}

	Size Texture::OnRequestMemoryRelease()
	{
		if(!mTextureManager || GetName().empty() || mBufferOption==BufferOptions::WRAP)
		{
			return 0;
		}
		
		if(mBuffer.unique())
		{
			mBuffer.reset();
			Size bytesFreed = mBufferSize;
			mBufferSize = 0;
			return bytesFreed;
		}

		//Another reference to the buffer is held somewhere so no memory was actually released.
		return 0;
	}
	
	bool Texture::_Unload()
	{
		if(!mTextureManager || GetName().empty() || mBufferOption==BufferOptions::WRAP)
		{
			return false;
		}
		
		mBuffer.reset();
		mBufferSize = 0;
		return true;
	}
	
	Texture::Texture(const Texture& rhs) : Resource<Texture>(rhs.IsLoaded()), mTextureManager(rhs.mTextureManager)
	{
		mWidth = rhs.mWidth;
		mHeight = rhs.mHeight;
		mBuffer = rhs.mBuffer;
		mBufferOption = rhs.mBufferOption;
		mBufferSize = rhs.mBufferSize;
		mFormat = rhs.mFormat;
		IncrementVersion();
	}

	Texture& Texture::operator=(const Texture& rhs)
	{
		if (this == &rhs)
			return *this;
		
		Unload();
		mTextureManager = rhs.mTextureManager;
		mWidth = rhs.mWidth;
		mHeight = rhs.mHeight;
		mBuffer = rhs.mBuffer;
		mBufferOption = rhs.mBufferOption;
		mBufferSize = rhs.mBufferSize;
		mFormat = rhs.mFormat;
		IncrementVersion();
		return *this;
	}

	bool Texture::HasAlpha() const
	{
		return Texture::Formats::HasAlpha(mFormat);
 	}
	
	Size Texture::GetNumberOfChannels() const
	{
		return Texture::Formats::GetNumberOfChannels(mFormat);
	}
	
	bool Texture::SetupBuffer()
	{
		size_t bufferSize = mWidth*mHeight*GetBytesPerPixel();
		if(bufferSize==0)
		{
			assert(bufferSize && "Texture::SetupBuffer() buffer size should not be 0. Make sure you have a width and height >1 and the format is valid.");
			ECHO_LOG_ERROR("Unable to allocate Texture buffer for \"" << GetName() << "\" with " << mWidth << "x" << mHeight << " and " << GetBytesPerPixel() << "bytes per pixel.");
			return false;
		}

		mBuffer = shared_ptr<u8>(new u8[bufferSize], ArrayDeleter<u8>());
		mBufferSize = bufferSize;
		return true;
	}
	
	shared_ptr<Texture> Texture::GetSubTexture(Vector2Generic<Size> pointA, Vector2Generic<Size> pointB)
	{
		Vector2Generic<Size> minimum(std::min(pointA.x, pointB.x), std::min(pointA.y, pointB.y));
		Vector2Generic<Size> maximum(std::max(pointA.x, pointB.x), std::max(pointA.y, pointB.y));
		
		// Can we extract it?
		if(!mBuffer || minimum.x > mWidth || minimum.y > mHeight || maximum.x >= mWidth || maximum.y >= mHeight)
		{
			// No!
			return nullptr;
		}
		
		Vector2Generic<Size> extractedSize = maximum - minimum;
		extractedSize.x+=1;	// The points are inclusive, so 0 -> 10 is 11 pixels.
		extractedSize.y+=1;
		shared_ptr<Texture> texture(new Texture(extractedSize.x, extractedSize.y,mFormat));
		u8* targetBuffer = texture->GetBuffer().get();
		u8* sourceBuffer = mBuffer.get();
		
		Size targetY = 0;
		const Size bytesPerPixel = GetBytesPerPixel();
		const Size stride = bytesPerPixel * mWidth;
		const Size targetStride = bytesPerPixel * extractedSize.x;
		const Size bytesXStart = minimum.x * bytesPerPixel;
		const Size bytesXEnd = (maximum.x+1) * bytesPerPixel;
		for(Size srcY = minimum.y; srcY <= maximum.y; ++srcY)
		{
			Size targetX = 0;
			Size yBase = srcY * stride;
			for(Size srcX = bytesXStart; srcX < bytesXEnd; ++srcX)
			{
				targetBuffer[targetX + targetY] = sourceBuffer[srcX + yBase];
				++targetX;
			}
			targetY+=targetStride;
		}
		return texture;
	}

	shared_ptr<Texture> Texture::Clone() const
	{
		shared_ptr<Texture> texture;
		// If the format is unknown we can't determine the size, so we use the static size value
		if(mFormat == Formats::UNKNOWN)
		{
			texture = make_shared<Texture>(mBufferSize, mBuffer.get(), mWidth, mHeight, BufferOptions::COPY);
		}else
		{
			texture = make_shared<Texture>(mBuffer.get(), mWidth, mHeight, mFormat, BufferOptions::COPY);
		}
		return texture;
	}
	
	std::ostream& operator<<(std::ostream& o, const Texture::Format& format)
	{
		switch(format)
		{
			case Texture::Formats::R8G8B8A8:
				o << "R8G8B8A8";
				break;
			case Texture::Formats::R8G8B8X8:
				o << "R8G8B8X8";
				break;
			case Texture::Formats::R8G8B8:
				o << "R8G8B8";
				break;
			case Texture::Formats::R5G6B5:
				o << "R5G6B5";
				break;
			case Texture::Formats::R5G5B5A1:
				o << "R5G5B5A1";
				break;
			case Texture::Formats::R4G4B4A4:
				o << "R4G4B4A4";
				break;
			case Texture::Formats::LUMINANCE8_ALPHA8:
				o << "LUMINANCE8_ALPHA8";
				break;
			case Texture::Formats::LUMINANCE8:
				o << "LUMINANCE8";
				break;
			case Texture::Formats::RGB_F32:
				o << "RGB_F32";
				break;
			case Texture::Formats::RGBA_F32:
				o << "RGBA_F32";
				break;
			case Texture::Formats::LUMINANCE32:
				o << "LUMINANCE32";
				break;
			case Texture::Formats::LUMINANCE32_ALPHA32:
				o << "LUMINANCE32_ALPHA32";
				break;
			case Texture::Formats::B8G8R8A8:
				o << "B8G8R8A8";
				break;
			case Texture::Formats::B8G8R8:
				o << "B8G8R8";
				break;
			case Texture::Formats::GREYSCALE8:
				o << "GREYSCALE8";
				break;
			case Texture::Formats::GREYSCALE16:
				o << "GREYSCALE16";
				break;
			case Texture::Formats::DEPTH16:
				o << "DEPTH16";
				break;
			case Texture::Formats::DEPTH24:
				o << "DEPTH24";
				break;
			case Texture::Formats::DEPTH32:
				o << "DEPTH32";
				break;
			case Texture::Formats::DEPTH24_STENCIL8:
				o << "DEPTH24_STENCIL8";
				break;
			case Texture::Formats::R10G10B10X2:
				o << "R10G10B10X2";
				break;
			case Texture::Formats::R10G10B10A2:
				o << "R10G10B10A2";
				break;
			case Texture::Formats::UNKNOWN:
			default:
				o << "UNKNOWN";
				break;
		}
		return o;
	}
	
	std::istream& operator>>(std::istream& i, Texture::Format& format)
	{
		// This is pretty crude, a map would be better so we can do a binary search.
		std::string temp;
		i >> temp;
		if(temp=="R8G8B8A8")
		{
			format = Texture::Formats::R8G8B8A8;
		}else
		if(temp=="R8G8B8X8")
		{
			format = Texture::Formats::R8G8B8X8;
		}else
		if(temp=="R8G8B8")
		{
			format = Texture::Formats::R8G8B8;
		}else
		if(temp=="R5G6B5")
		{
			format = Texture::Formats::R5G6B5;
		}else
		if(temp=="R5G5B5A1")
		{
			format = Texture::Formats::R5G5B5A1;
		}else
		if(temp=="R4G4B4A4")
		{
			format = Texture::Formats::R4G4B4A4;
		}else
		if(temp=="LUMINANCE8_ALPHA8")
		{
			format = Texture::Formats::LUMINANCE8_ALPHA8;
		}else
		if(temp=="LUMINANCE8")
		{
			format = Texture::Formats::LUMINANCE8;
		}else
		if(temp=="RGB_F32")
		{
			format = Texture::Formats::RGB_F32;
		}else
		if(temp=="RGBA_F32")
		{
			format = Texture::Formats::RGBA_F32;
		}else
		if(temp=="LUMINANCE32")
		{
			format = Texture::Formats::LUMINANCE32;
		}else
		if(temp=="LUMINANCE32_ALPHA32")
		{
			format = Texture::Formats::LUMINANCE32_ALPHA32;
		}else
		if(temp=="B8G8R8A8")
		{
			format = Texture::Formats::B8G8R8A8;
		}else
		if(temp=="B8G8R8")
		{
			format = Texture::Formats::B8G8R8;
		}else
		if(temp=="GREYSCALE8")
		{
			format = Texture::Formats::GREYSCALE8;
		}else
		if(temp=="GREYSCALE16")
		{
			format = Texture::Formats::GREYSCALE16;
		}else
		if(temp=="DEPTH16")
		{
			format = Texture::Formats::DEPTH16;
		}else
		if(temp=="DEPTH24")
		{
			format = Texture::Formats::DEPTH24;
		}else
		if(temp=="DEPTH32")
		{
			format = Texture::Formats::DEPTH32;
		}else
		if(temp=="DEPTH24_STENCIL8")
		{
			format = Texture::Formats::DEPTH24_STENCIL8;
		}else
		if(temp=="R10G10B10X2")
		{
			format = Texture::Formats::R10G10B10X2;
		}else
		if(temp=="R10G10B10A2")
		{
			format = Texture::Formats::R10G10B10A2;
		}else
		{
			format = Texture::Formats::UNKNOWN;
			i.setstate(std::ios_base::failbit);
		}
		return i;
	}
}
