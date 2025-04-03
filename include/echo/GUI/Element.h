#ifndef _ECHOGUIELEMENT_H_
#define _ECHOGUIELEMENT_H_

#include <echo/Graphics/SceneEntity.h>
#include <echo/Maths/Vector2.h>
#include <echo/Maths/Length.h>
#include <echo/Util/Configuration.h>

namespace Echo
{
	class FunctionBinder;
	namespace Parser
	{
		class ConditionalEvaluator;
	}
	
	namespace GUI
	{
		/**
		 * GUI classes are Elements.
		 * A GUI Element manages the resizing and positioning of elements.
		 * - Element positions are always percent relative to the parent size. A position of 0.5,0.5 is relative 0,0.
		 * - Element sizes are always a percentage of the parent element size, except for the top most element where
		 * the size will be the absolute size.
		 * - Elements can resize in a variety of ways that allow you to preserve the aspect ratio of the Element
		 * from when the size was set.
		 */
		class Element : public SceneEntity
		{
		public:
			/**
			* Elements can resize in a variety of ways that allow you to preserve the aspect ratio of the Element
			* from when the size was set.
			*/
			struct SizeModes
			{
				enum _
				{
					STRETCH,								/// The set percent size will be used.
					FILL,									/// Fill the parent element using the aspect ratio of the element content. If the aspect ratio differs
															/// from the parent then the element will overflow (exceed the bounds of) the parent on which ever axis
															/// is needed to fill the parent. The element is not cropped when this overflow occurs.
					WIDTH_AND_CALCULATE_ASPECT_RATIO,		/// Use the width and calculate the height based on the aspect ratio of the element content.
					HEIGHT_AND_CALCULATE_ASPECT_RATIO,		/// Use the height and calculate the width based on the aspect ratio of the element content.
					WIDTH_WITH_SET_ASPECT_RATIO,			/// Use the width and calculate height using the set aspect ratio to calculate the height based on the specified values.
					HEIGHT_WITH_SET_ASPECT_RATIO,			/// Use the height and calculate width using the set aspect ratio to calculate the width based on the specified values.
					FILL_WITH_SET_ASPECT_RATIO,				/// Fill the parent element using the set aspect ratio of the element content. If the aspect ratio differs
															/// from the parent then the element will overflow (exceed the bounds of) the parent on which ever axis
															/// is needed to fill the parent. The element is not cropped when this overflow occurs.
					MAX_FIT_WITH_SET_ASPECT_RATIO			/// Fit the Element to the parent on the smallest axis using the set aspect ratio to calculate the other axis.
				};
			};
			typedef SizeModes::_ SizeMode;
			
			struct IntersectTestMethods
			{
				enum _
				{
					BOUNDS_2D,
					RAY_CAST
				};
			};
			typedef IntersectTestMethods::_ IntersectTestMethod;
			
			/**
			 * Alignments are relative to the object's position.
			 * 
			 * An object with a minimum X alignment means its minimum X (left) edge will be located at
			 * the set position (usually percent). Similarly the maximum corresponds to the right edge.
			 * 
			 * On the vertically the default is minimum is bottom and top is maximum.
			 * 
			 * We use minimum and maximum instead of left, right top and bottom because the latter
			 * depend how a camera is position and orientated. Considering a GUI system may be visible
			 * through multiple cameras left, right top and bottom then become less deterministic,
			 * especially when positioning things.
			 * 
			 * The position of an Element at the Node level will be with the alignment applied but the
			 * position used with percent will always be relative to the parent's centre.
			 */
			struct Alignments
			{
				enum _
				{
					CENTRE,		///Default for both axes. The Element will be centred at the position.
					MINIMUM,	///On the specified axis, the element will be on its minimum edge.
					MAXIMUM,	///On the specified axis, the element will be on its maximum edge.
				};
			};
			typedef Alignments::_ Alignment;
			
			Element();
			virtual ~Element();
			
			/**
			 * Copying an Element will result in the size, relative position, alignment settings etc
			 * to be copied. No hierarchy is copied.
			 * To maintain consistency as a SceneEntity the base assignment operator is called during
			 * an Element copy but the Mesh of this element is preserved.
			 * @see Node::operator=() for documentation on copying.
			 */
			Element(const Element& other);
			Element& operator=(const Element& rhs);
			
			/**
			 * Get the axis aligned box.
			 * @param applyLocalTransform If true will apply the local transform to the AABB before returning.
			 * @return the axis aligned bounding box.
			 */
			virtual AxisAlignedBox GetAxisAlignedBox(bool applyLocalTransform = true) const override;

			/**
			 * Get the parent element.
			 * @return If not null the parent element.
			 */
			Element* GetParentElement() const
			{
				return mParent;
			}
			
			/**
			 * Show the element.
			 * If the visibility state changes, this also resets the cursor state to the initial
			 * state to prevent unexpected cursor events to occur.
			 */
			void Show();

			/**
			 * Hide the element.
			 */
			void Hide();
			
			/**
			 * Set whether the Element can be targeted with a cursor.
			 * If an Element is target about it will process cursor events.
			 * If an element is not targetable then it will process child elements automatically.
			 * @param targetable
			 */
			void SetTargetable(bool targetable) {mTargetable=targetable;}
			bool GetTargetable() const {return mTargetable;}
			
			/**
			 * Set the size mode.
			 * @see SizeModes.
			 * @param sizeMode The size mode the element will use when calculating the dimensions.
			 */
			void SetSizeMode(SizeMode sizeMode) {mSizeMode=sizeMode;}
			
			/**
			 * Get the size mode the element uses when calculating its dimensions.
			 * @see SizeModes.
			 * @return Get the size mode the element uses when calculating its dimensions.
			 */
			SizeMode GetSizeMode() const {return mSizeMode;}
			
			/**
			 * Add a child element.
			 * If the child element doesn't have a function binder it will inherit the parent's.
			 * Parent function binders aren't inherited in a chain. They need to be set at the top most
			 * Element first for automatic inheritance.
			 * @param element The child element.
			 */
			virtual void AddElement(shared_ptr<Element> element);
			
			/**
			 * Remove a child element.
			 */
			virtual void RemoveElement(shared_ptr<Element> element);
			virtual void RemoveElement(Element& element);

			/**
			 * Get the relative width of the element.
			 * If the Element has a parent then the relative width will be a percentage value of the parent element.
			 * If the Element is a root element then the width will be absolute.
			 * @return The relative width.
			 */
			virtual Scalar GetRelativeWidth();

			/**
			 * Get the relative height of the element.
			 * If the Element has a parent then the relative height will be a percentage value of the parent element.
			 * If the Element is a root element then the height will be absolute.
			 * @return The relative height.
			 */
			virtual Scalar GetRelativeHeight();

			/**
			 * Get the absolute width of the element.
			 * The width will be the latest width based on the relative settings.
			 * For example: You if a parent is 100 in width and this element has a width of 0.5 then this
			 * method will return 50. The final size will be determined by the hierarchy of elements.
			 * @note This method does not trigger any updates, it simply returns the last known size of the Element,
			 * which should be up to date based on the update procedure of Element.
			 * @note The size returned by this method is relative to the inherited Node space, which is usually world
			 * space, but if any Node properties have been set to modify the size, including through the parent chain,
			 * such as scale then that will be applied during rendering too.
			 * @return The absolute width.
			 */
			virtual Scalar GetAbsoluteWidth() const;

			/**
			 * Get the absolute height of the element.
			 * The height will be the latest height based on the relative settings.
			 * For example: You if a parent is 100 in height and this element has a height of 0.5 then this
			 * method will return 50. The final size will be determined by the hierarchy of elements.
			 * @note This method does not trigger any updates, it simply returns the last known size of the Element,
			 * which should be up to date based on the update procedure of Element.
			 * @note The size returned by this method is relative to the inherited Node space, which is usually world
			 * space, but if any Node properties have been set to modify the size, including through the parent chain,
			 * such as scale then that will be applied during rendering too.
			 * @return The absolute height.
			 */
			virtual Scalar GetAbsoluteHeight() const;

			
			/**
			 * Get the aspect ratio of the final size of the Element.
			 * @note When using WIDTH_WITH_SET_ASPECT_RATIO, HEIGHT_WITH_SET_ASPECT_RATIO and MAX_FIT_WITH_SET_ASPECT_RATIO
			 * size modes, the aspect ratio should be the same as specified with SetAspectRatio(). It is important to note
			 * that the value returned by this method is not necessarily a direct reading of the value passed to
			 * SetAspectRatio(). The value returned is calculated using the final width and height after its last size change.
			 * @note The aspect ratio may be zero even with a valid specified ratio if the parent entity has a size of zero.
			 * In these cases this causes the element to collapse and have at least one dimension with a size of 0.
			 * @return the aspect ratio of the final size of the Element, otherwise 0 if the width or hight is 0.
			 */
			virtual Scalar GetAspectRatio() const;

			/**
			 * Specify the aspect ratio to be used in aspect ratio calculations.
			 * @note This is used in WIDTH_WITH_SET_ASPECT_RATIO, HEIGHT_WITH_SET_ASPECT_RATIO and
			 * MAX_FIT_WITH_SET_ASPECT_RATIO size modes.
			 */
			void SetAspectRatio(Scalar aspectRatio);
			
			/**
			 * Set the size of the element.
			 * If the Element has a parent then the size will be a percentage value of the parent element.
			 * If the Element is a root element then the size will be absolute.
			 * @note You cannot set the absolute size of the element in terms of world coordinates. The purpose of the GUI
			 * system is to provide a display system independent of specific coordinate space. Elements generally have a size
			 * set relative to their parent so if the root coordinate space changes the GUI layout can adapt proportionally.
			 * Sizing can be specified in physical dimensions which is useful if you always want an Element to be the same
			 * physical size on screen (e.g. 10cm x 1cm button), @see SetPhysicalSize() for more information.
			 * @note Setting to the same size does not cause an update.
			 * @param width The width.
			 * @param height The height.
			 */
			virtual void SetSize(Scalar width, Scalar height);
			void SetSize(const Vector2& size)
			{
				SetSize(size.x, size.y);
			}
			/**
			 * Set the physical (as in real world) size of the element.
			 * In order to accurately display using physical sizes the density of the RenderTarget is
			 * required. On mobile platforms this is usually available. Although on PC it is not always
			 * available if the operating system cannot determine the screen density. Newer displays can
			 * report their density which can be acquired by the OS but on some platforms, or with some
			 * types of displays, this may not be available.
			 * When the display density is not available it is up to the platform implementation to set
			 * an appropriate default. We specifically don't enforce a expected default since it might be
			 * quite platform dependent.
			 * A WorldUnitConverter with the correct world unit to physical ratio (as calculated from the
			 * Camera, Viewport and a RenderTarget) is required to make this work. The calculations aren't
			 * complicated (just some simple trig) and Screen can take care of them for you.
			 * 
			 * For convenience you do not need to convert to Metres manually, you can pass any of the
			 * Length types which will automatically convert to Metres for you. e.g.
			 *	SetPhyscialSize(Centimetre(1),Millimetre(10)).
			 */
			virtual void SetPhysicalSize(Metre width, Metre height);
			
			/**
			 * Set the size in pixels.
			 * This requires the WorldUnitConverter to be configured with the correct ratio to translate
			 * world space (the final space the Element exists in) to Pixel space on the RenderTarget.
			 * The calculations are relatively straightforward and usually involves a Camera, Viewport
			 * and RenderTarget. A Screen Element as the root element can take care of this for you
			 * automatically.
			 * 
			 * It is recommended to use relative or physical sizes where possible since it will
			 * allow you to get more consistent results across different devices. This is because
			 * displays have different pixel densities so an image 100pixels wide might be small on
			 * a high pixel density screen and large on a low density display. The physical size
			 * methods are designed to allow you to create interfaces that will be the same physical
			 * size (e.g. a button). Relative size (the default) sizing with SetSize() allow you to
			 * create consistent looking displays that scale well.
			 * 
			 * There are some applications where you need to specify in pixel sizes because scaling is
			 * performed by another metric. An example of this is Font point sizing. Rather than
			 * specifying how big a font will be in pixels the font sizes are often specified in point
			 * size, which is meant to look similar on different displays with different densities. The
			 * way this is accomplished is characters are rendered to a larger canvas for displays with
			 * higher pixel densities, and a smaller one for lower density displays, this also helps make
			 * the font look sharp.
			 * 
			 * The Text and TextBox Elements will automatically switch the Element sizing mode to pixels
			 * when they are provided with a point size to work with.
			 * @param width The width in pixels.
			 * @param height The height in pixels.
			 */
			virtual void SetSizeInPixels(Scalar width, Scalar height);

			/**
			 * Set the width of the element.
			 * @see SetSize() for more information.
			 * @note Setting to the same width does not cause an update.
			 * @param width The width.
			 */
			virtual void SetWidth(Scalar width);

			/**
			 * Set the physical width, before you do though @see SetPhysicalSize().
			 */
			virtual void SetPhysicalWidth(Metre width);

			/**
			 * Set the width in pixels, before you do though @see SetSizeInPixels().
			 */
			virtual void SetWidthInPixels(Scalar width);

			/**
			 * Set the height of the element.
			 * @see SetSize() for more information.
			 * @note Setting to the same height does not cause an update.
			 * @param height The height.
			 */
			virtual void SetHeight(Scalar height);

			/**
			 * Set the physical height, before you do though @see SetPhysicalSize().
			 */
			virtual void SetPhysicalHeight(Metre height);

			/**
			 * Set the height in pixels, before you do though @see SetSizeInPixels().
			 */
			virtual void SetHeightInPixels(Scalar height);

			/**
			 * Process the Element with the world position of the cursor and call methods upon cursor events.
			 * This method will acquire a list of all Elements in the hierarchy from the object it is call
			 * on. The elements will be sorted via Z position then an intersection test is performed on each
			 * for element processing.
			 * This method calls the overload UpdateCursor(x,y,pressed,intersected) below for each element.
			 * @note Cursor event methods are only called if the Element is targetable. @see SetTargetable()
			 * @param absoluteX The world X position of the cursor.
			 * @param absoluteY The world Y position of the cursor.
			 * @param pressed Whether the cursor is being "pressed", for a Mouse device this would be if it
			 * was clicked, for a touch screen device it will be when the screen is being touched.
			 */
			void UpdateCursor(Scalar absoluteX, Scalar absoluteY, bool pressed, IntersectTestMethod intersectMethod = IntersectTestMethods::BOUNDS_2D);

			/**
			 * Update the Element's internal state with the cursor information.
			 * This overload does not perform any intersection tests and relies on the caller to have already
			 * performed the test and indicate as such with the intersected parameter.
			 * The position parameters are just passed to the internal event methods.
			 * Use the other overload of this method if you have the cursor position (world) and pressed
			 * state and want intersection tests done for you.
			 * @note Cursor event methods are only called if the Element is targetable. @see SetTargetable()
			 * @param absoluteX The world X position of the cursor.
			 * @param absoluteY The world Y position of the cursor.
			 * @param pressed Whether the cursor is being "pressed", for a Mouse device this would be if it
			 * was clicked, for a touch screen device it will be when the screen is being touched.
			 * @param intersected whether or not the cursor intersected with this Element.
			 */
			virtual void UpdateCursor(Scalar absoluteX, Scalar absoluteY, bool pressed, bool intersected);

			/**
			 * Method called when the Cursor enters the bounds of the element.
			 * @param absoluteX The world X position of the cursor.
			 * @param absoluteY The world Y position of the cursor.
			 */
			virtual void OnCursorEnter(Scalar absoluteX, Scalar absoluteY);
			
			/**
			 * Method called when the Cursor has entered and is hovering within the bounds of the element.
			 * @param absoluteX The world X position of the cursor.
			 * @param absoluteY The world Y position of the cursor.
			 */
			virtual void OnCursorHover(Scalar absoluteX, Scalar absoluteY);

			/**
			 * Method called when the Cursor leaves the bounds of the element.
			 * @param absoluteX The world X position of the cursor.
			 * @param absoluteY The world Y position of the cursor.
			 */
			virtual void OnCursorLeave(Scalar absoluteX, Scalar absoluteY);
			
			/**
			 * Method called when the Cursor is pressed.
			 * This method is called once, after which cursor hold will be called if the cursor is still
			 * pressed for the next frame.
			 * @param absoluteX The world X position of the cursor.
			 * @param absoluteY The world Y position of the cursor.
			 */
			virtual void OnCursorPress(Scalar absoluteX, Scalar absoluteY);
			
			/**
			 * Method called when the Cursor is held.
			 * This method is called while the cursor is held over the element.
			 * @param absoluteX The world X position of the cursor.
			 * @param absoluteY The world Y position of the cursor.
			 */
			virtual void OnCursorHold(Scalar absoluteX, Scalar absoluteY);

			/**
			 * Method called when the Cursor is released.
			 * This method is called when the cursor is released while over the Element.
			 * If the cursor is held and moves outside of the element bounds then release this method
			 * will not be called.
			 * @param absoluteX The world X position of the cursor.
			 * @param absoluteY The world Y position of the cursor.
			 */
			virtual void OnCursorRelease(Scalar absoluteX, Scalar absoluteY);
			
			/**
			 * Get the position of the element relative to the parent in percentage values.
			 * @note If the element does not have a parent then the relative value ends up
			 * being the absolute value.
			 * @return The position relative to the parent in percentage values.
			 */
			virtual const Vector3& GetRelativePosition() const;

			/**
			 * Get the position of the element in world space.
			 * Use this method when you need to position something, such as a particle effect, at the same
			 * location as the element. If you are positioning elements with respect to one another you
			 * should use relative positioning.
			 * @note Element ultimately inherit from Node which has a GetPosition(). When interacting with
			 * the GUI system you should always use the Element versions of methods rather than the lower
			 * level Node methods to be sure you get results in terms of the GUI. An absolute position in
			 * GUI terms is a world position and it is a cached value (in most cases up to date). Internally
			 * the position is defined with respect to a parent Element (with the root Element as the exception).
			 * @return The position of the element in the world.
			 */
			virtual const Vector3& GetAbsolutePosition() const;
			
			/**
			 * Set the position of the element relative to the parent in percentage value.
			 * @param val The position relative to the parent in percentage values.
			 */
			virtual void SetRelativePosition(const Vector3& val);
			
			/**
			 * Set the position of the element relative to the parent in percentage values.
			 * @param x The x position relative to the parent in percentage value.
			 * @param y The y position relative to the parent in percentage value.
			 * @param z The z position relative to the parent in percentage value.
			 */
			virtual void SetRelativePosition(f32 x, f32 y, f32 z);
			
			/**
			 * Set the horizontal alignment.
			 * @note Using a default camera, minimum is left and maximum is right.
			 */
			void SetHorizontalAlignment(Alignment alignment);

			/**
			 * Set the vertical alignment.
			 * @note Using a default camera, minimum is down and maximum is up.
			 */
			void SetVerticalAlignment(Alignment alignment);

			/**
			 * Set the function binder for the element.
			 * The function binder will be passed to any child element as they are added if
			 * the child does not already have one. This allows you to set the function
			 * binder for the root element and then as you add all other elements (from
			 * top to bottom in the tree) the function binder will be set.
			 * @param functionBinder Pointer to the function binder with which the functions
			 * added with Add*Function() should be bound with.
			 */
			void SetFunctionBinder(shared_ptr<FunctionBinder> functionBinder);
			
			/**
			 * Get the function binder.
			 * @return the function binder.
			 */
			shared_ptr<FunctionBinder> GetFunctionBinder() const;
			
			/**
			 * Add and event functions.
			 * The function will be called (through use of the function binder) when the
			 * cursor event occurs for the element.
			 * 
			 * The function needs to be registered with the function binder in order for
			 * the call to work.
			 * 
			 * @note We are using function binder calls for elements because it has been
			 * designed with a data driven model in mind. For situations where you need
			 * to perform a lot of work for a specialised element, consider implementing
			 * your own class and performing actions in an overridden On* method.
			 * @param function The function to call.
			 */
			void AddEnterFunction(const std::string& function);
			void AddHoverFunction(const std::string& function);
			void AddLeaveFunction(const std::string& function);
			void AddPressedFunction(const std::string& function);
			void AddHoldFunction(const std::string& function);
			void AddReleasedFunction(const std::string& function);

			/**
			 * Conditional actions are checked when certain properties are changed, such as size.
			 * If the condition evaluates to true the function will be executed.
			 * Multiple functions can be attached to the same condition by calling this method
			 * multiple times with the same condition or by calling AddConditionalFunctions().
			 * 
			 * Current support:
			 *	- Available variables
			 *		width					Absolute width of this element.
			 *		height					Absolute height of this element.
			 *		aspect_ratio			Aspect ratio of this element.
			 *		parent.width			Absolute width of the parent element.
			 *		parent.height			Absolute height of the parent element.
			 *		parent.aspect_ratio		Aspect ratio of the parent element.
			 *		root.width				Absolute width of the root element.
			 *		root.height				Absolute height of the root element.
			 *		root.aspect_ratio		Aspect ratio of the root element.
			 * 
			 *  - Functions available depend entirely on the function binder.
			 * 
			 * @param condition The condition string, @see Parser::ConditionalEvaluator
			 * @param function The function registered with the function binder.
			 */
			void AddConditionalFunction(const std::string& condition, const std::string& function);
			void AddConditionalFunctions(const std::string& condition, const std::list<std::string>& functions);

			/**
			 * Look for an element by name with an option to search children.
			 * The search occurs in this element first, if no child is found each child is
			 * searched.
			 * @note There is no guarantee as to the order the child elements are searched
			 * due to the way elements are stored, so you should try to ensure elements
			 * have unique names.
			 * @param name The name of the element.
			 * @param searchChildren if true the child elements will be searched as well.
			 * @return The named element otherwise a null pointer if the element isn't found.
			 */
			shared_ptr<Element> FindElement(const std::string& name, bool searchChildren = true) const;

			/**
			 * Find all elements with a given name.
			 * @see FindElement()
			 * @note If searchChildren is true the search is performed as depth first, where as FindElement()
			 * will always check the names of all children before continuing the search on the child elements.
			 * @param name The name of the element(s) you want to find.
			 * @param searchChildren If true then the search will include child elements searches.
			 * @return A list of elements whose name matches the specified name.
			 */
			std::list< shared_ptr<Element> > FindElements(const std::string& name, bool searchChildren = true) const;

			/**
			 * Clone the element along with each child element.
			 * The new element will have a cloned copy of each child element.
			 * The parent element is not used. You will still need to add a cloned element to
			 * a parent Element yourself.
			 */
			shared_ptr<Element> Clone() const;
			
			/**
			 * WorldUnitsConverter stores a ratio of physical dimensions to
			 * world dimensions which is used to convert physical values to world values.
			 * A single converter object can be used for an entire hierarchy and is normally
			 * set at the root node (in most cases a screen where the screen DPI is know).
			 */
			class WorldUnitsConverter
			{
			public:
				/**
				 * Default any of these parameters to 1 if they are not known since that is
				 * the default if no unit converter is created for an element.
				 * @param unitsPerMetreX World units per metre.
				 * @param unitsPerMetreX World units per metre.
				 * @param unitsPerPixelX World units per pixel.
				 * @param unitsPerPixelY World units per pixel.
				 */
				WorldUnitsConverter(Scalar unitsPerMetreX, Scalar unitsPerMetreY, Scalar unitsPerPixelX, Scalar unitsPerPixelY) :
					mWorldUnitsPerMetreX(unitsPerMetreX),
					mWorldUnitsPerMetreY(unitsPerMetreY),
					mWorldUnitsPerPixelX(unitsPerPixelX),
					mWorldUnitsPerPixelY(unitsPerPixelY)
				{}
				~WorldUnitsConverter()
				{}
				
				/**
				 * Set the number of world units per metre.
				 * @param unitsPerMetreX world units per metre on the X axis.
				 * @param unitsPerMetreY world units per metre on the Y axis.
				 */
				void SetUnitsPerMetre(Scalar unitsPerMetreX, Scalar unitsPerMetreY)
				{
					mWorldUnitsPerMetreX = unitsPerMetreX;
					mWorldUnitsPerMetreY = unitsPerMetreY;
				}
				void SetUnitsPerMetreX(Scalar unitsPerMetre) { mWorldUnitsPerMetreX = unitsPerMetre;}
				void SetUnitsPerMetreY(Scalar unitsPerMetre) { mWorldUnitsPerMetreY = unitsPerMetre;}

				/**
				 * Set the world units per pixel.
				 * These ratios make sense when the root element is a Screen object. They are used
				 * to size the Element relative to a known Element size, much in the same way
				 * physical sizes can be used, for example 10cm, using 10px means the element will
				 * maintain 10px in size if the root element has set the ratio correctly.
				 * 
				 * For example, if a root element takes up 100% of a camera's FOV it will take up
				 * 100% of the pixels on screen. If the resolution was 1000 but the world space is
				 * only 10 units, then the world to pixel ratio is 10/1000, or 0.01.
				 * 
				 * Screen takes care of these calculations for you if you're displaying on a
				 * RenderTarget, but if you're displaying on another surface you might want to set
				 * this ratio yourselt.
				 * 
				 * mapping to another surface you will want
				 * 
				 * @param worldUnitsPerPixelX Number of world units that correspond to a pixel.
				 * @param worldUnitsPerPixelY Number of world units that correspond to a pixel.
				 */
				void SetUnitsPerPixel(Scalar worldUnitsPerPixelX, Scalar worldUnitsPerPixelY)
				{
					mWorldUnitsPerPixelX = worldUnitsPerPixelX;
					mWorldUnitsPerPixelY = worldUnitsPerPixelY;
				}
				void SetWorldUnitsPerPixelX(Scalar worldUnitsPerPixelX) { mWorldUnitsPerPixelX = worldUnitsPerPixelX;}
				void SetWorldUnitsPerPixelY(Scalar worldUnitsPerPixelY) { mWorldUnitsPerPixelY = worldUnitsPerPixelY;}
				
				/**
				 * Get the number of world units per metre.
				 * @return 
				 */
				Scalar GetUnitsPerMetreX() const {return mWorldUnitsPerMetreX;}
				Scalar GetUnitsPerMetreY() const {return mWorldUnitsPerMetreY;}
				
				/**
				 * Get the number of world units per pixel.
				 * @return 
				 */
				Scalar GetUnitsPerPixelX() const {return mWorldUnitsPerPixelX;}
				Scalar GetUnitsPerPixelY() const {return mWorldUnitsPerPixelY;}

				/**
				 * Convert physical units to world units.
				 * @param metres The physical unit to convert, since this is a Length type you
				 * can specify other units and it will automatically convert.
				 * @return The measurement in world units.
				 */
				Scalar ToWorldUnitsX(Metre metres) const
				{
					return mWorldUnitsPerMetreX * metres.Count();
				}
				
				Scalar ToWorldUnitsY(Metre metres) const
				{
					return mWorldUnitsPerMetreY * metres.Count();
				}
				
				/**
				 * Convert pixels to world units.
				 * @param pixels The number of pixels to convert to world units.
				 * @return The measurement in world units.
				 */
				Scalar PixelsToWorldUnitsX(Scalar pixels) const
				{
					return mWorldUnitsPerPixelX * pixels;
				}
				
				Scalar PixelsToWorldUnitsY(Scalar pixels) const
				{
					return mWorldUnitsPerPixelY * pixels;
				}
			private:
				Scalar mWorldUnitsPerMetreX;
				Scalar mWorldUnitsPerMetreY;
				Scalar mWorldUnitsPerPixelX;
				Scalar mWorldUnitsPerPixelY;
			};
			
			/**
			 * This sets the number of world units per physical metre.
			 * Sets the ratio of the local unit converter. This allows you to override
			 * the parent's (directly or up the chain) converter if needed.
			 * Normally you'll just want to set the ratio on the root Element.
			 */
			void SetWorldToMetreRatio(Scalar worldUnitsPerMetreX, Scalar worldUnitsPerMetreY)
			{
				if(!mUnitConverter)
				{
					mUnitConverter = make_shared<WorldUnitsConverter>(worldUnitsPerMetreX, worldUnitsPerMetreY,1,1);
				}else
				{
					mUnitConverter->SetUnitsPerMetre(worldUnitsPerMetreX, worldUnitsPerMetreY);
				}
			}
			
			/**
			 * This sets the pixel to world unit ratio
			 * Sets the ratio of the local unit converter. This allows you to override
			 * the parent's (directly or up the chain) converter if needed.
			 * Normally you'll just want to set the ratio on the root Element.
			 */
			void SetWorldToPixelRatio(Scalar worldUnitsPerPixelX, Scalar worldUnitsPerPixelY)
			{
				if(!mUnitConverter)
				{
					mUnitConverter = make_shared<WorldUnitsConverter>(1,1,worldUnitsPerPixelX, worldUnitsPerPixelY);
				}else
				{
					mUnitConverter->SetUnitsPerPixel(worldUnitsPerPixelX, worldUnitsPerPixelY);
				}
			}
			
			/**
			 * Set the WorldUnitsConverter
			 * This sets the the local converter and overrides the inherited one.
			 * @see WorldUnitsConverter
			 */
			void SetWorldUnitsConverter(shared_ptr<WorldUnitsConverter> converter)
			{
				mUnitConverter = converter;
			}

			/**
			 * Remove one or bother of the PhysicalToWorldUnitsConverters.
			 * @param local Remove the local unit converter.
			 * @param inherited Remove the inherited unit converter.
			 */
			void RemoveWorldUnitsConverters(bool local = true, bool inherited = true)
			{
				if(local)
				{
					mUnitConverter.reset();
				}
				if(inherited)
				{
					mCachedParentUnitConverter.reset();
				}
			}
			
			/**
			 * Get the WorldUnitsConverter
			 * If this object does not have a converter it will attempt to find one in the hierarchy
			 * looking through the parent chain. If a parent object is found it will be cached.
			 * @return The first a WorldUnitsConverter found in the hierarchy.
			 */
			shared_ptr<WorldUnitsConverter> GetWorldUnitsConverter()
			{
				if(mUnitConverter)
				{
					return mUnitConverter;
				}
				if(!mCachedParentUnitConverter && mParent)
				{
					mCachedParentUnitConverter = mParent->GetWorldUnitsConverter();
				}
				return mCachedParentUnitConverter;
			}
			
			/**
			 * Recursive method that will add this element to the vector if it is targetable,
			 * then call the method on each of the child elements. If the Element is not visible
			 * the method will return immediately.
			 * This method will add all targetable elements in the hierarchy to the list.
			 */
			void AddTargetablesToList(std::vector< shared_ptr<Element> >& targetableElements);
			
			////////////////////////////////////////
			//PROPERTY METHODS
			////////////////////////////////////////

			/**
			 * Get the number of properties with the specified name.
			 * This will return the number of times a given property is specified, for example if you query with "person" and there is at least one
			 * line in the configuration file with "person" then the value returned will be 1 or more. The value represents the maxIndex+1 for
			 * that property.
			 * @param propertyName
			 * @return the number of entries for the specified property name.
			 */
			size_t GetNumberOfPropertiesNamed(const std::string& propertyName) const
			{
				return mProperties.GetNumberOfOptionsNamed(propertyName);
			}

			/**
			 * Get a property value.
			 * Use this version of Get() if you don't care if the option is available or not.
			 * @param  propertyName The name of the property to use as a key for look-up.
			 * @param  defaultValue If the configuration option cannot be found, or conversion fails, this will be the value returned.
			 * @return The configuration option if found, else the default value.
			 */
			template< typename T >
			T GetProperty(const std::string& propertyName, const T& defaultValue) const
			{
				return mProperties.Get<T>(propertyName, defaultValue);
			}

			template<size_t size>
			std::string GetProperty(const std::string& propertyName, const char (&defaultValue)[size]) const
			{
				return mProperties.Get<size>(propertyName, std::string(defaultValue));
			}

			/**
			 * Get a property value at the specified index.
			 * @param propertyName name of the property.
			 * @param defaultValue if the configuration option cannot be found or conversion fails this will be the value of outObject.
			 * @param index The index of the value.
			 * @return The configuration option or the default value.
			 */		
			template< typename T >
			T GetPropertyAtIndex(const std::string& propertyName, const T& defaultValue, size_t index) const
			{
				return mProperties.GetAtIndex<T>(propertyName, defaultValue, index);
			}

			/**
			 * Get a property value.
			 * @param propertyName name of the property.
			 * @param outObject The object to assign the value.
			 * @param defaultValue if the configuration option cannot be found or conversion fails this will be the value of outObject.
			 * @param index index of the configuration value if there are multiple of the same line.
			 * @param suppressWarning if true will suppress the property not found warning.
			 * @return true if successful.
			 */
			template< typename T >
			bool GetProperty(const std::string& propertyName, T& outObject, const T& defaultValue, size_t index = 0, bool suppressWarning = false) const
			{
				return mProperties.Get<T>(propertyName, outObject, defaultValue, index, suppressWarning);
			}

			/**
			 * Get all of the properties values in a vector using the specified key.
			 * This method will attempt to convert all of the values of all entries that have the specified key to the specified type.
			 * If any of the conversions fail the method will return an empty list.
			 * If the key isn't found the method will return an empty list.
			 * @param optionName The name, or key, to look up.
			 * @param suppressWarning if true any warnings will be suppressed
			 * @return An empty vector if the option name isn't found or if a conversion fails, otherwise a vector containing the converted values.
			 */
			template< typename T >
			std::vector<T> GetAllProperties(const std::string& propertyName, bool suppressWarning = false) const
			{
				return mProperties.GetAll<T>(propertyName, suppressWarning);
			}

			/**
			 * Set a property and attempt an automatic conversion.
			 * @see Configuration::Set() - non-template version
			 * @return false, if the conversion to a string failed otherwise the result from non-templated Set().
			 */	
			template< typename T >
			bool SetProperty(const std::string& optionName, const T& optionValue, bool overwriteExisting = true)
			{
				return mProperties.Set<T>(optionName,optionValue,overwriteExisting);
			}

			/**
			 * Set a property by name with the option to overwrite existing values.
			 * @param propertyName The option's name, used to reference the property.
			 * @param propertyValue The value of the option.
			 * @param overwriteExisting if true any property with the same name will be overwritten.
			 * @return If overwriteExisting is false and the property exists the method will return false,
			 * otherwise true.
			 */
			bool SetProperty(const std::string& propertyName, const std::string& propertyValue, bool overwriteExisting = true)
			{
				return mProperties.Set(propertyName, propertyValue, overwriteExisting);
			}

			/**
			 * Add a property and attempt to automatically convert to a storage string.
			 * This method appends the value to the option name and never overwrites.
			 * The type should be convertible using a stringstream.
			 * @param propertyName the name of the option.
			 * @param propertyValue The value.
			 * @return true if the option is added, false if conversion failed.
			 */
			template< typename T >
			bool AddProperty(const std::string& propertyName, const T& propertyValue)
			{
				return mProperties.Add(propertyName, propertyValue);
			}

			/**
			 * Add a property.
			 * This method appends the value to the option name and never overwrites.
			 * @param optionName the option name
			 * @param optionValue
			 */
			void AddProperty(const std::string& propertyName, const std::string& propertyValue)
			{
				mProperties.Add(propertyName,propertyValue);
			}

			/**
			 * Remove a property by name.
			 * @param optionName
			 * @return false if the option doesn't exist, otherwise true if the option was found and removed.
			 */
			bool RemoveProperty(const std::string& optionName)
			{
				return mProperties.Remove(optionName);
			}
			////////////////////////////////////////
			// End Property methods
			////////////////////////////////////////
		protected:
			/**
			 * Clone the specific Element object.
			 * This method is designed to be internal and called by Clone() which also worries
			 * about cloning child objects and adding them to the new parent. Without this
			 * internal _Clone() method then each implementation would also have to clone the
			 * children.
			 * _Clone() is called before each child is cloned so each element is added after
			 * the object has been cloned which means each implementation only has to worry
			 * about specific cloning details and not dealing with child elements.
			 * @return The cloned element.
			 */
			virtual shared_ptr<Element> _Clone() const = 0;
			
			/**
			 * Internal method called when a parent's size changes.
			 * This causes the element to update its size as well.
			 */
			virtual void OnParentSizeUpdate();
			/**
			 * Internal method called when a parent's position changes.
			 * This causes the element to update its position.
			 */
			virtual void OnParentPositionUpdate();

			/**
			 * Internal method called to process conditional expressions.
			 */
			virtual void ProcessConditionals();
			
			/**
			 * Internal method called when the element needs to be resized.
			 * The specified width and height are absolute values that the element must
			 * set its size to.
			 * 
			 * The specified dimensions will be provided as per the size mode settings
			 * so there is no need to adjust for aspect ratio.
			 *
			 * @note These values are NOT percentage values, these are actual dimensions
			 * that should be used.
			 * @param newWidth The new width to set the element to.
			 * @param newHeight The new height to set the element to.
			 */
			virtual void UpdateSize(Scalar newWidth, Scalar newHeight) = 0;
			
			/**
			 * This method should attempt to calculate the native aspect ratio of the element if possible.
			 * Often Elements will be created with external data which ideally would determine the aspect
			 * ratio designed for viewing. This method is called when the Element's size mode is set to
			 * attempt to use this native aspect ratio for sizing operations.
			 * @note This method should never return 0.
			 * @return The method should return either the width/height of the data or if not possible 1 to
			 * indicate a ratio of 1:1.
			 */
			virtual Scalar GetNativeAspectRatio() = 0;
			
			/**
			 * Internal method called when the size updates.
			 * This method causes a recalculation of the size of the element and
			 * eventually calls UpdateSize().
			 */
			virtual void SizeUpdated();
			
			/**
			 * Internal method called when the position updates.
			 * This method causes a recalculation of the position of the element and
			 * performs the reposition.
			 */
			virtual void PositionUpdated();
			
			virtual void ChildRemoved(Node& node) override;

			/**
			 * Reset the cursor state to the initial state which requires the cursor
			 * to be released before any event processing is done.
			 * This method will also be called for all child elements.
			 */
			void ResetCursorState();
		private:
			/**
			 * Calling GetPosition() will return the position from Node and not the
			 * relative position that you set calling this method. This method is
			 * overridden and made private because Node::SetPosition() is not what you want
			 * to access when dealing with Elements. The Node's position value is
			 * calculated and set in Element::PositionUpdated().
			 */
			virtual void SetPosition(const Vector3& val) override;

			/**
			 * Calling GetPosition() will return the position from Node and not the
			 * relative position that you set calling this method. This method is
			 * overridden and made private because Node::SetPosition() is not what you want
			 * to access when dealing with Elements. The Node's position value is
			 * calculated and set in Element::PositionUpdated().
			 */
			virtual void SetPosition(f32 x, f32 y, f32 z) override;
			
			/**
			 * Restrict access to GetPosition() to get people to use GetRelativePosition()
			 * or GetAbsolutePosition().
			 */
			virtual const Vector3& GetPosition() const override;
			
			void SetParentElement(Element* parent);
			
			/**
			 * Helper method as for MAX_FIT_WITH_SET_ASPECT_RATIO.
			 * See documentation in the method for further information and some really cool ASCII art. The
			 * documentation is too long to put here given it is a private method. It illustrates all cases.
			 * @note Although you can call this method with any values it usually is only called with values
			 * greater than zero as working with aspect ratios of zero or less usually results in a size of
			 * zero or an undefined size. The result does not take into account cases where these parameters
			 * are less than or equal to zero.
			 * @param aspectRatio The aspect ratio of this element.
			 * @param parentAspectRatio The aspect ratio of the parent element.
			 * @return true if sizing should be the max width, otherwise false if sizing should be the max height.
			 */
			inline bool GetMaxFitWidth(const Scalar& aspectRatio, const Scalar& parentAspectRatio);
			
			struct CursorStates
			{
				enum _
				{
					INITIAL,	///Before the cursor has not been detected, this state remains until the
								///cursor is released to prevent events occurring if an Element is added
								///to another during a cursor press and the new element appears under
								///the cursor. This state is set when an Element is given a new parent
								///or becomes visible via Show().
					NONE,		///When the cursor is not in the bounds.
					ENTER,		///When the cursor just entered.
					HOVER		///When the cursor has an intersect for a second frame or more.
				};
			};
			typedef CursorStates::_ CursorState;
			
			class Dimension
			{
			private:
				struct Units
				{
					enum _
					{
						PERCENT,	/// Percent of the parent type, this is default.
						ABSOLUTE,	/// Absolute size, in world units.
						PIXELS,		/// Size in pixels, a converter will need to be supplied to the Element
									/// with the units to pixel ratio specific.
						PHYSICAL,	/// Absolute size, in physical units, a converter will need to be
									/// supplied to the Element that specifies the units to metres ratio.
					};
				};
				typedef Units::_ Unit;
			public:
				Dimension(Metre value = Metre(1.f)) :
					mUnitType(Units::PERCENT),
					mValue(1.f)
				{}

				void SetRelative(Scalar rhs)
				{
					mValue=rhs;
					mUnitType=Units::PERCENT;
				}
				
				/**
				 * Get the value as a scalar.
				 * This should be called if the Unit type is anything but a physical size.
				 */
				Scalar GetScalar()
				{
					return mValue.Count();
				}

				/**
				 * Get the value as a physical value.
				 */
				Metre GetPhysical()
				{
					return mValue;
				}
				
				/**
				 * Get whether the value is in percent relative to the parent size or not.
				 * @return true if the value is a percent relative to the parent size, otherwise false.
				 */
				bool IsRelative() const {return (mUnitType==Units::PERCENT);}

				/**
				 * Get whether the value is absolute size or not.
				 * @return true if the value is a absolute size, otherwise false.
				 */
				bool IsAbsolute() const {return (mUnitType==Units::ABSOLUTE);}

				/**
				 * Get whether the value is in pixels or not.
				 * @return true if the value is a physical measurement, otherwise false.
				 */
				bool IsPixel() const {return (mUnitType==Units::PIXELS);}

				/**
				 * Get whether the value is a physical measurement or not.
				 * @return true if the value is a physical measurement, otherwise false.
				 */
				bool IsPhysical() const {return (mUnitType==Units::PHYSICAL);}

				void SetAbsolute(Scalar rhs)
				{
					mValue=rhs;
					mUnitType=Units::ABSOLUTE;
				}
				
				void SetPhysical(Metre rhs)
				{
					mValue=rhs;
					mUnitType=Units::PHYSICAL;
				}
				
				void SetPixel(Scalar rhs)
				{
					mValue=rhs;
					mUnitType=Units::PIXELS;
				}
				
				inline bool IsZeroOrNegative() const
				{
					return (mValue.Count()<=0.f);
				}

				Scalar CalculateAsWidth(shared_ptr<WorldUnitsConverter> unitConverter, Element* parent)
				{
					switch(mUnitType)
					{
						case Units::PERCENT:
							if(parent)
							{
								return mValue.Count()*parent->GetAbsoluteWidth();
							}
							//Fall through
						case Units::ABSOLUTE:
							return mValue.Count();

						case Units::PIXELS:
							if(unitConverter)
							{
								return unitConverter->PixelsToWorldUnitsX(mValue.Count());
							}
							return mValue.Count();

						case Units::PHYSICAL:
							if(unitConverter)
							{
								return unitConverter->ToWorldUnitsX(mValue);
							}
							return mValue.Count();
					}
					//Shouldn't be reached, but just incase.
					return mValue.Count();
				}
				
				Scalar CalculateAsHeight(shared_ptr<WorldUnitsConverter> unitConverter, Element* parent)
				{
					switch(mUnitType)
					{
						case Units::PERCENT:
							if(parent)
							{
								return mValue.Count()*parent->GetAbsoluteHeight();
							}
							//Fall through
						case Units::ABSOLUTE:
							return mValue.Count();

						case Units::PIXELS:
							if(unitConverter)
							{
								return unitConverter->PixelsToWorldUnitsY(mValue.Count());
							}
							return mValue.Count();

						case Units::PHYSICAL:
							if(unitConverter)
							{
								return unitConverter->ToWorldUnitsY(mValue);
							}
							return mValue.Count();
					}
					//Shouldn't be reached, but just incase.
					return mValue.Count();
				}

			private:
				Unit mUnitType;
				Metre mValue;	/// The value, stored as metres for easy converting.
			};
			
			typedef std::list< std::string > FunctionContainer;
			typedef std::list< shared_ptr<Element> > ChildContainer;
			ChildContainer mChildElements;
			FunctionContainer mEnterFunctions;
			FunctionContainer mHoverFunctions;
			FunctionContainer mLeaveFunctions;
			FunctionContainer mPressedFunctions;
			FunctionContainer mHoldFunctions;
			FunctionContainer mReleasedFunctions;
			typedef std::map< std::string, std::list<std::string> > ConditionalFunctionMap;
			ConditionalFunctionMap mConditionalFunctions;
			shared_ptr<FunctionBinder> mFunctionBinder;
			bool mTargetable;				/// Most elements can be targeted by the cursor, this is true by default. Set
											/// to false to pass input to children on intersect.
			Dimension mSetWidth;			/// The width this element has been set to.
			Dimension mSetHeight;			/// The height this element has been set to.
			Scalar mAspectRatio;			/// Used in aspect ratio modes to calculate the width or height, default 1.
			Scalar mWidth;					/// The actual width determined by the size mode and parent size, in parent space.
			Scalar mHeight;					/// The actual height determined by the size mode and parent size, in parent space.
			SizeMode mSizeMode;
			CursorState mCursorState;
			bool mCursorPressed;
			Vector3 mPercentPosition;
			Element* mParent;
			Alignment mHorizontalAlignment;
			Alignment mVerticalAlignment;
			shared_ptr<WorldUnitsConverter> mUnitConverter;				/// Unit converter created by/for this object,
																					/// takes priority over mCachedParentUnitConverter.
			shared_ptr<WorldUnitsConverter> mCachedParentUnitConverter;	/// The unit converter acquired via the parent chain.
			Configuration mProperties;
			
			shared_ptr<Parser::ConditionalEvaluator> mConditionalEvaluator;
			void SetupConditionalEvaluatorParser();
			
			/**
			 * Internal methods for the conditional evaluator.
			 */
			Scalar GetParentWidth() const;
			Scalar GetParentHeight() const;
			Scalar GetParentAspectRatio() const;
			Scalar GetRootWidth() const;
			Scalar GetRootHeight() const;
			Scalar GetRootAspectRatio() const;
		};
	}
}
#endif 
