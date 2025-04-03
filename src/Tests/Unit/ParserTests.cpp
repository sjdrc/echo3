#include <echo/Util/Parsers.h>
#include <echo/Util/Utils.h>

#include <doctest/doctest.h>
#undef INFO

using namespace Echo;
using namespace Echo::Parser;

void ExampleTest()
{
	/**
	 * Test the code from the example.
	 */
	Scalar mass = 120;
	CalculatorWithVariables parser;
	parser.RegisterVariable("mass",mass);
	optional<double> result = parser.Parse("mass + 2 * 3");
	CHECK(bool(result));
	if(result)
	{
		CHECK(*result==126);
	}
}

void VariableChangeTest()
{
	/**
	 * Test the code from the example.
	 */
	Scalar mass = 120;
	CalculatorWithVariables parser;
	parser.RegisterVariable("mass",mass);
	optional<double> result = parser.Parse("mass + 2 * 3");
	CHECK(bool(result));
	if(result)
	{
		CHECK(*result==126);
	}
	
	//Check that the value can be changed.
	mass = 10.5;
	result = parser.Parse("mass + 2 * 3");
	CHECK(bool(result));
	if(result)
	{
		CHECK(*result==16.5);
	}
}

void OperatorTests()
{
	CalculatorWithVariables parser;
	
	std::vector< std::pair<std::string, double> > mExpressions;
	
	mExpressions.push_back(std::make_pair("1.21 + 2.32",3.53));
	mExpressions.push_back(std::make_pair("6+5",11));
	mExpressions.push_back(std::make_pair("10-2",8));
	mExpressions.push_back(std::make_pair("10- 6",4));
	mExpressions.push_back(std::make_pair("10 * 2",20));
	mExpressions.push_back(std::make_pair("6 *7",42));
	mExpressions.push_back(std::make_pair("12/3",4));
	mExpressions.push_back(std::make_pair("12 / 4",3));
	mExpressions.push_back(std::make_pair("10%9",1));
	mExpressions.push_back(std::make_pair("127 % 10",7));
	mExpressions.push_back(std::make_pair("(5 + 3) * 4",32));

	//Test expressions
	for(size_t i =0; i < mExpressions.size(); ++i)
	{
		optional<double> result = parser.Parse(mExpressions[i].first);
		CHECK_MESSAGE(bool(result),mExpressions[i].first);
		if(result)
		{
			CHECK_MESSAGE(*result==mExpressions[i].second,mExpressions[i].first);
		}
	}
}

void ConditionalTests()
{
	ConditionalEvaluator parser;
	
	std::vector< std::pair<std::string, bool> > mExpressions;
	
	mExpressions.push_back(std::make_pair("10==10",true));
	mExpressions.push_back(std::make_pair("10!=10",false));
	mExpressions.push_back(std::make_pair("1==2",false));
	mExpressions.push_back(std::make_pair("1<2",true));
	mExpressions.push_back(std::make_pair("1>2",false));
	mExpressions.push_back(std::make_pair("1>=2",false));
	mExpressions.push_back(std::make_pair("2>=2",true));
	mExpressions.push_back(std::make_pair("3>=2",true));
	mExpressions.push_back(std::make_pair("1<=2",true));
	mExpressions.push_back(std::make_pair("2<=2",true));
	mExpressions.push_back(std::make_pair("3<=2",false));
	mExpressions.push_back(std::make_pair("1",true));
	mExpressions.push_back(std::make_pair("0",false));
	mExpressions.push_back(std::make_pair("1+2==3",true));
	mExpressions.push_back(std::make_pair("40==50-10",true));
	mExpressions.push_back(std::make_pair("8*5==50-10",true));
	mExpressions.push_back(std::make_pair("40==50-10 && 2==2",true));
	mExpressions.push_back(std::make_pair("1 || 0",true));
	mExpressions.push_back(std::make_pair("1==2 || 2==2",true));
	mExpressions.push_back(std::make_pair("1==2 || 2==3",false));
	mExpressions.push_back(std::make_pair("40==50 && 1==2 || 10==11",false));
	mExpressions.push_back(std::make_pair("1==2 && 100+1==1 || 11>=10",true));
	mExpressions.push_back(std::make_pair("1==2 && 11>=10 || 100+1==1",false));

	//Test expressions
	for(size_t i =0; i < mExpressions.size(); ++i)
	{
		optional<bool> result = parser.Parse(mExpressions[i].first);
		CHECK_MESSAGE(bool(result),mExpressions[i].first);
		if(result)
		{
			CHECK_MESSAGE(*result==mExpressions[i].second,mExpressions[i].first);
		}
	}
}

void SubTests()
{
	VariableSubstitutor parser;
	parser.RegisterVariableViaGetter("NAME",[](){return std::string("Echo");});
	parser.RegisterVariableViaGetter("COFFEE",[](){return std::string("Yes please!");});
	
	std::vector< std::pair<std::string, std::string> > mExpressions;
	
	mExpressions.push_back(std::make_pair("Hello NAME you said \"COFFEE\" to coffee.","Hello NAME you said \"COFFEE\" to coffee."));
	mExpressions.push_back(std::make_pair("Hello $NAME you said \"$COFFEE\" to coffee.","Hello Echo you said \"Yes please!\" to coffee."));
	mExpressions.push_back(std::make_pair("Hello $NAME$COFFEE hahaha.","Hello EchoYes please! hahaha."));
	mExpressions.push_back(std::make_pair("Hello $NAME/$COFFEE hahaha.","Hello Echo/Yes please! hahaha."));
	mExpressions.push_back(std::make_pair("Hello $NAME\\SomethingHere$COFFEE hahaha.","Hello Echo\\SomethingHereYes please! hahaha."));
	mExpressions.push_back(std::make_pair("Hello \\$NAME you said \"\\$COFFEE\" to coffee.","Hello $NAME you said \"$COFFEE\" to coffee."));
	mExpressions.push_back(std::make_pair("$NAME", "Echo"));
	mExpressions.push_back(std::make_pair("$NAME Engine", "Echo Engine"));
	mExpressions.push_back(std::make_pair("Use $NAME", "Use Echo"));

	//Test expressions
	for(size_t i =0; i < mExpressions.size(); ++i)
	{
		std::string result = parser.Parse(mExpressions[i].first);
		CHECK_MESSAGE(result==mExpressions[i].second,"mExpressions[i].first");
	}
}

TEST_CASE("Parser")
{
	// Turn off log output, we will just use output from this test.
	gDefaultLogger.SetLogMask(Echo::Logger::LogLevels::NONE);

	ExampleTest();
	VariableChangeTest();
	OperatorTests();
	ConditionalTests();
	SubTests();
}
