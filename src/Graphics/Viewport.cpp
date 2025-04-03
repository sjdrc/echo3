#include <echo/Graphics/Viewport.h>
#include <echo/Graphics/Camera.h>
#include <iostream>
#include <boost/foreach.hpp>

namespace Echo
{
	Viewport::Viewport(f32 left, f32 top, f32 right, f32 bottom)
	{
		SetFixed(left,top,right,bottom);
	}

	Viewport::~Viewport()
	{
	}

	void Viewport::SetMaxFit(f32 displayAspectRatio, f32 left, f32 top, f32 right, f32 bottom)
	{
		mRectangle.mLeft=left;
		mRectangle.mRight=right;
		mRectangle.mTop=top;
		mRectangle.mBottom=bottom;
		mAspectRatio = displayAspectRatio;
		mSizeOption = SizeOptions::MAX_FIT_PRESERVE_ASPECT_RATIO;
		mHasFixedAspectRatio = true;
	}
	
	void Viewport::SetMaxFit(f32 displayAspectRatio, Rectangle rectangle)
	{
		SetMaxFit(displayAspectRatio,rectangle.mLeft,rectangle.mTop,rectangle.mRight,rectangle.mBottom);
	}

	void Viewport::SetAsHorizontal(f32 displayAspectRatio, f32 left, f32 right, f32 verticalCentre)
	{
		mRectangle.mLeft=left;
		mRectangle.mRight=right;
		mRectangle.mTop=verticalCentre;	//Using mTop as vertical centre to save adding an extra member variable.
		mRectangle.mBottom=1.f;			//Not used for this mode but we're setting it to a reasonable value for safety.
		mAspectRatio = displayAspectRatio;
		mSizeOption = SizeOptions::WIDTH_FIRST_PRESERVE_ASPECT_RATIO;
		mHasFixedAspectRatio = true;
	}

	void Viewport::SetAsVertical(f32 displayAspectRatio, f32 top, f32 bottom, f32 horizontalCentre)
	{
		mRectangle.mLeft=horizontalCentre;	//Using mLeft as horizontal centre to save adding an extra member variable.
		mRectangle.mRight=1.f;				//Not used for this mode but we're setting it to a reasonable value for safety.
		mRectangle.mTop=top;
		mRectangle.mBottom=bottom;
		mAspectRatio = displayAspectRatio;
		mSizeOption = SizeOptions::HEIGHT_FIRST_PRESERVE_ASPECT_RATIO;
		mHasFixedAspectRatio = true;
	}

	void Viewport::SetFixed(f32 left, f32 top, f32 right, f32 bottom)
	{
		mRectangle.mLeft=left;
		mRectangle.mRight=right;
		mRectangle.mTop=top;
		mRectangle.mBottom=bottom;
		mAspectRatio = (right-left) / (bottom-top);
		mSizeOption = SizeOptions::STRETCH;
		mHasFixedAspectRatio = false;
	}

	Viewport::Rectangle Viewport::GetRectangle(f32 targetsAspectRatio) const
	{
		switch(mSizeOption)
		{
			case SizeOptions::STRETCH:
				return mRectangle;
			case SizeOptions::WIDTH_FIRST_PRESERVE_ASPECT_RATIO:
			{
				Rectangle result;
				result.mLeft = mRectangle.mLeft;
				result.mRight = mRectangle.mRight;
				f32 halfHeight = ((mRectangle.mRight - mRectangle.mLeft) / (mAspectRatio/targetsAspectRatio)) * 0.5f;
				
				//mRectangle.mTop is used as the vertical centre in this case.
				result.mTop = mRectangle.mTop-halfHeight;
				result.mBottom = mRectangle.mTop+halfHeight;
			}break;
			case SizeOptions::HEIGHT_FIRST_PRESERVE_ASPECT_RATIO:
			{
				Rectangle result;
				result.mTop = mRectangle.mTop;
				result.mBottom = mRectangle.mBottom;
				f32 halfWidth = ((mRectangle.mBottom - mRectangle.mTop) * (mAspectRatio/targetsAspectRatio)) * 0.5f;
				//mRectangle.mLeft is used as the horizontal centre in this case.
				result.mLeft = mRectangle.mLeft-halfWidth;
				result.mRight = mRectangle.mLeft+halfWidth;
				return result;
			}break;
			case SizeOptions::MAX_FIT_PRESERVE_ASPECT_RATIO:
			{
				//Need to size to 1 on width or height.
				f32 requestedWidth = (mRectangle.mRight - mRectangle.mLeft);
				f32 requestedHeight = (mRectangle.mBottom - mRectangle.mTop);
				f32 viewportAdjustedRatio=requestedWidth/requestedHeight;
				f32 viewportAspectRatio=targetsAspectRatio * viewportAdjustedRatio;

				if(mAspectRatio>viewportAspectRatio)
				{
					Rectangle result;
					//Filling the predefined viewport to the maximum width
					result.mLeft=mRectangle.mLeft;
					result.mRight=mRectangle.mRight;
					f32 halfHeight = (requestedWidth / (mAspectRatio/targetsAspectRatio)) * 0.5f;
					f32 verticalCentre=requestedHeight * 0.5f;
					result.mTop = mRectangle.mTop + verticalCentre-halfHeight;
					result.mBottom = mRectangle.mTop + verticalCentre+halfHeight;
					return result;
				}else
				{
					Rectangle result;
					//Filling the predefined viewport to the maximum height
					result.mTop=mRectangle.mTop;
					result.mBottom=mRectangle.mBottom;
					f32 halfWidth = (requestedHeight * (mAspectRatio/targetsAspectRatio)) * 0.5f;
					f32 horizontalCentre=requestedWidth * 0.5f;
					result.mLeft = mRectangle.mLeft + horizontalCentre-halfWidth;
					result.mRight = mRectangle.mLeft + horizontalCentre+halfWidth;
					return result;
				}
			}break;
		}
		return Rectangle(0,0,1,1);
	}
	
}
