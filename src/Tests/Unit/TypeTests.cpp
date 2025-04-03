#include <echo/Types.h>
#include <doctest/doctest.h>

/**
 * Author: sean
 * Test suite to ensure that basic types are the correct size for a platform.
 * Incorrect sizes may result in unexpected behaviour.
 */

#define CHECKSIZE(a,b) CHECK(sizeof(a)==b)

TEST_CASE("Types")
{
	using namespace Echo;
	CHECKSIZE(u8,1);
	CHECKSIZE(u16,2);
	CHECKSIZE(u32,4);
	CHECKSIZE(u64,8);
	CHECKSIZE(s8,1);
	CHECKSIZE(s16,2);
	CHECKSIZE(s32,4);
	CHECKSIZE(s64,8);
	CHECKSIZE(f32,4);
	CHECKSIZE(f64,8);
}

