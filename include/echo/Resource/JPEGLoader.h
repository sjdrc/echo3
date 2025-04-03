#ifndef _ECHOJPEGLOADER_H_
#define _ECHOJPEGLOADER_H_

#include <string>
#include <echo/Resource/TextureLoader.h>
#include <boost/scoped_ptr.hpp>

namespace Echo
{
	class JPEGLoader : public TextureLoader
	{
	public:
		JPEGLoader();
		~JPEGLoader();
		
		std::vector<std::string> GetFileExtensions() const;
		bool ProcessFile(File& textureFile);
		u32 GetWidth() const;
		u32 GetHeight() const;
		Texture::Format GetFormat() const;
		bool GetLoadInverted() const;
		void ReadLine(u8* lineStart, u32 maxLength);
		void CleanUp();
		
		/**
		 * Write a texture to a stream as JPEG file.
		 * Currently only the following formats are supported:
		 *	R5G6B5:
		 *	R8G8B8A8:
		 *	B8G8R8A8:
		 *	R8G8B8X8:
		 *	R8G8B8:
		 *	B8G8R8:
		 *	LUMINANCE8:
		 *	GREYSCALE8:
		 * @param texture The texture to write
		 * @param file the output file, must be opened in write mode.
		 * @return true if successful, otherwise false. See the log for more info.
		 */
		static bool Write(const Texture& texture, File outFile, int quality);
		static bool Write(const Texture& texture, std::ostream& outstream, int quality);
	private:
		// d-pointer idiom to hide libjpeg-turbo internals from callers
		class Implementation;
		boost::scoped_ptr<Implementation> mImplementation;
	};
}

#endif

