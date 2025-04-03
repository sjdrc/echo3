#include <echo/GUI/TextBox.h>
#include <echo/Graphics/TextMesh.h>

namespace Echo
{
	namespace GUI
	{
		TextBox::TextBox(const UTF8String& content, shared_ptr<FontManager> fontManager) : mFontManager(fontManager)
		{
			Initialise();
			SetText(content);
		}

		TextBox::TextBox(shared_ptr<FontManager> fontManager) : mFontManager(fontManager)
		{
			Initialise();
		}

		TextBox::~TextBox()
		{

		}

		void TextBox::Initialise()
		{
			SetTargetable(true);
			
			mLineSize = 0.f;	//Start with a zero size line size so when we initialise it to 1 it performs the change actions.
			mAspectRatio = 1.f;
			mOriginalHeight = 1.f;
			mUpdatingForTextAdjustment = false;;
			
			mText.reset(new Text(UTF8String(), mFontManager));
			//Align to the top left. We'll support other directions and alignments later.
			mText->SetHorizontalAlignment(Alignments::MINIMUM);
			mText->SetVerticalAlignment(Alignments::MAXIMUM);
			mText->SetRelativePosition(Vector3(0.,1.,0.));
			
			//Use the maximum width so the text will wrap.
			mText->SetUseMaxWidth(true);
			mText->SetMaxWidth(GetAspectRatio());
			
			//Enable mesh extent points so scaling is easier
			mText->SetAddMeshExtentVertices(true);
			
			// For horizontal text we'll know the width because it will be fixed, The height will depend on
			// the number of lines so we'll calculate it.
			mText->SetSizeMode(SizeModes::WIDTH_AND_CALCULATE_ASPECT_RATIO);

			// The TextBox is made targetable but the text itself isn't.
			mText->SetTargetable(false);
			
			//Set the line size to 1.
			SetLineSize(1.f);
			AddElement(mText);
		}
		
		void TextBox::SetFont(const std::string& fontName, Scalar pointSize)
		{
			mText->SetFont(fontName, pointSize);
			UpdateTextMesh();
		}

		void TextBox::SetFontSize(Scalar pointSize)
		{
			mText->SetFontSize(pointSize);
		}
		
		void TextBox::SetTextureFilterMethod(TextureUnit::TextureFilter filterMethod)
		{
			mText->SetTextureFilterMethod(filterMethod);
		}
		
		void TextBox::SetText(const UTF8String& text)
		{
			mText->SetText(text);
			UpdateTextMesh();
		}
		
		const UTF8String& TextBox::GetText() const
		{
			return mText->GetText();
		}
		
		void TextBox::SetLineSize(Scalar lineSize)
		{
			if(mLineSize!=lineSize)
			{
				mLineSize = lineSize;
				mCurrentLineSize = mLineSize;
				shared_ptr<Font> font=mText->GetFont();
				if(font)
				{
					Scalar heightScaledToOne = mLineSize/font->GetMaxHeight();
					mText->SetTextScale(heightScaledToOne);
					UpdateTextMesh();
				}
			}
		}
		
		void TextBox::UpdateTextMesh()
		{
			shared_ptr<Font> font=mText->GetFont();
			if(mUpdatingForTextAdjustment || !font)
			{
				return;
			}
			
			mUpdatingForTextAdjustment = true;

			if(mText->GetUsePointSize())
			{
				//Build the text mesh using the specified line height.
				mCurrentLineSize = mLineSize;
				if(mText->GetWorldUnitsConverter())
				{
					// The maximum width is the absolute width of the text box converted to pixels since the units on
					// the mesh are pixels when Text is in point mode.
					mText->SetMaxWidth(GetAbsoluteWidth()/mText->GetWorldUnitsConverter()->GetUnitsPerPixelX());
					
					//New height of the TextBox is calculated as the relative size of the parent.
					if(GetParentElement())
					{
						Scalar parentHeightInPixels = (GetParentElement()->GetAbsoluteHeight()/mText->GetWorldUnitsConverter()->GetUnitsPerPixelY());
						if(parentHeightInPixels>0)
						{
							Scalar newHeight =  mText->GetTextMeshSize().y / parentHeightInPixels;
							SetHeight(newHeight);
						}else
						{
							SetHeight(0);
						}
					}else
					{
						SetHeight(mText->GetTextMeshSize().y);
					}
				}
			}else
			{
				// Set the height of the textbox to the originally specified height as a starting
				// point if we don't do this then the box will never shrink.
				SetHeight(mOriginalHeight);
				
				//Build the text mesh using the specified line height.
				mCurrentLineSize = mLineSize;
				mText->SetTextScale( mCurrentLineSize/font->GetMaxHeight() );
				mText->SetMaxWidth(GetAspectRatio());

				//Check if the height exceeds the parent.
				Scalar currentHeight = mText->GetLineCount()*mCurrentLineSize;
				if(currentHeight > 1.f)
				{
					//Adjust the line size by the inverse of currentHeight (which will be a ratio)
					mCurrentLineSize /= currentHeight;

					mText->SetMaxWidth(GetAspectRatio());

					//Adjust the text box to fit the text
					SetHeight(mOriginalHeight * currentHeight);
				}
			}
			mUpdatingForTextAdjustment = false;
		}
		
		void TextBox::SetSize(Scalar width, Scalar height)
		{
			if(!mUpdatingForTextAdjustment)
			{
				mOriginalHeight = height;
			}
			Element::SetSize(width, height);
		}

		void TextBox::SetHeight(Scalar height)
		{
			if(!mUpdatingForTextAdjustment)
			{
				mOriginalHeight = height;
			}
			Element::SetHeight(height);
		}
		
		void TextBox::UpdateSize(Scalar newWidth, Scalar newHeight)
		{
			if(newHeight==0.f)
			{
				return;
			}
			Scalar newAspectRatio = newWidth/newHeight;
			if(mAspectRatio==newAspectRatio || newAspectRatio==0.f)
			{
				return;

			}
			//Record the aspect ratio to avoid rebuilding if they are the same.
			mAspectRatio = newAspectRatio;
			UpdateTextMesh();
			Container::UpdateSize(newWidth,newHeight);
		}
	}
}
