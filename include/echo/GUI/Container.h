#ifndef _ECHOGUICONTAINER_H_
#define _ECHOGUICONTAINER_H_

#include <echo/GUI/Element.h>

namespace Echo
{
	namespace GUI
	{
		/**
		 * A GUI Container is a container for Elements.
		 * It is simply an Element that doesn't use the size for a visual component.
		 * You can still render the AABB using SetRenderAABB(true).
		 */
		class Container : public Element
		{
		public:
			Container();
			virtual ~Container();
			
			/**
			 * @see Element::operator=() for documentation on Element copying.
			 * @see Node::operator=() for documentation on copying.
			 */
			Container(const Container& other);
			Container& operator=(const Container& rhs);

		protected:
			virtual void UpdateSize(Scalar newWidth, Scalar newHeight) override;
			virtual Scalar GetNativeAspectRatio() override;
		private:
			virtual shared_ptr<Element> _Clone() const override;
			Scalar mWidth;
			Scalar mHeight;
		};
	}
}
#endif 
