#include <echo/Graphics/Renderer.h>
#include <echo/Graphics/RenderTarget.h>
#include <echo/Graphics/Camera.h>
#include <echo/Graphics/Viewport.h>
#include <echo/Graphics/Scene.h>
#include <echo/Graphics/PickResult.h>
#include <boost/foreach.hpp>

namespace Echo
{
	Renderer::Renderer(shared_ptr<RenderTarget> renderTarget, shared_ptr<Viewport> viewport, shared_ptr<Camera> camera,
			std::string taskName, u32 priority) :
		Task(taskName,priority),
		mRenderTarget(renderTarget),
		mViewport(viewport),
		mCamera(camera),
		mClearColour(Colours::BLACK),
		mSwapBuffers(true),
		mClear(true),
		mScissor(false),
		mScissorReset(true),
		mProcessCallbacks(false),
		mScissorBox(0,0,1,1),
		mClearMask(RenderTarget::ClearMaskFlags::DEPTH | RenderTarget::ClearMaskFlags::COLOUR),
		mClearDepth(1.f)
	{
	}
	
	Renderer::~Renderer()
	{

	}

	optional<Ray> Renderer::GetRayIntoScene(f32 renderTargetXPercent, f32 renderTargetYPercent)
	{
		if (!mCamera || !mRenderTarget || !mViewport)
		{
			return none;
		}
		const f32 xScreen = renderTargetXPercent;
		const f32 yScreen = renderTargetYPercent;
		Viewport::Rectangle parameters = mViewport->GetRectangle(mRenderTarget->GetAspectRatio());
		
		if (xScreen < parameters.mLeft || xScreen > parameters.mRight ||
			yScreen < parameters.mTop || yScreen > parameters.mBottom)
		{
			return none;
		}
		// The coordinates need to be transformed into viewport space (0 - 1):
		const f32 viewportWidth = parameters.GetWidth();
		const f32 viewportHeight = parameters.GetHeight();
		if (viewportWidth <= 0 || viewportHeight <= 0)
		{
			return none;
		}
		const f32 xViewport = (xScreen - parameters.mLeft) / viewportWidth;
		const f32 yViewport = (yScreen - parameters.mTop) / viewportHeight;

		return mCamera->GetCameraToViewportRay(xViewport, yViewport);
	}
	
	optional<PickResult> Renderer::Pick(f32 renderTargetXPercent, f32 renderTargetYPercent)
	{
		optional<Ray> rayIntoScene = GetRayIntoScene(renderTargetXPercent,renderTargetYPercent);
		if(!rayIntoScene)
		{
			return none;
		}
		return mCamera->GetScene().Pick(*mCamera, rayIntoScene.value());
	}

	void Renderer::Update(Seconds)
	{
		if(!mRenderTarget || !mViewport || !mCamera)
		{
			return;
		}
		
		if(!mRenderTarget->Activate())
		{
			return;
		}
		
		if(mProcessCallbacks)
		{
			BOOST_FOREACH(CallbackPair& callback, mBeginRenderCallbacks)
			{
				callback.second(*this,*mRenderTarget,*mCamera,*mViewport);
			}
		}
		
		mRenderTarget->SetViewport(*mViewport);
		if(mScissor)
		{
			mRenderTarget->SetScissor(mScissorBox);
		}else
		{
			if(mScissorReset)
			{
				mRenderTarget->ResetScissor();
			}
		}
		
		mCamera->UpdateAspectForViewport(*mViewport,*mRenderTarget);
		
		if(mClear)
		{
			mRenderTarget->SetClearColour(mClearColour);
			mRenderTarget->SetClearDepth(mClearDepth);
			mRenderTarget->SetClearMask(mClearMask);
			mRenderTarget->Clear();
		}
		mCamera->RenderScene(*mRenderTarget);
		if(mSwapBuffers)
		{
			mRenderTarget->SwapBuffers();
		}
		mRenderTarget->Deactivate();

		if(mProcessCallbacks)
		{
			BOOST_FOREACH(CallbackPair& callback, mEndRenderCallbacks)
			{
				callback.second(*this,*mRenderTarget,*mCamera,*mViewport);
			}
		}
	}

	void Renderer::SetBeginNotificationCallback(const std::string& identifier, NotificationCallback callback)
	{
		mBeginRenderCallbacks[identifier] = callback;
	}
	
	void Renderer::SetEndNotificationCallback(const std::string& identifier, NotificationCallback callback)
	{
		mEndRenderCallbacks[identifier] = callback;
	}
	
	void Renderer::RemoveBeginNotificationCallback(const std::string& identifier)
	{
		mBeginRenderCallbacks.erase(identifier);
	}
	
	void Renderer::RemoveEndNotificationCallback(const std::string& identifier)
	{
		mEndRenderCallbacks.erase(identifier);
	}
}
