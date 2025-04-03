#include <ThirdPersonCameraController.h>
#include <echo/Maths/EchoMaths.h>
#include <echo/Graphics/Camera.h>

namespace MyProject
{
	ThirdPersonCameraController::ThirdPersonCameraController(shared_ptr<Camera> camera, shared_ptr<NodeInterface> target, f32 distance) : Task("ThirdPersonCameraController")
	{
		mCamera=camera;
		mTarget=target;

		mDistance=distance;
		mSpeed=1.0f;
		mMaxHeight=2.0f;
		mYOffset=1;
		mMode=MoveModes::SET;
		mPositions[0]=Vector3::ZERO;
		mPositions[1]=Vector3::ZERO;
		mPositions[2]=Vector3::ZERO;
		mPositions[3]=Vector3::ZERO;
		mTargetPos=Vector3::ZERO;
		mPositionsPointers[0]=&(mPositions[0]);
		mPositionsPointers[1]=&(mPositions[1]);
		mPositionsPointers[2]=&(mPositions[2]);
		mPositionsPointers[3]=&(mPositions[3]);
		mCurrentPosition=0;
		mTimerPercent=0;
		
		//Lets set up an initial position for the camera based on the target orientation.
		camera->SetPosition(target->GetPosition()+target->GetOrientation().ZAxis());
	}

	ThirdPersonCameraController::~ThirdPersonCameraController()
	{
	}

	void ThirdPersonCameraController::Update(Seconds lastFrameTime)
	{
		if(mCamera && mTarget)
		{
			Vector3 targetPos=mTarget->GetPosition();

			switch(mMode)
			{
				case MoveModes::HERMITE:
				{
					mTimerPercent+=lastFrameTime.count()*mSpeed;
					if(mTimerPercent>=1.0f)
					{
						mTimerPercent=1.0f;
					}

					Vector3 finalPos=Maths::HermiteInterpolate(*(mPositionsPointers[0]),*(mPositionsPointers[1]),*(mPositionsPointers[2]),*(mPositionsPointers[3]),mTimerPercent,0.0f,0.0f);

					if(mTimerPercent>=1.0f)
					{
						Vector3 currentPos=mCamera->GetPosition();

						Vector3 posDiff;
						posDiff=currentPos;

						posDiff=posDiff-targetPos;
						if(posDiff.y>mMaxHeight)
						{
							posDiff.y=mMaxHeight;
						}else
						{
							if(posDiff.y<-mMaxHeight)
							{
								posDiff.y=-mMaxHeight;
							}
						}

						posDiff.Normalise();
						posDiff*=mDistance;

						posDiff.x+=targetPos.x;
						posDiff.y+=targetPos.y+mYOffset;
						posDiff.z+=targetPos.z;

						mPositions[mCurrentPosition]=posDiff;
						mPositionsPointers[3]=mPositionsPointers[2];
						mPositionsPointers[2]=mPositionsPointers[1];
						mPositionsPointers[1]=mPositionsPointers[0];
						mPositionsPointers[0]=&(mPositions[mCurrentPosition]);
						mCurrentPosition++;
						if(mCurrentPosition>=4)
						{
							mCurrentPosition=0;
						}

						mTimerPercent=0;
					}

					mCamera->SetPosition(finalPos);

				}break;
				case MoveModes::PERCENT:
				{
					Vector3 currentPos;
					currentPos=mCamera->GetPosition();

					Vector3 posDiff;
					posDiff=currentPos;

					posDiff=posDiff-targetPos;
					if(posDiff.y>mMaxHeight)
					{
						posDiff.y=mMaxHeight;
					}else
					{
						if(posDiff.y<-mMaxHeight)
						{
							posDiff.y=-mMaxHeight;
						}
					}

					posDiff.Normalise();
					posDiff*=mDistance;

					f32 speedByDelta=mSpeed*lastFrameTime.count();
					if(speedByDelta>1.0f)
					{
						speedByDelta=1.0f;
					}

					currentPos.x=(currentPos.x*(1-speedByDelta)) + ((targetPos.x+posDiff.x)*speedByDelta);
					currentPos.y=(currentPos.y*(1-speedByDelta)) + ((targetPos.y+posDiff.y+mYOffset)*speedByDelta);
					currentPos.z=(currentPos.z*(1-speedByDelta)) + ((targetPos.z+posDiff.z)*speedByDelta);

					posDiff=targetPos-mTargetPos;
					targetPos=(mTargetPos*(1-speedByDelta)) + ((targetPos+posDiff)*speedByDelta);

					mCamera->SetPosition(currentPos);
				}break;
				case MoveModes::SET:
				{
					Vector3 currentPos;
					currentPos=mCamera->GetPosition();

					Vector3 posDiff;
					posDiff=currentPos;

					posDiff=posDiff-targetPos;
					posDiff.y-=mYOffset;
					if(posDiff.y>mMaxHeight)
						posDiff.y=mMaxHeight;
					else
						if(posDiff.y<-mMaxHeight)
							posDiff.y=-mMaxHeight;

					posDiff.Normalise();
					posDiff*=mDistance;

					currentPos.x=(targetPos.x+posDiff.x);
					currentPos.y=(targetPos.y+posDiff.y+mYOffset);
					currentPos.z=(targetPos.z+posDiff.z);

					mCamera->SetPosition(currentPos);
				}break;
			}
			mTargetPos=targetPos;
			mCamera->LookAt(targetPos);
		}
	}
}
