#ifndef _ECHO_MATRIX4_H_
#define _ECHO_MATRIX4_H_

#include <echo/Maths/Quaternion.h>
#include <echo/Maths/Vector3.h>
#include <echo/Maths/Vector4.h>
#include <echo/Maths/Matrix3.h>
#include <echo/Maths/Plane.h>

namespace Echo
{
	/** \addtogroup Core
	*  @{
	*/
	/** \addtogroup EMaths
	*  @{
	*/
	/** Class encapsulating a standard 4x4 homogeneous matrix.
		@remarks
			OGRE uses column vectors when applying matrix multiplications,
			This means a vector is represented as a single column, 4-row
			matrix. This has the effect that the transformations implemented
			by the matrices happens right-to-left e.g. if vector V is to be
			transformed by M1 then M2 then M3, the calculation would be
			M3 * M2 * M1 * V. The order that matrices are concatenated is
			vital since matrix multiplication is not commutative, i.e. you
			can get a different result if you concatenate in the wrong order.
		@par
			The use of column vectors and right-to-left ordering is the
			standard in most mathematical texts, and is the same as used in
			OpenGL. It is, however, the opposite of Direct3D, which has
			inexplicably chosen to differ from the accepted standard and uses
			row vectors and left-to-right matrix multiplication.
		@par
			OGRE deals with the differences between D3D and OpenGL etc.
			internally when operating through different render systems. OGRE
			users only need to conform to standard maths conventions, i.e.
			right-to-left matrix multiplication, (OGRE transposes matrices it
			passes to D3D to compensate).
			@par
			The generic form M * V which shows the layout of the matrix 
			entries is shown below:
			<pre>
				[ m[0][0]  m[0][1]  m[0][2]  m[0][3] ]   {x}
				| m[1][0]  m[1][1]  m[1][2]  m[1][3] | * {y}
				| m[2][0]  m[2][1]  m[2][2]  m[2][3] |   {z}
				[ m[3][0]  m[3][1]  m[3][2]  m[3][3] ]   {1}
			</pre>
	 */
	class Matrix4
	{
	protected:
		/// The matrix entries, indexed by [row][col].
		union {
			f32 m[4][4];
			f32 _m[16];
		};
	public:
		/** Default constructor.
			@note
				It does <b>NOT</b> initialize the matrix for efficiency.
		 */
		inline Matrix4()
		{
		}

		inline Matrix4(
			f32 m00, f32 m01, f32 m02, f32 m03,
			f32 m10, f32 m11, f32 m12, f32 m13,
			f32 m20, f32 m21, f32 m22, f32 m23,
			f32 m30, f32 m31, f32 m32, f32 m33 )
		{
			m[0][0] = m00;
			m[0][1] = m01;
			m[0][2] = m02;
			m[0][3] = m03;
			m[1][0] = m10;
			m[1][1] = m11;
			m[1][2] = m12;
			m[1][3] = m13;
			m[2][0] = m20;
			m[2][1] = m21;
			m[2][2] = m22;
			m[2][3] = m23;
			m[3][0] = m30;
			m[3][1] = m31;
			m[3][2] = m32;
			m[3][3] = m33;
		}
		
		/**
		 * Construct from a buffer of 16 entries in row major form.
		 * @note No checking is performed on the buffer, it is required that the buffer
		 * contains enough data.
		 */
		inline Matrix4(f32* buffer)
		{
			m[0][0] = buffer[0];
			m[0][1] = buffer[1];
			m[0][2] = buffer[2];
			m[0][3] = buffer[3];
			m[1][0] = buffer[4];
			m[1][1] = buffer[5];
			m[1][2] = buffer[6];
			m[1][3] = buffer[7];
			m[2][0] = buffer[8];
			m[2][1] = buffer[9];
			m[2][2] = buffer[10];
			m[2][3] = buffer[11];
			m[3][0] = buffer[12];
			m[3][1] = buffer[13];
			m[3][2] = buffer[14];
			m[3][3] = buffer[15];
		}
		
		/** Creates a standard 4x4 transformation matrix with a zero translation part from a rotation/scaling 3x3 matrix.
		 */

		inline Matrix4(const Matrix3& m3x3)
		{
		  operator=(IDENTITY);
		  operator=(m3x3);
		}

		/** Creates a standard 4x4 transformation matrix with a zero translation part from a rotation/scaling EQuaternion.
		 */

		inline Matrix4(const Quaternion& rot)
		{
			Matrix3 m3x3;
			rot.ToRotationMatrix(m3x3);
			operator=(IDENTITY);
			operator=(m3x3);
		}
		

		/** Exchange the contents of this matrix with another. 
		 */
		inline void swap(Matrix4& other)
		{
			std::swap(m[0][0], other.m[0][0]);
			std::swap(m[0][1], other.m[0][1]);
			std::swap(m[0][2], other.m[0][2]);
			std::swap(m[0][3], other.m[0][3]);
			std::swap(m[1][0], other.m[1][0]);
			std::swap(m[1][1], other.m[1][1]);
			std::swap(m[1][2], other.m[1][2]);
			std::swap(m[1][3], other.m[1][3]);
			std::swap(m[2][0], other.m[2][0]);
			std::swap(m[2][1], other.m[2][1]);
			std::swap(m[2][2], other.m[2][2]);
			std::swap(m[2][3], other.m[2][3]);
			std::swap(m[3][0], other.m[3][0]);
			std::swap(m[3][1], other.m[3][1]);
			std::swap(m[3][2], other.m[3][2]);
			std::swap(m[3][3], other.m[3][3]);
		}

		inline f32* operator [] ( size_t iRow )
		{
			assert( iRow < 4 );
			return m[iRow];
		}

		inline const f32 *operator [] ( size_t iRow ) const
		{
			assert( iRow < 4 );
			return m[iRow];
		}

		inline Matrix4 Concatenate(const Matrix4 &m2) const
		{
			Matrix4 r;
			r.m[0][0] = m[0][0] * m2.m[0][0] + m[0][1] * m2.m[1][0] + m[0][2] * m2.m[2][0] + m[0][3] * m2.m[3][0];
			r.m[0][1] = m[0][0] * m2.m[0][1] + m[0][1] * m2.m[1][1] + m[0][2] * m2.m[2][1] + m[0][3] * m2.m[3][1];
			r.m[0][2] = m[0][0] * m2.m[0][2] + m[0][1] * m2.m[1][2] + m[0][2] * m2.m[2][2] + m[0][3] * m2.m[3][2];
			r.m[0][3] = m[0][0] * m2.m[0][3] + m[0][1] * m2.m[1][3] + m[0][2] * m2.m[2][3] + m[0][3] * m2.m[3][3];

			r.m[1][0] = m[1][0] * m2.m[0][0] + m[1][1] * m2.m[1][0] + m[1][2] * m2.m[2][0] + m[1][3] * m2.m[3][0];
			r.m[1][1] = m[1][0] * m2.m[0][1] + m[1][1] * m2.m[1][1] + m[1][2] * m2.m[2][1] + m[1][3] * m2.m[3][1];
			r.m[1][2] = m[1][0] * m2.m[0][2] + m[1][1] * m2.m[1][2] + m[1][2] * m2.m[2][2] + m[1][3] * m2.m[3][2];
			r.m[1][3] = m[1][0] * m2.m[0][3] + m[1][1] * m2.m[1][3] + m[1][2] * m2.m[2][3] + m[1][3] * m2.m[3][3];

			r.m[2][0] = m[2][0] * m2.m[0][0] + m[2][1] * m2.m[1][0] + m[2][2] * m2.m[2][0] + m[2][3] * m2.m[3][0];
			r.m[2][1] = m[2][0] * m2.m[0][1] + m[2][1] * m2.m[1][1] + m[2][2] * m2.m[2][1] + m[2][3] * m2.m[3][1];
			r.m[2][2] = m[2][0] * m2.m[0][2] + m[2][1] * m2.m[1][2] + m[2][2] * m2.m[2][2] + m[2][3] * m2.m[3][2];
			r.m[2][3] = m[2][0] * m2.m[0][3] + m[2][1] * m2.m[1][3] + m[2][2] * m2.m[2][3] + m[2][3] * m2.m[3][3];

			r.m[3][0] = m[3][0] * m2.m[0][0] + m[3][1] * m2.m[1][0] + m[3][2] * m2.m[2][0] + m[3][3] * m2.m[3][0];
			r.m[3][1] = m[3][0] * m2.m[0][1] + m[3][1] * m2.m[1][1] + m[3][2] * m2.m[2][1] + m[3][3] * m2.m[3][1];
			r.m[3][2] = m[3][0] * m2.m[0][2] + m[3][1] * m2.m[1][2] + m[3][2] * m2.m[2][2] + m[3][3] * m2.m[3][2];
			r.m[3][3] = m[3][0] * m2.m[0][3] + m[3][1] * m2.m[1][3] + m[3][2] * m2.m[2][3] + m[3][3] * m2.m[3][3];

			return r;
		}

		/** Matrix concatenation using '*'.
		 */
		inline Matrix4 operator *(const Matrix4 &m2) const
		{
			return Concatenate( m2 );
		}

		/** Vector transformation using '*'.
			@remarks
				Transforms the given 3-D vector by the matrix, projecting the 
				result back into <i>w</i> = 1.
			@note
				This means that the initial <i>w</i> is considered to be 1.0,
				and then all the tree elements of the resulting 3-D vector are
				divided by the resulting <i>w</i>.
		 */
		inline Vector3 operator *(const Vector3 &v) const
		{
			Vector3 r;

			f32 fInvW = 1.0f / ( m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] );

			r.x = ( m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] ) * fInvW;
			r.y = ( m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] ) * fInvW;
			r.z = ( m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] ) * fInvW;

			return r;
		}
		inline Vector4 operator *(const Vector4& v) const
		{
			return Vector4(
				m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w, 
				m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w,
				m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w,
				m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w
				);
		}
		inline Plane operator *(const Plane& p) const
		{
			Plane ret;
			Matrix4 invTrans = Inverse().Transpose();
			Vector4 v4(p.normal.x, p.normal.y, p.normal.z, p.d);
			v4 = invTrans * v4;
			ret.normal.x = v4.x; 
			ret.normal.y = v4.y; 
			ret.normal.z = v4.z;
			ret.d = v4.w / ret.normal.Normalise();

			return ret;
		}


		/** Matrix addition.
		 */
		inline Matrix4 operator +(const Matrix4 &m2) const
		{
			Matrix4 r;

			r.m[0][0] = m[0][0] + m2.m[0][0];
			r.m[0][1] = m[0][1] + m2.m[0][1];
			r.m[0][2] = m[0][2] + m2.m[0][2];
			r.m[0][3] = m[0][3] + m2.m[0][3];

			r.m[1][0] = m[1][0] + m2.m[1][0];
			r.m[1][1] = m[1][1] + m2.m[1][1];
			r.m[1][2] = m[1][2] + m2.m[1][2];
			r.m[1][3] = m[1][3] + m2.m[1][3];

			r.m[2][0] = m[2][0] + m2.m[2][0];
			r.m[2][1] = m[2][1] + m2.m[2][1];
			r.m[2][2] = m[2][2] + m2.m[2][2];
			r.m[2][3] = m[2][3] + m2.m[2][3];

			r.m[3][0] = m[3][0] + m2.m[3][0];
			r.m[3][1] = m[3][1] + m2.m[3][1];
			r.m[3][2] = m[3][2] + m2.m[3][2];
			r.m[3][3] = m[3][3] + m2.m[3][3];

			return r;
		}

		/** Matrix subtraction.
		 */
		inline Matrix4 operator -(const Matrix4 &m2) const
		{
			Matrix4 r;
			r.m[0][0] = m[0][0] - m2.m[0][0];
			r.m[0][1] = m[0][1] - m2.m[0][1];
			r.m[0][2] = m[0][2] - m2.m[0][2];
			r.m[0][3] = m[0][3] - m2.m[0][3];

			r.m[1][0] = m[1][0] - m2.m[1][0];
			r.m[1][1] = m[1][1] - m2.m[1][1];
			r.m[1][2] = m[1][2] - m2.m[1][2];
			r.m[1][3] = m[1][3] - m2.m[1][3];

			r.m[2][0] = m[2][0] - m2.m[2][0];
			r.m[2][1] = m[2][1] - m2.m[2][1];
			r.m[2][2] = m[2][2] - m2.m[2][2];
			r.m[2][3] = m[2][3] - m2.m[2][3];

			r.m[3][0] = m[3][0] - m2.m[3][0];
			r.m[3][1] = m[3][1] - m2.m[3][1];
			r.m[3][2] = m[3][2] - m2.m[3][2];
			r.m[3][3] = m[3][3] - m2.m[3][3];

			return r;
		}

		/** Tests 2 matrices for equality.
		 */
		inline bool operator ==(const Matrix4& m2) const
		{
			if( 
				m[0][0] != m2.m[0][0] || m[0][1] != m2.m[0][1] || m[0][2] != m2.m[0][2] || m[0][3] != m2.m[0][3] ||
				m[1][0] != m2.m[1][0] || m[1][1] != m2.m[1][1] || m[1][2] != m2.m[1][2] || m[1][3] != m2.m[1][3] ||
				m[2][0] != m2.m[2][0] || m[2][1] != m2.m[2][1] || m[2][2] != m2.m[2][2] || m[2][3] != m2.m[2][3] ||
				m[3][0] != m2.m[3][0] || m[3][1] != m2.m[3][1] || m[3][2] != m2.m[3][2] || m[3][3] != m2.m[3][3] )
				return false;
			return true;
		}

		/** Tests 2 matrices for inequality.
		 */
		inline bool operator !=(const Matrix4& m2) const
		{
			if( 
				m[0][0] != m2.m[0][0] || m[0][1] != m2.m[0][1] || m[0][2] != m2.m[0][2] || m[0][3] != m2.m[0][3] ||
				m[1][0] != m2.m[1][0] || m[1][1] != m2.m[1][1] || m[1][2] != m2.m[1][2] || m[1][3] != m2.m[1][3] ||
				m[2][0] != m2.m[2][0] || m[2][1] != m2.m[2][1] || m[2][2] != m2.m[2][2] || m[2][3] != m2.m[2][3] ||
				m[3][0] != m2.m[3][0] || m[3][1] != m2.m[3][1] || m[3][2] != m2.m[3][2] || m[3][3] != m2.m[3][3] )
				return true;
			return false;
		}

		/** Assignment from 3x3 matrix.
		 */
		inline void operator =(const Matrix3& mat3)
		{
			m[0][0] = mat3.m[0][0]; m[0][1] = mat3.m[0][1]; m[0][2] = mat3.m[0][2];
			m[1][0] = mat3.m[1][0]; m[1][1] = mat3.m[1][1]; m[1][2] = mat3.m[1][2];
			m[2][0] = mat3.m[2][0]; m[2][1] = mat3.m[2][1]; m[2][2] = mat3.m[2][2];
		}

		inline Matrix4 Transpose(void) const
		{
			return Matrix4(m[0][0], m[1][0], m[2][0], m[3][0],
						   m[0][1], m[1][1], m[2][1], m[3][1],
						   m[0][2], m[1][2], m[2][2], m[3][2],
						   m[0][3], m[1][3], m[2][3], m[3][3]);
		}

		/*
		-----------------------------------------------------------------------
		Translation Transformation
		-----------------------------------------------------------------------
		*/
		/** Sets the translation transformation part of the matrix.
		 */
		inline void SetTranslation(const Vector3& v)
		{
			m[0][3] = v.x;
			m[1][3] = v.y;
			m[2][3] = v.z;
		}

		/** Extracts the translation transformation part of the matrix.
		 */
		inline Vector3 GetTranslation() const
		{
			return Vector3(m[0][3], m[1][3], m[2][3]);
		}
		

		/** Builds a translation matrix
		 */
		inline void Translation(const Vector3& v)
		{
			m[0][0] = 1.0; m[0][1] = 0.0; m[0][2] = 0.0; m[0][3] = v.x;
			m[1][0] = 0.0; m[1][1] = 1.0; m[1][2] = 0.0; m[1][3] = v.y;
			m[2][0] = 0.0; m[2][1] = 0.0; m[2][2] = 1.0; m[2][3] = v.z;
			m[3][0] = 0.0; m[3][1] = 0.0; m[3][2] = 0.0; m[3][3] = 1.0;
		}

		inline void Translation( f32 tx, f32 ty, f32 tz )
		{
			m[0][0] = 1.0; m[0][1] = 0.0; m[0][2] = 0.0; m[0][3] = tx;
			m[1][0] = 0.0; m[1][1] = 1.0; m[1][2] = 0.0; m[1][3] = ty;
			m[2][0] = 0.0; m[2][1] = 0.0; m[2][2] = 1.0; m[2][3] = tz;
			m[3][0] = 0.0; m[3][1] = 0.0; m[3][2] = 0.0; m[3][3] = 1.0;
		}

		/** Gets a translation matrix.
		 */
		inline static Matrix4 GetTranslation(const Vector3& v)
		{
			Matrix4 r;

			r.m[0][0] = 1.0; r.m[0][1] = 0.0; r.m[0][2] = 0.0; r.m[0][3] = v.x;
			r.m[1][0] = 0.0; r.m[1][1] = 1.0; r.m[1][2] = 0.0; r.m[1][3] = v.y;
			r.m[2][0] = 0.0; r.m[2][1] = 0.0; r.m[2][2] = 1.0; r.m[2][3] = v.z;
			r.m[3][0] = 0.0; r.m[3][1] = 0.0; r.m[3][2] = 0.0; r.m[3][3] = 1.0;

			return r;
		}

		/** Gets a translation matrix - variation for not using a vector.
		 */
		inline static Matrix4 GetTranslation(f32 t_x, f32 t_y, f32 t_z)
		{
			Matrix4 r;

			r.m[0][0] = 1.0; r.m[0][1] = 0.0; r.m[0][2] = 0.0; r.m[0][3] = t_x;
			r.m[1][0] = 0.0; r.m[1][1] = 1.0; r.m[1][2] = 0.0; r.m[1][3] = t_y;
			r.m[2][0] = 0.0; r.m[2][1] = 0.0; r.m[2][2] = 1.0; r.m[2][3] = t_z;
			r.m[3][0] = 0.0; r.m[3][1] = 0.0; r.m[3][2] = 0.0; r.m[3][3] = 1.0;

			return r;
		}

		/*
		-----------------------------------------------------------------------
		Scale Transformation
		-----------------------------------------------------------------------
		*/
		/** Sets the scale part of the matrix.
		 */
		inline void SetScale(const Vector3& v)
		{
			m[0][0] = v.x;
			m[1][1] = v.y;
			m[2][2] = v.z;
		}

		/** Gets a scale matrix.
		 */
		inline static Matrix4 GetScale(const Vector3& v)
		{
			Matrix4 r;
			r.m[0][0] = v.x; r.m[0][1] = 0.0; r.m[0][2] = 0.0; r.m[0][3] = 0.0;
			r.m[1][0] = 0.0; r.m[1][1] = v.y; r.m[1][2] = 0.0; r.m[1][3] = 0.0;
			r.m[2][0] = 0.0; r.m[2][1] = 0.0; r.m[2][2] = v.z; r.m[2][3] = 0.0;
			r.m[3][0] = 0.0; r.m[3][1] = 0.0; r.m[3][2] = 0.0; r.m[3][3] = 1.0;

			return r;
		}

		/** Gets a scale matrix - variation for not using a vector.
		 */
		inline static Matrix4 GetScale(f32 s_x, f32 s_y, f32 s_z)
		{
			Matrix4 r;
			r.m[0][0] = s_x; r.m[0][1] = 0.0; r.m[0][2] = 0.0; r.m[0][3] = 0.0;
			r.m[1][0] = 0.0; r.m[1][1] = s_y; r.m[1][2] = 0.0; r.m[1][3] = 0.0;
			r.m[2][0] = 0.0; r.m[2][1] = 0.0; r.m[2][2] = s_z; r.m[2][3] = 0.0;
			r.m[3][0] = 0.0; r.m[3][1] = 0.0; r.m[3][2] = 0.0; r.m[3][3] = 1.0;

			return r;
		}

		/** Extracts the rotation / scaling part of the Matrix as a 3x3 matrix. 
		@param m3x3 Destination EMatrix3
		 */
		inline void Extract3x3Matrix(Matrix3& m3x3) const
		{
			m3x3.m[0][0] = m[0][0];
			m3x3.m[0][1] = m[0][1];
			m3x3.m[0][2] = m[0][2];
			m3x3.m[1][0] = m[1][0];
			m3x3.m[1][1] = m[1][1];
			m3x3.m[1][2] = m[1][2];
			m3x3.m[2][0] = m[2][0];
			m3x3.m[2][1] = m[2][1];
			m3x3.m[2][2] = m[2][2];

		}

		/** Determines if this matrix involves a scaling. */
		inline bool HasScale() const
		{
			// check magnitude of column vectors (==local axes)
			f32 t = m[0][0] * m[0][0] + m[1][0] * m[1][0] + m[2][0] * m[2][0];
			if(!Maths::RealEqual<f32>(t, 1.0, (f32) 1e-04))
				return true;
			t = m[0][1] * m[0][1] + m[1][1] * m[1][1] + m[2][1] * m[2][1];
			if(!Maths::RealEqual<f32>(t, 1.0, (f32) 1e-04))
				return true;
			t = m[0][2] * m[0][2] + m[1][2] * m[1][2] + m[2][2] * m[2][2];
			if(!Maths::RealEqual<f32>(t, 1.0, (f32) 1e-04))
				return true;

			return false;
		}

		/** Determines if this matrix involves a negative scaling. */
		inline bool HasNegativeScale() const
		{
			return Determinant() < 0;
		}

		/** Extracts the rotation / scaling part as a quaternion from the Matrix.
		 */
		inline Quaternion ExtractQuaternion() const
		{
			Matrix3 m3x3;
		  Extract3x3Matrix(m3x3);
			return Quaternion(m3x3);
			}

		static const Matrix4 ZERO;
		static const Matrix4 IDENTITY;
		/** Useful little matrix which takes 2D clipspace {-1, 1} to {0,1}
			and inverts the Y. */
		static const Matrix4 CLIPSPACE2DTOIMAGESPACE;

		inline Matrix4 operator*(f32 scalar) const
		{
			return Matrix4(
				scalar*m[0][0], scalar*m[0][1], scalar*m[0][2], scalar*m[0][3],
				scalar*m[1][0], scalar*m[1][1], scalar*m[1][2], scalar*m[1][3],
				scalar*m[2][0], scalar*m[2][1], scalar*m[2][2], scalar*m[2][3],
				scalar*m[3][0], scalar*m[3][1], scalar*m[3][2], scalar*m[3][3]);
		}

		/** Function for writing to a stream.
		*/
		inline friend std::ostream& operator <<(std::ostream& o, const Matrix4& m)
		{
			o << "Matrix4(" << m[0][0] << "," << m[0][1] << "," << m[0][2] << "," << m[0][3] << ","
							<< m[1][0] << "," << m[1][1] << "," << m[1][2] << "," << m[1][3] << ","
							<< m[2][0] << "," << m[2][1] << "," << m[2][2] << "," << m[2][3] << ","
							<< m[3][0] << "," << m[3][1] << "," << m[3][2] << "," << m[3][3] << ")";
			return o;
		}
		
		inline friend std::istream& operator >>(std::istream& i, Matrix4& v)
		{
			std::string temp;
			i >> temp;
			
			if(!Utils::String::VerifyConstructorAndExtractParameters(temp,"Matrix4"))
			{
				v = Matrix4::IDENTITY;
				i.setstate(std::ios_base::failbit);
				return i;
			}
			
			if(!Utils::String::ConvertAndAssign(temp,&(v.m[0][0]),16))
			{
				v = Matrix4::IDENTITY;
				i.setstate(std::ios_base::failbit);
			}
			return i;
		}
		

		Matrix4 Adjoint() const;
		f32 Determinant() const;
		Matrix4 Inverse() const;

		/** Building a EMatrix4 from orientation / scale / position.
		@remarks
			Transform is performed in the order scale, rotate, translation, i.e. translation is independent
			of orientation axes, scale does not affect size of translation, rotation and scaling are always
			centered on the origin.
		 */
		void MakeTransform(const Vector3& position, const Vector3& scale, const Quaternion& orientation);

		/** Building an inverse EMatrix4 from orientation / scale / position.
		@remarks
			As MakeTransform except it build the inverse given the same data as MakeTransform, so
			performing -translation, -rotate, 1/scale in that order.
		 */
		void MakeInverseTransform(const Vector3& position, const Vector3& scale, const Quaternion& orientation);

		/** Decompose a EMatrix4 to orientation / scale / position.
		 */
		void Decomposition(Vector3& position, Vector3& scale, Quaternion& orientation) const;

		/** Check whether or not the matrix is affine matrix.
			@remarks
				An affine matrix is a 4x4 matrix with row 3 equal to (0, 0, 0, 1),
				e.g. no projective coefficients.
		*/
		inline bool IsAffine(void) const
		{
			return m[3][0] == 0 && m[3][1] == 0 && m[3][2] == 0 && m[3][3] == 1;
		}

		/** Returns the inverse of the affine matrix.
			@note
				The matrix must be an affine matrix. @see EMatrix4::IsAffine.
		 */
		Matrix4 InverseAffine(void) const;

		/** Concatenate two affine matrices.
			@note
				The matrices must be affine matrix. @see EMatrix4::IsAffine.
		 */
		inline Matrix4 ConcatenateAffine(const Matrix4 &m2) const
		{
			assert(IsAffine() && m2.IsAffine());

			return Matrix4(
				m[0][0] * m2.m[0][0] + m[0][1] * m2.m[1][0] + m[0][2] * m2.m[2][0],
				m[0][0] * m2.m[0][1] + m[0][1] * m2.m[1][1] + m[0][2] * m2.m[2][1],
				m[0][0] * m2.m[0][2] + m[0][1] * m2.m[1][2] + m[0][2] * m2.m[2][2],
				m[0][0] * m2.m[0][3] + m[0][1] * m2.m[1][3] + m[0][2] * m2.m[2][3] + m[0][3],

				m[1][0] * m2.m[0][0] + m[1][1] * m2.m[1][0] + m[1][2] * m2.m[2][0],
				m[1][0] * m2.m[0][1] + m[1][1] * m2.m[1][1] + m[1][2] * m2.m[2][1],
				m[1][0] * m2.m[0][2] + m[1][1] * m2.m[1][2] + m[1][2] * m2.m[2][2],
				m[1][0] * m2.m[0][3] + m[1][1] * m2.m[1][3] + m[1][2] * m2.m[2][3] + m[1][3],

				m[2][0] * m2.m[0][0] + m[2][1] * m2.m[1][0] + m[2][2] * m2.m[2][0],
				m[2][0] * m2.m[0][1] + m[2][1] * m2.m[1][1] + m[2][2] * m2.m[2][1],
				m[2][0] * m2.m[0][2] + m[2][1] * m2.m[1][2] + m[2][2] * m2.m[2][2],
				m[2][0] * m2.m[0][3] + m[2][1] * m2.m[1][3] + m[2][2] * m2.m[2][3] + m[2][3],

				0, 0, 0, 1);
		}

		/** 3-D Vector transformation specially for an affine matrix.
			@remarks
				Transforms the given 3-D vector by the matrix, projecting the 
				result back into <i>w</i> = 1.
			@note
				The matrix must be an affine matrix. @see EMatrix4::IsAffine.
		 */
		inline Vector3 TransformAffine(const Vector3& v) const
		{
			assert(IsAffine());

			return Vector3(
					m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3], 
					m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3],
					m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3]);
		}

		/** 4-D Vector transformation specially for an affine matrix.
			@note
				The matrix must be an affine matrix. @see EMatrix4::IsAffine.
		 */
		inline Vector4 TransformAffine(const Vector4& v) const
		{
			assert(IsAffine());

			return Vector4(
				m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w, 
				m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w,
				m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w,
				v.w);
		}
	};

	/* Removed from EVector4 and made a non-member here because otherwise
	   OgreMatrix4.h and OgreVector4.h have to try to include and inline each 
	   other, which frankly doesn't work ;)
	 */
	inline Vector4 operator *(const Vector4& v, const Matrix4& mat)
	{
		return Vector4(
			v.x*mat[0][0] + v.y*mat[1][0] + v.z*mat[2][0] + v.w*mat[3][0],
			v.x*mat[0][1] + v.y*mat[1][1] + v.z*mat[2][1] + v.w*mat[3][1],
			v.x*mat[0][2] + v.y*mat[1][2] + v.z*mat[2][2] + v.w*mat[3][2],
			v.x*mat[0][3] + v.y*mat[1][3] + v.z*mat[2][3] + v.w*mat[3][3]
			);
	}
	/** @} */
	/** @} */
}
#endif
