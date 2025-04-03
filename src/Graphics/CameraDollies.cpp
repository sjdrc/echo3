/* 
 * File:   CameraDollies.cpp
 * Author: minty
 * 
 * Created on 31 May 2015, 1:21 PM
 */

#include <echo/Graphics/CameraDollies.h>
#include <echo/Graphics/Camera.h>
#include <echo/Graphics/SceneRenderable.h>
#include <boost/foreach.hpp>

namespace Echo
{
	PerpendicularCameraDolly::PerpendicularCameraDolly(shared_ptr<Camera> camera,
	                                                   f32 visualPadding,
	                                                   const AxisAlignedBox& cameraBounds)
		:
		mRenderablesToTrack(),
		mNodesToTrack(),
		mCamera(camera),
		mVisualPadding(visualPadding),
		mMaximumBoundsForCameraView(cameraBounds)
	{
	}
	
	void PerpendicularCameraDolly::TrackItem(weak_ptr<NodeInterface> item)
	{
		if (shared_ptr<SceneRenderable> renderable = dynamic_pointer_cast<SceneRenderable>(item.lock()))
		{
			mRenderablesToTrack.push_back(renderable);
		}
		else
		{
			mNodesToTrack.push_back(item);
		}
	}
	
	void PerpendicularCameraDolly::UntrackItem(weak_ptr<NodeInterface> itemToRemove)
	{
		for (std::vector<weak_ptr<NodeInterface> >::iterator node = mNodesToTrack.begin();
		     node != mNodesToTrack.end();)
		{
			if (node->lock() == itemToRemove.lock())
			{
				node = mNodesToTrack.erase(node);
			}
			else
			{
				++node;
			}
		}
		shared_ptr<SceneRenderable> renderableToRemove = dynamic_pointer_cast<SceneRenderable>(itemToRemove.lock());
		for (std::vector<weak_ptr<SceneRenderable> >::iterator renderable = mRenderablesToTrack.begin();
		     renderable != mRenderablesToTrack.end();)
		{
			if (renderable->lock() == renderableToRemove)
			{
				renderable = mRenderablesToTrack.erase(renderable);
			}
			else
			{
				++renderable;
			}
		}
	}
	
	void PerpendicularCameraDolly::Update(Seconds lastFrameTime)
	{
		if (!mCamera || (mRenderablesToTrack.empty() && mNodesToTrack.empty()))
		{
			return;
		}
		AxisAlignedBox aabbForAllItems;
		BOOST_FOREACH(weak_ptr<NodeInterface> ptr, mNodesToTrack)
		{
			if (shared_ptr<NodeInterface> item = ptr.lock())
			{
				aabbForAllItems.Merge(item->GetDerivedPosition());
			}
		}
		BOOST_FOREACH(weak_ptr<SceneRenderable> ptr, mRenderablesToTrack)
		{
			if (shared_ptr<SceneRenderable> renderable = ptr.lock())
			{
				aabbForAllItems.Merge(renderable->GetSceneAxisAlignedBox());
			}
		}
		
		if (aabbForAllItems.IsFinite())
		{
			const float zPaddingToEnsureIntersectionIsNotNull = 1.f;
			aabbForAllItems.SetExtents(aabbForAllItems.GetMinimum() - Vector3(mVisualPadding, mVisualPadding, zPaddingToEnsureIntersectionIsNotNull),
			                           aabbForAllItems.GetMaximum() + Vector3(mVisualPadding, mVisualPadding, zPaddingToEnsureIntersectionIsNotNull));
			
			aabbForAllItems.MoveToWithinBoundsOf(mMaximumBoundsForCameraView);
			aabbForAllItems = aabbForAllItems.Intersection(mMaximumBoundsForCameraView);
		}
		
		if (aabbForAllItems.IsFinite())
		{
			const f32 distanceToFitX = aabbForAllItems.GetSize().x / (2.f * Maths::Tan<f32>(mCamera->GetFOV() /2.f));
			const f32 distanceToFitY = aabbForAllItems.GetSize().y / (2.f * Maths::Tan<f32>(mCamera->GetFOVy()/2.f));
			const f32 distanceZ = std::max(distanceToFitX, distanceToFitY);
			
			Vector3 lookAtPoint = aabbForAllItems.GetCentre();
			
			if (mMaximumBoundsForCameraView.IsFinite())
			{
				// This calculates how big the screen is actually going to be, based on
				// the aspect ratio, allowing the look-at point to be adjusted if the
				// screen view will overflow the maximum allowable bounds:
				const float aspectRatioBounds = aabbForAllItems.GetSize().x / aabbForAllItems.GetSize().y;
				const float aspectRatioCamera = mCamera->GetFOV() / mCamera->GetFOVy();
				if (aspectRatioCamera < aspectRatioBounds)
				{
					const float desiredHeight = aabbForAllItems.GetSize().x * 1.f / aspectRatioCamera;
					if (desiredHeight > mMaximumBoundsForCameraView.GetSize().y)
					{
						// Some vertical outside-of-bounds will have to be rendered:
						lookAtPoint.y = mMaximumBoundsForCameraView.GetCentre().y;
					}
					else
					{
						const float halfDesiredHeight = desiredHeight / 2.f;
						if (lookAtPoint.y - halfDesiredHeight < mMaximumBoundsForCameraView.GetMinimum().y)
						{
							lookAtPoint.y = mMaximumBoundsForCameraView.GetMinimum().y + halfDesiredHeight;
						}
						else
						if (lookAtPoint.y + halfDesiredHeight > mMaximumBoundsForCameraView.GetMaximum().y)
						{
							lookAtPoint.y = mMaximumBoundsForCameraView.GetMaximum().y - halfDesiredHeight;
						}
					}
				}
				else
				{
					const float desiredWidth = aabbForAllItems.GetSize().y * aspectRatioCamera;
					if (desiredWidth > mMaximumBoundsForCameraView.GetSize().x)
					{
						// Some horizontal outside-of-bounds will have to be rendered:
						lookAtPoint.x = mMaximumBoundsForCameraView.GetCentre().x;
					}
					else
					{
						const float halfDesiredWidth = desiredWidth / 2.f;
						if (lookAtPoint.x - halfDesiredWidth < mMaximumBoundsForCameraView.GetMinimum().x)
						{
							lookAtPoint.x = mMaximumBoundsForCameraView.GetMinimum().x + halfDesiredWidth;
						}
						else
						if (lookAtPoint.x + halfDesiredWidth > mMaximumBoundsForCameraView.GetMaximum().x)
						{
							lookAtPoint.x = mMaximumBoundsForCameraView.GetMaximum().x - halfDesiredWidth;
						}
					}
				}
			}
			
			mCamera->SetOrthoWindow(aabbForAllItems.GetSize().x, aabbForAllItems.GetSize().y);
			mCamera->SetPosition(lookAtPoint.x, lookAtPoint.y, distanceZ + aabbForAllItems.GetMaximum().z);
			mCamera->LookAt(lookAtPoint);
		}
	}
}
