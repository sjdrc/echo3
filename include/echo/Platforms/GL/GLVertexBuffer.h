#ifndef _ECHOGLVERTEXBUFFER_H_
#define _ECHOGLVERTEXBUFFER_H_

#include <echo/Platforms/GL/GLSupport.h>
#include <echo/Graphics/VertexBuffer.h>

namespace Echo
{
	/**
	 * GLVertexBuffer manages the Vertex Array Object and buffer objects for a VertexBuffer.
	 * One of these is mapped to each VertexBuffer that is used by a GLRenderTarget.
 	 */
	class GLVertexBuffer
	{
	public:
		GLVertexBuffer(const VertexBuffer& vertexBuffer);
		~GLVertexBuffer();

		/**
		 * Check the buffer version and update if required.
		 * Calling this method will cause the buffer to be bound if required.
		 * The buffer will not be unbound automatically.
		 */
		void Update(const VertexBuffer& vertexBuffer);

		/**
		 * Bind should be called when the buffer is going to be used.
		 * @note Bind does not need to be called to update the buffer. Binding and Unbinding is managed automatically for updates.
		 */
		void Bind();

		/**
		 * Unbind the buffer as the current vertex buffer
		 */
		void Unbind();

		/**
		 * Get the version of the buffer.
		 */
		Size GetVersion() const
		{
			return mVersion;
		}
		
		/**
		 * Force a version change.
		 */
		inline void IncrementVersion()
		{
			mVersion++;
		}
	private:
		Size mVersion;
		bool mIsReady;
		GLuint mVertexArrayObject;
		GLuint mVertexBuffer;
		Size mAllocatedBufferSize;
	};
}
#endif
