#ifndef _ECHOGRENDERTARGETNOTIFIER_H_
#define _ECHOGRENDERTARGETNOTIFIER_H_
#include <echo/Types.h>
#include <string>
#include <map>

namespace Echo
{
	class RenderTarget;
	
	/**
	 * Class to notify render targets of rendering context events.
	 * This class is a general class that allows multiple render targets to be
	 * notified of lost context without managing a list of render targets. It
	 * allows systems to be more distant to the RenderTarget class which has other
	 * dependencies.
	 */
	class RenderTargetNotifier
	{
	public:
		RenderTargetNotifier();
		~RenderTargetNotifier();

		//!\brief Add a render target.
		//!\param name A unique name for the render target. This name can be used later to
		//!			   get a reference to the render target.
		//!\param renderTarget A pointer to the render target.
		//!\param isPrimary set to true if this is the primary render target. The primary render target is usually
		bool AddRenderTarget(const std::string& name, shared_ptr<RenderTarget> renderTarget, bool isPrimary = false);
		shared_ptr<RenderTarget> GetRenderTarget(const std::string& name);

		void RemoveRenderTarget(const std::string& name);
		void RemoveAllRenderTargets();

		shared_ptr<RenderTarget> GetPrimaryRenderTarget() const;
		
		void GraphicsContextLost();
		void GraphicsContextRestored();
	protected:
		shared_ptr<RenderTarget> mPrimaryRenderTarget;
		typedef std::pair< const std::string, shared_ptr<RenderTarget> > NamedRenderTargetPair;
		typedef std::map < std::string, shared_ptr<RenderTarget> > NamedRenderTargetMap;
		NamedRenderTargetMap mRenderTargets;
	};
}
#endif
