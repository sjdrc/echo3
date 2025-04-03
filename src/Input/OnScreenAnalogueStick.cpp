#include <echo/Input/OnScreenAnalogueStick.h>
#include <echo/GUI/Image.h>

namespace Echo
{
	OnScreenAnalogueStick::OnScreenAnalogueStick(shared_ptr<Material> backgroundMaterial, shared_ptr<Material> stickMaterial) :
		InputDevice("OnScreenAnalogueStick"),
		mStickState(StickStates::IDLE)
	{
		//Containers are not targetable by default.
		SetTargetable(true);

		RegisterInput();
		
		mBackgroundElement.reset(new GUI::Image(backgroundMaterial));
		mBackgroundElement->SetTargetable(false);
		mBackgroundElement->SetRelativePosition(0.5,0.5,0);
		AddElement(mBackgroundElement);

		mStickElement.reset(new GUI::Image(stickMaterial));
		mStickElement->SetTargetable(false);
		mStickElement->SetRelativePosition(0.5,0.5,0.5);
		mStickElement->SetSize(0.3,0.3);
		AddElement(mStickElement);
	}
	
	OnScreenAnalogueStick::OnScreenAnalogueStick(shared_ptr<GUI::Element> backgroundElement, shared_ptr<GUI::Element> stickElement) :
		InputDevice("OnScreenAnalogueStick"),
		mStickState(StickStates::IDLE)
	{
		//Containers are not targetable by default.
		SetTargetable(true);
		
		RegisterInput();

		mBackgroundElement = backgroundElement;
		assert(backgroundElement && "backgroundElement should not be null");
		mBackgroundElement->SetTargetable(false);
		mBackgroundElement->SetRelativePosition(0.5,0.5,0);
		AddElement(mBackgroundElement);

		mStickElement = stickElement;
		assert(stickElement && "stickElement should not be null");
		mStickElement->SetTargetable(false);
		mStickElement->SetRelativePosition(0.5,0.5,0.5);
		AddElement(mStickElement);
	}
	
	OnScreenAnalogueStick::~OnScreenAnalogueStick()
	{
	}

	void OnScreenAnalogueStick::RegisterInput()
	{
		mStick=make_shared<Vector2>(0,0);
		AddInput<Vector2>("Stick",mStick,Vector2::ZERO);
		AddRawInput<f32,Vector2>("X",mStick,&mStick->x,0.f);
		AddRawInput<f32,Vector2>("Y",mStick,&mStick->y,0.f);
	}

	void OnScreenAnalogueStick::UpdateCursor(Scalar absoluteX, Scalar absoluteY, bool pressed, bool intersected)
	{
		if(mStickState==StickStates::IDLE)
		{
			if(intersected && pressed)
			{
				mStickState=StickStates::HELD;
			}
		}
		if(mStickState==StickStates::HELD)
		{
			Vector2 position(absoluteX,absoluteY);
			Vector3 myPosition=GetAbsolutePosition();
			Vector2 relativePosition(position.x-myPosition.x, position.y-myPosition.y);

			Vector2 halfSize(GetAbsoluteWidth() * 0.5f,GetAbsoluteHeight() * 0.5f);

			relativePosition.x /= halfSize.x;
			relativePosition.y /= halfSize.y;
			if(relativePosition.SquaredLength()>1.f)
			{
				relativePosition.Normalise();
			}
			*mStick = relativePosition;

			// We're dealing with Elements here which means the stick position
			// is relative to the container. 0.5,0.5 is centre.
			Vector2 onScreenStickPosition=(relativePosition*0.5f)+Vector2(0.5,0.5);
			mStickElement->SetRelativePosition(onScreenStickPosition.x,onScreenStickPosition.y,0.5f);
			if(!pressed)
			{
				mStickState=StickStates::IDLE;
				*mStick=Vector2::ZERO;
				mStickElement->SetRelativePosition(0.5f,0.5f,0.5f);
			}
		}
		GUI::Element::UpdateCursor(absoluteX,absoluteY,pressed,intersected);
	}
}
