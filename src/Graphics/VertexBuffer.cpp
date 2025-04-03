#include <echo/Graphics/VertexBuffer.h>

namespace Echo
{
	VertexBuffer::VertexBuffer(Type type) : Resource<VertexBuffer>(true),
		mType(type),
		mStride(0),
		mData(nullptr),
		mDataSize(0),
		mNumberOfElements(0),
		mCapacity(0)
	{
	}

	VertexBuffer::~VertexBuffer()
	{
		delete [] mData;
	}
	
	VertexBuffer::VertexBuffer(const VertexBuffer& rhs) : Resource<VertexBuffer>(true)
	{
		mType = rhs.mType;
		mStride = rhs.mStride;
		mVertexAttributes = rhs.mVertexAttributes;
		mNamedAttributes = rhs.mNamedAttributes;
		mNumberOfElements = rhs.mNumberOfElements;
		mCapacity = rhs.mCapacity;
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
	
	VertexBuffer::VertexBuffer(VertexBuffer&& rhs) : Resource<VertexBuffer>(true)
	{
		mType = rhs.mType;
		mStride = rhs.mStride;
		mVertexAttributes = rhs.mVertexAttributes;
		mNamedAttributes = rhs.mNamedAttributes;
		mNumberOfElements = rhs.mNumberOfElements;
		mCapacity = rhs.mCapacity;
		mDataSize = rhs.mDataSize;
		mData = rhs.mData;
		rhs.mDataSize = 0;;
		rhs.mData = nullptr;
	}
	
	VertexBuffer& VertexBuffer::operator=(const VertexBuffer& rhs)
	{
		if(&rhs==this)
		{
			return *this;
		}
		mType = rhs.mType;
		mStride = rhs.mStride;
		mVertexAttributes = rhs.mVertexAttributes;
		mNamedAttributes = rhs.mNamedAttributes;
		mNumberOfElements = rhs.mNumberOfElements;
		mCapacity = rhs.mCapacity;
		delete [] mData;
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
		return *this;
	}
	
	VertexBuffer& VertexBuffer::operator=(VertexBuffer&& rhs)
	{
		if(&rhs==this)
		{
			return *this;
		}
		mType = rhs.mType;
		mStride = rhs.mStride;
		mVertexAttributes = rhs.mVertexAttributes;
		mNamedAttributes = rhs.mNamedAttributes;
		mNumberOfElements = rhs.mNumberOfElements;
		mCapacity = rhs.mCapacity;
		delete [] mData;
		mDataSize = rhs.mDataSize;
		mData = rhs.mData;
		rhs.mDataSize = 0;;
		rhs.mData = nullptr;
		return *this;
	}

	Size VertexBuffer::GetStride() const
	{
		return mStride;
	}

	Size VertexBuffer::AddVertexAttribute(VertexAttribute vertexAttribute)
	{
		vertexAttribute.SetOffset(mStride);
		mStride+=vertexAttribute.GetWidth();
		Size position = mVertexAttributes.size();
		mVertexAttributes.push_back(vertexAttribute);
		return position;
	}

	Size VertexBuffer::AddVertexAttribute(std::string name, VertexAttribute vertexAttribute)
	{
		Size position = AddVertexAttribute(std::move(vertexAttribute));
		mNamedAttributes.insert(std::make_pair(std::move(name), position));
		return position;
	}
	
	bool VertexBuffer::Allocate(Size numberOfVertices)
	{
		if(mStride==0)
		{
			ECHO_LOG_ERROR("Cannot allocate a buffer of zero size. The stride is zero.");
			return false;
		}
		delete [] mData;
		mNumberOfElements = numberOfVertices;
		mCapacity = numberOfVertices;
		mDataSize = mStride*mCapacity;
		mData = new char[mDataSize];
		return true;
	}
	
	void VertexBuffer::Deallocate()
	{
		delete [] mData;
		mData = nullptr;
		mDataSize = 0;
		mNumberOfElements = 0;
		mCapacity = 0;
	}
	
	void VertexBuffer::SetNumberOfElements(Size numberOfElements)
	{
		assert(numberOfElements<=mCapacity && "Number of elements should be no larger than the capacity of the VertexBuffer. The value will be capped.");
		mNumberOfElements = std::min(mCapacity,numberOfElements);
	}

	bool VertexBuffer::_Unload()
	{
		return false;
	}

	size_t VertexBuffer::OnRequestMemoryRelease()
	{
		return 0;
	}

}
