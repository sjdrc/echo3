#ifndef _ECHO_MATHS_H_
#define _ECHO_MATHS_H_

#include <echo/Types.h>
#include <ostream>
#include <sstream>
#include <vector>
#include <list>
#include <limits>
#include <cmath>
#include <assert.h>
#include <cstdlib>

namespace Echo
{
	class Plane;
	class Degree;
	class AxisAlignedBox;
	class Quaternion;
	class Ray;
	class Sphere;

	/** Wrapper class which indicates a given angle value is in Radians.
	@remarks
		Radian values are interchangeable with Degree values, and conversions
		will be done automatically between them.
	*/
	class Radian
	{
		f32 mRad;

	public:
		explicit Radian ( f32 r=0 ) : mRad(r) {}
		Radian ( const Degree& d );
		Radian& operator = ( const f32& f ) { mRad = f; return *this; }
		Radian& operator = ( const Radian& r ) { mRad = r.mRad; return *this; }
		Radian& operator = ( const Degree& d );

		f32 ValueDegrees() const; // see bottom of this file
		f32 ValueRadians() const { return mRad; }

		const Radian& operator + () const { return *this; }
		Radian operator + ( const Radian& r ) const { return Radian ( mRad + r.mRad ); }
		Radian operator + ( const Degree& d ) const;
		Radian& operator += ( const Radian& r ) { mRad += r.mRad; return *this; }
		Radian& operator += ( const Degree& d );
		Radian operator - () const { return Radian(-mRad); }
		Radian operator - ( const Radian& r ) const { return Radian ( mRad - r.mRad ); }
		Radian operator - ( const Degree& d ) const;
		Radian& operator -= ( const Radian& r ) { mRad -= r.mRad; return *this; }
		Radian& operator -= ( const Degree& d );
		Radian operator * ( f32 f ) const { return Radian ( mRad * f ); }
		Radian operator * ( const Radian& f ) const { return Radian ( mRad * f.mRad ); }
		Radian& operator *= ( f32 f ) { mRad *= f; return *this; }
		Radian operator / ( f32 f ) const { return Radian ( mRad / f ); }
		f32 operator / ( const Radian& f ) const { return mRad / f.mRad; }
		Radian& operator /= ( f32 f ) { mRad /= f; return *this; }

		bool operator <  ( const Radian& r ) const { return mRad <  r.mRad; }
		bool operator <= ( const Radian& r ) const { return mRad <= r.mRad; }
		bool operator == ( const Radian& r ) const { return mRad == r.mRad; }
		bool operator != ( const Radian& r ) const { return mRad != r.mRad; }
		bool operator >= ( const Radian& r ) const { return mRad >= r.mRad; }
		bool operator >  ( const Radian& r ) const { return mRad >  r.mRad; }

		inline friend std::ostream& operator <<
			( std::ostream& o, const Radian& v )
		{
			o << "Radian(" << v.ValueRadians() << ")";
			return o;
		}
		
		friend std::istream& operator >>(std::istream& i, Radian& v);

	};

	/** Wrapper class which indicates a given angle value is in Degrees.
	@remarks
		Degree values are interchangeable with Radian values, and conversions
		will be done automatically between them.
	*/
	class Degree
	{
		f32 mDeg; // if you get an error here - make sure to define/typedef 'f32' first

	public:
		explicit Degree ( f32 d=0 ) : mDeg(d) {}
		Degree ( const Radian& r ) : mDeg(r.ValueDegrees()) {}
		Degree& operator = ( const f32& f ) { mDeg = f; return *this; }
		Degree& operator = ( const Degree& d ) { mDeg = d.mDeg; return *this; }
		Degree& operator = ( const Radian& r ) { mDeg = r.ValueDegrees(); return *this; }

		f32 ValueDegrees() const { return mDeg; }
		f32 ValueRadians() const; // see bottom of this file

		const Degree& operator + () const { return *this; }
		Degree operator + ( const Degree& d ) const { return Degree ( mDeg + d.mDeg ); }
		Degree operator + ( const Radian& r ) const { return Degree ( mDeg + r.ValueDegrees() ); }
		Degree& operator += ( const Degree& d ) { mDeg += d.mDeg; return *this; }
		Degree& operator += ( const Radian& r ) { mDeg += r.ValueDegrees(); return *this; }
		Degree operator - () const { return Degree(-mDeg); }
		Degree operator - ( const Degree& d ) const { return Degree ( mDeg - d.mDeg ); }
		Degree operator - ( const Radian& r ) const { return Degree ( mDeg - r.ValueDegrees() ); }
		Degree& operator -= ( const Degree& d ) { mDeg -= d.mDeg; return *this; }
		Degree& operator -= ( const Radian& r ) { mDeg -= r.ValueDegrees(); return *this; }
		Degree operator * ( f32 f ) const { return Degree ( mDeg * f ); }
		Degree operator * ( const Degree& f ) const { return Degree ( mDeg * f.mDeg ); }
		Degree& operator *= ( f32 f ) { mDeg *= f; return *this; }
		Degree operator / ( f32 f ) const { return Degree ( mDeg / f ); }
		Degree& operator /= ( f32 f ) { mDeg /= f; return *this; }

		bool operator <  ( const Degree& d ) const { return mDeg <  d.mDeg; }
		bool operator <= ( const Degree& d ) const { return mDeg <= d.mDeg; }
		bool operator == ( const Degree& d ) const { return mDeg == d.mDeg; }
		bool operator != ( const Degree& d ) const { return mDeg != d.mDeg; }
		bool operator >= ( const Degree& d ) const { return mDeg >= d.mDeg; }
		bool operator >  ( const Degree& d ) const { return mDeg >  d.mDeg; }

		inline friend std::ostream& operator <<
			( std::ostream& o, const Degree& v )
		{
			o << "Degree(" << v.ValueDegrees() << ")";
			return o;
		}

		inline friend std::istream& operator >>(std::istream& i, Degree& v)
		{
			std::string temp;
			i >> temp;	//Read the input chunk to the first whitespace.

			//temp should now be either one of the following formats:
			//1.  "Degree(degrees)" if it was converted using <<
			//2.  "degrees" if it was from some other input

			//Look for 1.

			//Set default values just incase of a failed conversion.
			v = Degree(0.f);

			size_t openBracket = temp.find_first_of('(');
			bool radianConversion = false;
			//Found an open bracket. Lets see what the string is before the open bracket. It should be Radian.
			if(openBracket!=std::string::npos)
			{
				//Cancel if the type isn't Radian or a Degree
				std::string type = temp.substr(0,openBracket);
				if(type=="Radian")
				{
					radianConversion = true;
				}else
				if(type!="Degree")
				{
					i.setstate(std::ios_base::failbit);
					return i;
				}
				//We're on track. Now find a close bracket
				size_t closeBracket = temp.find_first_of(')');
				if(closeBracket!=std::string::npos && closeBracket > openBracket)
				{
					//Now it should be radians option 2, so we'll pass the input to the next processing block.
					temp = temp.substr(openBracket+1, closeBracket - openBracket - 1);
				}
			}

			//Look for 2.			
			std::stringstream ss(temp);
			std::string t;
			if(radianConversion)
			{
				f32 convertedValue = 0.f;
				ss >> convertedValue;
				v = Radian(convertedValue);
			}else
			{
				ss >> v.mDeg;
			}
			if(ss.fail())
			{
				i.setstate(std::ios_base::failbit);
			}
			return i;
		}
	};

	// these functions could not be defined within the class definition of class
	// Radian because they required class Degree to be defined
	inline Radian::Radian ( const Degree& d ) : mRad(d.ValueRadians()) {
	}
	inline Radian& Radian::operator = ( const Degree& d ) {
		mRad = d.ValueRadians(); return *this;
	}
	inline Radian Radian::operator + ( const Degree& d ) const {
		return Radian ( mRad + d.ValueRadians() );
	}
	inline Radian& Radian::operator += ( const Degree& d ) {
		mRad += d.ValueRadians();
		return *this;
	}
	inline Radian Radian::operator - ( const Degree& d ) const {
		return Radian ( mRad - d.ValueRadians() );
	}
	inline Radian& Radian::operator -= ( const Degree& d ) {
		mRad -= d.ValueRadians();
		return *this;
	}
	
	inline std::istream& operator >>(std::istream& i, Radian& v)
	{
		std::string temp;
		i >> temp;	//Read the input chunk to the first whitespace.

		//temp should now be either one of the following formats:
		//1.  "Radian(radians)" if it was converted using <<
		//2.  "radians" if it was from some other input

		//Look for 1.

		//Set default values just incase of a failed conversion.
		v = Radian(0.f);

		size_t openBracket = temp.find_first_of('(');
		bool degreeConversion = false;
		//Found an open bracket. Lets see what the string is before the open bracket. It should be Radian.
		if(openBracket!=std::string::npos)
		{
			//Cancel if the type isn't Radian or a Degree
			std::string type = temp.substr(0,openBracket);
			if(type=="Degree")
			{
				degreeConversion = true;
			}else
			if(type!="Radian")
			{
				i.setstate(std::ios_base::failbit);
				return i;
			}
			//We're on track. Now find a close bracket
			size_t closeBracket = temp.find_first_of(')');
			if(closeBracket!=std::string::npos && closeBracket > openBracket)
			{
				//Now it should be radians option 2, so we'll pass the input to the next processing block.
				temp = temp.substr(openBracket+1, closeBracket - openBracket - 1);
			}
		}

		//Look for 2.			
		std::stringstream ss(temp);
		std::string t;
		if(degreeConversion)
		{
			f32 convertedValue = 0.f;
			ss >> convertedValue;
			v = Degree(convertedValue);
		}else
		{
			ss >> v.mRad;
		}
		if(ss.fail())
		{
			i.setstate(std::ios_base::failbit);
		}
		return i;
	}

	/** Namespace to provide access to common mathematical functions.
		@remarks
			Most of the maths functions are aliased versions of the C runtime
			library functions. They are aliased here to provide future
			optimisation opportunities, either from faster RTLs or custom
			math approximations.
		@note
			<br>This is based on MgcMath.h from
			<a href="http://www.geometrictools.com/">Wild Magic</a>.
	 */
	namespace Maths
	{
		class AutoInitialiser
		{
		public:
			/** Default constructor.
				@param
					trigTableSize Optional parameter to set the size of the
					tables used to implement Sin, Cos, Tan
			 */
			AutoInitialiser(unsigned int trigTableSize = 4096);
			~AutoInitialiser();
		};
		
		extern AutoInitialiser AUTO_INITIALISER;
		
		const f32 POS_INFINITY = std::numeric_limits<f32>::infinity();
		const f32 NEG_INFINITY = -std::numeric_limits<f32>::infinity();
		const f32 PI = f32( 4.0 * std::atan( 1.0 ) );
		const f32 TWO_PI = f32( 2.0 * PI );
		const f32 HALF_PI = f32( 0.5 * PI );
		const f32 fDeg2Rad = PI / f32(180.0);
		const f32 fRad2Deg = f32(180.0) / PI;
		const f32 LOG2 = std::log(f32(2.0));

		/// Size of the trig tables as determined by constructor.
		extern int mTrigTableSize;

		/// Radian -> index factor value ( mTrigTableSize / 2 * PI )
		extern f32 mTrigTableFactor;
		extern f32* mSinTable;
		extern f32* mTanTable;

		/** Private function to build trig tables.
		*/
		void buildTrigTables();

		f32 SinTable (f32 fValue);
		f32 TanTable (f32 fValue);

		inline int IAbs (int iValue) { return ( iValue >= 0 ? iValue : -iValue ); }

		template < typename T >
		inline int ICeil(T value) { return int(std::ceil(value)); }

		template < typename T >
		inline int IFloor(T value) { return int(std::floor(value)); }
		
		inline int ISign (int iValue)
		{
			return ( iValue > 0 ? +1 : ( iValue < 0 ? -1 : 0 ) );
		}

		template< typename T >
		inline T Abs(T fValue) { return T(std::fabs(fValue)); }
		
		inline Degree Abs(const Degree& dValue) { return Degree(std::fabs(dValue.ValueDegrees())); }
		inline Radian Abs(const Radian& rValue) { return Radian(std::fabs(rValue.ValueRadians())); }
		
		template < typename T >
		Radian ACos(T value)
		{
			if ( T(-1) < value )
			{
				if ( value < T(1) )
					return Radian(std::acos(value));
				else
					return Radian(T(0));
			}
			else
			{
				return Radian(PI);
			}
		}

		template < typename T >
		Radian ASin(T value)
		{
			if ( T(-1) < value )
			{
				if ( value < T(1) )
					return Radian(std::asin(value));
				else
					return Radian(HALF_PI);
			}
			else
			{
				return Radian(-HALF_PI);
			}
		}

		template < typename T >
		inline Radian ATan(T value) { return Radian(std::atan(value)); }

		template < typename T >
		inline Radian ATan2(T y, T x) { return Radian(std::atan2(y,x)); }
		
		template < typename T >
		inline T Ceil(T value) { return T(std::ceil(value)); }

		template < typename T >
		inline bool IsNaN(T f)
		{
			// std::isnan() is C99, not supported by all compilers
			// However NaN always fails this next test, no other number does.
			return f != f;
		}

		/**
		 * Determine whether or not a floating point number is finite.
		 * @return true if the value is finite, false if the value is NaN or INF
		 */
		template < typename T >
		inline bool IsFinite(T f)
		{
			return std::isfinite(f);
		}

		/** Cosine function.
			 @param
				 fValue Angle in radians
			 @param
				 useTables If true, uses lookup tables rather than
				 calculation - faster but less accurate.
		*/
		template< typename T >
		inline T Cos(const Radian& value, bool useTables = false)
		{
			return (!useTables) ? T(std::cos(value.ValueRadians())) : SinTable(value.ValueRadians() + HALF_PI);
		}
		
		/** Cosine function.
			 @param
				 fValue Angle in radians
			 @param
				 useTables If true, uses lookup tables rather than
				 calculation - faster but less accurate.
		*/
		template< typename T>
		inline T Cos(T value, bool useTables = false)
		{
			return (!useTables) ? T(std::cos(value)) : SinTable(value + HALF_PI);
		}

		template < typename T >
		inline T Exp(T value) { return T(std::exp(value)); }

		template < typename T >
		inline T Floor(T value) { return T(std::floor(value)); }

		template < typename T >
		inline T Log (T value) { return T(std::log(value)); }

		template < typename T >
		inline T Log2(T value) { return T(std::log(value)/LOG2); }

		template < typename T >
		inline T LogN(T base, T value) { return T(std::log(value)/std::log(base)); }

		template < typename T >
		inline T Pow(T base, T exponent) { return T(std::pow(base,exponent)); }

		template< typename T >
		T Sign (T value)
		{
			if ( value > 0.0 )
				return 1.0;

			if ( value < 0.0 )
				return -1.0;

			return 0.0;
		}
		
		inline Radian Sign( const Radian& rValue )
		{
			return Radian(Sign(rValue.ValueRadians()));
		}
		
		inline Degree Sign( const Degree& dValue )
		{
			return Degree(Sign(dValue.ValueDegrees()));
		}

		/** Sine function.
			 @param
				 fValue Angle in radians
			 @param
				 useTables If true, uses lookup tables rather than
				 calculation - faster but less accurate.
		*/
		template< typename T >
		inline T Sin(const Radian& fValue, bool useTables = false)
		{
			return (!useTables) ? T(std::sin(fValue.ValueRadians())) : SinTable(fValue.ValueRadians());
		}
		
		/** Sine function.
			 @param
				 fValue Angle in radians
			 @param
				 useTables If true, uses lookup tables rather than
				 calculation - faster but less accurate.
		*/
		template< typename T >
		inline T Sin(T fValue, bool useTables = false)
		{
			return (!useTables) ? T(std::sin(fValue)) : SinTable(fValue);
		}

		template< typename T >
		inline T Sqr (T value) { return value*value; }

		template< typename T >
		inline T Sqrt (T value) { return T(std::sqrt(value)); }

		inline Radian Sqrt (const Radian& value) { return Radian(std::sqrt(value.ValueRadians())); }

		inline Degree Sqrt (const Degree& value) { return Degree(std::sqrt(value.ValueDegrees())); }

		/** Inverse square root i.e. 1 / Sqrt(x), good for vector normalisation.
		*/
		template< typename T >
		T InvSqrt(T value)
		{
			return T(1) / sqrt( value );
		}

		template< typename T >
		T UnitRandom ()  // in [0,1]
		{
			return rand() / T(RAND_MAX);
		}

		template< typename T >
		T RangeRandom(T low, T high)  // in [fLow,fHigh]
		{
			return (high-low)*UnitRandom<T>() + low;
		}

		template< typename T >
		T SymmetricRandom () // in [-1,1]
		{
			return T(2) * UnitRandom<T>() - T(1);
		}

		//returns a random integer
		inline int Random()
		{
			return rand();
		}

		//returns a random integer between minValue and maxValue
		inline int Random(int minValue, int maxValue)
		{
			maxValue=maxValue-minValue;
			return minValue+rand()%maxValue;
		}

		/** Tangent function.
			 @param
				 fValue Angle in radians
			 @param
				 useTables If true, uses lookup tables rather than
				 calculation - faster but less accurate.
		*/
		template< typename T >
		inline T Tan(const Radian& fValue, bool useTables = false)
		{
			return (!useTables) ? T(std::tan(fValue.ValueRadians())) : TanTable(fValue.ValueRadians());
		}
		
		/** Tangent function.
			 @param
				 fValue Angle in radians
			 @param
				 useTables If true, uses lookup tables rather than
				 calculation - faster but less accurate.
		*/
		template< typename T >
		inline T Tan(T fValue, bool useTables = false)
		{
			return (!useTables) ? T(std::tan(fValue)) : TanTable(fValue);
		}

		/**
		 * Return the hypotenuse of a right angle triangle given the two other sides.
		 * It is assumed that the parameters are in the same units.
		 * @param a Side a length.
		 * @param b Side b length.
		 * @return The length of the hypotenuse.
		 */
		template< typename T >
		inline T Hypotenuse(T a, T b)
		{
			return Sqrt(a*a + b*b);
		}

		inline f32 DegreesToRadians(f32 degrees) { return degrees * fDeg2Rad; }
		inline f32 RadiansToDegrees(f32 radians) { return radians * fRad2Deg; }

		/** Compare 2 reals, using tolerance for inaccuracies.
		*/
		template< typename T >
		bool RealEqual(T a, T b, T tolerance = std::numeric_limits<T>::epsilon())
		{
			if (fabs(b-a) <= tolerance)
				return true;
			else
				return false;
		}

		/** Generates a value based on the Gaussian (normal) distribution function
			 with the given offset and scale parameters.
		*/
		template < typename T >
		T GaussianDistribution(T x, T offset = T(0), T scale = T(1))
		{
			f32 nom = Exp(
				-Sqr(x - offset) / (2 * Sqr(scale)));
			f32 denom = scale * Sqrt(2 * PI);

			return nom / denom;

		}
		
		/** Clamp a value within an inclusive range. */
		template <typename T>
		T Clamp(T val, T minval, T maxval)
		{
			assert (minval < maxval && "Invalid clamp range");
			return std::max(std::min(val, maxval), minval);
		}

		template<typename T, typename F>
		T LinearInterpolate(const T &from, const T &to, F percent)
		{
			T diff=to-from;
			T result=from+(diff*percent);
			return result;
		}

		template<typename T, typename F>
		T HermiteInterpolate(const T& v3, const T& v2, const T& v1, const T& v0, F mu, F tension, F bias)
		{
			T m0,m1;
			F mu2,mu3;
			F a0,a1,a2,a3;

			mu2 = mu * mu;
			mu3 = mu2 * mu;
			m0  = (v1-v0)*(1+bias)*(1-tension)/2;
			m0 += (v2-v1)*(1-bias)*(1-tension)/2;
			m1  = (v2-v1)*(1+bias)*(1-tension)/2;
			m1 += (v3-v2)*(1-bias)*(1-tension)/2;
			a0 =  2*mu3 - 3*mu2 + 1;
			a1 =    mu3 - 2*mu2 + mu;
			a2 =    mu3 -   mu2;
			a3 = -2*mu3 + 3*mu2;

			return(a0*v1+a1*m0+a2*m1+a3*v2);
		}

		template<typename T, typename F>
		static T CubicInterpolate(const T& v0, const T& v1, const T& v2, const T& v3, F t)
		{
			return (v3 - v2 - v0 + v1) * t * t * t + (2.0f * v0 - 2.0f * v1 + v2 - v3) * t * t + (v2 - v0) * t + v1;
		}
	}

	// these functions must be defined down here, because they rely on the
	// angle unit conversion functions in class EMaths:

	inline f32 Radian::ValueDegrees() const
	{
		return Maths::RadiansToDegrees(mRad);
	}

	inline f32 Degree::ValueRadians() const
	{
		return Maths::DegreesToRadians(mDeg);
	}

	inline Radian operator * ( f32 a, const Radian& b )
	{
		return Radian ( a * b.ValueRadians() );
	}

	inline Radian operator / ( f32 a, const Radian& b )
	{
		return Radian ( a / b.ValueRadians() );
	}

	inline Degree operator * ( f32 a, const Degree& b )
	{
		return Degree ( a * b.ValueDegrees() );
	}

	inline Degree operator / ( f32 a, const Degree& b )
	{
		return Degree ( a / b.ValueDegrees() );
	}
	/** @} */
	/** @} */
}
#endif
