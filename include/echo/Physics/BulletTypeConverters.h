#ifndef _ECHOBULLETTYPECONVERTERS_H_
#define _ECHOBULLETTYPECONVERTERS_H_

#include <echo/Types.h>
#include <echo/Maths/Vector3.h>
#include <LinearMath/btQuaternion.h>
#include <LinearMath/btVector3.h>

namespace Echo
{
	inline btVector3 Convert(const Vector3& from)
	{
		return btVector3(from.x,from.y,from.z);
	}
	inline Vector3 Convert(const btVector3& from)
	{
		return Vector3(from.getX(),from.getY(),from.getZ());
	}

	inline btQuaternion Convert(const Quaternion& from)
	{
		return btQuaternion(from.x,from.y,from.z,from.w);
	}
	
	inline Quaternion Convert(const btQuaternion& from)
	{
		return Quaternion(from.getX(),from.getY(),from.getZ(),from.getW());
	}
}
#endif
