#ifndef _ECHOUTILS_H_
#define _ECHOUTILS_H_

#include <echo/Types.h>
#include <boost/lexical_cast.hpp>

#ifndef ECHO_NO_NAME_DEMANGLE
#include <cxxabi.h>
#endif

namespace Echo
{
	template<typename T, size_t size>
	size_t SizeOfArray(T(&)[size])
	{
		return size;
	}
	
	template <typename T>
	struct ArrayDeleter
	{
		void operator()(T const* p)
		{
			delete [] p;
		}
	};

	template< typename T>
	T NextPow2(T x)
	{
		--x;
		x |= x >> 1;
		x |= x >> 2;
		x |= x >> 4;
		x |= x >> 8;
		x |= x >> 16;
		return ++x;
	}
	
	/**
	 * Get the highest bit using a base 0 index.
	 * Implementation taken from https://github.com/nothings/stb
	 * Example:
	 *		The value 496 in binary is 00000000 00000000 00000001 11110000
	 *		This function will return 8 since the 9th bit is the highest bit.
	 * @param v The value to evaluate.
	 * @return 0 - 31 for the highest set bit, this method also returns 0 if the value is 0.
	 */
	inline u32 HighestBit(u32 v)
	{
		int n=0;
		if (v == 0) return 0;
		if (v >= 0x10000) n += 16, v >>= 16;
		if (v >= 0x00100) n +=  8, v >>=  8;
		if (v >= 0x00010) n +=  4, v >>=  4;
		if (v >= 0x00004) n +=  2, v >>=  2;
		if (v >= 0x00002) n +=  1, v >>=  1;
	   return n;
	}

	/**
	 * Get the number of bits in use.
	 * Implementation taken from https://github.com/nothings/stb
	 * Example:
	 *		The value 144 in binary is 00000000 00000000 00000000 10010000
	 *		This function will return 2 since 2 bits are on.
	 * @param a The value to evaluate.
	 * @return 
	 */
	inline u32 BitCount(u32 a)
	{
	   a = (a & 0x55555555) + ((a >>  1) & 0x55555555); // max 2
	   a = (a & 0x33333333) + ((a >>  2) & 0x33333333); // max 4
	   a = (a + (a >> 4)) & 0x0f0f0f0f; // max 8 per 4, now 8 bits
	   a = (a + (a >> 8)); // max 16 per 8 bits
	   a = (a + (a >> 16)); // max 32 per 8 bits
	   return a & 0xff;
	}
	
	/**
	 * Generate a unique name for the given container.
	 * This method uses T::find and assumes the search will use a string, so the container needs to support this.
	 * The output will be baseName if the name is not found, otherwise it will be baseName-N where N is the number
	 * of additional entries that were found with the previous names. For example:
	 *		myMap["cat"]=something;
	 *		std::string newName = GenerateUniqueName("cat",myMap);
	 * newName will be "cat-1".
	 * Called again then the value returned will be "cat-2".
	 * @param baseName the base name of the identifier that you wish to use.
	 * @param target Container to perform the name searches on.
	 * @return a new name that will be unique for the container.
	 */
	template< typename T >
	std::string GenerateUniqueName(const std::string& baseName, const T& target, std::string separator="-")
	{
		size_t attempt=0;
		std::string testName=baseName;
		while(target.find(testName)!=target.end())
		{
			attempt++;
			testName = baseName + separator + boost::lexical_cast<std::string>(attempt);
		}
		return testName;
	}
	
	template<typename T>
	inline T Limit(T v, T min, T max)
	{
		if(v<min)
		{
			return min;
		}else
		if(v>max)
		{
			return max;
		}
		return v;
	}

	inline f32 LimitOne(f32 v)
	{
		return Limit(v,0.f,1.f);
	}

	inline f64 LimitOne(f64 v)
	{
		return Limit(v,0.,1.);
	}

	/**
	 * Returns the ratio of two numbers
	 * @param x The first value.
	 * @param y the second value.
	 * @return The ratio of x to y.
	 */
	inline f32 Ratio(f32 x, f32 y)
	{
		return x / y;
	}
	
	/**
	 * Simple class to hold and lookup a value via a shared pointer.
	 */
	template< typename T >
	struct SharedPtrValueDelegate
	{
		SharedPtrValueDelegate(shared_ptr<T> variable) : mVariable(variable){}
		shared_ptr<T> mVariable;
		double GetValue()
		{
			return *mVariable;
		}
	};

	/**
	 * Simple class to hold and lookup a value by reference.
	 */
	template< typename T >
	struct ReferenceValueDelegate
	{
		ReferenceValueDelegate(const T& variable) : mVariable(variable){}
		const T& mVariable;
		double GetValue()
		{
			return mVariable;
		}
	};	
	
	// Because this may not be compatible with all compilers I've added a simple macro that can be defined to prevent compile errors
	#ifndef ECHO_NO_NAME_DEMANGLE

	/**
	 * Demangle a C++ ABI name.
	 * @see https://gcc.gnu.org/onlinedocs/libstdc++/manual/ext_demangling.html
	 * @param mangledName Pass a mangled name as a string, for example DemangleName(typeid(yourObjectOrType).name())
	 * @return demangled name
	 */
	inline std::string DemangleName(const std::string& mangledName)
	{
		std::string realname;
		int status;
		char* realnameTemp = abi::__cxa_demangle(mangledName.c_str(), 0, 0, &status);
		realname = realnameTemp;
		free(realnameTemp);
		return realname;
	}
	#else

	/**
	 * Null Demangler implementation.
	 * @see https://gcc.gnu.org/onlinedocs/libstdc++/manual/ext_demangling.html
	 * @param mangledName Pass a mangled name as a string, for example DemangleName(typeid(yourObjectOrType).name())
	 * @return Non-demangled name, this is a place holder for compilers that don't support cxxabi.h
	 */
	inline std::string DemangleName(const std::string& mangledName)
	{
		return mangledName;
	}

	#endif

	/**
	 * Demangle a C++ ABI name.
	 * @see https://gcc.gnu.org/onlinedocs/libstdc++/manual/ext_demangling.html
	 * @param typeInfo Pass a type_info object, for example DemangleName(typeid(yourObjectOrType))
	 * @return demangled name
	 */
	inline std::string DemangleName(const std::type_info& typeInfo)
	{
		return DemangleName(typeInfo.name());
	}
}

#endif
