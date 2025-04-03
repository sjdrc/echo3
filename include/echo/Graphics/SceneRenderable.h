#ifndef _ECHOSCENERENDERABLE_H_
#define _ECHOSCENERENDERABLE_H_

#include <echo/Graphics/Renderable.h>
#include <echo/Graphics/Node.h>
#include <echo/Maths/AxisAlignedBox.h>

namespace Echo
{
	class Matrix4;
	class RenderTarget;
	class RenderPass;
	class SceneRenderable;
	class Camera;
	
	// Maybe Visitor<SceneRenderable> would be more generic,
	// with Visitable<SceneRenderable> corresponding
	class SceneRenderableVisitor
	{
	public:
		SceneRenderableVisitor(){}
		virtual ~SceneRenderableVisitor(){}
		/**
		 * Called by SceneRenderables when they are ready to be visited.
		 * Usually this occurs during a render operation. Accept() will be called on the SceneRenderable
		 * which gives the object the opportunity to perform any operations before visiting (like making
		 * itself look pretty).
		 * @param renderable
		 */
		virtual void SceneRenderableVisit(SceneRenderable& renderable) = 0;
		
		/**
		 * Get the current camera that is associated with the visit to the renderable.
		 * @note You do not have to support a camera but some classes may be try to use a camera
		 * when rendering.
		 * @return The camera associated with this visitor.
		 */
		virtual const Camera* GetCurrentCamera() = 0;

		/**
		 * Get the current render target that is associated with the visit to the renderable.
		 * @note You do not have to support a render target specifically but some classes may
		 * be try to use features from one.
		 * @return The render target associated with this visitor.
		 */
		virtual const RenderTarget* GetCurrentRenderTarget() = 0;
	};

	class SceneRenderable : public Renderable, public Node
	{
	public:
		SceneRenderable(const Vector3& position = Vector3::ZERO,
						const Quaternion& orientation = Quaternion::IDENTITY,
						const Vector3& scale = Vector3::UNIT_SCALE);
		virtual ~SceneRenderable();
		
		/**
		 * @see Node::operator=() for documentation on copying.
		 */
		SceneRenderable(const SceneRenderable& other);
		SceneRenderable& operator=(const SceneRenderable& rhs);
		
		AxisAlignedBox GetSceneAxisAlignedBox() const;
		
		/**
		 * Visitor pattern Accept method.
		 * If the derived SceneRenderable has children it should call Accept() and SceneRenderableVisit()
		 * for all active children.
		 * @note In most cases the Scene will be the one calling Accept to build a queue for rendering.
		 * The SceneRenderable must remain in scope until the Leave() method is called.
		 * @param visitor The visitor that SceneRenderableVisit() will be called on.
		 */
		virtual void Accept(SceneRenderableVisitor& visitor);
		
		/**
		 * Called by a visitor when it has completed operations all visited objects.
		 * @param visitor
		 */
		virtual void Leave(SceneRenderableVisitor&);
		
		/**
		 * Set the SceneRenderable visibility flag.
		 * @note The final visibility is not determined by this flag. A parent object may hide the object.
		 * Use GetVisible() to determine the final visibility.
		 * @param visible true if the object should be visible, otherwise false for invisible.
		 */
		void SetVisibilityFlag(bool visible) {mVisible = visible;}
		
		/**
		 * Get the SceneRenderable visibility flag.
		 * @note The final visibility is not determined by this flag. A parent object may hide the object.
		 * @return true if the object is visible, otherwise false.
		 */
		bool GetVisibilityFlag() const {return mVisible;}
		
		/**
		 * Get whether this object is visible based on the hierarchy it is in.
		 * @note This a more expensive call than GetVisibilityFlag() because it needs to query the
		 * chain of parent objects.
		 */
		bool GetVisible() const;
		
		/**
		 * Get the local axis aligned bounding box.
		 * @note This method needs to apply the local transform of the node to the box.
		 * @return the local axis aligned bounding box.
		 */
		virtual AxisAlignedBox GetAxisAlignedBox(bool applyLocalTransform = true) const = 0;
		
		virtual void Render(RenderContext& renderContext, Colour compoundDiffuse) override = 0;
	private:
		bool mVisible;		///Flag to indicate whether the object is visible or not.
		SceneRenderable* mParentSceneRenderable;
	protected:
		virtual void OnParentSet(Node* node) override;
	};
}
#endif 
