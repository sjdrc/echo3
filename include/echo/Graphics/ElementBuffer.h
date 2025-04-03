#ifndef _ECHOELEMENTBUFFER_H_
#define _ECHOELEMENTBUFFER_H_

#include <echo/Types.h>
#include <vector>
#include <map>
#include <assert.h>

namespace Echo
{
	/**
	 * ElementBuffer wraps a buffer of indices to be accessed as Graphics primitive elements.
	 * ElementBuffers are used to describe how vertices in an VertexBuffer are related to
	 * create the graphics primitives. For example, three vertices would create a triangle however
	 * they may not be in order in the VertexBuffer. The ElementBuffer would contain indices in
	 * order that point to the corresponding vertices.
	 * 
	 * ElementBuffer is also designed to be a higher level access to indices rather than client
	 * code dealing with an index buffer directly and worrying about element stride. Code written
	 * using the Accessors naturally describes in detail what is being manipulated. For example
	 * compare the two snippets:
	 * 
	 *	someIndexBuffer[i] = triangleIndexBase;
	 *	someIndexBuffer[i+1] = triangleIndexBase+1;
	 *	someIndexBuffer[i+2] = triangleIndexBase+2;
	 *	someIndexBuffer[i+3] = triangleIndexBase+3;
	 *	someIndexBuffer[i+4] = triangleIndexBase+4;
	 *	someIndexBuffer[i+5] = triangleIndexBase+5;
	 * 
	 * Vs:
	 * 
	 *	ElementBuffer::Triangle< u16 > triangles = elementBuffer->GetAccessor< ElementBuffer::Triangle< u16 > >();
	 *	triangles[0].mA = triangleIndexBase;
	 *	triangles[0].mB = triangleIndexBase+1;
	 *	triangles[0].mC = triangleIndexBase+2;
	 *	triangles[1].mA = triangleIndexBase+3;
	 *	triangles[1].mB = triangleIndexBase+4;
	 *	triangles[1].mC = triangleIndexBase+5;
	 * 
	 * While the code blocks are similar, the second communicates more that we are creating triangles where as the
	 * first block it is less obvious at first glance or without additional comments.
	 */
	class ElementBuffer
	{
	public:
		struct ElementTypes
		{
			enum _
			{
				UNDEFINED,
				POINTS,
				LINES,
				LINE_STRIP,
				LINE_LOOP,
				TRIANGLE,
				TRIANGLE_STRIP,
				TRIANGLE_FAN
			};
		};
		typedef ElementTypes::_ ElementType;

		struct IndexTypes
		{
			enum _
			{
				UNDEFINED,
				UNSIGNED_16BIT,					// Use when there are up to 65535 vertices
				UNSIGNED_32BIT					// Use when you have more than 65535 vertices
			};
		};
		typedef IndexTypes::_ IndexType;
		
		struct Types
		{
			enum _
			{
				STATIC,
				DYNAMIC
			};
		};
		typedef Types::_ Type;
		
		template < typename T >
		struct Point
		{
			T mIndex;
		};

		template < typename T >
		struct Line
		{
			T mA;
			T mB;
		};
		
		template < typename T >
		struct Triangle
		{
			T mA;
			T mB;
			T mC;
		};

		/**
		 * Element buffer constructor
		 * @param type The type of element buffer. This is a hint to indicate expected usage.
		 *	STATIC suggests that the buffer contents aren't going to change
		 *	DYNAMIC suggests the buffer will change frequently.
		 * Hint DYNAMIC if you plan on changing the buffer contents every frame or two, otherwise
		 * STATIC should be sufficient. The platform implementation may not even use this.
		 */
		ElementBuffer(Type type);
		~ElementBuffer();
		ElementBuffer(const ElementBuffer& rhs);
		ElementBuffer(ElementBuffer&& rhs);
		ElementBuffer& operator=(const ElementBuffer& rhs);
		ElementBuffer& operator=(ElementBuffer&& rhs);
		
		IndexType GetIndexType() const;
		ElementType GetElementType() const;

		/**
		 * Allocate a buffer big enough to contain the specified number of elements.
		 * @note If a buffer has previously been allocated it will be deallocated first.
		 * @param indexType The index type.
		 * @param elementType The type of element you want to allocate.
		 * @param numberOfElements The number of elements this buffer needs to be able to hold.
		 * @return true if allocation was successful, false if the stride or nulberOfElements is zero.
		 */
		bool Allocate(IndexType indexType, ElementType elementType, Size numberOfElements);
		
		/**
		 * Version of Allocate() that uses the currently set values for index and element type.
		 * @see Allocate(IndexType,ElementType,Size);
		 * @param onlyIfNeeded if true an allocation will only occur if the number of elements requested exceeds the current capacity.
		 */
		bool Allocate(Size numberOfElements, bool onlyIfNeeded = true);

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
			Accessor(char* base, Size stride, Size elementCapacity, IndexType indexType, ElementType elementType) :
				mBase(base),
				mStride(stride),
				mCapacity(elementCapacity),
				mIndexType(indexType),
				mElementType(elementType)
			{}
			Accessor(const Accessor<T>& rhs)
			{
				mBase = rhs.mBase;
				mStride = rhs.mStride;
				mCapacity = rhs.mCapacity;
				mIndexType = rhs.mIndexType;
				mElementType = rhs.mElementType;
			}
			Accessor<T>& operator=(const Accessor<T>& rhs)
			{
				mBase = rhs.mBase;
				mStride = rhs.mStride;
				mCapacity = rhs.mCapacity;
				mIndexType = rhs.mIndexType;
				mElementType = rhs.mElementType;
				return *this;
			}
			T& operator[](Size index)
			{
				ECHO_ASSERT(index < mCapacity, "Index needs to be less than the capacity");
				return *reinterpret_cast<T*>(&(mBase[mStride*index]));
			}
			
			const T& operator[](Size index) const
			{
				ECHO_ASSERT(index < mCapacity, "Index needs to be less than the capacity");
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
			IndexType mIndexType;
			ElementType mElementType;
		};

		/**
		 * Typesafe access to the buffer.
		 * Usage:
		 *	Accessor< ElementBuffer::Point< IndexType > > GetAccessor< ElementBuffer::Point< IndexType > >()
		 *	Accessor< ElementBuffer::Line< IndexType > > GetAccessor< ElementBuffer::Line< IndexType > >()
		 *	Accessor< ElementBuffer::Triangle< IndexType > > GetAccessor< ElementBuffer::Triangle< IndexType > >()
		 * Where IndexType is u16 or u32.
		 * The Accessor will be configured to step as per the buffer type. For example
		 * GetAccessor< ElementBuffer::Triangle<u16> >() will return an accessor that will correctly
		 * iterate over the buffer for TRIANGLE and TRIANGLE_STRIP.
		 * @note For some element types some elements will share indices with adjacent ones, for example Triangle
		 * strip at index 1 will share point A and B with index 0 (B and C).
		 * @note for some element types, such as TRIANGLE_FAN and LINE_LOOP, it is not possible to iterate
		 * over elements with the Accessor class due to the way they are indexed. For those types you will
		 * need to access the indices directly with GetAccesor<IndexType>() rather than the element type.
		 */
		template<typename T>
		Accessor<T> GetAccessor()
		{
			assert(mDataSize && "No allocation has been made. Did you forget to call Allocate()?");
			assert(GetElementSize()==sizeof(T) && "The specified type does not match the element size.");
			return Accessor<T>(mData, GetElementStride(), mCapacity, mIndexType, mElementType);
		}

		/**
		 * Const version of GetAccessor<T>()
		 * @see GetAccessor<T>()
		 */
		template<typename T>
		const Accessor<T> GetAccessor() const
		{
			assert(mDataSize && "No allocation has been made. Did you forget to call Allocate()?");
			assert(GetElementSize()==sizeof(T) && "The specified type does not match the element size.");
			return Accessor<T>(mData, GetElementStride(), mCapacity, mIndexType, mElementType);
		}
		
		/**
		 * Get access to indices.
		 */
		template<typename T>
		Accessor<T> GetIndexAccessor()
		{
			assert(mDataSize && "No allocation has been made. Did you forget to call Allocate()?");
			assert(((mIndexType==IndexTypes::UNSIGNED_16BIT && sizeof(T)==2) ||
					(mIndexType==IndexTypes::UNSIGNED_32BIT && sizeof(T)==4))
					&& "The specified type does not match the element size.");
			return Accessor<T>(mData, GetIndexSize(), mNumberOfIndices, mIndexType, mElementType);
		}
		
		/**
		 * Get access to const indices.
		 */
		template<typename T>
		const Accessor<T> GetIndexAccessor() const
		{
			assert(mDataSize && "No allocation has been made. Did you forget to call Allocate()?");
			assert(((mIndexType==IndexTypes::UNSIGNED_16BIT && sizeof(T)==2) ||
					(mIndexType==IndexTypes::UNSIGNED_32BIT && sizeof(T)==4))
					&& "The specified type does not match the element size.");
			return Accessor<T>(mData, GetIndexSize(), mNumberOfIndices, mIndexType, mElementType);
		}
		
		Size GetBufferSize() const {return mDataSize;}

		/**
		 * Set the number of elements this ElementBuffer has had written.
		 * @note This value should not exceed the capacity of the buffer, the number of elements will
		 * be clamped to the capacity. In debug mode the method will assert.
		 * @param numberOfElements The number of elements written to the buffer.
		 */
		void SetNumberOfElements(Size numberOfElements);

		/**
		 * Get the number of elements this buffer contains.
		 * @note For convenience, if SetNumberOfElements() has not been called then this method will
		 * return the capacity since it is assumed that a ElementBuffer is used entirely.
		 * @return The number of elements this ElementBuffer contains.
		 */
		Size GetNumberOfElements() const {return mNumberOfElements;}

		/**
		 * Get the number of indices this buffer contains.
		 * @return The number of indices this ElementBuffer contains.
		 */
		Size GetNumberOfIndices() const {return mNumberOfIndices;}
		
		/**
		 * Get the capacity of the element buffer, this is the number of elements this buffer can have.
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
		
		/**
		 * Get the byte stride between elements.
		 * This is not necessarily the element size if the element type is overlapping such as
		 * in the case of strip element types.
		 */
		inline Size GetElementStride() const
		{
			switch(mElementType)
			{
				case ElementTypes::POINTS:
					return GetIndexSize();
				case ElementTypes::LINES:
					return 2*GetIndexSize();
				case ElementTypes::LINE_STRIP:
					return GetIndexSize();
				case ElementTypes::LINE_LOOP:
					return GetIndexSize();
				case ElementTypes::TRIANGLE:
					return 3*GetIndexSize();
				case ElementTypes::TRIANGLE_STRIP:
					return GetIndexSize();
				case ElementTypes::TRIANGLE_FAN:
					return GetIndexSize();
				case ElementTypes::UNDEFINED:
					break;
			}
			assert(false && "Cannot calculate element size. Element type is not valid.");
			return 0;
		}

		/**
		 * Get the element size in bytes
		 */
		inline Size GetElementSize() const
		{
			switch(mElementType)
			{
				case ElementTypes::POINTS:
					return GetIndexSize();
				case ElementTypes::LINES:
					return 2*GetIndexSize();
				case ElementTypes::LINE_STRIP:
					return 2*GetIndexSize();
				case ElementTypes::LINE_LOOP:
					return 2*GetIndexSize();
				case ElementTypes::TRIANGLE:
					return 3*GetIndexSize();
				case ElementTypes::TRIANGLE_STRIP:
					return 3*GetIndexSize();
				case ElementTypes::TRIANGLE_FAN:
					return 3*GetIndexSize();
				case ElementTypes::UNDEFINED:
					break;
			}
			assert(false && "Cannot calculate element size. Element type is not valid.");
			return 0;
		}
		
		/**
		 * Get the size of each index
		 * @return 
		 */
		inline Size GetIndexSize() const
		{
			if(mIndexType==IndexTypes::UNSIGNED_16BIT)
			{
				return 2;
			}
			return 4;
		}
	private:
		Size CalculateNumberOfIndices(Size numberOfElements) const;
		Size CalculateBufferSize(Size numberOfElements) const;
		
		Type mType;
		ElementType mElementType;
		IndexType mIndexType;
		Size mVersion;
		char* mData;
		Size mDataSize;
		Size mNumberOfElements;
		Size mNumberOfIndices;
		Size mCapacity;
	};	
}
#endif
