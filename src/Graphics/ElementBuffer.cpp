#include <echo/Graphics/ElementBuffer.h>

namespace Echo
{
	ElementBuffer::ElementBuffer(Type type) :
		mType(type),
		mElementType(ElementTypes::UNDEFINED),
		mIndexType(IndexTypes::UNDEFINED),
		mVersion(0),
		mData(nullptr),
		mDataSize(0),
		mNumberOfElements(0),
		mNumberOfIndices(0),
		mCapacity(0)
	{}
	
	ElementBuffer::~ElementBuffer()
	{
		Deallocate();
	}
	
	ElementBuffer::ElementBuffer(const ElementBuffer& rhs) :
		mType(rhs.mType),
		mElementType(rhs.mElementType),
		mIndexType(rhs.mIndexType),
		mVersion(rhs.mVersion),
		mNumberOfElements(rhs.mNumberOfElements),
		mNumberOfIndices(rhs.mNumberOfIndices),
		mCapacity(rhs.mCapacity)
	{
		if(rhs.mDataSize!=0)
		{
			mData = new char[rhs.mDataSize];
			std::copy(rhs.mData,rhs.mData+rhs.mDataSize,mData);
			mDataSize = rhs.mDataSize;
		}else
		{
			mData = nullptr;
			mDataSize = 0;
		}
	}
	
	ElementBuffer::ElementBuffer(ElementBuffer&& rhs) :
		mType(rhs.mType),
		mElementType(rhs.mElementType),
		mIndexType(rhs.mIndexType),
		mVersion(rhs.mVersion),
		mNumberOfElements(rhs.mNumberOfElements),
		mNumberOfIndices(rhs.mNumberOfIndices),
		mCapacity(rhs.mCapacity)
	{
		mData = rhs.mData;
		mDataSize = rhs.mDataSize;
		rhs.mData = nullptr;
		rhs.mDataSize = 0;
		rhs.mCapacity = 0;
		rhs.mNumberOfIndices = 0;
		rhs.mNumberOfElements = 0;
		rhs.mVersion = 0;
		rhs.mIndexType = IndexTypes::UNDEFINED;
		rhs.mElementType = ElementTypes::UNDEFINED;
		// Type doesn't really matter .
	}
	
	ElementBuffer& ElementBuffer::operator=(const ElementBuffer& rhs)
	{
		if(&rhs == this)
		{
			return *this;
		}
		delete [] mData;
		*this = rhs;
		return *this;
	}
	
	ElementBuffer& ElementBuffer::operator=(ElementBuffer&& rhs)
	{
		if(&rhs == this)
		{
			return *this;
		}
		mType = rhs.mType;
		mElementType = rhs.mElementType;
		mIndexType = rhs.mIndexType;
		mVersion = rhs.mVersion;
		mNumberOfElements = rhs.mNumberOfElements;
		mNumberOfIndices = rhs.mNumberOfIndices;
		mCapacity = rhs.mCapacity;

		delete [] mData;
		mData = rhs.mData;
		mDataSize = rhs.mDataSize;
		rhs.mData = nullptr;
		rhs.mDataSize = 0;
		rhs.mCapacity = 0;
		rhs.mNumberOfIndices = 0;
		rhs.mNumberOfElements = 0;
		rhs.mVersion = 0;
		rhs.mIndexType = IndexTypes::UNDEFINED;
		rhs.mElementType = ElementTypes::UNDEFINED;
		// Type doesn't really matter.
		return *this;
	}

	ElementBuffer::IndexType ElementBuffer::GetIndexType() const
	{
		return mIndexType;
	}
	
	ElementBuffer::ElementType ElementBuffer::GetElementType() const
	{
		return mElementType;
	}

	bool ElementBuffer::Allocate(Size numberOfElements, bool onlyIfNeeded)
	{
		if(numberOfElements>mCapacity || !onlyIfNeeded)
		{
			return Allocate(mIndexType, mElementType, numberOfElements);
		}
		return true;
	}
	
	bool ElementBuffer::Allocate(IndexType indexType, ElementType elementType, Size numberOfElements)
	{
		if(indexType==IndexTypes::UNDEFINED || elementType==ElementTypes::UNDEFINED || numberOfElements==0)
		{
			return false;
		}
		mIndexType = indexType;
		mElementType = elementType;
		mCapacity = numberOfElements;
		mNumberOfElements = numberOfElements;
		mNumberOfIndices = CalculateNumberOfIndices(numberOfElements);
		mDataSize = CalculateBufferSize(numberOfElements);
		delete [] mData;
		mData = new char[mDataSize];
		return true;
	}
	
	void ElementBuffer::Deallocate()
	{
		delete [] mData;
		mData = nullptr;
		mDataSize = 0;
		mNumberOfElements = 0;
		mNumberOfIndices = 0;
		mCapacity = 0;
	}
	
	void ElementBuffer::SetNumberOfElements(Size numberOfElements)
	{
		assert(numberOfElements<=mCapacity && "Number of elements should be no larger than the capacity of the ElementBuffer. The value will be capped.");
		mNumberOfElements = std::min(mCapacity,numberOfElements);
		mNumberOfIndices = CalculateNumberOfIndices(mNumberOfElements);
	}
	
	Size ElementBuffer::CalculateNumberOfIndices(Size numberOfElements) const
	{
		switch(mElementType)
		{
			case ElementTypes::POINTS:
				return numberOfElements;
			case ElementTypes::LINES:
				return numberOfElements * 2;
			case ElementTypes::LINE_STRIP:
				return (numberOfElements + 1);
			case ElementTypes::LINE_LOOP:
				return (numberOfElements + 1);
			case ElementTypes::TRIANGLE:
				return numberOfElements * 3;
			case ElementTypes::TRIANGLE_STRIP:
				return (2+numberOfElements);
			case ElementTypes::TRIANGLE_FAN:
				return (2+numberOfElements);
			case ElementTypes::UNDEFINED:
				break;
		}
		assert(false && "Cannot calculate buffer size. Element type is not valid.");
		return 0;
	}


	Size ElementBuffer::CalculateBufferSize(Size numberOfElements) const
	{
		switch(mElementType)
		{
			case ElementTypes::POINTS:
				return GetIndexSize() * numberOfElements;
			case ElementTypes::LINES:
				return 2 * GetIndexSize() * numberOfElements;
			case ElementTypes::LINE_STRIP:
				return GetIndexSize() * (numberOfElements + 1);
			case ElementTypes::LINE_LOOP:
				return GetIndexSize() * (numberOfElements + 1);
			case ElementTypes::TRIANGLE:
				return 3 * GetIndexSize() * numberOfElements;
			case ElementTypes::TRIANGLE_STRIP:
				return GetIndexSize() * (2+numberOfElements);
			case ElementTypes::TRIANGLE_FAN:
				return GetIndexSize() * (2+numberOfElements);
			case ElementTypes::UNDEFINED:
				break;
		}
		assert(false && "Cannot calculate buffer size. Element type is not valid.");
		return 0;
	}
}
