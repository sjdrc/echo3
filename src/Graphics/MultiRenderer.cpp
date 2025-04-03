#include <echo/Graphics/MultiRenderer.h>
#include <echo/Graphics/Renderer.h>
#include <echo/Graphics/RenderTarget.h>
#include <boost/foreach.hpp>

namespace Echo
{
	MultiRenderer::MultiRenderer(std::string taskName, u32 priority) :
		TaskGroup(taskName,priority),
		mClearColour(Colours::BLACK),
		mSwapBuffers(true),
		mClear(true),
		mScissor(false),
		mProcessCallbacks(false),
		mScissorBox(Viewport::Rectangle()),
		mClearMask(RenderTarget::ClearMaskFlags::DEPTH | RenderTarget::ClearMaskFlags::COLOUR),
		mClearDepth(1.f)
	{
	}
	
	MultiRenderer::~MultiRenderer()
	{

	}

	shared_ptr<Renderer> MultiRenderer::CreateRenderer(shared_ptr<Viewport> viewport, shared_ptr<Camera> camera, std::string name, u32 priority)
	{
		shared_ptr<Renderer> renderer = shared_ptr<Renderer>(new Renderer(shared_ptr<RenderTarget>(), viewport, camera, name, priority));
		renderer->SetClear(false);
		renderer->SetSwapBuffers(false);
		AddTask(*renderer);
		mRenderers.push_back(renderer);
		return renderer;
	}

	shared_ptr<Renderer> MultiRenderer::CreateRenderer(std::string name, u32 priority)
	{
		shared_ptr<Renderer> renderer = shared_ptr<Renderer>(new Renderer(shared_ptr<RenderTarget>(), shared_ptr<Viewport>(), shared_ptr<Camera>(), name, priority));
		renderer->SetClear(false);
		renderer->SetSwapBuffers(false);
		AddTask(*renderer);
		mRenderers.push_back(renderer);
		return renderer;
	}
	
	void MultiRenderer::ClearRenderers()
	{
		while(!mRenderers.empty())
		{
			RemoveRenderer(mRenderers.front());
		}
	}
	
	void MultiRenderer::AddRenderer(shared_ptr<Renderer> renderer)
	{
		if(AddTask(renderer))
		{
			mRenderers.push_back(renderer);
		}
	}

	void MultiRenderer::RemoveRenderer(shared_ptr<Renderer> renderer)
	{
		RemoveTask(renderer);
		mRenderers.remove(renderer);
	}

	void MultiRenderer::SetScissor(bool val)
	{
		mScissor = val;
		BOOST_FOREACH(shared_ptr<Renderer>& renderer, mRenderers)
		{
			renderer->SetScissor(false);
			// If scissor is on then don't reset the scissor box in each renderer and vice versa.
			// This saves the programmer the task of resetting this flag.
			renderer->SetScissorReset(!mScissor);
		}
	}
	
	void MultiRenderer::AddRenderTarget(shared_ptr<RenderTarget> renderTarget)
	{
		mRenderTargets.push_back(renderTarget);
	}
	
	void MultiRenderer::RemoveRenderTarget(shared_ptr<RenderTarget> renderTarget)
	{
		mRenderTargets.remove(renderTarget);
	}
	
	void MultiRenderer::Update(Seconds lastFrameTime)
	{
		BOOST_FOREACH(shared_ptr<RenderTarget>& renderTarget, mRenderTargets)
		{
			if(!renderTarget->Activate())
			{
				return;
			}
			
			BOOST_FOREACH(shared_ptr<Renderer>& renderer, mRenderers)
			{
				renderer->SetRenderTarget(renderTarget);
			}
			

			if(mProcessCallbacks)
			{
				BOOST_FOREACH(CallbackPair& callback, mBeginRenderCallbacks)
				{
					callback.second(*this,*renderTarget);
				}
			}

			if(mClear)
			{
				renderTarget->SetClearColour(mClearColour);
				renderTarget->SetClearDepth(mClearDepth);
				renderTarget->SetClearMask(mClearMask);
				renderTarget->Clear();
			}
			
			if(mScissor)
			{
				renderTarget->SetScissor(mScissorBox);
			}else
			{
				renderTarget->ResetScissor();
			}
			
			TaskGroup::UpdateTasks(lastFrameTime);
			if(mSwapBuffers)
			{
				renderTarget->SwapBuffers();
			}
			renderTarget->Deactivate();

			if(mProcessCallbacks)
			{
				BOOST_FOREACH(CallbackPair& callback, mEndRenderCallbacks)
				{
					callback.second(*this,*renderTarget);
				}
			}
		}
	}

	void MultiRenderer::SetBeginNotificationCallback(const std::string& identifier, NotificationCallback callback)
	{
		mBeginRenderCallbacks[identifier] = callback;
	}
	
	void MultiRenderer::SetEndNotificationCallback(const std::string& identifier, NotificationCallback callback)
	{
		mEndRenderCallbacks[identifier] = callback;
	}
	
	void MultiRenderer::RemoveBeginNotificationCallback(const std::string& identifier)
	{
		mBeginRenderCallbacks.erase(identifier);
	}
	
	void MultiRenderer::RemoveEndNotificationCallback(const std::string& identifier)
	{
		mEndRenderCallbacks.erase(identifier);
	}
}
