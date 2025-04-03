#ifndef _ECHO_RAY_H_
#define _ECHO_RAY_H_

#include <echo/Maths/Vector3.h>
#include <echo/Maths/PlaneBoundedVolume.h>

namespace Echo
{
	/** \addtogroup Core
	 *  @{
	 */
	/** \addtogroup Maths
	*  @{
	*/
	/** Representation of a ray in space, i.e. a line with an origin and direction. */
	class Ray
	{
	protected:
		Vector3 mOrigin;
		Vector3 mDirection;
	public:
		Ray() : mOrigin(Vector3::ZERO), mDirection(Vector3::UNIT_Z)
		{
		}
		
		Ray(const Vector3& origin, const Vector3& direction) : mOrigin(origin), mDirection(direction)
		{
		}

		/** Sets the origin of the ray. */
		void SetOrigin(const Vector3& origin)
		{
			mOrigin = origin;
		}
		
		/** Gets the origin of the ray. */
		const Vector3& GetOrigin(void) const
		{
			return mOrigin;
		}

		/** Sets the direction of the ray. */
		void SetDirection(const Vector3& dir)
		{
			mDirection = dir;
		}
		
		/** Gets the direction of the ray. */
		const Vector3& GetDirection(void) const
		{
			return mDirection;
		}

		/** Gets the position of a point t units along the ray. */
		Vector3 GetPoint(f32 t) const
		{
			return Vector3(mOrigin + (mDirection * t));
		}
		
		/** Gets the position of a point t units along the ray. */
		Vector3 operator*(f32 t) const
		{ 
			return GetPoint(t);
		}

		/** Tests whether this ray intersects the given plane. 
		@returns A pair structure where the first element indicates whether
			an intersection occurs, and if true, the second element will
			indicate the distance along the ray at which it intersects. 
			This can be converted to a point in space by calling GetPoint().
		 */
		std::pair<bool, f32> Intersects(const Plane& p) const
		{
			return Maths::Intersects(*this, p);
		}
		
		/** Tests whether this ray intersects the given plane bounded volume. 
		@returns A pair structure where the first element indicates whether
		an intersection occurs, and if true, the second element will
		indicate the distance along the ray at which it intersects. 
		This can be converted to a point in space by calling GetPoint().
		*/
		std::pair<bool, f32> Intersects(const PlaneBoundedVolume& p) const
		{
			return Maths::Intersects(*this, p.planes, p.outside == Plane::Sides::POSITIVE);
		}
		
		/** Tests whether this ray intersects the given sphere. 
		@returns A pair structure where the first element indicates whether
			an intersection occurs, and if true, the second element will
			indicate the distance along the ray at which it intersects. 
			This can be converted to a point in space by calling GetPoint().
		 */
		std::pair<bool, f32> Intersects(const Sphere& s) const
		{
			return Maths::Intersects(*this, s);
		}
		
		/** Tests whether this ray intersects the given box. 
		@returns A pair structure where the first element indicates whether
			an intersection occurs, and if true, the second element will
			indicate the distance along the ray at which it Intersects. 
			This can be converted to a point in space by calling GetPoint().
		 */
		std::pair<bool, f32> Intersects(const AxisAlignedBox& box) const
		{
			return Maths::Intersects(*this, box);
		}
		
		friend std::ostream& operator<<(std::ostream& o, const Ray& ray)
		{
			o << "Ray(mOrigin=" << ray.mOrigin << ", mDirection=" << ray.mDirection << ")";
			return o;
		}
	};
	/** @} */
	/** @} */
}
#endif
