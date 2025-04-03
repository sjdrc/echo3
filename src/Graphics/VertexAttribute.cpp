#include <echo/Graphics/VertexAttribute.h>
#include <echo/Maths/Vector4.h>
#include <echo/Maths/Vector2.h>
#include <echo/Graphics/Colour.h>

namespace Echo
{
	VertexAttribute::VertexAttribute(ComponentType type, Size numberOfComponents)
	{
		mOffset=0;
		mNormalise = false;
		SetType(type,numberOfComponents);
	}
	
	VertexAttribute::~VertexAttribute()
	{
	}

	void VertexAttribute::SetNormalise(bool normalise)
	{
		mNormalise = normalise;
	}
	
	void VertexAttribute::SetType(ComponentType type, Size numberOfComponents)
	{
		mComponentType=type;
		mNumberOfComponents = numberOfComponents;
		switch(type)
		{
			case ComponentTypes::FLOAT32:
				mWidth=numberOfComponents*sizeof(f32);
				return;
			case ComponentTypes::FLOAT64:
				mWidth=numberOfComponents*sizeof(f64);
				return;
			case ComponentTypes::SIGNED8:
				mWidth=numberOfComponents*sizeof(s8);
				return;
			case ComponentTypes::UNSIGNED8:
				mWidth=numberOfComponents*sizeof(u8);
				return;
			case ComponentTypes::SIGNED16:
				mWidth=numberOfComponents*sizeof(s16);
				return;
			case ComponentTypes::UNSIGNED16:
				mWidth=numberOfComponents*sizeof(u16);
				return;
			case ComponentTypes::SIGNED32:
				mWidth=numberOfComponents*sizeof(s32);
				return;
			case ComponentTypes::UNSIGNED32:
				mWidth=numberOfComponents*sizeof(u32);
				return;

			case ComponentTypes::VECTOR2:
				SetType(ComponentTypes::FLOAT32,2);
				return;
			case ComponentTypes::VECTOR3:
				SetType(ComponentTypes::FLOAT32,3);
				return;
			case ComponentTypes::VECTOR4:
				SetType(ComponentTypes::FLOAT32,4);
				return;
			case ComponentTypes::TEXTUREUV:
				SetType(ComponentTypes::FLOAT32,2);
				return;
			case ComponentTypes::COLOUR:
				SetType(ComponentTypes::FLOAT32,4);
				return;
			case ComponentTypes::COLOUR_8:
				SetType(ComponentTypes::UNSIGNED8,4);
				mNormalise = true;
				return;
		}
	}
}
