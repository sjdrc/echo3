#ifndef _ECHOTEXTELEMENT_H_
#define _ECHOTEXTELEMENT_H_

#include <echo/GUI/Element.h>
#include <echo/Graphics/TextMesh.h>
#include <echo/Util/StringUtils.h>

namespace Echo
{
	class FontManager;
	namespace GUI
	{
		/**
		 * Wraps a TextMesh as a GUI Element.
		 * @note A Text GUI Element is not targetable by default. This is because text is often used
		 * as a label over the top of a button and if it was targetable the Text Element would
		 * receive and process cursor events rather than the Button, which is more likely to have
		 * callbacks set for the events.
		 * @see TextMesh
		 * @see Element
		 */
		class Text : public Element
		{
		public:
			Text(const UTF8String& content, shared_ptr<FontManager> fontManager);
			virtual ~Text();

			Text(const Text& other);
			Text& operator=(const Text& rhs);

			/**
			 * Set the font by name and point size.
			 * This overrides some internal settings to make the Text object appear in this
			 * point size relative to the default DPI setup in the FontManager (this will probably
			 * be the primary render target if rendering to the screen).
			 * The method will have the following effect:
			 *	- The Element will have its size set in pixels.
			 *	- The GetUsePointSize() will return true to indicate that point-sizing is enabled.
			 *  - The FontManager will be queried for the correct character map and as such
			 * calling this method might result in a new Font resource being generated.
			 * @param fontName The font name, resource should exist in the passed FontManager, if
			 * the resource doesn't exist the method will return false and not change the object.
			 * @param pointSize The point size of the font, if 0 the resource manager
			 * default will be used, if >0 SetFontSize() will be called following this call
			 */
			bool SetFont(const std::string& fontName, Scalar pointSize=0);

			/**
			 * Set the font size in point size.
			 * This method uses the font that was previously used with SetFont() to adjust the
			 * point size.
			 * @note This method depends on the font being set using SetFont() first in order to
			 * have a valid name. If the method is called without having a font set first then
			 * this method has no effect.
			 */
			void SetFontSize(Scalar pointSize);
			
			/**
			 * Set the texture filter method.
			 * @see TextMesh::SetTextureFilterMethod()
			 */
			void SetTextureFilterMethod(TextureUnit::TextureFilter filterMethod);
			
			/**
			 * Element overrides to block size setting if the text size has been set in point.
			 * To set the size using these methods, disable point size mode with
			 * DisablePointSizing().
			 */
			virtual void SetSize(Scalar width, Scalar height) override;
			virtual void SetPhysicalSize(Metre width, Metre height) override;
			virtual void SetSizeInPixels(Scalar width, Scalar height) override;
			virtual void SetWidth(Scalar width) override;
			virtual void SetPhysicalWidth(Metre width) override;
			virtual void SetWidthInPixels(Scalar width) override;
			virtual void SetHeight(Scalar height) override;
			virtual void SetPhysicalHeight(Metre height) override;
			virtual void SetHeightInPixels(Scalar height) override;
			
			/**
			 * Get whether point-size sizing is being used.
			 * @return true if point-size sizing is being used, otherwise false.
			 */
			bool GetUsePointSize() const {return mUsePointSize;}
			
			/**
			 * Disables point sizing.
			 * If the font size was specified in points the usual Element sizing rules will not apply.
			 * Calling this method turns off point sizing so the size can be specified relative to the
			 * parent, or a physical size.
			 * Trying to set the size through the usual Element size methods will not work if you do
			 * not call this method first since when point sizing is enabled standard sizing is disabled
			 * in order to make the font display correctly.
			 */
			void DisablePointSizing();

			// See TextMesh methods of the same names for additional details.
			void SetText(const UTF8String& content);

			template< typename T >
			void SetText(const T& v)
			{
				optional<std::string> asString = Utils::String::ToString<T>(v);
				if(asString)
				{
					SetText(UTF8String(*asString));
				}else
				{
					SetText("Conversion Failure");
				}
			}
			const UTF8String& GetText() const;
			void Append(const UTF8String& content);
			void SetColour(const Colour& colour);
			const Colour& GetColour() const;
			
			/**
			 * Set the maximum width of the text.
			 * This will cause the text to spread over multiple lines.
			 * The units in which to specify depend on the font size settings:
			 * - If point-size sizing is enabled then this method should be passed in the maximum width
			 * in pixels since the TextMesh is built as a 1:1 pixel ratio (scaled to world by Text).
			 * - If relative sizing is used then this method should take in a relative unit, for
			 * example a size of 1 would mean the Text will be no wider than the parent element.
			 * - Similarly when using absolute sizing, this method should be in absolute terms.
			 */
			void SetMaxWidth(f32 maxWidth);
			f32 GetMaxWidth() const;
			void SetUseMaxWidth(bool useMaxWidth);
			bool GetUseMaxWidth() const;

			/**
			 * Set the scale factor of the text to directly affect the size of the built mesh (i.e.
			 * no post build scaling is required).
			 * @param textScale The scale factor of the text.
			 */
			void SetTextScale(f32 textScale);
			f32 GetTextScale() const;
			void SetLineSpacing(f32 lineSpacing);
			f32 GetLineSpacing() const;
			f32 GetAbsoluteLineSpacing() const;
			void SetUseReplacementCharacter(bool useReplacementCharacterForGlyphFailure);
			bool GetUseReplacementCharacter(void) const;
			void SetAddMeshExtentVertices(bool addMeshExtentVertices);
			bool GetAddMeshExtentVertices() const;
			size_t GetLineCount();

			virtual Scalar GetNativeAspectRatio() override;
			shared_ptr<Font> GetFont() const;

			/**
			 * Get the size of the built TextMesh.
			 * The units that this returns are usually in terms of pixels relative to the font
			 * bitmap data (i.e. not necessarily pixels on screen). If a text scale is a applied then
			 * the units are with the applied scale.
			 * @return The size of the text mesh.
			 */
			Vector3 GetTextMeshSize();
		private:
			virtual shared_ptr<Element> _Clone() const override;
			virtual void UpdateSize(Scalar newWidth, Scalar newHeight) override;
			void AdjustForPointSizing();
			void _SetFontSize(Scalar pointSize);

			/**
			 * Set whether or not to use point sizing or normal sizing.
			 * @note This method will be called within SetFont() after the resource has been acquired or
			 * generated through the FontManager.
			 * @param usePointSize true if sizing should be defined by font point size or false if not,
			 * if the current internal point size is not greater than 0 and the parameter is true this
			 * method not enable point sizing since a point size of 0 is not possible.
			 */
			void SetUsePointSize(bool usePointSize);

			shared_ptr<TextMesh> mTextMesh;			/// The visual representation of the text, it is always built
													/// in terms of pixels (governed by Font parameters) and scaled
													/// by Text as needed to display in the appropriate size.
			shared_ptr<FontManager> mFontManager;	/// The FontManager used to acquire and generate Font character maps.
			bool mUsePointSize;		/// Flag to track whether or not point sizing is enabled.
			Scalar mPointSize;		/// Only valid when the Text object has been sized with a point size.
			std::string mFontName;
		};
	}
}
#endif 
