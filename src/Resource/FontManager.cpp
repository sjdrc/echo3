#include <echo/Resource/FontManager.h>
#include <echo/Resource/MaterialManager.h>
#include <echo/Resource/TextureManager.h>
#include <echo/FileSystem/FileSystem.h>
#include <echo/Graphics/Font.h>
#include <echo/Graphics/Material.h>
#include <echo/Graphics/Texture.h>
#include <echo/Util/Utils.h>
#include <echo/Util/StringUtils.h>
#include <echo/Maths/EchoMaths.h>
#include <echo/UTF8String.h>
#include <echo/Resource/MaterialManager.h>
#include <echo/Util/Capnp.h>
#include <boost/scoped_array.hpp>
#include <boost/scope_exit.hpp>
#include <boost/lexical_cast.hpp>

#ifdef ECHO_FREETYPE_SUPPORT_ENABLED
#include <ft2build.h>
#include FT_FREETYPE_H
#endif

namespace Echo
{
	FontManager::FontManager(FileSystem& fileSystem, TextureManager& textureManager,
							 MaterialManager& materialManager,
							 Size defaultXDPI,
							 Size defaultYDPI,
							 TextureUnit::TextureFilter defaultTextureFilter) :
		ResourceManager<Font>("font"),
		mFileSystem(fileSystem),
		mTextureManager(textureManager),
		mMaterialManager(materialManager),
		mDefaultXDPI(defaultXDPI),
		mDefaultYDPI(defaultYDPI),
		mDefaultTextureFilter(defaultTextureFilter)
	{
		mDefaultFontParameters.mPointSize=12;
		mDefaultFontParameters.mXDPI=mDefaultXDPI;
		mDefaultFontParameters.mYDPI=mDefaultXDPI;
	}
	
	FontManager::~FontManager()
	{
	}

	shared_ptr<Font> FontManager::LoadResource(const std::string& resourceFile, const std::string& resourceName)
	{
		try
		{
			shared_ptr<CapnpMessageHelper> message = CreateCapnpReaderFromData<Resources::Font>(resourceFile,mFileSystem);
			if(!message)
			{
				ECHO_LOG_ERROR("Unable to create reader from \"" << resourceFile << "\"");
				return nullptr;
			}
			Resources::Font::Reader fontData = message->GetReader<Resources::Font>();

			shared_ptr<Font> font(new Font());
			if(ParseFont(fontData,resourceName,resourceFile,*font))
			{
				return font;
			}
		}catch(...)
		{
		}
		ECHO_LOG_ERROR("Failed to load font resource " << resourceFile << " as " << resourceName);
		return nullptr;
	}

	bool FontManager::LoadIntoResource(const std::string& resourceName, Font& fontToLoadInto)
	{
		std::string resourceFileName = GetResourceFileName(resourceName);
		if(resourceFileName.empty())
		{
			return false;
		}
		try
		{		
			shared_ptr<CapnpMessageHelper> message = CreateCapnpReaderFromData<Resources::Font>(resourceFileName,mFileSystem);
			if(!message)
			{
				ECHO_LOG_ERROR("Unable to create reader from \"" << resourceFileName << "\"");
				return false;
			}
			Resources::Font::Reader fontData = message->GetReader<Resources::Font>();
			return ParseFont(fontData,resourceName,resourceFileName,fontToLoadInto);
		}catch(...)
		{
			ECHO_LOG_ERROR("Failed to create reader from \"" << resourceFileName << "\"");
			return false;
		}
	}

	bool FontManager::ProcessListEntry(const std::string& key, const std::string& resourceFileName, const std::vector<std::string>& params)
	{
		if(key=="font.bundle")
		{
			std::string wholeFile;
			File file = mFileSystem.Open(resourceFileName);
			if(!file.IsOpen())
			{
				ECHO_LOG_ERROR("Could not open \"" << resourceFileName << "\"");
				return false;
			}
			file.ReadFileIntoString(wholeFile);

			try
			{
				shared_ptr<CapnpMessageHelper> message = CreateCapnpReaderFromData<Resources::Fonts>(resourceFileName,mFileSystem);
				if(!message)
				{
					ECHO_LOG_ERROR("Unable to create reader from \"" << resourceFileName << "\"");
					return false;
				}
				Resources::Fonts::Reader fontBundleData = message->GetReader<Resources::Fonts>();
				if(fontBundleData.hasFonts())
				{
					ECHO_LOG_WARNING("Font bundle file \"" << resourceFileName << "\" does not list any fonts.");
					return true;
				}
				auto fonts = fontBundleData.getFonts();
				for(int i = 0; i < fonts.size(); ++i)
				{
					// Since there isn't a resource name we'll use: resourceFile:i this becomes the lookup name
					// if the material doesn't have a name.
					std::stringstream ss; ss << resourceFileName << ":" << i;
					shared_ptr<Font> font(new Font());
					if(ParseFont(fonts[i],ss.str(),resourceFileName, *font))
					{
						AddResource(font->GetName(),font);
					}
				}
				return true;
			}
			catch(...)
			{
				ECHO_LOG_ERROR("Failed to load fonts: " << resourceFileName);
				return false;
			}
		}
		//Any other keys are fine.
		return true;
	}
	
	shared_ptr<Font> FontManager::GetFont(const std::string& name, Size pointSize, Size xDPI, Size yDPI)
	{
		std::stringstream fullResourceName;
		fullResourceName << name << ":" << pointSize << ":" << xDPI << ":" << yDPI;
		if(ResourceExists(fullResourceName.str()))
		{
			return GetResource(fullResourceName.str());
		}
		
		std::string resourceFileName = GetResourceFileName(name);
		if(resourceFileName.empty())
		{
			ECHO_LOG_ERROR("Could not determine resource file name for \"" << name << "\"");
			return nullptr;
		}

		try
		{
			//Get the parameters for the character range
			shared_ptr<CapnpMessageHelper> message = CreateCapnpReaderFromData<Resources::Font>(resourceFileName,mFileSystem);
			if(!message)
			{
				ECHO_LOG_ERROR("Unable to create reader from \"" << resourceFileName << "\"");
				return nullptr;
			}
			::capnp::MallocMessageBuilder newFontMessage;

			// Copy the parameters
			Resources::Font::Reader existingFont = message->GetReader<Resources::Font>();
			newFontMessage.setRoot(existingFont);
			Resources::Font::Builder newFontBuilder = newFontMessage.getRoot<Resources::Font>();

			//Now we're going to add a new resource as name:size:xdpi:ydpi

			//Update the new parameters.
			newFontBuilder.setPointSize(pointSize);
			newFontBuilder.setXDPI(xDPI);
			newFontBuilder.setYDPI(yDPI);
			newFontBuilder.setName(fullResourceName.str());
			shared_ptr<Font> font(new Font());
			if(ParseFont(newFontBuilder.asReader(),fullResourceName.str(),resourceFileName,*font))
			{
				AddResource(fullResourceName.str(),font);
				return font;
			}
		}
		catch(...)
		{}
		ECHO_LOG_ERROR("Failed to process \"" << resourceFileName << "\" for creating a new font with parameters - pointSize: " << pointSize << " xdpi: " << xDPI << " ydpi: " << yDPI);
		return nullptr;
	}

	bool FontManager::ParseFont(Resources::Font::Reader fontParameters, const std::string& fallbackFontName, const std::string& fontFile, Font& fontObject)
	{
		std::string fontName = fontParameters.hasName() ? fontParameters.getName().cStr() : fallbackFontName;

		//When you see a bitwise shift in this method it is the conversion of a fixed point integer to a regular integer,
		//for example. FreeType uses 26.6 fixed point a lot, so we will be shifting the fractional part away with >> 6.

		std::string resolvedFileName = mFileSystem.ResolveFullNameForFileWithParent(fontParameters.getFilename().cStr(),fontFile);
		File f = mFileSystem.Open(resolvedFileName);
		if(!f.IsOpen())
		{
			ECHO_LOG_ERROR("Font could not open file " << resolvedFileName);
			return false;
		}

		if(f.GetSize()==0)
		{
			ECHO_LOG_ERROR("specified file has length of zero - " << resolvedFileName);
			return false;
		}

		boost::scoped_array<u8> fileBuffer(new u8[f.GetSize()]);
		size_t bytesRead = f.Read(fileBuffer.get(),1,f.GetSize());
		if(bytesRead!=f.GetSize())
		{
			ECHO_LOG_ERROR("Font could not read all file data - " << resolvedFileName);
			return false;
		}
		
		//Determine the material
		shared_ptr<Material> material = mMaterialManager.CreateMaterial(fontName + " Material");
		if(fontParameters.hasMaterial())
		{
			mMaterialManager.ParseMaterial(fontParameters.getMaterial(),fontName + " Material", fontFile, *material);
			if(material->GetNumberOfPasses()==0)
			{
				ECHO_LOG_WARNING("The specified material does not have any passes. Setting the material to the default.");
				material->SetToDefaultMaterial();
			}
		}else
		{
			material->SetToDefaultMaterial();
		}

		if(fontParameters.hasColour())
		{
			material->GetPass(0)->SetDiffuse(MaterialManager::ConvertColour(fontParameters.getColour()));
		}

		#ifdef ECHO_FREETYPE_SUPPORT_ENABLED
		FT_Library freeTypeLibrary;
		if( FT_Init_FreeType( &freeTypeLibrary) )
		{
			ECHO_LOG_ERROR("FontManager could not initialise FreeType library.");
			return false;
		}

		FT_Face face;

		// Load font
		FT_Error e=FT_New_Memory_Face( freeTypeLibrary, fileBuffer.get(), (FT_Long)f.GetSize() , 0, &face );
		if(e)
		{
			ECHO_LOG_ERROR("FreeType could not load font face: " << e);
			FT_Done_FreeType(freeTypeLibrary);
			return false;
		}

		u32 characterSpacer = 2;
		Size xDPI = (fontParameters.getXDPI()!=0) ? fontParameters.getXDPI() : mDefaultXDPI;
		Size yDPI = (fontParameters.getYDPI()!=0) ? fontParameters.getYDPI() : mDefaultYDPI;

		// Convert point size to FreeType 26.6 fixed point format
		s32 pointSize = (fontParameters.getPointSize()!=0) ? fontParameters.getPointSize() : 32; // This is an arbitrary point size. It was required with protobufs but capnp just defaults to 0.
		FT_F26Dot6 freeTypeSize = (FT_F26Dot6)( pointSize * (1 << 6));
		if( FT_Set_Char_Size( face, freeTypeSize, 0, xDPI, yDPI) )
		{
			ECHO_LOG_ERROR("FreeType could not set character size.");
			FT_Done_FreeType(freeTypeLibrary);
			return false;
		}

		int maxHeight = 0, maxWidth = 0;
		int maxBearingY=0;

		std::map<UTF32Code, shared_ptr<Glyph> > glyphMap;
		UTF32CodeRangeList codeRangeList;
		
		//Process code ranges.
		if(fontParameters.hasRange())
		{
			auto ranges = fontParameters.getRange();
			for(Size i=0; i<ranges.size(); ++i)
			{
				Resources::Font::CharacterRange::Reader range = ranges[i];
				if(range.getLowerCode()==0 || range.getUpperCode()==0)
				{
					ECHO_LOG_ERROR("Range codes cannot be zero. lower: " << range.getLowerCode() << " upper: " << range.getUpperCode());
					return false;
				}
				codeRangeList.push_back(UTF32CodeRangeInclusive(range.getLowerCode(),range.getUpperCode()));
			}
		}
		
		//Add characters
		if(fontParameters.hasCharacters())
		{
			auto characters = fontParameters.getCharacters();
			for(int i=0; i<characters.size(); ++i)
			{
				UTF8String utf8Characters(characters[i]);
				UTF8String::iterator it=utf8Characters.begin();
				UTF8String::iterator itEnd=utf8Characters.end();
				while(it!=itEnd)
				{
					UTF32Code code = *it;
					codeRangeList.push_back(UTF32CodeRangeInclusive(code, code));
					it++;
				}
			}
		}
		
		//Get some defaults if we can.
		if(codeRangeList.empty())
		{
			FT_ULong  charcode;
			FT_UInt   gindex;
			charcode = FT_Get_First_Char( face, &gindex );
			// 1024 means 32*32 at 128*128px per glyph would be a 4096x4096px texture. This doesn't perform any
			// enforcement for texture size though, it just tries to prevent a less likely case from occurring.
			// This will probably change in the future though.
			const size_t MAX_NUMBEROF_ENUMERATED_GLYPHS=1024;
			while ( gindex != 0 && codeRangeList.size() < MAX_NUMBEROF_ENUMERATED_GLYPHS)
			{
				codeRangeList.push_back(UTF32CodeRangeInclusive(charcode, charcode));
				charcode = FT_Get_Next_Char( face, charcode, &gindex );
			}
		}
		
		fontObject.SetHeightBetweenTwoBaseLines((face->height>>6));
		fontObject.SetName(fontName);
		fontObject.SetMaterial(material);

		//Should we load the replacement character?
		if(!fontParameters.getLoadReplacementCharacter())
		{
			UTF32Code replacementChar=0xFFFD;
			if(fontParameters.getReplacementCharacter())
			{
				replacementChar = fontParameters.getReplacementCharacter();
			}
			codeRangeList.push_back(UTF32CodeRangeInclusive(replacementChar, replacementChar));
			fontObject.SetReplacementCharacter(replacementChar);
		}
		if(fontParameters.getForceUseReplacementCharacter())
		{
			fontObject.SetForceUseReplacementCharacter(fontParameters.getForceUseReplacementCharacter());
		}
		if(fontParameters.getWarnOnReplacementCharacterUsage())
		{
			fontObject.SetWarnUseOfReplacementCharacter(fontParameters.getWarnOnReplacementCharacterUsage());
		}
		
		// Calculate maximum width, height and bearing and kerning
		size_t glyphCount = 0;
		size_t totalPixelCount = 0;
		for (UTF32CodeRangeList::const_iterator r = codeRangeList.begin();	r != codeRangeList.end(); ++r)
		{
			const UTF32CodeRangeInclusive& range = *r;
			for(UTF32Code code = range.first; code <= range.second; ++code)
			{
				FT_UInt leftIndex = FT_Get_Char_Index( face, code );
				if(leftIndex==0)
				{
					continue;
				}
				
				++glyphCount;
				
				//Find the glyph we're going to be working on.
				std::map<UTF32Code, shared_ptr<Glyph> >::iterator mit = glyphMap.find(code);
				shared_ptr<Glyph> currentGlyph;
				if(mit!=glyphMap.end())
				{
					currentGlyph = mit->second;
				}else
				{
					currentGlyph = make_shared<Glyph>();
					currentGlyph->mCode = code;
					glyphMap.insert(std::make_pair(code,currentGlyph));
				}

				//Calculate kerning.
				for (UTF32CodeRangeList::const_iterator r2 = codeRangeList.begin(); r2 != codeRangeList.end(); ++r2)
				{
					const UTF32CodeRangeInclusive& rightRange = *r2;
					for(UTF32Code rightCode = rightRange.first; rightCode <= rightRange.second; ++rightCode)
					{
						FT_Vector kerning;
						FT_UInt rightIndex = FT_Get_Char_Index( face, rightCode );
						if(leftIndex && rightIndex)
						{
							FT_Error error = FT_Get_Kerning( face,leftIndex,rightIndex,FT_KERNING_DEFAULT, &kerning );
							if(!error)
							{
								currentGlyph->mKerning[rightCode] = kerning.x >> 6;
							}
						}
					}
				}
				FT_Load_Char( face, code, FT_LOAD_RENDER );

				currentGlyph->mWidth = face->glyph->bitmap.width;
				currentGlyph->mHeight = face->glyph->bitmap.rows;
				currentGlyph->mAdvanceX = face->glyph->advance.x >> 6;
				currentGlyph->mAdvanceY = face->glyph->advance.y >> 6;

				if( face->glyph->metrics.height > maxHeight )
				{
					maxHeight = face->glyph->metrics.height;
				}
				totalPixelCount+=( 2 * ( face->glyph->bitmap.rows << 6 ) - face->glyph->metrics.horiBearingY );

				if( face->glyph->metrics.horiBearingY > maxBearingY )
				{
					maxBearingY = face->glyph->metrics.horiBearingY;
				}

				if( (face->glyph->advance.x >> 6 ) + ( face->glyph->metrics.horiBearingX >> 6 ) > maxWidth)
				{
					maxWidth = (face->glyph->advance.x >> 6 ) + ( face->glyph->metrics.horiBearingX >> 6 );
				}
				
				totalPixelCount+=(face->glyph->advance.x >> 6 ) + ( face->glyph->metrics.horiBearingX >> 6 );
			}

		}

		//Convert to an integer
		maxHeight = maxHeight >> 6;

		// Now work out how big our texture needs to be
		size_t rawSize = totalPixelCount;
		//Ogre does it this way, however this wastes a lot of texture space, using twice as much as necessary in some cases that I've come across.
		//size_t rawSize = (maxWidth + characterSpacer) * ((maxHeight >> 6) + characterSpacer) * glyphCount;

		u32 texSide = static_cast<u32>(Maths::Sqrt((f32)rawSize));

		// just in case the size might chop a glyph in half, add another glyph width/height
		texSide += std::max(maxWidth, maxHeight);

		// Now round up to nearest power of two
		u32 roundUpSize = NextPow2(texSide);

		// Would we benefit from using a non-square texture (2X width)
		size_t finalWidth, finalHeight;
		// Ogre was comparing just against rawSize, but with my adjustment above (with totalPixelCount to save space) we need to be careful that
		// there is enough height for the last row, this seems to be safe. To be honest I'm not happy with this texture size estimation. A more
		// calculated approach would be better. I have created a task, T122, to improve this at some point in the future.
		if (roundUpSize*roundUpSize/2 >= rawSize*1.5)
		{
			finalHeight = static_cast<size_t>(roundUpSize/2);
		}else
		{
			finalHeight = roundUpSize;
		}
		finalWidth = roundUpSize;

		const size_t pixelBytes = 2;
		size_t dataWidth = finalWidth * pixelBytes;
		size_t dataSize = finalWidth * finalHeight * pixelBytes;

		std::string textureName = fontName + " CharacterMap";
		ECHO_LOG_INFO("FontManager: Creating texture " << textureName << " " << finalWidth << "x" << finalHeight << " for " << glyphCount << " glyphs");

		shared_ptr<Texture> texture = mTextureManager.CreateTexture(textureName, finalWidth, finalHeight, Texture::Formats::LUMINANCE8_ALPHA8);
		shared_ptr<u8> imageData = texture->GetBuffer();
		// Reset content (White, transparent)
		for (size_t i = 0; i < dataSize; i += pixelBytes)
		{
			imageData.get()[i + 0] = 0xFF; // luminance
			imageData.get()[i + 1] = 0x00; // alpha
		}

		size_t l = 0, m = 0;
		for (UTF32CodeRangeList::const_iterator r = codeRangeList.begin(); r != codeRangeList.end(); ++r)
		{
			const UTF32CodeRangeInclusive& range = *r;
			for(UTF32Code code = range.first; code <= range.second; ++code )
			{
				FT_Error ftResult;

				FT_UInt leftIndex = FT_Get_Char_Index( face, code );
				if(leftIndex==0)
				{
					ECHO_LOG_WARNING("FreeType2 found an undefined character code " << (UTF8String() += code) <<
								 ": font face " << face->family_name << " may not include this character set. ");
					continue;
				}

				// Load & render glyph
				ftResult = FT_Load_Char( face, code, FT_LOAD_RENDER );
				if (ftResult)
				{
					// Problem loading this glyph, continue
					ECHO_LOG_WARNING("Could not load glyph: " << code << " error: " << std::hex << ftResult << std::dec);
					continue;
				}

				FT_Int advance = face->glyph->advance.x >> 6;

				unsigned char* buffer = face->glyph->bitmap.buffer;

				if (!buffer)
				{
					// FreeType didn't detect this but generated a null pointer!
					continue;
				}

				int yBearing = ( maxBearingY >> 6 ) - ( face->glyph->metrics.horiBearingY >> 6 );
				int xBearing = face->glyph->metrics.horiBearingX >> 6;

				for(unsigned int j = 0; j < face->glyph->bitmap.rows; j++ )
				{
					size_t row = j + m + yBearing;
					u8* pDest = &(imageData.get()[(row * dataWidth) + (l + xBearing) * pixelBytes]);
					for(unsigned int k = 0; k < face->glyph->bitmap.width; k++ )
					{
						// Always white whether 'on' or 'off' pixel, since alpha will turn off
						*pDest++= 0xFF;
						// Always use the greyscale value for alpha
						*pDest++= *buffer++; 
					}
				}

				//Find the glyph we want to add. This should always find a glyph because we iterated over the same range above to create the map.
				std::map<UTF32Code, shared_ptr<Glyph> >::iterator mit = glyphMap.find(code);
				shared_ptr<Glyph> currentGlyph = mit->second;
				currentGlyph = mit->second;

				currentGlyph->mXBearing = xBearing;
				currentGlyph->mYBearing = ( face->glyph->metrics.horiBearingY >> 6 );
				currentGlyph->mTextureCoordinates.first = TextureUV(	(Real)(l + xBearing)/ (Real)finalWidth,  // u1
																(Real)(m + yBearing) / (Real)finalHeight);
				currentGlyph->mTextureCoordinates.second = TextureUV((Real)( l + xBearing + ( face->glyph->bitmap.width ) ) / (Real)finalWidth, // u2
																(Real)( m + yBearing + face->glyph->bitmap.rows ) / (Real)finalHeight); // v2
				
				fontObject.AddGlyph(currentGlyph);

				// Advance a column
				l += (advance + characterSpacer);

				// If at end of row
				if( finalWidth - 1 < l + ( advance ) )
				{
					m += maxHeight + characterSpacer;
					l = 0;
				}
			}
		}

		material->SetTexture(texture);

		FT_Done_FreeType(freeTypeLibrary);
		#else

		// We'll create a font, it just won't have any characters.
		fontObject.SetName(fontName);
		fontObject.SetMaterial(material);
		fontObject.SetForceUseReplacementCharacter(true);

		shared_ptr<Glyph> replacementGlyph = make_shared<Glyph>();
		replacementGlyph->mCode = Font::UNICODE_REPLACEMENT_CHARACTER;
		replacementGlyph->mTextureCoordinates.first = TextureUV(0,0);
		replacementGlyph->mTextureCoordinates.second = TextureUV(1,1);
		replacementGlyph->mWidth = 32;
		replacementGlyph->mHeight = 32;
		replacementGlyph->mXBearing = 0;
		replacementGlyph->mYBearing = 0;
		replacementGlyph->mAdvanceX = 32;
		replacementGlyph->mAdvanceY = 0;
		
		fontObject.AddGlyph(replacementGlyph);

		std::string textureName = fontName + " CharacterMap";
		ECHO_LOG_INFO("FontManager: Creating texture " << textureName << " " << replacementGlyph->mWidth << "x" << replacementGlyph->mHeight << " for " << 1 << " glyphs");

		shared_ptr<Texture> texture = mTextureManager.CreateTexture(textureName, replacementGlyph->mWidth, replacementGlyph->mHeight, Texture::Formats::LUMINANCE8_ALPHA8);
		Texture::Accessor<u16> imageData = texture->GetAccessor<u16>();
		// Reset content (White, transparent)
		for (Size i = 0; i < imageData.GetSize(); ++i)
		{
			// White with zero alpha.
			imageData[i] = 0x00FF;
		}

		// Draw a box
		Size lineOffset = 4;
		Size width = imageData.GetWidth();
		Size height = imageData.GetHeight();
		Size topLineStart = width*(lineOffset-1);
		Size bottomLastLineStart = imageData.GetSize() - (width*lineOffset);
		// Horizontal lines
		for (Size i = lineOffset-1; i < width-lineOffset; ++i)
		{
			imageData[topLineStart + i] = 0xFFFF;
			imageData[bottomLastLineStart + i] = 0xFFFF;
		}
		// Vertical lines
		for (Size i = lineOffset-1; i <= height-lineOffset; ++i)
		{
			imageData[i*width+(lineOffset-1)] = 0xFFFF;
			imageData[(width-lineOffset)+i*width] = 0xFFFF;
		}

		material->SetTexture(texture);
		#endif
		
		material->GetPass(0)->GetTextureUnit(0)->SetMinFilter(MaterialManager::ConvertTextureFilter(fontParameters.getTextureMinFilter(),mDefaultTextureFilter));
		material->GetPass(0)->GetTextureUnit(0)->SetMagFilter(MaterialManager::ConvertTextureFilter(fontParameters.getTextureMagFilter(),mDefaultTextureFilter));
		return true;
	}

	FileSystem* FontManager::GetFileSystem() const
	{
		return &mFileSystem;
	}
}
