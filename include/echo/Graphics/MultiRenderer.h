#ifndef _ECHOMULTIRENDERER_H_
#define _ECHOMULTIRENDERER_H_

#include <echo/Kernel/TaskGroup.h>
#include <echo/Graphics/Colour.h>
#include <echo/Graphics/RenderTarget.h>
#include <echo/cpp/functional>
#include <map>

namespace Echo
{
	class Viewport;
	class Camera;
	class Renderer;
	
	/**
	 * A MultiRenderer allows you to render multiple times in a single Update.
	 * A MultiRenderer will manage one or more Renderer objects and initially configure them to render
	 * without clearing or swapping any buffers. This allows you to render from multiple cameras and
	 * viewports in the same update.
	 */
	class MultiRenderer : public TaskGroup
	{
	public:
		typedef function< void(const MultiRenderer&, RenderTarget&) > NotificationCallback;

		MultiRenderer(std::string taskName = "", u32 priority = 5000);
		virtual ~MultiRenderer();
		
		/**
		 * Create a renderer.
		 * @param viewport The viewport to render through.
		 * @param camera The camera to render from.
		 * @param name The name of the renderer.
		 * @param priority The priority of the renderer, see Task for more information about priorities.
		 * @return the new Renderer.
		 */
		shared_ptr<Renderer> CreateRenderer(shared_ptr<Viewport> viewport, shared_ptr<Camera> camera, std::string name = "", u32 priority = 5000);

		/**
		 * Create a renderer without a camera or viewport, you will need to specify them later.
		 * @param name The name of the renderer.
		 * @param priority The priority of the renderer, see Task for more information about priorities.
		 * @return the new Renderer.
		 */
		shared_ptr<Renderer> CreateRenderer(std::string name = "", u32 priority = 5000);
		
		void AddRenderTarget(shared_ptr<RenderTarget> renderTarget);
		void RemoveRenderTarget(shared_ptr<RenderTarget> renderTarget);

		void Update(Seconds);

		bool GetSwapBuffers() const {return mSwapBuffers;}
		void SetSwapBuffers(bool swapBuffers) {mSwapBuffers = swapBuffers;}

		bool GetClear() const { return mClear; }
		void SetClear(bool val) { mClear = val; }	
		
		bool GetScissor() const { return mScissor; }
	
		/**
		 * Turn scissor on.
		 * Turning scissor causes Renderers managed by this MultiRenderer to have settings
		 * changed so their individual scissor settings aren't used. You can still override
		 * the Renderers after calling this method but be aware every call to this method
		 * also modifies the Renderer settings.
		 * @param val true to turn on scissor test.
		 */
		void SetScissor(bool val);
		
		void SetClearMask(RenderTarget::ClearMask clearMask) {mClearMask = clearMask;}
		RenderTarget::ClearMask GetClearMask() const {return mClearMask;}
		
		f32 GetClearDepth() const {return mClearDepth; }
		void SetClearDepth(const f32 val) { mClearDepth = val; }

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
		
		/**
		 * Clear all of the renderers.
		 */
		void ClearRenderers();
		
		/**
		 * Add a renderer to the set.
		 * It is important to ensure that the renderer will be setup properly for the MultiRenderer
		 * to render all renderers in the same frame without. This normally means that the renderer shouldn't
		 * clear the screen or swap the back buffers. It is assumed this will be the case when calling this
		 * method so the renderers settings are not modified. If the renderer comes from the MultiRenderer
		 * originally then it should be fine.
		 * This method will not add the renderer if it has already been added.
		 * @param renderer The renderer to add.
		 */
		void AddRenderer(shared_ptr<Renderer> renderer);
		
		/**
		 * Removes the renderer.
		 * This will stop the renderer from being rendered in further updates.
		 * It is safe to call this method even if the renderer doesn't exist.
		 */
		void RemoveRenderer(shared_ptr<Renderer> renderer);
		
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
	private:
		std::list< shared_ptr<RenderTarget> > mRenderTargets;
		std::list< shared_ptr<Renderer> > mRenderers; //!< The Renderer objects to render.
		Colour mClearColour;			//!< The screen clear colour.
		bool mSwapBuffers;				//!< Whether to swap the screen buffers after rendering.
		bool mClear;					//!< Whether the screen should be cleared before rendering.
		bool mScissor;					//!< Whether the scissor box should be used.
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
