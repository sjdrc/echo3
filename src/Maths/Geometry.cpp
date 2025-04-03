#include <echo/Maths/EchoMaths.h>
#include <echo/Maths/Vector2.h>
#include <echo/Maths/Vector3.h>
#include <echo/Maths/Vector4.h>
#include <echo/Maths/Ray.h>
#include <echo/Maths/Sphere.h>
#include <echo/Maths/AxisAlignedBox.h>
#include <echo/Maths/Plane.h>
#include <list>

namespace Echo
{
namespace Maths
{
	bool PointInTri2D(const Vector2& p, const Vector2& a, const Vector2& b, const Vector2& c)
	{
		// Winding must be consistent from all edges for point to be inside
		Vector2 v1, v2;
		f32 dot[3];
		bool zeroDot[3];

		v1 = b - a;
		v2 = p - a;

		// Note we don't care about normalisation here since sign is all we need
		// It means we don't have to worry about magnitude of cross products either
		dot[0] = v1.CrossProduct(v2);
		zeroDot[0] = RealEqual(dot[0], 0.0f, 1e-3f);


		v1 = c - b;
		v2 = p - b;

		dot[1] = v1.CrossProduct(v2);
		zeroDot[1] = RealEqual(dot[1], 0.0f, 1e-3f);

		// Compare signs (ignore colinear / coincident points)
		if(!zeroDot[0] && !zeroDot[1] 
		&& Sign(dot[0]) != Sign(dot[1]))
		{
			return false;
		}

		v1 = a - c;
		v2 = p - c;

		dot[2] = v1.CrossProduct(v2);
		zeroDot[2] = RealEqual(dot[2], 0.0f, 1e-3f);
		// Compare signs (ignore colinear / coincident points)
		if((!zeroDot[0] && !zeroDot[2] 
			&& Sign(dot[0]) != Sign(dot[2])) ||
			(!zeroDot[1] && !zeroDot[2] 
			&& Sign(dot[1]) != Sign(dot[2])))
		{
			return false;
		}


		return true;
	}

	bool PointInTri3D(const Vector3& p, const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& normal)
	{
		// Winding must be consistent from all edges for point to be inside
		Vector3 v1, v2;
		f32 dot[3];
		bool zeroDot[3];

		v1 = b - a;
		v2 = p - a;

		// Note we don't care about normalisation here since sign is all we need
		// It means we don't have to worry about magnitude of cross products either
		dot[0] = v1.Cross(v2).Dot(normal);
		zeroDot[0] = RealEqual(dot[0], 0.0f, 1e-3f);


		v1 = c - b;
		v2 = p - b;

		dot[1] = v1.Cross(v2).Dot(normal);
		zeroDot[1] = RealEqual(dot[1], 0.0f, 1e-3f);

		// Compare signs (ignore colinear / coincident points)
		if(!zeroDot[0] && !zeroDot[1] 
			&& Sign(dot[0]) != Sign(dot[1]))
		{
			return false;
		}

		v1 = a - c;
		v2 = p - c;

		dot[2] = v1.Cross(v2).Dot(normal);
		zeroDot[2] = RealEqual(dot[2], 0.0f, 1e-3f);
		// Compare signs (ignore colinear / coincident points)
		if((!zeroDot[0] && !zeroDot[2] 
			&& Sign(dot[0]) != Sign(dot[2])) ||
			(!zeroDot[1] && !zeroDot[2] 
			&& Sign(dot[1]) != Sign(dot[2])))
		{
			return false;
		}


		return true;
	}


	std::pair<bool, f32> Intersects(const Ray& ray, const Plane& plane)
	{

		f32 denom = plane.normal.Dot(ray.GetDirection());
		if (Abs(denom) < std::numeric_limits<f32>::epsilon())
		{
			// Parallel
			return std::pair<bool, f32>(false, 0.0f);
		}
		else
		{
			f32 nom = plane.normal.Dot(ray.GetOrigin()) + plane.d;
			f32 t = -(nom/denom);
			return std::pair<bool, f32>(t >= 0, t);
		}
		
	}

	std::pair<bool, f32> Intersects(const Ray& ray,	const std::vector<Plane>& planes, bool normalIsOutside)
	{
		std::list<Plane> planesList;
		for(std::vector<Plane>::const_iterator i = planes.begin(); i != planes.end(); ++i)
		{
			planesList.push_back(*i);
		}
		return Intersects(ray, planesList, normalIsOutside);
	}

	std::pair<bool, f32> Intersects(const Ray& ray, const std::list<Plane>& planes, bool normalIsOutside)
	{
		std::list<Plane>::const_iterator planeit, planeitend;
		planeitend = planes.end();
		bool allInside = true;
		std::pair<bool, f32> ret;
		std::pair<bool, f32> end;
		ret.first = false;
		ret.second = 0.0f;
		end.first = false;
		end.second = 0;


		// derive side
		// NB we don't pass directly since that would require Plane::Side in
		// interface, which results in recursive includes since Maths is so fundamental
		Plane::Side outside = normalIsOutside ? Plane::Sides::POSITIVE : Plane::Sides::NEGATIVE;

		for (planeit = planes.begin(); planeit != planeitend; ++planeit)
		{
			const Plane& plane = *planeit;
			// is origin outside?
			if (plane.GetSide(ray.GetOrigin()) == outside)
			{
				allInside = false;
				// Test single plane
				std::pair<bool, f32> planeRes = 
					ray.Intersects(plane);
				if (planeRes.first)
				{
					// Ok, we intersected
					ret.first = true;
					// Use the most distant result since convex volume
					ret.second = std::max(ret.second, planeRes.second);
				}
				else
				{
					ret.first =false;
					ret.second=0.0f;
					return ret;
				}
			}
			else
			{
				std::pair<bool, f32> planeRes = 
					ray.Intersects(plane);
				if (planeRes.first)
				{
					if( !end.first )
					{
						end.first = true;
						end.second = planeRes.second;
					}
					else
					{
						end.second = std::min( planeRes.second, end.second );
					}

				}

			}
		}

		if (allInside)
		{
			// Intersecting at 0 distance since inside the volume!
			ret.first = true;
			ret.second = 0.0f;
			return ret;
		}

		if( end.first )
		{
			if( end.second < ret.second )
			{
				ret.first = false;
				return ret;
			}
		}
		return ret;
	}

	std::pair<bool, f32> Intersects(const Ray& ray, const Sphere& sphere, bool discardInside)
	{
		const Vector3& raydir = ray.GetDirection();
		// Adjust ray origin relative to sphere center
		const Vector3& rayorig = ray.GetOrigin() - sphere.GetCenter();
		f32 radius = sphere.GetRadius();

		// Check origin inside first
		if (rayorig.LengthSquared() <= radius*radius && discardInside)
		{
			return std::pair<bool, f32>(true, 0.0f);
		}

		// Mmm, quadratics
		// Build coeffs which can be used with std quadratic solver
		// ie t = (-b +/- sqrt(b*b + 4ac)) / 2a
		f32 a = raydir.Dot(raydir);
		f32 b = 2 * rayorig.Dot(raydir);
		f32 c = rayorig.Dot(rayorig) - radius*radius;

		// Calc determinant
		f32 d = (b*b) - (4 * a * c);
		if (d < 0)
		{
			// No intersection
			return std::pair<bool, f32>(false, 0.0f);
		}
		else
		{
			// BTW, if d=0 there is one intersection, if d > 0 there are 2
			// But we only want the closest one, so that's ok, just use the 
			// '-' version of the solver
			f32 t = ( -b - Sqrt(d) ) / (2 * a);
			if (t < 0)
				t = ( -b + Sqrt(d) ) / (2 * a);
			return std::pair<bool, f32>(true, t);
		}


	}

	std::pair<bool, f32> Intersects(const Ray& ray, const AxisAlignedBox& box)
	{
		if (box.IsNull()) return std::pair<bool, f32>(false, 0.0f);
		if (box.IsInfinite()) return std::pair<bool, f32>(true, 0.0f);

		f32 lowt = 0.0f;
		f32 t;
		bool hit = false;
		Vector3 hitpoint;
		const Vector3& min = box.GetMinimum();
		const Vector3& max = box.GetMaximum();
		const Vector3& rayorig = ray.GetOrigin();
		const Vector3& raydir = ray.GetDirection();

		// Check origin inside first
		if ( rayorig > min && rayorig < max )
		{
			return std::pair<bool, f32>(true, 0.0f);
		}

		// Check each face in turn, only check closest 3
		// Min x
		if (rayorig.x <= min.x && raydir.x > 0)
		{
			t = (min.x - rayorig.x) / raydir.x;
			if (t >= 0)
			{
				// Substitute t back into ray and check bounds and dist
				hitpoint = rayorig + raydir * t;
				if (hitpoint.y >= min.y && hitpoint.y <= max.y &&
					hitpoint.z >= min.z && hitpoint.z <= max.z &&
					(!hit || t < lowt))
				{
					hit = true;
					lowt = t;
				}
			}
		}
		// Max x
		if (rayorig.x >= max.x && raydir.x < 0)
		{
			t = (max.x - rayorig.x) / raydir.x;
			if (t >= 0)
			{
				// Substitute t back into ray and check bounds and dist
				hitpoint = rayorig + raydir * t;
				if (hitpoint.y >= min.y && hitpoint.y <= max.y &&
					hitpoint.z >= min.z && hitpoint.z <= max.z &&
					(!hit || t < lowt))
				{
					hit = true;
					lowt = t;
				}
			}
		}
		// Min y
		if (rayorig.y <= min.y && raydir.y > 0)
		{
			t = (min.y - rayorig.y) / raydir.y;
			if (t >= 0)
			{
				// Substitute t back into ray and check bounds and dist
				hitpoint = rayorig + raydir * t;
				if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
					hitpoint.z >= min.z && hitpoint.z <= max.z &&
					(!hit || t < lowt))
				{
					hit = true;
					lowt = t;
				}
			}
		}
		// Max y
		if (rayorig.y >= max.y && raydir.y < 0)
		{
			t = (max.y - rayorig.y) / raydir.y;
			if (t >= 0)
			{
				// Substitute t back into ray and check bounds and dist
				hitpoint = rayorig + raydir * t;
				if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
					hitpoint.z >= min.z && hitpoint.z <= max.z &&
					(!hit || t < lowt))
				{
					hit = true;
					lowt = t;
				}
			}
		}
		// Min z
		if (rayorig.z <= min.z && raydir.z > 0)
		{
			t = (min.z - rayorig.z) / raydir.z;
			if (t >= 0)
			{
				// Substitute t back into ray and check bounds and dist
				hitpoint = rayorig + raydir * t;
				if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
					hitpoint.y >= min.y && hitpoint.y <= max.y &&
					(!hit || t < lowt))
				{
					hit = true;
					lowt = t;
				}
			}
		}
		// Max z
		if (rayorig.z >= max.z && raydir.z < 0)
		{
			t = (max.z - rayorig.z) / raydir.z;
			if (t >= 0)
			{
				// Substitute t back into ray and check bounds and dist
				hitpoint = rayorig + raydir * t;
				if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
					hitpoint.y >= min.y && hitpoint.y <= max.y &&
					(!hit || t < lowt))
				{
					hit = true;
					lowt = t;
				}
			}
		}

		return std::pair<bool, f32>(hit, lowt);

	} 

	bool Intersects(const Ray& ray, const AxisAlignedBox& box, f32* d1, f32* d2)
	{
		if (box.IsNull())
			return false;

		if (box.IsInfinite())
		{
			if (d1) *d1 = 0;
			if (d2) *d2 = POS_INFINITY;
			return true;
		}

		const Vector3& min = box.GetMinimum();
		const Vector3& max = box.GetMaximum();
		const Vector3& rayorig = ray.GetOrigin();
		const Vector3& raydir = ray.GetDirection();

		Vector3 absDir;
		absDir[0] = Abs(raydir[0]);
		absDir[1] = Abs(raydir[1]);
		absDir[2] = Abs(raydir[2]);

		// Sort the axis, ensure check minimise floating error axis first
		int imax = 0, imid = 1, imin = 2;
		if (absDir[0] < absDir[2])
		{
			imax = 2;
			imin = 0;
		}
		if (absDir[1] < absDir[imin])
		{
			imid = imin;
			imin = 1;
		}
		else if (absDir[1] > absDir[imax])
		{
			imid = imax;
			imax = 1;
		}

		f32 start = 0, end = POS_INFINITY;

#define _CALC_AXIS(i)									   \
	do {													\
		f32 denom = 1 / raydir[i];						 \
		f32 newstart = (min[i] - rayorig[i]) * denom;	  \
		f32 newend = (max[i] - rayorig[i]) * denom;		\
		if (newstart > newend) std::swap(newstart, newend); \
		if (newstart > end || newend < start) return false; \
		if (newstart > start) start = newstart;			 \
		if (newend < end) end = newend;					 \
	} while(0)

		// Check each axis in turn

		_CALC_AXIS(imax);

		if (absDir[imid] < std::numeric_limits<f32>::epsilon())
		{
			// Parallel with middle and minimise axis, check bounds only
			if (rayorig[imid] < min[imid] || rayorig[imid] > max[imid] ||
				rayorig[imin] < min[imin] || rayorig[imin] > max[imin])
				return false;
		}
		else
		{
			_CALC_AXIS(imid);

			if (absDir[imin] < std::numeric_limits<f32>::epsilon())
			{
				// Parallel with minimise axis, check bounds only
				if (rayorig[imin] < min[imin] || rayorig[imin] > max[imin])
					return false;
			}
			else
			{
				_CALC_AXIS(imin);
			}
		}
#undef _CALC_AXIS

		if (d1) *d1 = start;
		if (d2) *d2 = end;

		return true;
	}

	std::pair<bool, f32> Intersects(const Ray& ray, const Vector3& a, const Vector3& b, const Vector3& c,
		const Vector3& normal, bool positiveSide, bool negativeSide)
	{
		//
		// Calculate intersection with plane.
		//
		f32 t;
		{
			f32 denom = normal.Dot(ray.GetDirection());

			// Check intersect side
			if (denom > + std::numeric_limits<f32>::epsilon())
			{
				if (!negativeSide)
					return std::pair<bool, f32>(false, 0);
			}
			else if (denom < - std::numeric_limits<f32>::epsilon())
			{
				if (!positiveSide)
					return std::pair<bool, f32>(false, 0);
			}
			else
			{
				// Parallel or triangle area is close to zero when
				// the plane normal not normalised.
				return std::pair<bool, f32>(false, 0);
			}

			t = normal.Dot(a - ray.GetOrigin()) / denom;

			if (t < 0)
			{
				// Intersection is behind origin
				return std::pair<bool, f32>(false, 0);
			}
		}

		//
		// Calculate the largest area projection plane in X, Y or Z.
		//
		size_t i0, i1;
		{
			f32 n0 = Abs(normal[0]);
			f32 n1 = Abs(normal[1]);
			f32 n2 = Abs(normal[2]);

			i0 = 1; i1 = 2;
			if (n1 > n2)
			{
				if (n1 > n0) i0 = 0;
			}
			else
			{
				if (n2 > n0) i1 = 0;
			}
		}

		//
		// Check the intersection point is inside the triangle.
		//
		{
			f32 u1 = b[i0] - a[i0];
			f32 v1 = b[i1] - a[i1];
			f32 u2 = c[i0] - a[i0];
			f32 v2 = c[i1] - a[i1];
			f32 u0 = t * ray.GetDirection()[i0] + ray.GetOrigin()[i0] - a[i0];
			f32 v0 = t * ray.GetDirection()[i1] + ray.GetOrigin()[i1] - a[i1];

			f32 alpha = u0 * v2 - u2 * v0;
			f32 beta  = u1 * v0 - u0 * v1;
			f32 area  = u1 * v2 - u2 * v1;

			// epsilon to avoid float precision error
			const f32 EPSILON = 1e-6f;

			f32 tolerance = - EPSILON * area;

			if (area > 0)
			{
				if (alpha < tolerance || beta < tolerance || alpha+beta > area-tolerance)
					return std::pair<bool, f32>(false, 0);
			}
			else
			{
				if (alpha > tolerance || beta > tolerance || alpha+beta < area-tolerance)
					return std::pair<bool, f32>(false, 0);
			}
		}

		return std::pair<bool, f32>(true, t);
	}

	std::pair<bool, f32> Intersects(const Ray& ray, const Vector3& a, const Vector3& b, const Vector3& c,
		bool positiveSide, bool negativeSide)
	{
		Vector3 normal = CalculateBasicFaceNormalWithoutNormalize(a, b, c);
		return Intersects(ray, a, b, c, normal, positiveSide, negativeSide);
	}
	
	bool Intersects(const Sphere& sphere, const AxisAlignedBox& box)
	{
		if (box.IsNull()) return false;
		if (box.IsInfinite()) return true;

		// Use splitting planes
		const Vector3& center = sphere.GetCenter();
		f32 radius = sphere.GetRadius();
		const Vector3& min = box.GetMinimum();
		const Vector3& max = box.GetMaximum();

		// Arvo's algorithm
		f32 s, d = 0;
		for (int i = 0; i < 3; ++i)
		{
			if (center.Ptr()[i] < min.Ptr()[i])
			{
				s = center.Ptr()[i] - min.Ptr()[i];
				d += s * s; 
			}
			else if(center.Ptr()[i] > max.Ptr()[i])
			{
				s = center.Ptr()[i] - max.Ptr()[i];
				d += s * s; 
			}
		}
		return d <= radius * radius;

	}
	
	bool Intersects(const Plane& plane, const AxisAlignedBox& box)
	{
		return(plane.GetSide(box) == Plane::Sides::BOTH);
	}

	bool Intersects(const Sphere& sphere, const Plane& plane)
	{
		return (
			Abs(plane.GetDistance(sphere.GetCenter()))
			<= sphere.GetRadius() );
	}

	Vector3 CalculateTangentSpaceVector(const Vector3& position1, const Vector3& position2, const Vector3& position3,
		f32 u1, f32 v1, f32 u2, f32 v2, f32 u3, f32 v3)
	{
		//side0 is the vector along one side of the triangle of vertices passed in, 
		//and side1 is the vector along another side. Taking the cross product of these returns the normal.
		Vector3 side0 = position1 - position2;
		Vector3 side1 = position3 - position1;
		//Calculate face normal
		Vector3 normal = side1.Cross(side0);
		normal.Normalise();
		//Now we use a formula to calculate the tangent. 
		f32 deltaV0 = v1 - v2;
		f32 deltaV1 = v3 - v1;
		Vector3 tangent = deltaV1 * side0 - deltaV0 * side1;
		tangent.Normalise();
		//Calculate binormal
		f32 deltaU0 = u1 - u2;
		f32 deltaU1 = u3 - u1;
		Vector3 binormal = deltaU1 * side0 - deltaU0 * side1;
		binormal.Normalise();
		//Now, we take the cross product of the tangents to get a vector which 
		//should point in the same direction as our normal calculated above. 
		//If it points in the opposite direction (the dot product between the normals is less than zero), 
		//then we need to reverse the s and t tangents. 
		//This is because the triangle has been mirrored when going from tangent space to object space.
		//reverse tangents if necessary
		Vector3 tangentCross = tangent.Cross(binormal);
		if (tangentCross.Dot(normal) < 0.0f)
		{
			tangent = -tangent;
			binormal = -binormal;
		}

		return tangent;

	}

	Matrix4 BuildReflectionMatrix(const Plane& p)
	{
		return Matrix4(
			-2 * p.normal.x * p.normal.x + 1,   -2 * p.normal.x * p.normal.y,	   -2 * p.normal.x * p.normal.z,	   -2 * p.normal.x * p.d, 
			-2 * p.normal.y * p.normal.x,	   -2 * p.normal.y * p.normal.y + 1,   -2 * p.normal.y * p.normal.z,	   -2 * p.normal.y * p.d, 
			-2 * p.normal.z * p.normal.x,	   -2 * p.normal.z * p.normal.y,	   -2 * p.normal.z * p.normal.z + 1,   -2 * p.normal.z * p.d, 
			0,								  0,								  0,								  1);
	}

	Vector4 CalculateFaceNormal(const Vector3& v1, const Vector3& v2, const Vector3& v3)
	{
		Vector3 normal = CalculateBasicFaceNormal(v1, v2, v3);
		// Now set up the w (distance of tri from origin
		return Vector4(normal.x, normal.y, normal.z, -(normal.Dot(v1)));
	}

	Vector3 CalculateBasicFaceNormal(const Vector3& v1, const Vector3& v2, const Vector3& v3)
	{
		Vector3 normal = (v2 - v1).Cross(v3 - v1);
		normal.Normalise();
		return normal;
	}

	Vector4 CalculateFaceNormalWithoutNormalize(const Vector3& v1, const Vector3& v2, const Vector3& v3)
	{
		Vector3 normal = CalculateBasicFaceNormalWithoutNormalize(v1, v2, v3);
		// Now set up the w (distance of tri from origin)
		return Vector4(normal.x, normal.y, normal.z, -(normal.Dot(v1)));
	}

	Vector3 CalculateBasicFaceNormalWithoutNormalize(const Vector3& v1, const Vector3& v2, const Vector3& v3)
	{
		Vector3 normal = (v2 - v1).Cross(v3 - v1);
		return normal;
	}

	Matrix4 MakeViewMatrix(const Vector3& position, const Quaternion& orientation, const Matrix4* reflectMatrix)
	{
		Matrix4 viewMatrix;

		// View matrix is:
		//
		//  [ Lx  Uy  Dz  Tx  ]
		//  [ Lx  Uy  Dz  Ty  ]
		//  [ Lx  Uy  Dz  Tz  ]
		//  [ 0   0   0   1   ]
		//
		// Where T = -(Transposed(Rot) * Pos)

		// This is most efficiently done using 3x3 Matrices
		Matrix3 rot;
		orientation.ToRotationMatrix(rot);

		// Make the translation relative to new axes
		Matrix3 rotT = rot.Transpose();
		Vector3 trans = -rotT * position;

		// Make final matrix
		viewMatrix = Matrix4::IDENTITY;
		viewMatrix = rotT; // fills upper 3x3
		viewMatrix[0][3] = trans.x;
		viewMatrix[1][3] = trans.y;
		viewMatrix[2][3] = trans.z;

		// Deal with reflections
		if (reflectMatrix)
		{
			viewMatrix = viewMatrix * (*reflectMatrix);
		}

		return viewMatrix;

	}

	f32 BoundingRadiusFromAABB(const AxisAlignedBox& aabb)
	{
		Vector3 max = aabb.GetMaximum();
		Vector3 min = aabb.GetMinimum();

		Vector3 magnitude = max;
		magnitude.MakeCeil(-max);
		magnitude.MakeCeil(min);
		magnitude.MakeCeil(-min);

		return magnitude.Length();
	}

	Quaternion GetRotationTo( Vector3 from, Vector3 v1, const Vector3& fallbackAxis)
	{
		// Based on Stan Melax's article in Game Programming Gems
		Vector3 v0 = from;
		Quaternion q;
		v0.Normalise();
		v1.Normalise();

		Vector3::StorageType d = v0.Dot(v1);
		// If dot == 1, vectors are the same
		if (d >= 1.0f)
		{
			return Quaternion::IDENTITY;
		}
		if (d < (1e-6f - 1.0f))
		{
			if(fallbackAxis != Vector3::ZERO)
			{
				// rotate 180 degrees about the fallback axis
				q.FromAngleAxis(Radian(Maths::PI), fallbackAxis);
			}
			else
			{
				// Generate an axis
				Vector3 axis = Vector3::UNIT_X.Cross(from);
				if (axis.IsZeroLength()) // pick another if colinear
					axis = Vector3::UNIT_Y.Cross(from);
				axis.Normalise();
				q.FromAngleAxis(Radian(Maths::PI), axis);
			}
		}
		else
		{
			Vector3::StorageType s = Maths::Sqrt((1 + d)*2);
			Vector3::StorageType invs = 1 / s;

			Vector3 c = v0.Cross(v1);

			q.x = c.x * invs;
			q.y = c.y * invs;
			q.z = c.z * invs;
			q.w = s * 0.5f;
			q.Normalise();
		}
		return q;
	}
}
}
