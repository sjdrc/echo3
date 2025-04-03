#ifndef _ECHOBONEKEYFRAME_H_
#define _ECHOBONEKEYFRAME_H_

#include <echo/Animation/KeyFrame.h>
#include <echo/Maths/Vector3.h>
#include <echo/Maths/Quaternion.h>

namespace Echo
{
	class BoneKeyFrame : public KeyFrame
	{
	public:
		BoneKeyFrame() :	mPosition(Vector3::ZERO),
							mOrientation(Quaternion::IDENTITY),
							mScale(1.0f,1.0f,1.0f)
		{}
		Vector3 mPosition;
		Quaternion mOrientation;
		Vector3 mScale;
	};
}
#endif 
