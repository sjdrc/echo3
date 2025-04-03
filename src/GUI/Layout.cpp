#include <echo/GUI/Layout.h>
#include <boost/foreach.hpp>

namespace Echo
{
	namespace GUI
	{
		Layout::Layout(Direction direction, PlacementMode placementMode) :
			mDirection(direction),
			mPlacementMode(placementMode),
			mUseOptimalChildSettings(true)
		{
		}

		Layout::~Layout()
		{

		}

		Layout::Layout(const Layout& other)
		{
			*this=other;
		}
		
		Layout& Layout::operator=(const Layout& rhs)
		{
			if(this==&rhs)
			{
				return *this;
			}
			Container::operator=(rhs);
			mDirection = rhs.mDirection;;
			mPlacementMode = rhs.mPlacementMode;
			mUseOptimalChildSettings = rhs.mUseOptimalChildSettings;
			//mOrderedChildren;			// Leave.
			return *this;
		}
		
		shared_ptr<Element> Layout::_Clone() const
		{
			return shared_ptr<Element>(new Container(*this));
		}
		
		void Layout::SetUseOptimalChildSettings(bool useOptimalChildSettings)
		{
			if(mUseOptimalChildSettings!=useOptimalChildSettings)
			{
				mUseOptimalChildSettings=useOptimalChildSettings;
				if(mUseOptimalChildSettings)
				{
					SetOptimalChildSettings();
				}
				UpdateChildren();
			}
		}
		
		void Layout::SetDirection(Direction direction)
		{
			if(mDirection!=direction)
			{
				mDirection = direction;
				if(mUseOptimalChildSettings)
				{
					SetOptimalChildSettings();
				}
				UpdateChildren();
			}
		}
		
		void Layout::SetPlacementMode(PlacementMode placementMode)
		{
			if(mPlacementMode!=placementMode)
			{
				mPlacementMode = placementMode;
				if(mUseOptimalChildSettings)
				{
					SetOptimalChildSettings();
				}
				UpdateChildren();
			}
		}
		
		void Layout::AddElement(shared_ptr<Element> element)
		{
			mOrderedChildren.push_back(element.get());
			Element::AddElement(element);
			UpdateChildren();
		}
		
		void Layout::RemoveElement(shared_ptr<Element> element)
		{
			RemoveElement(*element);
		}
		
		void Layout::RemoveElement(Element& element)
		{
			mOrderedChildren.remove(&element);
			Element::RemoveElement(element);
			UpdateChildren();
		}
		
		void Layout::SetOptimalChildSettings()
		{
			if(mPlacementMode==PlacementModes::FIT_CONTENTS)
			{
				switch(mDirection)
				{
					case Directions::HORIZONTAL_MAX_TO_MIN:	//Fall through
					case Directions::HORIZONTAL_MIN_TO_MAX:
					{
						BOOST_FOREACH(Element* element, mOrderedChildren)
						{
							element->SetVerticalAlignment(Alignments::CENTRE);
							element->SetSizeMode(SizeModes::WIDTH_AND_CALCULATE_ASPECT_RATIO);
						}
					}break;
					case Directions::VERTICAL_MAX_TO_MIN:	//Fall through
					case Directions::VERTICAL_MIN_TO_MAX:
					{
						BOOST_FOREACH(Element* element, mOrderedChildren)
						{
							element->SetHorizontalAlignment(Alignments::CENTRE);
							element->SetSizeMode(SizeModes::HEIGHT_AND_CALCULATE_ASPECT_RATIO);
						}
					}break;
				}
			} else
			{
				switch(mDirection)
				{
					case Directions::HORIZONTAL_MAX_TO_MIN:	//Fall through
					case Directions::HORIZONTAL_MIN_TO_MAX:
					{
						BOOST_FOREACH(Element* element, mOrderedChildren)
						{
							element->SetVerticalAlignment(Alignments::CENTRE);
						}
					}break;
					case Directions::VERTICAL_MAX_TO_MIN:	//Fall through
					case Directions::VERTICAL_MIN_TO_MAX:
					{
						BOOST_FOREACH(Element* element, mOrderedChildren)
						{
							element->SetHorizontalAlignment(Alignments::CENTRE);
						}
					}break;
				}
			}
		}

		void Layout::UpdateChildren()
		{
			switch(mDirection)
			{
				case Directions::HORIZONTAL_MIN_TO_MAX:
				{
					ChildrenHorizontalMinToMax();
				}break;
				case Directions::HORIZONTAL_MAX_TO_MIN:
				{
					ChildrenHorizontalMaxToMin();
				}break;
				case Directions::VERTICAL_MIN_TO_MAX:
				{
					ChildrenVerticalMinToMax();
				}break;
				case Directions::VERTICAL_MAX_TO_MIN:
				{
					ChildrenVerticalMaxToMin();
				}break;
			}
		}

		void Layout::ChildrenHorizontalMinToMax()
		{
			switch(mPlacementMode)
			{
				case PlacementModes::FIT_CONTENTS:
				{
					Vector3 currentPosition(0,0.5,0);
					Scalar totalWidth = 0.f;
					BOOST_FOREACH(Element* element, mOrderedChildren)
					{
						totalWidth+=element->GetRelativeWidth();
					}
					Scalar scaleFactor = 1.f/totalWidth;
					BOOST_FOREACH(Element* element, mOrderedChildren)
					{
						Scalar width = element->GetRelativeWidth()*scaleFactor;
						element->SetWidth(width);
						element->SetHorizontalAlignment(Alignments::MINIMUM);
						element->SetRelativePosition(currentPosition);
						currentPosition.x+=width;
					}
				}break;
				case PlacementModes::ALLOW_OVERFLOW:
				{
					Vector3 currentPosition(0,0.5,0);
					BOOST_FOREACH(Element* element, mOrderedChildren)
					{
						element->SetHorizontalAlignment(Alignments::MINIMUM);
						element->SetRelativePosition(currentPosition);
						currentPosition.x+=element->GetRelativeWidth();
					}
				}
			}
		}

		void Layout::ChildrenHorizontalMaxToMin()
		{
			switch(mPlacementMode)
			{
				case PlacementModes::FIT_CONTENTS:
				{
					Vector3 currentPosition(1.,0.5,0);
					Scalar totalWidth = 0.f;
					BOOST_FOREACH(Element* element, mOrderedChildren)
					{
						totalWidth+=element->GetRelativeWidth();
					}
					Scalar scaleFactor = 1.f/totalWidth;
					BOOST_FOREACH(Element* element, mOrderedChildren)
					{
						Scalar width = element->GetRelativeWidth()*scaleFactor;
						element->SetWidth(width);
						element->SetHorizontalAlignment(Alignments::MAXIMUM);
						element->SetRelativePosition(currentPosition);
						currentPosition.x-=width;
					}
				}break;
				case PlacementModes::ALLOW_OVERFLOW:
				{
					Vector3 currentPosition(1.,0.5,0);
					BOOST_FOREACH(Element* element, mOrderedChildren)
					{
						element->SetHorizontalAlignment(Alignments::MAXIMUM);
						element->SetRelativePosition(currentPosition);
						currentPosition.x-=element->GetRelativeWidth();
					}
				}
			}
		}
		
		void Layout::ChildrenVerticalMinToMax()
		{
			switch(mPlacementMode)
			{
				case PlacementModes::FIT_CONTENTS:
				{
					Vector3 currentPosition(0.5f,0,0);
					Scalar totalHeight = 0.f;
					BOOST_FOREACH(Element* element, mOrderedChildren)
					{
						totalHeight+=element->GetRelativeHeight();
					}
					Scalar scaleFactor = 1.f/totalHeight;
					BOOST_FOREACH(Element* element, mOrderedChildren)
					{
						Scalar height = element->GetRelativeHeight()*scaleFactor;
						element->SetHeight(height);
						element->SetVerticalAlignment(Alignments::MINIMUM);
						element->SetRelativePosition(currentPosition);
						currentPosition.y+=height;
					}
				}break;
				case PlacementModes::ALLOW_OVERFLOW:
				{
					Vector3 currentPosition(0.5f,0,0);
					BOOST_FOREACH(Element* element, mOrderedChildren)
					{
						element->SetVerticalAlignment(Alignments::MINIMUM);
						element->SetRelativePosition(currentPosition);
						currentPosition.y+=element->GetRelativeHeight();
					}
				}
			}
		}

		void Layout::ChildrenVerticalMaxToMin()
		{
			switch(mPlacementMode)
			{
				case PlacementModes::FIT_CONTENTS:
				{
					Vector3 currentPosition(0.5f,1.f,0);
					Scalar totalHeight = 0.f;
					BOOST_FOREACH(Element* element, mOrderedChildren)
					{
						totalHeight+=element->GetRelativeHeight();
					}
					Scalar scaleFactor = 1.f/totalHeight;
					BOOST_FOREACH(Element* element, mOrderedChildren)
					{
						Scalar height = element->GetRelativeHeight()*scaleFactor;
						element->SetHeight(height);
						element->SetVerticalAlignment(Alignments::MAXIMUM);
						element->SetRelativePosition(currentPosition);
						currentPosition.y-=height;
					}
				}break;
				case PlacementModes::ALLOW_OVERFLOW:
				{
					Vector3 currentPosition(0.5,1.f,0);
					BOOST_FOREACH(Element* element, mOrderedChildren)
					{
						element->SetVerticalAlignment(Alignments::MAXIMUM);
						element->SetRelativePosition(currentPosition);
						currentPosition.y-=element->GetRelativeHeight();
					}
				}
			}
		}		
	}
}
