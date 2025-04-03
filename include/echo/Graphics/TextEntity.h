#ifndef _ECHOTEXTENTITY_H_
#define _ECHOTEXTENTITY_H_

#include <echo/Graphics/SceneEntity.h>
#include <echo/Graphics/TextMesh.h>

namespace Echo
{
	/**
	 * TextEntity is a SceneEntity that manages a TextMesh as its mesh.
	 * This class is just a helper class so you don't have to always create a TextMesh and SceneEntity together.
	 */
	class TextEntity : public SceneEntity
	{
	public:
		/**
		 * Default constructor.
		 * You need to use SetFont() and Set() to set the font and content respectively.
		 */
		TextEntity()
		{
			mTextMesh.reset(new TextMesh());
			SetMesh(mTextMesh);
		}

		/**
		 * Constructor
		 * @param content UTF8 content.
		 * @param font The font to use.
		 */
		TextEntity(const UTF8String& content, shared_ptr<Font> font)
		{
			mTextMesh.reset(new TextMesh(content, font));
			SetMesh(mTextMesh);
		}

		~TextEntity()
		{
		}

		/**
		 * Set the font.
		 * This marks the mesh as out of date which causes the mesh to be rebuilt on the next render (if UpdateMesh()
		 * has not already been called).
		 * @note You need to make sure the font contains all of the characters for your content otherwise those characters won't be rendered.
		 * @param font the Font object, use the FontManager to acquire a font resource.
		 */
		void SetFont(shared_ptr<Font> font)
		{
			mTextMesh->SetFont(font);
			NeedUpdate(true);	//Force parent because the AABB to will need updating.
		}

		/**
		 * Set the text content.
		 * @note You need to make sure the font contains all of the characters for your content otherwise those characters won't be rendered.
		 * @param content UTF8String to build a mesh for, You can use ASCII strings (see UTF8String).
		 */
		void Set(const UTF8String& content)
		{
			mTextMesh->Set(content);
			NeedUpdate(true);	//Force parent because the AABB to will need updating.
		}

		/**
		 * Append to the existing content content.
		 * @see Set();
		 */
		void Append(const UTF8String& content)
		{
			mTextMesh->Append(content);
			NeedUpdate(true);	//Force parent because the AABB to will need updating.
		}
		
		/**
		 * Get the dimensions of the text mesh.
		 * The dimensions returned are the actual mesh size determined when it is built.
		 * The X component is the width.
		 * The Y component is the height.
		 * The Z component will be 0.
		 * @note If the text is out of date and the mesh needs to be rebuilt then calling
		 * this method will trigger a mesh rebuild.
		 * @return The dimensions of the text's mesh.
		 */
		Vector3 GetTextDimensions() const
		{
			return mTextMesh->GetTextDimensions();
		}
		
		/**
		 * Calls BuildMesh() passing in the internal content string.
		 */
		void UpdateMesh()
		{
			mTextMesh->UpdateMesh();
		}
		
		/**
		 * Set the colour of the text.
		 * @note This modifies the material.
		 * @param colour
		 */
		void SetColour(const Colour& colour)
		{
			mTextMesh->SetColour(colour);
		}
		
		/**
		 * Get the colour of the text.
		 * @return the colour.
		 */
		const Colour& GetColour() const {return mTextMesh->GetColour();}
		
		/**
		 * Set the maximum width the text can be.
		 * The default value for this is 100, which is just arbitrary and intended to give
		 * some dimension so that you will see something rather than something scaled to 0.
		 * @see SetUseMaxWidth() for more information.
		 * @param maxWidth
		 */
		void SetMaxWidth(f32 maxWidth)
		{
			mTextMesh->SetMaxWidth(maxWidth);
			NeedUpdate(true);	//Force parent because the AABB to will need updating.
		}
		
		/**
		 * Get the max width.
		 * @see SetUseMaxWidth() for more information.
		 * @return 
		 */
		f32 GetMaxWidth() const {return mTextMesh->GetMaxWidth();}
		
		/**
		 * Set whether or not to enforce max text width.
		 * This is enforced per word. If the word plus the current line length will exceed
		 * the max width the text will be moved to the next line. If a word is longer than
		 * the max width then scaling to the word will be applied to make it fit.
		 * @param useMaxWidth
		 */
		void SetUseMaxWidth(bool useMaxWidth)
		{
			mTextMesh->SetUseMaxWidth(useMaxWidth);
			NeedUpdate(true);	//Force parent because the AABB to will need updating.
		}
		
		/**
		 * Get whether or not to use the max width option.
		 * @see SetUseMaxWidth() for more information on this feature.
		 * @return 
		 */
		bool GetUseMaxWidth() const {return mTextMesh->GetUseMaxWidth();}
		
		/**
		 * Set the text scale.
		 * This scale is applied to the text while building the mesh.
		 * @param textScale
		 */
		void SetTextScale(f32 textScale)
		{
			mTextMesh->SetTextScale(textScale);
			NeedUpdate(true);	//Force parent because the AABB to will need updating.
		}
		
		/**
		 * Get the text scale.
		 * @see SetTextScale();
		 * @return 
		 */
		f32 GetTextScale() const {return mTextMesh->GetTextScale();}
		
		/**
		 * Set the fixed line spacing.
		 * @see TextMesh::SetLineSpacing().
		 */
		void SetLineSpacing(f32 fixedLineSpacing)
		{
			mTextMesh->SetLineSpacing(fixedLineSpacing);
			NeedUpdate(true);	//Force parent because the AABB to will need updating.
		}

		/**
		 * Get the line spacing.
		 * @see TextMesh::GetLineSpacing().
		 */
		f32 GetLineSpacing() const {return mTextMesh->GetLineSpacing();}

		/**
		 * Get the line spacing.
		 * @see TextMesh::GetAbsoluteLineSpacing().
		 */
		f32 GetAbsoluteLineSpacing() const {return mTextMesh->GetAbsoluteLineSpacing();}

		/**
		 * Sets whether or not this Text item should use the Unicode replacement
		 * character if a glyph can not be found in the font being used.
		 * @see   https://en.wikipedia.org/wiki/Specials_%28Unicode_block%29#Replacement_character
		 * @param useReplacementCharacterForGlyphFailure true for replacement character, else false.
		 */
		void SetUseReplacementCharacter(bool useReplacementCharacterForGlyphFailure)
		{
			mTextMesh->SetUseReplacementCharacter(useReplacementCharacterForGlyphFailure);
		}
		/**
		 * Gets whether or not this Text item should use the Unicode replacement
		 * character if a glyph can not be found in the font being used.
		 * @see   https://en.wikipedia.org/wiki/Specials_%28Unicode_block%29#Replacement_character
		 * @return true if the replacement character is to be used, false otherwise.
		 */
		bool GetUseReplacementCharacter(void) const
		{
			return mTextMesh->GetUseReplacementCharacter();
		}
		
	private:
		shared_ptr<TextMesh> mTextMesh;
	};
}
#endif
