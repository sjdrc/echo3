#ifndef ECHO_SCENEPICKER_H
#define ECHO_SCENEPICKER_H

#include <echo/Kernel/Task.h>
#include <echo/Input/Input.h>
#include <echo/GUI/Container.h>

namespace Echo
{
	class SceneRenderable;
	class Camera;
	class Renderer;
	class Scene;
	class PickResult;
	
	/**
	 * A Scene Picker will call a callback when something is picked in the scene.
	 * Picking is done when the button is pressed but only having previously been released.
	 * State checks are done in Update() rather than on input events.
	 */
	class ScenePicker : public Task
	{
	public:
		typedef function< void(optional<PickResult>)> SceneRenderablePickedCallback;
		ScenePicker(shared_ptr< Input<bool> > button,
					shared_ptr< Input<f32> > xInput,
					shared_ptr< Input<f32> > yInput,
					shared_ptr<Camera> camera,
					shared_ptr<Renderer> renderer,
					Scene& scene,
					SceneRenderablePickedCallback pickedCallback);
		void Update(Seconds);
		void Pick();
	private:
		shared_ptr< Input<bool> > mButton;
		shared_ptr< Input<f32> > mX;
		shared_ptr< Input<f32> > mY;
		bool mButtonPressed;
		Scene& mScene;
		shared_ptr<Camera> mCamera;
		shared_ptr<Renderer> mRenderer;
		SceneRenderablePickedCallback mPickedCallback;
	};
	
	/**
	 * A specialisation of ScenePicker that can be added to a GUI and be updated
	 * when the cursor is pressed. This allows scene picking to only be done if the
	 * other higher GUI Elements aren't pressed first.
	 */
	class ScenePickerGuard : public GUI::Container, public ScenePicker
	{
	public:
		typedef function< void(optional<PickResult>)> SceneRenderablePickedCallback;
		ScenePickerGuard(shared_ptr< Input<bool> > button,
					shared_ptr< Input<f32> > xInput,
					shared_ptr< Input<f32> > yInput,
					shared_ptr<Camera> camera,
					shared_ptr<Renderer> renderer,
					Scene& scene,
					SceneRenderablePickedCallback pickedCallback);
	private:
		virtual void OnCursorPress(Scalar absoluteX,Scalar absoluteY) override final;
		
	};
}

#endif
