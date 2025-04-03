#ifndef _ECHOPNGLOADER_H_
#define _ECHOPNGLOADER_H_

#include <string>
#include <echo/Resource/TextureLoader.h>
#include <boost/scoped_ptr.hpp>

namespace Echo
{
	class PNGLoader : public TextureLoader
	{
	public:
		PNGLoader();
		~PNGLoader();
		
		std::vector<std::string> GetFileExtensions() const;
		bool ProcessFile(File& textureFile);
		u32 GetWidth() const;
		u32 GetHeight() const;
		Texture::Format GetFormat() const;
		bool GetLoadInverted() const;
		void ReadLine(u8* lineStart, u32 maxLength);
		void CleanUp();
		
		/**
		 * Write a texture to a stream as PNG file.
		 * Currently only the following formats are supported:
		 *	- BGRA, RGBA 4 and 8 bit channels
		 *	- BGRX, RGBX 4 and 8 bit channels
		 *	- BGR, RGB 8 bit channels
		 *  - GREYSCALE 8 and 16 bit channels
		 *  - DEPTH 16 bit channel
		 *  - LUMINANCE 8 bit channel
		 *  - LUMINANCE ALPHA 8 bit channels
		 * @note Only minimal header information is written.
		 * @param texture The texture to write
		 * @param outstream the output stream.
		 * @return true if successful, otherwise false. See the log for more info.
		 */
		static bool WritePNG(const Texture& texture, std::ostream& outstream);
	private:
		// d-pointer idiom to hide libpng internals from callers:
		class Implementation;
		boost::scoped_ptr<Implementation> mImplementation;
	};
}

#endif

