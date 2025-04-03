#include <echo/Graphics/RenderTargetNotifier.h>
#include <echo/Graphics/RenderTarget.h>
#include <boost/foreach.hpp>

namespace Echo
{
	RenderTargetNotifier::RenderTargetNotifier()
	{
		
	}
	
	RenderTargetNotifier::~RenderTargetNotifier()
	{
		
	}

	bool RenderTargetNotifier::AddRenderTarget(const std::string& name, shared_ptr<RenderTarget> renderTarget, bool isPrimary)
	{
		NamedRenderTargetMap::iterator it = mRenderTargets.find(name);
		if(it!=mRenderTargets.end())
		{
			ECHO_LOG_ERROR("RenderTarget already associated with name \"" << name << "\"");
			return false;
		}
		if(!mPrimaryRenderTarget || isPrimary)
		{
			mPrimaryRenderTarget = renderTarget;
		}
		mRenderTargets.insert(std::make_pair(name, renderTarget));
		return true;
	}
	
	shared_ptr<RenderTarget> RenderTargetNotifier::GetRenderTarget(const std::string& name)
	{
		NamedRenderTargetMap::iterator it = mRenderTargets.find(name);
		if(it==mRenderTargets.end())
		{
			ECHO_LOG_ERROR("RenderTarget not found where name \"" << name << "\"");
			return shared_ptr<RenderTarget>();
		}
		return it->second;
	}

	void RenderTargetNotifier::RemoveRenderTarget(const std::string& name)
	{
		NamedRenderTargetMap::iterator it = mRenderTargets.find(name);
		if(it==mRenderTargets.end())
		{
			ECHO_LOG_ERROR("RenderTarget not found where name \"" << name << "\"");
			return;
		}
		mRenderTargets.erase(it);
	}
	
	void RenderTargetNotifier::RemoveAllRenderTargets()
	{
		mRenderTargets.clear();
	}

	shared_ptr<RenderTarget> RenderTargetNotifier::GetPrimaryRenderTarget() const
	{
		return mPrimaryRenderTarget;
	}

	void RenderTargetNotifier::GraphicsContextLost()
	{
		BOOST_FOREACH(NamedRenderTargetPair& namedRenderTarget, mRenderTargets)
		{
			if(namedRenderTarget.second)
			{
				namedRenderTarget.second->ContextLost();
			}
		}
	}
	
	void RenderTargetNotifier::GraphicsContextRestored()
	{
		BOOST_FOREACH(NamedRenderTargetPair& namedRenderTarget, mRenderTargets)
		{
			if(namedRenderTarget.second)
			{
				namedRenderTarget.second->ContextLost();
			}
		}
	}
}
