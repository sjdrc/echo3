#ifndef _ECHO_VECTOR4_H__
#define _ECHO_VECTOR4_H__

#include <echo/Maths/Vector3.h>
#include <echo/Util/StringUtils.h>

#ifdef ECHO_OGRE_SUPPORT
#include <OgreVector4.h>
#endif

namespace Echo
{

	/** \addtogroup Core
	*  @{
	*/
	/** \addtogroup EMaths
	*  @{
	*/
	/** 4-dimensional homogeneous vector.
	*/
	class Vector4
	{
	public:
		f32 x, y, z, w;

	public:
		inline Vector4()
		{
		}

		inline Vector4( const f32 fX, const f32 fY, const f32 fZ, const f32 fW )
			: x( fX ), y( fY ), z( fZ ), w( fW)
		{
		}

		inline explicit Vector4( const f32 afCoordinate[4] )
			: x( afCoordinate[0] ),
			  y( afCoordinate[1] ),
			  z( afCoordinate[2] ),
			  w( afCoordinate[3] )
		{
		}

		inline explicit Vector4( const int afCoordinate[4] )
		{
			x = (f32)afCoordinate[0];
			y = (f32)afCoordinate[1];
			z = (f32)afCoordinate[2];
			w = (f32)afCoordinate[3];
		}

		inline explicit Vector4( f32* const r )
			: x( r[0] ), y( r[1] ), z( r[2] ), w( r[3] )
		{
		}

		inline explicit Vector4( const f32 scaler )
			: x( scaler )
			, y( scaler )
			, z( scaler )
			, w( scaler )
		{
		}

		inline explicit Vector4(const Vector3& rhs)
			: x(rhs.x), y(rhs.y), z(rhs.z), w(1.0f)
		{
		}

		inline void SetZero()
		{
			x=0;
			y=0;
			z=0;
			w=0;
		}

		/** Exchange the contents of this vector with another. 
		*/
		inline void Swap(Vector4& other)
		{
			std::swap(x, other.x);
			std::swap(y, other.y);
			std::swap(z, other.z);
			std::swap(w, other.w);
		}
	
		inline f32 operator [] ( const size_t i ) const
		{
			assert( i < 4 );

			return *(&x+i);
		}

		inline f32& operator [] ( const size_t i )
		{
			assert( i < 4 );

			return *(&x+i);
		}

		/// Pointer accessor for direct copying
		inline f32* Ptr()
		{
			return &x;
		}
		/// Pointer accessor for direct copying
		inline const f32* Ptr() const
		{
			return &x;
		}

		/** Assigns the value of the other vector.
			@param
				rkVector The other vector
		*/
		inline Vector4& operator = ( const Vector4& rkVector )
		{
			x = rkVector.x;
			y = rkVector.y;
			z = rkVector.z;
			w = rkVector.w;

			return *this;
		}

		inline Vector4& operator = ( const f32 fScalar)
		{
			x = fScalar;
			y = fScalar;
			z = fScalar;
			w = fScalar;
			return *this;
		}

		inline bool operator == ( const Vector4& rkVector ) const
		{
			return ( x == rkVector.x &&
				y == rkVector.y &&
				z == rkVector.z &&
				w == rkVector.w );
		}

		inline bool operator != ( const Vector4& rkVector ) const
		{
			return ( x != rkVector.x ||
				y != rkVector.y ||
				z != rkVector.z ||
				w != rkVector.w );
		}

		inline Vector4& operator = (const Vector3& rhs)
		{
			x = rhs.x;
			y = rhs.y;
			z = rhs.z;
			w = 1.0f;
			return *this;
		}

		// arithmetic operations
		inline Vector4 operator + ( const Vector4& rkVector ) const
		{
			return Vector4(
				x + rkVector.x,
				y + rkVector.y,
				z + rkVector.z,
				w + rkVector.w);
		}

		inline Vector4 operator - ( const Vector4& rkVector ) const
		{
			return Vector4(
				x - rkVector.x,
				y - rkVector.y,
				z - rkVector.z,
				w - rkVector.w);
		}

		inline Vector4 operator * ( const f32 fScalar ) const
		{
			return Vector4(
				x * fScalar,
				y * fScalar,
				z * fScalar,
				w * fScalar);
		}

		inline Vector4 operator * ( const Vector4& rhs) const
		{
			return Vector4(
				rhs.x * x,
				rhs.y * y,
				rhs.z * z,
				rhs.w * w);
		}

		inline Vector4 operator / ( const f32 fScalar ) const
		{
			assert( fScalar != 0.0 );

			f32 fInv = 1.0f / fScalar;

			return Vector4(
				x * fInv,
				y * fInv,
				z * fInv,
				w * fInv);
		}

		inline Vector4 operator / ( const Vector4& rhs) const
		{
			return Vector4(
				x / rhs.x,
				y / rhs.y,
				z / rhs.z,
				w / rhs.w);
		}

		inline const Vector4& operator + () const
		{
			return *this;
		}

		inline Vector4 operator - () const
		{
			return Vector4(-x, -y, -z, -w);
		}

		inline friend Vector4 operator * ( const f32 fScalar, const Vector4& rkVector )
		{
			return Vector4(
				fScalar * rkVector.x,
				fScalar * rkVector.y,
				fScalar * rkVector.z,
				fScalar * rkVector.w);
		}

		inline friend Vector4 operator / ( const f32 fScalar, const Vector4& rkVector )
		{
			return Vector4(
				fScalar / rkVector.x,
				fScalar / rkVector.y,
				fScalar / rkVector.z,
				fScalar / rkVector.w);
		}

		inline friend Vector4 operator + (const Vector4& lhs, const f32 rhs)
		{
			return Vector4(
				lhs.x + rhs,
				lhs.y + rhs,
				lhs.z + rhs,
				lhs.w + rhs);
		}

		inline friend Vector4 operator + (const f32 lhs, const Vector4& rhs)
		{
			return Vector4(
				lhs + rhs.x,
				lhs + rhs.y,
				lhs + rhs.z,
				lhs + rhs.w);
		}

		inline friend Vector4 operator - (const Vector4& lhs, f32 rhs)
		{
			return Vector4(
				lhs.x - rhs,
				lhs.y - rhs,
				lhs.z - rhs,
				lhs.w - rhs);
		}

		inline friend Vector4 operator - (const f32 lhs, const Vector4& rhs)
		{
			return Vector4(
				lhs - rhs.x,
				lhs - rhs.y,
				lhs - rhs.z,
				lhs - rhs.w);
		}

		// arithmetic updates
		inline Vector4& operator += ( const Vector4& rkVector )
		{
			x += rkVector.x;
			y += rkVector.y;
			z += rkVector.z;
			w += rkVector.w;

			return *this;
		}

		inline Vector4& operator -= ( const Vector4& rkVector )
		{
			x -= rkVector.x;
			y -= rkVector.y;
			z -= rkVector.z;
			w -= rkVector.w;

			return *this;
		}

		inline Vector4& operator *= ( const f32 fScalar )
		{
			x *= fScalar;
			y *= fScalar;
			z *= fScalar;
			w *= fScalar;
			return *this;
		}

		inline Vector4& operator += ( const f32 fScalar )
		{
			x += fScalar;
			y += fScalar;
			z += fScalar;
			w += fScalar;
			return *this;
		}

		inline Vector4& operator -= ( const f32 fScalar )
		{
			x -= fScalar;
			y -= fScalar;
			z -= fScalar;
			w -= fScalar;
			return *this;
		}

		inline Vector4& operator *= ( const Vector4& rkVector )
		{
			x *= rkVector.x;
			y *= rkVector.y;
			z *= rkVector.z;
			w *= rkVector.w;

			return *this;
		}

		inline Vector4& operator /= ( const f32 fScalar )
		{
			assert( fScalar != 0.0 );

			f32 fInv = 1.0f / fScalar;

			x *= fInv;
			y *= fInv;
			z *= fInv;
			w *= fInv;

			return *this;
		}

		inline Vector4& operator /= ( const Vector4& rkVector )
		{
			x /= rkVector.x;
			y /= rkVector.y;
			z /= rkVector.z;
			w /= rkVector.w;

			return *this;
		}

		/** Calculates the dot (scalar) product of this vector with another.
			@param
				vec Vector with which to calculate the dot product (together
				with this one).
			@returns
				A float representing the dot product value.
		*/
		inline f32 DotProduct(const Vector4& vec) const
		{
			return x * vec.x + y * vec.y + z * vec.z + w * vec.w;
		}
		
		/// Check whether this vector contains valid values
		inline bool IsNaN() const
		{
			return Maths::IsNaN(x) || Maths::IsNaN(y) || Maths::IsNaN(z) || Maths::IsNaN(w);
		}
		
		/** Function for writing to a stream.
		*/
		inline friend std::ostream& operator<<( std::ostream& o, const Vector4& v )
		{
			o << "Vector4(" << v.x << "," << v.y << "," << v.z << "," << v.w << ")";
			return o;
		}

		inline friend std::istream& operator>>(std::istream& i, Vector4& v)
		{
			using namespace Utils::String;
			std::string temp;
			i >> temp;
			if(!Utils::String::VerifyConstructorAndExtractParameters(temp,"Vector4") ||
				!ConvertAndAssign(temp, v.x, v.y, v.z, v.w))
			{
				v = Vector4::ZERO;
				i.setstate(std::ios_base::failbit);
			}
			return i;
		}
		
		// special
		static const Vector4 ZERO;

#ifdef ECHO_OGRE_SUPPORT
		//Ogre support
		inline operator Ogre::Vector4() const
		{
			return Ogre::Vector4(x, y, z, w);
		}
		inline Vector4& operator=(const Ogre::Vector4& rhs)
		{
			x = rhs.x;
			y = rhs.y;
			z = rhs.z;
			return *this;
		}
		inline Vector4(const Ogre::Vector4& rhs)
		{
			x = rhs.x;
			y = rhs.y;
			z = rhs.z;
			w = rhs.w;
		}
#endif
	};
	/** @} */
	/** @} */
}
#endif

