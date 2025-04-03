#include <sstream>
#include <echo/Util/FunctionBinder.h>
#include <echo/Chrono/CPUTimer.h>
#include <echo/cpp/functional>
#include <doctest/doctest.h>
#undef INFO

//Uncomment if you want to run a simple profiling test for FunctionBinder
#define PROFILE_FUNCTIONBINDER

std::stringstream aGlobalStringStream;

void Foo()
{
	aGlobalStringStream << "Foo()";
}

void Bar(size_t a, std::string s)
{
	aGlobalStringStream << "Bar() " << a << " " << s;
}

std::string GetString()
{
	aGlobalStringStream << "GetString()";
	return "Here is a string";
}

std::string Saw(int v)
{
	aGlobalStringStream << "Saw() " << v;
	std::stringstream ss;
	ss << v;
	return ss.str();
}

std::string See(int v, std::string w)
{
	aGlobalStringStream << "See() " << v << " " << w;
	std::stringstream ss;
	ss << v << " " << w;
	return ss.str();
}

void CheckCallResult(std::string expected)
{
	std::string result = aGlobalStringStream.str();
	CHECK(result==expected);
}

void NestedCallTests()
{
	using namespace Echo;
	FunctionBinder binder;
	
	binder.RegisterVoid("Foo", bind(&Foo));
	binder.Register("Bar", bind(&Bar,placeholders::_1,placeholders::_2),false,boost::fusion::vector<size_t,std::string>());
	binder.Register("BarNoNest", bind(&Bar,placeholders::_1,placeholders::_2),false,boost::fusion::vector<size_t,std::string>());
	shared_ptr<FunctionBinder::BoundFunction> barNoNest = binder.GetFunction("BarNoNest");
	REQUIRE(barNoNest);
	barNoNest->SetNestedCalls(false);
	binder.RegisterVoid("GetString", bind(&GetString));
	binder.Register("Saw", bind(&Saw,placeholders::_1),false,boost::fusion::vector<int>());
	binder.Register("See", bind(&See,placeholders::_1,placeholders::_2),false,boost::fusion::vector<int,std::string>());
	
	std::vector< std::pair<std::string, std::string> > testCallsAndResults;
	
	testCallsAndResults.push_back(std::make_pair("Foo()",						"Foo()"));
	testCallsAndResults.push_back(std::make_pair("Bar(1,A string)",				"Bar() 1 A string"));
	testCallsAndResults.push_back(std::make_pair("Bar(2,GetString())",			"GetString()Bar() 2 Here is a string"));
	testCallsAndResults.push_back(std::make_pair("Bar(3,Saw(321))",				"Saw() 321Bar() 3 321"));
	testCallsAndResults.push_back(std::make_pair("Bar(3,See(-5, See(100, GetString())))",	"GetString()See() 100 Here is a stringSee() -5 100 Here is a stringBar() 3 -5 100 Here is a string"));
	testCallsAndResults.push_back(std::make_pair("See(6,This \\(should\\) contain a bracket)", "See() 6 This (should) contain a bracket"));
	testCallsAndResults.push_back(std::make_pair("Bar(987,\"quote\")",		"Bar() 987 quote"));
	testCallsAndResults.push_back(std::make_pair("Bar(1000,\"GetString()\")",		"Bar() 1000 GetString()"));
	
	for(size_t i = 0; i < testCallsAndResults.size(); ++i)
	{
		//This is not thread safe
		//Clear the string.
		aGlobalStringStream.str("");
		ECHO_LOG_INFO("Call: " << testCallsAndResults[i].first);
		binder.Call(testCallsAndResults[i].first);
		CheckCallResult(testCallsAndResults[i].second);
	}
	
	// Check that more natural calls are possible.
	aGlobalStringStream.str("");
	ECHO_LOG_INFO("Call Natural: Foo()");
	binder.Call("Foo");
	CheckCallResult("Foo()");
	
	aGlobalStringStream.str("");
	ECHO_LOG_INFO("Call Natural: Bar(1,A string)");
	binder.Call("Bar",(size_t)1,std::string("A string"));
	CheckCallResult("Bar() 1 A string");

	aGlobalStringStream.str("");
	ECHO_LOG_INFO("Call Natural: Bar(2,GetString())");
	binder.Call("Bar",(size_t)2,std::string("GetString()"));
	CheckCallResult("GetString()Bar() 2 Here is a string");
	
	aGlobalStringStream.str("");
	ECHO_LOG_INFO("Call Natural: Bar(3,Saw(321))");
	binder.Call("Bar",(size_t)3,std::string("Saw(321)"));
	CheckCallResult("Saw() 321Bar() 3 321");

	aGlobalStringStream.str("");
	ECHO_LOG_INFO("Call Natural: Bar(3,See(-5, See(100, GetString())))");
	binder.Call("Bar",(size_t)3,std::string("See(-5, See(100, GetString()))"));
	CheckCallResult("GetString()See() 100 Here is a stringSee() -5 100 Here is a stringBar() 3 -5 100 Here is a string");

	aGlobalStringStream.str("");
	ECHO_LOG_INFO("Call Natural: BarNoNest(3,See(-5, See(100, GetString())))");
	binder.Call("BarNoNest",(size_t)3,std::string("See(-5, See(100, GetString()))"));
	CheckCallResult("Bar() 3 See(-5, See(100, GetString()))");

	aGlobalStringStream.str("");
	ECHO_LOG_INFO("Call Natural: See(6,This \\(should\\) contain a bracket)");
	binder.Call("See",(int)6,std::string("This (should) contain a bracket"));
	CheckCallResult("See() 6 This (should) contain a bracket");

	aGlobalStringStream.str("");
	ECHO_LOG_INFO("Call Natural: Bar(987,quote)");
	binder.Call("Bar",(size_t)987,std::string("quote"));
	CheckCallResult("Bar() 987 quote");

	aGlobalStringStream.str("");
	ECHO_LOG_INFO("Call Natural: Bar(1000,\"GetString()\")");
	binder.Call("Bar",(size_t)1000,std::string("\"GetString()\""));
	CheckCallResult("Bar() 1000 \"GetString()\"");
}


void CallOnly(int, std::string)
{
}

std::string CallOnlyString()
{
	return std::string();
}

void SpeedTest()
{
	using namespace Echo;
	using namespace Echo::Timer;
	
	CPUTimer profiler;
	FunctionBinder binder;
	
	binder.RegisterVoid("CallOnlyString", bind(&CallOnlyString));
	binder.Register("CallOnly", bind(&See,placeholders::_1,placeholders::_2),false,boost::fusion::vector<int,std::string>());
	
	size_t iterations = 3;
	size_t multiplier = 10;
	size_t runCount = 5;
	size_t testCount = 1000;
	
	for(size_t k =0; k < iterations ; ++k)
	{
		ECHO_LOG_INFO(testCount << " calls");
		Seconds averagesDirectA(0.f);
		Seconds averagesDirectB(0.f);
		Seconds averageBinderA(0.f);
		Seconds averageBinderB(0.f);
		Seconds averageBinderFunctionA(0.f);
		Seconds averageBinderFunctionB(0.f);
		Seconds averageBinderFunctionAnyA(0.f);
		for(size_t r =0; r < runCount ; ++r)
		{
			ECHO_LOG_INFO("  Run: " << r);
			profiler.Start();
			for(size_t t =0; t < testCount ; ++t)
			{
				CallOnly(3,"Hello");
			}
			Seconds testTime = profiler.Stop();
			ECHO_LOG_INFO("    Direct CallOnly(3,Hello) x "<< testCount << ":" << testTime.count() << "s");
			averagesDirectA += testTime;

			profiler.Start();
			for(size_t t =0; t < testCount ; ++t)
			{
				CallOnly(3,CallOnlyString());
			}
			testTime = profiler.Stop();
			averagesDirectB += testTime;
			ECHO_LOG_INFO("    Direct CallOnly(3,CallOnlyString()) x "<< testCount << ":" << testTime.count() << "s");

			profiler.Start();
			for(size_t t =0; t < testCount ; ++t)
			{
				binder.Call("CallOnly(3,Hello)");
			}
			testTime = profiler.Stop();
			ECHO_LOG_INFO("CallOnly(3,Hello) x "<< testCount << ":" << testTime.count() << " s");
			averageBinderA += testTime;

			profiler.Start();
			for(size_t t =0; t < testCount ; ++t)
			{
				binder.Call("CallOnly(3,CallOnlyString())");
			}
			testTime = profiler.Stop();
			ECHO_LOG_INFO("CallOnly(3,CallOnlyString()) x "<< testCount << ":" << testTime.count() << " s");
			averageBinderB += testTime;

			profiler.Start();
			shared_ptr<FunctionBinder::BoundFunction> boundFunction = binder.GetFunction("CallOnly");
			REQUIRE(boundFunction);
			for(size_t t =0; t < testCount ; ++t)
			{
				(*boundFunction)(binder,3,"Hello");
			}
			testTime = profiler.Stop();
			ECHO_LOG_INFO("Bound call CallOnly(3,Hello) x "<< testCount << ":" << testTime.count() << " s");
			averageBinderFunctionA += testTime;

			profiler.Start();
			for(size_t t =0; t < testCount ; ++t)
			{
				(*boundFunction)(binder,3,"CallOnlyString()");
			}
			testTime = profiler.Stop();
			ECHO_LOG_INFO("Bound call CallOnly(3,\"CallOnlyString()\") x "<< testCount << ":" << testTime.count() << " s");
			averageBinderFunctionB += testTime;

			profiler.Start();
			std::vector< boost::any > anyParams;
			anyParams.push_back(static_cast<int>(3));
			anyParams.push_back(static_cast<std::string>("Hello"));
			for(size_t t =0; t < testCount ; ++t)
			{
				(*boundFunction)(binder,anyParams);
			}
			testTime = profiler.Stop();
			ECHO_LOG_INFO("Bound call Any CallOnly(3,Hello) x "<< testCount << ":" << testTime.count() << " s");
			averageBinderFunctionAnyA += testTime;

		}
		ECHO_LOG_INFO("Average for Direct CallOnly(3,Hello) over " << runCount << " tests of " << testCount << " calls " << (averagesDirectA.count() / static_cast<double>(runCount)) << " seconds.");
		ECHO_LOG_INFO("Average for Direct CallOnly(3,CallOnlyString()) over " << runCount << " tests of " << testCount << " calls " << (averagesDirectB.count() / static_cast<double>(runCount)) << " seconds.");
		ECHO_LOG_INFO("Average for Binder CallOnly(3,Hello) over " << runCount << " tests of " << testCount << " calls " << (averageBinderA.count() / static_cast<double>(runCount)) << " seconds.");
		ECHO_LOG_INFO("Average for Binder CallOnly(3,CallOnlyString()) over " << runCount << " tests of " << testCount << " calls " << (averageBinderB.count() / static_cast<double>(runCount)) << " seconds.");
		ECHO_LOG_INFO("Average for Binder Function CallOnly(3,Hello) over " << runCount << " tests of " << testCount << " calls " << (averageBinderFunctionA.count() / static_cast<double>(runCount)) << " seconds.");
		ECHO_LOG_INFO("Average for Binder Function CallOnly(3,\"CallOnlyString()\") over " << runCount << " tests of " << testCount << " calls " << (averageBinderFunctionB.count() / static_cast<double>(runCount)) << " seconds.");
		ECHO_LOG_INFO("Average for Binder Function Any params CallOnly(3,\"CallOnlyString()\") over " << runCount << " tests of " << testCount << " calls " << (averageBinderFunctionAnyA.count() / static_cast<double>(runCount)) << " seconds.");
		testCount*=multiplier;
	}
}

TEST_CASE("FunctionBinder")
{
	// Turn off log output, we will just use output from this test.
	Echo::gDefaultLogger.SetLogMask(Echo::Logger::LogLevels::INFO);
	NestedCallTests();
	
#ifdef PROFILE_FUNCTIONBINDER
	SpeedTest();
#endif
}
