#ifndef _TEST_SPINNING_CUBE_H_
#define _TEST_SPINNING_CUBE_H_
#include <echo/Graphics/SceneEntity.h>
#include <echo/Graphics/Mesh.h>
#include <echo/Graphics/SubMesh.h>

using namespace Echo;

namespace Tests
{
	// A basic custom scene renderable.
	class SpinningCube : public SceneEntity, public Task
	{
	public:
		// A basic custom scene renderable.
		SpinningCube(f32 speedMultiplier = 1.f) : mAngle(0), mSpeedMultiplier(speedMultiplier)
		{
			SetMesh(shared_ptr<Mesh>(new Mesh()));
			// Manually create a cube mesh. Normally you would load a mesh from a file with the MeshManager.
			GetMesh()->CreateCubeSubMesh(Vector3(4,4,4),1,1,1);

			// Create a basic material for the cube:
			shared_ptr<Material> material(new Material());
			RenderPass pass;
			pass.SetCullMode(RenderPass::CullModes::BACK);
			pass.SetVertexColouringEnabled(true);
			material->AddPass(pass);

			// Set the mesh material:
			GetMesh()->SetMaterial(material);
		}

		void Update(Seconds lastFrameTime)
		{
			// We'll change the orientation every update.
			SetOrientation(Quaternion(Radian(mAngle), Vector3(Maths::Sin(mAngle)*0.1f,Maths::Cos(mAngle)*0.5f,Maths::Sin(mAngle)*0.2f)));
			// Rotate once a second.
			mAngle += lastFrameTime.count() * Maths::TWO_PI * mSpeedMultiplier;
		}
	private:
		f32 mAngle;
		f32 mSpeedMultiplier;
	};
}

#endif
