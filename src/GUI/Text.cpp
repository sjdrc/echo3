#include <echo/GUI/Text.h>
#include <echo/Graphics/TextMesh.h>
#include <echo/Resource/FontManager.h>
#include <echo/Graphics/SubMesh.h>

namespace Echo
{
	namespace GUI
	{
		Text::Text(const UTF8String& content, shared_ptr<FontManager> fontManager)
			: mFontManager(fontManager),
			mUsePointSize(false),
			mPointSize(0.f)
		{
			assert(mFontManager && "fontManager cannot be null");
			//By default Text isn't targetable, see class documentation.
			SetTargetable(false);
			mTextMesh.reset(new TextMesh(content, shared_ptr<Font>()));
			SetMesh(mTextMesh);
		}

		Text::~Text()
		{

		}

		Text::Text(const Text& other)
		{
			*this=other;
		}
		
		Text& Text::operator=(const Text& rhs)
		{
			mTextMesh = rhs.mTextMesh->Clone();
			SetMesh(mTextMesh);
			Element::operator=(rhs);		//This will preserve the mesh for us.
			AdjustForPointSizing();
			SizeUpdated();
			PositionUpdated();
			return *this;
		}
		
		shared_ptr<Element> Text::_Clone() const
		{
			return shared_ptr<Element>(new Text(*this));
		}

		bool Text::SetFont(const std::string& fontName, Scalar pointSize)
		{
			shared_ptr<Font> font;
			if(pointSize==0.)
			{
				font = mFontManager->GetResource(fontName);
				if(!font)
				{
					ECHO_LOG_WARNING("The font named " << fontName << " does not exist.");
					return false;
				}
				mTextMesh->SetFont(font);
				UpdateSize(GetAbsoluteWidth(), GetAbsoluteHeight());
			}else
			{
				font = mFontManager->GetFont(fontName,pointSize);
				if(!font)
				{
					ECHO_LOG_WARNING("The font named " << fontName << " does not exist.");
					return false;
				}
				mTextMesh->SetFont(font);
				mFontName=fontName;
				mPointSize=pointSize;
				SetUsePointSize(true);
				SizeUpdated();
			}
			return true;
		}

		void Text::SetFontSize(Scalar pointSize)
		{
			SetFont(mFontName,pointSize);
		}
		
		void Text::SetTextureFilterMethod(TextureUnit::TextureFilter filterMethod)
		{
			mTextMesh->SetTextureFilterMethod(filterMethod);
		}
		
		void Text::SetText(const UTF8String& content)
		{
			mTextMesh->Set(content);
			AdjustForPointSizing();
			SizeUpdated();
			PositionUpdated();
		}
		
		const UTF8String& Text::GetText() const
		{
			return mTextMesh->GetText();
		}

		void Text::Append(const UTF8String& content)
		{
			mTextMesh->Append(content);
			AdjustForPointSizing();
		}

		void Text::SetColour(const Colour& colour)
		{
			mTextMesh->SetColour(colour);
		}

		const Colour& Text::GetColour() const
		{
			return mTextMesh->GetColour();
		}

		void Text::SetMaxWidth(f32 maxWidth)
		{
			mTextMesh->SetMaxWidth(maxWidth);
			AdjustForPointSizing();
		}

		f32 Text::GetMaxWidth() const
		{
			return mTextMesh->GetMaxWidth();
		}

		void Text::SetUseMaxWidth(bool useMaxWidth)
		{
			mTextMesh->SetUseMaxWidth(useMaxWidth);
			AdjustForPointSizing();
		}

		bool Text::GetUseMaxWidth() const
		{
			return mTextMesh->GetUseMaxWidth();
		}

		void Text::SetTextScale(f32 textScale)
		{
			mTextMesh->SetTextScale(textScale);
			UpdateSize(GetAbsoluteWidth(), GetAbsoluteHeight());
		}

		f32 Text::GetTextScale() const
		{
			return mTextMesh->GetTextScale();
		}

		void Text::SetLineSpacing(f32 lineSpacing)
		{
			mTextMesh->SetLineSpacing(lineSpacing);
			UpdateSize(GetAbsoluteWidth(), GetAbsoluteHeight());
		}

		f32 Text::GetLineSpacing() const
		{
			return mTextMesh->GetLineSpacing();
		}

		f32 Text::GetAbsoluteLineSpacing() const
		{
			return mTextMesh->GetAbsoluteLineSpacing();
		}

		void Text::SetUseReplacementCharacter(bool useReplacementCharacterForGlyphFailure)
		{
			return mTextMesh->SetUseReplacementCharacter(useReplacementCharacterForGlyphFailure);
		}

		bool Text::GetUseReplacementCharacter(void) const
		{
			return mTextMesh->GetUseReplacementCharacter();
		}
		
		void Text::SetAddMeshExtentVertices(bool addMeshExtentVertices)
		{
			mTextMesh->SetAddMeshExtentVertices(addMeshExtentVertices);
			UpdateSize(GetAbsoluteWidth(), GetAbsoluteHeight());
		}
		
		bool Text::GetAddMeshExtentVertices() const
		{
			return mTextMesh->GetAddMeshExtentVertices();
		}

		size_t Text::GetLineCount()
		{
			return mTextMesh->GetLineCount();
		}
		
		Scalar Text::GetNativeAspectRatio()
		{
			Vector3 dimensions = mTextMesh->GetTextDimensions();
			if(dimensions.y!=0.f)
			{
				return dimensions.x/dimensions.y;
			}
			return 1.f;
		}
		
		Vector3 Text::GetTextMeshSize()
		{
			return mTextMesh->GetTextDimensions();
		}
		
		void Text::UpdateSize(Scalar newWidth, Scalar newHeight)
		{
			Vector3 dimensions = mTextMesh->GetTextDimensions();
			if(dimensions.x==0.f)
			{
				dimensions.x=newWidth;
			}else
			{
				dimensions.x=newWidth / dimensions.x;
			}

			if(dimensions.y==0.f)
			{
				dimensions.y=newHeight;
			}else
			{
				dimensions.y=newHeight / dimensions.y;
			}
			dimensions.z=1.f;
			mTextMesh->SetTextRenderScale(Vector2(dimensions.x,dimensions.y));
		}

		shared_ptr<Font> Text::GetFont() const
		{
			return mTextMesh->GetFont();
		}
		
		void Text::SetUsePointSize(bool usePointSize)
		{
			if(usePointSize && mPointSize>0.f)
			{
				mUsePointSize=usePointSize;
				AdjustForPointSizing();
			}else
			{
				mUsePointSize=usePointSize;
				// The user should provide a new size via one of the set size methods. We
				// do not know what the size should be.
			}
		}

		void Text::SetSize(Scalar width, Scalar height)
		{
			if(mUsePointSize)
			{
				//We'll ignore this because we have our own sizing plan.
				return;
			}
			Element::SetSize(width,height);
		}
		
		void Text::SetPhysicalSize(Metre width, Metre height)
		{
			if(mUsePointSize)
			{
				//We'll ignore this because we have our own sizing plan.
				return;
			}
			Element::SetPhysicalSize(width,height);
		}
		
		void Text::SetSizeInPixels(Scalar width, Scalar height)
		{
			if(mUsePointSize)
			{
				//We'll ignore this because we have our own sizing plan.
				return;
			}
			Element::SetSizeInPixels(width,height);
		}
		
		void Text::SetWidth(Scalar width)
		{
			if(mUsePointSize)
			{
				//We'll ignore this because we have our own sizing plan.
				return;
			}
			Element::SetWidth(width);
		}

		void Text::SetPhysicalWidth(Metre width)
		{
			if(mUsePointSize)
			{
				//We'll ignore this because we have our own sizing plan.
				return;
			}
			Element::SetPhysicalWidth(width);
		}
		
		void Text::SetWidthInPixels(Scalar width)
		{
			if(mUsePointSize)
			{
				//We'll ignore this because we have our own sizing plan.
				return;
			}
			Element::SetWidthInPixels(width);
		}
		
		void Text::SetHeight(Scalar height)
		{
			if(mUsePointSize)
			{
				//We'll ignore this because we have our own sizing plan.
				return;
			}
			Element::SetHeight(height);
		}
		
		void Text::SetPhysicalHeight(Metre height)
		{
			if(mUsePointSize)
			{
				//We'll ignore this because we have our own sizing plan.
				return;
			}
			Element::SetPhysicalHeight(height);
		}
		
		void Text::SetHeightInPixels(Scalar height)
		{
			if(mUsePointSize)
			{
				//We'll ignore this because we have our own sizing plan.
				return;
			}
			Element::SetHeightInPixels(height);
		}
		
		void Text::AdjustForPointSizing()
		{
			if(!mUsePointSize)
			{
				return;
			}
			// We need to set the size of the Text object to the size in Pixels to
			// match the text mesh size. We'll rely on the Element UnitConverter to
			// have the correct world to pixel ratio set.
			Vector3 dimensions=mTextMesh->GetTextDimensions();
			Element::SetSizeInPixels(dimensions.x,dimensions.y);
		}

		void Text::DisablePointSizing()
		{
			SetUsePointSize(false);
		}
	}
}
