#ifndef _ECHORENDERCONTEXT_H_
#define _ECHORENDERCONTEXT_H_

#include <vector>

namespace Echo
{
	class RenderTarget;
	class Matrix4;
	class Camera;
	class Light;

	/**
	 * RenderContext is an object that holds references to objects pertaining to the current render settings.
	 * 
	 * Previously, Renderable had parameters for each render object that specialisations might be interested in
	 * however this was limited and hard to add new parameters as it would require a bit of a refactor.
	 * 
	 * This class provides a way to extend what is passed to Renderables without refactoring. The objects that
	 * were originally parameters are now members of this object along with others.
	 * 
	 * Although some of the matrices, such as the view*projection, can be calculated by using the matrices from
	 * the camera and some are directly from the camera (view and projection) they are provided precalculated
	 * in order to avoid additional cache checks within the camera or multiplying the view and projection
	 * matrices many times.
	 * 
	 * @note The context in this class does not correspond to a device level context as you may already be
	 * familiar with. Those API specific concepts are abstracted by Echo and when relevant managed at the
	 * Platform implementation level. The "context" here corresponds to object rendering such as in a Scene.
	 * 
	 * @note Nothing should be referenced after the context is used (i.e. do not store references to objects).
	 */
	class RenderContext
	{
	public:
		RenderContext(
			RenderTarget& renderTarget,
			const Matrix4& viewMatrix,
			const Matrix4& projectionMatrix,
			const Matrix4& viewProjectionMatrix,
			const Camera& camera,
			const std::vector< Light* >& lights) :
			mRenderTarget(renderTarget),
			mViewMatrix(viewMatrix),
			mProjectionMatrix(projectionMatrix),
			mViewProjectionMatrix(viewProjectionMatrix),
			mCamera(camera),
			mLights(lights)
		{
			
		}
		RenderTarget& mRenderTarget;
		const Matrix4& mViewMatrix;
		const Matrix4& mProjectionMatrix;
		const Matrix4& mViewProjectionMatrix;
		const Camera& mCamera;
		const std::vector< Light* >& mLights;
	};
}
#endif
