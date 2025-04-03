#ifndef _ECHOVERTEXATTRIBUTE_H_
#define _ECHOVERTEXATTRIBUTE_H_

#include <echo/Types.h>

namespace Echo
{
	class VertexBuffer;
	
	class VertexAttribute
	{
	public:
		struct ComponentTypes
		{
			enum _
			{
				FLOAT32,
				FLOAT64,
				SIGNED8,
				UNSIGNED8,
				SIGNED16,
				UNSIGNED16,
				SIGNED32,
				UNSIGNED32,

				// These are convenient types that define the base type and number of components.
				VECTOR2,
				VECTOR3,
				VECTOR4,
				TEXTUREUV,
				COLOUR,
				COLOUR_8
			};
		};
		typedef ComponentTypes::_ ComponentType;
		VertexAttribute(ComponentType type, Size numberOfComponents=1);
		~VertexAttribute();
		void SetNormalise(bool normalise);
		bool GetNormalise() const {return mNormalise;}
		Size GetWidth() const {return mWidth;}
		Size GetNumberOfComponents() const {return mNumberOfComponents;}
		Size GetOffset() const {return mOffset;}
		ComponentType GetComponentType() const {return mComponentType;}
	private:
		void SetType(ComponentType type, Size numberOfComponents);
		friend class VertexBuffer;
		// Offset only makes sense when part of a vertex buffer, it is determined by the buffer object.
		void SetOffset(Size offset){mOffset=offset;}
	private:
		ComponentType mComponentType;
		bool mNormalise;
		Size mWidth;
		Size mOffset;
		Size mNumberOfComponents;
	};
}
#endif
