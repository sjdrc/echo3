#include <echo/Platform.h>
#include <echo/FileSystem/FileSystem.h>
#include <echo/FileSystem/FileSystemSourceVFS.h>
#include <echo/FileSystem/FileSystemSourceMemory.h>
#include <doctest/doctest.h>
#undef INFO

using namespace Echo;

TEST_CASE("File")
{
	// Turn off log output, we will just use output from this test.
	gDefaultLogger.SetLogMask(Echo::Logger::LogLevels::NONE);
	shared_ptr<FileSystem> fileSystem = Platform::CreateDefaultFileSystem("");
	REQUIRE(fileSystem);

	// There currently isn't an easy way to test for platform capabilities so we're doing expected tests based on platform.
	#if defined(ECHO_PLATFORM_LINUX) || defined(ECHO_PLATFORM_WINDOWS)
		{
			File notOpen = fileSystem->Open("NotARealFile");
			CHECK(!notOpen.IsOpen());
		}
		{
			File openFile = fileSystem->Open("ADirectory/NewRealFile",File::OpenModes::WRITE);
			CHECK(openFile.IsOpen());
			CHECK(fileSystem->DeleteFile(openFile.GetActualFileName()));
			CHECK(fileSystem->DeleteFile("ADirectory"));
		}

		// Make sure an empty filename doesn't open for read or write
		{
			File notOpenWriteEmpty = fileSystem->Open("",File::OpenModes::WRITE);
			CHECK(!notOpenWriteEmpty.IsOpen());
		}
		{
			File notOpenReadEmpty = fileSystem->Open("",File::OpenModes::READ);
			CHECK(!notOpenReadEmpty.IsOpen());
		}

		CHECK(fileSystem->CreateDirectories("single1"));
		CHECK(fileSystem->DeleteFile("single1"));

		CHECK(fileSystem->CreateDirectories("level1/level2"));
		CHECK(fileSystem->DeleteFile("level1/level2"));
		CHECK(fileSystem->DeleteFile("level1"));
	#endif
}

TEST_CASE("VFS")
{
	shared_ptr<FileSystem> fileSystem = Platform::CreateDefaultFileSystem("evfsc");
	shared_ptr<FileSystemSourceVFS> vfs = make_shared<FileSystemSourceVFS>(*fileSystem);
	REQUIRE(fileSystem->InstallSource("vfsout",vfs));
	std::string helloVFS = "Echo VFS";
	CHECK(vfs->AddFile("test.txt","memory://"+FileSystemSourceMemory::MakeMemoryFileName(helloVFS.c_str(),helloVFS.length())));
	REQUIRE(vfs->SaveVFS("vfstest.vfs",FileSystemSourceVFS::VFSEndianModes::DEFAULT,FileSystemSourceVFS::VFSEntryNameModes::FULL)==true);

	shared_ptr<FileSystemSourceVFS> vfsLoad = make_shared<FileSystemSourceVFS>(*fileSystem);
	REQUIRE(vfsLoad->LoadVFS("vfstest.vfs"));
	REQUIRE(fileSystem->InstallSource("vfs",vfsLoad));
	File file = fileSystem->Open("vfs://test.txt");
	REQUIRE(file.IsOpen());
	std::string wholeFile;
	file.ReadFileIntoString(wholeFile);
	REQUIRE(wholeFile==helloVFS);
	fileSystem->DeleteFile("vfstest.vfs");
}

#if defined(ECHO_PLATFORM_LINUX)
TEST_CASE("CURLFS")
{
	shared_ptr<FileSystem> fileSystem = Platform::CreateDefaultFileSystem("curlfs");

	{
		// Test download a file outright
		File file = fileSystem->Open("https://www.emblem.net.au/preview-EchoLogo.png");
		REQUIRE(file.IsOpen());
		REQUIRE(file.GetActualFileName()=="cache/www.emblem.net.au/preview_EchoLogo.png");
		fileSystem->DeleteFile("cache/www.emblem.net.au/preview-EchoLogo.png");
	}

	{
		// Test streaming
		File file = fileSystem->Open("https://www.emblem.net.au/preview-EchoLogo.png|stream=true|useSocketFile=false|chunkSize=1024*5|maxChunks=2|quiet=true");
		REQUIRE(file.IsOpen());
		REQUIRE(file.GetActualFileName()=="https://www.emblem.net.au/preview-EchoLogo.png");
		REQUIRE(file.GetSize() == 20436);

		std::string wholeFile;
		file.ReadFileIntoString(wholeFile);
		CHECK(wholeFile.size()==20436);
		File outFile = fileSystem->Open("preview-EchoLogo-downloaded.png",File::OpenModes::WRITE);
		REQUIRE(outFile.IsOpen());
		outFile.Write(wholeFile.data(),wholeFile.size());
		outFile.Close();
	}
}
#endif
