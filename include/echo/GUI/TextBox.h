#ifndef _ECHOTEXTBOXELEMENT_H_
#define _ECHOTEXTBOXELEMENT_H_

#include <echo/GUI/Container.h>
#include <echo/GUI/Text.h>

namespace Echo
{
	class FontManager;
	namespace GUI
	{
		/**
		 * A box that contains text.
		 * Whats the difference between Text and a TextBox?
		 * 
		 * They are both very similar and TextBox actually contains a Text element. It configures the Text object
		 * to wrap within the TextBox bounds and position with the text aligned to the top left of the box.
		 * 
		 * TextBox specifies the Text height as a percentage of the TextBox's height.
		 */
		class TextBox : public Container
		{
		public:
			TextBox(const UTF8String& content, shared_ptr<FontManager> fontManager);
			TextBox(shared_ptr<FontManager> fontManager);
			virtual ~TextBox();

			/**
			 * Set the font by name and point size.
			 * @see Text::SetFont()
			 */
			void SetFont(const std::string& fontName, Scalar pointSize=0);

			/**
			 * Set the font size in point size.
			 * @see Text::SetFontSize()
			 */
			void SetFontSize(Scalar pointSize);
			
			/**
			 * Set the texture filter method.
			 * @see TextMesh::SetTextureFilterMethod()
			 */
			void SetTextureFilterMethod(TextureUnit::TextureFilter filterMethod);
			
			/**
			 * Set the text box contents
			 * @param text The text contents.
			 */
			void SetText(const UTF8String& text);
			
			/**
			 * Get the text box contents.
			 */
			const UTF8String& GetText() const;
			
			/**
			 * Set the line size.
			 * The line size follows the rules of element sizing. It is a percent value of
			 * the parent of if this is the top most element it is the absolute size.
			 * We're using 'size' instead of 'height' to be more general for when this class
			 * supports vertical text.
			 */
			void SetLineSize(Scalar lineSize);
			
			/**
			 * Overrides to record the original height.
			 */
			virtual void SetSize(Scalar width, Scalar height) override;	
			virtual void SetHeight(Scalar height) override;

			/**
			 * Get the original height of the TextBox before being resized to fit its content.
			 * Since the text contents of a TextBox can cause the TextBox to increase in size
			 * the original height is preserved so it can be used for resize calculations.
			 * This method returns the last height specified with SetHeight().
			 * @return The original height before resizing to fit content.
			 */
			Scalar GetOriginalHeight() const {return mOriginalHeight;}
		private:
			virtual void UpdateSize(Scalar newWidth, Scalar newHeight) override;
			void Initialise();
			void UpdateTextMesh();
			shared_ptr<Text> mText;
			shared_ptr<FontManager> mFontManager;
			Scalar mLineSize;
			Scalar mCurrentLineSize;
			Scalar mAspectRatio;
			Scalar mOriginalHeight;
			bool mUpdatingForTextAdjustment;
		};
	}
}
#endif 
