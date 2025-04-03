#ifndef _ECHORENDERPASS_H_
#define _ECHORENDERPASS_H_

#include <echo/Types.h>
#include <echo/Graphics/Texture.h>
#include <echo/Maths/Matrix4.h>
#include <echo/Graphics/BlendMode.h>
#include <echo/Graphics/Colour.h>
#include <echo/Graphics/TextureUnit.h>
#include <echo/Graphics/ShaderProgram.h>
#include <echo/Graphics/RenderContext.h>

namespace Echo
{
	class RenderTarget;

	class RenderPass
	{

	public:
		struct Options
		{
			enum _
			{
				NONE = 0,
				DEPTH_CHECK = 1,
				DEPTH_WRITE = 2,
				LIGHTING = 4,
				VERTEX_COLOURS_ENABLED = 8
			};
		};
		typedef Options::_ MaterialOption;

		struct DepthFunctions
		{
			enum _
			{
				NEVER = 1,
				LESS,
				EQUAL,
				LESS_OR_EQUAL,
				GREATER,
				NOT_EQUAL,
				GREATER_OR_EQUAL,
				ALWAYS
			};
		};
		typedef DepthFunctions::_ DepthFunction;

		struct AlphaTestFunctions
		{
			enum _
			{
				NEVER = 1,
				LESS,
				EQUAL,
				LESS_OR_EQUAL,
				GREATER,
				NOT_EQUAL,
				GREATER_OR_EQUAL,
				ALWAYS
			};
		};
		typedef AlphaTestFunctions::_ AlphaTestFunction;
		
		struct CullModes
		{
			enum CullModeEnum
			{
				NONE = 0,
				FRONT,
				BACK
			};
		};
		typedef CullModes::CullModeEnum CullMode;

		DepthFunction mDepthFunction;
		CullMode mCullMode;
		size_t mEnabledOptions;
		bool mActive;
		BlendMode mBlendMode;
		AlphaTestFunction mAlphaTestFunction;
		float mAlphaTestValue;
		shared_ptr<ShaderProgram> mProgram;
		shared_ptr<Matrix4> mProgramViewProjectionWorld;
		shared_ptr<Matrix4> mProgramViewMatrix;
		shared_ptr<Matrix4> mProgramWorldMatrix;
		shared_ptr<Matrix4> mProgramProjectionMatrix;
		shared_ptr<Vector3> mProgramCameraPosition;
		Colour mAmbient;
		Colour mDiffuse;
		Colour mSpecular;
		Colour mEmissive;
		f32 mShininess;
		f32 mPointAndLineSize;
		std::vector< TextureUnit > mTextureUnits;
		std::vector< shared_ptr<Vector3> > mProgramLightPositionCache;
		std::vector< shared_ptr<Vector3> > mProgramLightColourCache;
		std::vector< shared_ptr<f32> > mProgramLightPowerCache;
		shared_ptr<int> mNumberOfLightsCached;
		Size mProgramCacheVersion;

		RenderPass();
		RenderPass(const RenderPass& pass);
		RenderPass& operator=(const RenderPass& pass);
		~RenderPass();

		bool GetActive() const
		{
			return mActive;
		}

		void SetActive(bool val)
		{
			mActive = val;
		}

		const BlendMode& GetBlendMode() const
		{
			return mBlendMode;
		}

		const AlphaTestFunction& GetAlphaTestFunction() const
		{
			return mAlphaTestFunction;
		}

		float GetAlphaTestValue() const
		{
			return mAlphaTestValue;
		}

		const DepthFunction& GetDepthFunction() const
		{
			return mDepthFunction;
		}

		const CullMode& GetCullMode() const
		{
			return mCullMode;
		}

		const Colour& GetAmbient() const
		{
			return mAmbient;
		}

		const Colour& GetDiffuse() const
		{
			return mDiffuse;
		}

		const Colour& GetSpecular() const
		{
			return mSpecular;
		}

		const Colour& GetEmissive() const
		{
			return mEmissive;
		}

		f32 GetShininess() const
		{
			return mShininess;
		}

		f32 GetPointAndLineSize() const
		{
			return mPointAndLineSize;
		}

		shared_ptr<ShaderProgram> GetProgram() const
		{
			return mProgram;
		}

		void SetProgram(shared_ptr<ShaderProgram> val)
		{
			mProgram = val;
		}

		void SetBlendMode(const BlendMode& val)
		{
			mBlendMode = val;
		}

		void SetAlphaTestFunction(const AlphaTestFunction& val)
		{
			mAlphaTestFunction = val;
		}

		void SetAlphaTestValue(float val)
		{
			mAlphaTestValue = val;
		}

		void SetDepthFunction(const DepthFunction& val)
		{
			mDepthFunction = val;
		}

		void SetCullMode(const CullMode& val)
		{
			mCullMode = val;
		}

		void SetAmbient(const Colour& val)
		{
			mAmbient = val;
		}

		void SetDiffuse(const Colour& val)
		{
			mDiffuse = val;
		}

		void SetSpecular(const Colour& val)
		{
			mSpecular = val;
		}

		void SetEmissive(const Colour& val)
		{
			mEmissive = val;
		}

		void SetShininess(f32 val)
		{
			mShininess = val;
		}

		void SetPointAndLineSize(f32 pointAndLineSize)
		{
			mPointAndLineSize=pointAndLineSize;
		}

		Size GetNumTextureUnits() const;
		/**
		 * Get the specified texture unit.
		 * @param i zero based index.
		 * @return The texture unit at the given index or null if the index is greater or
		 *			equal to the value returned by GetNumTextureUnits().
		 */
		TextureUnit* GetTextureUnit(u32 i);
		const TextureUnit* GetTextureUnit(u32 i) const;
		void AddTextureUnit(const TextureUnit& u);
		void SetTextureUnit(const TextureUnit& u, u32 stage = 0);

		void SetDepthCheckEnabled(bool enabled)
		{
			if(enabled)
			{
				mEnabledOptions |= Options::DEPTH_CHECK;
			}else
			{
				if(mEnabledOptions & Options::DEPTH_CHECK)
				{
					mEnabledOptions ^= Options::DEPTH_CHECK;
				}
			}
		}
		
		bool GetDepthCheckEnabled() const
		{
			return (mEnabledOptions & Options::DEPTH_CHECK);
		}

		void SetDepthWriteEnabled(bool enabled)
		{
			if(enabled)
			{
				mEnabledOptions |= Options::DEPTH_WRITE;
			}else
			{
				if(mEnabledOptions & Options::DEPTH_WRITE)
				{
					mEnabledOptions ^= Options::DEPTH_WRITE;
				}
			}
		}

		bool GetDepthWriteEnabled() const
		{
			return (mEnabledOptions & Options::DEPTH_WRITE);
		}

		void SetLightingEnabled(bool enabled)
		{
			if(enabled)
			{
				mEnabledOptions |= Options::LIGHTING;
			}else
			{
				if(mEnabledOptions & Options::LIGHTING)
				{
					mEnabledOptions ^= Options::LIGHTING;
				}
			}
		}

		bool GetLightingEnabled() const
		{
			return (mEnabledOptions & Options::LIGHTING);
		}

		void SetVertexColouringEnabled(bool enabled)
		{
			if(enabled)
			{
				mEnabledOptions |= Options::VERTEX_COLOURS_ENABLED;
			}else
			{
				if(mEnabledOptions & Options::VERTEX_COLOURS_ENABLED)
				{
					mEnabledOptions ^= Options::VERTEX_COLOURS_ENABLED;
				}
			}
		}

		bool GetVertexColouringEnabled() const
		{
			return (mEnabledOptions & Options::VERTEX_COLOURS_ENABLED);
		}

		void SetTexture(shared_ptr<Texture> texture);
		shared_ptr<Texture> GetTexture() const;

		/**
		 * Checks the program variable cache lists.
		 * The cache avoids performing multiple map searches every render.
		 * @note This method should only be called if a program is set.
		 * @param numLights The number of lights to set.
		 * @return The number of lights that the shader supports up to the numLights (it may support more than the specified).
		 */
		Size CacheProgramVariables(Size numLights);

		void Apply(RenderContext& renderContext, const Matrix4& world, const Matrix4& worldView, Colour compoundDiffuse);
	};
}
#endif
