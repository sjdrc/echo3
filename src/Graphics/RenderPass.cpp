#include <echo/Graphics/RenderPass.h>
#include <echo/Graphics/RenderTarget.h>
#include <echo/Graphics/ShaderProgram.h>
#include <echo/Graphics/Camera.h>
#include <echo/Graphics/Light.h>

namespace Echo
{
	RenderPass::RenderPass()
	{
		mBlendMode = BlendModes::TRANSPARENT;
		mDepthFunction = RenderPass::DepthFunctions::LESS_OR_EQUAL;
		mEnabledOptions = RenderPass::Options::DEPTH_CHECK | RenderPass::Options::DEPTH_WRITE;
		mCullMode = RenderPass::CullModes::NONE;
		mAlphaTestFunction = RenderPass::AlphaTestFunctions::ALWAYS;
		mAlphaTestValue = 0.f;

		mAmbient.Set(0.8f, 0.8f, 0.8f, 1.0f);
		mDiffuse.Set(1.0f, 1.0f, 1.0f, 1.0f);
		mSpecular.Set(1.0f, 1.0f, 1.0f, 1.0f);
		mEmissive.Set(0.f, 0.f, 0.f, 0.f);
		mActive = true;
		mShininess = 0.0f;
		mPointAndLineSize = 1.f;
		mProgramCacheVersion = std::numeric_limits<Size>::max();
	}

	RenderPass::RenderPass(const RenderPass& pass)
	{
		mProgram = pass.mProgram;
		mProgramViewProjectionWorld = pass.mProgramViewProjectionWorld;
		mProgramViewMatrix = pass.mProgramViewMatrix;
		mProgramWorldMatrix = pass.mProgramWorldMatrix;
		mProgramProjectionMatrix = pass.mProgramProjectionMatrix;
		mProgramCameraPosition = pass.mProgramCameraPosition;
		mDepthFunction = pass.mDepthFunction;
		mCullMode = pass.mCullMode;
		mEnabledOptions = pass.mEnabledOptions;
		mActive = pass.mActive;
		mBlendMode = pass.mBlendMode;
		mAmbient = pass.mAmbient;
		mDiffuse = pass.mDiffuse;
		mSpecular = pass.mSpecular;
		mEmissive = pass.mEmissive;
		mShininess = pass.mShininess;
		mPointAndLineSize = pass.mPointAndLineSize;
		mTextureUnits = pass.mTextureUnits;
		mAlphaTestFunction = pass.mAlphaTestFunction;
		mAlphaTestValue = pass.mAlphaTestValue;
		mProgramCacheVersion = pass.mProgramCacheVersion;
		mProgramLightPositionCache = pass.mProgramLightPositionCache;
		mProgramLightColourCache = pass.mProgramLightColourCache;
		mProgramLightPowerCache = pass.mProgramLightPowerCache;
		mNumberOfLightsCached = pass.mNumberOfLightsCached;
	}

	RenderPass& RenderPass::operator=(const RenderPass& pass)
	{
		if (this == &pass)
			return *this;

		mProgram = pass.mProgram;
		mDepthFunction = pass.mDepthFunction;
		mProgramViewProjectionWorld = pass.mProgramViewProjectionWorld;
		mProgramViewMatrix = pass.mProgramViewMatrix;
		mProgramWorldMatrix = pass.mProgramWorldMatrix;
		mProgramProjectionMatrix = pass.mProgramProjectionMatrix;
		mProgramCameraPosition = pass.mProgramCameraPosition;
		mCullMode = pass.mCullMode;
		mEnabledOptions = pass.mEnabledOptions;
		mActive = pass.mActive;
		mBlendMode = pass.mBlendMode;
		mAmbient = pass.mAmbient;
		mDiffuse = pass.mDiffuse;
		mSpecular = pass.mSpecular;
		mEmissive = pass.mEmissive;
		mShininess = pass.mShininess;
		mPointAndLineSize = pass.mPointAndLineSize;
		mTextureUnits = pass.mTextureUnits;
		mAlphaTestFunction = pass.mAlphaTestFunction;
		mAlphaTestValue = pass.mAlphaTestValue;
		mProgramCacheVersion = pass.mProgramCacheVersion;
		mProgramLightPositionCache = pass.mProgramLightPositionCache;
		mProgramLightColourCache = pass.mProgramLightColourCache;
		mProgramLightPowerCache = pass.mProgramLightPowerCache;
		mNumberOfLightsCached = pass.mNumberOfLightsCached;

		return *this;
	}
		
	RenderPass::~RenderPass()
	{
	}

	void RenderPass::AddTextureUnit(const TextureUnit& u)
	{
		mTextureUnits.push_back(u);
	}

	void RenderPass::SetTextureUnit(const TextureUnit& u, u32 stage)
	{
		if(stage >= mTextureUnits.size())
		{
			AddTextureUnit(u);
			return;
		}
		mTextureUnits[stage] = u;
	}

	TextureUnit* RenderPass::GetTextureUnit(u32 i)
	{
		if(i >= mTextureUnits.size())
		{
			return 0;
		}
		return &(mTextureUnits[i]);
	}

	const TextureUnit* RenderPass::GetTextureUnit(u32 i) const
	{
		if(i >= mTextureUnits.size())
		{
			return 0;
		}
		return &(mTextureUnits[i]);
	}
	
	void RenderPass::SetTexture(shared_ptr<Texture> texture)
	{
		if(GetNumTextureUnits() == 0)
		{
			TextureUnit u;
			u.SetTexture(texture);
			AddTextureUnit(u);
		} else
		{
			mTextureUnits[0].SetTexture(texture);
		}
	}

	shared_ptr<Texture> RenderPass::GetTexture() const
	{
		if(GetNumTextureUnits() != 0)
		{
			return mTextureUnits[0].GetTexture();
		}
		return shared_ptr<Texture>();
	}

	void RenderPass::Apply(RenderContext& renderContext, const Matrix4& world, const Matrix4& worldView, Colour compoundDiffuse)
	{
		compoundDiffuse*=mDiffuse;
		RenderTarget& renderTarget = renderContext.mRenderTarget;
		if(mEnabledOptions & RenderPass::Options::LIGHTING)
		{
			renderTarget.SetLightingEnabled(true);
			renderTarget.SetMaterialColourEnabled(true);
			renderTarget.SetDiffuse(compoundDiffuse);
			renderTarget.SetMaterialColours(mAmbient, compoundDiffuse, mSpecular, mEmissive, mShininess);
		} else
		{
			renderTarget.SetDiffuse(compoundDiffuse);
			renderTarget.SetMaterialColours(mAmbient, compoundDiffuse, mSpecular, mEmissive, mShininess);

			renderTarget.SetMaterialColourEnabled(false);
			renderTarget.SetLightingEnabled(false);
		}

		if(mEnabledOptions & RenderPass::Options::DEPTH_CHECK)
		{
			renderTarget.SetDepthTestEnabled(true);
			renderTarget.SetDepthFunction(mDepthFunction);
		} else
		{
			renderTarget.SetDepthTestEnabled(false);
		}

		if(mEnabledOptions & RenderPass::Options::DEPTH_WRITE)
		{
			renderTarget.SetDepthWriteEnabled(true);
		}else
		{
			renderTarget.SetDepthWriteEnabled(false);
		}

		renderTarget.SetBlendMode(mBlendMode);
		renderTarget.SetAlphaTest(mAlphaTestFunction,mAlphaTestValue);
		Size numTexUnits = mTextureUnits.size();
		renderTarget.ResetTextureUnits();
		for(Size tu = 0; tu < numTexUnits; ++tu)
		{
			renderTarget.SetTextureUnit(mTextureUnits[tu], tu);
		}
		renderTarget.SetCullMode(mCullMode);
		renderTarget.SetPointAndLineSize(mPointAndLineSize);
		if(mProgram)
		{
			if(mProgramViewProjectionWorld)
			{
				*mProgramViewProjectionWorld = (renderContext.mProjectionMatrix * worldView);
			}
			
			if(mProgramCameraPosition)
			{
				*mProgramCameraPosition = renderTarget.GetCurrentCamera()->GetPosition();
			}
			
			if(mProgramViewMatrix)
			{
				*mProgramViewMatrix = renderContext.mViewMatrix;
			}
			
			if(mProgramWorldMatrix)
			{
				*mProgramWorldMatrix = world;
			}
			
			if(mProgramProjectionMatrix)
			{
				*mProgramProjectionMatrix = renderContext.mProjectionMatrix;
			}

			Size maxLights = CacheProgramVariables(renderContext.mLights.size());
			for(Size i=0; i < maxLights; ++i)
			{
				*mProgramLightPositionCache[i] = renderContext.mLights[i]->GetDerivedPosition();
				*mProgramLightColourCache[i] = renderContext.mLights[i]->GetDiffuseColour();
				*mProgramLightPowerCache[i] = renderContext.mLights[i]->GetPowerScale();
			}
			if(mNumberOfLightsCached)
			{
				*mNumberOfLightsCached = maxLights;
			}
		}
	}

	Size RenderPass::CacheProgramVariables(Size numLights)
	{
		Size programVersion = mProgram->GetVersion();

		if(	mProgramLightPositionCache.size()!=numLights ||
			mProgramLightColourCache.size()!=numLights ||
			mProgramLightPowerCache.size()!=numLights ||
			mProgramCacheVersion==programVersion)
		{
			mProgramLightPositionCache.resize(0);
			mProgramLightPositionCache.reserve(numLights);
			mProgramLightColourCache.resize(0);
			mProgramLightColourCache.reserve(numLights);
			mProgramLightPowerCache.resize(0);
			mProgramLightPowerCache.reserve(numLights);
			mProgramCacheVersion = programVersion;
		}else
		{
			// This method is the only method that modifies the cache lists. The code below ensures the
			// list sizes are consistent with one another.
			return mProgramLightPositionCache.size();
		}
		
		mNumberOfLightsCached = mProgram->GetUniformVariable<int>("numberOfLights");

		for( Size i = 0; i < numLights; ++i)
		{
			std::string index = std::to_string(i) + "]";
			shared_ptr<Vector3> position = mProgram->GetUniformVariable<Vector3>("lightPosition[" + index);
			shared_ptr<Vector3> colour = mProgram->GetUniformVariable<Vector3>("lightColour[" + index);
			shared_ptr<f32> power = mProgram->GetUniformVariable<float>("lightPower[" + index);
			if(!position || !colour || !power)
			{
				return i;
			}
			mProgramLightPositionCache.push_back(position);
			mProgramLightColourCache.push_back(colour);
			mProgramLightPowerCache.push_back(power);
		}
		return numLights;
	}

	size_t RenderPass::GetNumTextureUnits() const
	{
		return mTextureUnits.size();
	}
}
