#ifndef _ECHOVIEWPORT_H_
#define _ECHOVIEWPORT_H_

#include <echo/Types.h>
#include <echo/Graphics/Colour.h>

namespace Echo
{
	/**
	 * A Viewport represents a rendering area within a RenderTarget.
	 * Viewports are defined as coordinates from top left (0,0) to bottom right (1,1)
	 * The viewport class also provides functionality for automatic adjustment for
	 * a given aspect ratio. This allows you to specify the aspect ratio that you want
	 * the Viewport to maintain and it will automatically adjust as needed to fit with
	 * the correct aspect ratio on the render target. There are a number of different
	 * modes to accomplish this each having a different behaviour.
	 * @note All coordinates specified here are in viewport coordinates which represent
	 * percentages of the width and height of the RenderTarget.
	 */
	class Viewport
	{
	public:
		/**
		 * Rectangle contains the position of the viewport in percent values of the
		 * RenderTarget. For example a Rectangle with a left of 0, top of 0, right of 1
		 * and bottom of 0.5 will take up the top half of the render target.
		 */
		struct Rectangle
		{
			Rectangle(f32 l=0.f,f32 t=0.f,f32 r=1.f,f32 b=1.f) : mLeft(l), mTop(t), mRight(r), mBottom(b){}
			f32 mLeft;
			f32 mTop;
			f32 mRight;
			f32 mBottom;
			inline f32 GetWidth() const {return mRight-mLeft;}
			inline f32 GetHeight() const {return mBottom-mTop;}
			bool operator==(const Rectangle& rhs)
			{
				return (mLeft==rhs.mLeft &&
						mTop==rhs.mTop &&
						mRight==rhs.mRight &&
						mBottom==rhs.mBottom);
			}
		};
		Viewport(f32 left=0.0f, f32 top=0.0f, f32 right=1.0f, f32 bottom=1.0f);
		~Viewport();

		/**
		 * Set the viewport to best fit the area specified by the coordinates.
		 * If the RenderTarget changes in size the viewport will always ensure it is entirely visible
		 * within the provided area. Depending on the aspect ratio of the RenderTarget, this may cause
		 * bars on the top and bottom or on the left and right of the viewport.
		 * @param displayAspectRatio The aspect ratio the viewport is to maintain.
		 * @param left Viewport left coordinate.
		 * @param top Viewport top coordinate.
		 * @param right Viewport right coordinate.
		 * @param bottom Viewport Bottom coordinate.
		 */
		void SetMaxFit(f32 displayAspectRatio, f32 left=0.0f, f32 top=0.0f, f32 right=1.0f, f32 bottom=1.0f);
		
		/**
		 *@see SetMaxFit(f32, f32, f32, f32, f32)f
		 */
		void SetMaxFit(f32 displayAspectRatio, Rectangle rectangle);
		
		/**
		 * Set the viewport with a fixed aspect ratio and always maintain the horizontal size.
		 * @note This method may cause the viewport to extend beyond the top and bottom of the render target.
		 * This can cause part of your scene to be cut off from view. RenderTarget implementations must also
		 * ensure to clamp if necessary.
		 * @param displayAspectRatio The aspect ratio the viewport is to maintain.
		 * @param left The left coordinate of the viewport.
		 * @param right The right coordinate of the viewport.
		 * @param verticalCentre The vertical centre of the viewport. The top and bottom values will be
		 * calculated with this as a centre.
		 */
		void SetAsHorizontal(f32 displayAspectRatio, f32 left=0.0f, f32 right=1.0f, f32 verticalCentre=0.5f);

		/**
		 * Set the viewport with a fixed aspect ratio and always maintain the vertical size.
		 * @note This method may cause the viewport to extend beyond the left and right of the render target.
		 * This can cause part of your scene to be cut off from view. RenderTarget implementations must also
		 * ensure to clamp if necessary.
		 * @param displayAspectRatio The aspect ratio the viewport is to maintain.
		 * @param top The top coordinate of the viewport.
		 * @param bottom The bottom coordinate of the viewport.
		 * @param horizontalCentre The horizontal centre of the viewport. The left and right values will be
		 * calculated with this as a centre.
		 */
		void SetAsVertical(f32 displayAspectRatio, f32 top=0.0f, f32 bottom=1.0f, f32 horizontalCentre=0.5f);
		
		/**
		 * Set the viewport coordinates as fixed values.
		 * The viewport will always use the provided values as the values for the RenderTarget. This can
		 * cause stretching/squashing of your view if the camera isn't automatically adjusting for the aspect
		 * ratio change.
		 * @param left Viewport left coordinate.
		 * @param top Viewport top coordinate.
		 * @param right Viewport right coordinate.
		 * @param bottom Viewport Bottom coordinate.
		 */
		void SetFixed(f32 left=0.0f, f32 top=0.0f, f32 right=1.0f, f32 bottom=1.0f);

		/**
		 * Get the aspect ratio.
		 * The aspect ratio will either be the one provided in one of the set methods above, except in the
		 * case of SetFixed() where the aspect ratio will be the aspect ratio of the percentage values (it
		 * is known that this value isn't necessarily very useful).
		 * @return The aspect ratio.
		 */
		f32 GetAspectRatio() const
		{
			return mAspectRatio;
		}
		
		/**
		 * Get whether the viewport has a fixed aspect ratio.
		 * If the viewport has been set with one of the Set methods other than SetFixed() then the aspect
		 * ratio will be fixed.
         * @return true if the aspect ratio is fixed, otherwise false.
         */
		bool GetHasFixedAspectRatio() const {return mHasFixedAspectRatio;}
		
		/**
		 * Get the rectangle of the viewport.
		 * @param targetsAspectRatio The aspect ratio of the target you are going to use the viewport for.
		 * @return A Rectangle object that contains the coordinates as adjusted for the viewport settings.
		 */
		Rectangle GetRectangle(f32 targetsAspectRatio) const;
	private:
		struct SizeOptions
		{
			enum _
			{
				STRETCH,
				WIDTH_FIRST_PRESERVE_ASPECT_RATIO,		///As per SetAsHorizontal().
				HEIGHT_FIRST_PRESERVE_ASPECT_RATIO,		///As per SetAsVertical().
				MAX_FIT_PRESERVE_ASPECT_RATIO			///As per SetMaxFit().
			};
		};
		typedef SizeOptions::_ SizeOption;

		Rectangle mRectangle;
		f32 mAspectRatio;
		SizeOption mSizeOption;
		bool mHasFixedAspectRatio;
	};
}
#endif
