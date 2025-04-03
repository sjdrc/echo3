#include <echo/Graphics/Material.h>
#include <echo/Graphics/RenderPass.h>
#include <echo/Graphics/MultipassRenderable.h>
#include <echo/Graphics/Texture.h>

namespace Echo
{
	Material::Material() : Resource(true), mCurrentPass(0)
	{
	}
	
	Material::Material(const Material& other)
		: Resource(true),
		mPasses(other.mPasses),
		mCurrentPass(other.mCurrentPass)
	{
	}
	
	Material::~Material()
	{
	}

	shared_ptr< Material > Material::Clone() const
	{
		return make_shared<Material>(*this);
	}

	void Material::SetToDefaultMaterial()
	{
		mPasses.clear();
		mPasses.push_back(RenderPass());
	}

	void Material::SetTexture(shared_ptr<Texture> surface)
	{
		if(mPasses.empty())
			AddPass(RenderPass());
		mPasses[0].SetTexture(surface);
	}

	shared_ptr<Texture> Material::GetTexture() const
	{
		if(mPasses.empty())
			return shared_ptr<Texture>();
		return mPasses[0].GetTexture();
	}

	RenderPass* Material::GetPass(u32 passIndex)
	{
		if(passIndex < mPasses.size())
			return &(mPasses[passIndex]);
		return 0;
	}

	void Material::AddPass(const RenderPass& pass)
	{
		mPasses.push_back(pass);
	}

	void Material::SetPass(const RenderPass& pass, u32 passIndex)
	{
		if(passIndex < mPasses.size())
		{
			mPasses[passIndex] = pass;
			return;
		}
		AddPass(pass);
	}

	void Material::ApplyAndRender(RenderContext& renderContext, MultipassRenderable& renderable, const Matrix4& world, const Matrix4& worldView, Colour compoundDiffuse)
	{
		//Loop through the passes and apply each material followed by executing the render
		size_t numPasses = mPasses.size();
		for(size_t p = 0; p < numPasses; ++p)
		{
			RenderPass& pass = mPasses[p];
			if(!pass.GetActive())
			{
				continue;
			}
			mCurrentPass = &pass;
			pass.Apply(renderContext, world, worldView, compoundDiffuse);
			renderable.Render(renderContext,pass,compoundDiffuse);
			mCurrentPass = 0;
		}
	}

	Size Material::GetNumberOfPasses() const
	{
		return mPasses.size();
	}

	bool Material::_Unload()
	{
		mPasses.clear();
		return true;
	}

	Size Material::OnRequestMemoryRelease()
	{
		return 0;
	}
}

