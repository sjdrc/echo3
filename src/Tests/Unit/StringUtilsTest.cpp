#include <echo/Util/StringUtils.h>
#include <echo/Util/Utils.h>

#include <doctest/doctest.h>
#undef INFO

TEST_CASE("StringUtils")
{
	using namespace Echo;
	// Turn off log output, we will just use output from this test.
	gDefaultLogger.SetLogMask(Echo::Logger::LogLevels::NONE);

	typedef std::pair<std::string, std::string> StringAndResult;

	SUBCASE("GetPathFromFilename")
	{
		StringAndResult tests[] =	{	StringAndResult("noPath",""),				//No path
										StringAndResult("a/myFile","a/"),			//With a path
										StringAndResult("a/b/myFile.ext","a/b/"),	//With a longer path
			};
		for(size_t i=0;i<SizeOfArray(tests);++i)
		{
			std::string result=Utils::String::GetPathFromFilename(tests[i].first);
			CHECK(result==tests[i].second);
		}
	}	
	
	SUBCASE("GetFileExtension")
	{
		StringAndResult tests[] =	{	StringAndResult("noExtension",""),		//No extension
										StringAndResult("a/myFile",""),			//With a path
										StringAndResult("myFile.ext","ext"),	//With extension
										StringAndResult("a/myFile.txt","txt")	//With path and extension.
			};
		for(size_t i=0;i<SizeOfArray(tests);++i)
		{
			std::string result=Utils::String::GetFileExtension(tests[i].first);
			CHECK(result==tests[i].second);
		}
	}	
	
	SUBCASE("ReducePath")
	{
		StringAndResult tests[] =	{	StringAndResult("a/b/../c","a/c"),
										StringAndResult("a/b/c","a/b/c"),
										StringAndResult("/a/b/../c","/a/c"),
										StringAndResult("a/b/c/d/../../e","a/b/e"),
										StringAndResult("a/b/c/d/../../e/../f","a/b/f"),
										StringAndResult("a/b/c/d/../../e/../../f","a/f"),
										StringAndResult("./a//b/c","a/b/c"),
										StringAndResult("./a/b/./c","a/b/c"),
										StringAndResult("a","a"),
										StringAndResult("","")
		};
		for(size_t i=0;i<SizeOfArray(tests);++i)
		{
			std::string result=Utils::String::ReducePath(tests[i].first);
			CHECK(result==tests[i].second);
		}
	}

	SUBCASE("SplitWithQuotes")
	{
		typedef std::pair<std::string, std::vector<std::string> > StringAndVectorResult;
		StringAndVectorResult tests[] =	{
			{"Hello",{"Hello"}},
			{"Hello,World",{"Hello","World"}},
			{"\"One,Two,Three\"",{"One,Two,Three"}},
			{"\"\\\"One\\\",\\\"Two\\\",Three\"",{"\"One\",\"Two\",Three"}},
			{"One,\"One,Two\",\"One,Two,Three\"",{"One","One,Two","One,Two,Three"}}
		};
		for(size_t i=0;i<SizeOfArray(tests);++i)
		{
			std::vector<std::string> splitResult;
			Utils::String::Split(tests[i].first,splitResult);
			CHECK(splitResult.size()==tests[i].second.size());
			if(splitResult.size()==tests[i].second.size())
			{
				for(size_t s=0;s<splitResult.size();++s)
				{
					CHECK(tests[i].second[s]==splitResult[s]);
				}
			}
		}
	}	

	SUBCASE("EscapeCharacters")
	{
		StringAndResult tests[] =	{	StringAndResult("abc","abc"),
										StringAndResult("\"abc\"","\\\"abc\\\""),
										StringAndResult("\"a\"bc\"","\\\"a\\\"bc\\\""),
										StringAndResult("","")
		};
		for(size_t i=0;i<SizeOfArray(tests);++i)
		{
			std::string result=Utils::String::EscapeCharacters(tests[i].first, "\"");
			CHECK(result==tests[i].second);
		}
	}

	SUBCASE("URLTests")
	{
		std::string safeURL = Utils::String::MakeURLDirectorySafe("https://user:pass@somewhere.test.com/path/to/file.txt");
		CHECK(safeURL == "somewhere.test.com/path/to/file.txt");

		std::string longURL = Utils::String::MakeURLDirectorySafe("https://user:pass@somewhere.test.com/123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890/longname/123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890.txt");
		CHECK(longURL == "somewhere.test.com/1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123__/longname/123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789__.txt");

		std::string paramsInURL = Utils::String::MakeURLDirectorySafe("https://user:pass@somewhere.test.com/123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890/longname/123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890.txt?something=avalue&anotherValue=avalue");
		CHECK(longURL == "somewhere.test.com/1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123__/longname/123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789__.txt");
	}
}
