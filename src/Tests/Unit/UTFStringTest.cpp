#include <echo/UTF8String.h>
#include <echo/Util/StringUtils.h>

#include <doctest/doctest.h>

using namespace Echo;

void BasicIteratorTest()
{
	UTF8String twochars("\xe6\x97\xa5\xd1\x88");
	UTF8String::iterator it = twochars.begin();
	UTF32Code code = *it;
	CHECK(code == 0x65e5);
	CHECK(bool(it == twochars.begin()));
}

void IteratorLoopTest()
{
	//ASCII strings below 127 are UTF8 compatible, so this should allow us to test iterator loops and see the correct output.
	const std::string TEST_STRING = "abcdef";
	
	UTF8String testString(TEST_STRING);
	UTF8String::iterator it = testString.begin();
	UTF8String::iterator itEnd = testString.end();
	
	std::string output;
	while(it!=itEnd)
	{
		UTF32Code code = *it;
		//This is only possible because ASCII strings are UTF8 compatible. If we were using characters > 127 then this wouldn't work.
		output+=static_cast<char>(code);
		it++;
	}
	CHECK(output==TEST_STRING);
}

void OperatorTest()
{
	UTF8String utf8String("Hello");
	std::string normalString(" world");		
	UTF8String finalString;

	finalString+=utf8String;
	CHECK(finalString.GetContent()=="Hello");

	finalString+=normalString;
	CHECK(finalString.GetContent()=="Hello world");

	UTF8String excl("!");
	UTF32Code code = *excl.begin();
	finalString+=code;
	CHECK(finalString.GetContent()=="Hello world!");
}

template< typename RightType >
void SplitTest(std::string stringToSplit, std::string separators, std::string expectedLeft, RightType expectedRight, bool expectedResult)
{
	std::string left;
	RightType right;

	bool result = Utils::String::SplitInTwo(stringToSplit,separators,left,right);
	
	CHECK(result==expectedResult);
	CHECK(left==expectedLeft);
	CHECK(bool(right==expectedRight));
}

void SplitTests()
{
	//This test is testing the std::string version of the method too.
	// We are using the test data from the documentation.
	
	SplitTest<std::string>("Hello=There","=","Hello","There",true);
	SplitTest<std::string>("Hello there=","=","Hello there","",true);
	SplitTest<std::string>("=Hello there","=","","Hello there",true);
	SplitTest<std::string>("Hello there","=","","",false);
	SplitTest<std::string>("Hello=Thy,Person","=,","Hello","Thy,Person",true);
	SplitTest<UTF8String>("Hello=world","=,","Hello",UTF8String("world"),true);
}

void TestAllASCII()
{
	UTF8String allASCII("The thing we said, was something like this: Readable in code");
	UTF8String nonASCII("The thing we said, was something like this:\xe6\x97\xa5\xd1\x88");
	CHECK(allASCII.ContainsAllASCIICharacters());
	CHECK(!nonASCII.ContainsAllASCIICharacters());
}

TEST_CASE("UTF8String")
{
	// Turn off log output, we will just use output from this test.
	gDefaultLogger.SetLogMask(Echo::Logger::LogLevels::NONE);

	BasicIteratorTest();
	IteratorLoopTest();
	OperatorTest();
	SplitTests();
	TestAllASCII();
}
