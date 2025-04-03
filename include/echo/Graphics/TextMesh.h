#ifndef _ECHOTEXTMESH_H_
#define _ECHOTEXTMESH_H_

#include <echo/Graphics/SceneEntity.h>
#include <echo/Graphics/Font.h>
#include <echo/UTF8String.h>
#include <echo/Graphics/Mesh.h>
#include <echo/Maths/Vector2.h>
#include <echo/Graphics/ElementBuffer.h>

namespace Echo
{
	/**
	 * A Mesh specialisation that builds a text mesh in a given font.
	 */
	class TextMesh : public Mesh
	{
	public:
		/**
		 * Constructor
		 * @param content UTF8 content.
		 * @param fontManager The FontManager used to obtain and generate fonts.
		 * @note If you do not specify the font and content you will to use SetFont() and Set() to set the font and content later.
		 */
		TextMesh(UTF8String content = UTF8String(), shared_ptr<Font> font=shared_ptr<Font>());

		~TextMesh();
		
		TextMesh(const TextMesh& other);
		TextMesh& operator=(const TextMesh& rhs);
		
		/**
		 * Set the font.
		 * This marks the mesh as out of date which causes the mesh to be rebuilt on the next render (if UpdateMesh()
		 * has not already been called).
		 * @note You need to make sure the font contains all of the characters for your content otherwise those characters won't be rendered.
		 * @param font the Font object, use the FontManager to acquire a font resource.
		 */
		void SetFont(shared_ptr<Font> font);

		/**
		 * Get the font.
		 */
		shared_ptr<Font> GetFont() const {return mFont;}

		/**
		 * Set the texture filter method.
		 * This affects the way the texture is rendered:
		 * - TextureUnit::TextureFilters::NEAREST is used for a pixelated look
		 * - TextureUnit::TextureFilters::LINEAR is used for a softer look and scales better.
		 * TextMesh clones the Font material internally so this only affects the instance.
		 * @note The filter method is a property of the Mesh's Material which is initially obtained
		 * from the Font object. Calling SetFont() after this method will overwrite the property
		 * with the default from the Font (which is determined by the FontManager or resource
		 * configuration).
		 * @param filterMethod The filter method to use.
		 */
		void SetTextureFilterMethod(TextureUnit::TextureFilter filterMethod);

		/**
		 * Set the text content.
		 * @note You need to make sure the font contains all of the characters for your content otherwise those characters won't be rendered.
		 * @param content UTF8String to build a mesh for, You can use ASCII strings (see UTF8String).
		 */
		void Set(const UTF8String& content);
		
		/**
		 * Get the text.
		 */
		const UTF8String& GetText() const { return mString;}

		/**
		 * Append to the existing content content.
		 * @see Set();
		 */
		void Append(const UTF8String& content);
		
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
		Vector3 GetTextDimensions() const;
		
		/**
		 * A ConstructionResult is returned from BuildMesh().
		 */
		struct ConstructionResult
		{
			ConstructionResult(bool success) : mSuccess(success), mLineCount(0), mDimensions(Vector3::ZERO){}
			~ConstructionResult(){}
			ConstructionResult& operator=(const ConstructionResult& rhs)
			{
				mSuccess = rhs.mSuccess;
				mLineCount = rhs.mLineCount;
				mDimensions = rhs.mDimensions;
				return *this;
			}
			bool mSuccess;				/// Determine whether or not a mesh was built.
			size_t mLineCount;			/// The number of lines.
			Vector3 mDimensions;		/// The dimensions of the mesh.
		};

		/**
		 * Builds a text mesh into the specified mesh.
		 * The settings of the TextMesh are used to build a SubMesh into the one passed in.
		 * @note The SubMesh is not cleared before building. This is intentional to allow you to add to a SubMesh.
		 * @note The material of the SubMesh is not changed, make sure you set it to the correct Material.
		 * @param utf8String The string content to build.
		 * @param The mesh to build into. The mesh is not cleared before building.
		 * @return ConstructionResult.
		 */
		ConstructionResult BuildMesh(const UTF8String& utf8String, shared_ptr<SubMesh> subMesh) const;

		/**
		 * Clears the mesh and calls BuildMesh() passing in the internal content string.
		 */
		void UpdateMesh() const;
		 
		/**
		 * Override from Mesh.
		 */
		virtual const AxisAlignedBox& GetAxisAlignedBox() const;

		/**
		 * Override from Renderable.
		 * When called, will automatically update the mesh if the mesh is out of date.
		 */
		virtual void Render(RenderContext& renderContext, const Matrix4& world, const Matrix4& worldView, Colour compoundDiffuse);
		
		/**
		 * Set the colour of the text.
		 * @note This modifies the material.
		 * @param colour
		 */
		void SetColour(const Colour& colour);
		
		/**
		 * Get the colour of the text.
		 * @return the colour.
		 */
		const Colour& GetColour() const {return mColour;}
		
		/**
		 * Set the maximum width the text can be.
		 * The default value for this is 100, which is just arbitrary and intended to give
		 * some dimension so that you will see something rather than something scaled to 0.
		 * @see SetUseMaxWidth() for more information.
		 * @param maxWidth
		 */
		void SetMaxWidth(f32 maxWidth);
		
		/**
		 * Get the max width.
		 * @see SetUseMaxWidth() for more information.
		 * @return 
		 */
		f32 GetMaxWidth() const {return mMaxWidth;}
		
		/**
		 * Set whether or not to enforce max text width.
		 * This is enforced per word. If the word plus the current line length will exceed
		 * the max width the text will be moved to the next line. If a word is longer than
		 * the max width then scaling to the word will be applied to make it fit.
		 * @param useMaxWidth
		 */
		void SetUseMaxWidth(bool useMaxWidth);
		
		/**
		 * Get whether or not to use the max width option.
		 * @see SetUseMaxWidth() for more information on this feature.
		 * @return 
		 */
		bool GetUseMaxWidth() const {return mUseMaxWidth;}
		
		/**
		 * Set the text scale.
		 * This scale is applied to the text while building the mesh.
		 * @param textScale
		 */
		void SetTextScale(f32 textScale);

		/**
		 * Set the text render scale.
		 * This scale is applied right before rendering. There are multiple ways to apply a scale to a Text. Node has a
		 * scale but may not be desirable as it also affects child node scale and position. Using SetTextScale() will
		 * cause the mesh to be rebuilt. For Text that needs to change scale often (e.g. GUI::Text) you can use this
		 * scale which will be applied to the rendered mesh in addition to the Node's scale and build text scale.
		 * @param textRenderScale The text render scale.
		 */
		void SetTextRenderScale(const Vector2& textRenderScale);
		
		/**
		 * Get the text scale.
		 * @see SetTextScale();
		 * @return 
		 */
		f32 GetTextScale() const {return mTextScale;}
		
		/**
		 * Set the line spacing.
		 * The value passed in is a multiplier. The font's maximum character height is used
		 * to determine the line height.
		 * We use a multiplier because that is more useful than a specific value when you don't
		 * necessarily know what the font height is going to be.
		 * @note The default is 1.0
		 * @param lineSpacing value in font height multiples. e.g 1.0 or 1.5.
		 */
		void SetLineSpacing(f32 lineSpacing);

		/**
		 * Get the line spacing multiplier.
		 */
		f32 GetLineSpacing() const {return mLineSpacing;}
		
		/**
		 * Get the line spacing with respect to the font and line spacing.
		 * @returns line spacing * mFont->GetMaxHeight(), or 0 if the font has not been set.
		 */
		f32 GetAbsoluteLineSpacing() const;
		
		/**
		 * Sets whether or not this Text item should use the Unicode replacement
		 * character if a glyph can not be found in the font being used.
		 * @see   https://en.wikipedia.org/wiki/Specials_%28Unicode_block%29#Replacement_character
		 * @param useReplacementCharacterForGlyphFailure true for replacement character, else false.
		 */
		void SetUseReplacementCharacter(bool useReplacementCharacterForGlyphFailure)
		{
			mUseReplacementCharacterForGlyphFailure = useReplacementCharacterForGlyphFailure;
		}
		/**
		 * Gets whether or not this Text item should use the Unicode replacement
		 * character if a glyph can not be found in the font being used.
		 * @see   https://en.wikipedia.org/wiki/Specials_%28Unicode_block%29#Replacement_character
		 * @return true if the replacement character is to be used, false otherwise.
		 */
		bool GetUseReplacementCharacter(void) const
		{
			return mUseReplacementCharacterForGlyphFailure;
		}
		
		/**
		 * Set whether or not to add mesh extent vertices.
		 * Normally the AABB is only as big as the vertices making up the characters. When this is set
		 * to true the mesh will include extra vertices at the extents of the mesh. This will create
		 * a consistent height mesh for all text objects with the same number of lines. The width
		 * will be fixed when use max width is enabled.
		 * @param addMeshExtentVertices true to add extra vertices at the extents of the mesh.
		 */
		void SetAddMeshExtentVertices(bool addMeshExtentVertices);
		
		/**
		 * Get whether or not the object will add extra vertices when building the mesh.
		 * @see SetAddMeshExtentVertices().
		 */
		bool GetAddMeshExtentVertices() const {return mMeshExtentVertices;}

		size_t GetLineCount();
		
		shared_ptr<TextMesh> Clone() const;
	private:
		shared_ptr<Font> mFont;
		UTF8String mString;
		Vector2 mTextRenderScale;
		f32 mSpaceWidth;
		f32 mTextScale;
		f32 mMaxWidth;
		f32 mLineSpacing;
		bool mUseMaxWidth;
		mutable Mutex mMeshMutex;
		mutable bool mMeshOutOfDate;
		bool mUseReplacementCharacterForGlyphFailure;
		bool mTextRenderScaleTransformOutOfDate;
		bool mUseRenderScaleTransform;
		bool mMeshExtentVertices;
		mutable size_t mLineCount;
		Matrix4 mTextRenderScaleTransform;
		Colour mColour;

		shared_ptr<VertexBuffer> mVertexBuffer;
		mutable shared_ptr<ElementBuffer> mElementBuffer;
		mutable VertexBuffer::Accessor<Vector3> mVertices;
		mutable VertexBuffer::Accessor<Vector3> mNormals;
		mutable VertexBuffer::Accessor<TextureUV> mTexutreCoordinates;
		mutable ElementBuffer::Accessor< ElementBuffer::Triangle<u16> > mIndices;
		
		void AddWordToMesh(shared_ptr<SubMesh> subMesh, Vector2 position, const UTF8String& word, f32 scale) const;
		void AddCharacterToMesh(shared_ptr<SubMesh> subMesh, const Vector2& position, const Glyph& glyph, f32 scale) const;
		void AddExtentsVertices(shared_ptr<SubMesh> subMesh, Size lineCount) const;
	};
}
#endif
