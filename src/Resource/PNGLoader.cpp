#include <echo/Resource/PNGLoader.h>
#include <echo/FileSystem/File.h>
#include <iostream>
#include <bitset>

//#define PNGLOADER_DEBUG_OUTPUT

#define PNG_SETJMP_NOT_SUPPORTED
#include <png.h>

// png.h should #define the libpng version numbers. Test for oldest supported version:
BOOST_STATIC_ASSERT(PNG_LIBPNG_VER_MAJOR == 1);
BOOST_STATIC_ASSERT(PNG_LIBPNG_VER_MINOR >= 2);

namespace Echo
{
	class PNGLoader::Implementation
	{
	public:
		Implementation()
			:
			mWidth(0),
			mHeight(0),
			mBytesPerPixel(),
			mFormat(Texture::Formats::UNKNOWN),
			mCurrentRow(),
			mRowPointers(),
			mPNGPtr(),
			mInfoPtr(),
			mFile(nullptr)
		{}
		
		u32 mWidth;
		u32 mHeight;
		u32 mBytesPerPixel;
		Texture::Format mFormat;
		u32 mCurrentRow;
		png_bytepp mRowPointers;
		png_structp mPNGPtr;
		png_infop mInfoPtr;
		File* mFile;
	};

	png_voidp PngAlloc(png_structp p, png_size_t s)
	{
		png_voidp d = (png_voidp) (new u8[s]);
		return d;
	}

	void PngFree(png_structp p, png_voidp s)
	{
		u8* u8p = reinterpret_cast<u8*> (s);
		delete [] u8p;
	}

	void PngRead(png_structp png_ptr, png_bytep data, png_size_t length)
	{
		png_voidp read_io_ptr = png_get_io_ptr(png_ptr);
		if(read_io_ptr)
		{
			File& file = *reinterpret_cast<File*> (read_io_ptr);
			file.Read(data, 1, (u32) length);
		}
	}
	
	PNGLoader::PNGLoader()
		:
		mImplementation(new PNGLoader::Implementation())
	{}
	
	PNGLoader::~PNGLoader()
	{}
		
	bool PNGLoader::ProcessFile(File& textureFile)
	{
		mImplementation->mFile = nullptr;
		mImplementation->mCurrentRow = 0;
		
		if(!textureFile.IsOpen())
		{
			ECHO_LOG_ERROR("Error input file is not open: " << textureFile.GetActualFileName());
			return false;
		}
		
		u8 header[8];
		
		//Read first 8 bytes - if we couldn't then that really sucks
		if(textureFile.Read((void*) (&header), 1, 8) < 8)
		{
			ECHO_LOG_ERROR("Failed to read in first 8 bytes: " << textureFile.GetActualFileName());
			return 0;
		}
		
		//Check the file is a PNG
		const bool is_png = !png_sig_cmp(header, 0, 8);
		if (!is_png)
		{
			ECHO_LOG_ERROR("File is not a PNG file: " << textureFile.GetActualFileName());
			return 0;
		}
		
		//Allocate memory for the png stuct and the info struct
		mImplementation->mPNGPtr = png_create_read_struct_2(PNG_LIBPNG_VER_STRING, 0, 0, 0, 0, PngAlloc, PngFree);
		if (!mImplementation->mPNGPtr)
		{
			ECHO_LOG_ERROR("png_create_read_struct: " << textureFile.GetActualFileName());
			return 0;
		}

		mImplementation->mInfoPtr = png_create_info_struct(mImplementation->mPNGPtr);
		if (!mImplementation->mInfoPtr)
		{
			png_destroy_read_struct(&mImplementation->mPNGPtr, (png_infopp) 0, (png_infopp) 0);
			ECHO_LOG_ERROR("png_create_info_struct:1: " << textureFile.GetActualFileName());
			return 0;
		}

		mImplementation->mFile = &textureFile;

		png_set_read_fn(mImplementation->mPNGPtr, mImplementation->mFile, PngRead);
		png_set_sig_bytes(mImplementation->mPNGPtr, 8);

		int transforms = PNG_TRANSFORM_IDENTITY;

		// PNG files store 16-bit pixels in network byte order (big-endian, ie most significant bytes first). png_set_swap()
		// shall switch the byte-order to little-endian (ie, least significant bits first).
		#ifdef ECHO_LITTLE_ENDIAN
		// NOTE: This only applies to 16 bit colour channels
		// Also NOTE: png_set_swap does not appear to work for the png_read_png() interface
		transforms = PNG_TRANSFORM_SWAP_ENDIAN;
		#endif

		png_read_png(mImplementation->mPNGPtr, mImplementation->mInfoPtr, transforms, NULL);
		
		png_byte colourType = png_get_color_type(mImplementation->mPNGPtr, mImplementation->mInfoPtr);
		if (colourType == PNG_COLOR_TYPE_RGB_ALPHA)
		{
			png_set_swap_alpha(mImplementation->mPNGPtr);
		}
		
		mImplementation->mRowPointers = png_get_rows(mImplementation->mPNGPtr, mImplementation->mInfoPtr);
		mImplementation->mWidth = png_get_image_width(mImplementation->mPNGPtr, mImplementation->mInfoPtr);
		mImplementation->mHeight = png_get_image_height(mImplementation->mPNGPtr, mImplementation->mInfoPtr);
		
		int channels = png_get_channels(mImplementation->mPNGPtr, mImplementation->mInfoPtr);
		
		// PNG only supports 8 bit and 16bit channels. There is a chunk called sBIT which
		// can define which bits you consider significant. These can be used to shift the
		// bits to make them relevant. libpng will do this for you if you enable it.
		// I created a R4G4B4A4 image and ImageMagick was able to detect 4bits per channel
		// however when I load the image here I cannot work out a way to find this out.
		// I've inspected all of the info structure.
		// To test the loading of the R4G4B4A4 file I forced the bitDepth to 8. This doesn't
		// perform the shifts automatically for you but the loader code here will deal with
		// the 8 bit to 4 bit conversion.
		u32 bitDepth = png_get_bit_depth(mImplementation->mPNGPtr, mImplementation->mInfoPtr);
		mImplementation->mBytesPerPixel = channels * (bitDepth/8);
		
		if(bitDepth == 8)
		{
			if(colourType == PNG_COLOR_TYPE_RGB_ALPHA)
			{
				mImplementation->mFormat = Texture::Formats::R8G8B8A8;
			}else
			if(colourType == PNG_COLOR_TYPE_RGB)
			{
				if(mImplementation->mBytesPerPixel==3)
				{
					mImplementation->mFormat = Texture::Formats::R8G8B8;	
				}else
				{
					mImplementation->mFormat = Texture::Formats::R8G8B8X8;
				}
			}else
			if(colourType==PNG_COLOR_TYPE_GRAY)
			{
				mImplementation->mFormat = Texture::Formats::LUMINANCE8;
			}else
			{
				ECHO_LOG_ERROR("Unsupported PNG Format. BPP: " << bitDepth << " Colour Type: " << (int)colourType);
				return false;
			}
		}else
		if(bitDepth == 16 && colourType==PNG_COLOR_TYPE_GRAY)
		{
			mImplementation->mFormat = Texture::Formats::GREYSCALE16;
		}else
		if(bitDepth == 4 && (colourType == PNG_COLOR_TYPE_RGB_ALPHA))
		{
			mImplementation->mFormat = Texture::Formats::R4G4B4A4;
		}else
		{
			//PNG_COLOR_TYPE_GRAY_ALPHA
			//PNG_COLOR_TYPE_GRAY
			//PNG_COLOR_TYPE_PALETTE
			ECHO_LOG_ERROR("Unsupported PNG Format. BPP: " << bitDepth << " Colour Type: " << (int)colourType);
			return false;
		}
#ifdef PNGLOADER_DEBUG_OUTPUT
		png_infop info = mImplementation->mInfoPtr;
		ECHO_LOG_DEBUG("\twidth: " << info->width);
		ECHO_LOG_DEBUG("\theight: " << info->height);
		ECHO_LOG_DEBUG("\tvalid: " << info->valid);
		ECHO_LOG_DEBUG("\trowbytes: " << info->rowbytes);
		ECHO_LOG_DEBUG("\tnum_palette: " << info->num_palette);
		ECHO_LOG_DEBUG("\tnum_trans: " << info->num_trans);
		ECHO_LOG_DEBUG("\tbit_depth: " << (int)info->bit_depth);
		ECHO_LOG_DEBUG("\tcolor_type: " << (int)info->color_type);
		ECHO_LOG_DEBUG("\tcompression_type: " << (int)info->compression_type);
		ECHO_LOG_DEBUG("\tfilter_type: " << (int)info->filter_type);
		ECHO_LOG_DEBUG("\tinterlace_type: " << (int)info->interlace_type);
		ECHO_LOG_DEBUG("\tchannels: " << (int)info->channels);
		ECHO_LOG_DEBUG("\tpixel_depth: " << (int)info->pixel_depth);
		ECHO_LOG_DEBUG("\tspare_byte: " << (int)info->spare_byte);
		ECHO_LOG_DEBUG("\tsig_bit R:" << (int)info->sig_bit.red);
		ECHO_LOG_DEBUG("\tsig_bit G:" << (int)info->sig_bit.green);
		ECHO_LOG_DEBUG("\tsig_bit B:" << (int)info->sig_bit.blue);
		ECHO_LOG_DEBUG("\tsig_bit A:" << (int)info->sig_bit.alpha);
		ECHO_LOG_DEBUG("\tFormat:" << mImplementation->mFormat);
#endif
		return true;
	}
	
	std::vector<std::string> PNGLoader::GetFileExtensions() const
	{
		return {"png"};
	}
	
	u32 PNGLoader::GetWidth() const
	{
		return mImplementation->mWidth;
	}
	
	u32 PNGLoader::GetHeight() const
	{
		return mImplementation->mHeight;
	}
	
	Texture::Format PNGLoader::GetFormat() const
	{
		return mImplementation->mFormat;
	}
	
	bool PNGLoader::GetLoadInverted() const
	{
		return false;
	}
	
	void PNGLoader::ReadLine(u8* lineStart, u32 maxLength)
	{
		if (!mImplementation->mFile || !lineStart)
		{
			return;
		}
		
		if (mImplementation->mFormat == Texture::Formats::R4G4B4A4)
		{
			u32 widthBytes = std::min(mImplementation->mWidth * mImplementation->mBytesPerPixel, maxLength);
			for(u32 c = 0, di=0; di < widthBytes; c+=4,di+=2)
			{
				//Convert to 4bits per channel
#ifdef ECHO_BIG_ENDIAN
				// We have to deal with big endian byte difference here because File can't do it automatically
				// since PNG files are compressed. Byte order
				lineStart[di] = (mImplementation->mRowPointers[mImplementation->mCurrentRow][c]&0xF0) | (mImplementation->mRowPointers[mImplementation->mCurrentRow][c+1]>>4);
				lineStart[di+1] = (mImplementation->mRowPointers[mImplementation->mCurrentRow][c+2]&0xF0) | (mImplementation->mRowPointers[mImplementation->mCurrentRow][c+3]>>4);
#else
				lineStart[di] = (mImplementation->mRowPointers[mImplementation->mCurrentRow][c+2]&0xF0) | (mImplementation->mRowPointers[mImplementation->mCurrentRow][c+3]>>4);
				lineStart[di+1] = (mImplementation->mRowPointers[mImplementation->mCurrentRow][c]&0xF0) | (mImplementation->mRowPointers[mImplementation->mCurrentRow][c+1]>>4);
#endif
			}
		}else
		{
			//It is just an 8bit RGB or 8bit RGBA - or 16 bit and has been transformed by the library.
			u32 widthBytes = std::min(mImplementation->mWidth * mImplementation->mBytesPerPixel, maxLength);
			for(u32 c = 0; c < widthBytes; ++c)
			{
				lineStart[c] = mImplementation->mRowPointers[mImplementation->mCurrentRow][c];
			}
		}
		mImplementation->mCurrentRow++;
	}
	
	void PNGLoader::CleanUp()
	{
		png_destroy_read_struct(&mImplementation->mPNGPtr, &mImplementation->mInfoPtr, (png_infopp) 0);
	}
	
	void PngWriteStream(png_structp png_ptr, png_bytep data, png_size_t length)
	{
		png_voidp ioPointer = png_get_io_ptr(png_ptr);
		if(ioPointer)
		{
			std::ostream* stream = reinterpret_cast<std::ostream*>(ioPointer);
			stream->write(reinterpret_cast<const char*>(data),length);
		}
	}
	
	void PngFlushStream(png_structp png_ptr)
	{
		png_voidp ioPointer = png_get_io_ptr(png_ptr);
		if(ioPointer)
		{
			std::ostream* stream = reinterpret_cast<std::ostream*>(ioPointer);
			stream->flush();
		}
	}
	
	bool PNGLoader::WritePNG(const Texture& texture, std::ostream& stream)
	{
		png_structp png;
		png_infop info;

		png_uint_32 width = texture.GetWidth();
		png_uint_32 height = texture.GetHeight();
		png_uint_32 bytesPerPixel = texture.GetBytesPerPixel();
		
		png = png_create_write_struct_2(PNG_LIBPNG_VER_STRING, 0, 0, 0, 0, PngAlloc, PngFree);

		info = png_create_info_struct(png);
		if (info == NULL)
		{
			ECHO_LOG_ERROR("Failed to create PNG info structure");
			png_destroy_write_struct(&png,  NULL);
			return false;
		}

		png_uint_32 bitDepth;
		int colourType = -1;
		switch(texture.GetFormat())
		{
			case Texture::Formats::R5G5B5A1:
			case Texture::Formats::RGBA_F32:
			case Texture::Formats::LUMINANCE32_ALPHA32:
			case Texture::Formats::RGB_F32:
			case Texture::Formats::R5G6B5:
			case Texture::Formats::LUMINANCE32:
			case Texture::Formats::UNKNOWN:
				// Not supported yet
			break;
			case Texture::Formats::R4G4B4A4:
				colourType = PNG_COLOR_TYPE_RGB_ALPHA;
				bitDepth = 4;
			break;
			case Texture::Formats::R8G8B8A8:
			case Texture::Formats::B8G8R8A8:
				colourType = PNG_COLOR_TYPE_RGB_ALPHA;
				bitDepth = 8;
			break;
			case Texture::Formats::R8G8B8X8:
			case Texture::Formats::R8G8B8:
			case Texture::Formats::B8G8R8:
				colourType = PNG_COLOR_TYPE_RGB;
				bitDepth = 8;
			break;
			case Texture::Formats::LUMINANCE8_ALPHA8:
				colourType = PNG_COLOR_TYPE_GRAY_ALPHA;
				bitDepth = 8;
			break;
			case Texture::Formats::LUMINANCE8:
			case Texture::Formats::GREYSCALE8:
				bitDepth = 8;
				colourType = PNG_COLOR_TYPE_GRAY;
			break;
			case Texture::Formats::GREYSCALE16:
			case Texture::Formats::DEPTH16:
				bitDepth = 16;
				colourType = PNG_COLOR_TYPE_GRAY;
			break;
			// PNG_COLOR_TYPE_PALETTE isn't supported.
		}
		if(colourType==-1)
		{
			ECHO_LOG_ERROR("Failed to determine best colour type for texture format " << texture.GetFormat());
			png_destroy_write_struct(&png,  NULL);
			return false;
		}
		png_set_write_fn(png, (void*)&stream, PngWriteStream, PngFlushStream);
		png_set_IHDR(png, info, width, height, bitDepth, colourType, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

		/* Other optional chunks like cHRM, bKGD, tRNS, tIME, oFFs, pHYs */

		/* Note that if sRGB is present the gAMA and cHRM chunks must be ignored
		 * on read and, if your application chooses to write them, they must
		 * be written in accordance with the sRGB profile
		 */

		/* Write the file header information.  REQUIRED */
		png_write_info(png, info);

		/* Set up the transformations you want.  Note that these are
		 * all optional.  Only call them if you want them.
		 */
		
		// PNG files store 16-bit pixels in network byte order (big-endian, ie most significant bytes first). png_set_swap()
		// shall switch the byte-order to little-endian (ie, least significant bits first).
		#ifdef ECHO_LITTLE_ENDIAN
		if(bitDepth == 16)
		{
			png_set_swap(png);
		}
		#endif

		/* Invert monochrome pixels */
		//png_set_invert_mono(png_ptr);

		/* Shift the pixels up to a legal bit depth and fill in
		 * as appropriate to correctly scale the image.
		 */
		//png_set_shift(png_ptr, &sig_bit);

		/* Pack pixels into bytes */
		//png_set_packing(png_ptr);

		/* Swap location of alpha bytes from ARGB to RGBA */
		//png_set_swap_alpha(png);

		/* Get rid of filler (OR ALPHA) bytes, pack XRGB/RGBX/ARGB/RGBA into
		 * RGB (4 channels -> 3 channels). The second parameter is not used.
		 */
		if(texture.GetFormat()==Texture::Formats::R8G8B8X8)
		{
			png_set_filler(png, 0, PNG_FILLER_AFTER);
		}

		// Flip BGR pixels to RGB
		if(texture.GetFormat()==Texture::Formats::B8G8R8 || texture.GetFormat()==Texture::Formats::B8G8R8A8)
		{
			png_set_bgr(png);
		}

		/* Swap bytes of 16-bit files to most significant byte first */
		//png_set_swap(png);

		/* Swap bits of 1, 2, 4 bit packed pixel formats */
		//png_set_packswap(png);

		/* In this example, "image" is a one-dimensional array of bytes */
		png_bytep image = reinterpret_cast<png_bytep>(texture.GetBuffer().get());
		png_bytep row_pointers[height];

		if (height > PNG_UINT_32_MAX/(sizeof (png_bytep)))
		{
			ECHO_LOG_ERROR("Image is too tall to process in memory");
			return false;
		}

		/* Set up pointers into your "image" byte array */
		for (Size i = 0; i < height; ++i)
		{
			row_pointers[i] = image + i*width*bytesPerPixel;
		}

		png_write_image(png, row_pointers);
		png_write_end(png, info);
		png_destroy_write_struct(&png, &info);
		return true;
	}

}
