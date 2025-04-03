/* 
 * File:   CameraDollies.h
 * Author: minty
 *
 * Created on 31 May 2015, 1:21 PM
 */

#ifndef ECHO_CAMERADOLLIES_H
#define ECHO_CAMERADOLLIES_H

#include <echo/Kernel/Task.h>
#include <echo/Maths/AxisAlignedBox.h>
#include <vector>

namespace Echo
{
	class Camera;
	class SceneRenderable;
	class NodeInterface;
	
	/**
	 * This class is a "dolly" to allow a camera to be automatically positioned
	 * perpendicularly to track various items by always keeping them in view.
	 * @see  https://en.wikipedia.org/wiki/Camera_dolly
	 * @note At the moment, the camera's direction will always remain parallel to
	 *       the Z axis and will face towards negative Z.
	 * @todo Add a constructor parameter describing the axis or normal vector for
	 *       the camera direction to remain parallel to.
	 */
	class PerpendicularCameraDolly : public Task
	{
	public:
		/**
		 * The constructor for the camera dolly.
		 * @param camera        The camera to be controlled by this dolly.
		 * @param visualPadding An amount of extra visual padding to fit in the
		 *                      camera view, so the tracked items don't fill 100%
		 *                      of the view.
		 * @param cameraBounds  The maximum bounds by which this camera's view will
		 *                      be constrained. Please note that if these bounds
		 *                      don't have the same aspect ratio as the camera, it
		 *                      is still possible for the camera to see outside of
		 *                      these bounds when zoomed out to fully fit these
		 *                      bounds, in order to fit all the tracked objects in
		 *                      the camera's view.
		 */
		PerpendicularCameraDolly(shared_ptr<Camera> camera,
		                         f32 visualPadding = 0.f,
		                         const AxisAlignedBox& cameraBounds = AxisAlignedBox::BOX_INFINITE);
		
		/**
		 * The items for the controlled camera to track. The camera dolly will
		 * ensure these items automatically remain within view of the camera.
		 * These are stored as weak_ptrs so the camera will stop tracking them
		 * if/when they are deleted.
		 */
		void TrackItem(weak_ptr<NodeInterface> item);
		void UntrackItem(weak_ptr<NodeInterface> item);
		
		/**
		 * Updates the physical position and look-at position for the camera.
		 */
		void Update(Seconds lastFrameTime) override;
		
	private:
		std::vector<weak_ptr<SceneRenderable> > mRenderablesToTrack;
		std::vector<weak_ptr<NodeInterface> > mNodesToTrack;
		shared_ptr<Camera> mCamera;
		f32 mVisualPadding;
		const AxisAlignedBox& mMaximumBoundsForCameraView;
	};
}

#endif
