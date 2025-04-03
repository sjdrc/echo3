#ifndef _ECHO_GRAPHICS_PICKRESULT_H_
#define _ECHO_GRAPHICS_PICKRESULT_H_

#include <echo/Maths/Ray.h>

namespace Echo
{
	class SceneRenderable;

	struct PickResult
	{
		shared_ptr<SceneRenderable> mSceneRenderable;
		Vector3 mIntersectPosition;
		f32 mDistance;
		Ray mRay;
	};

}
#endif
