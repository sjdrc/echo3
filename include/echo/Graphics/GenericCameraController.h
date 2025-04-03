#ifndef ECHO_GENERICCAMERACONTROLLER_H
#define ECHO_GENERICCAMERACONTROLLER_H

#include <echo/Kernel/TaskGroup.h>
#include <echo/Graphics/Node.h>
#include <echo/Graphics/Camera.h>

namespace Echo
{
	class GenericCameraController : public TaskGroup
	{
	public:
		GenericCameraController(shared_ptr<Camera> camera) : mCamera(camera),
			mMoveSpeed(1,1,1),
			mLookSpeedRadians(Degree(1).ValueRadians(),Degree(1).ValueRadians(),Degree(1).ValueRadians()),
			mOrbitSpeed(Degree(1).ValueRadians(),Degree(1).ValueRadians(),Degree(1).ValueRadians())
		{
			mLookatTarget.reset(new Node());
		}
		
		~GenericCameraController()
		{
		}
		
		void SetOrbitTarget(const Vector3& orbitTarget)
		{
			mLookatTarget->SetPosition(orbitTarget);
		}
		
		void OrbitX(const f32& speedMultiplier)
		{
			Vector3 orbitPoint = mLookatTarget->GetDerivedPosition();
			Vector3 cameraPosition = mCamera->GetDerivedPosition();
			Vector3 toCamera = cameraPosition - orbitPoint;
			Quaternion rotation(Radian(speedMultiplier*mOrbitSpeed.x*mLastFrameTime.count()), Radian(0), Radian(0));
			Vector3 newPosition = orbitPoint + (rotation * toCamera);
			mCamera->SetPosition(newPosition);
			mCamera->LookAt(orbitPoint);
		}
		
		void OrbitY(const f32& speedMultiplier)
		{
			Vector3 orbitPoint = mLookatTarget->GetDerivedPosition();
			Vector3 cameraPosition = mCamera->GetDerivedPosition();
			Vector3 toCamera = cameraPosition - orbitPoint;
			Quaternion rotation(Radian(0), Radian(speedMultiplier*mOrbitSpeed.y*mLastFrameTime.count()), Radian(0));
			Vector3 newPosition = orbitPoint + (rotation * toCamera);
			mCamera->SetPosition(newPosition);
			mCamera->LookAt(orbitPoint);
		}
		
		void Orbit(const Vector2& speedMultiplier, const Vector3& aroundPosition)
		{
			// I plan to add interpolation at some point so we need to make sure the look at point is updated.
			SetOrbitTarget(aroundPosition);
			
			Vector3 orbitPoint = mLookatTarget->GetDerivedPosition();
			Vector3 cameraPosition = mCamera->GetDerivedPosition();
			Vector3 toCamera = cameraPosition - orbitPoint;
			Quaternion rotation(Radian(speedMultiplier.x*mOrbitSpeed.x*mLastFrameTime.count()), Radian(speedMultiplier.y*mOrbitSpeed.y*mLastFrameTime.count()), Radian(0));
			Vector3 newPosition = orbitPoint + (rotation * toCamera);
			mCamera->SetPosition(newPosition);
			mCamera->LookAt(orbitPoint);
		}

		void MoveX(const f32& speedMultiplier)
		{
			mCamera->MoveRelative(Vector3(speedMultiplier * mMoveSpeed.x*mLastFrameTime.count(),0,0));
		}
		
		void MoveY(const f32& speedMultiplier)
		{
			mCamera->MoveRelative(Vector3(0,speedMultiplier * mMoveSpeed.y*mLastFrameTime.count(),0));
		}
		
		void MoveZ(const f32& speedMultiplier)
		{
			mCamera->MoveRelative(Vector3(0,0,speedMultiplier * mMoveSpeed.z*mLastFrameTime.count()));
		}
		
		void Move(const Vector3& speedMultiplier)
		{
			mCamera->MoveRelative(speedMultiplier * mMoveSpeed * mLastFrameTime.count());
		}

		void Yaw(const f32& speedMultiplier)
		{
			mCamera->Yaw(Radian(mLookSpeedRadians.y*speedMultiplier*mLastFrameTime.count()));
		}

		void Pitch(const f32& speedMultiplier)
		{
			mCamera->Pitch(Radian(mLookSpeedRadians.x*speedMultiplier*mLastFrameTime.count()));
		}

		void Roll(const f32& speedMultiplier)
		{
			mCamera->Roll(Radian(mLookSpeedRadians.z*speedMultiplier*mLastFrameTime.count()));
		}
		
		const Vector3& GetMoveSpeed() const 
		{
			return mMoveSpeed;
		}
		const Vector3& GetLookSpeed() const
		{
			return mLookSpeedRadians;
		}
		const Vector3& GetOrbitSpeed() const
		{
			return mOrbitSpeed;
		}
		
		void SetMoveSpeed(Vector3 moveSpeed)
		{
			mMoveSpeed = moveSpeed;
		}
		void SetLookSpeed(Vector3 lookSpeedRadians)
		{
			mLookSpeedRadians=lookSpeedRadians;
		}
		void SetOrbitSpeed(Vector3 orbitSpeed)
		{
			mOrbitSpeed = orbitSpeed;
		}

		void SetCamera(shared_ptr<Camera> camera)
		{
			mCamera = camera;
		}
		
		shared_ptr<Camera> GetCamera() const
		{
			return mCamera;
		}
		void Update(Seconds lastFrameTime)
		{
			mLastFrameTime = lastFrameTime;
		}
	private:
		Vector3 mMoveSpeed;
		Vector3 mLookSpeedRadians;
		Vector3 mOrbitSpeed;
		shared_ptr<Camera> mCamera;
		shared_ptr<Node> mLookatTarget;
		Seconds mLastFrameTime;
//		shared_ptr< InterpolatorTask<Vector3> > mPositionInterpolator;
//		void OnTargetInterpolationComplete();
//		void OnPositiontInterpolationComplete();
	};
}

#endif
