#ifndef THIRDPERSONCAMERACONTROLLER_H
#define THIRDPERSONCAMERACONTROLLER_H

#include <echo/Kernel/Task.h>
#include <echo/Maths/Vector3.h>
#include <echo/Graphics/NodeInterface.h>

using namespace Echo;

namespace Echo
{
	class NodeInterface;
	class Camera;
}

namespace MyProject
{
	class ThirdPersonCameraController : public Task
	{
	public:
		struct MoveModes
		{
			enum _
			{
				HERMITE,	//Perform Hermite interpolation between updates
				PERCENT,	//Perform percent advance between updates.
				SET			//Always use the most up to date position (not smooth)
			}; 
		};
		typedef MoveModes::_ MoveMode;
		ThirdPersonCameraController(shared_ptr<Camera> camera, shared_ptr<NodeInterface> target = nullptr, Scalar distance = 10.0f);
		~ThirdPersonCameraController();

		//Camera Settings

		void SetDistance(const Scalar& distance)
		{
			mDistance = distance;
		}

		const Scalar& GetDistance() const
		{
			return mDistance;
		}

		void SetYOffset(const Scalar& offset)
		{
			mYOffset = offset;
		}

		const Scalar& GetYOffset() const
		{
			return mYOffset;
		}

		const Scalar& GetMaxHeight() const
		{
			return mMaxHeight;
		}

		void SetMaxHeight(const Scalar& val)
		{
			mMaxHeight = val;
		}

		void SetCamera(shared_ptr<Camera> camera)
		{
			mCamera = camera;
		}

		shared_ptr<Camera> GetCamera() const
		{
			return mCamera;
		}

		void SetTarget(shared_ptr<NodeInterface> target)
		{
			mTarget = target;
		}

		shared_ptr<NodeInterface> GetTarget() const
		{
			return mTarget;
		}

		void SetSpeed(const Scalar& speed)
		{
			mSpeed = speed;
		}

		const Scalar& GetSpeed() const
		{
			return mSpeed;
		}

		void SetMode(MoveMode mode)
		{
			mMode = mode;
		}

		MoveMode GetMoveMode() const
		{
			return mMode;
		}

		virtual void Update(Seconds lastFrameTime);
	private:
		MoveMode mMode; //The camera Move Mode
		shared_ptr<NodeInterface> mTarget; //The camera Target

		Scalar mDistance; //Distance to stay from target, the default is 5 units
		Scalar mMaxHeight;	//Max distance on the Y
		Scalar mYOffset;	//Most 3rd person cameras sit a little higher than the
							//target's y. This is the offset.
		Scalar mSpeed; //The speed the camera interpolates to the next position

		shared_ptr<Camera> mCamera; //Camera to control

		Vector3 mTargetPos;

		//MODE_HERMITE specific
		Vector3 mPositions[4];
		Vector3* mPositionsPointers[4];		//Used for easy swapping
		f32 mTimerPercent;
		u32 mCurrentPosition;
	};
}

#endif
