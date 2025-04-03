#ifndef ECHO_ORBIT_CAMERA_CONTROLLER
#define ECHO_ORBIT_CAMERA_CONTROLLER

#include <echo/Kernel/Task.h>
#include <echo/Graphics/Scene.h>
#include <echo/Graphics/Renderer.h>
#include <echo/Graphics/Camera.h>
#include <echo/Input/Input.h>

namespace Echo
{
	class OrbitCameraController : public Task
	{
	public:
		struct States
		{
			enum _
			{
				IDLE = 0,
				ORBITING,
				PANNING,
				ZOOMING
			};
		};
		typedef States::_ State;

		/**
		 * An "orbit" camera controller.
		 * This controller provides three functions:
		 *  - Orbit around a point
		 *  - Pan on XZ plane with respect to a point
		 *  - Zoom to/from a point
		 * The name "Orbit" comes from one of the functions and from discussions around describing this camera controller.
		 * The controller will
		 */
		OrbitCameraController(shared_ptr<Camera> camera, shared_ptr<Renderer> renderer, shared_ptr<SceneEntity> targetWidget, shared_ptr<InputManager> inputManager,
			std::string inputDevice = "Mouse",
			std::string orbitButtonName = "Middle",
			std::string panButtonName = "Left",
			std::string zoomButtonName = "Right",
			std::string zoomAxisName = "Z",
			std::string xAxisName = "X",
			std::string yAxisName = "Y") :
			mCamera(camera),
			mRenderer(renderer),
			mTargetWidget(targetWidget),
			mState(States::IDLE),
			mLastUpdatePosition(Vector2::ZERO),
			mLastUpdateZPosition(0),
			mZoomDeltaStep(0.1)
		{
			if(mTargetWidget)
			{
				mTargetWidget->SetVisibilityFlag(false);
			}
			shared_ptr<InputDevice> device = inputManager->GetDevice(inputDevice);
			if(device)
			{
				device->AddChangeCallback<bool>(orbitButtonName, [this](const bool& pressed)
				{
					OrbitButtonInputChange(pressed);
				});
				device->AddChangeCallback<bool>(panButtonName, [this](const bool& pressed)
				{
					PanButtonInputChange(pressed);
				});
				device->AddChangeCallback<bool>(zoomButtonName, [this](const bool& pressed)
				{
					ZoomButtonInputChange(pressed);
				});
				device->AddChangeCallback<f32>(zoomAxisName, [this](const f32& value)
				{
					f32 delta = value - mLastUpdateZPosition;
					mLastUpdateZPosition = value;
					ZoomStep(delta > 0 ? mZoomDeltaStep : -mZoomDeltaStep);
				});

				mPositionX = device->GetInput<f32>(xAxisName);
				mPositionY = device->GetInput<f32>(yAxisName);

				if(!mPositionX || !mPositionY)
				{
					ECHO_LOG_ERROR("Unable to acquire inputs " << xAxisName << " and " << yAxisName);
				}
			}
		}

		/**
		 * If an object isn't picked on click and this plane is set then it will be used to calculate the target point.
		 */
		void SetFallbackTargetPlane(optional<Plane> plane)
		{
			mFallbackTargetPlane = plane;
		}

		/**
		 * How much each zoom step will move by. 0.1 would be the same as 0.1 of the Y axis when zooming.
		 */
		void SetZoomDeltaStep(f32 zoomDeltaStep)
		{
			mZoomDeltaStep = zoomDeltaStep;
		}

		void OrbitButtonInputChange(bool pressed)
		{
			if(!pressed && mState==States::ORBITING)
			{
				mCurrentTarget = none;
				mState=States::IDLE;
				if(mTargetWidget)
				{
					mTargetWidget->SetVisibilityFlag(false);
				}
			}else
			{
				if(!mCurrentTarget)
				{
					SetTarget(States::ORBITING);
				}
			}
		}

		void PanButtonInputChange(bool pressed)
		{
			if(!pressed && mState==States::PANNING)
			{
				mCurrentTarget = none;
				mState=States::IDLE;
				if(mTargetWidget)
				{
					mTargetWidget->SetVisibilityFlag(false);
				}
			}else
			{
				if(!mCurrentTarget)
				{
					SetTarget(States::PANNING);
				}
			}
		}

		void ZoomStep(f32 delta)
		{
			if(!mPositionX || !mPositionY)
			{
				return;
			}
			f32 x = (*mPositionX);
			f32 y = (*mPositionY);
			optional<PickResult> target = mRenderer->Pick(x,y);
			if(target)
			{
				UpdateZoom(delta,target.value());
				return;
			}

			if(mFallbackTargetPlane)
			{
				optional<Ray> cameraToPlaneRay = mRenderer->GetRayIntoScene(x,y);
				if(!cameraToPlaneRay)
				{
					return;
				}

				std::pair<bool, f32> intersection = cameraToPlaneRay.value().Intersects(mFallbackTargetPlane.value());
				if(intersection.first)
				{
					PickResult result;
					result.mSceneRenderable = nullptr;
					result.mDistance = intersection.second;
					result.mRay = cameraToPlaneRay.value();
					result.mIntersectPosition = result.mRay.GetPoint(result.mDistance);
					UpdateZoom(delta,result);
					return;
				}
			}
		}

		void ZoomButtonInputChange(bool pressed)
		{
			if(!pressed && mState==States::ZOOMING)
			{
				mCurrentTarget = none;
				mState=States::IDLE;
				if(mTargetWidget)
				{
					mTargetWidget->SetVisibilityFlag(false);
				}
			}else
			{
				if(!mCurrentTarget)
				{
					SetTarget(States::ZOOMING);
				}
			}
		}

		void Update(Seconds)
		{
			if(!mPositionX || !mPositionY)
			{
				return;
			}

			Vector2 position(*mPositionX,*mPositionY);

			Vector2 relativeMotion = (position-mLastUpdatePosition);
			mLastUpdatePosition = position;

			if(!mCurrentTarget)
			{
				return;
			}
			switch(mState)
			{
				case States::IDLE:
				break;
				case States::ORBITING:
					UpdateOrbit(relativeMotion, mCurrentTarget.value());
				break;
				case States::PANNING:
					UpdatePan(relativeMotion, mCurrentTarget.value());
				break;
				case States::ZOOMING:
					UpdateZoom(relativeMotion.y, mCurrentTarget.value());
				break;
			}
		}
	private:
		void SetTarget(State stateToChangeIfPicked)
		{
			if(!mPositionX || !mPositionY)
			{
				return;
			}

			f32 x = (*mPositionX);
			f32 y = (*mPositionY);
			optional<PickResult> target = mRenderer->Pick(x,y);
			if(target)
			{
				mCurrentTarget = target;
				mLastUpdatePosition.x = x;
				mLastUpdatePosition.y = y;
				mState = stateToChangeIfPicked;
				if(mTargetWidget)
				{
					mTargetWidget->SetPosition(mCurrentTarget.value().mIntersectPosition);
					mTargetWidget->SetVisibilityFlag(true);
				}
				return;
			}

			// Attempt the fallback plane
			if(mFallbackTargetPlane)
			{
				optional<Ray> cameraToPlaneRay = mRenderer->GetRayIntoScene(x,y);
				if(!cameraToPlaneRay)
				{
					return;
				}

				std::pair<bool, f32> intersection = cameraToPlaneRay.value().Intersects(mFallbackTargetPlane.value());
				if(intersection.first)
				{
					mLastUpdatePosition.x = x;
					mLastUpdatePosition.y = y;
					PickResult result;
					result.mSceneRenderable = nullptr;
					result.mDistance = intersection.second;
					result.mRay = cameraToPlaneRay.value();
					result.mIntersectPosition = result.mRay.GetPoint(result.mDistance);
					mCurrentTarget = result;
					mState = stateToChangeIfPicked;
					if(mTargetWidget)
					{
						mTargetWidget->SetPosition(mCurrentTarget.value().mIntersectPosition);
						mTargetWidget->SetVisibilityFlag(true);
					}
					return;
				}
			}

			mCurrentTarget = none;
			if(mTargetWidget)
			{
				mTargetWidget->SetVisibilityFlag(false);
			}
		}

		void UpdateOrbit(const Vector2& delta, const PickResult& target)
		{
			// The current target will be the pivot points
			const Vector3& orbitPoint = target.mIntersectPosition;

			// Roate Left/Right
			{
				Vector3 cameraPosition = mCamera->GetPosition();
				Vector3 orbitToCamera = cameraPosition - orbitPoint;

				f32 theta = delta.x * Maths::TWO_PI;

				Matrix3 rotation;
				//rotation.FromEulerAnglesXYZ(Radian(0),Radian(-thetaX),Radian(0));
				rotation.FromAxisAngle(Vector3(0,1,0),Radian(-theta));
				
				cameraPosition = (rotation * orbitToCamera) + orbitPoint;
				mCamera->SetPosition(cameraPosition);

				mCamera->Yaw(Radian(-theta));
			}

			// Rotate up/down - this is with respect to the X axis of the camera
			{
				Vector3 cameraPosition = mCamera->GetPosition();
				Vector3 orbitToCamera = cameraPosition - orbitPoint;

				f32 theta = delta.y * Maths::PI;

				Matrix3 rotation;
				rotation.FromAxisAngle(mCamera->GetOrientation().XAxis(),Radian(-theta));
				
				cameraPosition = (rotation * orbitToCamera) + orbitPoint;
				mCamera->SetPosition(cameraPosition);
				mCamera->Pitch(Radian(-theta));
			}
		}

		void UpdatePan(const Vector2& delta, const PickResult& target)
		{
			f32 x = (*mPositionX);
			f32 y = (*mPositionY);
			optional<Ray> cameraToPlaneRay = mRenderer->GetRayIntoScene(x,y);
			if(!cameraToPlaneRay)
			{
				return;
			}

			// We move on an XZ planes
			Plane floor(Vector3(0,1,0),target.mIntersectPosition.y);
			std::pair<bool, f32> intersection = cameraToPlaneRay.value().Intersects(floor);
			if(intersection.first)
			{
				Vector3 newPosition = cameraToPlaneRay.value() * intersection.second;
				Vector3 moveAmount = mCurrentTarget.value().mIntersectPosition - newPosition;
				mCamera->Move(moveAmount);
			}
		}

		void UpdateZoom(const f32& delta, const PickResult& target)
		{
			// We need to move the camera along the ray to the target
			Vector3 moveAmount = target.mRay.GetDirection();
			moveAmount *= -(target.mDistance * delta);
			mCamera->Move(moveAmount);
		}
		
		bool mIntersecting;
		shared_ptr< Input<f32> > mPositionX;
		shared_ptr< Input<f32> > mPositionY;
		Vector2 mLastUpdatePosition;
		f32 mLastUpdateZPosition;
		f32 mZoomDeltaStep;

		shared_ptr<Camera> mCamera;
		shared_ptr<Renderer> mRenderer;
		shared_ptr<SceneEntity> mTargetWidget;
		State mState;
		optional<PickResult> mCurrentTarget;
		optional<Vector3> mMoveStartPosition;
		optional<Plane> mFallbackTargetPlane;
	};
}
#endif
