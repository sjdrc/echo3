#include <echo/Graphics/TextMesh.h>
#include <echo/Graphics/Font.h>
#include <echo/Graphics/Mesh.h>
#include <echo/Maths/Vector2.h>
#include <echo/Graphics/SubMesh.h>
#include <echo/Resource/MeshManager.h>
#include <echo/Kernel/ScopedLock.h>
#include <echo/UTF8String.h>

namespace Echo
{
	TextMesh::TextMesh(UTF8String content, shared_ptr<Font> font)
		:
		mFont(font),
		mString(content),
		mTextRenderScale(Vector2::UNIT_SCALE),
		mSpaceWidth(0.5f),
		mTextScale(1.f),
		mMaxWidth(100.f),		//Just an arbitrary size.'
		mLineSpacing(1.f),
		mUseMaxWidth(false),
		mMeshOutOfDate(true),
		mUseReplacementCharacterForGlyphFailure(true),
		mTextRenderScaleTransformOutOfDate(true),
		mUseRenderScaleTransform(false),
		mMeshExtentVertices(false),
		mLineCount(0),
		mTextRenderScaleTransform(Matrix4::IDENTITY),
		mColour()
	{ 
		shared_ptr<SubMesh> subMesh = CreateSubMesh();
		mVertexBuffer = subMesh->GetVertexBuffer(VertexBuffer::Types::DYNAMIC);
		mVertexBuffer->AddVertexAttribute("Position",VertexAttribute(VertexAttribute::ComponentTypes::VECTOR3, 1));	//Position
		mVertexBuffer->AddVertexAttribute("Normal",VertexAttribute(VertexAttribute::ComponentTypes::VECTOR3, 1));	//Normal
		mVertexBuffer->AddVertexAttribute("UV0",VertexAttribute(VertexAttribute::ComponentTypes::TEXTUREUV, 1));	// UV0
		
		if(mFont)
		{
			SetMaterial(mFont->GetMaterial()->Clone());
		}
	}

	TextMesh::~TextMesh()
	{
	}
	
	void TextMesh::SetFont(shared_ptr<Font> font)
	{
		if(mFont!=font)
		{
			mFont = font;
			mMeshOutOfDate=true;
			SetMaterial(mFont->GetMaterial()->Clone());
			MarkExtentsOutOfDate();
		}
	}
	
	void TextMesh::SetTextureFilterMethod(TextureUnit::TextureFilter filterMethod)
	{
		shared_ptr<SubMesh> subMesh = GetSubMesh(0);
		if(!subMesh || !subMesh->GetMaterial())
		{
			return;
		}
		//If there is a submesh and material there should be a pass and texture unit.
		assert(subMesh->GetMaterial()->GetPass(0) && "Pass should not be null.");
		assert(subMesh->GetMaterial()->GetPass(0)->GetTextureUnit(0) && "Texture unit should not be null.");
		subMesh->GetMaterial()->GetPass(0)->GetTextureUnit(0)->SetFilter(filterMethod);
	}	

	void TextMesh::Set(const UTF8String& content)
	{
		if(mString!=content)
		{
			mString = content;
			mMeshOutOfDate=true;
			MarkExtentsOutOfDate();
		}
	}
	
	void TextMesh::Append(const UTF8String& content)
	{
		mString += content;
		mMeshOutOfDate=true;
		MarkExtentsOutOfDate();
	}
	
	size_t TextMesh::GetLineCount()
	{
		UpdateMesh();
		return mLineCount;
	}

	void TextMesh::UpdateMesh() const
	{
		ScopedLock lock(mMeshMutex);
		if(!mMeshOutOfDate)
		{
			return;
		}
		shared_ptr<SubMesh> subMesh = GetSubMesh(0);
		if(!subMesh)
		{
			return;
		}
		subMesh->Clear();

		ConstructionResult result = BuildMesh(mString, subMesh);
		if(result.mSuccess)
		{
			mLineCount = result.mLineCount;
			mMeshOutOfDate = false;
			MarkExtentsOutOfDate();
		}
	}

	TextMesh::ConstructionResult TextMesh::BuildMesh(const UTF8String& utf8String, shared_ptr<SubMesh> subMesh) const
	{
		if(!mFont)
		{
			return ConstructionResult(false);
		}
		
		if(mString.Length()==0)
		{
			return ConstructionResult(true);
		}
		
		Size numberOfCharacters = utf8String.Length();
		const Size NUMBER_OF_VERTICES_PER_CHARACTER=4;
		const Size NUMBER_OF_EXTENTS_VERTICES=4;
		const Size NUMBER_OF_TRIANGLES_PER_CHARACTER=2;
		Size numberOfTriangles = NUMBER_OF_TRIANGLES_PER_CHARACTER*numberOfCharacters;
		if(!mElementBuffer)
		{
			if(!subMesh->SetElementBuffer(ElementBuffer::Types::STATIC,ElementBuffer::IndexTypes::UNSIGNED_16BIT,ElementBuffer::ElementTypes::TRIANGLE,numberOfTriangles))
			{
				ECHO_LOG_ERROR("Unable to setup Element Buffer for TextMesh " << utf8String);
				return ConstructionResult(false);
			}
			mElementBuffer = subMesh->GetElementBuffer();
		}else
		if(mElementBuffer->GetCapacity() < numberOfTriangles)
		{
			if(!mElementBuffer->Allocate(numberOfTriangles))
			{
				ECHO_LOG_ERROR("Reallocation of Element Buffer data failed for TextMesh " << utf8String);
				return ConstructionResult(false);
			}
		}
		
		if(mVertexBuffer->GetCapacity() < numberOfCharacters)
		{
			mVertexBuffer->Allocate(numberOfCharacters*NUMBER_OF_VERTICES_PER_CHARACTER + NUMBER_OF_EXTENTS_VERTICES);
		}
		
		// We'll adjust the number of elements as we go
		mVertexBuffer->SetNumberOfElements(0);
		mElementBuffer->SetNumberOfElements(0);

		// Update the accessors
		mVertices = mVertexBuffer->GetAccessor<Vector3>("Position");
		mNormals = mVertexBuffer->GetAccessor<Vector3>("Normal");
		mTexutreCoordinates = mVertexBuffer->GetAccessor<TextureUV>("UV0");
		mIndices = mElementBuffer->GetAccessor< ElementBuffer::Triangle<u16> >();

		UTF8String::iterator it = mString.begin();
		UTF8String::iterator itEnd = mString.end();

		shared_ptr<Glyph> previousGlyph = 0;
		Vector2 currentPosition(0,0);
		f32 currentWidth=0;	//Current height is the cursor y position.
		f32 currentWordWidth=0;
		f32 lineHeight = GetAbsoluteLineSpacing();
		f32 spaceWidth = mSpaceWidth*mFont->GetMaxWidth();
		
		// Determine the length of the current word.
		// If the current width plus word width is less than the specified object width
		//		Add the word.
		// Else
		//		Add new line
		//		Add the word.
		ConstructionResult result(true);
		result.mLineCount = 1;
		UTF8String currentWord;
		bool lastCharacter=false;
		while(!lastCharacter)
		{
			UTF32Code code;
			if(it==itEnd)
			{
				lastCharacter=true;
				code=0;
			}else
			{
				code = *it;
				it++;
			}
			const UTF32Code UTF8_SPACE = 32;
			const UTF32Code UTF8_NEWLINE = '\n';

			shared_ptr<Glyph> glyph;
			if(!lastCharacter && code != UTF8_SPACE && code != UTF8_NEWLINE)
			{
				glyph=mFont->GetGlyph(code, mUseReplacementCharacterForGlyphFailure);
			}
			
			if (!glyph || lastCharacter || code == UTF8_SPACE || code == UTF8_NEWLINE)
			{
				//We don't apply the scale while we're calculating the word width because we may need to adjust it if it exceeds max width.
				f32 wordScale = mTextScale;
				f32 scaledWordWidth=currentWordWidth*wordScale;
				//End of word as far as we are concerned.
				if(mUseMaxWidth && (currentWidth+scaledWordWidth)>mMaxWidth)
				{
					//If the word is larger than the width we need to scale it so it will fit.
					if(scaledWordWidth>mMaxWidth)
					{
						//Update the scaled values.
						wordScale = mMaxWidth/currentWordWidth;	//Find the new scale, use the unscaled word with.
						scaledWordWidth=currentWordWidth*wordScale;

						currentPosition.y-=lineHeight*wordScale;
						currentPosition.x=0;
						currentWidth=0;
					}else
					{
						//Create a new line if the current width is not 0.
						if(currentWidth>0)
						{
							currentPosition.x=0;
							currentWidth=0;
							currentPosition.y-=lineHeight*wordScale;
							previousGlyph = 0;
						}
					}
					if(code != UTF8_NEWLINE)
					{
						result.mLineCount++;
					}
				}
				
				if(currentWord.Length()>0)
				{
					AddWordToMesh(subMesh,currentPosition,currentWord,wordScale);
				}
				
				if(code == UTF8_NEWLINE)
				{
					//Update the position values for a new line
					currentPosition.x=0;
					currentWidth=0;
					currentWordWidth=0;
					currentWord.clear();
					currentPosition.y-=lineHeight*mTextScale;
					previousGlyph = 0;
					result.mLineCount++;
					continue;
				}else
				{
					//Move the cursor over
					currentPosition.x+=scaledWordWidth;
					currentPosition.x+=spaceWidth*wordScale;
					currentWidth+=scaledWordWidth + spaceWidth*wordScale;
					currentWordWidth=0;
					currentWord.clear();
					previousGlyph = 0;
				}
				continue;
			}else
			{
				currentWord+=code;
				currentWordWidth+=glyph->mAdvanceX;

				if(previousGlyph && code)
				{
					s32 kerning = previousGlyph->GetKerning(code);
					currentWordWidth+=kerning;
				}
				previousGlyph = glyph;
			}
		}
		
		//Do we need to add vertices to the extents?
		if(mMeshExtentVertices)
		{
			AddExtentsVertices(subMesh, result.mLineCount);
		}

		//Centre the SubMesh at its origin
		subMesh->Finalise();
		
		result.mDimensions = subMesh->GetDimensions();
		subMesh->TranslateVertices(-subMesh->GetCentre());
		return result;
	}
	
	void TextMesh::AddWordToMesh(shared_ptr<SubMesh> subMesh, Vector2 position, const UTF8String& word, f32 scale) const
	{
		f32 spaceWidth = mSpaceWidth*mFont->GetMaxWidth();
		UTF8String::iterator it = word.begin();
		UTF8String::iterator itEnd = word.end();
		shared_ptr<Glyph> previousGlyph=0;
		while(it!=itEnd)
		{
			UTF32Code code = *it;
			shared_ptr<Glyph> glyph= mFont->GetGlyph(code, mUseReplacementCharacterForGlyphFailure);
			if(!glyph)
			{
				position.x+=spaceWidth * scale;
				it++;
				previousGlyph = 0;
				continue;
			}

			if(previousGlyph && code)
			{
				f32 kerning = static_cast<f32>(previousGlyph->GetKerning(code));
				position.x+=kerning * scale;
			}
			
			AddCharacterToMesh(subMesh,position,*glyph,scale);

			position.x+=static_cast<f32>(glyph->mAdvanceX) * scale;
			previousGlyph = glyph;
			it++;
		}
	}
	
	void TextMesh::AddCharacterToMesh(shared_ptr<SubMesh> subMesh, const Vector2& position, const Glyph& glyph, f32 scale) const
	{
		if(!subMesh)
		{
			return;
		}

		//Vertices
		f32 left = position.x + static_cast<f32>(glyph.mXBearing) * scale;
		f32 right = left + static_cast<f32>(glyph.mWidth) * scale;
		f32 top = position.y + static_cast<f32>(glyph.mYBearing) * scale;
		f32 bottom = top - static_cast<f32>(glyph.mHeight) * scale;

		size_t indexBase = mVertexBuffer->GetNumberOfElements();

		mVertices[indexBase] = Vector3(left,top,0);
		mVertices[indexBase+1] = Vector3(right,top,0);
		mVertices[indexBase+2] = Vector3(left,bottom,0);
		mVertices[indexBase+3] = Vector3(right,bottom,0);

		//Texture coordinates
		TextureUV uv(glyph.mTextureCoordinates.first);
		TextureUV st(glyph.mTextureCoordinates.second);
		mTexutreCoordinates[indexBase] = uv;
		mTexutreCoordinates[indexBase+1] = TextureUV(st.u, uv.v);
		mTexutreCoordinates[indexBase+2] = TextureUV(uv.u, st.v);
		mTexutreCoordinates[indexBase+3] = st;
		
		mVertexBuffer->SetNumberOfElements(indexBase+4);

		Size elements = mElementBuffer->GetNumberOfElements();

		auto& triangle1 = mIndices[elements];
		triangle1.mA=indexBase;
		triangle1.mB=indexBase+1;
		triangle1.mC=indexBase+2;
		auto& triangle2 = mIndices[elements+1];
		triangle2.mA=indexBase+1;
		triangle2.mB=indexBase+3;
		triangle2.mC=indexBase+2;

		mElementBuffer->SetNumberOfElements(elements+2);
	}
	
	void TextMesh::AddExtentsVertices(shared_ptr<SubMesh> subMesh, Size lineCount) const
	{
		if(!subMesh)
		{
			return;
		}
		
		size_t vertexBase = mVertexBuffer->GetNumberOfElements();
		
		f32 maxY = static_cast<f32>(lineCount)*GetAbsoluteLineSpacing()*mTextScale;
		mVertices[vertexBase] = Vector3(0,0,0);
		mVertices[vertexBase+1] = Vector3(0,-maxY,0);
		mNormals[vertexBase] = Vector3(0,0,1);
		mNormals[vertexBase+1] = Vector3(0,0,1);
		mTexutreCoordinates[vertexBase] = TextureUV(0,0);
		mTexutreCoordinates[vertexBase+1] = TextureUV(0,0);

		vertexBase+=2;
		if(mUseMaxWidth)
		{
			mVertices[vertexBase] = Vector3(mMaxWidth,0,0);
			mVertices[vertexBase+1] = Vector3(mMaxWidth,-maxY,0);
			mNormals[vertexBase] = Vector3(0,0,1);
			mNormals[vertexBase+1] = Vector3(0,0,1);
			mTexutreCoordinates[vertexBase] = TextureUV(0,0);
			mTexutreCoordinates[vertexBase+1] = TextureUV(0,0);
			vertexBase+=2;
		}
		mVertexBuffer->SetNumberOfElements(vertexBase);
	}

	const AxisAlignedBox& TextMesh::GetAxisAlignedBox() const
	{
		UpdateMesh();
		return Mesh::GetAxisAlignedBox();
	}

	Vector3 TextMesh::GetTextDimensions() const
	{
		UpdateMesh();
		return Mesh::GetAxisAlignedBox().GetSize();
	}
	
	void TextMesh::Render(RenderContext& renderContext, const Matrix4& world, const Matrix4& worldView, Colour compoundDiffuse)
	{
		UpdateMesh();
		if(mUseRenderScaleTransform)
		{
			if(mTextRenderScaleTransformOutOfDate)
			{
				mTextRenderScaleTransform.MakeTransform(Vector3(0,0,0),Vector3(mTextRenderScale.x,mTextRenderScale.y,1.f),Quaternion::IDENTITY);
				mTextRenderScaleTransformOutOfDate=false;
			}

			Mesh::Render(renderContext,world*mTextRenderScaleTransform,worldView*mTextRenderScaleTransform,compoundDiffuse);
		}else
		{
			Mesh::Render(renderContext,world,worldView,compoundDiffuse);
		}
	}
	
	void TextMesh::SetColour(const Colour& colour)
	{
		if(mColour!=colour)
		{
			mColour = colour;
			SetDiffuseColour(mColour);
		}
	}
	
	void TextMesh::SetMaxWidth(f32 maxWidth)
	{
		if(maxWidth!=mMaxWidth && mUseMaxWidth)
		{
			mMeshOutOfDate=true;
			MarkExtentsOutOfDate();
		}
		mMaxWidth=maxWidth;
	}
		
	void TextMesh::SetUseMaxWidth(bool useMaxWidth)
	{
		if(useMaxWidth!=mUseMaxWidth)
		{
			mMeshOutOfDate=true;
			MarkExtentsOutOfDate();
		}
		mUseMaxWidth=useMaxWidth;
	}
		
	void TextMesh::SetTextScale(f32 textScale)
	{
		if(mTextScale!=textScale)
		{
			mTextScale = textScale;
			mMeshOutOfDate=true;
			MarkExtentsOutOfDate();
		}
	}

	void TextMesh::SetTextRenderScale(const Vector2& textRenderScale)
	{
		if(mTextRenderScale!=textRenderScale)
		{
			mTextRenderScale = textRenderScale;
			mTextRenderScaleTransformOutOfDate = true;
			mUseRenderScaleTransform = (mTextRenderScale!=Vector2::UNIT_SCALE);
		}
	}
	
	void TextMesh::SetLineSpacing(f32 lineSpacing)
	{
		if(lineSpacing!=mLineSpacing && mUseMaxWidth)
		{
			mMeshOutOfDate=true;
			MarkExtentsOutOfDate();
		}
		mLineSpacing = lineSpacing;
	}

	f32 TextMesh::GetAbsoluteLineSpacing() const
	{
		if(mFont)
		{
			return mLineSpacing * mFont->GetMaxHeight();
		}
		return 0.f;
	}
	
	void TextMesh::SetAddMeshExtentVertices(bool addMeshExtentVertices)
	{
		if(addMeshExtentVertices!=mMeshExtentVertices)
		{
			mMeshExtentVertices = addMeshExtentVertices;
			
			//Can we avoid rebuilding?
			if(mMeshExtentVertices && !mMeshOutOfDate)
			{
				AddExtentsVertices(GetSubMesh(0), mLineCount);
				CentreMeshToOrigin();
			}else
			{
				mMeshOutOfDate = true;
			}
		}
	}

	shared_ptr<TextMesh> TextMesh::Clone() const
	{
		//No need to clone the mesh.
		shared_ptr<TextMesh> newTextMesh(new TextMesh(mString,mFont));
		//mDimensions	 // Calculated later
		newTextMesh->mTextRenderScale = mTextRenderScale;
		newTextMesh->mSpaceWidth = mSpaceWidth;
		newTextMesh->mTextScale = mTextScale;
		newTextMesh->mMaxWidth = mMaxWidth;
		newTextMesh->mLineSpacing = mLineSpacing;
		newTextMesh->mUseMaxWidth = mUseMaxWidth;
		newTextMesh->mUseReplacementCharacterForGlyphFailure = mUseReplacementCharacterForGlyphFailure;
		newTextMesh->mUseRenderScaleTransform = mUseRenderScaleTransform;
		newTextMesh->mMeshExtentVertices = mMeshExtentVertices;
		// mLineCount					//Calculated later
		// mTextRenderScaleTransform //Calculated later
		newTextMesh->mColour = mColour;
		newTextMesh->UpdateMesh();
		return newTextMesh;
	}
}
