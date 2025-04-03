#include <echo/GUI/Element.h>
#include <echo/Util/FunctionBinder.h>
#include <echo/Util/Parsers.h>
#include <boost/foreach.hpp>

namespace Echo
{
	namespace GUI
	{
		Element::Element() :
			mTargetable(true),
			mSetWidth(),
			mSetHeight(),
			mAspectRatio(1.f),
			mWidth(1.f),
			mHeight(1.f),
			mSizeMode(SizeModes::STRETCH),
			mCursorState(CursorStates::NONE),
			mCursorPressed(false),
			mPercentPosition(Vector3::ZERO),
			mParent(nullptr),
			mHorizontalAlignment(Alignments::CENTRE),
			mVerticalAlignment(Alignments::CENTRE)
		{
		}

		Element::~Element()
		{
		}

		Element::Element(const Element& other)
		{
			mParent = 0;
			*this = other;
		}
		
		Element& Element::operator=(const Element& rhs)
		{
			if(this==&rhs)
			{
				return *this;
			}
			
			shared_ptr<Mesh> currentMesh = GetMesh();	//Preserve this objects mesh
			SceneEntity::operator=(rhs);
			SetMesh(currentMesh);						//Restore because SceneEntity copies the Mesh.
			
			//mChildElements		// Leave.
			mEnterFunctions = rhs.mEnterFunctions;
			mHoverFunctions = rhs.mHoverFunctions;
			mLeaveFunctions = rhs.mLeaveFunctions;
			mPressedFunctions = rhs.mPressedFunctions;
			mHoldFunctions = rhs.mHoldFunctions;
			mReleasedFunctions = rhs.mReleasedFunctions;
			mFunctionBinder = rhs.mFunctionBinder;
			mTargetable = rhs.mTargetable;
			mSetWidth = rhs.mSetWidth;
			mSetHeight = rhs.mSetHeight;
			mAspectRatio = rhs.mAspectRatio;
			mWidth = 1.f;			// Will update soon.
			mHeight = 1.f;
			mSizeMode = rhs.mSizeMode;
			mCursorState = CursorStates::NONE;
			mCursorPressed = false;
			mPercentPosition = rhs.mPercentPosition;
			//mParent				// Leave.
			mHorizontalAlignment = rhs.mHorizontalAlignment;
			mVerticalAlignment = rhs.mVerticalAlignment;
			return *this;
		}
		
		AxisAlignedBox Element::GetAxisAlignedBox(bool applyLocalTransform) const
		{
			AxisAlignedBox aabb;
			f32 halfWidth = GetAbsoluteWidth()/2.f;
			f32 halfHeight = GetAbsoluteHeight()/2.f;
			aabb.SetExtents(Vector3(-halfWidth,-halfHeight,0),Vector3(halfWidth,halfHeight,0));
			if(applyLocalTransform)
			{
				aabb.Transform(GetLocalTransform());
			}
			return aabb;
		}
		
		void Element::Show()
		{
			if(!GetVisibilityFlag())
			{
				SetVisibilityFlag(true);
				ResetCursorState();
			}
		}

		void Element::Hide()
		{
			SetVisibilityFlag(false);
		}

		void Element::AddElement(shared_ptr<Element> element)
		{
			if(element && element.get()!=this)
			{
				Element* parentAsElement = dynamic_cast<Element*>(GetParent());
				if(parentAsElement==element.get())
				{
					ECHO_LOG_ERROR("Element:AddElement() Cannot add a parent as a child element.");
					return;
				}
				Node::AddChild(element);
				mChildElements.push_back(element);
				element->SetParentElement(this);
				if(!element->GetFunctionBinder())
				{
					element->SetFunctionBinder(mFunctionBinder);
				}
				element->OnParentSizeUpdate();
				element->OnParentPositionUpdate();
			}else
			{
				ECHO_LOG_ERROR("Element:AddElement() Cannot add a null element or self.");
			}
		}

		void Element::RemoveElement(shared_ptr<Element> element)
		{
			RemoveElement(*element);
		}
		
		void Element::RemoveElement(Element& element)
		{
			Node::RemoveChild(element);
		}
		
		inline bool Element::GetMaxFitWidth(const Scalar& ar, const Scalar& par)
		{
			// The following cover all cases for setting width first based on the aspect ratios.
			// The easiest thing to do is to draw six boxes that make all cases:
			//	- is this element
			//	= is the parent
			//
			// Vertical parent:
			//
			//	   A                   B                  C
			//	=======             =======            -------
			//	=     = ar >= 1     = - - = ar < 1     - = = - ar < 1
			//	=-----= par < 1     = - - = par < 1    - = = - par < 1
			//	=-----= par < ar    = - - = par >= ar  - = = - par < ar
			//	=     =             = - - =            - = = -
			//	=======             =======            -------
			//
			// Horizontal parent:
			//
			//              D
			//	=========================
			//	=        -     -        =
			//	=        -     -        = ar < 1
			//	=        -     -        = par >= 1
			//	=        -     -        = par >= ar
			//	=        -     -        =
			//	=========================
			//
			//              E
			//	=========================
			//  =                       =
			//	=-----------------------= ar >= 1
			//	=                       = par >= 1
			//	=-----------------------= par < ar
			//  =                       =
			//	=========================
			//
			//              F
			//	-------------------------
			//  -                       -
			//	-=======================- ar >= 1
			//	-                       - par >= 1
			//	-=======================- ar > par
			//  -                       -
			//	-------------------------
			//
			// It is worth noting that C and F don't ever get rendered with sizes like that.
			// In the case of F it would end up looking more D (but with ar>=1), and vice
			// versa for C (would look more like E). You could think about and draw C and F
			// inside the parent, however I just visualised it this way.
			//
			// In cases A, C and E we do a width first.

			// Any suggestions for optimisations here?
			return ((ar>=1 && par<1) ||				//A
					(ar<1 && par<1 && par<ar) ||	//C
					(ar>=1 && par >=1 && par<ar));	//E
		}
		
		Scalar Element::GetRelativeWidth()
		{
			switch(mSizeMode)
			{
				case SizeModes::FILL:
				{
					if(GetAspectRatio()==0 || !mParent)
					{
						return 0.f;
					}
					
					Scalar parentAspectRatio = mParent->GetAspectRatio();
					if(parentAspectRatio==0.f)
					{
						return 0.f;
					}
					
					if(!GetMaxFitWidth(GetAspectRatio(),parentAspectRatio))
					{
						return 1.f;
					}
					
					Scalar parentWidth = mParent->GetAbsoluteWidth();
					if(parentWidth==0.f)
					{
						return 0.f;
					}
					return ((mSetHeight.CalculateAsHeight(GetWorldUnitsConverter(),mParent)*GetAspectRatio()) / parentWidth);
				}
				break;
				case SizeModes::HEIGHT_AND_CALCULATE_ASPECT_RATIO:
				{
					Scalar aspectRatio = GetNativeAspectRatio();
					if(aspectRatio==0.f)
					{
						return 0.f;
					}
					Scalar parentWidth = 1.f;
					if(mParent)
					{
						parentWidth = mParent->GetAbsoluteWidth();
						if(parentWidth==0.f)
						{
							return 0.f;
						}
					}
					return ((mSetHeight.CalculateAsHeight(GetWorldUnitsConverter(),mParent) * aspectRatio) / parentWidth);
				}
				break;
				case SizeModes::HEIGHT_WITH_SET_ASPECT_RATIO:
				{
					Scalar parentWidth = 1.f;
					if(mParent)
					{
						parentWidth = mParent->GetAbsoluteWidth();
						if(parentWidth==0.f)
						{
							return 0.f;
						}
					}
					return ((mSetHeight.CalculateAsHeight(GetWorldUnitsConverter(),mParent) * mAspectRatio) / parentWidth);
				}break;
				case SizeModes::MAX_FIT_WITH_SET_ASPECT_RATIO:
				{
					if(mAspectRatio==0 || !mParent)
					{
						return 0.f;
					}
					
					Scalar parentAspectRatio = mParent->GetAspectRatio();
					if(parentAspectRatio==0.f)
					{
						return 0.f;
					}
					
					if(GetMaxFitWidth(mAspectRatio,parentAspectRatio))
					{
						return 1.f;
					}
					
					Scalar parentWidth = mParent->GetAbsoluteWidth();
					if(parentWidth==0.f)
					{
						return 0.f;
					}
					return ((mSetHeight.CalculateAsHeight(GetWorldUnitsConverter(),mParent)*mAspectRatio) / parentWidth);
				}
				case SizeModes::FILL_WITH_SET_ASPECT_RATIO:
				{
					if(mAspectRatio==0 || !mParent)
					{
						return 0.f;
					}
					
					Scalar parentAspectRatio = mParent->GetAspectRatio();
					if(parentAspectRatio==0.f)
					{
						return 0.f;
					}
					
					if(!GetMaxFitWidth(mAspectRatio,parentAspectRatio))
					{
						return 1.f;
					}
					
					Scalar parentWidth = mParent->GetAbsoluteWidth();
					if(parentWidth==0.f)
					{
						return 0.f;
					}
					return ((mSetHeight.CalculateAsHeight(GetWorldUnitsConverter(),mParent)*mAspectRatio) / parentWidth);
				}
				break;
				case SizeModes::WIDTH_AND_CALCULATE_ASPECT_RATIO:
				case SizeModes::WIDTH_WITH_SET_ASPECT_RATIO:
				case SizeModes::STRETCH:
				default:
				{
					Scalar parentWidth = 1.f;
					if(mParent)
					{
						parentWidth = mParent->GetAbsoluteWidth();
						if(parentWidth==0.f)
						{
							return 0.f;
						}
					}
					return (mSetWidth.CalculateAsWidth(GetWorldUnitsConverter(),mParent) / parentWidth);
				}
			}
			assert(false && "mSizeMode is invalid.");
			//Catch all.
			return 0.f;
		}

		Scalar Element::GetRelativeHeight()
		{
			switch(mSizeMode)
			{
				case SizeModes::FILL:
				{
					if(GetAspectRatio()==0 || !mParent)
					{
						return 0.f;
					}
					
					Scalar parentAspectRatio = mParent->GetAspectRatio();
					if(parentAspectRatio==0.f)
					{
						return 0.f;
					}
					
					if(!GetMaxFitWidth(GetAspectRatio(),parentAspectRatio))
					{
						Scalar parentHeight = 1.f;
						if(mParent)
						{
							parentHeight = mParent->GetAbsoluteHeight();
							if(parentHeight==0.f)
							{
								return 0.f;
							}
						}
						return ((mSetWidth.CalculateAsWidth(GetWorldUnitsConverter(),mParent) / GetAspectRatio()) / parentHeight);
					}
					return 1.f;
				}break;
				case SizeModes::WIDTH_AND_CALCULATE_ASPECT_RATIO:
				{
					Scalar aspectRatio = GetNativeAspectRatio();
					if(aspectRatio==0.f)
					{
						return 0.f;
					}
					Scalar parentHeight = 1.f;
					if(mParent)
					{
						parentHeight = mParent->GetAbsoluteHeight();
						if(parentHeight==0.f)
						{
							return 0.f;
						}
					}
					return ((mSetWidth.CalculateAsWidth(GetWorldUnitsConverter(),mParent) / GetNativeAspectRatio()) / parentHeight);
				}break;
				case SizeModes::WIDTH_WITH_SET_ASPECT_RATIO:
				{
					if(mAspectRatio==0.f)
					{
						return 0.f;
					}
					Scalar parentHeight = 1.f;
					if(mParent)
					{
						parentHeight = mParent->GetAbsoluteHeight();
						if(parentHeight==0.f)
						{
							return 0.f;
						}
					}
					return ((mSetWidth.CalculateAsWidth(GetWorldUnitsConverter(),mParent) / mAspectRatio) / parentHeight);
				}break;
				case SizeModes::FILL_WITH_SET_ASPECT_RATIO:
				{
					if(mAspectRatio==0 || !mParent)
					{
						return 0.f;
					}
					
					Scalar parentAspectRatio = mParent->GetAspectRatio();
					if(parentAspectRatio==0.f)
					{
						return 0.f;
					}
					
					if(!GetMaxFitWidth(mAspectRatio,parentAspectRatio))
					{
						Scalar parentHeight = 1.f;
						if(mParent)
						{
							parentHeight = mParent->GetAbsoluteHeight();
							if(parentHeight==0.f)
							{
								return 0.f;
							}
						}
						return ((mSetWidth.CalculateAsWidth(GetWorldUnitsConverter(),mParent) / mAspectRatio) / parentHeight);
					}
					return 1.f;
				}break;
				case SizeModes::MAX_FIT_WITH_SET_ASPECT_RATIO:
				{
					if(mAspectRatio==0 || !mParent)
					{
						return 0.f;
					}
					
					Scalar parentAspectRatio = mParent->GetAspectRatio();
					if(parentAspectRatio==0.f)
					{
						return 0.f;
					}
					
					if(GetMaxFitWidth(mAspectRatio,parentAspectRatio))
					{
						Scalar parentHeight = 1.f;
						if(mParent)
						{
							parentHeight = mParent->GetAbsoluteHeight();
							if(parentHeight==0.f)
							{
								return 0.f;
							}
						}
						return ((mSetWidth.CalculateAsWidth(GetWorldUnitsConverter(),mParent) / mAspectRatio) / parentHeight);
					}
					return 1.f;
				}break;
				case SizeModes::HEIGHT_AND_CALCULATE_ASPECT_RATIO:
				case SizeModes::HEIGHT_WITH_SET_ASPECT_RATIO:
				case SizeModes::STRETCH:
				default:
				{
					Scalar parentHeight = 1.f;
					if(mParent)
					{
						parentHeight = mParent->GetAbsoluteHeight();
						if(parentHeight==0.f)
						{
							return 0.f;
						}
					}
					return (mSetHeight.CalculateAsHeight(GetWorldUnitsConverter(),mParent) / parentHeight) ;
				}
				break;
			}
			assert(false && "mSizeMode is invalid.");
			//Catch all.
			return 0.f;
		}
		
		Scalar Element::GetAbsoluteWidth() const
		{
			return mWidth;
		}

		Scalar Element::GetAbsoluteHeight() const
		{
			return mHeight;
		}

		Scalar Element::GetAspectRatio() const
		{
			if(mHeight>0.f)
			{
				return mWidth/mHeight;
			}
			return 0.f;
		}

		void Element::SetAspectRatio(Scalar aspectRatio)
		{
			if(mAspectRatio!=aspectRatio)
			{
				mAspectRatio=aspectRatio;
				SizeUpdated();
			}
		}

		void Element::SetSize(Scalar width, Scalar height)
		{
			assert(width>=0.f && "Cannot have a negative width.");
			assert(height>=0.f && "Cannot have a negative height.");
			if(width>=0.f && height>=0.f &&
				(width!=mSetWidth.GetScalar() || height!=mSetHeight.GetScalar() ||
				 !mSetWidth.IsRelative() || !mSetHeight.IsRelative())
				 )
			{
				mSetWidth.SetRelative(width);
				mSetHeight.SetRelative(height);
				SizeUpdated();
			}
		}
		
		void Element::SetPhysicalSize(Metre width, Metre height)
		{
			assert(width.Count()>=0.f && "Cannot have a negative width.");
			assert(height.Count()>=0.f && "Cannot have a negative height.");
			if(width.Count()>=0.f && height.Count()>=0.f &&
				(width!=mSetWidth.GetPhysical() || height!=mSetHeight.GetPhysical() ||
				 !mSetWidth.IsPhysical() || !mSetHeight.IsPhysical())
				 )
			{
				mSetWidth.SetPhysical(width);
				mSetHeight.SetPhysical(height);
				SizeUpdated();
			}
		}
		
		void Element::SetSizeInPixels(Scalar width, Scalar height)
		{
			assert(width>=0.f && "Cannot have a negative width.");
			assert(height>=0.f && "Cannot have a negative height.");
			if(width>=0.f && height>=0.f &&
				(width!=mSetWidth.GetScalar() || height!=mSetHeight.GetScalar() ||
				 !mSetWidth.IsPixel() || !mSetHeight.IsPixel())
				 )
			{
				mSetWidth.SetPixel(width);
				mSetHeight.SetPixel(height);
				SizeUpdated();
			}
		}
		
		void Element::SetWidth(Scalar width)
		{
			assert(width>=0.f && "Cannot have a negative width.");
			if(width>=0.f && (width!=mSetWidth.GetScalar() || !mSetWidth.IsRelative()))
			{
				mSetWidth.SetRelative(width);
				SizeUpdated();
			}
		}

		void Element::SetPhysicalWidth(Metre width)
		{
			assert(width.Count()>=0.f && "Cannot have a negative width.");
			if(width.Count()>=0.f && (width!=mSetWidth.GetPhysical() || !mSetWidth.IsPhysical()))
			{
				mSetWidth.SetPhysical(width);
				SizeUpdated();
			}
		}
		
		void Element::SetWidthInPixels(Scalar width)
		{
			assert(width>=0.f && "Cannot have a negative width.");
			if(width>=0.f && (width!=mSetWidth.GetScalar() || !mSetWidth.IsPixel()))
			{
				mSetWidth.SetPixel(width);
				SizeUpdated();
			}
		}
		
		void Element::SetHeight(Scalar height)
		{
			assert(height>=0.f && "Cannot have a negative height.");
			if(height>=0.f && (height!=mSetHeight.GetScalar() || !mSetWidth.IsRelative()))
			{
				mSetHeight.SetRelative(height);
				SizeUpdated();
			}
		}

		void Element::SetPhysicalHeight(Metre height)
		{
			assert(height.Count()>=0.f && "Cannot have a negative height.");
			if(height.Count()>=0.f && (height!=mSetHeight.GetPhysical() || !mSetHeight.IsPhysical()))
			{
				mSetHeight.SetPhysical(height);
				SizeUpdated();
			}
		}
		
		void Element::SetHeightInPixels(Scalar height)
		{
			assert(height>=0.f && "Cannot have a negative width.");
			if(height>=0.f && (height!=mSetHeight.GetScalar() || !mSetHeight.IsPixel()))
			{
				mSetHeight.SetPixel(height);
				SizeUpdated();
			}
		}

		void Element::SizeUpdated()
		{
			SizeMode sizeMode = mSizeMode;
			switch(sizeMode)
			{
				case SizeModes::STRETCH:
					if(mSetWidth.IsZeroOrNegative() || mSetHeight.IsZeroOrNegative())
					{
						mWidth = 0;
						mHeight = 0;
						break;
					}
					mWidth=mSetWidth.CalculateAsWidth(GetWorldUnitsConverter(),mParent);
					mHeight=mSetHeight.CalculateAsHeight(GetWorldUnitsConverter(),mParent);
					break;
				case SizeModes::FILL:
				{
					if(mAspectRatio==0 || !mParent)
					{
						mWidth = 0;
						mHeight = 0;
						break;
					}
					
					Scalar parentAspectRatio = mParent->GetAspectRatio();
					if(parentAspectRatio==0.f)
					{
						mWidth=0;
						mHeight=0;
						break;
					}
					
					if(GetMaxFitWidth(mAspectRatio,parentAspectRatio))
					{
						mHeight = mParent->GetAbsoluteHeight();
						mWidth=mHeight *  GetNativeAspectRatio();
					}else
					{
						mWidth = mParent->GetAbsoluteWidth();
						mHeight=mWidth / GetNativeAspectRatio();
					}
				}break;
				case SizeModes::WIDTH_AND_CALCULATE_ASPECT_RATIO:
					if(mSetWidth.IsZeroOrNegative())
					{
						mWidth = 0;
						mHeight = 0;
						break;
					}
					mWidth=mSetWidth.CalculateAsWidth(GetWorldUnitsConverter(),mParent);
					mHeight=mWidth / GetNativeAspectRatio();
					break;
				break;
				case SizeModes::WIDTH_WITH_SET_ASPECT_RATIO:
					if(mSetWidth.IsZeroOrNegative() || mSetHeight.IsZeroOrNegative() || mAspectRatio<=0.f)
					{
						mWidth = 0;
						mHeight = 0;
						break;
					}
					mWidth=mSetWidth.CalculateAsWidth(GetWorldUnitsConverter(),mParent);
					mHeight=mWidth / mAspectRatio;
					break;
				case SizeModes::HEIGHT_AND_CALCULATE_ASPECT_RATIO:
					if(mSetHeight.IsZeroOrNegative())
					{
						mWidth = 0;
						mHeight = 0;
						break;
					}
					mHeight=mSetHeight.CalculateAsHeight(GetWorldUnitsConverter(),mParent);
					mWidth=mHeight * GetNativeAspectRatio();
					break;
				case SizeModes::HEIGHT_WITH_SET_ASPECT_RATIO:
					if(mSetWidth.IsZeroOrNegative() || mSetHeight.IsZeroOrNegative() || mAspectRatio<=0.f)
					{
						mWidth = 0;
						mHeight = 0;
						break;
					}
					mHeight=mSetHeight.CalculateAsHeight(GetWorldUnitsConverter(),mParent);
					mWidth=mHeight * mAspectRatio;
					break;
				case SizeModes::FILL_WITH_SET_ASPECT_RATIO:
				{
					if(mAspectRatio==0 || !mParent)
					{
						mWidth = 0;
						mHeight = 0;
						break;
					}
					
					Scalar parentAspectRatio = mParent->GetAspectRatio();
					if(parentAspectRatio==0.f)
					{
						mWidth=0;
						mHeight=0;
						break;
					}
					
					if(GetMaxFitWidth(mAspectRatio,parentAspectRatio))
					{
						mHeight = mParent->GetAbsoluteHeight();
						mWidth=mHeight * mAspectRatio;
					}else
					{
						mWidth = mParent->GetAbsoluteWidth();
						mHeight=mWidth / mAspectRatio;
					}
				}break;
				case SizeModes::MAX_FIT_WITH_SET_ASPECT_RATIO:
				{
					if(mAspectRatio==0 || !mParent)
					{
						mWidth = 0;
						mHeight = 0;
						break;
					}
					
					Scalar parentAspectRatio = mParent->GetAspectRatio();
					if(parentAspectRatio==0.f)
					{
						mWidth=0;
						mHeight=0;
						break;
					}
					
					if(GetMaxFitWidth(mAspectRatio,parentAspectRatio))
					{
						mWidth = mParent->GetAbsoluteWidth();
						mHeight=mWidth / mAspectRatio;
					}else
					{
						mHeight = mParent->GetAbsoluteHeight();
						mWidth=mHeight * mAspectRatio;
					}
				}break;
				default:
					mWidth=0.f;
					mHeight=0.f;
					break;
			}
			UpdateSize(mWidth, mHeight);
			ProcessConditionals();
			BOOST_FOREACH(shared_ptr<Element> element, mChildElements)
			{
				element->OnParentSizeUpdate();
				element->OnParentPositionUpdate();
			}
			//When the size changes, the position needs to update for non centred Element.
			if(mHorizontalAlignment!=Alignments::CENTRE ||
				mVerticalAlignment!=Alignments::CENTRE)
			{
				PositionUpdated();
			}
		}

		void Element::PositionUpdated()
		{
			Vector3 newPosition = mPercentPosition;
			if(mParent)
			{
				newPosition.x=(mPercentPosition.x - 0.5f)*mParent->GetAbsoluteWidth();
				newPosition.y=(mPercentPosition.y - 0.5f)*mParent->GetAbsoluteHeight();
			}
			
			switch(mHorizontalAlignment)
			{
				case Alignments::MINIMUM:
					newPosition.x+=mWidth/2;
				break;
				case Alignments::MAXIMUM:
					newPosition.x-=mWidth/2;
				break;
				case Alignments::CENTRE:	//Fall through to default. This is the default.
				default:break;
			}
			
			switch(mVerticalAlignment)
			{
				case Alignments::MINIMUM:
					newPosition.y+=mHeight/2;
				break;
				case Alignments::MAXIMUM:
					newPosition.y-=mHeight/2;
				break;
				case Alignments::CENTRE:	//Fall through to default. This is the default.
				default:break;
			}
			
			Node::SetPosition(newPosition);
			BOOST_FOREACH(shared_ptr<Element> element, mChildElements)
			{
				element->OnParentPositionUpdate();
			}
		}
		
		void Element::AddTargetablesToList(std::vector< shared_ptr<Element> >& targetableElements)
		{
			if(!GetVisibilityFlag())
			{
				return;
			}
			if(mTargetable)
			{
				targetableElements.push_back(shared_from_this<Element>());
			}
			BOOST_FOREACH(shared_ptr<Element> element, mChildElements)
			{
				element->AddTargetablesToList(targetableElements);
			}
		}
			
		void Element::UpdateCursor(Scalar absoluteX, Scalar absoluteY, bool pressed, IntersectTestMethod intersectMethod)
		{
			struct Sorter
			{
				static bool DistanceCompareZ(const shared_ptr<Element>& a, const shared_ptr<Element>& b)
				{
					return (a->GetDerivedPosition().z<b->GetDerivedPosition().z);
				}
			};
			std::vector< shared_ptr<Element> > targetableElements;
			AddTargetablesToList(targetableElements);
			
			//Sort the list.
			std::sort(targetableElements.rbegin(),targetableElements.rend(),Sorter::DistanceCompareZ);

			switch(intersectMethod)
			{
				case IntersectTestMethods::BOUNDS_2D:
				{
					BOOST_FOREACH(shared_ptr<Element> element, targetableElements)
					{
						AxisAlignedBox sceneAABB = element->GetSceneAxisAlignedBox();
						// Set the extents to 1 in each Z direction because we will test
						// against Z=0. This just ensures the Z is always in range.
						sceneAABB.GetMinimum().z=-1.f;
						sceneAABB.GetMaximum().z=1.f;

						//Detect if the position is inside the element bounds.
						bool intersected = sceneAABB.Contains(Vector3(absoluteX,absoluteY,0));
						element->UpdateCursor(absoluteX,absoluteY,pressed,intersected);

						// We only press one element, others get a no-press update. The list is
						// sorted so the top most should only get the pressed flag.
						if(intersected)
						{
							pressed = false;
						}
					}
				}break;
				case IntersectTestMethods::RAY_CAST:
				{
					BOOST_FOREACH(shared_ptr<Element> element, targetableElements)
					{
						AxisAlignedBox sceneAABB = element->GetSceneAxisAlignedBox();
						// Set the extents to 1 in each Z direction because we will test
						// against Z=0. This just ensures the Z is always in range.
						sceneAABB.GetMinimum().z=-1.f;
						sceneAABB.GetMaximum().z=1.f;

						//Detect if the position is inside the element bounds.
						bool intersected = sceneAABB.Intersects(Vector3(absoluteX,absoluteY,0));
						element->UpdateCursor(absoluteX,absoluteY,pressed,intersected);

						// We only press one element, others get a no-press update. The list is
						// sorted so the top most should only get the pressed flag.
						if(intersected)
						{
							pressed = false;
						}
					}
				}break;
			}
		}

		void Element::UpdateCursor(Scalar absoluteX, Scalar absoluteY, bool pressed, bool intersected)
		{
			if(mCursorState==CursorStates::INITIAL)
			{
				if(!pressed)
				{
					mCursorState=CursorStates::NONE;
					mCursorPressed = false;
				}
				return;
			}
			//Detect if the position is inside the element bounds.
			if(intersected)
			{
				if(mTargetable)
				{
					//Hit
					switch(mCursorState)
					{
						case CursorStates::NONE:
							mCursorState = CursorStates::ENTER;
							OnCursorEnter(absoluteX,absoluteY);
							break;
						case CursorStates::ENTER:
							mCursorState = CursorStates::HOVER;
							OnCursorHover(absoluteX,absoluteY);
							break;
						case CursorStates::HOVER:
							OnCursorHover(absoluteX,absoluteY);
							break;
						case CursorStates::INITIAL:
							ECHO_LOG_ERROR("Element::UpdateCursor() Logic error - reached unexpected case when checking cursor state.");
							break;
					}
					if(pressed)
					{
						if(!mCursorPressed)
						{
							OnCursorPress(absoluteX,absoluteY);
							mCursorPressed = true;
						}else
						{
							OnCursorHold(absoluteX,absoluteY);
						}
					}else
					{
						if(mCursorPressed)
						{
							OnCursorRelease(absoluteX,absoluteY);
							mCursorPressed = false;
						}
					}
				}
			} else
			{
				//Make sure that we update children at least one last time.
				if(mCursorState!=CursorStates::NONE)
				{
					if(mTargetable)
					{
						OnCursorLeave(absoluteX,absoluteY);
					}
					mCursorState=CursorStates::NONE;
					mCursorPressed = false;
				}
			}
		}
		
		void Element::OnCursorEnter(Scalar absoluteX, Scalar absoluteY)
		{
			if(!mFunctionBinder)
			{
				return;
			}
				
			BOOST_FOREACH(const std::string& function, mEnterFunctions)
			{
				switch (mFunctionBinder->Call(function).mStatus)
				{
					case FunctionBinder::CallStatuses::SUCCESS:
						break;
					case FunctionBinder::CallStatuses::FUNCTION_NOT_FOUND:
						ECHO_LOG_ERROR("Element(\"" << GetName() << "\")::OnCursorEnter() failed to find function " << function);
						break;
					case FunctionBinder::CallStatuses::NOT_ENOUGH_PARAMETERS:
					case FunctionBinder::CallStatuses::PARAMETER_CONVERSION_FAILED:
						ECHO_LOG_ERROR("Element(\"" << GetName() << "\")::OnCursorEnter() failed because a bad parameter list was passed for function " << function);
						break;
					case FunctionBinder::CallStatuses::RETURN_CONVERSION_FAILED:
						ECHO_LOG_ERROR("Element(\"" << GetName() << "\")::OnCursorEnter() failed to convert the return type for the result of function " << function);
						break;
				}
			}
		}
		
		void Element::OnCursorHover(Scalar absoluteX, Scalar absoluteY)
		{
			if(!mFunctionBinder)
			{
				return;
			}
				
			BOOST_FOREACH(const std::string& function, mHoverFunctions)
			{
				switch (mFunctionBinder->Call(function).mStatus)
				{
					case FunctionBinder::CallStatuses::SUCCESS:
						break;
					case FunctionBinder::CallStatuses::FUNCTION_NOT_FOUND:
						ECHO_LOG_ERROR("Element(\"" << GetName() << "\")::OnCursorHover() failed to find function " << function);
						break;
					case FunctionBinder::CallStatuses::NOT_ENOUGH_PARAMETERS:
					case FunctionBinder::CallStatuses::PARAMETER_CONVERSION_FAILED:
						ECHO_LOG_ERROR("Element(\"" << GetName() << "\")::OnCursorHover() failed because a bad parameter list was passed for function " << function);
						break;
					case FunctionBinder::CallStatuses::RETURN_CONVERSION_FAILED:
						ECHO_LOG_ERROR("Element(\"" << GetName() << "\")::OnCursorHover() failed to convert the return type for the result of function " << function);
						break;
				}
			}
		}

		void Element::OnCursorLeave(Scalar absoluteX, Scalar absoluteY)
		{
			if(!mFunctionBinder)
			{
				return;
			}
				
			BOOST_FOREACH(const std::string& function, mLeaveFunctions)
			{
				switch (mFunctionBinder->Call(function).mStatus)
				{
					case FunctionBinder::CallStatuses::SUCCESS:
						break;
					case FunctionBinder::CallStatuses::FUNCTION_NOT_FOUND:
						ECHO_LOG_ERROR("Element(\"" << GetName() << "\")::OnCursorLeave() failed to find function " << function);
						break;
					case FunctionBinder::CallStatuses::NOT_ENOUGH_PARAMETERS:
					case FunctionBinder::CallStatuses::PARAMETER_CONVERSION_FAILED:
						ECHO_LOG_ERROR("Element(\"" << GetName() << "\")::OnCursorLeave() failed because a bad parameter list was passed for function " << function);
						break;
					case FunctionBinder::CallStatuses::RETURN_CONVERSION_FAILED:
						ECHO_LOG_ERROR("Element(\"" << GetName() << "\")::OnCursorLeave() failed to convert the return type for the result of function " << function);
						break;
				}
			}
		}

		void Element::OnCursorPress(Scalar absoluteX, Scalar absoluteY)
		{
			if(!mFunctionBinder)
			{
				return;
			}

			BOOST_FOREACH(const std::string& function, mPressedFunctions)
			{
				switch (mFunctionBinder->Call(function).mStatus)
				{
					case FunctionBinder::CallStatuses::SUCCESS:
						break;
					case FunctionBinder::CallStatuses::FUNCTION_NOT_FOUND:
						ECHO_LOG_ERROR("Element(\"" << GetName() << "\")::OnCursorPress() failed to find function " << function);
						break;
					case FunctionBinder::CallStatuses::NOT_ENOUGH_PARAMETERS:
					case FunctionBinder::CallStatuses::PARAMETER_CONVERSION_FAILED:
						ECHO_LOG_ERROR("Element(\"" << GetName() << "\")::OnCursorPress() failed because a bad parameter list was passed for function " << function);
						break;
					case FunctionBinder::CallStatuses::RETURN_CONVERSION_FAILED:
						ECHO_LOG_ERROR("Element(\"" << GetName() << "\")::OnCursorPress() failed to convert the return type for the result of function " << function);
						break;
				}
			}
		}

		void Element::OnCursorHold(Scalar absoluteX, Scalar absoluteY)
		{
			if(!mFunctionBinder)
			{
				return;
			}
				
			BOOST_FOREACH(const std::string& function, mHoldFunctions)
			{
				switch (mFunctionBinder->Call(function).mStatus)
				{
					case FunctionBinder::CallStatuses::SUCCESS:
						break;
					case FunctionBinder::CallStatuses::FUNCTION_NOT_FOUND:
						ECHO_LOG_ERROR("Element(\"" << GetName() << "\")::OnCursorHold() failed to find function " << function);
						break;
					case FunctionBinder::CallStatuses::NOT_ENOUGH_PARAMETERS:
					case FunctionBinder::CallStatuses::PARAMETER_CONVERSION_FAILED:
						ECHO_LOG_ERROR("Element(\"" << GetName() << "\")::OnCursorHold() failed because a bad parameter list was passed for function " << function);
						break;
					case FunctionBinder::CallStatuses::RETURN_CONVERSION_FAILED:
						ECHO_LOG_ERROR("Element(\"" << GetName() << "\")::OnCursorHold() failed to convert the return type for the result of function " << function);
						break;
				}
			}
		}

		void Element::OnCursorRelease(Scalar absoluteX, Scalar absoluteY)
		{
			if(!mFunctionBinder)
			{
				return;
			}
				
			BOOST_FOREACH(const std::string& function, mReleasedFunctions)
			{
				switch (mFunctionBinder->Call(function).mStatus)
				{
					case FunctionBinder::CallStatuses::SUCCESS:
						break;
					case FunctionBinder::CallStatuses::FUNCTION_NOT_FOUND:
						ECHO_LOG_ERROR("Element(\"" << GetName() << "\")::OnCursorRelease() failed to find function " << function);
						break;
					case FunctionBinder::CallStatuses::NOT_ENOUGH_PARAMETERS:
					case FunctionBinder::CallStatuses::PARAMETER_CONVERSION_FAILED:
						ECHO_LOG_ERROR("Element(\"" << GetName() << "\")::OnCursorRelease() failed because a bad parameter list was passed for function " << function);
						break;
					case FunctionBinder::CallStatuses::RETURN_CONVERSION_FAILED:
						ECHO_LOG_ERROR("Element(\"" << GetName() << "\")::OnCursorRelease() failed to convert the return type for the result of function " << function);
						break;
				}
			}
		}

		const Vector3& Element::GetRelativePosition() const
		{
			return mPercentPosition;
		}
		
		const Vector3& Element::GetAbsolutePosition() const
		{
			return Node::GetPosition();
		}
		
		const Vector3& Element::GetPosition() const
		{
			//Drop back to Node, this will have been called from Node.
			return Node::GetPosition();
		}

		void Element::ResetCursorState()
		{
			mCursorState = CursorStates::INITIAL;
			BOOST_FOREACH(shared_ptr<Element> element, mChildElements)
			{
				element->ResetCursorState();
			}
		}
		
		void Element::SetPosition(const Vector3& val)
		{
			//Drop back to Node, this will have been called from Node.
			Node::SetPosition(val);
		}

		void Element::SetPosition(f32 x, f32 y, f32 z)
		{
			//Drop back to Node, this will have been called from Node.
			Node::SetPosition(x,y,z);
		}
					
		void Element::SetRelativePosition(const Vector3& val)
		{
			mPercentPosition=val;
			PositionUpdated();
		}
		
		void Element::SetHorizontalAlignment(Alignment alignment)
		{
			mHorizontalAlignment=alignment;
			PositionUpdated();
		}
		
		void Element::SetVerticalAlignment(Alignment alignment)
		{
			mVerticalAlignment=alignment;
			PositionUpdated();
		}
		
		void Element::SetRelativePosition(f32 x, f32 y, f32 z)
		{
			mPercentPosition.Set(x,y,z);
			PositionUpdated();
		}
		
		void Element::OnParentSizeUpdate()
		{
			SizeUpdated();
		}

		void Element::OnParentPositionUpdate()
		{
			PositionUpdated();
		}

		void Element::ProcessConditionals()
		{
			if(!mConditionalEvaluator || !mFunctionBinder)
			{
				return;
			}
			BOOST_FOREACH(const ConditionalFunctionMap::value_type& conditionPair, mConditionalFunctions)
			{
				optional<bool> result = mConditionalEvaluator->Parse(conditionPair.first);
				if(result)
				{
					//What did the expression evaluate to?
					if(*result)
					{
						BOOST_FOREACH(const std::string& function, conditionPair.second)
						{
							mFunctionBinder->Call(function);
						}
					}
				}else
				{
					ECHO_LOG_ERROR("Element could not parse condition.");
				}
			}
		}
		
		void Element::SetFunctionBinder(shared_ptr<FunctionBinder> functionBinder)
		{
			mFunctionBinder=functionBinder;
		}
		
		shared_ptr<FunctionBinder> Element::GetFunctionBinder() const
		{
			return mFunctionBinder;
		}
		
		void Element::AddEnterFunction(const std::string& function)
		{
			mEnterFunctions.push_back(function);
		}
		
		void Element::AddHoverFunction(const std::string& function)
		{
			mHoverFunctions.push_back(function);
		}
		
		void Element::AddLeaveFunction(const std::string& function)
		{
			mLeaveFunctions.push_back(function);
		}
		
		void Element::AddPressedFunction(const std::string& function)
		{
			mPressedFunctions.push_back(function);
		}
		
		void Element::AddHoldFunction(const std::string& function)
		{
			mHoldFunctions.push_back(function);
		}
		
		void Element::AddReleasedFunction(const std::string& function)
		{
			mReleasedFunctions.push_back(function);
		}
		
		void Element::AddConditionalFunction(const std::string& condition, const std::string& function)
		{
			ConditionalFunctionMap::iterator it = mConditionalFunctions.find(condition);
			if(it==mConditionalFunctions.end())
			{
				std::pair<ConditionalFunctionMap::iterator, bool> insertResult = mConditionalFunctions.insert(std::make_pair(condition, std::list<std::string>()));
				if(!insertResult.second)
				{
					ECHO_LOG_ERROR("Element::AddConditionalFunction() There was an error inserting the condition \"" << condition << "\" into the conditional functions lookup.");
					return;
				}
				it = insertResult.first;
			}
			//Add the function
			it->second.push_back(function);
			
			//Make sure the ConditionalEvaluator is setup.
			SetupConditionalEvaluatorParser();
		}

		void Element::AddConditionalFunctions(const std::string& condition, const std::list<std::string>& functions)
		{
			ConditionalFunctionMap::iterator it = mConditionalFunctions.find(condition);
			if(it==mConditionalFunctions.end())
			{
				std::pair<ConditionalFunctionMap::iterator, bool> insertResult = mConditionalFunctions.insert(std::make_pair(condition, std::list<std::string>()));
				if(!insertResult.second)
				{
					ECHO_LOG_ERROR("Element::AddConditionalFunctions() There was an error inserting the condition \"" << condition << "\" into the conditional functions lookup.");
					return;
				}
				it = insertResult.first;
			}
			//Add the function
			it->second.insert(it->second.end(), functions.begin(), functions.end());

			//Make sure the ConditionalEvaluator is setup.
			SetupConditionalEvaluatorParser();
		}
		
		void Element::SetParentElement(Element* parent)
		{
			//Reset the parent unit converter since a new one will be acquired via the parent as needed.
			mCachedParentUnitConverter.reset();
			
			if(mParent)
			{
				mParent->RemoveElement(*this);
			}					
			mParent = parent;
			mCursorState = CursorStates::INITIAL;
			SizeUpdated();
			PositionUpdated();
		}

		shared_ptr<Element> Element::FindElement(const std::string& name, bool searchChildren) const
		{
			BOOST_FOREACH(shared_ptr<Element> child, mChildElements)
			{
				if(child->GetName()==name)
				{
					return child;
				}
			}

			if(searchChildren)
			{
				BOOST_FOREACH(shared_ptr<Element> child, mChildElements)
				{
					shared_ptr<Element> element = child->FindElement(name, true);
					if(element)
					{
						return element;
					}
				}
			}
			return shared_ptr<Element>();
		}
		
		std::list< shared_ptr<Element> > Element::FindElements(const std::string& name, bool searchChildren) const
		{
			std::list< shared_ptr<Element> > matches;
			BOOST_FOREACH(shared_ptr<Element> child, mChildElements)
			{
				if(child->GetName()==name)
				{
					matches.push_back(child);
					if(searchChildren)
					{
						std::list< shared_ptr<Element> > childMatches = child->FindElements(name,true);
						matches.merge(childMatches);
					}
				}
			}
			return matches;
		}
		
		void Element::ChildRemoved(Node& node)
		{
			SceneEntity::ChildRemoved(node);
			
			Element* element = dynamic_cast<Element*>(&node);
			if(element)
			{
				ChildContainer::iterator it=mChildElements.begin();
				while(it!=mChildElements.end())
				{
					if((*it).get() == element)
					{
						mChildElements.erase(it);
						element->SetParentElement(nullptr);
						return;
					}
					++it;
				}
			}
		}
		
		shared_ptr<Element> Element::Clone() const
		{
			shared_ptr<Element> newElement = _Clone();
			BOOST_FOREACH(shared_ptr<Element> child, mChildElements)
			{
				shared_ptr<Element> element = child->Clone();
				if(element)
				{
					newElement->AddElement(element);
				}
			}
			return newElement;
		}
		
		void Element::SetupConditionalEvaluatorParser()
		{
			if(!mFunctionBinder)
			{
				ECHO_LOG_WARNING("Element: A conditional functions can not execute without a function binder. To prevent this warning, setup your Element with a FunctionBinder before adding conditionals.");
			}
			if(mConditionalEvaluator)
			{
				//Already done, this is fine.
				return;
			}
			
			//Create a parser and register some locals.
			mConditionalEvaluator.reset(new Parser::ConditionalEvaluator());
			
			//Register locals
			mConditionalEvaluator->RegisterVariable("width",mWidth);
			mConditionalEvaluator->RegisterVariable("height",mHeight);
			mConditionalEvaluator->RegisterVariableViaGetter("aspect_ratio",bind(&Element::GetAspectRatio,this));

			//Add some additional likely useful variables.
			mConditionalEvaluator->RegisterVariableViaGetter("parent.width",bind(&Element::GetParentWidth,this));
			mConditionalEvaluator->RegisterVariableViaGetter("parent.height",bind(&Element::GetParentHeight,this));
			mConditionalEvaluator->RegisterVariableViaGetter("parent.aspect_ratio",bind(&Element::GetParentAspectRatio,this));
			mConditionalEvaluator->RegisterVariableViaGetter("root.width",bind(&Element::GetRootWidth,this));
			mConditionalEvaluator->RegisterVariableViaGetter("root.height",bind(&Element::GetRootHeight,this));
			mConditionalEvaluator->RegisterVariableViaGetter("root.aspect_ratio",bind(&Element::GetRootAspectRatio,this));
		}

		Scalar Element::GetParentWidth() const
		{
			if(mParent)
			{
				return mParent->GetAbsoluteWidth();
			}
			return 0;
		}
		
		Scalar Element::GetParentHeight() const
		{
			if(mParent)
			{
				return mParent->GetAbsoluteHeight();
			}
			return 0;
		}
		
		Scalar Element::GetParentAspectRatio() const
		{
			if(mParent)
			{
				return mParent->GetAspectRatio();
			}
			return 0;
		}
		
		Scalar Element::GetRootWidth() const
		{
			if(mParent)
			{
				return mParent->GetRootWidth();
			}
			return mWidth;
		}
		
		Scalar Element::GetRootHeight() const
		{
			if(mParent)
			{
				return mParent->GetRootHeight();
			}
			return mHeight;
		}
		
		Scalar Element::GetRootAspectRatio() const
		{
			if(mParent)
			{
				return mParent->GetRootAspectRatio();
			}
			return GetAspectRatio();
		}
	}
}
