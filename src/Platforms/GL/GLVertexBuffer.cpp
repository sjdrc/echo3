#include <echo/Platforms/GL/GLVertexBuffer.h>
#include <limits>

namespace Echo
{
	GLVertexBuffer::GLVertexBuffer(const VertexBuffer& vertexBuffer) : mVersion(std::numeric_limits<Size>::max()), mIsReady(false), mAllocatedBufferSize(0)
	{
		glGenVertexArrays(1,&mVertexArrayObject);
		EchoCheckOpenGLError();
		glGenBuffers(1,&mVertexBuffer);
		EchoCheckOpenGLError();
		Update(vertexBuffer);
	}

	GLVertexBuffer::~GLVertexBuffer()
	{
		Unbind();
		glDeleteBuffers(1,&mVertexBuffer);
		EchoCheckOpenGLError();
		glDeleteVertexArrays(1,&mVertexArrayObject);
		EchoCheckOpenGLError();
	}
	
	void GLVertexBuffer::Update(const VertexBuffer& vertexBuffer)
	{
		if(mVersion==vertexBuffer.GetVersion())
		{
			return;
		}
		Bind();

		EchoCheckOpenGLError();

		if(mAllocatedBufferSize!=vertexBuffer.GetBufferSize())
		{
			if(vertexBuffer.GetType()==VertexBuffer::Types::STATIC)
			{
				glBufferStorage(GL_ARRAY_BUFFER, vertexBuffer.GetBufferSize(), vertexBuffer.GetDataPointer(), 0);
				EchoCheckOpenGLError();
			}else
			{
				glBufferStorage(GL_ARRAY_BUFFER, vertexBuffer.GetBufferSize(), vertexBuffer.GetDataPointer(), GL_DYNAMIC_STORAGE_BIT);
				EchoCheckOpenGLError();
			}
			mAllocatedBufferSize=vertexBuffer.GetBufferSize();
		}else
		{
			if(vertexBuffer.GetType()==VertexBuffer::Types::STATIC)
			{
				// A static buffer needs to be reallocated
				glBufferStorage(GL_ARRAY_BUFFER, vertexBuffer.GetBufferSize(), vertexBuffer.GetDataPointer(), 0);
				EchoCheckOpenGLError();
			}else
			{
				// Dynamic buffer can be updated.
				glBufferSubData(GL_ARRAY_BUFFER, 0, vertexBuffer.GetBufferSize(), vertexBuffer.GetDataPointer());
				EchoCheckOpenGLError();
			}
		}

		Size numberOfAttributes = vertexBuffer.GetNumberOfVertexAttributes();
		Size stride = vertexBuffer.GetStride();
		for(Size i = 0; i < numberOfAttributes; ++i)
		{
			const VertexAttribute* attribute = vertexBuffer.GetVertexAttribute(i);
			GLenum type;
			GLint size = attribute->GetNumberOfComponents();
			GLboolean normalise = attribute->GetNormalise();
			switch( attribute->GetComponentType() )
			{
				case VertexAttribute::ComponentTypes::FLOAT32:
					type = GL_FLOAT;
					normalise = GL_FALSE;
				break;
				case VertexAttribute::ComponentTypes::FLOAT64:
					type = GL_DOUBLE;
					normalise = GL_FALSE;
				break;
				case VertexAttribute::ComponentTypes::SIGNED8:		type = GL_BYTE;				break;
				case VertexAttribute::ComponentTypes::UNSIGNED8:	type = GL_UNSIGNED_BYTE;	break;
				case VertexAttribute::ComponentTypes::SIGNED16:		type = GL_SHORT;			break;
				case VertexAttribute::ComponentTypes::UNSIGNED16:	type = GL_UNSIGNED_SHORT;	break;
				case VertexAttribute::ComponentTypes::SIGNED32:		type = GL_INT;				break;
				case VertexAttribute::ComponentTypes::UNSIGNED32:	type = GL_UNSIGNED_INT;		break;
				case VertexAttribute::ComponentTypes::VECTOR2:		type = GL_FLOAT; 			break;
				case VertexAttribute::ComponentTypes::VECTOR3:		type = GL_FLOAT; 			break;
				case VertexAttribute::ComponentTypes::VECTOR4:		type = GL_FLOAT;			break;
				case VertexAttribute::ComponentTypes::TEXTUREUV:	type = GL_FLOAT;			break;
				case VertexAttribute::ComponentTypes::COLOUR:		type = GL_FLOAT;			break;
				case VertexAttribute::ComponentTypes::COLOUR_8:		type = GL_UNSIGNED_BYTE;	break;
				default:
					ECHO_LOG_ERROR("Unsupported VertexAttribute component type");
					return;
			}
			
			// https://www.khronos.org/opengl/wiki/Vertex_Specification#Vertex_buffer_offset_and_stride
			// The offset​​ defines the buffer object offset. Note that it is a parameter of type const void * rather than an integer of some kind. This is in
			// part why it's called glVertexAttribPointer, due to old legacy stuff where this was actually a client pointer. So you will need to cast the
			// integer offset into a pointer.
			glVertexAttribPointer( i, size,type, normalise, stride, reinterpret_cast<void*>(attribute->GetOffset()));
			EchoCheckOpenGLError();

			glEnableVertexAttribArray(i);
			EchoCheckOpenGLError();
		}
		mVersion = vertexBuffer.GetVersion();
	}

	void GLVertexBuffer::Bind()
	{
		glBindVertexArray(mVertexArrayObject);
		glBindBuffer(GL_ARRAY_BUFFER,mVertexBuffer);
		EchoCheckOpenGLError();
	}

	void GLVertexBuffer::Unbind()
	{
		glBindBuffer(GL_ARRAY_BUFFER,0);
		glBindVertexArray(0);
		EchoCheckOpenGLError();
	}
}
