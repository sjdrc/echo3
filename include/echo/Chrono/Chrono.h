#ifndef _ECHO_CHRONO_H_
#define _ECHO_CHRONO_H_

#include <echo/Types.h>
#include <echo/cpp/chrono>
#include <boost/optional.hpp>

namespace Echo
{
	
	template< typename T >
	struct Duration : public T 
	{
		Duration() : T() {}
		
		template< typename X >
		Duration( const X& x ) : T( x ){}
	};

	typedef Duration< chrono::duration<double, nano> >  Nanoseconds;
	typedef Duration< chrono::duration<double, micro> >  Microseconds;
	typedef Duration< chrono::duration<double, milli> >  Milliseconds;
	typedef Duration< chrono::duration<double> > Seconds;
	typedef Duration< chrono::duration<double, ratio<60 > > > Minutes;
	typedef Duration< chrono::duration<double, ratio<3600 > > > Hours;

	typedef chrono::nanoseconds NanosecondsInt;
	typedef chrono::microseconds MicrosecondsInt;
	typedef chrono::milliseconds MillisecondsInt;
	typedef chrono::seconds SecondsInt;
	typedef chrono::minutes MinutesInt;
	typedef chrono::hours HoursInt;
	
	std::istream& operator>>(std::istream& i, Seconds& v);
	std::istream& operator>>(std::istream& i, Milliseconds& v);
	std::istream& operator>>(std::istream& i, Microseconds& v);
	std::istream& operator>>(std::istream& i, Nanoseconds& v);
	std::istream& operator>>(std::istream& i, Minutes& v);
	std::istream& operator>>(std::istream& i, Hours& v);
	std::ostream& operator<< (std::ostream& o, const Seconds& v);
	std::ostream& operator<< (std::ostream& o, const Milliseconds& v);
	std::ostream& operator<< (std::ostream& o, const Microseconds& v);
	std::ostream& operator<< (std::ostream& o, const Nanoseconds& v);
	std::ostream& operator<< (std::ostream& o, const Minutes& v);
	std::ostream& operator<< (std::ostream& o, const Hours& v);

namespace Chrono
{
	/**
	 * Get the current time since Linux Epoch in milliseconds.
	 */
	MillisecondsInt GetMillisecondsSinceEpoch();

	/**
	 * The IOS8601 format is:
	 *	YYYY-MM-DD-HH:MM:S.S
	 */
	std::string FormatIOS8601(MillisecondsInt millisecondsSinceEpoch, bool localTime = true, std::string dateSeparator="-", std::string dateTimeSeparator="T", std::string timeSeparator=":", std::string subSecondSeparator=".");
	std::string GetIOS8601(bool localTime = true);
	std::string GetIOS8601PathSafe(bool localTime = true);
}
}
#endif
