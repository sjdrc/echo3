#ifndef _ECHOSTEREOSCOPICRENDERER_H_
#define _ECHOSTEREOSCOPICRENDERER_H_

#include <echo/Graphics/MultiRenderer.h>
#include <echo/Graphics/Node.h>

namespace Echo
{
	class Camera;
	class Viewport;
	class RenderTarget;
	class Scene;
	
	/**
	 * The StereoscopicRenderer sets up multiple viewports and cameras for stereoscopic rendering.
	 * This class will manage the cameras for stereoscopic rendering with a simple interface.
	 */
	class StereoscopicRenderer : public MultiRenderer
	{
	public:
		/** Constructor.
		 */
		StereoscopicRenderer(shared_ptr<RenderTarget> renderTarget, std::string name, Scene& scene, u32 taskPriority = 5000);

		/** Destructor.
		 */
		virtual ~StereoscopicRenderer();
		
		void SetFocusDistance(f32 distanceToFocus);

		f32 GetFocusDistance();
		
		void SetEyeDistance(f32 distanceBetweenEyes);
		
		f32 GetEyeDistance() const;
		
		void SetFocusSplit(f32 split);

		void SetFOV(Radian fov);
		
		Radian GetFOV() const;
		
		void SetFOVy(Radian fov);
		
		Radian GetFOVy() const;
		
		shared_ptr<Node> GetFocusNode(){return mFocusNode;}
	
		f32 GetFocusSplit() const {return mFocusSplit;}

		Node& GetNode() {return mHeadNode;}
		void SetStereo(bool on);
	private:
		Node mHeadNode;
		f32 mFocusSplit;
		f32 mDistanceBetweenEyes;
		bool mStereoOn;
		shared_ptr<Node> mFocusNode;
		shared_ptr<Node> mFocusNodeLeft;
		shared_ptr<Node> mFocusNodeRight;
		shared_ptr<Camera> mLeftEye;
		shared_ptr<Camera> mRightEye;
		shared_ptr<Renderer> mLeftRenderer;
		shared_ptr<Renderer> mRightRenderer;
	};
}
#endif
