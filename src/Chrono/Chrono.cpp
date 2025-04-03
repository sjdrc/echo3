#include <echo/Chrono/Chrono.h>
#include <sstream>
#include <iomanip>
#include <echo/Util/StringUtils.h>

namespace Echo
{
	template< typename ChronoType >
	inline optional<ChronoType> StringToChrono(std::string s, const char* name)
	{
		using namespace Utils::String;
		double d;
		if(!Utils::String::VerifyConstructorAndExtractParameters(s,name) ||
			!ConvertAndAssign(s, d))
		{
			return none;
		}
		return ChronoType(d);
	}
	
	template< typename ChronoType >
	inline optional<ChronoType> AnyChronoStringToChrono(std::string s)
	{
		// I tried making this more elegant with boost fusion but it just got
		// convoluted and harder to read for no real benefit.
		// "Seconds" is first to default time to Seconds
		{		
			optional<Seconds> convertedValue = StringToChrono<Seconds>(s, "Seconds");
			if(convertedValue)
			{
				// This implicitly converts to the type we need
				return ChronoType(convertedValue.value());
			}
		}
		{
			optional<Milliseconds> convertedValue = StringToChrono<Milliseconds>(s, "Milliseconds");
			if(convertedValue)
			{
				// This implicitly converts to the type we need
				return ChronoType(convertedValue.value());
			}
		}
		{
			optional<Microseconds> convertedValue = StringToChrono<Microseconds>(s, "Microseconds");
			if(convertedValue)
			{
				// This implicitly converts to the type we need
				return ChronoType(convertedValue.value());
			}
		}
		{
			optional<Nanoseconds> convertedValue = StringToChrono<Nanoseconds>(s, "Nanoseconds");
			if(convertedValue)
			{
				// This implicitly converts to the type we need
				return ChronoType(convertedValue.value());
			}
		}
		{
			optional<Minutes> convertedValue = StringToChrono<Minutes>(s, "Minutes");
			if(convertedValue)
			{
				return ChronoType(convertedValue.value());
			}
		}
		{
			optional<Hours> convertedValue = StringToChrono<Hours>(s, "Hours");
			if(convertedValue)
			{
				return ChronoType(convertedValue.value());
			}
		}
		
		return none;
	}

	std::istream& operator>>(std::istream& i, Seconds& v)
	{
		std::string temp;
		i >> temp;
		optional<Seconds> converted=AnyChronoStringToChrono<Seconds>(temp);
		if(converted)
		{
			v = converted.value();
		}else
		{
			i.setstate(std::ios_base::failbit);
		}
		return i;
	}
	
	std::istream& operator>>(std::istream& i, Milliseconds& v)
	{
		std::string temp;
		i >> temp;
		optional<Milliseconds> converted=AnyChronoStringToChrono<Milliseconds>(temp);
		if(converted)
		{
			v = converted.value();
		}else
		{
			i.setstate(std::ios_base::failbit);
		}
		return i;
	}
	
	std::istream& operator>>(std::istream& i, Microseconds& v)
	{
		std::string temp;
		i >> temp;
		optional<Microseconds> converted=AnyChronoStringToChrono<Microseconds>(temp);
		if(converted)
		{
			v = converted.value();
		}else
		{
			i.setstate(std::ios_base::failbit);
		}
		return i;
	}
	
	std::istream& operator>>(std::istream& i, Nanoseconds& v)
	{
		std::string temp;
		i >> temp;
		optional<Nanoseconds> converted=AnyChronoStringToChrono<Nanoseconds>(temp);
		if(converted)
		{
			v = converted.value();
		}else
		{
			i.setstate(std::ios_base::failbit);
		}
		return i;
	}
	
	std::istream& operator>>(std::istream& i, Minutes& v)
	{
		std::string temp;
		i >> temp;
		optional<Minutes> converted=AnyChronoStringToChrono<Minutes>(temp);
		if(converted)
		{
			v = converted.value();
		}else
		{
			i.setstate(std::ios_base::failbit);
		}
		return i;
	}
	
	std::istream& operator>>(std::istream& i, Hours& v)
	{
		std::string temp;
		i >> temp;
		optional<Hours> converted=AnyChronoStringToChrono<Hours>(temp);
		if(converted)
		{
			v = converted.value();
		}else
		{
			i.setstate(std::ios_base::failbit);
		}
		return i;
	}

	
	std::ostream& operator<< (std::ostream& o, const Seconds& v)
	{
		o << "Seconds("  << v.count() << ")";

		return o;
	}

	std::ostream& operator<< (std::ostream& o, const Milliseconds& v)
	{
		o << "Milliseconds(" << v.count() << ")";
		return o;
	}

	std::ostream& operator<< (std::ostream& o, const Microseconds& v)
	{
		o << "Microseconds(" << v.count() << ")";
		return o;
	}

	std::ostream& operator<< (std::ostream& o, const Nanoseconds& v)
	{
		o << "Nanoseconds(" << v.count() << ")";
		return o;
	}

	std::ostream& operator<< (std::ostream& o, const Minutes& v)
	{
		o << "Minutes(" << v.count() << ")";
		return o;
	}

	std::ostream& operator<< (std::ostream& o, const Hours& v)
	{
		o << "Hours(" << v.count() << ")";
		return o;
	}
	
namespace Chrono
{

	MillisecondsInt GetMillisecondsSinceEpoch()
	{
		using namespace chrono;
		#if !defined(ECHO_PLATFORM_NINTENDO_GAMECUBE) && !defined(ECHO_PLATFORM_NINTENDO_WII)
			return duration_cast<MillisecondsInt>(system_clock::now().time_since_epoch());
		#else
			return MillisecondsInt(0);
		#endif
	}

	/**
	 * YYYY-MM-DD-HH:MM:S.S
	 */
	std::string FormatIOS8601(MillisecondsInt millisecondsSinceEpoch, bool localTime, std::string dateSeparator, std::string dateTimeSeparator, std::string timeSeparator, std::string subSecondSeparator)
	{
		#if !defined(ECHO_PLATFORM_NINTENDO_GAMECUBE) && !defined(ECHO_PLATFORM_NINTENDO_WII)
			using namespace chrono;
			time_t tt = system_clock::to_time_t(system_clock::time_point(millisecondsSinceEpoch));
			const int TM_YEAR_BASE = 1900;		// See http://www.cplusplus.com/reference/ctime/tm/
			tm t = localTime ? *localtime(&tt) : *gmtime(&tt);
			std::stringstream ss;
			ss << std::setfill('0') << std::setw(4) << (TM_YEAR_BASE+t.tm_year)
				<< dateSeparator
				<< std::setfill('0') << std::setw(2) << (t.tm_mon+1)
				<< dateSeparator
				<< std::setfill('0') << std::setw(2) << t.tm_mday
				<< dateTimeSeparator
				<< std::setfill('0') << std::setw(2) << t.tm_hour
				<< timeSeparator
				<< std::setfill('0') << std::setw(2) << t.tm_min
				<< timeSeparator
				<< std::setfill('0') << std::setw(2) << t.tm_sec
				<< subSecondSeparator
				<< std::setfill('0') << std::setw(3) << millisecondsSinceEpoch.count() % 1000;
			return ss.str();
		#else
			std::stringstream ss;
			ss << "1970" << dateSeparator << "01" << dateSeparator << "01" << dateTimeSeparator << "00" << timeSeparator << "00" << timeSeparator << "00" << subSecondSeparator << "000";
			return ss.str();
		#endif
	}

	std::string GetIOS8601(bool localTime)
	{
		return FormatIOS8601(GetMillisecondsSinceEpoch(),localTime);
	}

	std::string GetIOS8601PathSafe(bool localTime)
	{
		return FormatIOS8601(GetMillisecondsSinceEpoch(),localTime,"","T","","");
	}
}
}
