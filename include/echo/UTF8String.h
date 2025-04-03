#ifndef _ECHOUTFSTRING_H_
#define _ECHOUTFSTRING_H_
#include <echo/Types.h>
#include <string>
#include <vector>
#include <iostream>
#include <utf8cpp/utf8.h>

namespace Echo
{
	typedef std::vector<UTF16Code> UTF16String;
	typedef std::vector<UTF32Code> UTF32String;
	
	/**
	 * UTF8String is a Unicode string that uses UTF-8 encoding.
	 * The class is essentially a C++ wrapper around some of the libutfcpp.
	 * See http://utfcpp.sourceforge.net/
	 * Feel free to add functionality as needed.
	 * 
	 * Example:
	 * UTF8String greeting("coi ro do");
	 * UTF8String something("\xE2\x88\x83y \xE2\x88\x80x \xC2\xAC(x \xE2\x89\xBA y)");
	 */
	class UTF8String
	{
	public:
		/**
		 * Default constructor.
		 */
		UTF8String()
			: mContent()
		{
		}
		
		/**
		 * Construct a UTF8String from a null-terminated ASCII character sequence.
		 * The character sequence should not use any extended ASCII characters, this may result in unexpected UTF-8 strings.
		 * @param asciiCharString
		 */
		UTF8String(const char* asciiCharString)
		{
			std::string asciiString(asciiCharString);
			std::string utfSafe;
			utf8::replace_invalid(asciiString.begin(), asciiString.end(), std::back_inserter(utfSafe));
			mContent = utfSafe;
		}

		/**
		 * Construct a UTF8String from a null-terminated ASCII character sequence.
		 * The character sequence should not use any extended ASCII characters, this may result in unexpected UTF-8 strings.
		 * @param asciiCharString
		 */
		UTF8String(const std::string& asciiString)
		{
			std::string utfSafe;
			utf8::replace_invalid(asciiString.begin(), asciiString.end(), std::back_inserter(utfSafe));
			mContent = utfSafe;
		}
		
		/**
		 * Copy constructor
		 * @param utfString
		 */
		UTF8String(const UTF8String& utfString) : mContent(utfString.mContent)
		{
			
		}
		
		/**
		 * Destructor
		 */
		~UTF8String()
		{
			
		}
		
		/**
		 * Iterator class.
		 * When an iterator is dereferenced on a valid iterator a UTF-32 code is returned.
		 */
		class iterator
		{
		public:
			/**
			 * Constructor used by UTF8String to assign the position.
			 */
			iterator(const UTF8String& utf8String, const char* position) : mString(&utf8String)
			{
				if(position)
				{
					mPosition = position;
					mCurrent = utf8::next(mPosition,mString->raw_end());
				}else
				{
					mPosition = 0;
					mCurrent = 0;
				}
			}

			/**
			 * Copy constructor
			 */
			iterator(const iterator& rhs) : mPosition(rhs.mPosition), mString(rhs.mString), mCurrent(rhs.mCurrent)
			{
			}
			
			/**
			 * Default constructor.
			 */
			iterator() : mPosition(0), mString(0), mCurrent(0)
			{
			}
			
			/**
			 * Prefix
			 */
			const iterator& operator++()
			{
				try
				{
					mCurrent = utf8::next(mPosition,mString->raw_end());
				}catch(utf8::not_enough_room&)
				{
					//Now it's an end iterator.
					mPosition = 0;
					mCurrent = 0;
				}
				return *this;
			}

			/**
			 * Postfix
			 */
			iterator operator++(int)
			{
				iterator i = *this;
				operator++();
				return i;
			}			

			bool operator==(const iterator& rhs)
			{
				return mPosition==rhs.mPosition;
			}
			
			bool operator!=(const iterator& rhs)
			{
				return mPosition!=rhs.mPosition;
			}

			iterator& operator=(const iterator& rhs)
			{
				if (this == &rhs)
					return *this;
				
				mPosition=rhs.mPosition;
				mString=rhs.mString;
				mPosition=rhs.mPosition;
				return *this;
			}
			
			/**
			 * Dereferencing the iterator returns the UTF32Code.
			 * The returned code will be 0 for invalid iterators.
			 * @return UTF32Code.
			 */
			UTF32Code operator*() const {return mCurrent;}
		private:
			const char* mPosition;		//When 
			const UTF8String* mString;
			UTF32Code mCurrent;
		};
		
		/**
		 * Get a iterator pointing to the beginning of the UTF-8 sequence.
		 * @return 
		 */
		iterator begin()
		{
			return iterator(*this, raw_begin());
		}

		iterator end()
		{
			//End iterators have a position value of 0.
			return iterator(*this, 0);
		}

		iterator begin() const
		{
			return iterator(*this, raw_begin());
		}

		iterator end() const
		{
			//End iterators have a position value of 0.
			return iterator(*this, 0);
		}
		
		size_t Length() const
		{
			try
			{
				return utf8::distance(mContent.begin(), mContent.end());
			} catch(utf8::invalid_code_point& except)
			{
				ECHO_LOG_ERROR(except.what());
				return 0;
			}
		}
		
		UTF8String& operator=(const UTF8String& rhs)
		{
			if (this == &rhs)
				return *this;
			
			mContent=rhs.mContent;
			return *this;
		}
		
		bool operator==(const UTF8String& rhs)
		{
			return mContent==rhs.mContent;
		}

		bool operator!=(const UTF8String& rhs)
		{
			return mContent!=rhs.mContent;
		}
		const UTF8String& operator+=(const UTF8String& rhs)
		{
			mContent+=rhs.mContent;
			return *this;
		}

		UTF8String operator+(const UTF8String& rhs) const
		{
			UTF8String newString(*this);
			newString+=rhs;
			return newString;
		}

		const UTF8String& operator+=(const std::string& rhs)
		{
			UTF8String utf8rhs(rhs);
			mContent+=utf8rhs.mContent;
			return *this;
		}
		
		const UTF8String& operator+=(UTF32Code code)
		{
			try
			{
				utf8::append(code,std::back_inserter(mContent));
			}
			catch(utf8::invalid_code_point& except)
			{
				ECHO_LOG_ERROR(except.what());
			}
			return *this;
		}
		
		friend std::ostream& operator<<(std::ostream& o, const UTF8String& s)
		{
			//Unix systems tend to be able to handle UTF-8 strings in consoles but might be a problem for other platforms.
			o << s.mContent;
			//o << "UTF8String(" << s.Length() << "):" << s.mContent;
			return o;
		}
		
		/**
		 * Convert to a UTF16String
		 * @return 
		 */
		operator UTF16String() const
		{
			UTF16String utf16String;
			utf8::utf8to16(mContent.begin(), mContent.end(), std::back_inserter(utf16String));
			return utf16String;
		}
		
		/**
		 * Convert to a UTF32String
		 * @return 
		 */
		operator UTF32String() const
		{
			UTF32String utf32String;
			utf8::utf8to32(mContent.begin(), mContent.end(), std::back_inserter(utf32String));
			return utf32String;
		}
		
		UTF8String& operator=(const UTF16String& rhs)
		{
			utf8::utf16to8(rhs.begin(), rhs.end(), std::back_inserter(mContent));
			return *this;
		}
		
		/**
		 * Allows you to get the UTF-8 sequence.
		 * NOTE: The returned string generally should not be used as a normal string. This method is
		 * made available for situations where you might need the raw data something, like to writing
		 * to a file or sending over a network.
		 * Alternatively if ContainsAllASCIICharacters() returns true then it is safe to use the returned.
		 */
		const std::string& GetContent() const {return mContent;}

		void clear()
		{
			mContent.clear();
		}
		
		/**
		 * Check whether the string contains all ASCII characters.
		 * If it does then it is safe to use GetContent() and store the string as a std::string.
		 * @return true if all characters are ASCII characters, otherwise false.
		 */
		bool ContainsAllASCIICharacters()
		{
			for(iterator c=begin(); c!=end();++c)
			{
				// 0x80 is 128 which is the 8th bit in a byte, this indicates in UTF8 that the code point
				// contains more code units. http://en.wikipedia.org/wiki/UTF-8 explains how UTF8 is encoded.
				if(*c>=UTF32Code(0x80))
				{
					return false;
				}
			}
			//no characters >=0x80 were found, it is an ASCII string.
			return true;
		}
	private:
		std::string mContent;	//Use a normal string to simplify operations.
		friend class iterator;
		const char* raw_begin() const
		{
			return mContent.c_str();
		}
		const char* raw_end() const
		{
			return mContent.c_str()+mContent.length();
		}
	};
}
#endif
