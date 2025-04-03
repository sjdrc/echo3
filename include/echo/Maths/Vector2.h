#ifndef _ECHO_VECTOR2_H_
#define _ECHO_VECTOR2_H_

#include <echo/Maths/EchoMaths.h>
#include <echo/Util/StringUtils.h>
#include <sstream>

namespace Echo
{
	/** \addtogroup Core
	 *  @{
	 */
	/** \addtogroup Maths
	*  @{
	*/
	/** Standard 2-dimensional vector.
		@remarks
			A direction in 2D space represented as distances along the 2
			orthogonal axes (x, y). Note that positions, directions and
			scaling factors can be represented by a vector, depending on how
			you interpret the values.
	 */
	template<typename T>
	class Vector2Generic
	{
	public:
		T x, y;

	public:
		inline Vector2Generic()
		{
		}

		inline Vector2Generic(const T fX, const T fY)
			: x( fX ), y( fY )
		{
		}

		inline explicit Vector2Generic(const T scaler)
			: x( scaler), y( scaler )
		{
		}

		inline explicit Vector2Generic(const T components[2])
			: x( components[0] ),
			  y( components[1] )
		{
		}

		inline explicit Vector2Generic(T * const r)
			: x( r[0] ), y( r[1] )
		{
		}

		/** Exchange the contents of this vector with another. 
		 */
		inline void Swap(Vector2Generic<T>& other)
		{
			std::swap(x, other.x);
			std::swap(y, other.y);
		}

		inline T operator [] ( const size_t i ) const
		{
			assert( i < 2 );

			return *(&x+i);
		}

		inline T& operator [] ( const size_t i )
		{
			assert( i < 2 );

			return *(&x+i);
		}

		/** Assigns the value of the other vector.
			@param
				rhs The other vector
		 */
		inline Vector2Generic<T>& operator =(const Vector2Generic<T>& rhs)
		{
			x = rhs.x;
			y = rhs.y;

			return *this;
		}

		inline Vector2Generic<T>& operator =(const T scalar)
		{
			x = scalar;
			y = scalar;

			return *this;
		}

		inline bool operator ==(const Vector2Generic<T>& rhs) const
		{
			return ( x == rhs.x && y == rhs.y );
		}

		inline bool operator !=(const Vector2Generic<T>& rhs) const
		{
			return ( x != rhs.x || y != rhs.y  );
		}

		// arithmetic operations
		inline Vector2Generic<T> operator +(const Vector2Generic<T>& rhs) const
		{
			return Vector2Generic<T>(
				x + rhs.x,
				y + rhs.y);
		}

		inline Vector2Generic<T> operator -(const Vector2Generic<T>& rhs) const
		{
			return Vector2Generic<T>(
				x - rhs.x,
				y - rhs.y);
		}

		inline Vector2Generic<T> operator *(const T scalar) const
		{
			return Vector2Generic<T>(
				x * scalar,
				y * scalar);
		}

		inline Vector2Generic<T> operator *(const Vector2Generic<T>& rhs) const
		{
			return Vector2Generic<T>(
				x * rhs.x,
				y * rhs.y);
		}

		inline Vector2Generic<T> operator /(const T scalar) const
		{
			assert( scalar != T(0) );

			T fInv = T(1) / scalar;

			return Vector2Generic<T>(
				x * fInv,
				y * fInv);
		}

		inline Vector2Generic<T> operator /(const Vector2Generic<T>& rhs) const
		{
			return Vector2Generic<T>(
				x / rhs.x,
				y / rhs.y);
		}

		inline const Vector2Generic<T>& operator +() const
		{
			return *this;
		}

		inline Vector2Generic<T> operator -() const
		{
			return Vector2Generic<T>(-x, -y);
		}

		// overloaded operators to help EVector2Generic<T>
		inline friend Vector2Generic<T> operator *(const T scalar, const Vector2Generic<T>& rhs)
		{
			return Vector2Generic<T>(
				scalar * rhs.x,
				scalar * rhs.y);
		}

		inline friend Vector2Generic<T> operator /(const T scalar, const Vector2Generic<T>& rhs)
		{
			return Vector2Generic<T>(
				scalar / rhs.x,
				scalar / rhs.y);
		}

		inline friend Vector2Generic<T> operator +(const Vector2Generic<T>& lhs, const T rhs)
		{
			return Vector2Generic<T>(
				lhs.x + rhs,
				lhs.y + rhs);
		}

		inline friend Vector2Generic<T> operator +(const T lhs, const Vector2Generic<T>& rhs)
		{
			return Vector2Generic<T>(
				lhs + rhs.x,
				lhs + rhs.y);
		}

		inline friend Vector2Generic<T> operator -(const Vector2Generic<T>& lhs, const T rhs)
		{
			return Vector2Generic<T>(
				lhs.x - rhs,
				lhs.y - rhs);
		}

		inline friend Vector2Generic<T> operator -(const T lhs, const Vector2Generic<T>& rhs)
		{
			return Vector2Generic<T>(
				lhs - rhs.x,
				lhs - rhs.y);
		}
		// arithmetic updates
		inline Vector2Generic<T>& operator +=(const Vector2Generic<T>& rhs)
		{
			x += rhs.x;
			y += rhs.y;

			return *this;
		}

		inline Vector2Generic<T>& operator +=(const T scalar)
		{
			x += scalar;
			y += scalar;

			return *this;
		}

		inline Vector2Generic<T>& operator -=(const Vector2Generic<T>& rhs)
		{
			x -= rhs.x;
			y -= rhs.y;

			return *this;
		}

		inline Vector2Generic<T>& operator -=(const T scalar)
		{
			x -= scalar;
			y -= scalar;

			return *this;
		}

		inline Vector2Generic<T>& operator *=(const T scalar)
		{
			x *= scalar;
			y *= scalar;

			return *this;
		}

		inline Vector2Generic<T>& operator *=(const Vector2Generic<T>& rhs)
		{
			x *= rhs.x;
			y *= rhs.y;

			return *this;
		}

		inline Vector2Generic<T>& operator /=(const T scalar)
		{
			assert( scalar != T(0) );

			T fInv = T(1) / scalar;

			x *= fInv;
			y *= fInv;

			return *this;
		}

		inline Vector2Generic<T>& operator /=(const Vector2Generic<T>& rhs)
		{
			x /= rhs.x;
			y /= rhs.y;

			return *this;
		}

		/** Returns the length (magnitude) of the vector.
			@warning
				This operation requires a square root and is expensive in
				terms of CPU operations. If you don't need to know the exact
				length (e.g. for just comparing lengths) use SquaredLength()
				instead.
		*/
		inline T Length() const
		{
			return Maths::Sqrt(x * x + y * y);
		}

		/** Returns the square of the length(magnitude) of the vector.
			@remarks
				This  method is for efficiency - calculating the actual
				length of a vector requires a square root, which is expensive
				in terms of the operations required. This method returns the
				square of the length of the vector, i.e. the same as the
				length but before the square root is taken. Use this if you
				want to find the longest / shortest vector without incurring
				the square root.
		*/
		inline T SquaredLength() const
		{
			return x * x + y * y;
		}
		/** Returns the distance to another vector.
			@warning
				This operation requires a square root and is expensive in
				terms of CPU operations. If you don't need to know the exact
				distance (e.g. for just comparing distances) use SquaredDistance()
				instead.
		 */
		inline T Distance(const Vector2Generic<T>& rhs) const
		{
			return (*this - rhs).Length();
		}

		/** Returns the square of the distance to another vector.
			@remarks
				This method is for efficiency - calculating the actual
				distance to another vector requires a square root, which is
				expensive in terms of the operations required. This method
				returns the square of the distance to another vector, i.e.
				the same as the distance but before the square root is taken.
				Use this if you want to find the longest / shortest distance
				without incurring the square root.
		 */
		inline T SquaredDistance(const Vector2Generic<T>& rhs) const
		{
			return (*this - rhs).SquaredLength();
		}

		/** Calculates the dot (scalar) product of this vector with another.
			@remarks
				The dot product can be used to calculate the angle between 2
				vectors. If both are unit vectors, the dot product is the
				cosine of the angle; otherwise the dot product must be
				divided by the product of the lengths of both vectors to get
				the cosine of the angle. This result can further be used to
				calculate the distance of a point from a plane.
			@param
				vec Vector with which to calculate the dot product (together
				with this one).
			@returns
				A float representing the dot product value.
		 */
		inline T DotProduct(const Vector2Generic<T>& vec) const
		{
			return x * vec.x + y * vec.y;
		}

		/** Normalises the vector.
			@remarks
				This method normalises the vector such that it's
				length / magnitude is 1. The result is called a unit vector.
			@note
				This function will not crash for zero-sized vectors, but there
				will be no changes made to their components.
			@returns The previous length of the vector.
		*/
		inline T Normalise()
		{
			T length = Maths::Sqrt(x * x + y * y);

			T inverseLength = T(1) / length;

			// Allows this to work for zero-sized vectors, but not change anything
			if(!Maths::IsFinite(inverseLength))
			{
				return T(0);
			}
			
			x *= inverseLength;
			y *= inverseLength;

			return length;
		}



		/** Returns a vector at a point half way between this and the passed
			in vector.
		 */
		inline Vector2Generic<T> MidPoint(const Vector2Generic<T>& vec) const
		{
			return Vector2Generic<T>(	( x + vec.x ) * T(0.5), ( y + vec.y ) * T(0.5) );
		}

		/** Returns true if the vector's scalar components are all greater
			that the ones of the vector it is compared against.
		 */
		inline bool operator<(const Vector2Generic<T>& rhs) const
		{
			if( x < rhs.x && y < rhs.y )
				return true;
			return false;
		}

		/** Returns true if the vector's scalar components are all smaller
			that the ones of the vector it is compared against.
		 */
		inline bool operator>(const Vector2Generic<T>& rhs) const
		{
			if( x > rhs.x && y > rhs.y )
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
		inline void MakeFloor(const Vector2Generic<T>& cmp)
		{
			if( cmp.x < x ) x = cmp.x;
			if( cmp.y < y ) y = cmp.y;
		}

		/** Sets this vector's components to the maximum of its own and the
			ones of the passed in vector.
			@remarks
				'Maximum' in this case means the combination of the highest
				value of x, y and z from both vectors. Highest is taken just
				numerically, not magnitude, so 1 > -3.
		 */
		inline void MakeCeil(const Vector2Generic<T>& cmp)
		{
			if( cmp.x > x ) x = cmp.x;
			if( cmp.y > y ) y = cmp.y;
		}

		/**
		 * Generates a vector perpendicular to this vector.
		 * In 2 dimensions the perpendicular vector is -y,x
		 */
		inline Vector2Generic<T> Perpendicular(void) const
		{
			return Vector2Generic<T>(-y, x);
		}
		/** Calculates the 2 dimensional cross-product of 2 vectors, which results
			in a single floating point value which is 2 times the area of the triangle.
		 */
		inline T CrossProduct(const Vector2Generic<T>& rhs) const
		{
			return x * rhs.y - y * rhs.x;
		}
		/** Generates a new random vector which deviates from this vector by a
			given angle in a random direction.
			@remarks
				This method assumes that the random number generator has already
				been seeded appropriately.
			@param
				angle The angle at which to deviate in radians
			@param
				up Any vector perpendicular to this one (which could generated
				by cross-product of this vector and any other non-colinear
				vector). If you choose not to provide this the function will
				derive one on it's own, however if you provide one yourself the
				function will be faster (this allows you to reuse up vectors if
				you call this method more than once)
			@returns
				A random vector which deviates from this vector by angle. This
				vector will not be normalised, normalise it if you wish
				afterwards.
		 */
		inline Vector2Generic<T> RandomDeviant(T angle) const
		{
			angle *= Maths::UnitRandom<T>() * Maths::TWO_PI;
			T cosa = cos(angle);
			T sina = sin(angle);
			return Vector2Generic<T>(cosa * x - sina * y, sina * x + cosa * y);
		}

		/** Returns true if this vector is zero length. */
		inline bool IsZeroLength(T thresholdAsEpsilonFactor = 2) const
		{
			T squaredLength = (x * x) + (y * y);
			return (squaredLength <= (thresholdAsEpsilonFactor*std::numeric_limits<T>::epsilon()));
		}

		/** As normalise, except that this vector is unaffected and the
			normalised vector is returned as a copy. */
		inline Vector2Generic<T> NormalisedCopy(void) const
		{
			Vector2Generic<T> ret = *this;
			ret.Normalise();
			return ret;
		}

		/** Calculates a reflection vector to the plane with the given normal .
		@remarks NB assumes 'this' is pointing AWAY FROM the plane, invert if it is not.
		 */
		inline Vector2Generic<T> Reflect(const Vector2Generic<T>& normal) const
		{
			return Vector2Generic<T>(*this -(2 * this->DotProduct(normal) * normal));
		}
		
		/** Gets the angle between 2 vectors.
		 * Vectors do not have to be unit-Length but must represent directions.
		 */
		inline Radian AngleBetween(const Vector2Generic<T>& dest)
		{
			T lenProduct = Length() * dest.Length();


			T f = DotProduct(dest) / lenProduct;

			// Did we divide by zero?
			if(!Maths::IsFinite(f))
			{
				return Radian(0);
			}

			f = Maths::Clamp(f, T(-1), T(1));
			return Maths::ACos(f);

		}
		
		/// Check whether this vector contains valid values
		inline bool IsNaN() const
		{
			return Maths::IsNaN(x) || Maths::IsNaN(y);
		}

		// special points
		static const Vector2Generic<T> ZERO;
		static const Vector2Generic<T> UNIT_X;
		static const Vector2Generic<T> UNIT_Y;
		static const Vector2Generic<T> NEGATIVE_UNIT_X;
		static const Vector2Generic<T> NEGATIVE_UNIT_Y;
		static const Vector2Generic<T> UNIT_SCALE;

		/** Function for writing to a stream.
		*/
		inline friend std::ostream& operator<< (std::ostream& o, const Vector2Generic<T>& v)
		{
			o << "Vector2(" << v.x << "," << v.y <<  ")";
			return o;
		}

		inline friend std::istream& operator>>(std::istream& i, Vector2Generic<T>& v)
		{
			using namespace Utils::String;
			std::string temp;
			i >> temp;
			if(!Utils::String::VerifyConstructorAndExtractParameters(temp,"Vector2") ||
				!ConvertAndAssign(temp, v.x, v.y))
			{
				v = Vector2Generic<T>::ZERO;
				i.setstate(std::ios_base::failbit);
			}
			return i;
		}
	};
	
	template<typename T> const Vector2Generic<T> Vector2Generic<T>::ZERO(0, 0);
	template<typename T> const Vector2Generic<T> Vector2Generic<T>::UNIT_X(1, 0);
	template<typename T> const Vector2Generic<T> Vector2Generic<T>::UNIT_Y(0, 1);
	template<typename T> const Vector2Generic<T> Vector2Generic<T>::NEGATIVE_UNIT_X(-1, 0);
	template<typename T> const Vector2Generic<T> Vector2Generic<T>::NEGATIVE_UNIT_Y(0, -1);
	template<typename T> const Vector2Generic<T> Vector2Generic<T>::UNIT_SCALE(1, 1);
	
	typedef Vector2Generic<f32> Vector2;
	typedef Vector2Generic<f32> Vector2f;
	typedef Vector2Generic<f64> Vector2Double;
	typedef Vector2Generic<f64> Vector2d;
	
	typedef Vector2Generic<u8> Vector2u8;
	typedef Vector2Generic<u16> Vector2u16;
	typedef Vector2Generic<u32> Vector2u32;
	typedef Vector2Generic<u64> Vector2u64;
	
	typedef Vector2Generic<s8> Vector2s8;
	typedef Vector2Generic<s16> Vector2s16;
	typedef Vector2Generic<s32> Vector2s32;
	typedef Vector2Generic<s64> Vector2s64;

	/** @} */
	/** @} */
}

#endif
