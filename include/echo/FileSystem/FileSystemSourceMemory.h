#ifndef _ECHOFILESYSTEMSOURCEMEMORY_H_
#define _ECHOFILESYSTEMSOURCEMEMORY_H_
#include <echo/Types.h>
#include <echo/FileSystem/File.h>
#include <echo/FileSystem/FileSystemSource.h>

namespace Echo
{
	class FileSystemSourceMemory : public FileSystemSource
	{
	public:
		FileSystemSourceMemory(const std::string& identifier="memory");
		virtual ~FileSystemSourceMemory();

		/**
		 * Helper to make a memory file name from an address and size.
		 * @note This does not include the identifier since the identifier can change based on how it is installed.
		 * @param address The pointer to the start of the memory block.
		 * @param dataSize The maximum number of bytes that can be read.
		 * @return std::string A string with the format "address:size" which can be used to open the memory block as a file via
		 * a FileSystem.
		 */
		inline static std::string MakeMemoryFileName(const void* address, Size dataSize)
		{
			std::stringstream ss;
			ss << address << ":" << dataSize;
			return ss.str();
		}

		/**
		 * Helper to make a memory file name from an array.
		 * @note This does not include the identifier since the identifier can change based on how it is installed.
		 * @param t A reference to a staticly sized array. The size is automatically deduced by the compiler.
		 * @return std::string A string with the format "address:size" which can be used to open the memory block as a file via
		 * a FileSystem.
		 */
		template<typename T, size_t size>
		inline static std::string MakeMemoryFileName(T (&t)[size])
		{
			return MakeMemoryFileName(t,sizeof(T)*size);
		}

		/**
		 * Helper to make a memory file name from a const char array.
		 * @note This does not include the identifier since the identifier can change based on how it is installed.
		 * @param string The start of the string. This must be in scope while the file may be accessed.
		 * @param lengthInBytes 
		 * @return std::string 
		 */
		inline static std::string MakeMemoryFileName(const char* string, Size lengthInBytes)
		{
			return MakeMemoryFileName(reinterpret_cast<const void*>(string),lengthInBytes);
		}

		/**
		 * Open a block of memory as a File.
		 * @note This function will return an unmanaged File object, this just means that you won't be able to
		 * query a FileSystem about File objects that were opened this way.
		 * @param address The memory address to point to.
		 * @param dataSize The size of the block in bytes.
		 * @return A File object that references the specified memory for data.
		 */
		static File OpenDirect(const void* address, Size dataSize);

		/**
		 * Open a read/write block of memory as a File.
		 * @note This function will return an unmanaged File object, this just means that you won't be able to
		 * query a FileSystem about File objects that were opened this way.
		 * @param address The memory address to point to.
		 * @param dataSize The size of the block in bytes.
		 * @return A File object that references the specified memory for data.
		 */
		static File OpenDirect(void* address, Size dataSize);
	protected:
		File _Open(const std::string& originalFileName, const std::string& modifiedFileName, File::OpenMode openMode = File::OpenModes::READ) override;
	};
}
#endif
