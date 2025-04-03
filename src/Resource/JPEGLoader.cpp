#include <echo/Resource/JPEGLoader.h>
#include <echo/FileSystem/File.h>
#include <iostream>
#include <bitset>
#include <jpeglib.h>

namespace Echo
{
	struct ErrorManager
	{
		jpeg_error_mgr pub;		/* "public" fields */
		//jmp_buf setjmp_buffer;	/* for return to caller */
	};
	
	class JPEGLoader::Implementation
	{
	public:
		Implementation()
			:
			mWidth(0),
			mHeight(0),
			mBytesPerPixel(),
			mFormat(Texture::Formats::UNKNOWN)
		{}
		
		u32 mWidth;
		u32 mHeight;
		u32 mBytesPerPixel;
		Texture::Format mFormat;
		JSAMPARRAY mBuffer;					// Output row buffer
		jpeg_decompress_struct mDecompress;
		ErrorManager mErrorManager;
	};
	
	const size_t OUTPUT_BUFFER_SIZE=4096;
	const size_t INPUT_BUFFER_SIZE=4096;

	struct FileDestination
	{
		jpeg_destination_mgr pub; /* public fields */
		File* file;
		JOCTET* buffer;
	};

	void InitFileDestination(j_compress_ptr cinfo)
	{
	  FileDestination* dest = reinterpret_cast<FileDestination*>(cinfo->dest);

	  /* Allocate the output buffer --- it will be released when done with image */
	  dest->buffer = (JOCTET*)(*cinfo->mem->alloc_small) ((j_common_ptr)cinfo, JPOOL_IMAGE, OUTPUT_BUFFER_SIZE * sizeof(JOCTET));
	  dest->pub.next_output_byte = dest->buffer;
	  dest->pub.free_in_buffer = OUTPUT_BUFFER_SIZE;
	}

	boolean EmptyOutputBufferToFile(j_compress_ptr cinfo)
	{
		FileDestination* dest = reinterpret_cast<FileDestination*>(cinfo->dest);
		if(dest->file->Write(dest->buffer, OUTPUT_BUFFER_SIZE)!=OUTPUT_BUFFER_SIZE)
		{
			ECHO_LOG_ERROR("Error writing File");
			return FALSE;
		}
		dest->pub.next_output_byte = dest->buffer;
		dest->pub.free_in_buffer = OUTPUT_BUFFER_SIZE;
		return TRUE;
	}	

	void TerminateFileDestination(j_compress_ptr cinfo)
	{
		FileDestination* dest = reinterpret_cast<FileDestination*>(cinfo->dest);
		size_t datacount = OUTPUT_BUFFER_SIZE - dest->pub.free_in_buffer;

		/* Write any data remaining in the buffer */
		if (datacount > 0)
		{
			if(dest->file->Write(dest->buffer, datacount)!=datacount)
			{
				ECHO_LOG_ERROR("Error writing end stream buffer");
			}
		}
	}	

	bool SetupFileDestination(j_compress_ptr cinfo, File* file)
	{
		/* The destination object is made permanent so that multiple JPEG images
		 * can be written to the same file without re-executing jpeg_stdio_dest.
		 */
		if (!cinfo->dest)
		{
			/* first time for this JPEG object? */
			cinfo->dest = (struct jpeg_destination_mgr*)(*cinfo->mem->alloc_small)((j_common_ptr)cinfo, JPOOL_PERMANENT, sizeof(FileDestination));
		} else
		if (cinfo->dest->init_destination != InitFileDestination)
		{
			/* It is unsafe to reuse the existing destination manager unless it was
			 * created by this function.  Otherwise, there is no guarantee that the
			 * opaque structure is the right size.  Note that we could just create a
			 * new structure, but the old structure would not be freed until
			 * jpeg_destroy_compress() was called.
			 */
			ECHO_LOG_ERROR("j_compress points to a different output initialisation function. It is unsafe to reinitialise without first destroying the compressor.");
			return false;
		}

		FileDestination* dest = reinterpret_cast<FileDestination*>(cinfo->dest);
		dest->pub.init_destination = InitFileDestination;
		dest->pub.empty_output_buffer = EmptyOutputBufferToFile;
		dest->pub.term_destination = TerminateFileDestination;
		dest->file = file;
		return true;
	}
	
	struct FileSource
	{
	  jpeg_source_mgr pub;   /* public fields */
	  File* infile;				/* source stream */
	  JOCTET* buffer;			/* start of buffer */
	  bool startOfFile;		/* have we gotten any data yet? */
	};

	void InitFileSource(j_decompress_ptr cinfo)
	{
		FileSource* src = reinterpret_cast<FileSource*>(cinfo->src);

		/* We reset the empty-input-file flag for each image,
		 * but we don't clear the input buffer.
		 * This is correct behavior for reading a series of images from one source.
		 */
		src->startOfFile = true;
	}

	/*
	 * Fill the input buffer --- called whenever buffer is emptied.
	 *
	 * In typical applications, this should read fresh data into the buffer
	 * (ignoring the current state of next_input_byte & bytes_in_buffer),
	 * reset the pointer & count to the start of the buffer, and return TRUE
	 * indicating that the buffer has been reloaded.  It is not necessary to
	 * fill the buffer entirely, only to obtain at least one more byte.
	 *
	 * There is no such thing as an EOF return.  If the end of the file has been
	 * reached, the routine has a choice of ERREXIT() or inserting fake data into
	 * the buffer.  In most cases, generating a warning message and inserting a
	 * fake EOI marker is the best course of action --- this will allow the
	 * decompressor to output however much of the image is there.  However,
	 * the resulting error message is misleading if the real problem is an empty
	 * input file, so we handle that case specially.
	 */

	boolean FillFileInputBuffer(j_decompress_ptr cinfo)
	{
		FileSource* src = reinterpret_cast<FileSource*>(cinfo->src);
		
		size_t nbytes = src->infile->Read(src->buffer,INPUT_BUFFER_SIZE);

		if (nbytes==0)
		{
			if (src->startOfFile)     /* Treat empty input file as fatal error */
			{
				ECHO_LOG_ERROR("Cannot load JPEG. An empty file was provided");
				//ERREXIT(cinfo, JERR_INPUT_EMPTY);
			}
			//WARNMS(cinfo, JWRN_JPEG_EOF);
			/* Insert a fake EOI marker */
			src->buffer[0] = (JOCTET)0xFF;
			src->buffer[1] = (JOCTET)JPEG_EOI;
			nbytes = 2;
		}

		src->pub.next_input_byte = src->buffer;
		src->pub.bytes_in_buffer = nbytes;
		src->startOfFile = false;

		return TRUE;
	}
	
	/*
	 * Skip data --- used to skip over a potentially large amount of
	 * uninteresting data (such as an APPn marker).
	 */
	void SkipFileInputData(j_decompress_ptr cinfo, long num_bytes)
	{
		jpeg_source_mgr* src = cinfo->src;
		/* Just a dumb implementation for now.  Could use fseek() except
		 * it doesn't work on pipes.  Not clear that being smart is worth
		 * any trouble anyway --- large skips are infrequent.
		 */
		if (num_bytes > 0)
		{
			while (num_bytes > (long)src->bytes_in_buffer)
			{
			  num_bytes -= (long)src->bytes_in_buffer;
			  (void)(*src->fill_input_buffer) (cinfo);
			  /* note we assume that fill_input_buffer will never return FALSE,
			   * so suspension need not be handled.
			   */
			}
			src->next_input_byte += (size_t)num_bytes;
			src->bytes_in_buffer -= (size_t)num_bytes;
		}
	}

	/*
	 * Terminate source --- called by jpeg_finish_decompress
	 * after all data has been read.  Often a no-op.
	 *
	 * NB: *not* called by jpeg_abort or jpeg_destroy; surrounding
	 * application must deal with any cleanup that should happen even
	 * for error exit.
	 */

	void TerminateFileSource(j_decompress_ptr cinfo)
	{
		// no work necessary here
	}

	/*
	 * Prepare for input from a File.
	 * The caller must have already opened the file, and is responsible for closing it after finishing decompression.
	 */
	bool SetupFileSource(j_decompress_ptr cinfo, File* infile)
	{
		FileSource* src = reinterpret_cast<FileSource*>(cinfo->src);

		/* The source object and input buffer are made permanent so that a series
		 * of JPEG images can be read from the same file by calling jpeg_stdio_src
		 * only before the first one.  (If we discarded the buffer at the end of
		 * one image, we'd likely lose the start of the next one.)
		 */
		if (cinfo->src == NULL)	     /* first time for this JPEG object? */
		{
		  cinfo->src = (struct jpeg_source_mgr *)(*cinfo->mem->alloc_small) ((j_common_ptr)cinfo, JPOOL_PERMANENT, sizeof(FileSource));
		  src = reinterpret_cast<FileSource*>(cinfo->src);
		  src->buffer = (JOCTET *)(*cinfo->mem->alloc_small) ((j_common_ptr)cinfo, JPOOL_PERMANENT, INPUT_BUFFER_SIZE * sizeof(JOCTET));
		} else if (cinfo->src->init_source != InitFileSource)
		{
			/* It is unsafe to reuse the existing source manager unless it was created
			 * by this function.  Otherwise, there is no guarantee that the opaque
			 * structure is the right size.  Note that we could just create a new
			 * structure, but the old structure would not be freed until
			 * jpeg_destroy_decompress() was called.
			 */
			ECHO_LOG_ERROR("j_decompress points to a different input initialisation function. It is unsafe to reinitialise without first destroying the decompressor.");
			return false;
		}

		src = reinterpret_cast<FileSource*>(cinfo->src);
		src->pub.init_source = InitFileSource;
		src->pub.fill_input_buffer = FillFileInputBuffer;
		src->pub.skip_input_data = SkipFileInputData;
		src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */
		src->pub.term_source = TerminateFileSource;
		src->infile = infile;
		src->pub.bytes_in_buffer = 0;		/* forces fill_input_buffer on first read */
		src->pub.next_input_byte = NULL;	/* until buffer loaded */
		return true;
	}

	JPEGLoader::JPEGLoader() : mImplementation(new JPEGLoader::Implementation())
	{}
	
	JPEGLoader::~JPEGLoader()
	{}

	void JPEGErrorExit(j_common_ptr cinfo)
	{
		//ErrorManager* myerr = reinterpret_cast<ErrorManager*>(cinfo->err);

		/* Always display the message. */
		/* We could postpone this until after returning, if we chose. */
		ECHO_LOG_ERROR("Failed to process JPEG file");
		//(*cinfo->err->output_message) (cinfo);

		// Return control to the setjmp point
		//longjmp(myerr->setjmp_buffer, 1);
	}
	
	bool JPEGLoader::ProcessFile(File& textureFile)
	{
		if(!textureFile.IsOpen())
		{
			ECHO_LOG_ERROR("Error input file is not open: " << textureFile.GetActualFileName());
			return false;
		}

		// Step 1: allocate and initialize JPEG decompression object

		/* We set up the normal JPEG error routines, then override error_exit. */
		mImplementation->mDecompress.err = jpeg_std_error(&mImplementation->mErrorManager.pub);
		mImplementation->mErrorManager.pub.error_exit = JPEGErrorExit;
//		/* Establish the setjmp return context for my_error_exit to use. */
//		if (setjmp(jerr.setjmp_buffer))
//		{
//			return false;
//		}

		/* Now we can initialize the JPEG decompression object. */
		jpeg_create_decompress(&mImplementation->mDecompress);

		/* Step 2: specify data source (eg, a file) */
		if(!SetupFileSource(&mImplementation->mDecompress, &textureFile))
		{
			return false;
		}

		/* Step 3: read file parameters with jpeg_read_header() */
		(void)jpeg_read_header(&mImplementation->mDecompress, TRUE);
		/* We can ignore the return value from jpeg_read_header since
		 *   (a) suspension is not possible with the stdio data source, and
		 *   (b) we passed TRUE to reject a tables-only JPEG file as an error.
		 * See libjpeg.txt for more info.
		 */

		/* Step 4: set parameters for decompression */
		/* In this example, we don't need to change any of the defaults set by
		 * jpeg_read_header(), so we do nothing here.
		 */
		
		switch(mImplementation->mDecompress.out_color_space)
		{
			case JCS_RGB565:
				mImplementation->mFormat = Texture::Formats::R5G6B5;
			break;
			case JCS_EXT_RGBA:
				mImplementation->mFormat = Texture::Formats::R8G8B8A8;
			break;
			case JCS_EXT_BGRA:
				mImplementation->mFormat = Texture::Formats::B8G8R8A8;
			break;
			case JCS_EXT_RGBX:
				mImplementation->mFormat = Texture::Formats::R8G8B8X8;
			break;
			case JCS_RGB:
				mImplementation->mFormat = Texture::Formats::R8G8B8;
			break;
			case JCS_EXT_BGR:
				mImplementation->mFormat = Texture::Formats::B8G8R8;
			break;
			case JCS_GRAYSCALE:
				mImplementation->mFormat = Texture::Formats::GREYSCALE8;
			break;
			default:
				ECHO_LOG_ERROR("Unsupported colour format: " << mImplementation->mDecompress.out_color_space);
				return false;
		}		

		/* Step 5: Start decompressor */
		(void)jpeg_start_decompress(&mImplementation->mDecompress);
		/* We can ignore the return value since suspension is not possible
		 * with the stdio data source.
		 */
		
		/* We may need to do some setup of our own at this point before reading
		 * the data.  After jpeg_start_decompress() we have the correct scaled
		 * output image dimensions available, as well as the output colormap
		 * if we asked for color quantization.
		 * In this example, we need to make an output work buffer of the right size.
		 */
		/* JSAMPLEs per row in output buffer */
		mImplementation->mWidth = mImplementation->mDecompress.output_width;
		mImplementation->mHeight = mImplementation->mDecompress.output_height;
		mImplementation->mBytesPerPixel = mImplementation->mDecompress.output_components;
		
		return true;
	}
	
	std::vector<std::string> JPEGLoader::GetFileExtensions() const
	{
		return {"jpeg","jpg"};
	}
	
	u32 JPEGLoader::GetWidth() const
	{
		return mImplementation->mWidth;
	}
	
	u32 JPEGLoader::GetHeight() const
	{
		return mImplementation->mHeight;
	}
	
	Texture::Format JPEGLoader::GetFormat() const
	{
		return mImplementation->mFormat;
	}
	
	bool JPEGLoader::GetLoadInverted() const
	{
		return false;
	}
	
	void JPEGLoader::ReadLine(u8* lineStart, u32 maxLength)
	{
		if (!lineStart)
		{
			return;
		}
		// JSAMPARRAY may be unsigned in situations where JPEG can't detect that u8 is supported
		(void)jpeg_read_scanlines(&mImplementation->mDecompress, reinterpret_cast<JSAMPARRAY>(&lineStart), 1);
	}
	
	void JPEGLoader::CleanUp()
	{
		(void)jpeg_finish_decompress(&mImplementation->mDecompress);
		jpeg_destroy_decompress(&mImplementation->mDecompress);
	}

	struct StreamDestination
	{
		jpeg_destination_mgr pub; /* public fields */
		std::ostream* stream;
		JOCTET* buffer;
	};

	void InitStreamDestination(j_compress_ptr cinfo)
	{
	  StreamDestination* dest = reinterpret_cast<StreamDestination*>(cinfo->dest);

	  /* Allocate the output buffer --- it will be released when done with image */
	  dest->buffer = (JOCTET*)(*cinfo->mem->alloc_small) ((j_common_ptr)cinfo, JPOOL_IMAGE, OUTPUT_BUFFER_SIZE * sizeof(JOCTET));

	  dest->pub.next_output_byte = dest->buffer;
	  dest->pub.free_in_buffer = OUTPUT_BUFFER_SIZE;
	}
	
	/*
	 * Empty the output buffer --- called whenever buffer fills up.
	 * In typical applications, this should write the entire output buffer
	 * (ignoring the current state of next_output_byte & free_in_buffer),
	 * reset the pointer & count to the start of the buffer, and return TRUE
	 * indicating that the buffer has been dumped.
	 */
	boolean EmptyOutputBufferToStream(j_compress_ptr cinfo)
	{
		StreamDestination* dest = reinterpret_cast<StreamDestination*>(cinfo->dest);
		dest->stream->write(reinterpret_cast<const char*>(dest->buffer), OUTPUT_BUFFER_SIZE);
		if(dest->stream->bad())
		{
			ECHO_LOG_ERROR("Error writing stream buffer");
			return FALSE;
		}
		dest->pub.next_output_byte = dest->buffer;
		dest->pub.free_in_buffer = OUTPUT_BUFFER_SIZE;
		return TRUE;
	}	

	/*
	 * Terminate destination --- called by jpeg_finish_compress after all data has been written.  Usually needs to flush buffer.
	 */

	void TerminateStreamDestination(j_compress_ptr cinfo)
	{
		StreamDestination* dest = reinterpret_cast<StreamDestination*>(cinfo->dest);
		size_t datacount = OUTPUT_BUFFER_SIZE - dest->pub.free_in_buffer;

		/* Write any data remaining in the buffer */
		if (datacount > 0)
		{
			dest->stream->write(reinterpret_cast<const char*>(dest->buffer), datacount);
			if(dest->stream->bad())
			{
				ECHO_LOG_ERROR("Error writing end stream buffer");
			}
		}
		dest->stream->flush();
		if(dest->stream->bad())
		{
			ECHO_LOG_ERROR("Error flushing buffer");
		}
	}	

	bool SetupStreamDestination(j_compress_ptr cinfo, std::ostream& stream)
	{
		/* The destination object is made permanent so that multiple JPEG images
		 * can be written to the same file without re-executing jpeg_stdio_dest.
		 */
		if (!cinfo->dest)
		{
			/* first time for this JPEG object? */
			cinfo->dest = (struct jpeg_destination_mgr*)(*cinfo->mem->alloc_small)((j_common_ptr)cinfo, JPOOL_PERMANENT, sizeof(StreamDestination));
		} else
		if (cinfo->dest->init_destination != InitStreamDestination)
		{
			/* It is unsafe to reuse the existing destination manager unless it was
			 * created by this function.  Otherwise, there is no guarantee that the
			 * opaque structure is the right size.  Note that we could just create a
			 * new structure, but the old structure would not be freed until
			 * jpeg_destroy_compress() was called.
			 */
			ECHO_LOG_ERROR("j_compress points to a different output initialisation function. It is unsafe to reinitialise without first destroying the compressor.");
			return false;
		}

		StreamDestination* dest = reinterpret_cast<StreamDestination*>(cinfo->dest);
		dest->pub.init_destination = InitStreamDestination;
		dest->pub.empty_output_buffer = EmptyOutputBufferToStream;
		dest->pub.term_destination = TerminateStreamDestination;
		dest->stream = &stream;
		return true;
	}

	bool WriteJPEG(const Texture& texture, File* file, std::ostream* stream, int quality)
	{
		if(!file && !stream)
		{
			ECHO_LOG_ERROR("Cannot write without output. Please specify either file or stream.");
			return false;
		}
		// Make sure we're not dealing with insanity!
		u8* buffer = texture.GetBuffer().get();
		
		if(!buffer)
		{
			ECHO_LOG_ERROR("Cannot write texture without a buffer");
			return false;
		}

		if(texture.GetWidth()==0 || texture.GetHeight()==0)
		{
			ECHO_LOG_ERROR("Cannot save JPEG using Texture with zero dimension " << texture.GetWidth() << "x" << texture.GetHeight());
			return false;
		}
		if(texture.GetBytesPerPixel()==0 || texture.GetFormat()==Texture::Formats::UNKNOWN)
		{
			ECHO_LOG_ERROR("Cannot save JPEG using Texture with unknown format: " << texture.GetFormat());
			return false;
		}
		
		J_COLOR_SPACE colourSpace = JCS_UNKNOWN;
		int numberOfComponents = 0;
		switch(texture.GetFormat())
		{
			case Texture::Formats::R5G5B5A1:
			case Texture::Formats::RGBA_F32:
			case Texture::Formats::LUMINANCE32_ALPHA32:
			case Texture::Formats::RGB_F32:
			case Texture::Formats::LUMINANCE32:
			case Texture::Formats::UNKNOWN:
			case Texture::Formats::R4G4B4A4:
			case Texture::Formats::LUMINANCE8_ALPHA8:
			case Texture::Formats::GREYSCALE16:
			// Not supported
			break;
			case Texture::Formats::R5G6B5:
				colourSpace = JCS_RGB565;
				numberOfComponents = 3;
			break;
			case Texture::Formats::R8G8B8A8:
				colourSpace = JCS_EXT_RGBA;
				numberOfComponents = 4;
			break;
			case Texture::Formats::B8G8R8A8:
				colourSpace = JCS_EXT_BGRA;
				numberOfComponents = 4;
			break;
			case Texture::Formats::R8G8B8X8:
				colourSpace = JCS_EXT_RGBX;
				numberOfComponents = 3;
			break;
			case Texture::Formats::R8G8B8:
				colourSpace = JCS_RGB;
				numberOfComponents = 3;
			break;
			case Texture::Formats::B8G8R8:
				colourSpace = JCS_EXT_BGR;
				numberOfComponents = 3;
			break;
			case Texture::Formats::LUMINANCE8:
			case Texture::Formats::GREYSCALE8:
				colourSpace = JCS_GRAYSCALE;
				numberOfComponents = 1;
			break;
		}
		
		if(numberOfComponents==0 || colourSpace==JCS_UNKNOWN)
		{
			ECHO_LOG_ERROR("Failed to determine colour space for texture format " << texture.GetFormat());
			return false;
		}
		
		/* This struct contains the JPEG compression parameters and pointers to
		 * working space (which is allocated as needed by the JPEG library).
		 * It is possible to have several such structures, representing multiple
		 * compression/decompression processes, in existence at once.  We refer
		 * to any one struct (and its associated working data) as a "JPEG object".
		 */
		jpeg_compress_struct cinfo;
		
		/* This struct represents a JPEG error handler.  It is declared separately
		 * because applications often want to supply a specialized error handler
		 * (see the second half of this file for an example).  But here we just
		 * take the easy way out and use the standard error handler, which will
		 * print a message on stderr and call exit() if compression fails.
		 * Note that this struct must live as long as the main JPEG parameter
		 * struct, to avoid dangling-pointer problems.
		 */
		jpeg_error_mgr jerr;
		
		JSAMPROW row_pointer[1];      /* pointer to JSAMPLE row[s] */

		/* Step 1: allocate and initialize JPEG compression object */

		/* We have to set up the error handler first, in case the initialization
		 * step fails.  (Unlikely, but it could happen if you are out of memory.)
		 * This routine fills in the contents of struct jerr, and returns jerr's
		 * address which we place into the link field in cinfo.
		 */
		cinfo.err = jpeg_std_error(&jerr);
		/* Now we can initialize the JPEG compression object. */
		jpeg_create_compress(&cinfo);
		
		/* Step 2: specify data destination (eg, a file) */
		/* Note: steps 2 and 3 can be done in either order. */
		if(stream)
		{
			if(!SetupStreamDestination(&cinfo, *stream))
			{
				return false;
			}
		}else
		{
			if(!SetupFileDestination(&cinfo, file))
			{
				return false;
			}
		}
		
		/* Step 3: set parameters for compression */

		/* First we supply a description of the input image.
		 * Four fields of the cinfo struct must be filled in:
		 */
		cinfo.image_width = texture.GetWidth();      /* image width and height, in pixels */
		cinfo.image_height = texture.GetHeight();
		cinfo.in_color_space = colourSpace;
		cinfo.input_components = numberOfComponents;
		
		/* Now use the library's routine to set default compression parameters.
		 * (You must set at least cinfo.in_color_space before calling this,
		 * since the defaults depend on the source color space.)
		 */
		jpeg_set_defaults(&cinfo);
		/* Now you can set any non-default parameters you wish to.
		 * Here we just illustrate the use of quality (quantization table) scaling:
		 */
		jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);		
		
		/* Step 4: Start compressor */

		/* TRUE ensures that we will write a complete interchange-JPEG file.
		 * Pass TRUE unless you are very sure of what you're doing.
		 */
		jpeg_start_compress(&cinfo, TRUE);
		
		/* Step 5: while (scan lines remain to be written) */
		/*           jpeg_write_scanlines(...); */

		/* Here we use the library's state variable cinfo.next_scanline as the
		 * loop counter, so that we don't have to keep track ourselves.
		 * To keep things simple, we pass one scanline per call; you can pass
		 * more if you wish, though.
		 */
		int rowStride = texture.GetWidth() * texture.GetBytesPerPixel(); /* JSAMPLEs per row in image_buffer */

		while (cinfo.next_scanline < cinfo.image_height)
		{
			/* jpeg_write_scanlines expects an array of pointers to scanlines.
			* Here the array is only one element long, but you could pass
			* more than one scanline at a time if that's more convenient.
			*/
			row_pointer[0] =  reinterpret_cast<JSAMPLE*>(&buffer[cinfo.next_scanline * rowStride]);
			(void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
		}
		
		/* Step 6: Finish compression */
		jpeg_finish_compress(&cinfo);
		return true;
	}

	bool JPEGLoader::Write(const Texture& texture, std::ostream& stream, int quality)
	{
		return WriteJPEG(texture, nullptr, &stream, quality);
	}

	bool JPEGLoader::Write(const Texture& texture, File file, int quality)
	{
		return WriteJPEG(texture, &file, nullptr, quality);
	}
	
}
