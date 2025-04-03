#ifndef _ECHOMULTIPASSRENDERABLE_H_
#define _ECHOMULTIPASSRENDERABLE_H_

#include <echo/Graphics/Colour.h>

namespace Echo
{
	class RenderPass;
	class RenderContext;

	class MultipassRenderable
	{
	public:
		MultipassRenderable();
		virtual ~MultipassRenderable();
		
		virtual void Render(RenderContext& renderContext, const RenderPass& pass, Colour compoundDiffuse) = 0;
	};
}
#endif 
