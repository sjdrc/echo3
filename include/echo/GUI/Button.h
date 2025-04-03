#ifndef _ECHOBUTTONELEMENT_H_
#define _ECHOBUTTONELEMENT_H_

#include <echo/GUI/Image.h>

namespace Echo
{
	namespace GUI
	{
		/**
		 * A GUI Button applies a different material to each button state.
		 * To make the button respond to events use Element::Set*Function() methods.
		 */
		class Button : public Image
		{
		public:
			Button(shared_ptr<Material> idleState);
			virtual ~Button();
			
			/**
			 * Button copies the materials for each element state and follows the rules for
			 * copying as per the base classes.
			 * @see Image::operator=()  for documentation on Image copying.
			 * @see Element::operator=() for documentation on Element copying.
			 * @see Node::operator=() for documentation on copying.
			 */
			Button(const Button& other);
			Button& operator=(const Button& rhs);
			
			void SetIdle(shared_ptr<Material> material);
			void SetPressed(shared_ptr<Material> material);
			void SetReleased(shared_ptr<Material> material);
			void SetHover(shared_ptr<Material> material);
			
			shared_ptr<Material> GetIdle() const {return mIdleMaterial;}
			shared_ptr<Material> GetPressed() const {return mPressedMaterial;}
			shared_ptr<Material> GetReleased() const {return mReleasedMaterial;}
			shared_ptr<Material> GetHover() const {return mHoverMaterial;}
		private:
			virtual shared_ptr<Element> _Clone() const override;
			virtual Scalar GetNativeAspectRatio() override;
			virtual void OnCursorEnter(Scalar absoluteX, Scalar absoluteY) override;
			virtual void OnCursorHover(Scalar absoluteX, Scalar absoluteY) override;
			virtual void OnCursorLeave(Scalar absoluteX, Scalar absoluteY) override;
			virtual void OnCursorPress(Scalar absoluteX, Scalar absoluteY) override;
			virtual void OnCursorHold(Scalar absoluteX, Scalar absoluteY) override;
			virtual void OnCursorRelease(Scalar absoluteX, Scalar absoluteY) override;

			shared_ptr<Material> mIdleMaterial;
			shared_ptr<Material> mPressedMaterial;
			shared_ptr<Material> mReleasedMaterial;
			shared_ptr<Material> mHoverMaterial;
		};
	}
}
#endif 
