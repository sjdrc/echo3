#ifndef _ECHOMESH_H_
#define _ECHOMESH_H_

#include <echo/Graphics/Renderable.h>
#include <echo/Graphics/Material.h>
#include <echo/Animation/Skeleton.h>
#include <echo/Maths/AxisAlignedBox.h>
#include <map>
#include <sstream>

#include "SubMesh.h"

namespace Echo
{
	class SubMesh;

	class BoneBinding
	{
	public:
		std::map< Size, f32 > mBoneWeights;
		Matrix4 mOffsetTransform;
		Vector3 mPosition;
		Quaternion mOrientation;
		Vector3 mScale;

		bool operator==(const BoneBinding& rhs) const
		{
			if(rhs.mBoneWeights.size() != mBoneWeights.size())
				return false;

			std::map< Size, f32 >::const_iterator it = mBoneWeights.begin();
			std::map< Size, f32 >::const_iterator itEnd = mBoneWeights.end();
			while(it != itEnd)
			{
				std::map< Size, f32 >::const_iterator oit = rhs.mBoneWeights.find(it->first);
				if(oit == rhs.mBoneWeights.end())
					return false;

				if(Maths::Abs(oit->second - oit->first) > 0.001f)
					return false;

				++it;
			}

			return true;
		}

		bool operator==(const std::map< Size, f32 >& rhs) const
		{
			if(rhs.size() != mBoneWeights.size())
				return false;

			std::map< Size, f32 >::const_iterator it = mBoneWeights.begin();
			std::map< Size, f32 >::const_iterator itEnd = mBoneWeights.end();
			while(it != itEnd)
			{
				std::map< Size, f32 >::const_iterator oit = rhs.find(it->first);
				if(oit == rhs.end())
					return false;

				if(Maths::Abs(oit->second - oit->first) > 0.001f)
					return false;

				++it;
			}

			return true;
		}
	};
	
	class Mesh : public Resource<Mesh>
	{
	public:
		/**
		 * Mesh Listeners can be notified of changes to the Mesh when it occurs.
		 * @note If you reposition a vertex then the change isn't detected automatically.
         */
		class Listener
		{
		public:
			Listener(){}
			virtual ~Listener(){}

			/**
			 * When a mesh changes this method will be called for any registered Listeners.
			 * Changes to a mesh are one of the following:
			 * - A vertex is added
			 * - A bone transform is applied
			 * - A Mesh sub class has a reason to notify the mesh that the extents changed.
			 * @note For reference, this method is essentially an external version of Mesh::MarkExtentsOutOfDate()
			 * and is called within Mesh::MarkExtentsOutOfDate().
             * @param mesh The mesh that changed.
             */
			virtual void OnMeshAABBNeedsUpdate(const Mesh& mesh) const = 0;
			virtual void OnMeshAABBUpdated(const Mesh& mesh) const = 0;
		};
		Mesh();
		virtual ~Mesh();

		shared_ptr< Skeleton > GetSkeleton() const
		{
			return mSkeleton;
		}

		void SetSkeleton(const shared_ptr< Skeleton >& val)
		{
			mSkeleton = val;
		}

		bool GetUseSkeleton() const
		{
			return mUseSkeleton;
		}

		void SetUseSkeleton(bool val)
		{
			mUseSkeleton = val;
		}
		
		template<typename T>
		VertexBuffer::Accessor<T> GetSubMeshComponents(Size subMeshIndex, Size componentIndex)
		{
			assert(subMeshIndex < mSubMeshes.size() && "SubMesh index is out of range.");
			if(subMeshIndex >= mSubMeshes.size())
			{
				return VertexBuffer::Accessor<T>();
			}
			return mSubMeshes[subMeshIndex]->GetComponents<T>(componentIndex);
		}

		template<typename T>
		VertexBuffer::Accessor<T> GetSubMeshComponents(Size subMeshIndex, const std::string& name)
		{
			assert(subMeshIndex < mSubMeshes.size() && "SubMesh index is out of range.");
			if(subMeshIndex >= mSubMeshes.size())
			{
				return VertexBuffer::Accessor<T>();
			}
			return mSubMeshes[subMeshIndex]->GetComponents<T>(name);
		}
		
		BoneBinding* CreateBinding(const std::map< Size, f32 >& bindingMap);
		void CacheBindings();

		/**
		 * Create a SubMesh
		 * @param name If not specified, the name will be determined internally.
		 * @return the new SubMesh or a null pointer if the mesh already has a mesh with the
		 * specified name.
		 */
		shared_ptr<SubMesh> CreateSubMesh(std::string name = "");

		/**
		 * Create a SubMesh with a VertexBuffer set up with a common set of attributes.
		 * The attributes created are:
		 * "Position"	- 1 x VertexAttribute::ComponentTypes::VECTOR3
		 * "Normal"		- 1 x VertexAttribute::ComponentTypes::VECTOR3
		 * "Colour"		- 1 x VertexAttribute::ComponentTypes::COLOUR
		 * "UV0"		- 1 x VertexAttribute::ComponentTypes::TEXTUREUV
		 * @param name If not specified, the name will be determined internally.
		 * @return the new SubMesh or a null pointer if the mesh already has a mesh with the
		 * specified name.
		 */
		shared_ptr<SubMesh> CreateCommonSubMesh(std::string name = "");

		/**
		 * Creates a submesh in the shape of a quad; i.e. a 2D square.
		 * @param dimension The dimension of the quad, this will create a square of the given dimension.
		 * @return A shared_ptr to the submesh created.
		 */
		shared_ptr<SubMesh> CreateQuadSubMesh(f32 dimension = 1.f);

		/**
		 * Creates a submesh in the shape of a quad; i.e. a 2D rectangle.
		 * @param width The width of the quad.
		 * @param height The height of the quad.
		 * @return A shared_ptr to the submesh created.
		 */
		shared_ptr<SubMesh> CreateQuadSubMesh(f32 width, f32 height);
		shared_ptr<SubMesh> CreateQuadSubMesh(f32 width, f32 height, const TextureUVPair& textureUV);

		/**
		 * Creates a submesh in the shape of a cube.
		 * The cube is made up of 24 vertices and 12 triangles. Vertices are duplicated
		 * to ensure texture coordinates for vertices can display an image on all faces.
		 * @param extents The dimensions of the cube.
		 * @param xResolution The number of quads on the X axis.
		 * @param yResolution The number of quads on the Y axis.
		 * @param zResolution The number of quads on the Z axis.
		 * @return A shared_ptr to the submesh created.
		 */
		shared_ptr<SubMesh> CreateCubeSubMesh(Vector3 extents, Size xResolution=1, Size yResolution=1, Size zResolution=1);

		/**
		 * Creates a UV sphere SubMesh using the specified resolution.
		 * @param meridians The number of meridian lines in the sphere,
		 * https://en.wikipedia.org/wiki/Meridian_(geography), if 0 the default of
		 * 10 times the radius will be used.
		 * @param parallels The number of circle of latitude (known as parallels) in the,
		 * if 0 the default of 10 times the radius will be used.
		 * @param uvsVertical Determine which orientation the texture UVs will be generated,
		 * if true the Y axis of the texture is mapped to the Y axis of the sphere, if false
		 * the X axis of the texture is mapped to the Y axis.
		 * @return A shared_ptr to the SubMesh created.
		 */
		shared_ptr<SubMesh> CreateUVSphereSubMesh(Scalar radius, Size meridians = 0, Size parallels = 0, bool uvsVertical = true);
		
		//Returns a pointer to the submesh "name" if it exists, otherwise returns 0
		shared_ptr<SubMesh> GetSubMesh(const std::string& name) const;

		//Returns a pointer to the submesh at index if the index is in range, otherwise returns 0
		shared_ptr<SubMesh> GetSubMesh(u32 index = 0u) const;
		
		shared_ptr<Mesh> Clone() const;

		//Removes and deletes the submesh "name" if it exists
		//returns true if the submesh existed and was removed.
		bool RemoveSubmesh(const std::string& name);

		//returns true if this mesh has a submesh with name, otherwise false
		bool HasSubmesh(const std::string& name) const;

		//Returns the number of submeshes this mesh has

		Size GetNumberOfSubMeshes() const
		{
			return mSubMeshes.size();
		}

		/**
		 * Removes all submeshes and bone bindings and invalidates the AABB.
		 * @note Does not remove listeners.
		 */
		void Clear();

		Vector3 GetCentre() const;
		void CentreMeshToOrigin();

		//SetMaterial sets all sub meshes with the same material.
		//For control over each sub mesh's material, use SubMesh::SetMaterial.
		void SetMaterial(shared_ptr<Material> material);

		virtual const AxisAlignedBox& GetAxisAlignedBox() const;

		virtual void Render(RenderContext& renderContext, const Matrix4& world, const Matrix4& worldView, Colour compoundDiffuse);
		
		/**
		 * Helper function to set the diffuse colour of all sub meshes' materials.
		 * @param diffuseColour The new diffuse colour
		 */
		void SetDiffuseColour(const Colour& diffuseColour);
		
		/**
		 * Add a listener for mesh change notifications.
		 * @note Before destruction of the listener RemoveListener() should be called.
		 * @note The reason this isn't a shared pointer is because it could create a cyclic dependency as most
		 * listeners will be owners of meshes.
		 * @note The reason that this isn't a weak pointer is because the listener is likely to add and remove
		 * itself internally and shared_from_this() may not be applicable or available at the time of call.
         */
		void AddListener(Listener* listener);

		/**
		 * Remove a listener from mesh change notifications.
		 * @note Make sure this is called before the listener is destroyed.
         */
		void RemoveListener(Listener* listener);
	protected:
		friend class SubMesh;
		void MarkExtentsOutOfDate() const;
	private:
		/**
		 * Update the AxisAlignedBox based on sub mesh vertices
		 */
		void UpdateAxisAlignedBox() const;
		virtual bool _Unload() override;
		virtual Size OnRequestMemoryRelease() override;
		
		std::vector< shared_ptr<SubMesh> > mSubMeshes;
		std::list< Listener* > mListeners;
		Size mSubMeshNames;
		mutable Mutex mAxisAlignedBoxMutex;
		mutable AxisAlignedBox mAxisAlignedBox;
		mutable bool mAxisAlignedBoxOutOfDate;
		shared_ptr< Skeleton > mSkeleton;
		std::vector< BoneBinding* > mBoneBindings;
		bool mUseSkeleton;
	};
}
#endif
