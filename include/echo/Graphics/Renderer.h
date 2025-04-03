#ifndef _ECHORENDERER_H_
#define _ECHORENDERER_H_

#include <echo/Kernel/Task.h>
#include <echo/Graphics/Colour.h>
#include <echo/cpp/functional>
#include <echo/Graphics/PickResult.h>
#include <map>

#include <echo/Graphics/RenderTarget.h>

namespace Echo
{
	class RenderTarget;
	class Viewport;
	class Camera;
	class SceneRenderable;
	
	class Renderer : public Task
	{
	public:
		typedef function< void(const Renderer&, RenderTarget&, const Camera&, const Viewport&) > NotificationCallback;
		
		Renderer(shared_ptr<RenderTarget> renderTarget, shared_ptr<Viewport> viewport, shared_ptr<Camera> camera,
				std::string taskName = "", u32 priority = 5000);
		virtual ~Renderer();
		
		/**
		 * Get a Ray into the Scene from a renderTarget X and Y position.
		 * @note Requires a valid render target.
		 * @note Requires a valid camera.
		 * @note Requires a valid viewport.
		 * @param renderTargetXPercent x coordinate of the target range 0-1 where 0 is left and 1 is right.
		 * @param renderTargetYPercent y coordinate of the target range 0-1 where 0 is top and 1 is bottom.
		 * @return A ray if all of the required objects are valid and the point is inside the viewport.
		 */
		optional<Ray> GetRayIntoScene(f32 renderTargetXPercent, f32 renderTargetYPercent);

		/**
		 * Pick an object from the Camera's scene using coordinates from within the viewport.
		 * @note Requires a valid render target.
		 * @note Requires a valid camera.
		 * @note Requires a valid viewport.
		 * @note See Scene::Pick() for more information on how a pick is performed.
		 * @param renderTargetXPercent x coordinate of the target range 0-1 where 0 is left and 1 is right.
		 * @param renderTargetYPercent y coordinate of the target range 0-1 where 0 is top and 1 is bottom.
		 * @return A PickResult object if there was an intersect, otherwise nothing.
		 */
		optional<PickResult> Pick(f32 renderTargetXPercent, f32 renderTargetYPercent);
		
		void SetRenderTarget(shared_ptr<RenderTarget> renderTarget) {mRenderTarget=renderTarget;}
		
		void Update(Seconds);

		bool GetSwapBuffers() const {return mSwapBuffers;}
		void SetSwapBuffers(bool swapBuffers) {mSwapBuffers = swapBuffers;}

		bool GetClear() const { return mClear; }
		void SetClear(bool val) { mClear = val; }

		bool GetScissor() const { return mScissor; }
		void SetScissor(bool val) { mScissor = val; }

		bool GetScissorReset() const { return mScissorReset; }
		void SetScissorReset(bool val) { mScissorReset = val; }
		
		/**
		 * Set the clear mask.
		 * @see RenderTarget::ClearMask for more information.
		 * @param clearMask
		 */
		void SetClearMask(RenderTarget::ClearMask clearMask) {mClearMask = clearMask;}
		RenderTarget::ClearMask GetClearMask() const {return mClearMask;}
		
		f32 GetClearDepth() const {return mClearDepth; }
		void SetClearDepth(f32 val) { mClearDepth = val; }
		
		void SetScissorBox(const Viewport::Rectangle& box)
		{
			mScissorBox = box;
		}

		const Viewport::Rectangle& GetScissorBox() const
		{
			return mScissorBox;
		}

		const Colour& GetClearColour() const {return mClearColour;}
		void SetClearColour(const Colour& colour) {mClearColour = colour;}
		
		void SetProcessCallbacks(bool val) {mProcessCallbacks = val;}
		bool GetProcessCallbacks() const {return mProcessCallbacks;}
		
		void SetBeginNotificationCallback(const std::string& identifier, NotificationCallback callback);
		void SetEndNotificationCallback(const std::string& identifier, NotificationCallback callback);
		void RemoveBeginNotificationCallback(const std::string& identifier);
		void RemoveEndNotificationCallback(const std::string& identifier);
		void RemoveAllBeginCallbacks()
		{
			mBeginRenderCallbacks.clear();
		}
		void RemoveAllEndCallbacks()
		{
			mEndRenderCallbacks.clear();
		}
		void RemoveAllCallbacks()
		{
			RemoveAllBeginCallbacks();
			RemoveAllEndCallbacks();
		}
		shared_ptr<Viewport> GetViewport() const {return mViewport;}
		shared_ptr<Camera> GetCamera() const {return mCamera;}
		shared_ptr<RenderTarget> GetRenderTarget() const {return mRenderTarget;}
		void SetViewport(shared_ptr<Viewport> viewport) {mViewport=viewport;}
		void SetCamera(shared_ptr<Camera> camera) {mCamera=camera;}
	private:
		shared_ptr<RenderTarget> mRenderTarget;	//!< The render target to render to.
		shared_ptr<Viewport> mViewport;			//!< The viewport to render through.
		shared_ptr<Camera> mCamera;				//!< The camera to render.
		Colour mClearColour;			//!< The screen clear colour.
		bool mSwapBuffers;				//!< Whether to swap the screen buffers after rendering.
		bool mClear;					//!< Whether the screen should be cleared before rendering.
		bool mScissor;					//!< Whether the scissor box should be used.
		bool mScissorReset;				//!< Whether the scissor should be reset if it is not used (used by MultiRenderer
										//   to prevent the scissor from resetting).
		bool mProcessCallbacks;			//!< Whether to process the begin and end render callbacks, false by default.
		Viewport::Rectangle mScissorBox;//!< The scissor box.
		RenderTarget::ClearMask mClearMask;
		f32 mClearDepth;				//!< Clear depth, valid range 0.0-1.0, see RenderTarget::SetClearDepth().
		typedef std::pair<const std::string, NotificationCallback> CallbackPair;
		std::map<std::string, NotificationCallback> mBeginRenderCallbacks;
		std::map<std::string, NotificationCallback> mEndRenderCallbacks;
	};
}
#endif 
