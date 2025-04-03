#ifndef _ECHORENDERABLE_H_
#define _ECHORENDERABLE_H_

#include <echo/Graphics/Colour.h>
#include <echo/Graphics/RenderContext.h>

namespace Echo
{
	class Matrix4;
	class RenderTarget;

	/**
	 * Defines the interface to a Renderable object.
	 * A Renderable requires that the RenderTarget be set up with the appropriate perspective matrix.
	 */
	class Renderable
	{
	public:
		Renderable();
		virtual ~Renderable();

		/**
		 * Render the object.
		 * The implementation should, at some point, call renderTarget.SetModelViewMatrix() to set the
		 * matrix in the render system that will be applied to any vertices rendered.
		 * 
		 * Rendering usually consists of setting the model-view matrix, setting the render system
		 * parameters to modify how things will be rendered, then a call to DrawTriangles() or other
		 * Draw method.
		 * 
		 * There are higher level classes that take care of rendering for you so you can focus on what
		 * and where to render things. For example you can use a SceneEntity and give it a Mesh to
		 * render.
		 * 
		 * @param renderContext a reference to an object that holds references to rendering relevant
		 * objects such as the RenderTarget, Camera, and matrices.
		 * render target via renderTarget.SetModelViewMatrix(). Usually the matrix passed in is a view
		 * matrix from a camera which can be combined with the object's local transform (if it has one)
		 * before being used.
		 * @param renderTarget The render target that will be responsible for performing render operations.
		 * @param compoundDiffuse The caller can provide a colour that they want the object to use for
		 * rendering. The actual behaviour is up to the implementation. If you're not sure what to use
		 * pass Colour::WHITE.
		 */
		virtual void Render(RenderContext& renderContext, Colour compoundDiffuse) = 0;
	};
}
#endif 
