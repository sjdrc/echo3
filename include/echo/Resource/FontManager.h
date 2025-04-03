#ifndef _ECHOFONTMANAGER_H_
#define _ECHOFONTMANAGER_H_

#include <echo/Resource/ResourceManager.h>
#include <echo/Graphics/TextureUnit.h>
#include <echo/Schema/Font.capnp.h>

namespace capnp
{
	class MessageReader;
}

namespace Echo
{
	class FileSystem;
	class TextureManager;
	class MaterialManager;
	class Font;
	class File;
	
	/**
	 * FontManager is a resource manager that loads, generates and manages Font resources.
	 * Fonts are object that contain a Material that points to a character map Texture and
	 * a list of glyphs (characters and combinations of characters) where they are located
	 * in the Texture in terms of texture coordinates.
	 * The Glyph information is used by other objects, such as TextMesh, to build meshes
	 * that make up strings.
	 * FontManager can also attempt to automatically generate new Font resources on demand
	 * if existing fonts do not match the rendering requirements.
	 * 
	 * =About DPI and font quality=
	 * If you aren't familiar with how font rendering works this might be useful.
	 * 
	 * When rendering text using point size the display you are rendering to matters in
	 * determining the pixel resolution. This is why on high density displays 12pt text
	 * requires more pixels than 12pt text on a low density display. The goal is to have
	 * text in point size be the same size across displays. For example, 12pt text on a
	 * 5 inch screen at a resolution of 1920x1080 will require more pixels to display
	 * the same physical size compared to 12pt text on a 5inch screen with a resolution
	 * of 1280x720.
	 * 
	 * It always possible to display text in accurate point sizes when the display density
	 * is not available. Some operating systems use the same display density for all GUI
	 * rendering which is why switching to a high resolution display would make text appear
	 * small. (I'm mainly referring to older versions of Windows since it is the first I
	 * know off the top of my head). It wasn't always the operating system's fault though,
	 * with older displays that do not report their pixel density you have to settle for
	 * a suitable default. This will be platform specific.
	 * 
	 * =How is DPI used in Echo?=
	 * There are multiple ways Echo deals with DPI for fonts, and it is designed to be
	 * transparent where possible, but also give you some control if you want it.
	 * 
	 * 1. When you obtain a font resource you can specify the target density if you know it.
	 * This can generate a custom Font resource specific to your purpose.
	 * 2. Font resource files can specify the default pixel density of the font for when
	 * the usage target density is unknown. This is useful in cases when you're trying to
	 * save memory or if a font is of particularly high detail that a low resolution (in
	 * pixels) version of it would look bad.
	 * 3. Each FontManager is configured with a default pixel density which is used when
	 * generating fonts but you do not know the target DPI or when loading Fonts that
	 * do not have the pixel density specified.
	 * 
	 * Typically, a FontManager will be configured with the default density to match the
	 * display target where possible. This is taken care of for you if you're building
	 * an application uses the Application class.
	 * 
	 * GetResource() will load the font using the DPI from the Font defaults in 2, but
	 * fall back to 3 if the Font wasn't configured with a default.
	 * GetFont() will load using the specified DPI but fall back to 3 if the DPI is not
	 * specified (or 0).
	 * 
	 * =Pixels and world space=
	 * Since we can have a scene where the world units map to an arbitrary number of
	 * pixels on screen for any given frame, rendering a Mesh in a Scene means you can't
	 * rely on a world unit of 1 to be 1 pixel on screen, and TextMesh typically is built
	 * using 1 unit=1 pixel. The GUI system allows you to set a world unit converter per
	 * Element or use a single one for an entire hierarchy. There are also classes that
	 * take care of the automatic calculation of this for you, such as Screen.
	 * 
	 * It is recommended, at least initially, to use the GUI system to display text
	 * accurately on screen.
	 */
	class FontManager : public ResourceManager<Font>
	{
	public:
		/**
		 * @param fileSystem the FileSystem used to load data from.
		 * @param textureManager The textureManager used to create character map textures.
		 * The Texture resources will exist in the TextureManager so can be accessed like
		 * normal Textures after they have been created which means you could map a whole
		 * character map texture to something else if you want. The resource name will
		 * be the "FontName CharacterMap".
		 * @param materialManager The material manager used to create materials, like the
		 * Textures, each one will correspond to a single Font resource and can be
		 * accessed elsewhere through the MaterialManager. Names are "FontName Material"
		 * @param defaultDPIX The default X pixel density in dots per inch to render to
		 * if the target DPI is not specified in the resource file or when obtaining a
		 * Font resource.
		 * @param defaultDPIY The default Y pixel density in dots per inch to render to
		 * if the target DPI is not specified in the resource file or when obtaining a
		 * Font resource.
		 * @param defaultTextureFilter the default texture filter method if it is not
		 * specified in the resource file.
		 */
		FontManager(FileSystem& fileSystem, TextureManager& textureManager, MaterialManager& materialManager,
				 Size defaultDPIX, Size defaultDPIY,
				 TextureUnit::TextureFilter defaultTextureFilter = TextureUnit::TextureFilters::LINEAR);
		~FontManager();
	
		/**
		 * Load a font resource.
		 * @param resourceFile Resource filename name.
		 * @param resourceName Resource we are attempting to loading.
		 * @return A Font resource if the resource could be loaded, nullptr if any errors occurred. Errors will
		 * be output to the console.
		 */
		shared_ptr<Font> LoadResource(const std::string& resourceFile, const std::string& resourceName) override;

		bool LoadIntoResource(const std::string& resourceName, Font& fontToLoadInto) override;

		/**
		 * Get a font resource.
		 * This provides more control over the variation of the Font resource. GetResource() will load a
		 * preset resource but does not allow for variations. This method will create new resource entries
		 * for each font given a base name and size if needed. It requires a resource entry for the base
		 * font to exist in order to find the font to load and render. For example:
		 *		- A font file might specify a base font of "EchoFont" which has default render
		 *		parameters and maps to the file "EchoFont.ttf".
		 *		- Using GetResource("EchoFont") would return a Font resource with the default render
		 *		parameters that may not be specifically configured for the display target.
		 *		- Using GetFont("EchoFont",12) will generate (if needed) a font designed to be rendered
		 *		to a display with the default DPI (often the primary render target) with a 1:1 pixel
		 *		mapping.
		 * This method is used by GUI::Text to display more accurate fonts than using the one character
		 * map for all text and scaling it.
		 * If a new resource needs to be created the resource name becomes a concatenation of the resource
		 * name, the point size and target DPI, each separated by a colon. For example, if "EchoFont" needed
		 * to be generated in point size 16 for a render target of 150dpi the new resource name would be
		 * "EchoFont:16:150:150". These new resources are added to the FontManager so they only need to be
		 * generated once.
		 * @note The new resource name takes on the parameter values so if the default DPI is used the
		 * resource name will have :0:0 at the end rather than the actual DPI used name.
		 * @param name The resource name.
		 * @param pointSize The point size of the font.
		 * @param xDPI The pixels per inch of the target display on the X axis, 0 indicates to use
		 * the FontManager configured default (not the Font default, use GetResource() for the Font
		 * default).
		 * @param yDPI The pixels per inch of the target display on the Y axis, 0 indicates to use
		 * the FontManager configured default (not the Font default, use GetResource() for the Font
		 * default).
		 * @return a Font resource if the font already exists or can be generated, otherwise null pointer.
		 */
		shared_ptr<Font> GetFont(const std::string& name, Size pointSize, Size xDPI=0, Size yDPI=0);
		
		//bool LoadList(File listFile) override;

		FileSystem* GetFileSystem() const override;
		
		/**
		 * Creates a font from resource data.
		 * @param fontParameters Object containing the font parameters.
		 * @param fontName The name of the Font if the material data does not contain a name.
		 * @param fontFile The file that this data came from, used for resolving relative file names.
		 * @param font The font to build into.
		 * @return true if the font was parsed successfully, otherwise false. On failure the font object won't be modified.
		 */
		bool ParseFont(Resources::Font::Reader fontParameters, const std::string& fallbackFontName, const std::string& fontFile, Font& font);
		
	private:
		bool ProcessListEntry(const std::string& key, const std::string& resourceFile, const std::vector<std::string>& params) override;
		FileSystem& mFileSystem;
		TextureManager& mTextureManager;
		MaterialManager& mMaterialManager;
		Size mDefaultXDPI;											/// The default X DPI if not specified when loading the Font.
		Size mDefaultYDPI;											/// The default Y DPI if not specified when loading the Font.
		TextureUnit::TextureFilter mDefaultTextureFilter;			/// The default texture filter method for Font Materials
																	/// if not specified when loading the Font.
		typedef std::pair<UTF32Code, UTF32Code> UTF32CodeRangeInclusive;
		typedef std::vector<UTF32CodeRangeInclusive> UTF32CodeRangeList;
		
		struct FontParameters
		{
			u32 mPointSize;
			u32 mXDPI;
			u32 mYDPI;
			UTF32CodeRangeList mCharacterRanges;
		};
		typedef std::map<std::string,FontParameters> FontParametersMap;
		FontParametersMap mFontParameters;
		FontParameters mDefaultFontParameters;
	};
}
#endif
