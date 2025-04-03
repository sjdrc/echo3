#include <echo/Graphics/TextureUnit.h>

namespace Echo
{
	TextureUnit::TextureUnit()
	{	
		mBlendMode.operation = LayerBlendOperationExs::MODULATE;
		mBlendMode.blendType = LayerBlendTypes::COLOUR;
		mBlendMode.source1 = LayerBlendSources::TEXTURE;
		mBlendMode.source2 = LayerBlendSources::CURRENT;
		mBlendMode.colour = Colour();

		mAlphaBlendMode.operation = LayerBlendOperationExs::MODULATE;
		mAlphaBlendMode.blendType = LayerBlendTypes::ALPHA;
		mAlphaBlendMode.source1 = LayerBlendSources::TEXTURE;
		mAlphaBlendMode.source2 = LayerBlendSources::CURRENT;
		mAlphaBlendMode.colour = Colour();
		
		mWrapModeU=TextureUnit::TextureWrapModes::REPEAT;
		mWrapModeV=TextureUnit::TextureWrapModes::REPEAT;
		mMinFilter=TextureUnit::TextureFilters::LINEAR;
		mMagFilter=TextureUnit::TextureFilters::LINEAR;
		mUScrollSpeed=0.0f;
		mVScrollSpeed=0.0;
		mUScaleSpeed=0;
		mVScaleSpeed=0;
		mUseTransformForStipAnimation=true;
		mRotationSpeed.SetZero();
		mTextureMatrix = Matrix4::IDENTITY;
		mTextureUnitSetToUse = 0;
	}

	TextureUnit::~TextureUnit()
	{
	}
	
	void TextureUnit::SetBlendMode(const BlendMode& val)
	{
		LayerBlendModeEx alphaBlend;
		alphaBlend.blendType = LayerBlendTypes::ALPHA;
		LayerBlendModeEx colourBlend;
		colourBlend.blendType = LayerBlendTypes::COLOUR;
		
		switch(val)
		{
			case BlendModes::NONE:
				colourBlend.operation=LayerBlendOperationExs::SOURCE1;
				alphaBlend.operation=LayerBlendOperationExs::SOURCE1;
			break;
			case BlendModes::TRANSPARENT:
				colourBlend.operation=LayerBlendOperationExs::BLEND_TEXTURE_ALPHA;
				alphaBlend.operation=LayerBlendOperationExs::BLEND_TEXTURE_ALPHA;
			break;
			case BlendModes::ADDITIVE:
				colourBlend.operation=LayerBlendOperationExs::ADD;
				alphaBlend.operation=LayerBlendOperationExs::ADD;
			break;
			case BlendModes::ADDITIVE_COLOUR:
				colourBlend.operation=LayerBlendOperationExs::ADD_SIGNED;
				alphaBlend.operation=LayerBlendOperationExs::ADD_SIGNED;
			break;
			case BlendModes::MODULATE:
				//This is actually the default for texture units.
				colourBlend.operation=LayerBlendOperationExs::MODULATE;
				alphaBlend.operation=LayerBlendOperationExs::MODULATE;
			break;
			case BlendModes::MODULATE_AND_BRIGHTEN:
				colourBlend.operation=LayerBlendOperationExs::MODULATE_X2;
				alphaBlend.operation=LayerBlendOperationExs::MODULATE_X2;
			break;
			case BlendModes::MODULATE_AND_BRIGHTEN_MORE:
				colourBlend.operation=LayerBlendOperationExs::MODULATE_X4;
				alphaBlend.operation=LayerBlendOperationExs::MODULATE_X4;
			break;
			case BlendModes::SUBTRACT:
				colourBlend.operation=LayerBlendOperationExs::SUBTRACT;
				alphaBlend.operation=LayerBlendOperationExs::SUBTRACT;
			break;
			default:
				//Should never be reached.
				break;
		}
		SetAlphaBlendMode(alphaBlend);
		SetBlendMode(colourBlend);
	}

}
