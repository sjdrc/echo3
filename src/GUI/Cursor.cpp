#include <echo/GUI/Element.h>
#include <echo/GUI/Cursor.h>
#include <echo/Graphics/Renderer.h>
#include <echo/Maths/Ray.h>

namespace Echo
{
	namespace GUI
	{
		Cursor::Cursor(shared_ptr< Input<bool> > button,
					 shared_ptr< Input<f32> > xInput,
					 shared_ptr< Input<f32> > yInput,
					 shared_ptr<Element> rootElement,
					 shared_ptr<Renderer> renderer,
					 shared_ptr<Element> cursorElement) :
			mRootElement(rootElement),
			mCursorElement(cursorElement),
			mRenderer(renderer),
			mButton(button),
			mX(xInput),
			mY(yInput),
			mLastPosition(0,0,0)
		{
			if(mCursorElement)
			{
				// Turn off targetting of this element because it should never be targetable
				// otherwise it will receive click events and nothing else will.
				mCursorElement->SetTargetable(false);
			}
		}

		bool Cursor::OnStart()
		{
			//Prevent starting this task if we have any null required parameters.
			return (mRenderer && mRootElement && mButton && mX && mY);
		}

		void Cursor::Update(Seconds)
		{
			optional<Ray> rayIntoScene = mRenderer->GetRayIntoScene(*mX,*mY);
			if(rayIntoScene)
			{
				const Ray& ray = rayIntoScene.value();
				Plane plane(Vector3::UNIT_Z,0.f);
				std::pair<bool, f32> intersectResult = ray.Intersects(plane);
				if(intersectResult.first)
				{
					mLastPosition = ray.GetPoint(intersectResult.second);
					if(mCursorElement && mRenderer->GetRenderTarget())
					{
						mCursorElement->SetRelativePosition(Vector3(*mX,1.f-*mY,mCursorElement->GetRelativePosition().z));
					}
					mRootElement->UpdateCursor(mLastPosition.x,mLastPosition.y,*mButton);
				}
			}
		}

		Vector3 Cursor::GetPosition() const
		{
			return mLastPosition;
		}
	}
}
