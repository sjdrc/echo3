#include <echo/Util/ScenePicker.h>
#include <echo/Graphics/Camera.h>
#include <echo/Graphics/Renderer.h>
#include <echo/Graphics/Scene.h>
#include <echo/Graphics/PickResult.h>

namespace Echo
{
	ScenePicker::ScenePicker(shared_ptr< Input<bool> > button,
				shared_ptr< Input<f32> > xInput,
				shared_ptr< Input<f32> > yInput,
				shared_ptr<Camera> camera,
				shared_ptr<Renderer> renderer,
				Scene& scene,
				SceneRenderablePickedCallback pickedCallback) :
		mButton(button),
		mX(xInput),
		mY(yInput),
		mButtonPressed(false),
		mScene(scene),
		mCamera(camera),
		mRenderer(renderer),
		mPickedCallback(pickedCallback)
	{
	}

	void ScenePicker::Update(Seconds)
	{
		if(*mButton && !mButtonPressed)
		{
			Pick();
		}

		mButtonPressed = *mButton;
	}
	
	void ScenePicker::Pick()
	{
		optional<Ray> rayIntoScene = mRenderer->GetRayIntoScene(*mX,*mY);
		if(rayIntoScene)
		{
			optional<PickResult> pickResult = mScene.Pick(*mCamera,rayIntoScene.value());
			mPickedCallback(pickResult);
		}
	}
	
	ScenePickerGuard::ScenePickerGuard(shared_ptr< Input<bool> > button,
				shared_ptr< Input<f32> > xInput,
				shared_ptr< Input<f32> > yInput,
				shared_ptr<Camera> camera,
				shared_ptr<Renderer> renderer,
				Scene& scene,
				SceneRenderablePickedCallback pickedCallback)
		: ScenePicker(button,xInput,yInput,camera,renderer,scene,pickedCallback)
	{
		SetTargetable(true);
	}
	
	void ScenePickerGuard::OnCursorPress(Scalar absoluteX,Scalar absoluteY)
	{
		Pick();
	}
}
