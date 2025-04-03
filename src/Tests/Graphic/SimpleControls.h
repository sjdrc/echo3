#ifndef ECHO_TESTING_SIMPLE_CONTROLS
#define ECHO_TESTING_SIMPLE_CONTROLS

#include <echo/Kernel/TaskGroup.h>
#include <echo/Graphics/Scene.h>
#include <echo/Graphics/MultiRenderer.h>
#include <echo/Graphics/Renderer.h>
#include <echo/Graphics/Camera.h>
#include <echo/Graphics/Viewport.h>
#include <echo/Application.h>
#include <echo/GUI/Screen.h>
#include <echo/GUI/Cursor.h>
#include <echo/Util/StringUtils.h>
#include "TrackPad.h"

namespace Echo
{
namespace Testing
{
	class SimpleControls : public TaskGroup
	{
	public:
		SimpleControls(shared_ptr<Camera> cameraToControl, Application& application)
		{
			shared_ptr<Scene> scene=make_shared<Scene>();
			shared_ptr<Camera> camera = scene->CreateCamera("GUI");
			camera->SetProjectionType(ProjectionTypes::ORTHOGRAPHIC);
			camera->SetOrthoWindowWidth(1);
			camera->SetPosition(0,0,100);
			shared_ptr<InputManager> inputManager = application.GetInputManager();

			shared_ptr<Renderer> renderer = application.GetRenderer()->CreateRenderer(make_shared<Viewport>(0,0,1,1),camera);
			renderer->SetCamera(camera);

			shared_ptr<GUI::Screen> screen(new GUI::Screen());
			scene->AddRenderable(screen);
			
			shared_ptr<TrackPad> leftInputMotion(new TrackPad());
			leftInputMotion->SetWidth(1.f);
			leftInputMotion->SetHeight(1.f);
			leftInputMotion->SetRenderAABB(true);
			scene->AddRenderable(leftInputMotion);
			std::string thisString = Utils::String::ToString(reinterpret_cast<Size>(this)).value();
			inputManager->InstallDevice("ScreenMotionLeft-0x"+thisString,leftInputMotion);

			leftInputMotion->AddChangeCallback<Vector2>("Move", [cameraToControl](const Vector2& motion)
			{
				cameraToControl->Yaw(Degree(motion.x * 180));
				cameraToControl->Pitch(Degree(-motion.y * 90));
				//ECHO_LOG_ERROR("Rotating: " << motion);
			});

			shared_ptr<TrackPad> rightInputMotion(new TrackPad());
			rightInputMotion->SetWidth(1.f);
			rightInputMotion->SetHeight(1.f);
			rightInputMotion->SetRenderAABB(true);
			scene->AddRenderable(rightInputMotion);
			inputManager->InstallDevice("ScreenMotionRight-0x"+thisString,rightInputMotion);

			rightInputMotion->AddChangeCallback<Vector2>("Move", [cameraToControl](const Vector2& motion)
			{
				cameraToControl->MoveRelative(-Vector3(motion.x*5,motion.y*5,0));
			});

			shared_ptr<TrackPad> middleInputMotion(new TrackPad());
			middleInputMotion->SetWidth(1.f);
			middleInputMotion->SetHeight(1.f);
			middleInputMotion->SetRenderAABB(true);
			scene->AddRenderable(middleInputMotion);
			inputManager->InstallDevice("ScreenMotionMiddle-0x"+thisString,middleInputMotion);

			middleInputMotion->AddChangeCallback<f32>("Move.Y", [cameraToControl](const f32& motion)
			{
				cameraToControl->MoveRelative(-Vector3(0,0,motion*5));
			});

			shared_ptr<GUI::Cursor> leftInput(new GUI::Cursor(	inputManager->GetInput<bool>("Mouse:Left"),
										inputManager->GetInput<f32>("Mouse:X"),
										inputManager->GetInput<f32>("Mouse:Y"),
										leftInputMotion,renderer,nullptr));
			AddTask(leftInput);

			shared_ptr<GUI::Cursor> rightInput(new GUI::Cursor(	inputManager->GetInput<bool>("Mouse:Right"),
										inputManager->GetInput<f32>("Mouse:X"),
										inputManager->GetInput<f32>("Mouse:Y"),
										rightInputMotion,renderer,nullptr));

			AddTask(rightInput);
			shared_ptr<GUI::Cursor> middleInput(new GUI::Cursor(	inputManager->GetInput<bool>("Mouse:Middle"),
										inputManager->GetInput<f32>("Mouse:X"),
										inputManager->GetInput<f32>("Mouse:Y"),
										middleInputMotion,renderer,nullptr));

			AddTask(middleInput);
			AddTask(scene);
		}
	};
}
}
#endif
