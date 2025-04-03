#ifndef _ECHOSTRINGUTIL_H_
#define _ECHOSTRINGUTIL_H_
#include <vector>
#include <string>
#include <sstream>
#include <utility>
#include <echo/Util/Utils.h>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <iomanip>
#include <boost/type_traits/has_right_shift.hpp>

namespace Echo
{
	class UTF8String;
	namespace Utils
	{
		namespace String
		{
			/**
			 * Split a string in two.
			 * The following use cases are possible:
			 *	stringToSplit		separators		outLeft			outRight		return
			 *	"Hello=There"		"="				"Hello"			"There"			true
			 *	"Hello there="		"="				"Hello there"	(empty)			true
			 *	"=Hello there"		"="				(empty)			"Hello there"	true
			 *	"Hello there"		"="				(unmodified)	(unmodified)	false
			 *  "Hello=Thy,Person"	"=,"			"Hello"			"Thy,Person"	true
			 * 
			 * As you can see in the last example, if separators contains more than one character then
			 * the separator becomes the first occurrence of any of the characters in stringToSplit.
			 * 
			 * @param stringToSplit The string that will be divided by the separator
			 * @param separators The first of any of the characters in this string will be the separator.
			 * @param left The left side of the separator in the string to split.
			 * @param right The right side of the separator in the string to split.
			 * @return true if the string was split, otherwise false if no separator was found.
			 */
			bool SplitInTwo(const std::string& stringToSplit, std::string separators, std::string& outLeft, std::string& outRight);
			
			/**
			 * Convenience UTF8 right version of SplitInTwo().
			 * The purpose of this function is to take a UTF8String as the right parameter to save you creating a temporary string.
			 * @see SplitInTwo().
			 */
			bool SplitInTwo(const std::string& stringToSplit, std::string separators, std::string& outLeft, UTF8String& outRight);

			//!\brief Split a string by a list of separators.
			//!\param stringToSplit The string to split.
			//!\param separators A list of characters that can be separators. This is not used for a string match, it matches any single character.
			//!\param outStrings The output is split and added to this vector.
			void Split(const std::string& stringToSplit, std::string separators, std::vector<std::string>& outStrings);
			
			/**
			 * Overload of Split that supports escaping separators. 
			 * @param stringToSplit The string to split
			 * @param separator The character to use as a separators.
			 * @param escapeCharacter The character to use as an escape character.
			 * @param quoteCharacter The character to use as a quote character, wrapping separators in quotes makes means
			 * they won't be used as a separator.
			 * @param outStrings The output is split and added to this vector.
			 */
			void Split(const std::string& stringToSplit, std::vector<std::string>& outStrings, char separator = ',', char escape = '\\', char quote = '\"');

			/**
			 * Escape characters in a string.
			 * @param stringToEscape The string containing characters to escape.
			 * @param charactersToEscape The characters that should be escaped.
			 * @param escape The escape character
			 * @return A copy of the original string with the escape character inserted before each 
			 */
			std::string EscapeCharacters(const std::string& stringToEscape, const std::string& charactersToEscape, char escape = '\\');

			//!\brief Split a line into two parts and return the right hand side as split output.
			//!\details Example:
			//!				left = SplitLine("abc=123,456","=",",",output);
			//!				left = "abc"
			//!				output = {"123","456"}
			//!\param line The line, or string, to process.
			//!\param lineSeparators A list of characters that can be separators. This is not used for a string match, it matches
			//!						 any single character. The first character found in the line is used as the split position.
			//!\param rightPartSeparators Similar to the lineSeparators parameter but used on the right side as the split character for the vector.
			//!\param splitOut The vector with the right hand side of the inline split by rightPartSeparators.
			//!\return the left part of the line.
			std::string SplitLine(const std::string& line, std::string lineSeparators, std::string rightPartSeparators, std::vector<std::string>& splitOut);

			/**
			 * Get the file extension name from a string.
			 * This method will look for the last '.' and return the string that follows.
			 * If the file name contains a path as determined by the '/' character then
			 * the function will split the string and use the string after the last '/'
			 * to avoid returning part of path names if a directory contains a '.'. For
			 * example: myfolder/some.path/noExtensionFile
             * @param fileName The name of the file, this can include a path.
             * @return The extension after the last '.' or and empty string if there is
			 * no '.'.
             */
			std::string GetFileExtension(const std::string& fileName);
			
			/**
			 * Takes a file name and returns the path component.
			 * @param fileName The name of the file, may or may not include the path.
			 * @note Only supports forward slashes.
			 * @return An empty string if there isn't a path otherwise the path with trailing slash.
			 */
			std::string GetPathFromFilename(const std::string& fileName);
			
			/**
			 * Reduce a path to a more minimal form.
			 * This function will take a path string and remove duplicate slashes, remove ".." by
			 * "navigating", and will also remove leading . characters. For example:
			 *	./a/b/c//../d reduces to a/b/c
			 * The path can include a filename.
			 * @note This function does not check if any files or folders exist.
			 * @note This function does not follow symbolic links.
			 * @param pathString The string containing the path to reduce.
			 * @return The reduced path.
			 */
			std::string ReducePath(const std::string& pathString);
			
			/**
			 * Check whether the passed string takes the form of a constructor and extract the parameters.
			 * The function checks that the passed string takes the form Type(parameters) Where the type
			 * is specified by the second parameters. If the type matches then the input string will be
			 * assigned to the 'parameters'.
			 * 
			 * For example:
			 * 
			 *	std::string myString = "Vector2(10,20)";
			 *  bool isVector2 = VerifyConstructorAndExtractParameters(myString,"Vector2");
			 * 
			 * Will result in:
			 *	isVector2 = true;
			 *	myString = "10,20";
			 * 
			 * This function is used for preparing a string for type conversions and used in a lot of >>
			 * operators.
			 * 
			 * @param constructorInOut The string to check and to contain the parameters section if the
			 * function returns true
			 * @param type The type as a string, for example "Vector2".
			 * @param assumeOnlyParametersIfNoType If type is not found at the start of the string and this is true
			 * the function will be considered a success. This allows for the assumption that the input
			 * contains only parameters. This is useful for delegating a full failure to ConvertAndAssign()
			 * and using this function to trim the type and brackets out.
			 * @return true if the type matched and the parameters fall within the first open and close
			 * brackets, otherwise false. Upon returning false, constructorInOut will be unmodified.
			 */
			inline bool VerifyConstructorAndExtractParameters(std::string& constructorInOut, const std::string& type, bool assumeOnlyParametersIfNoType = true)
			{
				size_t openBracket = constructorInOut.find_first_of('(');
				
				//Found an open bracket. Lets see what the string is before the open bracket. It should be "type".
				if(openBracket!=std::string::npos)
				{
					//Cancel if the type isn't "type".
					if(constructorInOut.substr(0,openBracket)==type)
					{
						//We're on track. Now find a close bracket
						size_t closeBracket = constructorInOut.find_first_of(')');
						if(closeBracket!=std::string::npos && closeBracket > openBracket)
						{
							//Now it should be x,y,z option 2, so we'll pass the input to the next processing block.
							constructorInOut = constructorInOut.substr(openBracket+1, closeBracket - openBracket - 1);
							return true;
						}
					}
					return false;
				}
				// Return assumeOnlyParametersIfNoType for cases where an open bracket isn't found.
				// Only parameters means x,y,z,... etc, assumeOnlyParametersIfNoType determines if this
				// is successful or not.
				return assumeOnlyParametersIfNoType;
			}
			
			/**
			 * Take a string and convert it to a value.
			 * @param parameters The string to convert to the specified type, can include multiple values separated
			 * by commas and only the first one is used.
			 * @param v the variable to assign.
			 * @return true if conversion was successful, the value will have been assigned, otherwise false.
			 */
			template<typename T>
			inline bool ConvertAndAssign(const std::string& parameters, T& v)
			{
				std::stringstream ss(parameters);
 				std::string t;
				if(!std::getline(ss, t, ','))
				{
					return false;
				}
				try
				{
					v = boost::lexical_cast<T>(t);
					return true;
				}catch(...){} //Catch the convert exception.
				return false;
			}

			/**
			 * Take a string of parameters, separated by comas, convert and assign each variables from pointers.
			 * @param parameters The string to convert to the specified type, can include multiple values separated
			 * by commas. If more parameters are provided than necessary the extra parameters are ignored.
			 * @param v the pointers to variables to assign.
			 * @return true if conversion was successful, the value will have been assigned, otherwise false. When
			 * this function fails it may be that some of the values were successfully converted, due to this, you
			 * should check the return result and upon failure re-assign your values to a fallback values before you
			 * use them.
			 */
			template<typename T, size_t size>
			inline bool ConvertAndAssign(const std::string& parameters, T* (&vs)[size])
			{
				std::stringstream ss(parameters);
				for(size_t p=0; p < size; ++p) 
				{
	 				std::string t;
					if(!std::getline(ss, t, ','))
					{
						return false;
					}
					try
					{
						*(vs[p]) = boost::lexical_cast<T>(t);
					}catch(...)
					{
						return false;
					}
				}
				return true;
			}

			/**
			 * Much the same as ConvertAndAssign(const std::string&, T* (&vs)[]) but uses a pointer and
			 * number of parameters.
			 * This version is more suited to longer consecutive elements.
			 * @see ConvertAndAssign(const std::string& parameters, T* (&vs)[size])
			 * @param parameters The string to convert to the specified type, can include multiple values separated
			 * by commas. If more parameters are provided than necessary the extra parameters are ignored.
			 * @param v the pointers to the first of the elements to assign.
			 * @param numParams The number of elements to convert. Must not exceed the number of elements.
			 * @return true if conversion was successful, the value will have been assigned, otherwise false. When
			 * this function fails it may be that some of the values were successfully converted, due to this, you
			 * should check the return result and upon failure re-assign your values to a fallback values before you
			 * use them.
			 */
			template<typename T>
			inline bool ConvertAndAssign(const std::string& parameters, T* vs, size_t numParams)
			{
				std::stringstream ss(parameters);
				for(size_t p=0; p < numParams; ++p) 
				{
	 				std::string t;
					if(!std::getline(ss, t, ','))
					{
						return false;
					}
					try
					{
						vs[p] = boost::lexical_cast<T>(t);
					}catch(...)
					{
						return false;
					}
				}
				return true;
			}

			/**
			 * Two parameter specialisation of ConvertAndAssign for convenience.
			 */
			template<typename T>
			inline bool ConvertAndAssign(const std::string& parameters, T& v1, T& v2)
			{
				T* vs[2] = {&v1, &v2};
				return ConvertAndAssign(parameters,vs);
			}

			/**
			 * Three parameter specialisation of ConvertAndAssign for convenience.
			 */
			template<typename T>
			inline bool ConvertAndAssign(const std::string& parameters, T& v1, T& v2, T& v3)
			{
				T* vs[3] = {&v1, &v2, &v3};
				return ConvertAndAssign(parameters,vs);
			}			

			/**
			 * Four parameter specialisation of ConvertAndAssign for convenience.
			 */
			template<typename T>
			inline bool ConvertAndAssign(const std::string& parameters, T& v1, T& v2, T& v3, T& v4)
			{
				T* vs[4] = {&v1, &v2, &v3, &v4};
				return ConvertAndAssign(parameters,vs);
			}
			
			namespace Detail
			{
				template < typename T, bool hasRightShift >
				struct StreamAssignOrFail;

				template < typename T >
				struct StreamAssignOrFail< T, true >
				{
					static void AssignOrFail(std::istream& s, T& outValue)
					{
						s >> outValue;
					}
				};

				template < typename T >
				struct StreamAssignOrFail< T, false >
				{
					static void AssignOrFail(std::istream& s, T&)
					{
						s.setstate(std::ios::failbit);
					}
				};
				
				template < typename T, bool hasLeftShift >
				struct StreamOutOrFail;

				template < typename T >
				struct StreamOutOrFail< T, true >
				{
					static void OutOrFail(std::ostream& s, const T& value)
					{
						s << value;
					}
				};

				template < typename T >
				struct StreamOutOrFail< T, false >
				{
					static void OutOrFail(std::ostream& s, const T&)
					{
						s.setstate(std::ios::failbit);
					}
				};
			}

			/**
			 * Attempt to assign a value using operator>> and set the fail bit on the stream if the operator does not exist.
			 * This is to allow code to compile for runtime conversion failures.
			 */
			template < typename T >
			struct StreamAssignOrFail
			{
				static void AssignOrFail(std::istream& s, T& outValue)
				{
					Detail::StreamAssignOrFail<T, boost::has_right_shift<std::istream, T>::value>::AssignOrFail(s,outValue);
				}
			};
			
			/**
			 * Attempt to output a value using operator<< and set the fail bit on the stream if the operator does not exist.
			 * This is to allow code to compile for runtime conversion failures.
			 */
			template < typename T >
			struct StreamOutOrFail
			{
				static void OutOrFail(std::ostream& s, const T& outValue)
				{
					Detail::StreamOutOrFail<T, boost::has_left_shift<std::ostream, T>::value>::OutOrFail(s,outValue);
				}
			};

			/**
			 * Uses stream operator >> to try and convert from string
			 * @param v the input string
			 * @return optional converted value.
			 */
			template< typename T>
			optional<T> FromString(const std::string& v, Size streamPrecision = 16)
			{
				std::stringstream ss(v);
				T tempValue;
				ss >> std::setprecision(streamPrecision);
				Utils::String::StreamAssignOrFail<T>::AssignOrFail(ss,tempValue);
				if(ss.fail())
				{
					ECHO_LOG_ERROR("Failed to convert: \"" << v << "\" to \"" << DemangleName(typeid(T)) << "\" with operator>>.");
					return none;
				}
				return tempValue;
			}
			
			template<>
			optional<bool> FromString<bool>(const std::string& v, Size streamPrecision);

			
			template<>
			optional<std::string> FromString<std::string>(const std::string& v, Size);
			
			template< typename T>
			T FromStringOrDefault(const std::string& v, T defaultValue, Size streamPrecision = 16)
			{
				auto optionalValue = FromString<T>(v,streamPrecision);
				if(optionalValue)
				{
					return optionalValue.value();
				}
				return defaultValue;
			}

			/**
			 * Attempt to convert the value to a string.
			 * @param v The value you to attempt to convert to a string.
			 * @return optional string.
			 */
			template< typename T>
			optional<std::string> ToString(const T& v, Size streamPrecision = 16)
			{
				std::stringstream ss;
				ss << std::setprecision(streamPrecision);
				StreamOutOrFail<T>::OutOrFail(ss, v);
				if(ss.fail())
				{
					ECHO_LOG_ERROR("Failed to input value: \"" << DemangleName(typeid(T)) << "\" to string with operator<<.");
					return none;
				}
				return ss.str();
			}

			template<>
			optional<std::string> ToString<std::string>(const std::string& v, Size streamPrecision);

			/**
			 * Remove http:// or https:// from start of string as well as any credentials leading up to @ from
			 * a string.
			 */
			std::string RemoveURLSchemeAndCreds(std::string url);

			/**
			 * Remove anything following the first ? in the string.
			 * @param url The URL that may have URL parameters.
			 * @return std::string The URL with parameters removed.
			 */
			std::string RemoveURLParameters(std::string url);
			
			/**
			 * Calls RemoveURLSchemeAndCreds() and replaces any other characters that match "[^A-Za-z0-9.-/]" with
			 * an underscore.
			 */
			std::string MakeURLDirectorySafe(std::string url);
		}
	}
}
#endif
