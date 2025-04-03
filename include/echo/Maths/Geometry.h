#ifndef _ECHO_MATHS_GEOMETRYH_
#define _ECHO_MATHS_GEOMETRYH_

#include <echo/Maths/Vector2.h>
#include <echo/Maths/Vector3.h>

namespace Echo
{
	class Ray;
	class AxisAlignedBox;
	class Sphere;

namespace Maths
{
	/** Checks whether a given point is inside a triangle, in a
	  2-dimensional (Cartesian) space.
	  @remarks
		  The vertices of the triangle must be given in either
		  trigonometrical (anticlockwise) or inverse trigonometrical
		  (clockwise) order.
	  @param
		  p The point.
	  @param
		  a The triangle's first vertex.
	  @param
		  b The triangle's second vertex.
	  @param
		  c The triangle's third vertex.
	  @returns
		  If the point resides in the triangle, <b>true</b> is
		  returned.
	  @par
		  If the point is outside the triangle, <b>false</b> is
		  returned.
	 */
	bool PointInTri2D(const Vector2& p, const Vector2& a, const Vector2& b, const Vector2& c);

   /** Checks whether a given 3D point is inside a triangle.
	@remarks
		 The vertices of the triangle must be given in either
		 trigonometrical (anticlockwise) or inverse trigonometrical
		 (clockwise) order, and the point must be guaranteed to be in the
		 same plane as the triangle
	 @param
		 p The point.
	 @param
		 a The triangle's first vertex.
	 @param
		 b The triangle's second vertex.
	 @param
		 c The triangle's third vertex.
	 @param 
		 normal The triangle plane's normal (passed in rather than calculated
			 on demand since the caller may already have it)
	 @returns
		 If the point resides in the triangle, <b>true</b> is
		 returned.
	 @par
		 If the point is outside the triangle, <b>false</b> is
		 returned.
	 */
	bool PointInTri3D(const Vector3& p, const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& normal);

	/** Ray / plane intersection, returns boolean result and distance. */
	std::pair<bool, f32> Intersects(const Ray& ray, const Plane& plane);

	/** Ray / sphere intersection, returns boolean result and distance. */
	std::pair<bool, f32> Intersects(const Ray& ray, const Sphere& sphere,
		bool discardInside = true);

	/** Ray / box intersection, returns boolean result and distance. */
	std::pair<bool, f32> Intersects(const Ray& ray, const AxisAlignedBox& box);

	/** Ray / box intersection, returns boolean result and two intersection distance.
	@param
		ray The ray.
	@param
		box The box.
	@param
		d1 A real pointer to retrieve the near intersection distance
			from the ray origin, maybe <b>null</b> which means don't care
			about the near intersection distance.
	@param
		d2 A real pointer to retrieve the far intersection distance
			from the ray origin, maybe <b>null</b> which means don't care
			about the far intersection distance.
	@returns
		If the ray is intersects the box, <b>true</b> is returned, and
		the near intersection distance is return by <i>d1</i>, the
		far intersection distance is return by <i>d2</i>. Guarantee
		<b>0</b> <= <i>d1</i> <= <i>d2</i>.
	@par
		If the ray isn't intersects the box, <b>false</b> is returned, and
		<i>d1</i> and <i>d2</i> is unmodified.
	 */
	bool Intersects(const Ray& ray, const AxisAlignedBox& box, f32* d1, f32* d2);

	/** Ray / triangle intersection, returns boolean result and distance.
	@param
		ray The ray.
	@param
		a The triangle's first vertex.
	@param
		b The triangle's second vertex.
	@param
		c The triangle's third vertex.
	@param 
		normal The triangle plane's normal (passed in rather than calculated
			on demand since the caller may already have it), doesn't need
			normalised since we don't care.
	@param
		positiveSide Intersect with "positive side" of the triangle
	@param
		negativeSide Intersect with "negative side" of the triangle
	@returns
		If the ray is intersects the triangle, a pair of <b>true</b> and the
		distance between intersection point and ray origin returned.
	@par
		If the ray isn't intersects the triangle, a pair of <b>false</b> and
		<b>0</b> returned.
	 */
	std::pair<bool, f32> Intersects(const Ray& ray, const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& normal,
		bool positiveSide = true, bool negativeSide = true);

	/** Ray / triangle intersection, returns boolean result and distance.
	@param
		ray The ray.
	@param
		a The triangle's first vertex.
	@param
		b The triangle's second vertex.
	@param
		c The triangle's third vertex.
	@param
		positiveSide Intersect with "positive side" of the triangle
	@param
		negativeSide Intersect with "negative side" of the triangle
	@returns
		If the ray is intersects the triangle, a pair of <b>true</b> and the
		distance between intersection point and ray origin returned.
	@par
		If the ray isn't intersects the triangle, a pair of <b>false</b> and
		<b>0</b> returned.
	 */
	std::pair<bool, f32> Intersects(const Ray& ray, const Vector3& a, const Vector3& b, const Vector3& c,
		bool positiveSide = true, bool negativeSide = true);

	/** ESphere / box intersection test. */
	bool Intersects(const Sphere& sphere, const AxisAlignedBox& box);

	/** EPlane / box intersection test. */
	bool Intersects(const Plane& plane, const AxisAlignedBox& box);

	/** Ray / convex plane list intersection test. 
	@param ray The ray to test with
	@param plaeList List of planes which form a convex volume
	@param normalIsOutside Does the normal point outside the volume
	*/
	std::pair<bool, f32> Intersects(const Ray& ray, const std::vector<Plane>& planeList, bool normalIsOutside);
	/** Ray / convex plane list intersection test. 
	@param ray The ray to test with
	@param plaeList List of planes which form a convex volume
	@param normalIsOutside Does the normal point outside the volume
	*/
	std::pair<bool, f32> Intersects(const Ray& ray, const std::list<Plane>& planeList, bool normalIsOutside);

	/** ESphere / plane intersection test. 
	@remarks NB just do a plane.GetDistance(sphere.getCenter()) for more detail!
	 */
	bool Intersects(const Sphere& sphere, const Plane& plane);

	/** Calculates the tangent space vector for a given set of positions / texture coords. */
	Vector3 CalculateTangentSpaceVector(const Vector3& position1, const Vector3& position2, const Vector3& position3,
		f32 u1, f32 v1, f32 u2, f32 v2, f32 u3, f32 v3);

	/** Build a reflection matrix for the passed in plane. */
	Matrix4 BuildReflectionMatrix(const Plane& p);
	
	/** Calculate a face normal, including the w component which is the offset from the origin. */
	Vector4 CalculateFaceNormal(const Vector3& v1, const Vector3& v2, const Vector3& v3);
	/** Calculate a face normal, no w-information. */
	Vector3 CalculateBasicFaceNormal(const Vector3& v1, const Vector3& v2, const Vector3& v3);
	/** Calculate a face normal without normalize, including the w component which is the offset from the origin. */
	Vector4 CalculateFaceNormalWithoutNormalize(const Vector3& v1, const Vector3& v2, const Vector3& v3);
	/** Calculate a face normal without normalize, no w-information. */
	Vector3 CalculateBasicFaceNormalWithoutNormalize(const Vector3& v1, const Vector3& v2, const Vector3& v3);

	Matrix4 MakeViewMatrix(const Vector3& position, const Quaternion& orientation, const Matrix4* reflectMatrix = 0);

	/** Get a bounding radius value from a bounding box. */
	f32 BoundingRadiusFromAABB(const AxisAlignedBox& aabb);
	
	/** Gets the shortest arc quaternion to rotate this vector to the destination
		vector.
	@remarks
		If you call this with a dest vector that is close to the inverse
		of this vector, we will rotate 180 degrees around the 'fallbackAxis'
		(if specified, or a generated axis if not) since in this case
		ANY axis of rotation is valid.
	 */
	Quaternion GetRotationTo(Vector3 from, Vector3 to, const Vector3& fallbackAxis = Vector3::ZERO);
}
}
#endif
