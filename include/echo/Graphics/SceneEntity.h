#ifndef _ECHOSCENEENTITY_H_
#define _ECHOSCENEENTITY_H_

#include <echo/Graphics/SceneRenderable.h>
#include <echo/Graphics/Mesh.h>

namespace Echo
{
	class Material;
	
	class SceneEntity : public SceneRenderable, public Mesh::Listener
	{
	public:
		SceneEntity(const Vector3& position = Vector3::ZERO,
					const Quaternion& orientation = Quaternion::IDENTITY,
					const Vector3& scale = Vector3::UNIT_SCALE);
		virtual ~SceneEntity();
		
		/**
		 * Copying a SceneEntity will copy its colour, mesh reference (they will share
		 * the same resource) and render settings.
		 * @see Node::operator=() for documentation on copying.
		 */
		SceneEntity(const SceneEntity& other);
		SceneEntity& operator=(const SceneEntity& rhs);
		
		/**
		 * Set the AxisAlignedBox of the scene entity manually.
		 * This is returned by GetAxisAlignedBox() if set rather than the one from the mesh.
		 */
		void SetAxisAlignedBox(AxisAlignedBox localAxisAlignedBox);

		/**
		 * Clears the manually set axis aligned box.
		 */
		void RemoveManualAxisAlignedBox();

		/**
		 * Get the axis aligned box of the scene entity.
		 * @param applyLocalTransform If true will apply the local transform from the Node.
		 * @return The manually set axis aligned box, or if one isn't set, the one derived from the Mesh,
		 * or a default constructed AxisAlignedBox (with AxisAlignedBox::Extents::NULL_EXTENT).
		 */
		virtual AxisAlignedBox GetAxisAlignedBox(bool applyLocalTransform = true) const override;
		virtual void Render(RenderContext& renderContext, Colour compoundDiffuse) override;

		void SetMesh(shared_ptr<Mesh> mesh);

		shared_ptr<Mesh> GetMesh() const
		{
			return mMesh;
		}
		
		// Law of Demeter
		void SetMaterial(shared_ptr<Material> material);
		
		void SetRenderAABB(bool renderAABB) {mRenderAABB = renderAABB;}
		bool GetRenderAABB() const {return mRenderAABB;}

		/**
		 * Set the local colour of the SceneEntity.
		 * The local colour can be used to modify the object's colour without the need to
		 * modify the material (which would modify for all object using that material).
		 * Instead, the colour will be combined with the diffuse colour of the material
		 * using the operator*. This means that if you combined a Colours::RED and
		 * Colours::BLUE then the result will be black because:
		 *		Colour(1,0,0) * Colour(0,0,1) = Colour(0,0,0)
		 * This is because the components are each multiplied together. There may be an
		 * option in the future to change the combine operation.
		 * @param colour The colour of this object.
		 */
		void SetColour(Colour colour) {mColour=colour;}
		
		/**
		 * Get the colour of this object.
		 * If the parent node is a SceneEntity and the combineWithParentIfEnabled flag is
		 * true then the colour returned will be the product of the parent's colour and
		 * local colour. There are two flags that control how the blending with combining
		 * of the parents colours is performed. One for the colour components and another
		 * for the alpha component as it may be desirable to control these separately.
		 * You would call this method and pass in false if you only wanted to find out the
		 * local colour.
		 * @see SetInheritParentColourWhenRendering()
		 * @see SetInheritParentAlphaWhenRendering()
		 * @param combineWithParentIfEnabled if false the local colour is returned, otherwise
		 * if the object is configured to combine the parent colour then the combined colour
		 * will be returned.
		 * @return Either the local colour of the object or the local colour combined with the
		 * parent's colour.
		 */
		Colour GetColour(bool combineWithParentIfEnabled) const;
	
		/**
		 * Set whether the parent colour should be inherited and combined with the local colour.
		 * If set to true the local colour will be combined with the parent colour if the
		 * parent node is a SceneEntity.
		 * The flag is true by default.
		 * @param inherit
		 */
		void SetInheritParentColour(bool inherit) {mInheritParentColour=inherit;}
		
		/**
		 * Get whether the parent colour should be inherited and combined with the local colour.
		 * If the flag is true then the local colour will be combined with the parent colour
		 * but only if the parent node is a SceneEntity.
		 * @return true if inheriting the parent colour will occur, otherwise false.
		 */
		bool GetInheritParentColour() const {return mInheritParentColour;}

		/**
		 * Set whether the parent alpha should be inherited and combined with the local alpha.
		 * If set to true the local alpha will be combined with the parent alpha if the
		 * parent node is a SceneEntity.
		 * The flag is true by default.
		 * @param inherit
		 */
		void SetInheritParentAlpha(bool inherit) {mInheritParentAlpha=inherit;}
		
		/**
		 * Get whether the parent alpha should be inherited and combined with the local alpha.
		 * If the flag is true then the local alpha will be combined with the parent alpha
		 * but only if the parent node is a SceneEntity.
		 * @return true if inheriting the parent alpha will occur, otherwise false.
		 */
		bool GetInheritParentAlpha() const {return mInheritParentAlpha;}
	protected:
		/**
		 * 
		 * @param node
		 */
		virtual void ChildAdded(Node& node) override;
		virtual void ChildRemoved(Node& node) override;
		virtual void Accept(SceneRenderableVisitor& visitor) override;
		virtual void Leave(SceneRenderableVisitor&) override;
		void UpdateAABBMesh();
		virtual void OnParentSet(Node* node) override;

		/**
		 * Mesh::Listener methods
		 */
		virtual void OnMeshAABBNeedsUpdate(const Mesh& mesh) const override;
		virtual void OnMeshAABBUpdated(const Mesh& mesh) const override;
	private:
		shared_ptr<Mesh> mMesh;
		shared_ptr<Mesh> mAABBMesh;
		std::list< SceneRenderable* > mChildren;
		SceneEntity* mParentSceneEntity;
		Colour mColour;
		bool mRenderAABB;
		bool mInheritParentColour;
		bool mInheritParentAlpha;
		optional<AxisAlignedBox> mManualAxisAlignedBox;
	};
}
#endif 
