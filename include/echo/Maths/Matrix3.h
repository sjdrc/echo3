#ifndef _ECHO_MATRIX3_H_
#define _ECHO_MATRIX3_H_

#include <echo/Maths/Vector3.h>
#include <echo/Util/StringUtils.h>
#include <algorithm>

namespace Echo
{

// NB All code adapted from Wild Magic 0.2 Matrix math (free source code)
// http://www.geometrictools.com/

// NOTE.  The (x,y,z) coordinate system is assumed to be right-handed.
// Coordinate axis rotation matrices are of the form
//   RX =	1	   0	   0
//		   0	 cos(t) -sin(t)
//		   0	 sin(t)  cos(t)
// where t > 0 indicates a counterclockwise rotation in the yz-plane
//   RY =  cos(t)	0	 sin(t)
//		   0	   1	   0
//		-sin(t)	0	 cos(t)
// where t > 0 indicates a counterclockwise rotation in the zx-plane
//   RZ =  cos(t) -sin(t)	0
//		 sin(t)  cos(t)	0
//		   0	   0	   1
// where t > 0 indicates a counterclockwise rotation in the xy-plane.

	/** \addtogroup Core
	*  @{
	*/
	/** \addtogroup Maths
	*  @{
	*/
	/** A 3x3 matrix which can represent rotations around axes.
		@note
			<b>All the code is adapted from the Wild Magic 0.2 Matrix
			library (http://www.geometrictools.com/).</b>
		@par
			The coordinate system is assumed to be <b>right-handed</b>.
	 */
	template< typename T >
	class Matrix3Generic
	{
	public:
		/** Default constructor.
			@note
				It does <b>NOT</b> initialize the matrix for efficiency.
		 */
		inline Matrix3Generic()
		{
		}
		inline explicit Matrix3Generic(const T arr[3][3])
		{
			m[0][0] = arr[0][0];
			m[0][1] = arr[0][1];
			m[0][2] = arr[0][2];
			m[1][0] = arr[1][0];
			m[1][1] = arr[1][1];
			m[1][2] = arr[1][2];
			m[2][0] = arr[2][0];
			m[2][1] = arr[2][1];
			m[2][2] = arr[2][2];
		}
		inline Matrix3Generic(const Matrix3Generic<T>& rhs)
		{
			m[0][0] = rhs[0][0];
			m[0][1] = rhs[0][1];
			m[0][2] = rhs[0][2];
			m[1][0] = rhs[1][0];
			m[1][1] = rhs[1][1];
			m[1][2] = rhs[1][2];
			m[2][0] = rhs[2][0];
			m[2][1] = rhs[2][1];
			m[2][2] = rhs[2][2];
		}
		Matrix3Generic(T fEntry00, T fEntry01, T fEntry02,
					T fEntry10, T fEntry11, T fEntry12,
					T fEntry20, T fEntry21, T fEntry22)
		{
			m[0][0] = fEntry00;
			m[0][1] = fEntry01;
			m[0][2] = fEntry02;
			m[1][0] = fEntry10;
			m[1][1] = fEntry11;
			m[1][2] = fEntry12;
			m[2][0] = fEntry20;
			m[2][1] = fEntry21;
			m[2][2] = fEntry22;
		}
		
		/**
		 * Construct from a buffer of 9 entries in row major form.
		 * @note No checking is performed on the buffer, it is required that the buffer
		 * contains enough data.
		 */
		inline Matrix3Generic(T* buffer)
		{
			m[0][0] = buffer[0];
			m[0][1] = buffer[1];
			m[0][2] = buffer[2];
			m[1][0] = buffer[3];
			m[1][1] = buffer[4];
			m[1][2] = buffer[5];
			m[2][0] = buffer[6];
			m[2][1] = buffer[7];
			m[2][2] = buffer[8];
		}		

		/** Exchange the contents of this matrix with another. 
		 */
		inline void Swap(Matrix3Generic<T>& other)
		{
			std::swap(m[0][0], other.m[0][0]);
			std::swap(m[0][1], other.m[0][1]);
			std::swap(m[0][2], other.m[0][2]);
			std::swap(m[1][0], other.m[1][0]);
			std::swap(m[1][1], other.m[1][1]);
			std::swap(m[1][2], other.m[1][2]);
			std::swap(m[2][0], other.m[2][0]);
			std::swap(m[2][1], other.m[2][1]);
			std::swap(m[2][2], other.m[2][2]);
		}

		// member access, allows use of construct mat[r][c]
		inline T* operator[] (size_t iRow) const
		{
			return (T*)m[iRow];
		}
		
		Vector3Generic<T> GetColumn(size_t col) const
		{
			assert( 0 <= col && col < 3 );
			return Vector3Generic<T>(m[0][col], m[1][col], m[2][col]);
		}
		
		void SetColumn(size_t col, const Vector3Generic<T>& vec)
		{
			assert( 0 <= col && col < 3 );
			m[0][col] = vec.x;
			m[1][col] = vec.y;
			m[2][col] = vec.z;
		}
		
		void FromAxes(const Vector3Generic<T>& xAxis, const Vector3Generic<T>& yAxis, const Vector3Generic<T>& zAxis)
		{
			SetColumn(0,xAxis);
			SetColumn(1,yAxis);
			SetColumn(2,zAxis);
		}

		// assignment and comparison
		inline Matrix3Generic<T>& operator=(const Matrix3Generic<T>& rhs)
		{
			m[0][0] = rhs[0][0];
			m[0][1] = rhs[0][1];
			m[0][2] = rhs[0][2];
			m[1][0] = rhs[1][0];
			m[1][1] = rhs[1][1];
			m[1][2] = rhs[1][2];
			m[2][0] = rhs[2][0];
			m[2][1] = rhs[2][1];
			m[2][2] = rhs[2][2];
			return *this;
		}
		bool operator==(const Matrix3Generic<T>& rhs) const
		{
			for (Size row = 0; row < 3; row++)
			{
				for (Size col = 0; col < 3; col++)
				{
					if ( m[row][col] != rhs.m[row][col] )
						return false;
				}
			}

			return true;
		}
		
		inline bool operator!=(const Matrix3Generic<T>& rkMatrix) const
		{
			return !operator==(rkMatrix);
		}

		// arithmetic operations
		Matrix3Generic<T> operator+(const Matrix3Generic<T>& rhs) const
		{
			Matrix3Generic<T> sum;
			for (Size row = 0; row < 3; row++)
			{
				for (Size col = 0; col < 3; col++)
				{
					sum.m[row][col] = m[row][col] + rhs.m[row][col];
				}
			}
			return sum;
		}
		
		Matrix3Generic<T> operator-(const Matrix3Generic<T>& rhs) const
		{
			Matrix3Generic<T> diff;
			for (Size row = 0; row < 3; row++)
			{
				for (Size col = 0; col < 3; col++)
				{
					diff.m[row][col] = m[row][col] - rhs.m[row][col];
				}
			}
			return diff;
		}
		
		Matrix3Generic<T> operator*(const Matrix3Generic<T>& rhs) const
		{
			Matrix3Generic<T> prod;
			for (Size row = 0; row < 3; row++)
			{
				for (Size col = 0; col < 3; col++)
				{
					prod.m[row][col] = m[row][0]*rhs.m[0][col] +
						m[row][1]*rhs.m[1][col] +
						m[row][2]*rhs.m[2][col];
				}
			}
			return prod;
		}
		
		Matrix3Generic<T> operator-() const
		{
			Matrix3Generic<T> neg;
			for (Size row = 0; row < 3; row++)
			{
				for (Size col = 0; col < 3; col++)
				{
					neg[row][col] = -m[row][col];
				}
			}
			return neg;
		}

		// matrix * vector [3x3 * 3x1 = 3x1]
		Vector3 operator*(const Vector3& rhs) const
		{
			Vector3 product;
			for (Size row = 0; row < 3; row++)
			{
				product[row] = m[row][0]*rhs[0] +
							m[row][1]*rhs[1] +
							m[row][2]*rhs[2];
			}
			return product;
		}

		// vector * matrix [1x3 * 3x3 = 1x3]
		friend Vector3Generic<T> operator*(const Vector3Generic<T>& vector, const Matrix3Generic<T>& matrix)
		{
			Vector3 product;
			for (Size row = 0; row < 3; row++)
			{
				product[row] =	vector[0]*matrix.m[0][row] +
							vector[1]*matrix.m[1][row] +
							vector[2]*matrix.m[2][row];
			}
			return product;
		}

		// matrix * scalar
		Matrix3Generic<T> operator*(T scalar) const
		{
			Matrix3Generic<T> product;
			for (Size row = 0; row < 3; row++)
			{
				for (Size col = 0; col < 3; col++)
				{
					product[row][col] = scalar*m[row][col];
				}
			}
			return product;
		}

		// scalar * matrix
		friend Matrix3Generic<T> operator*(T scalar, const Matrix3Generic<T>& matrix)
		{
			Matrix3Generic<T> product;
			for (Size row = 0; row < 3; row++)
			{
				for (Size col = 0; col < 3; col++)
				{
					product[row][col] = scalar*matrix.m[row][col];
				}
			}
			return product;
		}

		// utilities
		Matrix3Generic<T> Transpose() const
		{
			Matrix3Generic<T> transpose;
			for (Size row = 0; row < 3; row++)
			{
				for (Size col = 0; col < 3; col++)
				{
					transpose[row][col] = m[col][row];
				}
			}
			return transpose;
		}

		bool Inverse(Matrix3Generic<T>& inverse, T tolerance = 1e-06) const
		{
			// Invert a 3x3 using cofactors.  This is about 8 times faster than
			// the Numerical Recipes code which uses Gaussian elimination.

			inverse[0][0] = m[1][1]*m[2][2] - m[1][2]*m[2][1];
			inverse[0][1] = m[0][2]*m[2][1] - m[0][1]*m[2][2];
			inverse[0][2] = m[0][1]*m[1][2] - m[0][2]*m[1][1];
			inverse[1][0] = m[1][2]*m[2][0] - m[1][0]*m[2][2];
			inverse[1][1] = m[0][0]*m[2][2] - m[0][2]*m[2][0];
			inverse[1][2] = m[0][2]*m[1][0] - m[0][0]*m[1][2];
			inverse[2][0] = m[1][0]*m[2][1] - m[1][1]*m[2][0];
			inverse[2][1] = m[0][1]*m[2][0] - m[0][0]*m[2][1];
			inverse[2][2] = m[0][0]*m[1][1] - m[0][1]*m[1][0];

			T det = m[0][0]*inverse[0][0] + m[0][1]*inverse[1][0] + m[0][2]*inverse[2][0];

			if(Maths::Abs(det) <= tolerance)
			{
				return false;
			}

			T invDet = T(1)/det;
			for (Size row = 0; row < 3; row++)
			{
				for (Size col = 0; col < 3; col++)
				{
					inverse[row][col] *= invDet;
				}
			}

			return true;
		}

		Matrix3Generic<T> Inverse(T tolerance = 1e-06) const
		{
			Matrix3Generic<T> inverse = Matrix3Generic<T>::ZERO;
			Inverse(inverse,tolerance);
			return inverse;
		}
		
		T Determinant () const
		{
			T cofactor00 = m[1][1]*m[2][2] - m[1][2]*m[2][1];
			T cofactor10 = m[1][2]*m[2][0] - m[1][0]*m[2][2];
			T cofactor20 = m[1][0]*m[2][1] - m[1][1]*m[2][0];
			T det = m[0][0]*cofactor00 + m[0][1]*cofactor10 + m[0][2]*cofactor20;
			return det;
		}

		// singular value decomposition
		void SingularValueDecomposition(Matrix3Generic<T>& kL, Vector3& kS, Matrix3Generic<T>& kR) const
		{
			// temas: currently unused
			//const int iMax = 16;
			Size row, col;

			Matrix3Generic<T> kA = *this;
			Bidiagonalize(kA,kL,kR);

			for (unsigned int i = 0; i < ms_iSvdMaxIterations; i++)
			{
				T fTmp, fTmp0, fTmp1;
				T fSin0, fCos0, fTan0;
				T fSin1, fCos1, fTan1;

				bool bTest1 = (Maths::Abs(kA[0][1]) <=
							   ms_fSvdEpsilon * (Maths::Abs(kA[0][0]) + Maths::Abs(kA[1][1])));
				bool bTest2 = (Maths::Abs(kA[1][2]) <=
							   ms_fSvdEpsilon * (Maths::Abs(kA[1][1]) + Maths::Abs(kA[2][2])));
				if ( bTest1 )
				{
					if ( bTest2 )
					{
						kS[0] = kA[0][0];
						kS[1] = kA[1][1];
						kS[2] = kA[2][2];
						break;
					}
					else
					{
						// 2x2 closed form factorization
						fTmp = (kA[1][1]*kA[1][1] - kA[2][2]*kA[2][2] +
							kA[1][2]*kA[1][2])/(kA[1][2]*kA[2][2]);
						fTan0 = 0.5f * (fTmp + Maths::Sqrt(fTmp * fTmp + 4.0f));
						fCos0 = Maths::InvSqrt(T(1) + fTan0 * fTan0);
						fSin0 = fTan0*fCos0;

						for (col = 0; col < 3; col++)
						{
							fTmp0 = kL[col][1];
							fTmp1 = kL[col][2];
							kL[col][1] = fCos0*fTmp0-fSin0*fTmp1;
							kL[col][2] = fSin0*fTmp0+fCos0*fTmp1;
						}

						fTan1 = (kA[1][2]-kA[2][2]*fTan0)/kA[1][1];
						fCos1 = Maths::InvSqrt(T(1) + fTan1 * fTan1);
						fSin1 = -fTan1*fCos1;

						for (row = 0; row < 3; row++)
						{
							fTmp0 = kR[1][row];
							fTmp1 = kR[2][row];
							kR[1][row] = fCos1*fTmp0-fSin1*fTmp1;
							kR[2][row] = fSin1*fTmp0+fCos1*fTmp1;
						}

						kS[0] = kA[0][0];
						kS[1] = fCos0*fCos1*kA[1][1] -
							fSin1*(fCos0*kA[1][2]-fSin0*kA[2][2]);
						kS[2] = fSin0*fSin1*kA[1][1] +
							fCos1*(fSin0*kA[1][2]+fCos0*kA[2][2]);
						break;
					}
				}
				else
				{
					if ( bTest2 )
					{
						// 2x2 closed form factorization
						fTmp = (kA[0][0]*kA[0][0] + kA[1][1]*kA[1][1] -
							kA[0][1]*kA[0][1])/(kA[0][1]*kA[1][1]);
						fTan0 = 0.5f * (-fTmp + Maths::Sqrt(fTmp * fTmp + 4.0f));
						fCos0 = Maths::InvSqrt(T(1) + fTan0 * fTan0);
						fSin0 = fTan0*fCos0;

						for (col = 0; col < 3; col++)
						{
							fTmp0 = kL[col][0];
							fTmp1 = kL[col][1];
							kL[col][0] = fCos0*fTmp0-fSin0*fTmp1;
							kL[col][1] = fSin0*fTmp0+fCos0*fTmp1;
						}

						fTan1 = (kA[0][1]-kA[1][1]*fTan0)/kA[0][0];
						fCos1 = Maths::InvSqrt(T(1) + fTan1 * fTan1);
						fSin1 = -fTan1*fCos1;

						for (row = 0; row < 3; row++)
						{
							fTmp0 = kR[0][row];
							fTmp1 = kR[1][row];
							kR[0][row] = fCos1*fTmp0-fSin1*fTmp1;
							kR[1][row] = fSin1*fTmp0+fCos1*fTmp1;
						}

						kS[0] = fCos0*fCos1*kA[0][0] -
							fSin1*(fCos0*kA[0][1]-fSin0*kA[1][1]);
						kS[1] = fSin0*fSin1*kA[0][0] +
							fCos1*(fSin0*kA[0][1]+fCos0*kA[1][1]);
						kS[2] = kA[2][2];
						break;
					}
					else
					{
						GolubKahanStep(kA,kL,kR);
					}
				}
			}

			// positize diagonal
			for (row = 0; row < 3; row++)
			{
				if ( kS[row] < T(0) )
				{
					kS[row] = -kS[row];
					for (col = 0; col < 3; col++)
						kR[row][col] = -kR[row][col];
				}
			}
		}
		
		void SingularValueComposition(const Matrix3Generic<T>& kL, const Vector3& kS, const Matrix3Generic<T>& kR)
		{
			Size row, col;
			Matrix3Generic<T> kTmp;

			// product S*R
			for (row = 0; row < 3; row++)
			{
				for (col = 0; col < 3; col++)
				{
					kTmp[row][col] = kS[row]*kR[row][col];
				}
			}

			// product L*S*R
			for (row = 0; row < 3; row++)
			{
				for (col = 0; col < 3; col++)
				{
					m[row][col] = T(0);
					for (int iMid = 0; iMid < 3; iMid++)
					{
						m[row][col] += kL[row][iMid]*kTmp[iMid][col];
					}
				}
			}
		}


		// Gram-Schmidt orthonormalisation (applied to columns of rotation matrix)
		void Orthonormalise()
		{
			// Algorithm uses Gram-Schmidt orthogonalisation.  If 'this' matrix is
			// M = [m0|m1|m2], then orthonormal output matrix is Q = [q0|q1|q2],
			//
			//   q0 = m0/|m0|
			//   q1 = (m1-(q0*m1)q0)/|m1-(q0*m1)q0|
			//   q2 = (m2-(q0*m2)q0-(q1*m2)q1)/|m2-(q0*m2)q0-(q1*m2)q1|
			//
			// where |V| indicates length of vector V and A*B indicates dot
			// product of vectors A and B.

			// compute q0
			T fInvLength = Maths::InvSqrt(m[0][0] * m[0][0] + m[1][0]*m[1][0] + m[2][0]*m[2][0]);

			m[0][0] *= fInvLength;
			m[1][0] *= fInvLength;
			m[2][0] *= fInvLength;

			// compute q1
			T fDot0 = m[0][0]*m[0][1] + m[1][0]*m[1][1] + m[2][0]*m[2][1];

			m[0][1] -= fDot0*m[0][0];
			m[1][1] -= fDot0*m[1][0];
			m[2][1] -= fDot0*m[2][0];

			fInvLength = Maths::InvSqrt(m[0][1] * m[0][1] + m[1][1]*m[1][1] + m[2][1]*m[2][1]);

			m[0][1] *= fInvLength;
			m[1][1] *= fInvLength;
			m[2][1] *= fInvLength;

			// compute q2
			T fDot1 = m[0][1]*m[0][2] + m[1][1]*m[1][2] + m[2][1]*m[2][2];

			fDot0 = m[0][0]*m[0][2] + m[1][0]*m[1][2] +m[2][0]*m[2][2];

			m[0][2] -= fDot0*m[0][0] + fDot1*m[0][1];
			m[1][2] -= fDot0*m[1][0] + fDot1*m[1][1];
			m[2][2] -= fDot0*m[2][0] + fDot1*m[2][1];

			fInvLength = Maths::InvSqrt(m[0][2] * m[0][2] + m[1][2]*m[1][2] + m[2][2]*m[2][2]);
			m[0][2] *= fInvLength;
			m[1][2] *= fInvLength;
			m[2][2] *= fInvLength;
		}

		// orthogonal Q, diagonal D, upper triangular U stored as (u01,u02,u12)
		void QDUDecomposition(Matrix3Generic<T>& kQ, Vector3Generic<T>& kD, Vector3Generic<T>& kU) const
		{
			// Factor M = QR = QDU where Q is orthogonal, D is diagonal,
			// and U is upper triangular with ones on its diagonal.  Algorithm uses
			// Gram-Schmidt orthogonalization (the QR algorithm).
			//
			// If M = [ m0 | m1 | m2 ] and Q = [ q0 | q1 | q2 ], then
			//
			//   q0 = m0/|m0|
			//   q1 = (m1-(q0*m1)q0)/|m1-(q0*m1)q0|
			//   q2 = (m2-(q0*m2)q0-(q1*m2)q1)/|m2-(q0*m2)q0-(q1*m2)q1|
			//
			// where |V| indicates length of vector V and A*B indicates dot
			// product of vectors A and B.  The matrix R has entries
			//
			//   r00 = q0*m0  r01 = q0*m1  r02 = q0*m2
			//   r10 = 0	  r11 = q1*m1  r12 = q1*m2
			//   r20 = 0	  r21 = 0	  r22 = q2*m2
			//
			// so D = diag(r00,r11,r22) and U has entries u01 = r01/r00,
			// u02 = r02/r00, and u12 = r12/r11.

			// Q = rotation
			// D = scaling
			// U = shear

			// D stores the three diagonal entries r00, r11, r22
			// U stores the entries U[0] = u01, U[1] = u02, U[2] = u12

			// build orthogonal matrix Q
			T fInvLength = Maths::InvSqrt(m[0][0] * m[0][0] + m[1][0]*m[1][0] + m[2][0]*m[2][0]);
			kQ[0][0] = m[0][0]*fInvLength;
			kQ[1][0] = m[1][0]*fInvLength;
			kQ[2][0] = m[2][0]*fInvLength;

			T fDot = kQ[0][0]*m[0][1] + kQ[1][0]*m[1][1] + kQ[2][0]*m[2][1];
			kQ[0][1] = m[0][1]-fDot*kQ[0][0];
			kQ[1][1] = m[1][1]-fDot*kQ[1][0];
			kQ[2][1] = m[2][1]-fDot*kQ[2][0];
			fInvLength = Maths::InvSqrt(kQ[0][1] * kQ[0][1] + kQ[1][1] * kQ[1][1] + kQ[2][1]*kQ[2][1]);
			kQ[0][1] *= fInvLength;
			kQ[1][1] *= fInvLength;
			kQ[2][1] *= fInvLength;

			fDot = kQ[0][0]*m[0][2] + kQ[1][0]*m[1][2] + kQ[2][0]*m[2][2];
			kQ[0][2] = m[0][2]-fDot*kQ[0][0];
			kQ[1][2] = m[1][2]-fDot*kQ[1][0];
			kQ[2][2] = m[2][2]-fDot*kQ[2][0];
			fDot = kQ[0][1]*m[0][2] + kQ[1][1]*m[1][2] + kQ[2][1]*m[2][2];
			kQ[0][2] -= fDot*kQ[0][1];
			kQ[1][2] -= fDot*kQ[1][1];
			kQ[2][2] -= fDot*kQ[2][1];
			fInvLength = Maths::InvSqrt(kQ[0][2] * kQ[0][2] + kQ[1][2] * kQ[1][2] + kQ[2][2]*kQ[2][2]);
			kQ[0][2] *= fInvLength;
			kQ[1][2] *= fInvLength;
			kQ[2][2] *= fInvLength;

			// guarantee that orthogonal matrix has determinant 1 (no reflections)
			T fDet =kQ[0][0]*kQ[1][1]*kQ[2][2] + kQ[0][1]*kQ[1][2]*kQ[2][0] +
					kQ[0][2]*kQ[1][0]*kQ[2][1] - kQ[0][2]*kQ[1][1]*kQ[2][0] -
					kQ[0][1]*kQ[1][0]*kQ[2][2] - kQ[0][0]*kQ[1][2]*kQ[2][1];

			if ( fDet < T(0) )
			{
				for (Size row = 0; row < 3; row++)
				{
					for (Size col = 0; col < 3; col++)
					{
						kQ[row][col] = -kQ[row][col];
					}
				}
			}

			// build "right" matrix R
			Matrix3Generic<T> kR;
			kR[0][0] = kQ[0][0]*m[0][0] + kQ[1][0]*m[1][0] + kQ[2][0]*m[2][0];
			kR[0][1] = kQ[0][0]*m[0][1] + kQ[1][0]*m[1][1] + kQ[2][0]*m[2][1];
			kR[1][1] = kQ[0][1]*m[0][1] + kQ[1][1]*m[1][1] + kQ[2][1]*m[2][1];
			kR[0][2] = kQ[0][0]*m[0][2] + kQ[1][0]*m[1][2] + kQ[2][0]*m[2][2];
			kR[1][2] = kQ[0][1]*m[0][2] + kQ[1][1]*m[1][2] + kQ[2][1]*m[2][2];
			kR[2][2] = kQ[0][2]*m[0][2] + kQ[1][2]*m[1][2] + kQ[2][2]*m[2][2];

			// the scaling component
			kD[0] = kR[0][0];
			kD[1] = kR[1][1];
			kD[2] = kR[2][2];

			// the shear component
			T fInvD0 = T(1)/kD[0];
			kU[0] = kR[0][1]*fInvD0;
			kU[1] = kR[0][2]*fInvD0;
			kU[2] = kR[1][2]/kD[1];
		}
		
		T SpectralNorm() const
		{
			Matrix3Generic<T> kP;
			Size row, col;
			T fPmax = T(0);
			for (row = 0; row < 3; row++)
			{
				for (col = 0; col < 3; col++)
				{
					kP[row][col] = T(0);
					for (int iMid = 0; iMid < 3; iMid++)
					{
						kP[row][col] += m[iMid][row]*m[iMid][col];
					}
					if ( kP[row][col] > fPmax )
					{
						fPmax = kP[row][col];
					}
				}
			}

			T fInvPmax = T(1)/fPmax;
			for (row = 0; row < 3; row++)
			{
				for (col = 0; col < 3; col++)
				{
					kP[row][col] *= fInvPmax;
				}
			}

			T afCoeff[3];
			afCoeff[0] = -( kP[0][0]*(kP[1][1]*kP[2][2]-kP[1][2]*kP[2][1]) +
							kP[0][1]*(kP[2][0]*kP[1][2]-kP[1][0]*kP[2][2]) +
							kP[0][2]*(kP[1][0]*kP[2][1]-kP[2][0]*kP[1][1]));
			afCoeff[1]=	kP[0][0]*kP[1][1]-kP[0][1]*kP[1][0] +
						kP[0][0]*kP[2][2]-kP[0][2]*kP[2][0] +
						kP[1][1]*kP[2][2]-kP[1][2]*kP[2][1];
			afCoeff[2] = -(kP[0][0]+kP[1][1]+kP[2][2]);

			T fRoot = MaxCubicRoot(afCoeff);
			T fNorm = Maths::Sqrt(fPmax * fRoot);
			return fNorm;
		}

		// matrix must be orthonormal
		void ToAxisAngle(Vector3& rkAxis, Radian& rfRadians) const
		{
			// Let (x,y,z) be the unit-length axis and let A be an angle of rotation.
			// The rotation matrix is R = I + sin(A)*P + (1-cos(A))*P^2 where
			// I is the identity and
			//
			//	   +-		-+
			//   P = |  0 -z +y |
			//	   | +z  0 -x |
			//	   | -y +x  0 |
			//	   +-		-+
			//
			// If A > 0, R represents a counterclockwise rotation about the axis in
			// the sense of looking from the tip of the axis vector towards the
			// origin.  Some algebra will show that
			//
			//   cos(A) = (trace(R)-1)/2  and  R - R^t = 2*sin(A)*P
			//
			// In the event that A = pi, R-R^t = 0 which prevents us from extracting
			// the axis through P.  Instead note that R = I+2*P^2 when A = pi, so
			// P^2 = (R-I)/2.  The diagonal entries of P^2 are x^2-1, y^2-1, and
			// z^2-1.  We can solve these for axis (x,y,z).  Because the angle is pi,
			// it does not matter which sign you choose on the square roots.

			T fTrace = m[0][0] + m[1][1] + m[2][2];
			T fCos = 0.5f*(fTrace-T(1));
			rfRadians = Maths::ACos(fCos); // in [0,PI]

			if ( rfRadians > Radian(0) )
			{
				if(rfRadians < Radian(Maths::PI))
				{
					rkAxis.x = m[2][1]-m[1][2];
					rkAxis.y = m[0][2]-m[2][0];
					rkAxis.z = m[1][0]-m[0][1];
					rkAxis.Normalise();
				}
				else
				{
					// angle is PI
					T fHalfInverse;
					if ( m[0][0] >= m[1][1] )
					{
						// r00 >= r11
						if ( m[0][0] >= m[2][2] )
						{
							// r00 is maximum diagonal term
							rkAxis.x = 0.5f * Maths::Sqrt(m[0][0] -
								m[1][1] - m[2][2] + T(1));
							fHalfInverse = 0.5f/rkAxis.x;
							rkAxis.y = fHalfInverse*m[0][1];
							rkAxis.z = fHalfInverse*m[0][2];
						}
						else
						{
							// r22 is maximum diagonal term
							rkAxis.z = 0.5f * Maths::Sqrt(m[2][2] -
								m[0][0] - m[1][1] + T(1));
							fHalfInverse = 0.5f/rkAxis.z;
							rkAxis.x = fHalfInverse*m[0][2];
							rkAxis.y = fHalfInverse*m[1][2];
						}
					}
					else
					{
						// r11 > r00
						if ( m[1][1] >= m[2][2] )
						{
							// r11 is maximum diagonal term
							rkAxis.y = 0.5f * Maths::Sqrt(m[1][1] -
								m[0][0] - m[2][2] + T(1));
							fHalfInverse  = 0.5f/rkAxis.y;
							rkAxis.x = fHalfInverse*m[0][1];
							rkAxis.z = fHalfInverse*m[1][2];
						}
						else
						{
							// r22 is maximum diagonal term
							rkAxis.z = 0.5f * Maths::Sqrt(m[2][2] -
								m[0][0] - m[1][1] + T(1));
							fHalfInverse = 0.5f/rkAxis.z;
							rkAxis.x = fHalfInverse*m[0][2];
							rkAxis.y = fHalfInverse*m[1][2];
						}
					}
				}
			}
			else
			{
				// The angle is 0 and the matrix is the identity.  Any axis will
				// work, so just use the x-axis.
				rkAxis.x = T(1);
				rkAxis.y = T(0);
				rkAxis.z = T(0);
			}
		}
		
		inline void ToAxisAngle(Vector3& rkAxis, Degree& rfAngle) const
		{
			Radian r;
			ToAxisAngle( rkAxis, r );
			rfAngle = r;
		}
		
		void FromAxisAngle(const Vector3& rkAxis, const Radian& fRadians)
		{
			T fCos = Maths::Cos(fRadians.ValueRadians());
			T fSin = Maths::Sin(fRadians.ValueRadians());
			T fOneMinusCos = T(1)-fCos;
			T fX2 = rkAxis.x*rkAxis.x;
			T fY2 = rkAxis.y*rkAxis.y;
			T fZ2 = rkAxis.z*rkAxis.z;
			T fXYM = rkAxis.x*rkAxis.y*fOneMinusCos;
			T fXZM = rkAxis.x*rkAxis.z*fOneMinusCos;
			T fYZM = rkAxis.y*rkAxis.z*fOneMinusCos;
			T fXSin = rkAxis.x*fSin;
			T fYSin = rkAxis.y*fSin;
			T fZSin = rkAxis.z*fSin;

			m[0][0] = fX2*fOneMinusCos+fCos;
			m[0][1] = fXYM-fZSin;
			m[0][2] = fXZM+fYSin;
			m[1][0] = fXYM+fZSin;
			m[1][1] = fY2*fOneMinusCos+fCos;
			m[1][2] = fYZM-fXSin;
			m[2][0] = fXZM-fYSin;
			m[2][1] = fYZM+fXSin;
			m[2][2] = fZ2*fOneMinusCos+fCos;
		}
		

		// The matrix must be orthonormal.  The decomposition is yaw*pitch*roll
		// where yaw is rotation about the Up vector, pitch is rotation about the
		// Right axis, and roll is rotation about the Direction axis.
		bool ToEulerAnglesXYZ (Radian& rfYAngle, Radian& rfPAngle, Radian& rfRAngle) const
		{
			// rot =  cy*cz		  -cy*sz		   sy
			//		cz*sx*sy+cx*sz  cx*cz-sx*sy*sz -cy*sx
			//	   -cx*cz*sy+sx*sz  cz*sx+cx*sy*sz  cx*cy

			rfPAngle = Radian(Maths::ASin(m[0][2]));
			if(rfPAngle < Radian(Maths::HALF_PI))
			{
				if(rfPAngle > Radian(-Maths::HALF_PI))
				{
					rfYAngle = Maths::ATan2(-m[1][2], m[2][2]);
					rfRAngle = Maths::ATan2(-m[0][1], m[0][0]);
					return true;
				}
				else
				{
					// WARNING.  Not a unique solution.
					Radian fRmY = Maths::ATan2(m[1][0], m[1][1]);
					rfRAngle = Radian(0);  // any angle works
					rfYAngle = rfRAngle - fRmY;
					return false;
				}
			}
			else
			{
				// WARNING.  Not a unique solution.
				Radian fRpY = Maths::ATan2(m[1][0], m[1][1]);
				rfRAngle = Radian(0);  // any angle works
				rfYAngle = fRpY - rfRAngle;
				return false;
			}
		}
		
		bool ToEulerAnglesXZY (Radian& rfYAngle, Radian& rfPAngle, Radian& rfRAngle) const
		{
			// rot =  cy*cz		  -sz			  cz*sy
			//		sx*sy+cx*cy*sz  cx*cz		  -cy*sx+cx*sy*sz
			//	   -cx*sy+cy*sx*sz  cz*sx		   cx*cy+sx*sy*sz

			rfPAngle = Maths::ASin(-m[0][1]);
			if(rfPAngle < Radian(Maths::HALF_PI))
			{
				if(rfPAngle > Radian(-Maths::HALF_PI))
				{
					rfYAngle = Maths::ATan2(m[2][1], m[1][1]);
					rfRAngle = Maths::ATan2(m[0][2], m[0][0]);
					return true;
				}
				else
				{
					// WARNING.  Not a unique solution.
					Radian fRmY = Maths::ATan2(-m[2][0], m[2][2]);
					rfRAngle = Radian(0);  // any angle works
					rfYAngle = rfRAngle - fRmY;
					return false;
				}
			}
			else
			{
				// WARNING.  Not a unique solution.
				Radian fRpY = Maths::ATan2(-m[2][0], m[2][2]);
				rfRAngle = Radian(0);  // any angle works
				rfYAngle = fRpY - rfRAngle;
				return false;
			}
		}
		
		bool ToEulerAnglesYXZ (Radian& rfYAngle, Radian& rfPAngle, Radian& rfRAngle) const
		{
			// rot =  cy*cz+sx*sy*sz  cz*sx*sy-cy*sz  cx*sy
			//		cx*sz		   cx*cz		  -sx
			//	   -cz*sy+cy*sx*sz  cy*cz*sx+sy*sz  cx*cy

			rfPAngle = Maths::ASin(-m[1][2]);
			if(rfPAngle < Radian(Maths::HALF_PI))
			{
				if(rfPAngle > Radian(-Maths::HALF_PI))
				{
					rfYAngle = Maths::ATan2(m[0][2], m[2][2]);
					rfRAngle = Maths::ATan2(m[1][0], m[1][1]);
					return true;
				}
				else
				{
					// WARNING.  Not a unique solution.
					Radian fRmY = Maths::ATan2(-m[0][1], m[0][0]);
					rfRAngle = Radian(0);  // any angle works
					rfYAngle = rfRAngle - fRmY;
					return false;
				}
			}
			else
			{
				// WARNING.  Not a unique solution.
				Radian fRpY = Maths::ATan2(-m[0][1], m[0][0]);
				rfRAngle = Radian(0);  // any angle works
				rfYAngle = fRpY - rfRAngle;
				return false;
			}
		}
		
		bool ToEulerAnglesYZX (Radian& rfYAngle, Radian& rfPAngle, Radian& rfRAngle) const
		{
			// rot =  cy*cz		   sx*sy-cx*cy*sz  cx*sy+cy*sx*sz
			//		sz			  cx*cz		  -cz*sx
			//	   -cz*sy		   cy*sx+cx*sy*sz  cx*cy-sx*sy*sz

			rfPAngle = Maths::ASin(m[1][0]);
			if(rfPAngle < Radian(Maths::HALF_PI))
			{
				if(rfPAngle > Radian(-Maths::HALF_PI))
				{
					rfYAngle = Maths::ATan2(-m[2][0], m[0][0]);
					rfRAngle = Maths::ATan2(-m[1][2], m[1][1]);
					return true;
				}
				else
				{
					// WARNING.  Not a unique solution.
					Radian fRmY = Maths::ATan2(m[2][1], m[2][2]);
					rfRAngle = Radian(0);  // any angle works
					rfYAngle = rfRAngle - fRmY;
					return false;
				}
			}
			else
			{
				// WARNING.  Not a unique solution.
				Radian fRpY = Maths::ATan2(m[2][1], m[2][2]);
				rfRAngle = Radian(0);  // any angle works
				rfYAngle = fRpY - rfRAngle;
				return false;
			}
		}
		
		bool ToEulerAnglesZXY (Radian& rfYAngle, Radian& rfPAngle, Radian& rfRAngle) const
		{
			// rot =  cy*cz-sx*sy*sz -cx*sz		   cz*sy+cy*sx*sz
			//		cz*sx*sy+cy*sz  cx*cz		  -cy*cz*sx+sy*sz
			//	   -cx*sy		   sx			  cx*cy

			rfPAngle = Maths::ASin(m[2][1]);
			if(rfPAngle < Radian(Maths::HALF_PI))
			{
				if(rfPAngle > Radian(-Maths::HALF_PI))
				{
					rfYAngle = Maths::ATan2(-m[0][1], m[1][1]);
					rfRAngle = Maths::ATan2(-m[2][0], m[2][2]);
					return true;
				}
				else
				{
					// WARNING.  Not a unique solution.
					Radian fRmY = Maths::ATan2(m[0][2], m[0][0]);
					rfRAngle = Radian(0);  // any angle works
					rfYAngle = rfRAngle - fRmY;
					return false;
				}
			}
			else
			{
				// WARNING.  Not a unique solution.
				Radian fRpY = Maths::ATan2(m[0][2], m[0][0]);
				rfRAngle = Radian(0);  // any angle works
				rfYAngle = fRpY - rfRAngle;
				return false;
			}
		}
		
		bool ToEulerAnglesZYX (Radian& rfYAngle, Radian& rfPAngle, Radian& rfRAngle) const
		{
			// rot =  cy*cz		   cz*sx*sy-cx*sz  cx*cz*sy+sx*sz
			//		cy*sz		   cx*cz+sx*sy*sz -cz*sx+cx*sy*sz
			//	   -sy			  cy*sx		   cx*cy

			rfPAngle = Maths::ASin(-m[2][0]);
			if(rfPAngle < Radian(Maths::HALF_PI))
			{
				if(rfPAngle > Radian(-Maths::HALF_PI))
				{
					rfYAngle = Maths::ATan2(m[1][0], m[0][0]);
					rfRAngle = Maths::ATan2(m[2][1], m[2][2]);
					return true;
				}
				else
				{
					// WARNING.  Not a unique solution.
					Radian fRmY = Maths::ATan2(-m[0][1], m[0][2]);
					rfRAngle = Radian(0);  // any angle works
					rfYAngle = rfRAngle - fRmY;
					return false;
				}
			}
			else
			{
				// WARNING.  Not a unique solution.
				Radian fRpY = Maths::ATan2(-m[0][1], m[0][2]);
				rfRAngle = Radian(0);  // any angle works
				rfYAngle = fRpY - rfRAngle;
				return false;
			}
		}
		
		void FromEulerAnglesXYZ(const Radian& fYAngle, const Radian& fPAngle, const Radian& fRAngle)
		{
			T fCos, fSin;

			fCos = Maths::Cos(fYAngle.ValueRadians());
			fSin = Maths::Sin(fYAngle.ValueRadians());
			Matrix3Generic<T> kXMat(T(1), T(0), T(0), T(0), fCos, -fSin, T(0), fSin, fCos);

			fCos = Maths::Cos(fPAngle.ValueRadians());
			fSin = Maths::Sin(fPAngle.ValueRadians());
			Matrix3Generic<T> kYMat(fCos, T(0), fSin, T(0), T(1), T(0), -fSin, T(0), fCos);

			fCos = Maths::Cos(fRAngle.ValueRadians());
			fSin = Maths::Sin(fRAngle.ValueRadians());
			Matrix3Generic<T> kZMat(fCos, -fSin, T(0), fSin, fCos, T(0), T(0), T(0), T(1));

			*this = kXMat*(kYMat*kZMat);
		}

		void FromEulerAnglesXZY(const Radian& fYAngle, const Radian& fPAngle, const Radian& fRAngle)
		{
			T fCos, fSin;

			fCos = Maths::Cos(fYAngle.ValueRadians());
			fSin = Maths::Sin(fYAngle.ValueRadians());
			Matrix3Generic<T> kXMat(T(1), T(0), T(0), T(0), fCos, -fSin, T(0), fSin, fCos);

			fCos = Maths::Cos(fPAngle.ValueRadians());
			fSin = Maths::Sin(fPAngle.ValueRadians());
			Matrix3Generic<T> kZMat(fCos, -fSin, T(0), fSin, fCos, T(0), T(0), T(0), T(1));

			fCos = Maths::Cos(fRAngle.ValueRadians());
			fSin = Maths::Sin(fRAngle.ValueRadians());
			Matrix3Generic<T> kYMat(fCos, T(0), fSin, T(0), T(1), T(0), -fSin, T(0), fCos);

			*this = kXMat*(kZMat*kYMat);
		}

		void FromEulerAnglesYXZ(const Radian& fYAngle, const Radian& fPAngle, const Radian& fRAngle)
		{
			T fCos, fSin;

			fCos = Maths::Cos(fYAngle.ValueRadians());
			fSin = Maths::Sin(fYAngle.ValueRadians());
			Matrix3Generic<T> kYMat(fCos, T(0), fSin, T(0), T(1), T(0), -fSin, T(0), fCos);

			fCos = Maths::Cos(fPAngle.ValueRadians());
			fSin = Maths::Sin(fPAngle.ValueRadians());
			Matrix3Generic<T> kXMat(T(1), T(0), T(0), T(0), fCos, -fSin, T(0), fSin, fCos);

			fCos = Maths::Cos(fRAngle.ValueRadians());
			fSin = Maths::Sin(fRAngle.ValueRadians());
			Matrix3Generic<T> kZMat(fCos, -fSin, T(0), fSin, fCos, T(0), T(0), T(0), T(1));

			*this = kYMat*(kXMat*kZMat);
		}

		void FromEulerAnglesYZX(const Radian& fYAngle, const Radian& fPAngle, const Radian& fRAngle)
		{
			T fCos, fSin;

			fCos = Maths::Cos(fYAngle.ValueRadians());
			fSin = Maths::Sin(fYAngle.ValueRadians());
			Matrix3Generic<T> kYMat(fCos, T(0), fSin, T(0), T(1), T(0), -fSin, T(0), fCos);

			fCos = Maths::Cos(fPAngle.ValueRadians());
			fSin = Maths::Sin(fPAngle.ValueRadians());
			Matrix3Generic<T> kZMat(fCos, -fSin, T(0), fSin, fCos, T(0), T(0), T(0), T(1));

			fCos = Maths::Cos(fRAngle.ValueRadians());
			fSin = Maths::Sin(fRAngle.ValueRadians());
			Matrix3Generic<T> kXMat(T(1), T(0), T(0), T(0), fCos, -fSin, T(0), fSin, fCos);

			*this = kYMat*(kZMat*kXMat);
		}
		
		void FromEulerAnglesZXY(const Radian& fYAngle, const Radian& fPAngle, const Radian& fRAngle)
		{
			T fCos, fSin;

			fCos = Maths::Cos(fYAngle.ValueRadians());
			fSin = Maths::Sin(fYAngle.ValueRadians());
			Matrix3Generic<T> kZMat(fCos, -fSin, T(0), fSin, fCos, T(0), T(0), T(0), T(1));

			fCos = Maths::Cos(fPAngle.ValueRadians());
			fSin = Maths::Sin(fPAngle.ValueRadians());
			Matrix3Generic<T> kXMat(T(1), T(0), T(0), T(0), fCos, -fSin, T(0), fSin, fCos);

			fCos = Maths::Cos(fRAngle.ValueRadians());
			fSin = Maths::Sin(fRAngle.ValueRadians());
			Matrix3Generic<T> kYMat(fCos, T(0), fSin, T(0), T(1), T(0), -fSin, T(0), fCos);

			*this = kZMat*(kXMat*kYMat);
		}

		void FromEulerAnglesZYX(const Radian& fYAngle, const Radian& fPAngle, const Radian& fRAngle)
		{
			T fCos, fSin;

			fCos = Maths::Cos(fYAngle.ValueRadians());
			fSin = Maths::Sin(fYAngle.ValueRadians());
			Matrix3Generic<T> kZMat(fCos, -fSin, T(0), fSin, fCos, T(0), T(0), T(0), T(1));

			fCos = Maths::Cos(fPAngle.ValueRadians());
			fSin = Maths::Sin(fPAngle.ValueRadians());
			Matrix3Generic<T> kYMat(fCos, T(0), fSin, T(0), T(1), T(0), -fSin, T(0), fCos);

			fCos = Maths::Cos(fRAngle.ValueRadians());
			fSin = Maths::Sin(fRAngle.ValueRadians());
			Matrix3Generic<T> kXMat(T(1), T(0), T(0), T(0), fCos, -fSin, T(0), fSin, fCos);

			*this = kZMat*(kYMat*kXMat);
		}

		// eigensolver, matrix must be symmetric
		void EigenSolveSymmetric (T afEigenvalue[3], Vector3 akEigenvector[3]) const
		{
			Matrix3Generic<T> kMatrix = *this;
			T afSubDiag[3];
			kMatrix.Tridiagonal(afEigenvalue,afSubDiag);
			kMatrix.QLAlgorithm(afEigenvalue,afSubDiag);

			for (Size i = 0; i < 3; i++)
			{
				akEigenvector[i][0] = kMatrix[0][i];
				akEigenvector[i][1] = kMatrix[1][i];
				akEigenvector[i][2] = kMatrix[2][i];
			}

			// make eigenvectors form a right--handed system
			Vector3 kCross = akEigenvector[1].Cross(akEigenvector[2]);
			T fDet = akEigenvector[0].Dot(kCross);
			if ( fDet < T(0) )
			{
				akEigenvector[2][0] = - akEigenvector[2][0];
				akEigenvector[2][1] = - akEigenvector[2][1];
				akEigenvector[2][2] = - akEigenvector[2][2];
			}
		}

		static void TensorProduct(const Vector3& rkU, const Vector3& rkV, Matrix3Generic<T>& rkProduct)
		{
			for (Size row = 0; row < 3; row++)
			{
				for (Size col = 0; col < 3; col++)
				{
					rkProduct[row][col] = rkU[row]*rkV[col];
				}
			}
		}

		/** Determines if this matrix involves a scaling. */
		inline bool HasScale() const
		{
			// check magnitude of column vectors (==local axes)
			T t = m[0][0] * m[0][0] + m[1][0] * m[1][0] + m[2][0] * m[2][0];
			if(!Maths::RealEqual(t, T(1), T(1e-04)))
				return true;
			t = m[0][1] * m[0][1] + m[1][1] * m[1][1] + m[2][1] * m[2][1];
			if(!Maths::RealEqual(t, T(1), T(1e-04) ))
				return true;
			t = m[0][2] * m[0][2] + m[1][2] * m[1][2] + m[2][2] * m[2][2];
			if(!Maths::RealEqual(t, T(1), T(1e-04)))
				return true;

			return false;
		}

		/** Function for writing to a stream.
		*/
		inline friend std::ostream& operator<<(std::ostream& o, const Matrix3Generic<T>& m)
		{
			o << "Matrix3(" << m[0][0] << "," << m[0][1] << "," << m[0][2] << "," 
							<< m[1][0] << "," << m[1][1] << "," << m[1][2] << "," 
							<< m[2][0] << "," << m[2][1] << "," << m[2][2] << ")";
			return o;
		}
		
		inline friend std::istream& operator>>(std::istream& i, Matrix3Generic<T>& v)
		{
			std::string temp;
			i >> temp;	//Read the input chunk to the first whitespace.
			
			//temp should now be either one of the following formats:
			//1.  "Matrix3(00,01,02,10,11,12,20,21,22)" if it was converted using <<
			//2.  "00,01,02,10,11,12,20,21,22" if it was from some other input
			
			if(!Utils::String::VerifyConstructorAndExtractParameters(temp,"Matrix3"))
			{
				v = Matrix3Generic<T>::IDENTITY;
				i.setstate(std::ios_base::failbit);
				return i;
			}
			
			if(!Utils::String::ConvertAndAssign(temp,&(v.m[0][0]),9))
			{
				v = Matrix3Generic<T>::IDENTITY;
				i.setstate(std::ios_base::failbit);
			}
			return i;
		}

		static const T EPSILON;
		static const Matrix3Generic<T> ZERO;
		static const Matrix3Generic<T> IDENTITY;

	protected:
		// support for eigensolver
		void Tridiagonal (T afDiag[3], T afSubDiag[3])
		{
			// Householder reduction T = Q^t M Q
			//   Input:
			//	 mat, symmetric 3x3 matrix M
			//   Output:
			//	 mat, orthogonal matrix Q
			//	 diag, diagonal entries of T
			//	 subd, subdiagonal entries of T (T is symmetric)

			T fA = m[0][0];
			T fB = m[0][1];
			T fC = m[0][2];
			T fD = m[1][1];
			T fE = m[1][2];
			T fF = m[2][2];

			afDiag[0] = fA;
			afSubDiag[2] = T(0);
			if(Maths::Abs(fC) >= EPSILON)
				{
				T fLength = Maths::Sqrt(fB * fB + fC * fC);
				T fInvLength = T(1)/fLength;
				fB *= fInvLength;
				fC *= fInvLength;
				T fQ = T(2)*fB*fE+fC*(fF-fD);
				afDiag[1] = fD+fC*fQ;
				afDiag[2] = fF-fC*fQ;
				afSubDiag[0] = fLength;
				afSubDiag[1] = fE-fB*fQ;
				m[0][0] = T(1);
				m[0][1] = T(0);
				m[0][2] = T(0);
				m[1][0] = T(0);
				m[1][1] = fB;
				m[1][2] = fC;
				m[2][0] = T(0);
				m[2][1] = fC;
				m[2][2] = -fB;
			}
			else
			{
				afDiag[1] = fD;
				afDiag[2] = fF;
				afSubDiag[0] = fB;
				afSubDiag[1] = fE;
				m[0][0] = T(1);
				m[0][1] = T(0);
				m[0][2] = T(0);
				m[1][0] = T(0);
				m[1][1] = T(1);
				m[1][2] = T(0);
				m[2][0] = T(0);
				m[2][1] = T(0);
				m[2][2] = T(1);
			}
		}
		
		bool QLAlgorithm (T afDiag[3], T afSubDiag[3])
		{
			// QL iteration with implicit shifting to reduce matrix from tridiagonal
			// to diagonal

			for (int i0 = 0; i0 < 3; i0++)
			{
				const unsigned int iMaxIter = 32;
				unsigned int iIter;
				for (iIter = 0; iIter < iMaxIter; iIter++)
				{
					int i1;
					for (i1 = i0; i1 <= 1; i1++)
					{
						T fSum = Maths::Abs(afDiag[i1]) + Maths::Abs(afDiag[i1 + 1]);
						if(Maths::Abs(afSubDiag[i1]) + fSum == fSum)
							break;
					}
					if ( i1 == i0 )
						break;

					T fTmp0 = (afDiag[i0+1]-afDiag[i0])/(T(2)*afSubDiag[i0]);
					T fTmp1 = Maths::Sqrt(fTmp0 * fTmp0 + T(1));
					if ( fTmp0 < T(0) )
						fTmp0 = afDiag[i1]-afDiag[i0]+afSubDiag[i0]/(fTmp0-fTmp1);
					else
						fTmp0 = afDiag[i1]-afDiag[i0]+afSubDiag[i0]/(fTmp0+fTmp1);
					T fSin = T(1);
					T fCos = T(1);
					T fTmp2 = T(0);
					for (int i2 = i1-1; i2 >= i0; i2--)
					{
						T fTmp3 = fSin*afSubDiag[i2];
						T fTmp4 = fCos*afSubDiag[i2];
						if(Maths::Abs(fTmp3) >= Maths::Abs(fTmp0))
						{
							fCos = fTmp0/fTmp3;
							fTmp1 = Maths::Sqrt(fCos * fCos + T(1));
							afSubDiag[i2+1] = fTmp3*fTmp1;
							fSin = T(1)/fTmp1;
							fCos *= fSin;
						}
						else
						{
							fSin = fTmp3/fTmp0;
							fTmp1 = Maths::Sqrt(fSin * fSin + T(1));
							afSubDiag[i2+1] = fTmp0*fTmp1;
							fCos = T(1)/fTmp1;
							fSin *= fCos;
						}
						fTmp0 = afDiag[i2+1]-fTmp2;
						fTmp1 = (afDiag[i2]-fTmp0)*fSin+T(2)*fTmp4*fCos;
						fTmp2 = fSin*fTmp1;
						afDiag[i2+1] = fTmp0+fTmp2;
						fTmp0 = fCos*fTmp1-fTmp4;

						for (int row = 0; row < 3; row++)
						{
							fTmp3 = m[row][i2+1];
							m[row][i2+1] = fSin*m[row][i2] +
								fCos*fTmp3;
							m[row][i2] = fCos*m[row][i2] -
								fSin*fTmp3;
						}
					}
					afDiag[i0] -= fTmp2;
					afSubDiag[i0] = fTmp0;
					afSubDiag[i1] = T(0);
				}

				if ( iIter == iMaxIter )
				{
					// should not get here under normal circumstances
					return false;
				}
			}

			return true;
		}
		
		// support for singular value decomposition
		static const T ms_fSvdEpsilon;
		static const unsigned int ms_iSvdMaxIterations;
		static void Bidiagonalize(Matrix3Generic<T>& kA, Matrix3Generic<T>& kL, Matrix3Generic<T>& kR)
		{
			T afV[3], afW[3];
			T fLength, fSign, fT1, fInvT1, fT2;
			bool bIdentity;

			// map first column to (*,0,0)
			fLength = Maths::Sqrt(kA[0][0] * kA[0][0] + kA[1][0] * kA[1][0] + kA[2][0]*kA[2][0]);
			if ( fLength > T(0) )
			{
				fSign = (kA[0][0] > T(0) ? T(1) : T(-1));
				fT1 = kA[0][0] + fSign*fLength;
				fInvT1 = T(1)/fT1;
				afV[1] = kA[1][0]*fInvT1;
				afV[2] = kA[2][0]*fInvT1;

				fT2 = T(-2)/(T(1)+afV[1]*afV[1]+afV[2]*afV[2]);
				afW[0] = fT2*(kA[0][0]+kA[1][0]*afV[1]+kA[2][0]*afV[2]);
				afW[1] = fT2*(kA[0][1]+kA[1][1]*afV[1]+kA[2][1]*afV[2]);
				afW[2] = fT2*(kA[0][2]+kA[1][2]*afV[1]+kA[2][2]*afV[2]);
				kA[0][0] += afW[0];
				kA[0][1] += afW[1];
				kA[0][2] += afW[2];
				kA[1][1] += afV[1]*afW[1];
				kA[1][2] += afV[1]*afW[2];
				kA[2][1] += afV[2]*afW[1];
				kA[2][2] += afV[2]*afW[2];

				kL[0][0] = T(1)+fT2;
				kL[0][1] = kL[1][0] = fT2*afV[1];
				kL[0][2] = kL[2][0] = fT2*afV[2];
				kL[1][1] = T(1)+fT2*afV[1]*afV[1];
				kL[1][2] = kL[2][1] = fT2*afV[1]*afV[2];
				kL[2][2] = T(1)+fT2*afV[2]*afV[2];
				bIdentity = false;
			}
			else
			{
				kL = Matrix3Generic<T>::IDENTITY;
				bIdentity = true;
			}

			// map first row to (*,*,0)
			fLength = Maths::Sqrt(kA[0][1] * kA[0][1] + kA[0][2] * kA[0][2]);
			if ( fLength > T(0) )
			{
				fSign = (kA[0][1] > T(0) ? T(1) : T(-1));
				fT1 = kA[0][1] + fSign*fLength;
				afV[2] = kA[0][2]/fT1;

				fT2 = T(-2)/(T(1)+afV[2]*afV[2]);
				afW[0] = fT2*(kA[0][1]+kA[0][2]*afV[2]);
				afW[1] = fT2*(kA[1][1]+kA[1][2]*afV[2]);
				afW[2] = fT2*(kA[2][1]+kA[2][2]*afV[2]);
				kA[0][1] += afW[0];
				kA[1][1] += afW[1];
				kA[1][2] += afW[1]*afV[2];
				kA[2][1] += afW[2];
				kA[2][2] += afW[2]*afV[2];

				kR[0][0] = T(1);
				kR[0][1] = kR[1][0] = T(0);
				kR[0][2] = kR[2][0] = T(0);
				kR[1][1] = T(1)+fT2;
				kR[1][2] = kR[2][1] = fT2*afV[2];
				kR[2][2] = T(1)+fT2*afV[2]*afV[2];
			}
			else
			{
				kR = Matrix3Generic<T>::IDENTITY;
			}

			// map second column to (*,*,0)
			fLength = Maths::Sqrt(kA[1][1] * kA[1][1] + kA[2][1] * kA[2][1]);
			if ( fLength > T(0) )
			{
				fSign = (kA[1][1] > T(0) ? T(1) : T(-1));
				fT1 = kA[1][1] + fSign*fLength;
				afV[2] = kA[2][1]/fT1;

				fT2 = T(-2)/(T(1)+afV[2]*afV[2]);
				afW[1] = fT2*(kA[1][1]+kA[2][1]*afV[2]);
				afW[2] = fT2*(kA[1][2]+kA[2][2]*afV[2]);
				kA[1][1] += afW[1];
				kA[1][2] += afW[2];
				kA[2][2] += afV[2]*afW[2];

				T fA = T(1)+fT2;
				T fB = fT2*afV[2];
				T fC = T(1)+fB*afV[2];

				if ( bIdentity )
				{
					kL[0][0] = T(1);
					kL[0][1] = kL[1][0] = T(0);
					kL[0][2] = kL[2][0] = T(0);
					kL[1][1] = fA;
					kL[1][2] = kL[2][1] = fB;
					kL[2][2] = fC;
				}
				else
				{
					for (int row = 0; row < 3; row++)
					{
						T fTmp0 = kL[row][1];
						T fTmp1 = kL[row][2];
						kL[row][1] = fA*fTmp0+fB*fTmp1;
						kL[row][2] = fB*fTmp0+fC*fTmp1;
					}
				}
			}
		}
		
		static void GolubKahanStep(Matrix3Generic<T>& kA, Matrix3Generic<T>& kL, Matrix3Generic<T>& kR)
		{
			T fT11 = kA[0][1]*kA[0][1]+kA[1][1]*kA[1][1];
			T fT22 = kA[1][2]*kA[1][2]+kA[2][2]*kA[2][2];
			T fT12 = kA[1][1]*kA[1][2];
			T fTrace = fT11+fT22;
			T fDiff = fT11-fT22;
			T fDiscr = Maths::Sqrt(fDiff * fDiff + 4.0f * fT12 * fT12);
			T fRoot1 = 0.5f*(fTrace+fDiscr);
			T fRoot2 = 0.5f*(fTrace-fDiscr);

			// adjust right
			T fY = kA[0][0] - (Maths::Abs(fRoot1 - fT22) <= Maths::Abs(fRoot2 - fT22) ? fRoot1 : fRoot2);
			T fZ = kA[0][1];
			T fInvLength = Maths::InvSqrt(fY * fY + fZ * fZ);
			T fSin = fZ*fInvLength;
			T fCos = -fY*fInvLength;

			T fTmp0 = kA[0][0];
			T fTmp1 = kA[0][1];
			kA[0][0] = fCos*fTmp0-fSin*fTmp1;
			kA[0][1] = fSin*fTmp0+fCos*fTmp1;
			kA[1][0] = -fSin*kA[1][1];
			kA[1][1] *= fCos;

			Size row;
			for (row = 0; row < 3; row++)
			{
				fTmp0 = kR[0][row];
				fTmp1 = kR[1][row];
				kR[0][row] = fCos*fTmp0-fSin*fTmp1;
				kR[1][row] = fSin*fTmp0+fCos*fTmp1;
			}

			// adjust left
			fY = kA[0][0];
			fZ = kA[1][0];
			fInvLength = Maths::InvSqrt(fY * fY + fZ * fZ);
			fSin = fZ*fInvLength;
			fCos = -fY*fInvLength;

			kA[0][0] = fCos*kA[0][0]-fSin*kA[1][0];
			fTmp0 = kA[0][1];
			fTmp1 = kA[1][1];
			kA[0][1] = fCos*fTmp0-fSin*fTmp1;
			kA[1][1] = fSin*fTmp0+fCos*fTmp1;
			kA[0][2] = -fSin*kA[1][2];
			kA[1][2] *= fCos;

			Size col;
			for (col = 0; col < 3; col++)
			{
				fTmp0 = kL[col][0];
				fTmp1 = kL[col][1];
				kL[col][0] = fCos*fTmp0-fSin*fTmp1;
				kL[col][1] = fSin*fTmp0+fCos*fTmp1;
			}

			// adjust right
			fY = kA[0][1];
			fZ = kA[0][2];
			fInvLength = Maths::InvSqrt(fY * fY + fZ * fZ);
			fSin = fZ*fInvLength;
			fCos = -fY*fInvLength;

			kA[0][1] = fCos*kA[0][1]-fSin*kA[0][2];
			fTmp0 = kA[1][1];
			fTmp1 = kA[1][2];
			kA[1][1] = fCos*fTmp0-fSin*fTmp1;
			kA[1][2] = fSin*fTmp0+fCos*fTmp1;
			kA[2][1] = -fSin*kA[2][2];
			kA[2][2] *= fCos;

			for (row = 0; row < 3; row++)
			{
				fTmp0 = kR[1][row];
				fTmp1 = kR[2][row];
				kR[1][row] = fCos*fTmp0-fSin*fTmp1;
				kR[2][row] = fSin*fTmp0+fCos*fTmp1;
			}

			// adjust left
			fY = kA[1][1];
			fZ = kA[2][1];
			fInvLength = Maths::InvSqrt(fY * fY + fZ * fZ);
			fSin = fZ*fInvLength;
			fCos = -fY*fInvLength;

			kA[1][1] = fCos*kA[1][1]-fSin*kA[2][1];
			fTmp0 = kA[1][2];
			fTmp1 = kA[2][2];
			kA[1][2] = fCos*fTmp0-fSin*fTmp1;
			kA[2][2] = fSin*fTmp0+fCos*fTmp1;

			for (col = 0; col < 3; col++)
			{
				fTmp0 = kL[col][1];
				fTmp1 = kL[col][2];
				kL[col][1] = fCos*fTmp0-fSin*fTmp1;
				kL[col][2] = fSin*fTmp0+fCos*fTmp1;
			}
		}
		
		// support for spectral norm
		static T MaxCubicRoot (T afCoeff[3])
		{
			// Spectral norm is for A^T*A, so characteristic polynomial
			// P(x) = c[0]+c[1]*x+c[2]*x^2+x^3 has three positive real roots.
			// This yields the assertions c[0] < 0 and c[2]*c[2] >= 3*c[1].

			// quick out for uniform scale (triple root)
			const T fOneThird = T(1)/T(3);
			const T fEpsilon = 1e-06;
			T fDiscr = afCoeff[2]*afCoeff[2] - T(3)*afCoeff[1];
			if ( fDiscr <= fEpsilon )
				return -fOneThird*afCoeff[2];

			// Compute an upper bound on roots of P(x).  This assumes that A^T*A
			// has been scaled by its largest entry.
			T fX = T(1);
			T fPoly = afCoeff[0]+fX*(afCoeff[1]+fX*(afCoeff[2]+fX));
			if ( fPoly < T(0) )
			{
				// uses a matrix norm to find an upper bound on maximum root
				fX = Maths::Abs(afCoeff[0]);
				T fTmp = T(1) + Maths::Abs(afCoeff[1]);
				if ( fTmp > fX )
					fX = fTmp;
				fTmp = T(1) + Maths::Abs(afCoeff[2]);
				if ( fTmp > fX )
					fX = fTmp;
			}

			// Newton's method to find root
			T fTwoC2 = T(2)*afCoeff[2];
			for (int i = 0; i < 16; i++)
			{
				fPoly = afCoeff[0]+fX*(afCoeff[1]+fX*(afCoeff[2]+fX));
				if(Maths::Abs(fPoly) <= fEpsilon)
				{
					return fX;
				}

				T fDeriv = afCoeff[1]+fX*(fTwoC2+T(3)*fX);
				fX -= fPoly/fDeriv;
			}

			return fX;
		}
		
		T m[3][3];

		// for faster access
		friend class Matrix4;
	};
	
	template<typename T> const T Matrix3Generic<T>::EPSILON = T(1e-06);
	template<typename T> const Matrix3Generic<T> Matrix3Generic<T>::ZERO(0, 0, 0, 0, 0, 0, 0, 0, 0);
	template<typename T> const Matrix3Generic<T> Matrix3Generic<T>::IDENTITY(1, 0, 0, 0, 1, 0, 0, 0, 1);
	template<typename T> const T Matrix3Generic<T>::ms_fSvdEpsilon = T(1e-04);
	template<typename T> const unsigned int Matrix3Generic<T>::ms_iSvdMaxIterations = 32;

	typedef Matrix3Generic<f32> Matrix3;
	/** @} */
	/** @} */
}
#endif
