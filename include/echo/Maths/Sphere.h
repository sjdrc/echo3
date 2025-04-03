#ifndef _ECHO_SPHERE_H_
#define _ECHO_SPHERE_H_

#include <echo/Maths/Vector3.h>

namespace Echo
{
	/** \addtogroup Core
	 *  @{
	 */
	/** \addtogroup Maths
	*  @{
	*/
	/** A sphere primitive, mostly used for bounds checking. 
	@remarks
		A sphere in math texts is normally represented by the function
		x^2 + y^2 + z^2 = r^2 (for sphere's centered on the origin). Ogre stores spheres
		simply as a center point and a radius.
	 */
	class Sphere
	{
	protected:
		f32 mRadius;
		Vector3 mCenter;
	public:
		/** Standard constructor - creates a unit sphere around the origin.*/
		Sphere() : mRadius(1.0), mCenter(Vector3::ZERO)
		{
		}
		
		/** Constructor allowing arbitrary spheres. 
			@param center The center point of the sphere.
			@param radius The radius of the sphere.
		 */
		Sphere(const Vector3& center, f32 radius)
			: mRadius(radius), mCenter(center) {}

		/** Returns the radius of the sphere. */
		f32 GetRadius(void) const { return mRadius; }

		/** Sets the radius of the sphere. */
		void SetRadius(f32 radius) { mRadius = radius; }

		/** Returns the center point of the sphere. */
		const Vector3& GetCenter(void) const
		{
			return mCenter;
		}

		/** Sets the center point of the sphere. */
		void SetCenter(const Vector3& center)
		{
			mCenter = center;
		}

		/** Returns whether or not this sphere intersects another sphere. */
		bool Intersects(const Sphere& s) const
		{
			return (s.mCenter - mCenter).LengthSquared() <=
					Maths::Sqr(s.mRadius + mRadius);
		}
		/** Returns whether or not this sphere intersects a box. */
		bool Intersects(const AxisAlignedBox& box) const
		{
			return Maths::Intersects(*this, box);
		}
		/** Returns whether or not this sphere intersects a plane. */
		bool Intersects(const Plane& plane) const
		{
			return Maths::Intersects(*this, plane);
		}
		/** Returns whether or not this sphere intersects a point. */
		bool Intersects(const Vector3& v) const
		{
			return((v - mCenter).LengthSquared() <= Maths::Sqr(mRadius));
		}
		
		Vector3 GetRandomPointInVolume()
		{
			Vector3 eulerAngles = Vector3::Random(Vector3::ZERO,Vector3(Maths::TWO_PI,Maths::TWO_PI,Maths::TWO_PI));
			Quaternion orientation = Quaternion(Radian(eulerAngles.x),Radian(eulerAngles.y),Radian(eulerAngles.z));
			Vector3 position(Maths::RangeRandom<f32>(0.f,mRadius),0,0);
			position = orientation * position;
			return position;
		}
	};
	/** @} */
	/** @} */
}
#endif

