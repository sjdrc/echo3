#ifndef _ECHOTEXTUREUNIT_H_
#define _ECHOTEXTUREUNIT_H_

#include <echo/Types.h>
#include <echo/Graphics/Texture.h>
#include <echo/Graphics/BlendMode.h>
#include <echo/Maths/Matrix4.h>

namespace Echo
{
	class CubeMapTexture;
	
	class TextureUnit
	{
	public:
		struct TextureWrapModes
		{
			enum TextureWrapModeEnum{	CLAMP=0,
										CLAMP_TO_EDGE=1,
										REPEAT=2};		
		};
		typedef TextureWrapModes::TextureWrapModeEnum TextureWrapMode;

		struct TextureGenerationModes
		{
			enum _
			{
				NONE=0,
				LINEAR=1,
				EYE_LINEAR=2,
				SPHERE=3,
				CUBE=4
			};
		};
		typedef TextureGenerationModes::_ TextureGenerationMode;
		
		struct TextureGenerationModeSet
		{
			struct TextureGenerationData
			{
				TextureGenerationData(){}
				virtual ~TextureGenerationData(){}
			};
			struct PlaneData : public TextureGenerationData
			{
				PlaneData() : a(0), b(0), c(1.0f), d(0){}
				~PlaneData(){}
				float a;
				float b;
				float c;
				float d;
			};			
			TextureGenerationModeSet() : mMode(TextureGenerationModes::NONE){}
			~TextureGenerationModeSet(){}
			TextureGenerationMode mMode;
			PlaneData mQPlaneData;
			PlaneData mRPlaneData;
			PlaneData mSPlaneData;
			PlaneData mTPlaneData;
		};

		struct TextureFilters
		{
			enum _
			{
				NEAREST=0,
				LINEAR=1
			};
		};
		typedef TextureFilters::_ TextureFilter;

		TextureUnit();
		~TextureUnit();

		bool HasTexture() const {return !!mTexture;}

		bool IsCubeMap() const {return bool(mCubeMap);}
		void SetCubeMap(shared_ptr<CubeMapTexture> cubeMap) { mCubeMap = cubeMap;}
		shared_ptr<CubeMapTexture> GetCubeMap() const { return mCubeMap;}
		
		shared_ptr<Texture> GetTexture() const {return mTexture;}
		size_t GetTextureUnitSetToUse() const {return mTextureUnitSetToUse;}
		const LayerBlendModeEx& GetBlendMode() const { return mBlendMode; }
		const LayerBlendModeEx& GetAlphaBlendMode() const { return mAlphaBlendMode; }
		const TextureWrapMode& GetWrapModeU() const { return mWrapModeU; }
		const TextureWrapMode& GetWrapModeV() const { return mWrapModeV; }
		const TextureFilter& GetMinFilter() const { return mMinFilter; }
		const TextureFilter& GetMagFilter() const { return mMagFilter; }
		const Matrix4& GetTextureMatrix() const {return mTextureMatrix;}
		const TextureGenerationModeSet& GetTexGenMode() const {return mTexGenMode;}
		Vector3 GetRotationSpeed() const {return mRotationSpeed;}
		f32 GetVScaleSpeed() const {return mVScaleSpeed;}
		f32 GetUScaleSpeed() const {return mUScaleSpeed;}
		f32 GetVScrollSpeed() const {return mVScrollSpeed;}
		f32 GetUScrollSpeed() const { return mUScrollSpeed;}
		
		void SetTexture(shared_ptr<Texture> texture){mTexture=texture;}
		void SetTextureUnitSetToUse(size_t textureUnitSetToUse) {mTextureUnitSetToUse = textureUnitSetToUse;}
		void SetTextureMatrix(const Matrix4& m) {mTextureMatrix=m;}
		/**
		 * Set blend mode using basic blend modes.
		 * These are expanded to layer blend modes automatically.
		 * @param val
		 */
		void SetBlendMode(const BlendMode& val);
		void SetBlendMode(const LayerBlendModeEx& val) { mBlendMode = val; }
		void SetAlphaBlendMode(const LayerBlendModeEx& val) { mAlphaBlendMode = val; }
		void SetWrapModeU(const TextureWrapMode& val) { mWrapModeU = val; }
		void SetWrapModeV(const TextureWrapMode& val) { mWrapModeV = val; }
		void SetWrapMode(const TextureWrapMode& val)
		{
			SetWrapModeU(val);
			SetWrapModeV(val);
		}
		void SetMinFilter(const TextureFilter& val) { mMinFilter = val; }
		void SetMagFilter(const TextureFilter& val) { mMagFilter = val; }

		void SetFilter(const TextureFilter& val)
		{
			SetMinFilter(val);
			SetMagFilter(val);
		}
		void SetTexGenMode(TextureGenerationModeSet texGenMode) {mTexGenMode=texGenMode;}
		void SetRotationSpeed(Vector3 rotationSpeed) {mRotationSpeed = rotationSpeed;}
		void SetVScaleSpeed(f32 vScaleSpeed){mVScaleSpeed = vScaleSpeed;}
		void SetUScaleSpeed(f32 uScaleSpeed){mUScaleSpeed = uScaleSpeed;}
		void SetVScrollSpeed(f32 vScrollSpeed) {mVScrollSpeed = vScrollSpeed;}
		void SetUScrollSpeed(f32 uScrollSpeed) {mUScrollSpeed = uScrollSpeed;}

	private:
		shared_ptr<Texture> mTexture;
		shared_ptr<CubeMapTexture> mCubeMap;
		LayerBlendModeEx mBlendMode;			//These blend modes aren't used if a fragment shader is used.
		LayerBlendModeEx mAlphaBlendMode;
		TextureWrapMode mWrapModeU;
		TextureWrapMode mWrapModeV;
		TextureFilter mMinFilter;
		TextureFilter mMagFilter;
		TextureGenerationModeSet mTexGenMode;
		size_t mTextureUnitSetToUse;
		bool mUseTransformForStipAnimation;
		Matrix4 mTextureMatrix;
		f32 mUScrollSpeed;
		f32 mVScrollSpeed;
		f32 mUScaleSpeed;
		f32 mVScaleSpeed;
		Vector3 mRotationSpeed;
	};
}
#endif
