#ifndef ONSCREENANALOGUESTICK_H
#define ONSCREENANALOGUESTICK_H
#include <echo/Input/InputDevice.h>
#include <echo/GUI/Container.h>

namespace Echo
{
	class Material;
	
	/**
	 * An interactive GUI Element to mimic the behaviour of an analogue input device such as a control stick.
	 * The default name for the input device is "OnScreenAnalogueStick" and it has the following inputs:
	 *	- Stick as a Vector2
	 *  - X mapped directly to Stick.x
	 *  - Y mapped directly to Stick.y
	 * 
	 * OnScreenAnalogueStick is a GUI::Container that has two child elements:
	 *  - A background Element that does not move.
	 *  - A stick Element that is positioned around the container within a circle.
	 * 
	 * There are two ways to create an OnScreenAnalogueStick Element:
	 * - By passing in two materials, this will create two child Image Elements and apply the materials.
	 * - By passing two Elements.
	 * 
	 * In both cases the child elements are modified for the following before being added to the container:
	 * - Positioned at 0.5,0.5 relative to the container
	 * - Targetable flag is disabled.
	 * 
	 * When the class creates Image Elements for you (by passing materials), the stick Element is given a
	 * size of 0.3,0.3. You can modify this by using GetStickElement() and making the adjustment through
	 * one of the Element set size methods.
	 */
	class OnScreenAnalogueStick : public InputDevice, public GUI::Container
	{
	public:
		struct StickStates
		{
			enum _
			{
				IDLE,
				HELD
			};
		};
		typedef StickStates::_ StickState;
		
		/**
		 * Constructor, using materials.
		 * @param backgroundMaterial The material to be applied to the background Image Element
		 * @param stickMaterial The material to be applied to the stick Element.
		 */
		OnScreenAnalogueStick(shared_ptr<Material> backgroundMaterial, shared_ptr<Material> stickMaterial);
		
		/**
		 * Constructor using Elements.
		 * This constructor will assert on null parameters.
		 * Both Elements will be positioned to 0.5,0.5 and set to not targetable.
		 * @param backgroundMaterial The background Element, must not be null.
		 * @param stickMaterial The stick Element, must not be null.
		 */
		OnScreenAnalogueStick(shared_ptr<GUI::Element> backgroundMaterial, shared_ptr<GUI::Element> stickMaterial);
		~OnScreenAnalogueStick();
		
		/**
		 * Get the current internal state of the virtual stick.
		 * @return the stick state.
		 */
		StickState GetStickState() const {return mStickState;}
		
		/**
		 * Get the stick Element.
		 * @return The stick Element. This will always return a valid Element.
		 */
		shared_ptr<GUI::Element> GetStickElement() const { return mStickElement;}

		/**
		 * Get the background element.
		 * @return The background element. This will always return a valid Element.
		 */
		shared_ptr<GUI::Element> GetBackgroundElement() const { return mBackgroundElement;}

		/**
		 * We are going to intercept slightly lower level cursor update events because
		 * the OnCursor methods don't allow us to track once the cursor has left.
		 */
		void UpdateCursor(Scalar absoluteX, Scalar absoluteY, bool pressed, bool intersected);
	private:
		/**
		 * Register the input bindings.
		 * This should only happen once during construction.
		 */
		void RegisterInput();
		
		StickState mStickState;
		shared_ptr<Vector2> mStick;
		shared_ptr<GUI::Element> mBackgroundElement;
		shared_ptr<GUI::Element> mStickElement;
	};
}

#endif
