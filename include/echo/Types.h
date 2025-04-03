#ifndef _ECHO_TYPES_H_
#define _ECHO_TYPES_H_

#ifndef BOOST_VERSION
#include <boost/version.hpp>	// If this include path is not found, install Boost using the instructions below.
#endif
#ifndef BOOST_VERSION
#error Please install the Boost C++ libraries. Refer to instructions here: \
developer.emblem.net.au/w/technical_documents/installing_or_upgrading_boost/
#else
#define ECHO_MINIMUM_SUPPORTED_BOOST_VERSION 106900
#if BOOST_VERSION < ECHO_MINIMUM_SUPPORTED_BOOST_VERSION
#error Please update the Boost C++ libraries to a newer version. Refer to instructions here: \
developer.emblem.net.au/w/technical_documents/installing_or_upgrading_boost/
#endif
#endif

#include <boost/predef/other/endian.h>

#ifdef BOOST_ENDIAN_BIG_BYTE_AVAILABLE
#define ECHO_BIG_ENDIAN
#define ECHO_ENDIAN_DETECTED
#endif
#ifdef BOOST_ENDIAN_LITTLE_BYTE_AVAILABLE
#define ECHO_LITTLE_ENDIAN
#define ECHO_ENDIAN_DETECTED
#endif

#define ECHO_BYTE_ORDER BOOST_BYTE_ORDER

#if defined(ECHO_LITTLE_ENDIAN) && defined(ECHO_BIG_ENDIAN)
#error "ECHO_LITTLE_ENDIAN and ECHO_BIG_ENDIAN have been defined. Only one can be set"
#endif
#if defined(ECHO_PLATFORM_NINTENDO_GAMECUBE) || defined(ECHO_PLATFORM_NINTENDO_WII)
#include <echo/Platforms/GCN/Types.h>
#endif

#include <boost/cstdint.hpp>
namespace Echo
{
	//--------------------------------------------
	typedef boost::uint8_t u8;
	typedef boost::uint16_t u16;
	typedef boost::uint32_t u32;
	typedef boost::uint64_t u64;

	typedef boost::int8_t s8;
	typedef boost::int16_t s16;
	typedef boost::int32_t s32;
	typedef boost::int64_t s64;

	typedef float f32;
	typedef double f64;
}

#ifndef ECHO_ENDIAN_DETECTED
#error Could not determine platform endian format. Please define ECHO_BIG_ENDIAN or ECHO_LITTLE_ENDIAN on the commandline.
#endif

namespace Echo
{
	typedef size_t Size;
	typedef f32 Real;
	typedef f32 Scalar;
	typedef u32 UTF32Code;
	typedef u16 UTF16Code;
}

//This file should be included for everything so it is an ideal place to add some simple C++11 support
// of classes, keywords and identifiers for use in ancient C++.

// Microsoft C++ compiler versions which support C++11 don't have the right value for __cplusplus
#if (defined(_MSC_VER) && _MSC_VER >= 1700)
#ifdef __cplusplus
#undef __cplusplus
#define __cplusplus 201103L
#endif
#endif


#if (__cplusplus > 199711L)
// C++ ISO/IEC 14882:2011 supported by the compiler ("C++11")

#define if_cplusplus11(x) x

#include <memory>
namespace Echo
{
	using std::unique_ptr;
	using std::shared_ptr;
	using std::weak_ptr;
	using std::make_shared;
	using std::dynamic_pointer_cast;
	using std::static_pointer_cast;
	using std::enable_shared_from_this;
}
	#if (__cplusplus >= 201703L)
		#include <optional>
		namespace Echo
		{
			template< typename T >
			using optional = std::optional<T>;
			static auto none = std::nullopt;
		}
	#else
		#include <boost/optional.hpp>
		namespace Echo
		{
			using boost::optional;
			using boost::none;
		}
	#endif
#else

// C++ ISO/IEC 14882:1998 supported by the compiler (hopefully)
#define ECHO_USE_BOOST_FOR_STANDARDS

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/pointer_cast.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/optional.hpp>
namespace Echo
{
	using boost::shared_ptr;
	using boost::weak_ptr;
	using boost::make_shared;
	using boost::dynamic_pointer_cast;
	using boost::static_pointer_cast;
	using boost::enable_shared_from_this;
	using boost::optional;
	using boost::none;
}
#define if_cplusplus11(x)

// Placeholder for http://en.cppreference.com/w/cpp/language/final
#ifndef final
//#define final	// breaks boost
#endif

// Placeholder for http://en.cppreference.com/w/cpp/language/override
#ifndef override
#define override
#endif

namespace Echo
{
	// Placeholder for http://en.cppreference.com/w/cpp/language/nullptr
	// from http://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/nullptr
	const class nullptr_t
	{
	public:
		template<class T>
		inline operator T*(void) const
		{
			return 0;
		}
		template<class C, class T>
		inline operator T C::*(void) const
		{
			return 0;
		}
		// nullptr_t support for boost::shared_ptr<T> in lieu of std::shared_ptr<T>
		template<class T>
		inline operator shared_ptr<T>(void) const
		{
			return shared_ptr<T>();
		}
	private:
		void operator &(void) const;
	} nullptr = {};
}
#endif

#if defined(ECHO_PLATFORM_WINDOWS)
#include <echo/Platforms/Windows/Types.h>
#endif

#include <echo/Logging/Logging.h>
#include <echo/Assert.h>
#endif
