#include <echo/Maths/Quaternion.h>
#include <echo/Maths/Matrix3.h>
#include <echo/Maths/Vector3.h>
#include <echo/Maths/Matrix4.h>
namespace Echo
{
	const f32 Quaternion::ms_fEpsilon = 1e-03f;
	const Quaternion Quaternion::ZERO(0.0f, 0.0f, 0.0f, 0.0f);
	const Quaternion Quaternion::IDENTITY(0.0f, 0.0f, 0.0f, 1.0f);

	//-----------------------------------------------------------------------
	void Quaternion::FromRotationMatrix(const Matrix3& kRot)
	{
		// Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
		// article "Quaternion Calculus and Fast Animation".

		f32 fTrace = kRot[0][0]+kRot[1][1]+kRot[2][2];
		f32 fRoot;

		if ( fTrace > 0.0 )
		{
			// |w| > 1/2, may as well choose w > 1/2
			fRoot = Maths::Sqrt(fTrace + 1.0f); // 2w
			w = 0.5f*fRoot;
			fRoot = 0.5f/fRoot;  // 1/(4w)
			x = (kRot[2][1]-kRot[1][2])*fRoot;
			y = (kRot[0][2]-kRot[2][0])*fRoot;
			z = (kRot[1][0]-kRot[0][1])*fRoot;
		}
		else
		{
			// |w| <= 1/2
			static size_t s_iNext[3] = { 1, 2, 0 };
			size_t i = 0;
			if ( kRot[1][1] > kRot[0][0] )
				i = 1;
			if ( kRot[2][2] > kRot[i][i] )
				i = 2;
			size_t j = s_iNext[i];
			size_t k = s_iNext[j];

			fRoot = Maths::Sqrt(kRot[i][i] - kRot[j][j] - kRot[k][k] + 1.0f);
			f32* apkQuat[3] = { &x, &y, &z };
			*apkQuat[i] = 0.5f*fRoot;
			fRoot = 0.5f/fRoot;
			w = (kRot[k][j]-kRot[j][k])*fRoot;
			*apkQuat[j] = (kRot[j][i]+kRot[i][j])*fRoot;
			*apkQuat[k] = (kRot[k][i]+kRot[i][k])*fRoot;
		}
	}
	//-----------------------------------------------------------------------
	void Quaternion::ToRotationMatrix(Matrix3& kRot) const
	{
		f32 fTx  = x+x;
		f32 fTy  = y+y;
		f32 fTz  = z+z;
		f32 fTwx = fTx*w;
		f32 fTwy = fTy*w;
		f32 fTwz = fTz*w;
		f32 fTxx = fTx*x;
		f32 fTxy = fTy*x;
		f32 fTxz = fTz*x;
		f32 fTyy = fTy*y;
		f32 fTyz = fTz*y;
		f32 fTzz = fTz*z;

		kRot[0][0] = 1.0f-(fTyy+fTzz);
		kRot[0][1] = fTxy-fTwz;
		kRot[0][2] = fTxz+fTwy;
		kRot[1][0] = fTxy+fTwz;
		kRot[1][1] = 1.0f-(fTxx+fTzz);
		kRot[1][2] = fTyz-fTwx;
		kRot[2][0] = fTxz-fTwy;
		kRot[2][1] = fTyz+fTwx;
		kRot[2][2] = 1.0f-(fTxx+fTyy);
	}
	//-----------------------------------------------------------------------
	void Quaternion::FromAngleAxis(const Radian& rfAngle, const Vector3& rkAxis)
	{
		// assert:  axis[] is unit length
		//
		// The quaternion representing the rotation is
		//   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

		Radian fHalfAngle ( 0.5*rfAngle );
		f32 fSin = Maths::Sin<f32>(fHalfAngle);
		w = Maths::Cos<f32>(fHalfAngle);
		x = fSin*rkAxis.x;
		y = fSin*rkAxis.y;
		z = fSin*rkAxis.z;
	}
	//-----------------------------------------------------------------------
	void Quaternion::ToAngleAxis(Radian& rfAngle, Vector3& rkAxis) const
	{
		// The quaternion representing the rotation is
		//   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

		f32 fSqrLength = x*x+y*y+z*z;
		if ( fSqrLength > 0.0 )
		{
			rfAngle = 2.0 * Maths::ACos(w);
			f32 fInvLength = Maths::InvSqrt(fSqrLength);
			rkAxis.x = x*fInvLength;
			rkAxis.y = y*fInvLength;
			rkAxis.z = z*fInvLength;
		}
		else
		{
			// angle is 0 (mod 2*pi), so any axis will do
			rfAngle = f32(0.0);
			rkAxis.x = 1.0;
			rkAxis.y = 0.0;
			rkAxis.z = 0.0;
		}
	}
	//-----------------------------------------------------------------------
	void Quaternion::FromAxes(const Vector3* akAxis)
	{
		Matrix3 kRot;

		for (size_t iCol = 0; iCol < 3; iCol++)
		{
			kRot[0][iCol] = akAxis[iCol].x;
			kRot[1][iCol] = akAxis[iCol].y;
			kRot[2][iCol] = akAxis[iCol].z;
		}

		FromRotationMatrix(kRot);
	}
	//-----------------------------------------------------------------------
	void Quaternion::FromAxes(const Vector3& xaxis, const Vector3& yaxis, const Vector3& zaxis)
	{
		Matrix3 kRot;

		kRot[0][0] = xaxis.x;
		kRot[1][0] = xaxis.y;
		kRot[2][0] = xaxis.z;

		kRot[0][1] = yaxis.x;
		kRot[1][1] = yaxis.y;
		kRot[2][1] = yaxis.z;

		kRot[0][2] = zaxis.x;
		kRot[1][2] = zaxis.y;
		kRot[2][2] = zaxis.z;

		FromRotationMatrix(kRot);

	}
	//-----------------------------------------------------------------------
	void Quaternion::ToAxes(Vector3* akAxis) const
	{
		Matrix3 kRot;

		ToRotationMatrix(kRot);

		for (size_t iCol = 0; iCol < 3; iCol++)
		{
			akAxis[iCol].x = kRot[0][iCol];
			akAxis[iCol].y = kRot[1][iCol];
			akAxis[iCol].z = kRot[2][iCol];
		}
	}
	//-----------------------------------------------------------------------
	Vector3 Quaternion::XAxis(void) const
	{
		//f32 fTx  = 2.0*x;
		f32 fTy  = 2.0f*y;
		f32 fTz  = 2.0f*z;
		f32 fTwy = fTy*w;
		f32 fTwz = fTz*w;
		f32 fTxy = fTy*x;
		f32 fTxz = fTz*x;
		f32 fTyy = fTy*y;
		f32 fTzz = fTz*z;

		return Vector3(1.0f - (fTyy + fTzz), fTxy + fTwz, fTxz - fTwy);
	}
	//-----------------------------------------------------------------------
	Vector3 Quaternion::YAxis(void) const
	{
		f32 fTx  = 2.0f*x;
		f32 fTy  = 2.0f*y;
		f32 fTz  = 2.0f*z;
		f32 fTwx = fTx*w;
		f32 fTwz = fTz*w;
		f32 fTxx = fTx*x;
		f32 fTxy = fTy*x;
		f32 fTyz = fTz*y;
		f32 fTzz = fTz*z;

		return Vector3(fTxy - fTwz, 1.0f - (fTxx + fTzz), fTyz + fTwx);
	}
	//-----------------------------------------------------------------------
	Vector3 Quaternion::ZAxis(void) const
	{
		f32 fTx  = 2.0f*x;
		f32 fTy  = 2.0f*y;
		f32 fTz  = 2.0f*z;
		f32 fTwx = fTx*w;
		f32 fTwy = fTy*w;
		f32 fTxx = fTx*x;
		f32 fTxz = fTz*x;
		f32 fTyy = fTy*y;
		f32 fTyz = fTz*y;

		return Vector3(fTxz + fTwy, fTyz - fTwx, 1.0f - (fTxx + fTyy));
	}
	//-----------------------------------------------------------------------
	void Quaternion::ToAxes(Vector3& xaxis, Vector3& yaxis, Vector3& zaxis) const
	{
		Matrix3 kRot;

		ToRotationMatrix(kRot);

		xaxis.x = kRot[0][0];
		xaxis.y = kRot[1][0];
		xaxis.z = kRot[2][0];

		yaxis.x = kRot[0][1];
		yaxis.y = kRot[1][1];
		yaxis.z = kRot[2][1];

		zaxis.x = kRot[0][2];
		zaxis.y = kRot[1][2];
		zaxis.z = kRot[2][2];
	}

	//-----------------------------------------------------------------------
	Quaternion Quaternion::operator+(const Quaternion& rkQ) const
	{
		return Quaternion(x + rkQ.x, y + rkQ.y, z + rkQ.z, w + rkQ.w);
	}
	//-----------------------------------------------------------------------
	Quaternion Quaternion::operator-(const Quaternion& rkQ) const
	{
		return Quaternion(x - rkQ.x, y - rkQ.y, z - rkQ.z, w - rkQ.w);
	}
	//-----------------------------------------------------------------------
	Quaternion Quaternion::operator*(const Quaternion& rkQ) const
	{
		// NOTE:  Multiplication is not generally commutative, so in most
		// cases p*q != q*p.

		return Quaternion
		(
			w * rkQ.x + x * rkQ.w + y * rkQ.z - z * rkQ.y,
			w * rkQ.y + y * rkQ.w + z * rkQ.x - x * rkQ.z,
			w * rkQ.z + z * rkQ.w + x * rkQ.y - y * rkQ.x,
			w * rkQ.w - x * rkQ.x - y * rkQ.y - z * rkQ.z
		);
	}
	//-----------------------------------------------------------------------
	Quaternion Quaternion::operator*(f32 fScalar) const
	{
		return Quaternion(fScalar*x, fScalar*y, fScalar*z, fScalar * w);
	}
	//-----------------------------------------------------------------------
	Quaternion operator*(f32 fScalar, const Quaternion& rkQ)
	{
		return Quaternion(fScalar * rkQ.x, fScalar * rkQ.y,
			fScalar*rkQ.z,fScalar*rkQ.w);
	}
	//-----------------------------------------------------------------------
	Quaternion Quaternion::operator-() const
	{
		return Quaternion(-x, -y, -z, -w);
	}
	//-----------------------------------------------------------------------
	f32 Quaternion::Dot(const Quaternion& rkQ) const
	{
		return w*rkQ.w+x*rkQ.x+y*rkQ.y+z*rkQ.z;
	}
	//-----------------------------------------------------------------------
	f32 Quaternion::LengthSquared() const
	{
		return w*w+x*x+y*y+z*z;
	}
	//-----------------------------------------------------------------------
	Quaternion Quaternion::Inverse() const
	{
		f32 fNorm = w*w+x*x+y*y+z*z;
		if ( fNorm > 0.0 )
		{
			f32 fInvNorm = 1.0f/fNorm;
			return Quaternion(-x*fInvNorm, -y*fInvNorm, -z*fInvNorm, w * fInvNorm);
		}
		else
		{
			// return an invalid result to flag the error
			return ZERO;
		}
	}
	//-----------------------------------------------------------------------
	Quaternion Quaternion::UnitInverse() const
	{
		// assert:  'this' is unit length
		return Quaternion(-x, -y, -z, w);
	}
	//-----------------------------------------------------------------------
	Quaternion Quaternion::Exp() const
	{
		// If q = A*(x*i+y*j+z*k) where (x,y,z) is unit length, then
		// exp(q) = cos(A)+sin(A)*(x*i+y*j+z*k).  If sin(A) is near zero,
		// use exp(q) = cos(A)+A*(x*i+y*j+z*k) since A/sin(A) has limit 1.

		Radian fAngle(Maths::Sqrt(x * x + y * y + z * z));
		f32 fSin = Maths::Sin<f32>(fAngle);

		Quaternion kResult;
		kResult.w = Maths::Cos<f32>(fAngle);

		if(Maths::Abs(fSin) >= ms_fEpsilon)
		{
			f32 fCoeff = fSin/(fAngle.ValueRadians());
			kResult.x = fCoeff*x;
			kResult.y = fCoeff*y;
			kResult.z = fCoeff*z;
		}
		else
		{
			kResult.x = x;
			kResult.y = y;
			kResult.z = z;
		}

		return kResult;
	}
	//-----------------------------------------------------------------------
	Quaternion Quaternion::Log() const
	{
		// If q = cos(A)+sin(A)*(x*i+y*j+z*k) where (x,y,z) is unit length, then
		// log(q) = A*(x*i+y*j+z*k).  If sin(A) is near zero, use log(q) =
		// sin(A)*(x*i+y*j+z*k) since sin(A)/A has limit 1.

		Quaternion kResult;
		kResult.w = 0.0;

		if(Maths::Abs(w) < 1.0)
			{
			Radian fAngle(Maths::ACos(w));
			f32 fSin = Maths::Sin<f32>(fAngle);
			if(Maths::Abs(fSin) >= ms_fEpsilon)
			{
				f32 fCoeff = fAngle.ValueRadians()/fSin;
				kResult.x = fCoeff*x;
				kResult.y = fCoeff*y;
				kResult.z = fCoeff*z;
				return kResult;
			}
		}

		kResult.x = x;
		kResult.y = y;
		kResult.z = z;

		return kResult;
	}
	//-----------------------------------------------------------------------
	Vector3 Quaternion::operator*(const Vector3& v) const
	{
		// nVidia SDK implementation
		Vector3 uv, uuv;
		Vector3 qvec(x, y, z);
		uv = qvec.Cross(v);
		uuv = qvec.Cross(uv);
		uv *= (2.0f * w);
		uuv *= 2.0f;

		return v + uv + uuv;

	}
	//-----------------------------------------------------------------------
	bool Quaternion::Equals(const Quaternion& rhs, const Radian& tolerance) const
	{
		f32 fCos = Dot(rhs);
		Radian angle = Maths::ACos(fCos);

		return(Maths::Abs(angle.ValueRadians()) <= tolerance.ValueRadians())
				|| Maths::RealEqual(angle.ValueRadians(), Maths::PI, tolerance.ValueRadians());


	}
	//-----------------------------------------------------------------------
	Quaternion Quaternion::Slerp(f32 fT, const Quaternion& rkP,
								 const Quaternion& rkQ, bool shortestPath)
	{
		f32 fCos = rkP.Dot(rkQ);
		Quaternion rkT;

		// Do we need to invert rotation?
		if (fCos < 0.0f && shortestPath)
		{
			fCos = -fCos;
			rkT = -rkQ;
		}
		else
		{
			rkT = rkQ;
		}

		if(Maths::Abs(fCos) < 1 - ms_fEpsilon)
		{
			// Standard case (slerp)
			f32 fSin = Maths::Sqrt(1 - Maths::Sqr(fCos));
			Radian fAngle = Maths::ATan2(fSin, fCos);
			f32 fInvSin = 1.0f / fSin;
			f32 fCoeff0 = Maths::Sin<f32>((1.0f - fT) * fAngle) * fInvSin;
			f32 fCoeff1 = Maths::Sin<f32>(fT * fAngle) * fInvSin;
			return fCoeff0 * rkP + fCoeff1 * rkT;
		}
		else
		{
			// There are two situations:
			// 1. "rkP" and "rkQ" are very close (fCos ~= +1), so we can do a linear
			//	interpolation safely.
			// 2. "rkP" and "rkQ" are almost inverse of each other (fCos ~= -1), there
			//	are an infinite number of possibilities interpolation. but we haven't
			//	have method to fix this case, so just use linear interpolation here.
			Quaternion t = (1.0f - fT) * rkP + fT * rkT;
			// taking the complement requires renormalisation
			t.Normalise();
			return t;
		}
	}
	//-----------------------------------------------------------------------
	Quaternion Quaternion::SlerpExtraSpins(f32 fT,
										   const Quaternion& rkP, const Quaternion& rkQ, int iExtraSpins)
	{
		f32 fCos = rkP.Dot(rkQ);
		Radian fAngle(Maths::ACos(fCos));

		if(Maths::Abs(fAngle.ValueRadians()) < ms_fEpsilon)
			return rkP;

		f32 fSin = Maths::Sin<f32>(fAngle);
		f32 fPhase(Maths::PI * iExtraSpins * fT);
		f32 fInvSin = 1.0f/fSin;
		f32 fCoeff0 = Maths::Sin((1.0f - fT) * fAngle.ValueRadians() - fPhase) * fInvSin;
		f32 fCoeff1 = Maths::Sin(fT * fAngle.ValueRadians() + fPhase) * fInvSin;
		return fCoeff0*rkP + fCoeff1*rkQ;
	}
	//-----------------------------------------------------------------------
	void Quaternion::Intermediate(const Quaternion& rkQ0,
								  const Quaternion& rkQ1, const Quaternion& rkQ2,
								  Quaternion& rkA, Quaternion& rkB)
	{
		// assert:  q0, q1, q2 are unit quaternions

		Quaternion kQ0inv = rkQ0.UnitInverse();
		Quaternion kQ1inv = rkQ1.UnitInverse();
		Quaternion rkP0 = kQ0inv*rkQ1;
		Quaternion rkP1 = kQ1inv*rkQ2;
		Quaternion kArg = 0.25 * (rkP0.Log() - rkP1.Log());
		Quaternion kMinusArg = -kArg;

		rkA = rkQ1*kArg.Exp();
		rkB = rkQ1*kMinusArg.Exp();
	}
	//-----------------------------------------------------------------------
	Quaternion Quaternion::Squad(f32 fT,
								 const Quaternion& rkP, const Quaternion& rkA,
								 const Quaternion& rkB, const Quaternion& rkQ, bool shortestPath)
	{
		f32 fSlerpT = 2.0f*fT*(1.0f-fT);
		Quaternion kSlerpP = Slerp(fT, rkP, rkQ, shortestPath);
		Quaternion kSlerpQ = Slerp(fT, rkA, rkB);
		return Slerp(fSlerpT, kSlerpP ,kSlerpQ);
	}
	//-----------------------------------------------------------------------
	f32 Quaternion::Normalise(void)
	{
		f32 len = LengthSquared();
		f32 factor = 1.0f / Maths::Sqrt(len);
		*this = *this * factor;
		return len;
	}
	//-----------------------------------------------------------------------
	Radian Quaternion::GetRoll(bool reprojectAxis) const
	{
		if (reprojectAxis)
		{
			// roll = atan2(localx.y, localx.x)
			// pick parts of xAxis() implementation that we need
//			f32 fTx  = 2.0*x;
			f32 fTy  = 2.0f*y;
			f32 fTz  = 2.0f*z;
			f32 fTwz = fTz*w;
			f32 fTxy = fTy*x;
			f32 fTyy = fTy*y;
			f32 fTzz = fTz*z;

			// Vector3(1.0-(fTyy+fTzz), fTxy+fTwz, fTxz-fTwy);

			return Radian(Maths::ATan2(fTxy + fTwz, 1.0f - (fTyy + fTzz)));

		}
		else
		{
			return Radian(Maths::ATan2(2 * (x * y + w * z), w * w + x * x - y * y - z * z));
		}
	}
	//-----------------------------------------------------------------------
	Radian Quaternion::GetPitch(bool reprojectAxis) const
	{
		if (reprojectAxis)
		{
			// pitch = atan2(localy.z, localy.y)
			// pick parts of YAxis() implementation that we need
			f32 fTx  = 2.0f*x;
//			f32 fTy  = 2.0f*y;
			f32 fTz  = 2.0f*z;
			f32 fTwx = fTx*w;
			f32 fTxx = fTx*x;
			f32 fTyz = fTz*y;
			f32 fTzz = fTz*z;

			// Vector3(fTxy-fTwz, 1.0-(fTxx+fTzz), fTyz+fTwx);
			return Radian(Maths::ATan2(fTyz + fTwx, 1.0f - (fTxx + fTzz)));
		}
		else
		{
			// internal version
			return Radian(Maths::ATan2(2 * (y * z + w * x), w * w - x * x - y * y + z * z));
		}
	}
	//-----------------------------------------------------------------------
	Radian Quaternion::GetYaw(bool reprojectAxis) const
	{
		if (reprojectAxis)
		{
			// yaw = atan2(localz.x, localz.z)
			// pick parts of ZAxis() implementation that we need
			f32 fTx  = 2.0f*x;
			f32 fTy  = 2.0f*y;
			f32 fTz  = 2.0f*z;
			f32 fTwy = fTy*w;
			f32 fTxx = fTx*x;
			f32 fTxz = fTz*x;
			f32 fTyy = fTy*y;

			// Vector3(fTxz+fTwy, fTyz-fTwx, 1.0-(fTxx+fTyy));

			return Radian(Maths::ATan2(fTxz + fTwy, 1.0f - (fTxx + fTyy)));

		}
		else
		{
			// internal version
			return Radian(Maths::ASin(-2 * (x * z - w * y)));
		}
	}
	//-----------------------------------------------------------------------
	Quaternion Quaternion::Nlerp(const Quaternion& rkQ, f32 fT, bool shortestPath) const
	{
		const Quaternion& rkP = *this;
		Quaternion result;
		f32 fCos = rkP.Dot(rkQ);
		if (fCos < 0.0f && shortestPath)
		{
			result = rkP + fT * ((-rkQ) - rkP);
		}
		else
		{
			result = rkP + fT * (rkQ - rkP);
		}
		result.Normalise();
		return result;
	}

	void Quaternion::SetAlongVector(const Vector3& vect, const Vector3& up)
	{
		Vector3 xAxis = vect.Cross(up);
		Vector3 yAxis = vect.Cross(xAxis);
		FromAxes(xAxis,yAxis,vect);
	}
}
