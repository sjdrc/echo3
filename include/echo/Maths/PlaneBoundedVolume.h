#ifndef _ECHO_PLANEBOUNDEDVOLUME_H_
#define _ECHO_PLANEBOUNDEDVOLUME_H_

#include <echo/Maths/AxisAlignedBox.h>
#include <echo/Maths/Sphere.h>
#include <echo/Maths/EchoMaths.h>
#include <echo/Maths/Plane.h>

namespace Echo
{
	/** \addtogroup Core
	 *  @{
	 */
	/** \addtogroup Maths
	*  @{
	*/
	/** Represents a convex volume bounded by planes.
	 */
	class PlaneBoundedVolume
	{
	public:
		typedef std::vector<Plane> PlaneList;
		/// Publicly accessible plane list, you can modify this direct
		PlaneList planes;
		Plane::Side outside;
		PlaneBoundedVolume() : outside(Plane::Sides::NEGATIVE)
		{
		}
		/** Constructor, determines which side is deemed to be 'outside' */
		PlaneBoundedVolume(Plane::Side theOutside)
			: outside(theOutside) {}

		/** Intersection test with AABB
		@remarks May return false positives but will never miss an intersection.
		 */
		inline bool Intersects(const AxisAlignedBox& box) const
		{
			if (box.IsNull()) return false;
			if (box.IsInfinite()) return true;

			// Get centre of the box
			Vector3 centre = box.GetCentre();
			// Get the half-size of the box
			Vector3 halfSize = box.GetHalfSize();
			
			PlaneList::const_iterator i, iend;
			iend = planes.end();
			for (i = planes.begin(); i != iend; ++i)
			{
				const Plane& plane = *i;

				Plane::Side side = plane.GetSide(centre, halfSize);
				if (side == outside)
				{
					// Found a splitting plane therefore return not intersecting
					return false;
				}
			}

			// couldn't find a splitting plane, assume intersecting
			return true;

		}
		/** Intersection test with ESphere
		@remarks May return false positives but will never miss an intersection.
		 */
		inline bool Intersects(const Sphere& sphere) const
		{
			PlaneList::const_iterator i, iend;
			iend = planes.end();
			for (i = planes.begin(); i != iend; ++i)
			{
				const Plane& plane = *i;

				// Test which side of the plane the sphere is
				f32 d = plane.GetDistance(sphere.GetCenter());
				// Negate d if planes point inwards
				if(outside == Plane::Sides::NEGATIVE) d = -d;

				if ( (d - sphere.GetRadius()) > 0)
					return false;
			}

			return true;

		}

		/** Intersection test with a ERay
		@returns std::pair of hit (bool) and distance
		@remarks May return false positives but will never miss an intersection.
		 */
		inline std::pair<bool, f32> Intersects(const Ray& ray)
		{
			return Maths::Intersects(ray, planes, outside == Plane::Sides::POSITIVE);
		}

	};

	typedef std::vector<PlaneBoundedVolume> PlaneBoundedVolumeList;

	/** @} */
	/** @} */
}
#endif

