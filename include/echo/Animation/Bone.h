#ifndef _ECHOBONE_H_
#define _ECHOBONE_H_

#include <echo/Graphics/Node.h>

namespace Echo
{
	class Skeleton;

	class Bone : public Node
	{
	private:
		friend class Skeleton;
		Skeleton& mSkeleton;
		size_t mIndex;
		Vector3 mBindDerivedInversePosition;
		Vector3 mBindDerivedInverseScale;
		Quaternion mBindDerivedInverseOrientation;
		bool mManuallyControlled;
		Matrix4 mCachedOffsetTransform;
		Bone(Skeleton& skeleton, size_t index) : mSkeleton(skeleton), mIndex(index), mManuallyControlled(false)
		{
			mCachedOffsetTransform=Matrix4::IDENTITY;
		}
	public:
		~Bone()
		{
		}

		Skeleton& GetSkeleton() { return mSkeleton; }

		const size_t& GetIndex() const { return mIndex; }

		void SetInitialState()
		{
			mInitialOrientation=mOrientation;
			mInitialPosition=mPosition;
			mInitialScale=mScale;
		}

		void SetBindingPose()
		{
			// Save inverse derived position/scale/orientation, used for calculate offset transform later
			mBindDerivedInversePosition = - GetDerivedPosition();
			mBindDerivedInverseScale = Vector3(1,1,1) / GetDerivedScale();
			mBindDerivedInverseOrientation = GetDerivedOrientation().Inverse();
		}

		const Vector3& GetBindDerivedInversePosition() const { return mBindDerivedInversePosition; }
		const Vector3& GetBindDerivedInverseScale() const { return mBindDerivedInverseScale; }
		const Quaternion& GetBindDerivedInverseOrientation() const { return mBindDerivedInverseOrientation; }

		//Currently not used
		bool GetManuallyControlled() const { return mManuallyControlled; }
		void SetManuallyControlled(bool val) { mManuallyControlled = val; }

		const Matrix4& GetOffsetTransform()
		{
			if(mTransformOutOfDate)
			{
				// Combine scale with binding pose inverse scale,
				// NB just combine as equivalent axes, no shearing
				Vector3 scale = GetDerivedScale() * mBindDerivedInverseScale;

				// Combine orientation with binding pose inverse orientation
				Quaternion rotate = GetDerivedOrientation() * mBindDerivedInverseOrientation;

				// Combine position with binding pose inverse position,
				// Note that translation is relative to scale & rotation,
				// so first reverse transform original derived position to
				// binding pose bone space, and then transform to current
				// derived bone space.
				Vector3 translate = GetDerivedPosition() + rotate * (scale * mBindDerivedInversePosition);

				mCachedOffsetTransform.MakeTransform(translate, scale, rotate);
			}
			return mCachedOffsetTransform;
		}
	};
}
#endif 
