#ifndef _ECHOFONT_H_
#define _ECHOFONT_H_

#include <echo/Types.h>
#include <echo/Graphics/PrimitiveTypes.h>
#include <echo/Resource/Resource.h>
#include <echo/UTF8String.h>
#include <map>
#include <algorithm>
#include <limits>

namespace Echo
{
	class Texture;
	class Material;

	/**
	 * A Glyph contains the information about a Unicode code point to be used for
	 * displaying the character.
	 */
	struct Glyph
	{
		UTF32Code mCode;					/// The Unicode code point this Glyph corresponds to.
		TextureUVPair mTextureCoordinates;	/// defines the portion of the Texture to use.
		u32 mWidth;							/// The width of the glyph in pixels.
		u32 mHeight;						/// The height of the glyph in pixels.
		s32 mXBearing;						/// An X adjust from a given cursor position in pixels.
		s32 mYBearing;						/// A Y adjust from a given cursor position in pixels.
		u32 mAdvanceX;						/// How much to advance the cursor (to the next character) in pixels.
											/// This value should be used by Horizontal fonts.
		u32 mAdvanceY;						/// How much to advance the cursor (to the next character) in pixels.
											/// This value should be used for vertical fonts.
		std::map< UTF32Code, s32 > mKerning;
		s32 GetKerning(UTF32Code nextCharacter) const
		{
			std::map< UTF32Code, s32 >::const_iterator it = mKerning.find(nextCharacter);
			if(it!=mKerning.end())
			{
				return it->second;
			}
			return 0;
		}
	};
	
	typedef std::map< UTF32Code, shared_ptr<Glyph> > GlyphMap;
	
	class Font : public Resource<Font>
	{
	public:
		static const UTF32Code UNICODE_REPLACEMENT_CHARACTER = 0xFFFD;
		
		Font() : Resource<Font>(true),
			mMaxHeight(std::numeric_limits<u32>::min()),
			mMaxWidth(std::numeric_limits<u32>::min()),
			mMinHeight(std::numeric_limits<u32>::max()),
			mMinWidth(std::numeric_limits<u32>::max()),
			mHeightBetweenTwoBaseLines(1),
			mReplacementCharacter(UNICODE_REPLACEMENT_CHARACTER),
			mForceUseReplacementCharacter(false),
			mWarnUseOfReplacementCharacter(false)
		{
		}
		
		~Font()
		{
		}
		
		/**
		 * Get glyph information for a particular Unicode character code point.
		 * The Glyph contains information about the code points and where it exist on the
		 * Texture.
		 * @see Glyph.
		 * @param code The code point you want to get (You can use ASCII values up to 127 here
		 * since Unicode is backwards compatible).
		 * @param useReplacementCharacterOnFailure If true the Font will return the replacement
		 * character if the character isn't found. This may be overridden by the font.
		 * @see SetWarnUseOfReplacementCharacter() for information on warnings.
		 * @see SetForceUseReplacementCharacter() for information on overriding
		 * useReplacementCharacterOnFailure.
		 * @return If the code point has a corresponding Glyph a Glyph will be returned,
		 * otherwise a null pointer.
		 */
		shared_ptr<Glyph> GetGlyph(UTF32Code code, bool useReplacementCharacterOnFailure = true) const
		{
			GlyphMap::const_iterator it = mGlyphs.find(code);
			if (it != mGlyphs.end() && it->second)
			{
				return it->second;
			}
			if(mWarnUseOfReplacementCharacter)
			{
				ECHO_LOG_WARNING("No glyph found for " << (UTF8String() += code) << " (" << code << ")");
			}
			if (useReplacementCharacterOnFailure || mForceUseReplacementCharacter)
			{
				// The font might contain the replacement character to show errors:
				it = mGlyphs.find(mReplacementCharacter);
				if (it != mGlyphs.end() && it->second)
				{
					if(mWarnUseOfReplacementCharacter)
					{
						ECHO_LOG_WARNING("Replacement character used for " << (UTF8String() += code) << " (" << code << ")");
					}
					return it->second;
				}
				if(mWarnUseOfReplacementCharacter)
				{
					ECHO_LOG_WARNING("Replacement character not found. " << (UTF8String() += code) << " (" << code << ") doesn't have a Glyph.");
				}
				return nullptr;
			}
			return nullptr;
		}
	
		/**
		 * Add a glyph to the font.
		 * Unless you're building your own font, you won't need to use this method.
		 * The glyph corresponds to a portion of the Texture that Material references.
		 * @param glyph The glyph to add.
		 */
		void AddGlyph(shared_ptr<Glyph> glyph)
		{
			mGlyphs[glyph->mCode] = glyph;
			mMaxHeight = std::max(mMaxHeight,glyph->mHeight);
			mMaxWidth = std::max(mMaxWidth,glyph->mWidth);
			mMinHeight = std::min(mMinHeight,glyph->mHeight);
			mMinWidth = std::min(mMinWidth,glyph->mWidth);
		}
		
		/**
		 * Set the font material.
		 * Typically you do not need to modify the font material. Doing so might change the
		 * Texture which will affect how the font maps. This method is for internal and
		 * advanced usage.
		 * @param material The new material.
		 */
		void SetMaterial(shared_ptr<Material> material)
		{
			mMaterial = material;
		}
		
		/**
		 * Get the font material.
		 * The material defines the render settings of the Font. By default, the font material
		 * includes a Texture that the Glyphs map to.
		 * @return The font's material.
		 */
		shared_ptr<Material> GetMaterial() const
		{
			return mMaterial;
		}
		
		/**
		 * Get the maximum character height of the font in pixels.
		 * @return The maximum height of a character in pixels.
		 */
		const u32& GetMaxHeight() const {return mMaxHeight;}

		/**
		 * Get the maximum character width of the font in pixels.
		 * @return The maximum width of a character in pixels.
		 */
		const u32& GetMaxWidth() const {return mMaxWidth;}

		/**
		 * Get the minimum character height of the font in pixels.
		 * @return The minimum height of a character in pixels.
		 */
		const u32& GetMinHeight() const {return mMinHeight;}

		/**
		 * Get the minimum character width of the font in pixels.
		 * @return The minimum width of a character in pixels.
		 */
		const u32& GetMinWidth() const {return mMinWidth;}

		/**
		 * Set the height between two base lines in pixels.
		 * @param heightBetweenTwoBaseLines in pixels..
		 */
		void SetHeightBetweenTwoBaseLines(u32 heightBetweenTwoBaseLines)
		{
			mHeightBetweenTwoBaseLines=heightBetweenTwoBaseLines;
		}

		/**
		 * Get the height between two base lines in pixels.
		 * @return The height between two base lines in pixels.
		 */
		const u32& GetHeightBetweenTwoBaseLines() const {return mHeightBetweenTwoBaseLines;}

		/**
		 * Set the name of this font.
		 * @param name The font name.
		 */
		void SetName(const std::string& name){mName=name;}

		/**
		 * Get the name of the font.
		 * The font name isn't derived from the font data file, it is obtained from the .font meta
		 * data or Font list file.
		 * @return the font name.
		 */
		const std::string& GetName() const {return mName;}

		/**
		 * Set the replacement character to use when a character isn't found.
		 * The default for this value is the Unicode default (@see UNICODE_REPLACEMENT_CHARACTER).
		 * You can use this method to override what the replacement character is.
		 * @param replacementCharacter The new replacement character.
		 */
		void SetReplacementCharacter(UTF32Code replacementCharacter) {mReplacementCharacter=replacementCharacter;}
		
		/**
		 * Get the replacement character that this Font will use if a character isn't found.
		 * @see SetReplacementCharacter()
		 * @return The replacement character.
		 */
		UTF32Code GetReplacementCharacter() const {return mReplacementCharacter;}
		
		/**
		 * Set whether or not to override attempting to use the replacement character.
		 * The replacement character is used if a character is not found. This overrides the
		 * request made with GetGlyph().
		 * @param force Set to true to force replacement character usage.
		 */
		void SetForceUseReplacementCharacter(bool force) {mForceUseReplacementCharacter=force;}

		/**
		 * Get whether or not the replacement character will always be used when a character isn't found.
		 * @see SetForceUseReplacementCharacter()
		 * @return bool
		 */
		bool GetForceUseReplacementCharacter() const {return mForceUseReplacementCharacter;}
		
		/**
		 * Set whether or not to warn when the replacement character is used.
		 * This will a message when the requested character is not found. Turning this on will
		 * cause a message to be output if the character isn't found and use of a replacement
		 * character is off.
		 * @param warn true to display warning messages, otherwise false.
		 */
		void SetWarnUseOfReplacementCharacter(bool warn) {mWarnUseOfReplacementCharacter=warn;}

		/**
		 * Get whether or not warning messages will display on not found characters.
		 * @see SetWarnUseOfReplacementCharacter().
		 */
		bool GetWarnUseOfReplacementCharacter() const {return mWarnUseOfReplacementCharacter;}
	private:
		virtual bool _Unload() override
		{
			return false;
		}
		virtual Size OnRequestMemoryRelease() override
		{
			return 0;
		}
	
		std::string mName;
		GlyphMap mGlyphs;
		shared_ptr<Material> mMaterial;
		u32 mMaxHeight;
		u32 mMaxWidth;
		u32 mMinHeight;
		u32 mMinWidth;
		u32 mHeightBetweenTwoBaseLines;
		UTF32Code mReplacementCharacter;
		bool mForceUseReplacementCharacter;
		bool mWarnUseOfReplacementCharacter;
	};
}
#endif
