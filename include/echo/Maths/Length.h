#ifndef _ECHO_LENGTH_H_
#define _ECHO_LENGTH_H_

#include <echo/Types.h>
#include <echo/Maths/EchoMaths.h>
#include <echo/Util/StringUtils.h>
#include <boost/ratio.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

namespace Echo
{
	/**
	 * Length provides facilities to convert between different length units automatically.
	 * For example, you can choose to work in one set of units but still assign and manipulate
	 * using others without worrying about converting to the same unit.
	 * Adapted from http://www.boost.org/doc/libs/1_56_0/libs/ratio/example/si_physics.cpp
	 */
	template <typename Representation, class Ratio>
	class Length
	{
	public:
		typedef Ratio ratio;
		typedef Representation ValueType;
	private:
		Representation mLength;
	public:

		Length() : mLength(0) {}
		Length(const Representation& len) : mLength(len) {}

		// conversions
		template <class R>
		Length(const Length<Representation,R>& d) :
			mLength(d.Count() * boost::ratio_divide<Ratio, R>::type::den /
								boost::ratio_divide<Ratio, R>::type::num)
		{}

		// observer
		Representation Count() const {return mLength;}

		// arithmetic
		Length& operator+=(const Length& d) {mLength += d.Count(); return *this;}
		Length& operator-=(const Length& d) {mLength -= d.Count(); return *this;}

		Length operator+() const {return *this;}
		Length operator-() const {return Length(-mLength);}

		Length& operator*=(Representation rhs) {mLength *= rhs; return *this;}
		Length& operator/=(Representation rhs) {mLength /= rhs; return *this;}

		Length operator*(const Length& rhs) const {return Length(mLength * rhs.mLength);}
		Length operator/(const Length& rhs) const {return Length(mLength / rhs.mLength);}
		Length operator+(const Length& rhs) const {return Length(mLength + rhs.mLength);}
		Length operator-(const Length& rhs) const {return Length(mLength - rhs.mLength);}
		Length operator*(const Representation& rhs) const {return Length(mLength * rhs);}
		Length operator/(const Representation& rhs) const {return Length(mLength / rhs);}
		Length operator+(const Representation& rhs) const {return Length(mLength + rhs);}
		Length operator-(const Representation& rhs) const {return Length(mLength - rhs);}

		bool operator==(const Length& rhs) const {return mLength==rhs.Count();}
		bool operator!=(const Length& rhs) const {return mLength!=rhs.Count();}

		bool operator>(const Length& rhs) const {return mLength>rhs.Count();}
		bool operator<(const Length& rhs) const {return mLength<rhs.Count();}
		bool operator>=(const Length& rhs) const {return mLength>=rhs.Count();}
		bool operator<=(const Length& rhs) const {return mLength<=rhs.Count();}
	};
	
	typedef f64 LengthStorage;
	typedef Length<LengthStorage, boost::kilo > Kilometre;			// 1000 metres
	typedef Length<LengthStorage, boost::ratio<1> > Metre;			// Set metre as "unity"
	typedef Length<LengthStorage, boost::centi> Centimetre;			// 1/100 metre
	typedef Length<LengthStorage, boost::milli> Millimetre;			// 1/1000 metre
	typedef Length<LengthStorage, boost::micro> Micrometre;			// 1/1000000 metre
	typedef Length<LengthStorage, boost::nano> Nanometre;			// 1/1000000000 metre
	
	typedef Length<LengthStorage, boost::ratio<254, 10000> > Inch;	// 254/10000 metres
	
	// Length takes a ratio instead of two integral types so that definitions can be made like so:
	typedef Length<LengthStorage, boost::ratio_multiply<boost::ratio<12>, Inch::ratio>::type>   Foot;  // 12 inches
	typedef Length<LengthStorage, boost::ratio_multiply<boost::ratio<5280>, Foot::ratio>::type> Mile;  // 5280 feet

	inline bool GetPixelMeasurment(std::string s, LengthStorage& outPixels)
	{
		if(s.length()>=3)
		{
			try
			{
				std::string ext=s.substr(s.length()-2);
				std::string valuePart=s.substr(0,s.length()-2);
				f64 value = boost::lexical_cast<f64>(valuePart);
				if(ext=="px")
				{
					outPixels=value;
					return true;
				}
			}
			catch(...){}
		}
		return false;
	}

	template< typename LengthType >
	inline void StringToLength(std::istream& i, LengthType& v)
	{
		std::string s;
		i >> s;

		if(s.length()>=3)
		{
			try
			{
				std::string ext=s.substr(s.length()-2);
				std::string valuePart=s.substr(0,s.length()-2);
				
				f64 value = boost::lexical_cast<f64>(valuePart);
				if(ext=="mm")
				{
					v = Millimetre(value);
					return;
				}else
				if(ext=="cm")
				{
					v = Centimetre(value);
					return;
				}else
				if(ext=="km")
				{
					v = Kilometre(value);
					return;
				}else
				if(ext=="um")
				{
					v = Micrometre(value);
					return;
				}else
				if(ext=="nm")
				{
					v = Nanometre(value);
					return;
				}else
				if(ext=="in")
				{
					v = Inch(value);
					return;
				}else
				if(ext=="ft")
				{
					v = Foot(value);
					return;
				}else
				if(ext=="mi")
				{
					v = Mile(value);
					return;
				}
			}catch(...)
			{
				v = Metre(0);
				i.setstate(std::ios_base::failbit);
			}
		}
		//If it didn't convert via the two character postfix then we'll try single characters.
		if(s.length()>=2)
		{
			std::string ext=s.substr(s.length()-1);
			if(ext=="m")
			{
				s=s.substr(0,s.length()-1);
			}
		}
		
		//Just metres.
		try
		{
			v = Metre(boost::lexical_cast<f64>(s));
		}catch(...)
		{
			v = Metre(0);
			i.setstate(std::ios_base::failbit);
		}
	}
	
	inline std::istream& operator>>(std::istream& i, Metre& v)
	{
		StringToLength<Metre>(i, v);
		return i;
	}
	
	inline std::istream& operator>>(std::istream& i, Kilometre& v)
	{
		StringToLength<Kilometre>(i, v);
		return i;
	}
	
	inline std::istream& operator>>(std::istream& i, Centimetre& v)
	{
		StringToLength<Centimetre>(i, v);
		return i;
	}

	inline std::istream& operator>>(std::istream& i, Millimetre& v)
	{
		StringToLength<Millimetre>(i, v);
		return i;
	}

	inline std::istream& operator>>(std::istream& i, Micrometre& v)
	{
		StringToLength<Micrometre>(i, v);
		return i;
	}

	inline std::istream& operator>>(std::istream& i, Nanometre& v)
	{
		StringToLength<Nanometre>(i, v);
		return i;
	}

	inline std::istream& operator>>(std::istream& i, Inch& v)
	{
		StringToLength<Inch>(i, v);
		return i;
	}

	inline std::istream& operator>>(std::istream& i, Foot& v)
	{
		StringToLength<Foot>(i, v);
		return i;
	}

	inline std::istream& operator>>(std::istream& i, Mile& v)
	{
		StringToLength<Mile>(i, v);
		return i;
	}
	
	inline std::ostream& operator<< (std::ostream& o, const Metre& v)
	{
		o << v.Count() << "m";
		return o;
	}

	inline std::ostream& operator<< (std::ostream& o, const Kilometre& v)
	{
		o << v.Count() << "km";
		return o;
	}

	inline std::ostream& operator<< (std::ostream& o, const Centimetre& v)
	{
		o << v.Count() << "cm";
		return o;
	}

	inline std::ostream& operator<< (std::ostream& o, const Millimetre& v)
	{
		o << v.Count() << "mm";
		return o;
	}

	inline std::ostream& operator<< (std::ostream& o, const Micrometre& v)
	{
		o << v.Count() << "um";
		return o;
	}

	inline std::ostream& operator<< (std::ostream& o, const Nanometre& v)
	{
		o << v.Count() << "nm";
		return o;
	}

	inline std::ostream& operator<< (std::ostream& o, const Inch& v)
	{
		o << v.Count() << "in";
		return o;
	}

	inline std::ostream& operator<< (std::ostream& o, const Foot& v)
	{
		o << v.Count() << "ft";
		return o;
	}

	inline std::ostream& operator<< (std::ostream& o, const Mile& v)
	{
		o << v.Count() << "mi";
		return o;
	}

	
	template< typename LengthType >
	inline LengthType StringToLength(const std::string& s)
	{
		optional<LengthType> v = Utils::String::FromString<LengthType>(s);
		if(v)
		{
			return v.value();
		}
		return LengthType(0);
	}
	
	namespace Maths
	{
		inline Radian ACos(Metre value){return ACos<Metre::ValueType>(value.Count());}
		inline Radian ASin(Metre value){return ASin<Metre::ValueType>(value.Count());}
		inline Radian ATan(Metre value){return ATan<Metre::ValueType>(value.Count());}
		inline Radian ATan2(Metre x, Metre y){return ATan2<Metre::ValueType>(x.Count(),y.Count());}
		inline Metre Ceil(Metre value){return Metre(Ceil<Metre::ValueType>(value.Count()));}
		inline bool IsNaN(Metre f){return IsNaN<Metre::ValueType>(f.Count());}
		inline bool IsFinite(Metre f){return IsFinite<Metre::ValueType>(f.Count());}
		inline Metre Exp(Metre value){return Metre(Exp<Metre::ValueType>(value.Count()));}
		inline Metre Floor(Metre value){return Metre(Floor<Metre::ValueType>(value.Count()));}
		inline Metre Log (Metre value){return Metre(Log<Metre::ValueType>(value.Count()));}
		inline Metre Log2(Metre value){return Metre(Log2<Metre::ValueType>(value.Count()));}
		inline Metre LogN(Metre base, Metre value){return Metre(LogN<Metre::ValueType>(value.Count(),base.Count()));}
		inline Metre Pow(Metre base, Metre exponent){return Metre(Pow<Metre::ValueType>(base.Count(),exponent.Count()));}
		inline Metre Sign(Metre value){return Metre(Sign<Metre::ValueType>(value.Count()));}
		inline Metre Sqr(Metre value){return Metre(value.Count()*value.Count());}
		inline Metre Sqrt(Metre value){return Metre(Sqrt<Metre::ValueType>(value.Count()));}
	}
}
#endif
