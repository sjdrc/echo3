#include <echo/Maths/EchoMaths.h>
#include <echo/Maths/Vector2.h>
#include <echo/Maths/Vector3.h>
#include <echo/Maths/Vector4.h>
#include <echo/Maths/Ray.h>
#include <echo/Maths/Sphere.h>
#include <echo/Maths/AxisAlignedBox.h>
#include <echo/Maths/Plane.h>
#include <list>
#include <echo/Maths/easm_math.h>

namespace Echo
{
namespace Maths
{
	int mTrigTableSize = 0;
	f32 mTrigTableFactor = 0;
	f32 *mSinTable = nullptr;
	f32 *mTanTable = nullptr;

	AutoInitialiser AUTO_INITIALISER = AutoInitialiser();

	AutoInitialiser::AutoInitialiser( unsigned int trigTableSize )
	{
		mTrigTableSize = trigTableSize;
		mTrigTableFactor = mTrigTableSize / TWO_PI;

		mSinTable = new f32[mTrigTableSize];
		mTanTable = new f32[mTrigTableSize];

		buildTrigTables();
	}

	AutoInitialiser::~AutoInitialiser()
	{
		delete [] mSinTable;
		delete [] mTanTable;
	}

	void buildTrigTables(void)
	{
		// Build trig lookup tables
		// Could get away with building only PI sized Sin table but simpler this 
		// way. Who cares, it'll ony use an extra 8k of memory anyway and I like 
		// simplicity.
		f32 angle;
		for (int i = 0; i < mTrigTableSize; ++i)
		{
			angle = TWO_PI * i / mTrigTableSize;
			mSinTable[i] = sin(angle);
			mTanTable[i] = tan(angle);
		}
	}

	f32 SinTable (f32 fValue)
	{
		// Convert range to index values, wrap if required
		int idx;
		if (fValue >= 0)
		{
			idx = int(fValue * mTrigTableFactor) % mTrigTableSize;
		}
		else
		{
			idx = mTrigTableSize - (int(-fValue * mTrigTableFactor) % mTrigTableSize) - 1;
		}

		return mSinTable[idx];
	}

	f32 TanTable (f32 fValue)
	{
		// Convert range to index values, wrap if required
		int idx = int(fValue *= mTrigTableFactor) % mTrigTableSize;
		return mTanTable[idx];
	}
}
}