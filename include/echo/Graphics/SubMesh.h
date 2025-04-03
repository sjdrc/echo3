#ifndef _ECHOSUBMESH_H_
#define _ECHOSUBMESH_H_

#include <echo/Types.h>
#include <echo/Graphics/Renderable.h>
#include <echo/Graphics/MultipassRenderable.h>
#include <echo/Graphics/Material.h>
#include <echo/Graphics/PrimitiveTypes.h>
#include <echo/Graphics/ElementBuffer.h>
#include <echo/Graphics/VertexBuffer.h>
#include <echo/Maths/AxisAlignedBox.h>
#include <echo/Kernel/Mutex.h>
#include <boost/foreach.hpp>
#include <vector>

namespace Echo
{
	class BoneBinding;
	class Mesh;

	class SubMesh : public MultipassRenderable
	{
	public:
		struct MeshTypes
		{
			enum _
			{
				TRIANGLES,
				TRIANGLE_STRIP,
				POINTS,
				LINES,
				LINE_STRIP
			};
		};
		typedef MeshTypes::_ MeshType;

		/**
		 * @see SetVisible()
		 */
		void Show() {mVisible=true;}

		/**
		 * @see SetVisible()
		 */
		void Hide() {mVisible=false;}
		
		/**
		 * Set whether this SubMesh is be visible.
		 * @note Visibility is culled at the Render() level so SubMeshes whose parents are visible
		 * and proceeded through a render pipeline might still Render() on the SubMesh.
		 */
		void SetVisible(bool visible) {mVisible = visible;}

		/**
		 * Get whether this SubMesh is be visible.
		 * @note Visibility is culled at the Render() level so SubMeshes whose parents are visible
		 * and proceeded through a render pipeline might still Render() on the SubMesh.
		 */
		bool GetVisible() const {return mVisible;}

		shared_ptr<SubMesh> Clone(Mesh& clonesParent) const;
		
		template<typename T>
		VertexBuffer::Accessor<T> GetComponents(Size index)
		{
			assert(mVertexBuffer && "Get Components called before a VertexBuffer was created.");
			if(!mVertexBuffer)
			{
				return VertexBuffer::Accessor<T>();
			}
			return mVertexBuffer->GetAccessor<T>(index);
		}

		template<typename T>
		VertexBuffer::Accessor<T> GetComponents(std::string name)
		{
			assert(mVertexBuffer && "Get Components called before a VertexBuffer was created.");
			if(!mVertexBuffer)
			{
				return VertexBuffer::Accessor<T>();
			}
			return mVertexBuffer->GetAccessor<T>(name);
		}
		
		template<typename T>
		const VertexBuffer::Accessor<T> GetComponents(Size index) const
		{
			assert(mVertexBuffer && "Get Components called before a VertexBuffer was created.");
			if(!mVertexBuffer)
			{
				return VertexBuffer::Accessor<T>();
			}
			return mVertexBuffer->GetAccessor<T>(index);
		}

		template<typename T>
		const VertexBuffer::Accessor<T> GetComponents(std::string name) const
		{
			assert(mVertexBuffer && "Get Components called before a VertexBuffer was created.");
			if(!mVertexBuffer)
			{
				return VertexBuffer::Accessor<T>();
			}
			return mVertexBuffer->GetAccessor<T>(name);
		}
		
		/**
		 * Set the SubMesh bone weights.
		 * SubMeshes can apply bone bindings for animations. The Bone bindings apply a weighted transform to vertices.
		 * Bone bindings may be shared across like-SubMeshes.
		 * If you are interacting with this method manually you probably already have a good understanding of what
		 * you're doing, so I'll leave you to it. Otherwise, run away!
		 * @param boneBindings the new Bone bindings
		 */
		void SetBoneWeights(shared_ptr< std::vector<BoneBinding*> > boneBindings)
		{
			mBoneWeights = boneBindings;
		}
		
		/**
		 * Get the name of the SubMesh.
		 * @return the name given to the SubMesh, perhaps affectionate or formal, but either way given to on creation.
		 */
		const std::string& GetName() const
		{
			return mName;
		}
		
		/**
		 * SubMeshes belong to a Mesh.
		 * This method gets the Mesh.
		 * @return The parent Mesh.
		 */
		Mesh& GetParent()
		{
			return mParent;
		}
		
		/**
		 * @copydoc GetParent()
		 */
		const Mesh& GetParent() const
		{
		  return mParent;
		}

		Vector3 GetDimensions() const;
		
		/**
		 * Set the vertex buffer.
		 * The vertex buffer should be completed before this method is called. This method
		 * performs some internal house keeping.
		 */
		void SetVertexBuffer(shared_ptr<VertexBuffer> vertexBuffer);
		
		/**
		 * Set the element buffer
		 * The element buffer buffer should be completed before this method is called. This method
		 * performs some internal house keeping.
		 */
		void SetElementBuffer(shared_ptr<ElementBuffer> elementBuffer);
		
		/**
		 * Create and set and element buffer with the specified settings.
		 * @see ElementBuffer
		 * @return true if the element buffer was succesfully created and assigned, false if allocation failed.
		 */
		bool SetElementBuffer(ElementBuffer::Type type, ElementBuffer::IndexType indexType, ElementBuffer::ElementType elementType, Size numberOfElements);
		
		/**
		 * Get the vertex buffer.
		 * If one has not been set this method will initialise an empty vertex buffer.
		 * It is a requirement that once you've finished manipulating the vertex buffer
		 * you should call SubMesh::Finalise() to indicate that you're done. This triggers
		 * internal house keeping.
		 * @return The vertex buffer.
		 */
		shared_ptr<VertexBuffer> GetVertexBuffer(VertexBuffer::Type type = VertexBuffer::Types::DYNAMIC);
		
		/**
		 * Get the element buffer.
		 * This may be shared across multiple SubMesh instances.
		 * @see Finalise() for more information about modifying data in this vector.
		 * @return if not null, a vector containing mesh indices.
		 */
		shared_ptr< ElementBuffer > GetElementBuffer() const
		{
			return mElementBuffer;
		}

		shared_ptr< std::vector<BoneBinding*> > GetBoneWeights() const
		{
			return mBoneWeights;
		}
		
		/**
		 * Since SubMesh allows direct access to mesh data, after you are done modifying
		 * any data you should call Finalise() so the SubMesh can perform any necessary
		 * house keeping. Not doing so could result in unexpected results, especially
		 * with respect to the local AABB.
		 * 
		 * You should finalise after modifying the mesh arrays and before any further
		 * operations on the SubMesh. Modifying a mesh's array data means you should
		 * consider the SubMesh invalid until calling Finalise(). For example, if you
		 * modify vertex data then want to get the centre of the mesh you should call
		 * Finalise() before GetCentre(). This is because GetCentre() depends on the
		 * AABB to be up to date to return the correct centre.
		 * 
		 * This method does not just perform AABB calculation. AABB calculation is just
		 * one example of some internal house keeping that is performed.
		 */
		void Finalise();

		/**
		 * Clear the SubMesh.
		 * @param deallocate if true clear(), otherwise resize(0), is called on buffers.
		 */
		void Clear(bool deallocate = true);

		/**
		 * Set the AxisAlignedBox of the SubMesh manually.
		 * This is returned by GetAxisAlignedBox() rather than the one calculated from the "Position" vertices.
		 */
		void SetAxisAlignedBox(AxisAlignedBox localAxisAlignedBox);

		/**
		 * Clears the manually set axis aligned box.
		 */
		void RemoveManualAxisAlignedBox();

		const AxisAlignedBox& GetAxisAlignedBox() const;
		Vector3 GetCentre() const;
		shared_ptr<Material> GetMaterial() const
		{
			return mMaterial;
		}
		void SetMaterial(shared_ptr<Material> material);
		void SetType(MeshType t){mType=t;}
		void Render(RenderContext& renderContext, const Matrix4& world, const Matrix4& worldView, Colour compoundDiffuse);
		void Render(RenderContext& renderContext, const RenderPass& pass, Colour compoundDiffuse);
		void GenerateNormals();
		void GenerateTangents(bool logError);
		void TranslateVertices(const Vector3& translation);
		
		void SetPointAndLineSize(f32 pointAndLineSize) {mPointAndLineSize=pointAndLineSize;}
		f32 GetPointAndLineSize() const {return mPointAndLineSize;}

		~SubMesh();
	private:
		friend class Mesh;
		shared_ptr<Material> mMaterial;
		shared_ptr<VertexBuffer> mVertexBuffer;
		shared_ptr< std::vector<Vector3> > mOriginalVertices;
		shared_ptr< std::vector<BoneBinding*> > mBoneWeights;
		shared_ptr<ElementBuffer> mElementBuffer;
		Mesh& mParent;
		mutable AxisAlignedBox mAxisAlignedBox;
		optional<AxisAlignedBox> mManualAxisAlignedBox;
		std::string mName;
		bool mVisible;
		mutable bool mAxisAlignedBoxOutOfDate;
		MeshType mType;					///Defaults to MeshType::TRIANGLES
		f32 mPointAndLineSize;			///Used for MeshType::POINTS, MeshType::LINES, MeshType::LINE_STRIP.
		mutable Mutex mAxisAlignedBoxMutex;

		/**
		 * Apply the Bone transforms to the vertices
		 */
		void ApplyVertexBoneTransforms();
		
		/**
		 * Generate a copy of the current vertices buffer (in its current state) so we have
		 * a starting point for Bone transformations. Whatever the current state of the vertex
		 * buffer is considered the "initial" (mOriginalVertices) position.
		 */
		void GenerateTransformBuffers();
		
		/**
		 * Update the SubMesh's AABB.
		 * This method is used from various const methods and updates the mutable value which
		 * is why it is declared const.
		 */
		void UpdateAxisAlignedBox() const;

		void MarkExtentsOutOfDate();

		SubMesh(const std::string& name, Mesh& parent);
	};
}
#endif
