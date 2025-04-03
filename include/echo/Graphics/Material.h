#ifndef _ECHOMATERIAL_H_
#define _ECHOMATERIAL_H_

#include <echo/Types.h>
#include <echo/Graphics/RenderPass.h>

namespace Echo
{
	class Matrix4;
	class MultipassRenderable;
	class Texture;

	class Material : public Resource<Material>
	{
	public:
		Material();
		Material(const Material& other);
		virtual ~Material();
		shared_ptr< Material > Clone() const;

		void SetToDefaultMaterial();

		Size GetNumberOfPasses() const;
		RenderPass* GetCurrentPass() const
		{
			return mCurrentPass;
		}
		RenderPass* GetPass(u32 passIndex);
		void AddPass(const RenderPass& p);
		void SetPass(const RenderPass& p, u32 pass = 0);

		void SetTexture(shared_ptr<Texture> surface);
		shared_ptr<Texture> GetTexture() const;

		virtual void ApplyAndRender(RenderContext& renderContext, MultipassRenderable& renderable, const Matrix4& world, const Matrix4& worldView, Colour compoundDiffuse);
	private:
		virtual bool _Unload() override;
		virtual Size OnRequestMemoryRelease() override;

		std::vector<RenderPass> mPasses;
		RenderPass* mCurrentPass;
	};
}
#endif
