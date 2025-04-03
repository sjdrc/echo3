#ifndef _ECHOLAYOUTELEMENT_H_
#define _ECHOLAYOUTELEMENT_H_

#include <echo/GUI/Container.h>

namespace Echo
{
	namespace GUI
	{
		/**
		 * An element that positions its child elements either horizontally or vertically.
		 * The space that the contents of a layout vary depending on the size mode used.
		 */
		class Layout : public Container
		{
		public:
			/**
			 * Available Directions the layout can place elements in.
			 * Typically the X axis is horizontal and the Y axis is vertical. There aren't any
			 * situations at the moment where this is not true.
			 */
			struct Directions
			{
				enum _
				{
					HORIZONTAL_MIN_TO_MAX,	/// Place elements horizontally from the minimum to maximum in the order they were added.
					HORIZONTAL_MAX_TO_MIN,	/// Place elements horizontally from the maximum to minimum in the order they were added.
					VERTICAL_MIN_TO_MAX,	/// Place elements vertically from the minimum to maximum in the order they were added.
					VERTICAL_MAX_TO_MIN		/// Place elements vertically from the maximum to minimum in the order they were added.
				};
			};
			typedef Directions::_ Direction;
			
			struct PlacementModes
			{
				enum _
				{
					FIT_CONTENTS,			/// Contents will be scaled to fit the layout size, some child size modes have unspecified* behaviour,
											/// For example: Horizontal directions do not support height first child element size modes as these
											/// modes expect to be able to determine the width but the horizontal placement needs to be able to
											/// scale by the width of the child element, which in doing so would break the height first rule.
											/// Children using the max fit size modes 
					ALLOW_OVERFLOW,			/// Contents will be positioned and are allowed to be positioned outside of the layout's bounds. All size
											/// modes are supported.
				};
			};
			typedef PlacementModes::_ PlacementMode; 
			
			Layout(Direction direction = Directions::HORIZONTAL_MIN_TO_MAX, PlacementMode placementMode = PlacementModes::FIT_CONTENTS);
			virtual ~Layout();
			
			/**
			 * Copying a layout only copies the layout settings and not the child elements.
			 * See the base class documentation for more information on element copying.
			 * If you want to copy the layout with contents consider the Clone() method.
			 * @see Element::operator=() for documentation on Element copying.
			 * @see Node::operator=() for documentation on copying.
			 */
			Layout(const Layout& other);
			Layout& operator=(const Layout& other);
			
			/**
			 * Set the direction of the layout.
			 * @param direction The direction.
			 */
			void SetDirection(Direction direction);

			/**
			 * Set the placement mode of the layout.
			 * @see PlacementModes.
			 * @param palcementMode the placement mode.
			 */
			void SetPlacementMode(PlacementMode placementMode);

			/**
			 * Get the direction of the layout.
			 * @return the direction.
			 */
			Direction GetDirection() const {return mDirection;}
			
			/**
			 * Get the placement mode of the layout.
			 * @see PlacementModes.
			 * @return the placement mode.
			 */
			PlacementMode GetPlacementMode() const {return mPlacementMode;}
			
			/**
			 * Set whether to apply settings on child elements when the layout changes so they are positioned consistently.
			 * When this is turned off child parameters are persistent when you modify layout settings. For example,
			 * changing from a vertical layout to a horizontal layout will mean all child elements will still have the
			 * vertical from the previous direction.
			 * This setting was added so you could create a layout and modify the child element's parameters to get the
			 * desired result and not worry about your changes being overwritten.
			 * When this setting is enabled elements are aligned centre to the the layout and the size mode is modified
			 * to the most appropriate for the layout's setting.
			 * This setting is enabled by default.
			 * @note Positions are always overwritten.
			 * @note Sizes are always overwritten when using PlacementModes::FIT_CONTENTS.
			 * @note Each placement mode and direction has will modify the alignment for the corresponding axis to lay the
			 * items out adjacent to one another.
			 */
			void SetUseOptimalChildSettings(bool useOptimalChildSettings);
			
			/**
			 * These methods are overridden from Element so we can manage our own ordered
			 * list of elements.
			 * Adding an element to a Layout will cause some of its parameters to be modified. For example, when
			 * the layout is updated the alignment is set to best fit with positioning elements.
			 */
			virtual void AddElement(shared_ptr<Element> element) override;
			virtual void RemoveElement(shared_ptr<Element> element) override;
			virtual void RemoveElement(Element& element) override;
			
			/**
			 * Update the position and size of the children as per the layout settings.
			 * This method is usually automatically called within the class however because elements currently aren't
			 * reporting size changes to their parent elements this method is public for classes that will need the
			 * layout updated at specific times.
			 */
			void UpdateChildren();
		private:
			virtual shared_ptr<Element> _Clone() const override;
			
			void ChildrenHorizontalMinToMax();
			void ChildrenHorizontalMaxToMin();
			void ChildrenVerticalMinToMax();
			void ChildrenVerticalMaxToMin();
			void SetOptimalChildSettings();
			
			Direction mDirection;
			PlacementMode mPlacementMode;
			bool mUseOptimalChildSettings;
			typedef std::list< Element* > OrderedChildrenContainer;
			OrderedChildrenContainer mOrderedChildren;
		};
	}
}
#endif 
