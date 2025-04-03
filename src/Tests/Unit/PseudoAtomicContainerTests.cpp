#include <echo/Util/PseudoAtomicMap.h>
#include <doctest/doctest.h>
#undef INFO

using namespace Echo;

TEST_CASE("PseudoAtomicMap")
{
	// Turn off log output, we will just use output from this test.
	gDefaultLogger.SetLogMask(Echo::Logger::LogLevels::NONE);
	
	// Check book keeping works

	PseudoAtomicMap< std::string, Size > pAMap;

	pAMap.Insert("A",10);
	pAMap.Insert("A",20); // Should not insert

	{
		std::map< std::string, Size >& contents = pAMap.GetMap();
		REQUIRE(contents.find("A")!=contents.end());
		CHECK(contents.find("A")->second==10);
	}

	pAMap.Set("A",20);
	{
		std::map< std::string, Size >& contents = pAMap.GetMap();
		REQUIRE(contents.find("A")!=contents.end());
		CHECK(contents.find("A")->second==20);
	}

	pAMap.Erase("A");
	pAMap.Insert("A",30);
	{
		std::map< std::string, Size >& contents = pAMap.GetMap();
		REQUIRE(contents.find("A")!=contents.end());
		CHECK(contents.find("A")->second==30);
	}
}

TEST_CASE("PseudoAtomicMappedContainer")
{
	// Turn off log output, we will just use output from this test.
	gDefaultLogger.SetLogMask(Echo::Logger::LogLevels::NONE);
	
	// Check book keeping works

	PseudoAtomicMappedContainer< std::string, std::vector< Size > > pAMap;

	pAMap.Add("A",10);
	pAMap.Add("A",20);

	{
		std::map< std::string, std::vector< Size > >& contents = pAMap.GetMap();
		REQUIRE(contents.find("A")!=contents.end());
		REQUIRE(contents.find("A")->second.size()==2);
		CHECK(contents.find("A")->second[0]==10);
		CHECK(contents.find("A")->second[1]==20);
	}

	pAMap.Remove("A");
	pAMap.Add("A",30);
	{
		std::map< std::string, std::vector< Size > >& contents = pAMap.GetMap();
		REQUIRE(contents.find("A")!=contents.end());
		REQUIRE(contents.find("A")->second.size()==1);
		CHECK(contents.find("A")->second[0]==30);
	}
}
