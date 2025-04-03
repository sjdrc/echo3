#ifndef _ECHO_QUATERNION_H_
#define _ECHO_QUATERNION_H_

#include <echo/Types.h>
#include <echo/Maths/EchoMaths.h>
#include <echo/Maths/Vector3.h>
#include <echo/Maths/Matrix3.h>
#include <echo/Util/StringUtils.h>
#include <ostream>
#include <assert.h>

#ifdef ECHO_BULLET_SUPPORT
#include <LinearMath/btQuaternion.h>
#endif
#ifdef ECHO_OGRE_SUPPORT
#include <OgreQuaternion.h>
#endif
namespace Echo
{
	class Matrix4;

	/** Implementation of a Quaternion, i.e. a rotation around an axis.
	 */
	class Quaternion
	{
	public:
		/**
		 * Create a Quaternion from Euler angles.
		 * @see SetEuler
		 * @return A quaternion set from the specified Euler angles.
		 */
		Quaternion(Radian yaw, Radian pitch, Radian roll)
		{
			SetEuler(yaw,pitch,roll);
		}
		inline Quaternion(f32 fX = 0.0, f32 fY = 0.0, f32 fZ = 0.0, f32 fW = 1.0)
		{
			w = fW;
			x = fX;
			y = fY;
			z = fZ;
		}
		/// Construct a quaternion from a rotation matrix
		inline Quaternion(const Matrix3& rot)
		{
			this->FromRotationMatrix(rot);
		}
		/// Construct a quaternion from an angle/axis
		inline Quaternion(const Radian& rfAngle, const Vector3& rkAxis)
		{
			this->FromAngleAxis(rfAngle, rkAxis);
		}
		/// Construct a quaternion from 3 orthonormal local axes
		inline Quaternion(const Vector3& xaxis, const Vector3& yaxis, const Vector3& zaxis)
		{
			this->FromAxes(xaxis, yaxis, zaxis);
		}
		/// Construct a quaternion from 3 orthonormal local axes
		inline Quaternion(const Vector3* akAxis)
		{
			this->FromAxes(akAxis);
		}
		/// Construct a quaternion from 4 manual w/x/y/z values
		inline Quaternion(f32* valptr)
		{
			w = valptr[0];
			x = valptr[1];
			y = valptr[2];
			z = valptr[3];
		}

		/** Exchange the contents of this quaternion with another. 
		 */
		inline void Swap(Quaternion& other)
		{
			std::swap(w, other.w);
			std::swap(x, other.x);
			std::swap(y, other.y);
			std::swap(z, other.z);
		}

		/// Array accessor operator
		inline f32 operator [] ( const size_t i ) const
		{
			assert( i < 4 );

			return *(&w+i);
		}

		/// Array accessor operator
		inline f32& operator [] ( const size_t i )
		{
			assert( i < 4 );

			return *(&w+i);
		}

		void SetAlongVector(const Vector3& vect, const Vector3& up);

		f32 Dot(const Quaternion& rkQ) const; // dot product

		void FromRotationMatrix(const Matrix3& kRot);
		void ToRotationMatrix(Matrix3& kRot) const;
		void FromAngleAxis(const Radian& rfAngle, const Vector3& rkAxis);
		void ToAngleAxis(Radian& rfAngle, Vector3& rkAxis) const;
		inline void ToAngleAxis(Degree& dAngle, Vector3& rkAxis) const
		{
			Radian rAngle;
			ToAngleAxis ( rAngle, rkAxis );
			dAngle = rAngle;
		}
		void FromAxes(const Vector3* akAxis);
		void FromAxes(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis);
		void ToAxes(Vector3* akAxis) const;
		void ToAxes(Vector3& xAxis, Vector3& yAxis, Vector3& zAxis) const;
		/// Get the local x-axis
		Vector3 XAxis(void) const;
		/// Get the local y-axis
		Vector3 YAxis(void) const;
		/// Get the local z-axis
		Vector3 ZAxis(void) const;

		inline Quaternion& operator=(const Quaternion& rkQ)
		{
			w = rkQ.w;
			x = rkQ.x;
			y = rkQ.y;
			z = rkQ.z;
			return *this;
		}
		Quaternion operator+(const Quaternion& rkQ) const;
		Quaternion operator-(const Quaternion& rkQ) const;
		Quaternion operator*(const Quaternion& rkQ) const;
		Quaternion operator*(f32 fScalar) const;
		friend Quaternion operator*(f32 fScalar,
									const Quaternion& rkQ);
		Quaternion operator-() const;
		inline bool operator==(const Quaternion& rhs) const
		{
			return (rhs.x == x) && (rhs.y == y) &&
				(rhs.z == z) && (rhs.w == w);
		}
		inline bool operator!=(const Quaternion& rhs) const
		{
			return !operator==(rhs);
		}

		f32 LengthSquared() const;
		/// Normalises this quaternion, and returns the previous length
		f32 Normalise();
		Quaternion Normalised() const
		{
			Quaternion n = *this;
			n.Normalise();
			return n;
		}
		Quaternion Inverse() const; // apply to non-zero quaternion
		Quaternion UnitInverse() const; // apply to unit-length quaternion
		Quaternion Exp() const;
		Quaternion Log() const;

		// rotation of a vector by a quaternion
		Vector3 operator*(const Vector3& rkVector) const;

   		/** Calculate the local roll element of this quaternion.
		@param reprojectAxis By default the method returns the 'intuitive' result
			that is, if you projected the local Y of the quaternion onto the X and
			Y axes, the angle between them is returned. If set to false though, the
			result is the actual yaw that will be used to implement the quaternion,
			which is the shortest possible path to get to the same orientation and 
			may involve less axial rotation. 
		*/
		Radian GetRoll(bool reprojectAxis = true) const;
   		/** Calculate the local pitch element of this quaternion
		@param reprojectAxis By default the method returns the 'intuitive' result
			that is, if you projected the local Z of the quaternion onto the X and
			Y axes, the angle between them is returned. If set to true though, the
			result is the actual yaw that will be used to implement the quaternion,
			which is the shortest possible path to get to the same orientation and 
			may involve less axial rotation. 
		*/
		Radian GetPitch(bool reprojectAxis = true) const;
   		/** Calculate the local yaw element of this quaternion
		@param reprojectAxis By default the method returns the 'intuitive' result
			that is, if you projected the local Z of the quaternion onto the X and
			Z axes, the angle between them is returned. If set to true though, the
			result is the actual yaw that will be used to implement the quaternion,
			which is the shortest possible path to get to the same orientation and 
			may involve less axial rotation. 
		*/
		Radian GetYaw(bool reprojectAxis = true) const;		
		/// Equality with tolerance (tolerance is max angle difference)
		bool Equals(const Quaternion& rhs, const Radian& tolerance) const;

		inline void Set(f32 ix, f32 iy, f32 iz, f32 iw)
		{
			x=ix;	y=iy;	z=iz;	w=iw;
		}
		inline void SetX(f32 ix){x=ix;}
		inline void SetY(f32 iy){y=iy;}
		inline void SetZ(f32 iz){z=iz;}
		inline void SetW(f32 iw){w=iw;}

		inline void SetZero()					//Set all Elements to 0
		{
			x=y=z=w=0;
		}

		inline void Identity()				//Make the quaternion a (multiplication) Identity EQuaternion
		{
			x=y=z=0.0f;
			w=1.0f;
		}

		inline bool IsIdentity() const //Is the quaternion a (multiplication) Identity EQuaternion?
		{
			return ((w==1.0f) && (x==0.0f) && (y==0.0f) && (z==0.0f));
		}

		inline bool IsZero() const // Returns true if all elements of the quaternion are zero
		{
			if(x!=0)
				return false;
			if(y!=0)
				return false;
			if(z!=0)
				return false;
			if(w!=0)
				return false;
			return (true);
		}

		inline const f32& GetX() const {return x;}
		inline const f32& GetY() const {return y;}
		inline const f32& GetZ() const {return z;}
		inline const f32& GetW() const {return w;}

		inline void SetEuler(const Radian& yaw, const Radian& pitch, const Radian& roll)
		{
			f32 halfYaw = yaw.ValueRadians() * 0.5f; 
			f32 halfPitch = pitch.ValueRadians() * 0.5f;
			f32 halfRoll = roll.ValueRadians() * 0.5f;
			f32 cosYaw = cos(halfYaw);
			f32 sinYaw = sin(halfYaw);
			f32 cosPitch = cos(halfPitch);
			f32 sinPitch = sin(halfPitch);
			f32 cosRoll = cos(halfRoll);
			f32 sinRoll = sin(halfRoll);
			Set(cosRoll * sinPitch * cosYaw + sinRoll * cosPitch * sinYaw,
				cosRoll * cosPitch * sinYaw - sinRoll * sinPitch * cosYaw,
				sinRoll * cosPitch * cosYaw - cosRoll * sinPitch * sinYaw,
				cosRoll * cosPitch * cosYaw + sinRoll * sinPitch * sinYaw);
		}
		
		// spherical linear interpolation
		static Quaternion Slerp(f32 fT, const Quaternion& rkP,
								const Quaternion& rkQ, bool shortestPath = false);

		static Quaternion SlerpExtraSpins(f32 fT,
										  const Quaternion& rkP, const Quaternion& rkQ,
			int iExtraSpins);

		// setup for spherical quadratic interpolation
		static void Intermediate(const Quaternion& rkQ0,
								 const Quaternion& rkQ1, const Quaternion& rkQ2,
								 Quaternion& rka, Quaternion& rkB);

		// spherical quadratic interpolation
		static Quaternion Squad(f32 fT, const Quaternion& rkP,
								const Quaternion& rkA, const Quaternion& rkB,
								const Quaternion& rkQ, bool shortestPath = false);

		// normalised linear interpolation - faster but less accurate (non-constant rotation velocity)
		Quaternion Nlerp(const Quaternion& rkQ, f32 fT, bool shortestPath = false) const;

		// cutoff for sine near zero
		static const f32 ms_fEpsilon;

		// special values
		static const Quaternion ZERO;
		static const Quaternion IDENTITY;

		f32 w, x, y, z;

		/// Check whether this quaternion contains valid values
		bool IsNaN() const
		{
			return Maths::IsNaN(x) || Maths::IsNaN(y) || Maths::IsNaN(z) || Maths::IsNaN(w);
		}

		/** Function for writing to a stream. Outputs "Quaternion(x, y, z, w)" with x,y,z,w
			being the member values of the quaternion.
		*/
		inline friend std::ostream& operator<<(std::ostream& o, const Quaternion& q)
		{
			o << "Quaternion(" << q.x << "," << q.y << "," << q.z << "," << q.w << ")";
			return o;
		}

		inline friend std::istream& operator>>(std::istream& i, Quaternion& v)
		{
			using namespace Utils::String;
			std::string temp;
			i >> temp;
			if(!VerifyConstructorAndExtractParameters(temp,"Quaternion") ||
				!ConvertAndAssign(temp, v.x, v.y, v.z, v.w))
			{
				v = Quaternion::IDENTITY;
				i.setstate(std::ios_base::failbit);
			}
			return i;
		}
	};
}
#endif 
