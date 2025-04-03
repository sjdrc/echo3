#ifndef _ECHOVERTEXBUFFER_H_
#define _ECHOVERTEXBUFFER_H_

#include <echo/Types.h>
#include <echo/Graphics/VertexAttribute.h>
#include <echo/Resource/Resource.h>
#include <vector>
#include <map>
#include <assert.h>

namespace Echo
{
	/**
	 * Simple abstraction of VertexBuffers.
	 * A VertexBuffer is made up of a number of VertexAttributes which correspond to the
	 * mesh properties. For example: a Mesh with vertices containing positions, normals
	 * and colours would have a VertexBuffer with three attributes a Vector3,Vector3 and
	 * Colour respectively. The vertex buffer defines where each of these components are
	 * positioned (based on the order the attributes are added) in each vertex.
	 * 
	 * VertexBuffers are much more explicit and flexible and ultimately more predictable
	 * than when using a default rendering pipeline which allows for additional
	 * performance improvements to be made in shaders.
	 * 
	 * The buffer stride is determined by the sum of the width of the vertex attributes.
	 * 
	 * @note IncrementVersion() needs to be called if the vertex buffer changes to
	 * trigger an update in any render targets that use the version number to track
	 * state.
	 */
	class VertexBuffer : public Resource<VertexBuffer>
	{
	public:
		struct Types
		{
			enum _
			{
				STATIC,
				DYNAMIC
			};
		};
		typedef Types::_ Type;

		VertexBuffer(Type type);
		~VertexBuffer();
		VertexBuffer(const VertexBuffer& rhs);
		VertexBuffer(VertexBuffer&& rhs);
		VertexBuffer& operator=(const VertexBuffer& rhs);
		VertexBuffer& operator=(VertexBuffer&& rhs);

		Type GetType() const
		{
			return mType;
		}
		Size GetStride() const;

		Size AddVertexAttribute(VertexAttribute vertexAttribute);
		Size AddVertexAttribute(std::string name, VertexAttribute vertexAttribute);
		
		/**
		 * Get a vertex attribute.
		 * @param attributePosition The index of the attribute.
		 * @return nullptr if the index exceeds the number of attributes, otherwise the
		 * attribute at that index.
		 */
		VertexAttribute* GetVertexAttribute(Size attributePosition)
		{
			if(attributePosition >= mVertexAttributes.size())
			{
				ECHO_LOG_ERROR("VertexAttribute index out of range " << attributePosition << " >= " << mVertexAttributes.size() );
				return nullptr;
			}
			return &mVertexAttributes[attributePosition];
		}

		/**
		 * Get a vertex attribute.
		 * @param attributePosition The index of the attribute.
		 * @return nullptr if the index exceeds the number of attributes, otherwise the
		 * attribute at that index.
		 */
		const VertexAttribute* GetVertexAttribute(Size attributePosition) const
		{
			if(attributePosition >= mVertexAttributes.size())
			{
				ECHO_LOG_ERROR("VertexAttribute index out of range " << attributePosition << " >= " << mVertexAttributes.size() );
				return nullptr;
			}
			return &mVertexAttributes[attributePosition];
		}

		/**
		 * Get a vertex attribute by name.
		 * @param name Name of the attribute.
		 * @return nullptr if the name does not point to an attribute, otherwise the attribute.
		 */
		VertexAttribute* GetVertexAttribute(const std::string& name)
		{
			std::map< std::string, Size >::iterator it = mNamedAttributes.find(name);
			if(it==mNamedAttributes.end())
			{
				return nullptr;
			}
			return GetVertexAttribute(it->second);
		}

		/**
		 * Get a vertex attribute by name.
		 * @param name Name of the attribute.
		 * @return nullptr if the name does not point to an attribute, otherwise the attribute.
		 */
		const VertexAttribute* GetVertexAttribute(const std::string& name) const
		{
			std::map< std::string, Size >::const_iterator it = mNamedAttributes.find(name);
			if(it==mNamedAttributes.end())
			{
				return nullptr;
			}
			return GetVertexAttribute(it->second);
		}

		Size GetNumberOfVertexAttributes() const
		{
			return mVertexAttributes.size();
		}

		/**
		 * Allocate a buffer big enough to contain the specified number of vertices.
		 * This method needs to be called after all of the VertexAttributes have been added.
		 * In debug mode this method will assert if attributes have not been added first.
		 * @note If a buffer has previously been allocated it will be deallocated first.
		 * @param numberOfVertices The number of vertices this buffer needs to be able to hold.
		 * @return true if allocation was successful, false if the stride is zero.
		 */
		bool Allocate(Size numberOfVertices);
		
		/**
		 * Deallocate the buffer and set the capacity and number of elements to 0.
		 */
		void Deallocate();

		/**
		 * An Accessor is an element component accessor.
		 * Accessor provides convenient access to the element component of by index of the given type.
		 * You can request an Accessor from using VertexBuffer::GetAccessor<T>().
		 * You can check validity of an Accessor by converting it to a bool. For example:
		 *	if(accessor)
		 *	{
		 *		//Ok to use
		 *	}
		 */
		template<typename T>
		class Accessor
		{
		public:
			Accessor() : mBase(nullptr), mStride(0), mCapacity(0){}
			Accessor(char* base, Size stride, Size elementCapacity) : mBase(base), mStride(stride), mCapacity(elementCapacity){}
			Accessor(const Accessor<T>& rhs)
			{
				mBase = rhs.mBase;
				mStride = rhs.mStride;
				mCapacity = rhs.mCapacity;
			}
			Accessor<T>& operator=(const Accessor<T>& rhs)
			{
				mBase = rhs.mBase;
				mStride = rhs.mStride;
				mCapacity = rhs.mCapacity;
				return *this;
			}
			T& operator[](size_t index)
			{
				return *reinterpret_cast<T*>(&(mBase[mStride*index]));
			}
			
			const T& operator[](size_t index) const
			{
				return *reinterpret_cast<T*>(&(mBase[mStride*index]));
			}
			
			inline operator bool() const
			{
				return (mBase!=nullptr);
			}
			
			inline Size GetCapacity() const {return mCapacity;}
		private:
			char* mBase;
			Size mStride;
			Size mCapacity;
		};

		/**
		 * Get an Accessor for elements of of the format at the specified attribute index.
		 * @note The only sanity check performed on the type is a comparison of the size of the specified type
		 * and the size of the component as determined by the VertexAttribute. You need to you make sure you
		 * specify the type that matches the attributes data format otherwise your buffer results may not be
		 * as you expect.
		 * @param attributePosition The attribute index. The index is the value returned by AddVertexAttribute()
		 * @return An Accessor if the attribute position is valid, otherwise a null Accessor.
		 */
		template<typename T>
		Accessor<T> GetAccessor(Size attributePosition)
		{
			assert(mDataSize && "No allocation has been made. Did you forget to call Allocate()?");
			assert(attributePosition < mVertexAttributes.size());
			if(attributePosition >= mVertexAttributes.size())
			{
				return Accessor<T>();
			}
			VertexAttribute& attribute = mVertexAttributes[attributePosition];
			assert(attribute.GetWidth()==sizeof(T) && "The specified type does not match the attribute width.");
			return Accessor<T>(mData+attribute.GetOffset(), mStride, mCapacity);
		}
		
		/**
		 * Get an Accessor by name.
		 * @param name The name of the element data to access, this name is the name of the VertexAttribute.
		 * @see GetAccessor<T>(Size)
		 * @return An Accessor if the name is valid, otherwise a null Accessor.
		 */
		template<typename T>
		Accessor<T> GetAccessor(std::string name)
		{
			std::map< std::string, Size >::iterator it = mNamedAttributes.find(name);
			if(it==mNamedAttributes.end())
			{
				return Accessor<T>();
			}
			return GetAccessor<T>(it->second);
		}
		
		/**
		 * Const version of GetAccessor<T>(Size)
		 * @see GetAccessor<T>(Size)
		 */
		template<typename T>
		const Accessor<T> GetAccessor(Size attributePosition) const
		{
			assert(mDataSize && "No allocation has been made. Did you forget to call Allocate()?");
			assert(attributePosition < mVertexAttributes.size());
			const VertexAttribute& attribute = mVertexAttributes[attributePosition];
			assert(attribute.GetWidth()==sizeof(T) && "The specified type does not match the attribute width.");
			return Accessor<T>(mData+attribute.GetOffset(), mStride, mCapacity);
		}
		
		/**
		 * Const version of GetAccessor<T>(std::string)
		 * @see GetAccessor<T>(std::string)
		 */
		template<typename T>
		const Accessor<T> GetAccessor(std::string name) const
		{
			std::map< std::string, Size >::const_iterator it = mNamedAttributes.find(name);
			assert(it!=mNamedAttributes.end() && "No attribute found with specified name");
			if(it==mNamedAttributes.end())
			{
				return Accessor<T>();
			}
			return GetAccessor<T>(it->second);
		}
		
		Size GetBufferSize() const {return mDataSize;}

		/**
		 * Set the number of elements this VertexBuffer has had written.
		 * @note This value should not exceed the capacity of the buffer, the number of elements will
		 * be clamped to the capacity. In debug mode the method will assert.
		 * @param numberOfElements The number of elements written to the buffer.
		 */
		void SetNumberOfElements(Size numberOfElements);

		/**
		 * Get the number of elements this buffer contains.
		 * @note For convenience, if SetNumberOfElements() has not been called then this method will
		 * return the capacity since it is assumed that a VertexBuffer is used entirely.
		 * @return The number of elements this VertexBuffer contains.
		 */
		Size GetNumberOfElements() const {return mNumberOfElements;}
		
		/**
		 * Get the capacity of the vertex buffer, this is the number of vertices this buffer can have.
		 */
		Size GetCapacity() const {return mCapacity;}
		
		/**
		 * Get the pointer to the data.
		 * @return A pointer to the start of the allocated buffer.
		 */
		const char* GetDataPointer() const
		{
			return mData;
		}
	private:
		virtual bool _Unload() override;
		virtual size_t OnRequestMemoryRelease() override;

		Type mType;
		Size mStride;
		std::vector< VertexAttribute > mVertexAttributes;
		std::map< std::string, Size > mNamedAttributes;
		char* mData;
		Size mDataSize;
		Size mNumberOfElements;
		Size mCapacity;
	};	
}
#endif
