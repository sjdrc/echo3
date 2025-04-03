#include <echo/GUI/Container.h>

namespace Echo
{
	namespace GUI
	{
		Container::Container()
		{
			SetTargetable(false);
		}
		
		Container::~Container()
		{
		}

		Container::Container(const Container& other)
		{
			*this=other;
		}
		
		Container& Container::operator=(const Container& rhs)
		{
			if(this==&rhs)
			{
				return *this;
			}
			Element::operator=(rhs);
			mWidth = rhs.mWidth;
			mHeight = rhs.mHeight;
			return *this;
		}

		shared_ptr<Element> Container::_Clone() const
		{
			return shared_ptr<Element>(new Container(*this));
		}

		void Container::UpdateSize(Scalar newWidth, Scalar newHeight)
		{
			//Store for later if necessary.
			mWidth = newWidth;
			mHeight = newHeight;
			
			//Nothing else to do really.
		}

		Scalar Container::GetNativeAspectRatio()
		{
			//We just use the set ratio.
			if(mHeight==0.f)
			{
				return 1.f;
			}
			return (mWidth/mHeight);
		}
	}
}
