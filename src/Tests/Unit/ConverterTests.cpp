#include <echo/Maths/Vector2.h>
#include <echo/Maths/Vector3.h>
#include <echo/Maths/Matrix3.h>
#include <echo/Maths/Matrix4.h>
#include <echo/Maths/Vector4.h>
#include <echo/Maths/Quaternion.h>
#include <echo/Graphics/Colour.h>
#include <echo/Maths/Length.h>
#include <echo/Chrono/Chrono.h>
#include <doctest/doctest.h>

using namespace Echo;

template <typename T>
void ConvertTest(const T& value, const std::string& asString)
{
	std::stringstream ss(asString);
	T v;
	ss >> v;

	std::stringstream message;
	message << "Value " <<  v << " does not match" << typeid(value).name() << " with " << asString << " - " << value;
	CHECK_MESSAGE(v==value,message.str());
}

TEST_CASE("Converter")
{
	// Turn off log output, we will just use output from this test.
	gDefaultLogger.SetLogMask(Echo::Logger::LogLevels::NONE);

	ConvertTest<Vector2>(Vector2(1,2),"Vector2(1,2)");
	ConvertTest<Vector2>(Vector2(3,4),"3,4");
	ConvertTest<Vector3>(Vector3(1,2,3),"Vector3(1,2,3)");
	ConvertTest<Vector3>(Vector3(3,4,5),"3,4,5");
	ConvertTest<Vector4>(Vector4(1,2,3,4),"Vector4(1,2,3,4)");
	ConvertTest<Vector4>(Vector4(3,4,5,6),"3,4,5,6");
	ConvertTest<Matrix3>(Matrix3(1,2,3,4,5,6,7,8,9),"Matrix3(1,2,3,4,5,6,7,8,9)");
	ConvertTest<Matrix3>(Matrix3(3,4,5,6,7,8,9,10,11),"3,4,5,6,7,8,9,10,11");
	ConvertTest<Matrix4>(Matrix4(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16),"Matrix4(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16)");
	ConvertTest<Matrix4>(Matrix4(3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18),"3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18");
	ConvertTest<Quaternion>(Quaternion(1,2,3,4),"Quaternion(1,2,3,4)");
	ConvertTest<Quaternion>(Quaternion(3,4,5,6),"3,4,5,6");
	ConvertTest<Colour>(Colour(1,1,1,1),"Colour(1,1,1,1)");
	ConvertTest<Colour>(Colour(0.5,0.5,0.5,0.5),"0.5,0.5,0.5,0.5");
	ConvertTest<Colour>(Colour(1,0,0,128.f/255.f),"255,0,0,128");
	ConvertTest<Colour>(Colour(0,1,0,128.f/255.f),"h00FF0080");
	ConvertTest<Colour>(Colour(1,1,1,1),"hFFFFFFFF");
	ConvertTest<Colour>(Colour(1.f/255.f,1.f/255.f,1.f/255.f,1.f/255.f),"Colour8(1,1,1,1)");	//Make sure 8bit mode for 1,1,1,1 works

	//Length Tests
	ConvertTest<Kilometre>(Kilometre(4),"4km");
	ConvertTest<Metre>(Metre(1.2),"1.2m");
	ConvertTest<Centimetre>(Centimetre(50),"50cm");
	ConvertTest<Millimetre>(Millimetre(750),"750mm");
	ConvertTest<Micrometre>(Micrometre(60),"60um");
	ConvertTest<Nanometre>(Nanometre(1248),"1248nm");
	ConvertTest<Inch>(Inch(12),"12in");
	ConvertTest<Foot>(Foot(3),"3ft");

	//Chrono Tests
	ConvertTest<Seconds>(Seconds(4),"Seconds(4)");
	ConvertTest<Milliseconds>(Milliseconds(1.2),"Milliseconds(1.2)");
	ConvertTest<Microseconds>(Microseconds(50),"Microseconds(50)");
	ConvertTest<Nanoseconds>(Nanoseconds(750),"Nanoseconds(750)");
	ConvertTest<Minutes>(Minutes(60),"Minutes(60)");
	ConvertTest<Hours>(Hours(1248),"Hours(1248)");
	
	// Make sure implicit chrono works
	ConvertTest<Seconds>(Seconds(4),"Seconds(4)");
	ConvertTest<Seconds>(Seconds(Milliseconds(1.2)),"Milliseconds(1.2)");
	ConvertTest<Seconds>(Seconds(Microseconds(50)),"Microseconds(50)");
	ConvertTest<Seconds>(Seconds(Nanoseconds(750)),"Nanoseconds(750)");
	ConvertTest<Seconds>(Seconds(Minutes(60)),"Minutes(60)");
	ConvertTest<Seconds>(Seconds(Hours(1248)),"Hours(1248)");
}
