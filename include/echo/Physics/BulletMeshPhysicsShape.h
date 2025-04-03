#ifndef BULLETMESHPHYSICSSHAPE_H
#define BULLETMESHPHYSICSSHAPE_H

#include <echo/Types.h>
#include <echo/Physics/BulletPhysicsShape.h>

class btTriangleIndexVertexArray;

namespace Echo
{
	class Mesh;
	
	class BulletMeshPhysicsShape : public BulletPhysicsShape
	{
	public:
		/**
		 */
		struct Modes
		{
			enum _
			{
				DYNAMIC_BODY,
				STATIC_BODY
			};
		};
		typedef Modes::_ Mode;

		/**
		 * @param mesh The mesh needs to have a "Position" vertex attribute. This class supports using
		 * double or single precision vertex values.
		 * @param mode Selecting this correctly is important for optimisation, a different physics mesh
		 * object type is created based on the mode.
		 */
		BulletMeshPhysicsShape(shared_ptr<Mesh> mesh, Mode mode);
		~BulletMeshPhysicsShape();

		btTriangleIndexVertexArray* GetTriangleIndexVertexArrayObject() const {return mIndexVertexArrays;}
	private:
		/**
		 * Set the mesh.
		 * Generates of the index vertex arrays for bullet.
		 * Currently we don't support setting a new mesh, you'll need to create a new instance with the
		 * new mesh and set the shape accordingly.
		 */
		void SetMesh(shared_ptr<Mesh> mesh);
		btTriangleIndexVertexArray* mIndexVertexArrays;
		shared_ptr<Mesh> mMesh;
	};
}
#endif /* BULLETMESHPHYSICSSHAPE_H */

