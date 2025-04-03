#include <echo/Graphics/VertexBuffer.h>
#include <echo/Maths/Vector3.h>
#include <echo/Graphics/PrimitiveTypes.h>

#include <doctest/doctest.h>

void AllocateTests()
{
	using namespace Echo;

	{
		VertexBuffer buffer(VertexBuffer::Types::STATIC);
		// This should fail since no attributes were added.
		CHECK(!buffer.Allocate(10));
	}
	{
		VertexBuffer buffer(VertexBuffer::Types::STATIC);
		buffer.AddVertexAttribute(VertexAttribute(VertexAttribute::ComponentTypes::VECTOR3));
		REQUIRE(buffer.GetStride()==sizeof(Vector3));
		
		buffer.AddVertexAttribute(VertexAttribute(VertexAttribute::ComponentTypes::TEXTUREUV));
		REQUIRE(buffer.GetStride()==(sizeof(Vector3)+sizeof(TextureUV)));		//Vector3 + TextureUV
	
		// Check allocation works
		CHECK(buffer.Allocate(10));

		// Verify that the buffer size is the expected.
		Size expectedBufferSize = (sizeof(Vector3)+sizeof(TextureUV))*10;
		Size bufferSize = buffer.GetBufferSize();
		CHECK(bufferSize==expectedBufferSize);
	}
}

void AccessorTests()
{
	using namespace Echo;

	// This is a duplicate of the allocate test.
	VertexBuffer buffer(VertexBuffer::Types::STATIC);
	buffer.AddVertexAttribute(VertexAttribute(VertexAttribute::ComponentTypes::VECTOR3));
	buffer.AddVertexAttribute(VertexAttribute(VertexAttribute::ComponentTypes::TEXTUREUV));
	buffer.Allocate(10);

	// Access and set the positions
	VertexBuffer::Accessor<Vector3> vertices = buffer.GetAccessor<Vector3>(0);
	for(Size i=0; i<vertices.GetCapacity(); i++)
	{
		vertices[i]=Vector3(i,i*10.f,i*100.f);
	}
	
	Size numberOfIncorrectValues=0;
	// Verify the vertices with a new accessor
	VertexBuffer::Accessor<Vector3> vertices2 = buffer.GetAccessor<Vector3>(0);
	for(Size i=0; i<vertices2.GetCapacity(); i++)
	{
		if(vertices2[i]!=Vector3(i,i*10.f,i*100.f))
		{
			numberOfIncorrectValues++;
		}
	}
	
	CHECK(numberOfIncorrectValues==0);

	// Access and set the uvs
	VertexBuffer::Accessor<TextureUV> uvs = buffer.GetAccessor<TextureUV>(1);
	for(Size i=0; i<uvs.GetCapacity(); i++)
	{
		uvs[i]=TextureUV(i*0.1f,-i);
	}
	
	// Verify that the other element accessor didn't step on the previous accessor's value assignment.
	numberOfIncorrectValues = 0;
	VertexBuffer::Accessor<Vector3> vertices3 = buffer.GetAccessor<Vector3>(0);
	for(Size i=0; i<vertices3.GetCapacity(); i++)
	{
		if(vertices3[i]!=Vector3(i,i*10.f,i*100.f))
		{
			numberOfIncorrectValues++;
		}
	}
	
	CHECK(numberOfIncorrectValues==0);
	
	// Verify the uv values
	numberOfIncorrectValues = 0;
	VertexBuffer::Accessor<TextureUV> uvs2 = buffer.GetAccessor<TextureUV>(1);
	for(Size i=0; i<uvs2.GetCapacity(); i++)
	{
		if(uvs2[i]!=TextureUV(i*0.1f,-i))
		{
			numberOfIncorrectValues++;
		}
	}
	
	CHECK(numberOfIncorrectValues==0);
	
	// Finally lets verify the raw data
	struct MyVertex
	{
		Vector3 position;
		TextureUV uv;
	};
	const MyVertex* myVertices=reinterpret_cast<const MyVertex*>(buffer.GetDataPointer());
	numberOfIncorrectValues = 0;
	for(Size i=0; i<buffer.GetCapacity(); i++)
	{
		if(myVertices[i].position!=Vector3(i,i*10.f,i*100.f))
		{
			numberOfIncorrectValues++;
		}
		if(myVertices[i].uv!=TextureUV(i*0.1f,-i))
		{
			numberOfIncorrectValues++;
		}
	}
	CHECK(numberOfIncorrectValues==0);
}

TEST_CASE("VertexBuffer")
{
	// Turn off log output, we will just use output from this test.
	Echo::gDefaultLogger.SetLogMask(Echo::Logger::LogLevels::NONE);

	AllocateTests();
	AccessorTests();
}
