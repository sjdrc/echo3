#ifndef _ECHOFILE_H_
#define _ECHOFILE_H_
#include <echo/Types.h>
#include <string>
#include <vector>

namespace Echo
{
	class FileSystem;
	class FileReference;
	
	class File
	{
	public:
		/**
		 * Setting an endian mode tells the File object what the file data endianness is expected to be.
		 * @see SetEndianMode()
		 */
		struct EndianModes
		{
			enum _ {UNKNOWN = 0,
					BIG,
					LITTLE};
		};
		typedef EndianModes::_ EndianMode;
		
		/**
		 * File Open modes.
		 * @note Text mode is intentionally omitted. All files are opened in binary format.
		 */
		struct OpenModes
		{
			enum _{	READ,
					WRITE,
					APPEND,
					READ_WRITE,
					READ_APPEND};
		};
		typedef OpenModes::_ OpenMode;

		File();
		~File();
		File(const File& rhs);
		bool operator==(const File& rhs) const;
		File& operator=(const File& rhs);
		
		//!\brief Attempts to close the file.
		//!\details If the same file is referenced by multiple File object all file objects need to be closed
		//!			or destroyed for the file to close.
		//!\return true if this was the last file object referencing the file.
		bool Close();

		template< typename T >
		size_t Read(T& object)
		{
			return Read(&object,sizeof(T));
		}
		
		size_t Read(void* buffer, size_t numberOfBytes)
		{
			return Read(buffer,1,numberOfBytes);
		}
		template< typename T >
		size_t Write(const T& object)
		{
			return Write(&object,sizeof(T));
		}
		size_t Write(const std::string& s);
		size_t Write(const char* s);
		size_t Write(const void* buffer, size_t numberOfBytes)
		{
			return Write(buffer,1,numberOfBytes);
		}

		/**
		 * Read data into a buffer.
		 * The total amount of data read is typeSize*numberToGet
		 * @param buffer The destination buffer.
		 * @param typeSize The type size, see SetEndianMode(), set to 1 if no conversion should be done.
		 * @param numberToGet The number of elements to get.
		 * @return the total number of bytes read.
		 */
		size_t Read(void* buffer, size_t typeSize, size_t numberToGet);

		size_t Write(const void* buffer, size_t typeSize, size_t numberToGet);
		size_t Seek(size_t position);
		size_t GetSize();
		size_t GetPosition() const;
		bool EndOfFile();

		/**
		 * Get the requested file name.
		 * The requested file name is the one specified in FileSystem::Open().
		 * @return the requested file name.
		 */
		const std::string& GetRequestedFileName() const {return mRequestedFileName;}

		/**
		 * Get the actual file name.
		 * The file name that was specified during a call to FileSystem::Open() may need to be modified
		 * by FileSystemSource implementations to access a file. This method returns the modified name.
		 * @return The actual file name used by the implementation.
		 */
		const std::string& GetActualFileName() const {return mActualFileName;}
		
		/**
		 * Get the FileReference object.
		 * If a file is opened this will be a valid specialisation created by the FileSystemSource
		 * implementation that opened the file.
		 * Normally you don't need to deal with these objects.
		 * @return File reference object.
		 */
		shared_ptr<FileReference> GetReference() const {return mFileReference;}

		/**
		 * Get the endian mode that the file object is in.
		 * @see SetEndianMode()
		 * @return The endian mode this File object is using.
		 */
		const EndianMode& GetEndianMode() const {return mEndianMode;}

		/**
		 * Sets the data's expected endian format.
		 * When endian conversion has been enabled on the file and the endian format is known, the file
		 * object can automatically re-order the bytes read or written if the platform endianness does
		 * not match.
		 * @note This is per File object so if the same file is opened and referenced by multiple File
		 * objects (i.e. they share a FileReference) then modifying this only affects the current File
		 * object's operations.
		 * @param newEndianMode the file's expected endianness.
		 */
		void SetEndianMode(EndianMode newEndianMode){mEndianMode=newEndianMode;}
		
		void SetEndianConversionEnabled(bool enabled){mConvertEndian=enabled;}
		bool IsConvertEndianEnabled() const {return mConvertEndian;}

		bool IsOpen() const {return (mFileReference!=0);}

		void ReadLines(std::vector<std::string>& linesOut, bool cullSpaces=true, std::string ignoreLinesStartingWithOneOf = "@#");
		void ReadFileIntoString(std::string& outString);

		/**
		 * Constructor typically used by FileSystemSources.
		 * Since File is a wrapper around a FileReference, creating a File object using this constructor without a valid FileReference
		 * won't give you access to any data. Typically you should be opening a File using a FileSystem object.
		 * In some cases you might decide that using this overload manually is the behaviour you want. For example, you could create a
		 * File with this constructor and pass in a FileReferenceMemory to access memory as a File. Although that is technically
		 * unnecessary as you can do the same with the static method FileSystemSourceMemory::OpenDirect().
		 * @param requestedFileName the name the client code used to request access to the file
		 * @param actualFileName Name transformed based on the FileSystemSource's current directory and any internal name lookup.
		 */
		File(const std::string& requestedFileName, const std::string& actualFileName, shared_ptr<FileReference> fileReference);
		
		/**
		 * Constructor typically used by FileSystemSources.
		 * Since File is a wrapper around a FileReference, creating a File object using this constructor without a valid FileReference
		 * won't give you access to any data. Typically you should be opening a File using a FileSystem object.
		 * This overload is used to create File objects that could not be opened but contain file name information.
		 * @param requestedFileName the name the client code used to request access to the file
		 * @param actualFileName Name transformed based on the FileSystemSource's current directory and any internal name lookup.
		 */
		File(const std::string& requestedFileName, const std::string& actualFileName);
	private:
		friend class FileSystem;
		std::string mRequestedFileName;
		std::string mActualFileName;
		shared_ptr<FileReference> mFileReference;
		EndianMode mEndianMode;
		//When mConvertEndian is true the byte order of data is converted
		//to the opposite when read or written. The following occurs (and
		//checks in this order):
		//typeSize&1 - not converted
		//typeSize==2 - bytes swapped: 1234 > 3412
		//typeSize>=4 - bytes swapped: 1234 > 3412 and every > 2 bytes
		//swapped with previous two bytes 1234 5678 > 7856 3412
		bool mConvertEndian;
	};
}
#endif
