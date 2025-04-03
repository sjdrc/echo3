#ifndef _ECHO_VECTOR3_H_
#define _ECHO_VECTOR3_H_

#include <ostream>
#include <echo/Util/StringUtils.h>
#include <echo/Maths/EchoMaths.h>

namespace Echo
{
	/** \addtogroup Core
	*  @{
	*/
	/** \addtogroup EMaths
	*  @{
	*/
	/** Standard 3-dimensional vector.
	@remarks
		A direction in 3D space represented as distances along the 3
		orthogonal axes (x, y, z). Note that positions, directions and
		scaling factors can be represented by a vector, depending on how
		you interpret the values.
	 */
	template<typename T>
	class Vector3Generic
	{
	public:
		typedef T StorageType;
		
		T x, y, z;

		inline Vector3Generic()
		{
		}

		inline Vector3Generic(const T fX, const T fY, const T fZ)
			: x( fX ), y( fY ), z( fZ )
		{
		}

		inline explicit Vector3Generic(const T components[3])
			: x( components[0] ),
			  y( components[1] ),
			  z( components[2] )
		{
		}

		inline explicit Vector3Generic(T * const r)
			: x( r[0] ), y( r[1] ), z( r[2] )
		{
		}

		inline bool IsZero() const
		{
			return (x==0 && y==0 && z==0);
		}

		inline bool HasZeroComponent() const
		{
			return (x==0 || y==0 || z==0);
		}

		inline void Set(T ix, T iy, T iz)
		{
			x=ix;
			y=iy;
			z=iz;
		}

		inline void Set(const Vector3Generic<T>& v)
		{
			x=v.x;
			y=v.y;
			z=v.z;
		}

		inline void SetZero()
		{
			x=0;
			y=0;
			z=0;
		}

		inline explicit Vector3Generic(const T scaler)
			: x( scaler )
			, y( scaler )
			, z( scaler )
		{
		}


		/** Exchange the contents of this vector with another. 
		 */
		inline void Swap(Vector3Generic<T>& other)
		{
			std::swap(x, other.x);
			std::swap(y, other.y);
			std::swap(z, other.z);
		}

		inline T operator [] ( const size_t i ) const
		{
			assert( i < 3 );

			return *(&x+i);
		}

		inline T& operator [] ( const size_t i )
		{
			assert( i < 3 );

			return *(&x+i);
		}
		
		/// Pointer accessor for direct copying
		inline T* Ptr()
		{
			return &x;
		}
		/// Pointer accessor for direct copying
		inline const T* Ptr() const
		{
			return &x;
		}
		

		/** Assigns the value of the other vector.
			@param
				rhs The other vector
		 */
		inline Vector3Generic<T>& operator =(const Vector3Generic<T>& rhs)
		{
			x = rhs.x;
			y = rhs.y;
			z = rhs.z;

			return *this;
		}

		inline Vector3Generic<T>& operator =(const T scalar)
		{
			x = scalar;
			y = scalar;
			z = scalar;

			return *this;
		}

		inline bool operator ==(const Vector3Generic<T>& rhs) const
		{
			return ( x == rhs.x && y == rhs.y && z == rhs.z );
		}

		inline bool operator !=(const Vector3Generic<T>& rhs) const
		{
			return ( x != rhs.x || y != rhs.y || z != rhs.z );
		}

		// arithmetic operations
		inline Vector3Generic<T> operator +(const Vector3Generic<T>& rhs) const
		{
			return Vector3Generic<T>(
				x + rhs.x,
				y + rhs.y,
				z + rhs.z);
		}

		inline Vector3Generic<T> operator -(const Vector3Generic<T>& rhs) const
		{
			return Vector3Generic<T>(
				x - rhs.x,
				y - rhs.y,
				z - rhs.z);
		}

		inline Vector3Generic<T> operator *(const T scalar) const
		{
			return Vector3Generic<T>(
				x * scalar,
				y * scalar,
				z * scalar);
		}

		inline Vector3Generic<T> operator *(const Vector3Generic<T>& rhs) const
		{
			return Vector3Generic<T>(
				x * rhs.x,
				y * rhs.y,
				z * rhs.z);
		}

		inline Vector3Generic<T> operator /(const T scalar) const
		{
			assert( scalar != T(0) );

			T inverse = T(1) / scalar;

			return Vector3Generic<T>(
				x * inverse,
				y * inverse,
				z * inverse);
		}

		inline Vector3Generic<T> operator /(const Vector3Generic<T>& rhs) const
		{
			return Vector3Generic<T>(
				x / rhs.x,
				y / rhs.y,
				z / rhs.z);
		}

		inline const Vector3Generic<T>& operator +() const
		{
			return *this;
		}

		inline Vector3Generic<T> operator -() const
		{
			return Vector3Generic<T>(-x, -y, -z);
		}

		// overloaded operators to help Vector3Generic<T>
		inline friend Vector3Generic<T> operator *(const T scalar, const Vector3Generic<T>& rhs)
		{
			return Vector3Generic<T>(
				scalar * rhs.x,
				scalar * rhs.y,
				scalar * rhs.z);
		}

		inline friend Vector3Generic<T> operator /(const T scalar, const Vector3Generic<T>& rhs)
		{
			return Vector3Generic<T>(
				scalar / rhs.x,
				scalar / rhs.y,
				scalar / rhs.z);
		}

		inline friend Vector3Generic<T> operator +(const Vector3Generic<T>& lhs, const T rhs)
		{
			return Vector3Generic<T>(
				lhs.x + rhs,
				lhs.y + rhs,
				lhs.z + rhs);
		}

		inline friend Vector3Generic<T> operator +(const T lhs, const Vector3Generic<T>& rhs)
		{
			return Vector3Generic<T>(
				lhs + rhs.x,
				lhs + rhs.y,
				lhs + rhs.z);
		}

		inline friend Vector3Generic<T> operator -(const Vector3Generic<T>& lhs, const T rhs)
		{
			return Vector3Generic<T>(
				lhs.x - rhs,
				lhs.y - rhs,
				lhs.z - rhs);
		}

		inline friend Vector3Generic<T> operator -(const T lhs, const Vector3Generic<T>& rhs)
		{
			return Vector3Generic<T>(
				lhs - rhs.x,
				lhs - rhs.y,
				lhs - rhs.z);
		}

		// arithmetic updates
		inline Vector3Generic<T>& operator +=(const Vector3Generic<T>& rhs)
		{
			x += rhs.x;
			y += rhs.y;
			z += rhs.z;

			return *this;
		}

		inline Vector3Generic<T>& operator +=(const T scalar)
		{
			x += scalar;
			y += scalar;
			z += scalar;
			return *this;
		}

		inline Vector3Generic<T>& operator -=(const Vector3Generic<T>& rhs)
		{
			x -= rhs.x;
			y -= rhs.y;
			z -= rhs.z;

			return *this;
		}

		inline Vector3Generic<T>& operator -=(const T scalar)
		{
			x -= scalar;
			y -= scalar;
			z -= scalar;
			return *this;
		}

		inline Vector3Generic<T>& operator *=(const T scalar)
		{
			x *= scalar;
			y *= scalar;
			z *= scalar;
			return *this;
		}

		inline Vector3Generic<T>& operator *=(const Vector3Generic<T>& rhs)
		{
			x *= rhs.x;
			y *= rhs.y;
			z *= rhs.z;

			return *this;
		}

		inline Vector3Generic<T>& operator /=(const T scalar)
		{
			assert( scalar != T(0) );

			T inverse = T(1) / scalar;

			x *= inverse;
			y *= inverse;
			z *= inverse;

			return *this;
		}

		inline Vector3Generic<T>& operator /=(const Vector3Generic<T>& rhs)
		{
			x /= rhs.x;
			y /= rhs.y;
			z /= rhs.z;

			return *this;
		}


		/** Returns the Length (magnitude) of the vector.
			@warning
				This operation requires a square root and is expensive in
				terms of CPU operations. If you don't need to know the exact
				Length (e.g. for just comparing lengths) use LengthSquared()
				instead.
		*/
		inline T Length () const
		{
			return Maths::Sqrt(x * x + y * y + z * z);
		}

		/** Returns the square of the Length(magnitude) of the vector.
			@remarks
				This  method is for efficiency - calculating the actual
				Length of a vector requires a square root, which is expensive
				in terms of the operations required. This method returns the
				square of the Length of the vector, i.e. the same as the
				Length but before the square root is taken. Use this if you
				want to find the longest / shortest vector without incurring
				the square root.
		*/
		inline T LengthSquared () const
		{
			return x * x + y * y + z * z;
		}

		/** Returns the Distance to another vector.
			@warning
				This operation requires a square root and is expensive in
				terms of CPU operations. If you don't need to know the exact
				Distance (e.g. for just comparing distances) use DistanceSquared()
				instead.
		 */
		inline T Distance(const Vector3Generic<T>& rhs) const
		{
			return (*this - rhs).Length();
		}

		/** Returns the square of the Distance to another vector.
			@remarks
				This method is for efficiency - calculating the actual
				Distance to another vector requires a square root, which is
				expensive in terms of the operations required. This method
				returns the square of the Distance to another vector, i.e.
				the same as the Distance but before the square root is taken.
				Use this if you want to find the longest / shortest Distance
				without incurring the square root.
		 */
		inline T DistanceSquared(const Vector3Generic<T>& rhs) const
		{
			return (*this - rhs).LengthSquared();
		}

		/** Calculates the dot (scalar) product of this vector with another.
			@remarks
				The dot product can be used to calculate the angle between 2
				vectors. If both are unit vectors, the dot product is the
				cosine of the angle; otherwise the dot product must be
				divided by the product of the lengths of both vectors to get
				the cosine of the angle. This result can further be used to
				calculate the Distance of a point from a plane.
			@param
				vec Vector with which to calculate the dot product (together
				with this one).
			@returns
				A float representing the dot product value.
		 */
		inline T Dot(const Vector3Generic<T>& vec) const
		{
			return x * vec.x + y * vec.y + z * vec.z;
		}

		/** Calculates the absolute dot (scalar) product of this vector with another.
			@remarks
				This function work similar Dot, except it use absolute value
				of each component of the vector to computing.
			@param
				vec Vector with which to calculate the absolute dot product (together
				with this one).
			@returns
				A T representing the absolute dot product value.
		 */
		inline T AbsDotProduct(const Vector3Generic<T>& vec) const
		{
			return Maths::Abs(x * vec.x) + Maths::Abs(y * vec.y) + Maths::Abs(z * vec.z);
		}

		/** Normalises the vector.
			@remarks
				This method normalises the vector such that it's
				Length / magnitude is 1. The result is called a unit vector.
			@note
				This function will not crash for zero-sized vectors, but there
				will be no changes made to their components.
			@returns The previous Length of the vector.
		*/
		inline T Normalise()
		{
			T length = Maths::Sqrt(x * x + y * y + z * z);

			T inverseLength = T(1) / length;

			// Allows this to work for zero-sized vectors, but not change anything
			if(!Maths::IsFinite(inverseLength))
			{
				return T(0);
			}
			
			x *= inverseLength;
			y *= inverseLength;
			z *= inverseLength;
			return length;
		}

		/** Calculates the cross-product of 2 vectors, i.e. the vector that
			lies perpendicular to them both.
			@remarks
				The cross-product is normally used to calculate the normal
				vector of a plane, by calculating the cross-product of 2
				non-equivalent vectors which lie on the plane (e.g. 2 edges
				of a triangle).
			@param
				vec Vector which, together with this one, will be used to
				calculate the cross-product.
			@returns
				A vector which is the result of the cross-product. This
				vector will <b>NOT</b> be normalised, to maximise efficiency
				- call Vector3::Normalise on the result if you wish this to
				be done. As for which side the resultant vector will be on, the
				returned vector will be on the side from which the arc from 'this'
				to rhs is anticlockwise, e.g. UNIT_Y.Cross(UNIT_Z)
				= UNIT_X, whilst UNIT_Z.Cross(UNIT_Y) = -UNIT_X.
				This is because OGRE uses a right-handed coordinate system.
			@par
				For a clearer explanation, look a the left and the bottom edges
				of your monitor's screen. Assume that the first vector is the
				left edge and the second vector is the bottom edge, both of
				them starting from the lower-left corner of the screen. The
				resulting vector is going to be perpendicular to both of them
				and will go <i>inside</i> the screen, towards the cathode tube
				(assuming you're using a CRT monitor, of course).
		 */
		inline Vector3Generic<T> Cross(const Vector3Generic<T>& rhs) const
		{
			return Vector3Generic<T>( y * rhs.z - z * rhs.y,
							z * rhs.x - x * rhs.z,
							x * rhs.y - y * rhs.x);
		}

		/** Returns a vector at a point half way between this and the passed
			in vector.
		 */
		inline Vector3Generic<T> MidPoint(const Vector3Generic<T>& vec) const
		{
			return Vector3Generic<T>( ( x + vec.x ) * T(0.5),
							( y + vec.y ) * T(0.5),
							( z + vec.z ) * T(0.5) );
		}

		/** Returns true if the vector's scalar components are all greater
			that the ones of the vector it is compared against.
		 */
		inline bool operator<(const Vector3Generic<T>& rhs) const
		{
			if( x < rhs.x && y < rhs.y && z < rhs.z )
				return true;
			return false;
		}

		/** Returns true if the vector's scalar components are all smaller
			that the ones of the vector it is compared against.
		 */
		inline bool operator>(const Vector3Generic<T>& rhs) const
		{
			if( x > rhs.x && y > rhs.y && z > rhs.z )
				return true;
			return false;
		}

		/** Sets this vector's components to the minimum of its own and the
			ones of the passed in vector.
			@remarks
				'Minimum' in this case means the combination of the lowest
				value of x, y and z from both vectors. Lowest is taken just
				numerically, not magnitude, so -1 < 0.
		 */
		inline void MakeFloor(const Vector3Generic<T>& cmp)
		{
			if( cmp.x < x ) x = cmp.x;
			if( cmp.y < y ) y = cmp.y;
			if( cmp.z < z ) z = cmp.z;
		}

		/** Sets this vector's components to the maximum of its own and the
			ones of the passed in vector.
			@remarks
				'Maximum' in this case means the combination of the highest
				value of x, y and z from both vectors. Highest is taken just
				numerically, not magnitude, so 1 > -3.
		 */
		inline void MakeCeil(const Vector3Generic<T>& cmp)
		{
			if( cmp.x > x ) x = cmp.x;
			if( cmp.y > y ) y = cmp.y;
			if( cmp.z > z ) z = cmp.z;
		}

		/** Generates a vector perpendicular to this vector (eg an 'up' vector).
			@remarks
				This method will return a vector which is perpendicular to this
				vector. There are an infinite number of possibilities but this
				method will guarantee to generate one of them. If you need more
				control you should use the Quaternion class.
		 */
		inline Vector3Generic<T> Perpendicular(void) const
		{
			Vector3Generic<T> perp = Cross(Vector3Generic<T>::UNIT_X);

			// Check Length
			if( perp.IsZeroLength() )
			{
				/* This vector is the Y axis multiplied by a scalar, so we have
				   to use another axis.
				 */
				perp = Cross(Vector3Generic<T>::UNIT_Y);
			}
			perp.Normalise();

			return perp;
		}

		/** Gets the angle between 2 vectors.
		@remarks
			Vectors do not have to be unit-Length but must represent directions.
		 */
		inline Radian AngleBetween(const Vector3Generic<T>& dest)
		{
			T lenProduct = Length() * dest.Length();
			T f = Dot(dest) / lenProduct;

			// Did we divide by zero?
			if(!Maths::IsFinite(f))
			{
				return Radian(0);
			}

			f = Maths::Clamp(f, T(-1), T(1));
			return Maths::ACos(f);

		}

		/** Returns true if this vector is zero Length. */
		inline bool IsZeroLength(T thresholdAsEpsilonFactor = 2) const
		{
			T squaredLength = (x * x) + (y * y) + (z * z);
			return (squaredLength <= (thresholdAsEpsilonFactor*std::numeric_limits<T>::epsilon()));
		}

		/** As Normalise, except that this vector is unaffected and the
			normalised vector is returned as a copy. */
		inline Vector3Generic<T> NormalisedCopy(void) const
		{
			Vector3Generic<T> ret = *this;
			ret.Normalise();
			return ret;
		}

		/** Calculates a reflection vector to the plane with the given normal .
		@remarks NB assumes 'this' is pointing AWAY FROM the plane, invert if it is not.
		 */
		inline Vector3Generic<T> Reflect(const Vector3Generic<T>& normal) const
		{
			return Vector3Generic<T>(*this -(2 * this->Dot(normal) * normal));
		}

		/** Returns whether this vector is within a positional tolerance
			of another vector.
		@param rhs The vector to compare with
		@param tolerance The amount that each element of the vector may vary by
			and still be considered equal
		 */
		inline bool PositionEquals(const Vector3Generic<T>& rhs, T tolerance = 1e-03) const
		{
			return Maths::RealEqual(x, rhs.x, tolerance) &&
					Maths::RealEqual(y, rhs.y, tolerance) &&
					Maths::RealEqual(z, rhs.z, tolerance);

		}

		/** Returns whether this vector is within a positional tolerance
			of another vector, also take scale of the vectors into account.
		@param rhs The vector to compare with
		@param tolerance The amount (related to the scale of vectors) that Distance
			of the vector may vary by and still be considered close
		 */
		inline bool PositionCloses(const Vector3Generic<T>& rhs, T tolerance = 1e-03f) const
		{
			return DistanceSquared(rhs) <= (LengthSquared() + rhs.LengthSquared()) * tolerance;
		}

		/** Returns whether this vector is within a directional tolerance
			of another vector.
		@param rhs The vector to compare with
		@param tolerance The maximum angle by which the vectors may vary and
			still be considered equal
		@note Both vectors should be normalised.
		 */
		inline bool DirectionEquals(const Vector3Generic<T>& rhs, const Radian& tolerance) const
		{
			T dot = Dot(rhs);
			Radian angle = Maths::ACos(dot);

			return Maths::Abs(angle.ValueRadians()) <= tolerance.ValueRadians();

		}

		/// Check whether this vector contains valid values
		inline bool IsNaN() const
		{
			return Maths::IsNaN(x) || Maths::IsNaN(y) || Maths::IsNaN(z);
		}

		// special points
		static const Vector3Generic<T> ZERO;
		static const Vector3Generic<T> UNIT_X;
		static const Vector3Generic<T> UNIT_Y;
		static const Vector3Generic<T> UNIT_Z;
		static const Vector3Generic<T> NEGATIVE_UNIT_X;
		static const Vector3Generic<T> NEGATIVE_UNIT_Y;
		static const Vector3Generic<T> NEGATIVE_UNIT_Z;
		static const Vector3Generic<T> UNIT_SCALE;

		/** Function for writing to a stream.
		*/
		inline friend std::ostream& operator<<(std::ostream& o, const Vector3Generic<T>& v)
		{
			o << "Vector3(" << v.x << "," << v.y << "," << v.z << ")";
			return o;
		}

		inline friend std::istream& operator>>(std::istream& i, Vector3Generic<T>& v)
		{
			using namespace Utils::String;
			std::string temp;
			i >> temp;
			if(!Utils::String::VerifyConstructorAndExtractParameters(temp,"Vector3") ||
				!ConvertAndAssign(temp, v.x, v.y, v.z))
			{
				v = Vector3Generic<T>::ZERO;
				i.setstate(std::ios_base::failbit);
			}
			return i;
		}
		
		static Vector3Generic<T> Random(Vector3Generic<T> minValue, Vector3Generic<T> maxValue)
		{
			maxValue=maxValue-minValue;
			maxValue.x*=(T)rand()/(T)RAND_MAX;
			maxValue.y*=(T)rand()/(T)RAND_MAX;
			maxValue.z*=(T)rand()/(T)RAND_MAX;
			maxValue+=minValue;
			return maxValue;
		}
	};

	template<typename T> const Vector3Generic<T> Vector3Generic<T>::ZERO(0, 0, 0);
	template<typename T> const Vector3Generic<T> Vector3Generic<T>::UNIT_X(1, 0, 0);
	template<typename T> const Vector3Generic<T> Vector3Generic<T>::UNIT_Y(0, 1, 0);
	template<typename T> const Vector3Generic<T> Vector3Generic<T>::UNIT_Z(0, 0, 1);
	template<typename T> const Vector3Generic<T> Vector3Generic<T>::NEGATIVE_UNIT_X(-1, 0, 0);
	template<typename T> const Vector3Generic<T> Vector3Generic<T>::NEGATIVE_UNIT_Y(0, -1, 0);
	template<typename T> const Vector3Generic<T> Vector3Generic<T>::NEGATIVE_UNIT_Z(0, 0, -1);
	template<typename T> const Vector3Generic<T> Vector3Generic<T>::UNIT_SCALE(1, 1, 1);

	typedef Vector3Generic<f32> Vector3;
	typedef Vector3Generic<f32> Vector3f;
	typedef Vector3Generic<f64> Vector3Double;
	typedef Vector3Generic<f64> Vector3d;

	typedef Vector3Generic<u8> Vector3u8;
	typedef Vector3Generic<u16> Vector3u16;
	typedef Vector3Generic<u32> Vector3u32;
	typedef Vector3Generic<u64> Vector3u64;

	typedef Vector3Generic<s8> Vector3s8;
	typedef Vector3Generic<s16> Vector3s16;
	typedef Vector3Generic<s32> Vector3s32;
	typedef Vector3Generic<s64> Vector3s64;

	/** @} */
	/** @} */
}
#endif
