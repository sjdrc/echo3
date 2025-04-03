#ifndef _ECHOGUICURSOR_H_
#define _ECHOGUICURSOR_H_

#include <echo/Kernel/Task.h>
#include <echo//Input/Input.h>

namespace Echo
{
	class Renderer;
	namespace GUI
	{
		class Element;
		
		/**
		 * A Cursor deals with input processing of cursor input from a render target to the GUI system.
		 * A Cursor needs the Input objects for the button press, X and Y (which should be input of a
		 * cursor on the render target). The inputs will be used to determine if there is an intersection
		 * with the GUI plane and pass the transformed input to the GUI system accordingly.
		 */
		class Cursor : public Task
		{
		public:
			/**
			 * Constructor.
			 * @param button The button that will be considered the pressed button.
			 * @note All parameters are required except for the cursorElement. The Task will fail to start
			 * if any parameter is null.
			 * @param xInput The x coordinate input source for the cursor/pointer in render target space.
			 * @param yInput The y coordinate input source for the cursor/pointer in render target space.
			 * @param rootElement The root GUI element that input is injected into. This element should
			 * not have a parent Element or Node (unless the node is at the Scene origin).
			 * @param renderer The renderer by which coordinates are transformed from. The Renderer will
			 * have a Viewport, Camera and RenderTarget which is used to determine the Scene space
			 * coordinates from the render target coordinates (xInput and yInput).
			 * @param cursorElement If not null a GUI Element that will be repositioned to the Scene
			 * space position. This element should be a lone element added to the Scene.
			 */
			Cursor(shared_ptr< Input<bool> > button,
						 shared_ptr< Input<f32> > xInput,
						 shared_ptr< Input<f32> > yInput,
						 shared_ptr<Element> rootElement,
						 shared_ptr<Renderer> renderer,
						 shared_ptr<Element> cursorElement = shared_ptr<Element>());
			
			/**
			 * OnStart() will prevent the task from starting if one of the required parameter is null.
			 * @return 
			 */
			virtual bool OnStart() override;
			
			/**
			 * Performs the scene ray calculation and updates the cursor position for the root element.
			 */
			void Update(Seconds) override;
			
			/**
			 * Returns the last known position of the cursor.
			 * If either of the X or Y inputs aren't valid this method will return ZERO.
			 */
			Vector3 GetPosition() const;
		private:
			shared_ptr<Element> mRootElement;
			shared_ptr<Element> mCursorElement;
			shared_ptr<Renderer> mRenderer;
			shared_ptr< Input<bool> > mButton;
			shared_ptr< Input<f32> > mX;
			shared_ptr< Input<f32> > mY;
			Vector3 mLastPosition;
		};
	}
}
#endif 
