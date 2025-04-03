#ifndef ECHO_TESTING_TRACKPAD
#define ECHO_TESTING_TRACKPAD
#include <echo/Input/InputDevice.h>
#include <echo/GUI/Container.h>

namespace Echo
{
namespace Testing
{
	/**
	 * A simple GUI element input device that provides Position and Move inputs.
	 * Any elements added to this container should not be targetable.
	 */
	class TrackPad : public InputDevice, public GUI::Container
	{
	public:
		struct TouchStates
		{
			enum _
			{
				IDLE,
				HELD
			};
		};
		typedef TouchStates::_ TouchState;
		
		TrackPad() : InputDevice("TrackPad"),
			mTouchState(TouchStates::IDLE)
		{
			//Containers are not targetable by default.
			SetTargetable(true);
			RegisterInput();
		}

		/**
		 * We are going to intercept slightly lower level cursor update events because
		 * the OnCursor methods don't allow us to track once the cursor has left.
		 */
		void UpdateCursor(Scalar absoluteX, Scalar absoluteY, bool pressed, bool intersected)
		{
			if(mTouchState==TouchStates::IDLE)
			{
				if(intersected && pressed)
				{
					mTouchState=TouchStates::HELD;
					mLastUpdatePosition.x = absoluteX;
					mLastUpdatePosition.y = absoluteY;
				}
			}
			if(mTouchState==TouchStates::HELD)
			{
				Vector2 position(absoluteX,absoluteY);
				Vector3 myPosition=GetAbsolutePosition();
				Vector2 relativePosition(position.x-myPosition.x, position.y-myPosition.y);

				*mPosition = relativePosition;
				*mRelativeMotion = (position-mLastUpdatePosition);
				mLastUpdatePosition = position;

				if(!pressed)
				{
					mTouchState=TouchStates::IDLE;
				}
			}
			GUI::Element::UpdateCursor(absoluteX,absoluteY,pressed,intersected);
		}
	private:
		void RegisterInput()
		{
			mPosition=make_shared<Vector2>(0,0);
			AddInput<Vector2>("Position",mPosition,Vector2::ZERO);
			AddRawInput<f32,Vector2>("Position.X",mPosition,&mPosition->x,0.f);
			AddRawInput<f32,Vector2>("Position.Y",mPosition,&mPosition->y,0.f);
			mRelativeMotion=make_shared<Vector2>(0,0);
			AddInput<Vector2>("Move",mRelativeMotion,Vector2::ZERO);
			AddRawInput<f32,Vector2>("Move.X",mRelativeMotion,&mRelativeMotion->x,0.f);
			AddRawInput<f32,Vector2>("Move.Y",mRelativeMotion,&mRelativeMotion->y,0.f);
		}
		
		TouchState mTouchState;
		shared_ptr<Vector2> mPosition;
		shared_ptr<Vector2> mRelativeMotion;
		Vector2 mLastUpdatePosition;
	};
}
}

#endif
