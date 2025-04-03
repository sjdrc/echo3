#include <echo/Types.h>
#include <echo/Maths/Vector3.h>
#include <echo/Platform.h>
#include <echo/FileSystem/FileSystem.h>
#include <echo/Util/Configuration.h>
#include <echo/Util/FunctionBinder.h>

#include <echo/cpp/functional>
#include <iostream>
#include <doctest/doctest.h>

template<typename T>
void CheckSectionValueT(Echo::Configuration& configuration, std::string section, std::string optionName, T expectedValue)
{
	// This is all about access not value conversion since that's the same as the base
	T value = configuration.GetFromSection(section,optionName,T());
	std::stringstream ss;
	ss << "Section: " << section << "Option: " << optionName << " does not match expected value";
	CHECK_MESSAGE(value==expectedValue,ss.str());
}

void CheckSectionValue(Echo::Configuration& configuration, std::string section, std::string optionName, std::string expectedValue)
{
	// This is all about access not value conversion since that's the same as the base
	std::string value = configuration.GetFromSection(section,optionName,"");
	std::stringstream ss;
	ss << "Section: " << section << "Option: " << optionName << " does not match expected value";
	CHECK_MESSAGE(value==expectedValue,ss.str());
}

void SetInternalValue(std::string* stringToSet, std::string v)
{
	*stringToSet=v;
}

TEST_CASE("Configuration")
{
	using namespace Echo;
	
	// Turn off log output, we will just use output from this test.
	//gDefaultLogger.SetLogMask(Echo::Logger::LogLevels::NONE);
	
	
	shared_ptr<FileSystem> fileSystem=Platform::CreateDefaultFileSystem("ConfigurationTest");
	REQUIRE(fileSystem);

	File file;
	
	//Test writing a configuration file.
	Configuration output(fileSystem);
	output.Add("position",Vector3(10.20,20.10,30.30));
	output.Add("name","Sean");
	output.Add("number",1234567);
	output.AddLine("SetInternalValue(Testing)");
	REQUIRE(output.WriteFile("ConfigurationTest.temp"));
	
	shared_ptr<FunctionBinder> binder(new FunctionBinder());
	std::string myInternalValue;
	binder->Register("SetInternalValue",bind(&SetInternalValue,&myInternalValue,placeholders::_1),false,boost::fusion::vector<std::string>());
	
	//Test the configuration file.
	Configuration configuration(fileSystem, binder);

	//Load
	REQUIRE(configuration.LoadFile("ConfigurationTest.temp"));
	
	//Attempt to get a Vector3
	Vector3 position;
	configuration.Get("position",position,Vector3::ZERO);
	CHECK(position==Vector3(10.20,20.10,30.30));
	
	//Test setting a value directly
	REQUIRE(configuration.Set("setNumber", 987654u));
	CHECK(configuration.Get("setNumber", 0u) == 987654u);
	REQUIRE(configuration.Add("listOfNumbers", 1u));
	REQUIRE(configuration.Add("listOfNumbers", 22u));
	CHECK(configuration.GetAtIndex<u32>("listOfNumbers",0,0)==1);
	CHECK(configuration.GetAtIndex<u32>("listOfNumbers",0,1)==22);

	//Attempt to get a string
	std::string name;
	configuration.Get("name",name,"NotSet");
	CHECK(name!="NotSet");

	//Attempt to get a u32
	u32 number;
	configuration.Get<u32>("number",number,0);
	CHECK(number!=0);
	
	//Make sure that attempting to get a value that doesn't exist fails and sets default.
	configuration.Get<u32>("age",number,100);
	CHECK(number==100);
	
	//Try parsing a file.
	file = fileSystem->Open("ConfigurationTest.temp",File::OpenModes::READ);
	REQUIRE(file.IsOpen());
	REQUIRE(configuration.ParseConfigurationScript(file));
	CHECK(myInternalValue=="Testing");
	file.Close();

	//Clean up
	CHECK(fileSystem->DeleteFile("ConfigurationTest.temp"));
	
	//Test bool Get overload
	Configuration boolTest;
	//We don't need to check for "false" since we're not checking stringstream's conversion
	// just that the correct overload is selected.
	boolTest.Add("thing","true");
	boolTest.Add("thing2","1");
	CHECK(boolTest.Get("thing",false));
	CHECK(boolTest.Get<bool>("thing",false));
	CHECK(boolTest.Get("thing2",false));
	CHECK(boolTest.Get<bool>("thing2",false));
	
	//Test the multiple configuration file loading
	Configuration configuration2(fileSystem, binder);

	//Test include option (this file has include lines).
	REQUIRE(configuration2.LoadFile("data/MultipleConfigurationFilesTest1.config"));
	
	u32 fileNumber=0;
	CHECK(configuration2.Get<u32>("from.file.one",fileNumber,0));
	CHECK(configuration2.Get<u32>("from.file.two",fileNumber,0));
	CHECK(configuration2.Get<u32>("from.file.three",fileNumber,0));
	
	// Section tests
	Configuration sectionTest(fileSystem);
	REQUIRE(sectionTest.LoadFile("data/SectionTest.config"));
	
	// Base section
	CheckSectionValue(sectionTest,"","something","In Base Section");
	CheckSectionValue(sectionTest,"","something.else","Also in Base Section");
	CheckSectionValue(sectionTest,"","something.in.base","Ya this is in base");		// Should be set from resetting the current section for load
	CheckSectionValueT<int>(sectionTest,"","anInt",10);
	CheckSectionValueT<bool>(sectionTest,"","aTrueBool",true);
	CheckSectionValueT<bool>(sectionTest,"","aFalseBool",false);
	CheckSectionValueT<bool>(sectionTest,"","aTrueBoolInteger",1);
	CheckSectionValueT<bool>(sectionTest,"","aFalseBoolInteger",0);
	
	// Section A
	CheckSectionValue(sectionTest,"Section A","something","In Section A");
	CheckSectionValue(sectionTest,"Section A","something.else","Also in Section A");
	CheckSectionValue(sectionTest,"Section A","something.not.in.base","Hello");
	CheckSectionValueT<int>(sectionTest,"Section A","anInt",20);

	// Section B
	CheckSectionValue(sectionTest,"Section B","something","In Section B");
	CheckSectionValue(sectionTest,"Section B","something.else","Also in Section B");
	CheckSectionValue(sectionTest,"Section B","that.is.enough","The spaces are not required");
	CheckSectionValueT<int>(sectionTest,"Section B","anInt",30);

	// Section C
	CheckSectionValue(sectionTest,"Section C","something","In Section C");
	CheckSectionValue(sectionTest,"Section C","something.else","I have a sub section named \"Section D\"");
	CheckSectionValueT<int>(sectionTest,"Section C","anInt",40);

	// These values should be in the base of the loaded file - so made available in Section C from the parent
	CheckSectionValue(sectionTest,"Section C","something.in.sub.base","In sub base Section");
	CheckSectionValue(sectionTest,"Section C","something.else.in.sub.base","Also in sub base Section");
	CheckSectionValue(sectionTest,"Section C","something.in.base","Ya this is in sub base");
	

	// Section D - should not exist from the root
	Configuration* sectionD = sectionTest.GetSection("Section D");
	CHECK_MESSAGE(sectionD==nullptr,"Section D was found in the root. This should only be available from Section C.");

	sectionD = sectionTest.GetSection("Section C.Section D");
	CHECK_MESSAGE(sectionD!=nullptr,"Section D was not found in Section C");
	
	// Check access using the path.
	CheckSectionValue(sectionTest,"Section C.Section A","something","This Should be different to the parent A");
	CheckSectionValue(sectionTest,"Section C.Section A","something.not.in.base","Hello");
	CheckSectionValueT<int>(sectionTest,"Section C.Section A","anInt",50);
	CheckSectionValue(sectionTest,"Section C.Section D","something","In Section D");
	CheckSectionValue(sectionTest,"Section C.Section D","something.else","Also in Section D");
	CheckSectionValue(sectionTest,"Section C.Section D","hello","world");
	CheckSectionValueT<int>(sectionTest,"Section C.Section D","anInt",60);
	CheckSectionValueT<bool>(sectionTest,"Section C.Section D","aTrueBool",true);
	CheckSectionValueT<bool>(sectionTest,"Section C.Section D","aFalseBool",false);
	CheckSectionValueT<bool>(sectionTest,"Section C.Section D","aTrueBoolInteger",1);
	CheckSectionValueT<bool>(sectionTest,"Section C.Section D","aFalseBoolInteger",0);
	
	// Check that the non option lines are found in the correct sections.
	// These checks depend on each of these lines be the first non-option and non-blank line in the section.
	CheckSectionValue(sectionTest,"","","NotAnOptionLineBase");
	CheckSectionValue(sectionTest,"Section A","","NotAnOptionLineSectionA");
	CheckSectionValue(sectionTest,"Section B","","NotAnOptionLineSectionB");
	CheckSectionValue(sectionTest,"Section C","","NotAnOptionLineSectionC");
	
	
	// Parse test
	// Replacement (last stage)
	// x = 10
	// y = 2
	// z = $y * x
	Configuration parseTest;
	parseTest.Add("x","10");
	parseTest.Add("y","2");
	parseTest.Add("z","$y * x");
	u32 z = parseTest.Get<u32>("z",0);
	CHECK_MESSAGE(z==20,"z=$y * x should be 20");
	std::string zStr = parseTest.Get<std::string>("z","0");
	CHECK_MESSAGE(zStr=="20","z=y * x should be 20");

	Configuration parseTest2;
	parseTest2.Add("x","10");
	parseTest2.Add("y","$x");
	parseTest2.Add("z","$y * 2");
	z = parseTest.Get<u32>("z",0);
	CHECK_MESSAGE(z==20,"z=$y * 2 should be 20");
	
	Configuration boolReplaceTest;
	boolReplaceTest.Add("x","true");
	boolReplaceTest.Add("y","$x");
	bool boolReplaceResult = boolReplaceTest.Get<bool>("y",false);
	CHECK_MESSAGE(boolReplaceResult,"y=$x should result in true, got false");

	//Test getting values in a list as parsed
	Configuration listTest;
	listTest.Add("thing","10");
	listTest.Add("thing","20");
	listTest.Add("thing","30");
	std::vector<int> allValues=listTest.GetAll<int>("thing");
	REQUIRE(allValues.size()==3);

	CHECK(allValues[0]==10);
	CHECK(allValues[1]==20);
	CHECK(allValues[2]==30);

	listTest.Add("a","5");
	listTest.Add("b","4");
	listTest.Add("c","5*4");
	listTest.Add("c","12");
	listTest.Add("c","34");
	listTest.Add("1_NUMBERS","abc");
	listTest.Add("88","crazy");
	listTest.Add("NUMBERS_2","987");
	allValues=listTest.GetAll<int>("c");
	REQUIRE(allValues.size()==3);

	CHECK(allValues[0]==20);
	CHECK(allValues[1]==12);
	CHECK(allValues[2]==34);

	// Double underscore disables the calculator.
	listTest.Add("d","$a times $b is $__c");
	listTest.Add("d","This is just normal");
	listTest.Add("d","This $1_NUMBERS is a number (variable)");
	listTest.Add("d","This $88 is 88");
	listTest.Add("d","This $NUMBERS_2 is a number");
	std::vector<std::string> allStringValues=listTest.GetAll<std::string>("d");
	REQUIRE(allStringValues.size()==5);

	// Note that because we're doing a string substitution here the calculator isn't run.
	CHECK(allStringValues[0]=="5 times 4 is 5*4");
	CHECK(allStringValues[1]=="This is just normal");
	CHECK(allStringValues[2]=="This abc is a number (variable)");
	CHECK(allStringValues[3]=="This crazy is 88");
	CHECK(allStringValues[4]=="This 987 is a number");

	// Internal function tests
	Configuration internalFunctionTests;
	std::vector<std::string> lines = {
		"a=10",
		"a=20",
		"a=30",
		"RemoveAtIndex(a,1)",
		"b=1",
		"b=2",
		"b=3",
		"Remove(b)",
		"AddToSection(NewSection,z,999)",
		"AddToSection(AnotherNewSection,s,77)",
		"AddToSection(AnotherNewSection,s,78)",
		"AddToSection(AnotherNewSection,s,79)",
		"AddToSection(AnotherNewSection,t,88)",
		"RemoveFromSection(AnotherNewSection,t)",
		"RemoveFromSectionAtIndex(AnotherNewSection,s,1)",
		"[SectionA]",
		"c=10",
		"c=20",
		"c=30",
		"RemoveAtIndex(c,1)",
		"d=10",
		"d=20",
		"d=30",
		"Remove(d)",
		};
	internalFunctionTests.ParseLines(lines);
	CHECK(internalFunctionTests.GetAtIndex<u32>("a",0,0)==10);
	CHECK(internalFunctionTests.GetAtIndex<u32>("a",0,1)==30);
	CHECK(internalFunctionTests.GetAtIndex<u32>("a",0,2)==0);
	CHECK(internalFunctionTests.Get<u32>("b",0)==0);

	CHECK(internalFunctionTests.GetFromSectionAtIndex<u32>("SectionA","c",0,0)==10);
	CHECK(internalFunctionTests.GetFromSectionAtIndex<u32>("SectionA","c",0,1)==30);
	CHECK(internalFunctionTests.GetFromSectionAtIndex<u32>("SectionA","c",0,2)==0);
	CHECK(internalFunctionTests.GetFromSection<u32>("SectionA","d",0)==0);
	CHECK(internalFunctionTests.GetFromSection<u32>("NewSection","z",0)==999);
	CHECK(internalFunctionTests.GetFromSectionAtIndex<u32>("AnotherNewSection","s",0,0)==77);
	CHECK(internalFunctionTests.GetFromSectionAtIndex<u32>("AnotherNewSection","s",0,1)==79);
	CHECK(internalFunctionTests.GetFromSectionAtIndex<u32>("AnotherNewSection","s",0,2)==0);
	CHECK(internalFunctionTests.GetFromSection<u32>("AnotherNewSection","t",0)==0);

}

