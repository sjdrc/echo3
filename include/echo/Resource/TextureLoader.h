#ifndef _ECHOTEXTUTURELOADER_H_
#define _ECHOTEXTUTURELOADER_H_
#include <echo/Graphics/Texture.h>

namespace Echo
{
	class File;
	
	/**
	 * A texture loader loads a specific file type and creates a texture.
	 */
	class TextureLoader
	{
	public:
		TextureLoader();
		virtual ~TextureLoader();
		
		Texture* LoadTexture(File& file, bool forcePowerOfTwoTextures, Texture* textureToLoadInto = nullptr);
		
		/**
		 * Get supported file extensions that the loader is designed to load.
		 * The extension should not include the dot. So for example, bmp should be "bmp" and not ".bmp".
		 * @return the file extension that the loader loads.
		 */
		virtual std::vector<std::string> GetFileExtensions() const = 0;

		/**
		 * Read the specified file for information about the image we are going to load.
		 * The image will be loaded line by line using ReadLine(). If it is more appropriate to load the entire file at
		 * this point then that is fine.
		 * This method will be called before the first row will be read using ReadLine() so if your loader pre-loads you
		 * will need to reset a row counter within this method.
		 * @param textureFile
		 * @return 
		 */
		virtual bool ProcessFile(File& textureFile) = 0;

		/**
		 * The width of the image.
		 * @return width.
		 */
		virtual u32 GetWidth() const = 0;
		
		/**
		 * The height of the image.
		 * @return height.
		 */
		virtual u32 GetHeight() const = 0;
		/**
		 * Get the format of the image.
		 * @return format.
		 */
		virtual Texture::Format GetFormat() const = 0;
		
		/**
		 * If the image is stored upside down (like in bitmap images) this method should return true.
		 * @return Whether to load from bottom to top instead of top to bottom.
		 */
		virtual bool GetLoadInverted() const = 0;
		
		/**
		 * Read a single line from the image.
		 * This method will be called height number of times. The first time it is called after ProcessFile() should return the first image row of pixels.
		 * @param lineStart the destination buffer - start of the line.
		 * @param maxLengthInBytes maximum number of bytes you can write to the destination. The loading process will allocate as many bytes as determined
		 * by the width and format.
		 */
		virtual void ReadLine(u8* lineStart, u32 maxLengthInBytes) = 0;
		
		/**
		 * Clean up any allocated resources used for loading.
		 * This method will be called when an error occurs or when the loader is no longer required.
		 */
		virtual void CleanUp() = 0;
	};
}
#endif
