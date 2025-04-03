#include <echo/Graphics/Light.h>

namespace Echo
{
	Light::Light() :
		mLightType(LightTypes::POINT),
		mDiffuse(Colour()),
		mSpecular(Colour(0, 0, 0, 1.0f)),
		mSpotOuter(Degree(40.0f)),
		mSpotInner(Degree(30.0f)),
		mSpotFalloff(1.0f),
		mRange(100000),
		mAttenuationConst(1.0f),
		mAttenuationLinear(0.0f),
		mAttenuationQuad(0.0f),
		mPowerScale(1.0f),
		mOwnShadowFarDist(false),
		mShadowFarDist(0),
		mShadowFarDistSquared(0),
		mShadowNearClipDist(-1),
		mShadowFarClipDist(-1)
	{ }

	Light::Light(const std::string& name) :
		mLightType(LightTypes::POINT),
		mDiffuse(Colour()),
		mSpecular(Colour(0.0f, 0.0f, 0.0f, 1.0f)),
		mSpotOuter(Degree(40.0f)),
		mSpotInner(Degree(30.0f)),
		mSpotFalloff(1.0f),
		mRange(100000),
		mAttenuationConst(1.0f),
		mAttenuationLinear(0.0f),
		mAttenuationQuad(0.0f),
		mPowerScale(1.0f),
		mOwnShadowFarDist(false),
		mShadowFarDist(0),
		mShadowFarDistSquared(0),
		mShadowNearClipDist(-1),
		mShadowFarClipDist(-1)
	{ }
	//-----------------------------------------------------------------------

	Light::~Light() { }
	//-----------------------------------------------------------------------

	void Light::SetType(LightType type)
	{
		mLightType = type;
	}
	//-----------------------------------------------------------------------

	Light::LightType Light::GetType(void) const
	{
		return mLightType;
	}

	void Light::SetSpotlightRange(const Radian& innerAngle, const Radian& outerAngle, f32 falloff)
	{
		assert(mLightType == LightTypes::SPOTLIGHT);
		mSpotInner = innerAngle;
		mSpotOuter = outerAngle;
		mSpotFalloff = falloff;
	}
	//-----------------------------------------------------------------------

	void Light::SetSpotlightInnerAngle(const Radian& val)
	{
		mSpotInner = val;
	}
	//-----------------------------------------------------------------------

	void Light::SetSpotlightOuterAngle(const Radian& val)
	{
		mSpotOuter = val;
	}
	//-----------------------------------------------------------------------

	void Light::SetSpotlightFalloff(f32 val)
	{
		mSpotFalloff = val;
	}
	//-----------------------------------------------------------------------

	const Radian& Light::GetSpotlightInnerAngle(void) const
	{
		return mSpotInner;
	}
	//-----------------------------------------------------------------------

	const Radian& Light::GetSpotlightOuterAngle(void) const
	{
		return mSpotOuter;
	}
	//-----------------------------------------------------------------------

	f32 Light::GetSpotlightFalloff(void) const
	{
		return mSpotFalloff;
	}
	//-----------------------------------------------------------------------

	void Light::SetDiffuseColour(f32 red, f32 green, f32 blue)
	{
		mDiffuse.mRed = red;
		mDiffuse.mBlue = blue;
		mDiffuse.mGreen = green;
	}
	//-----------------------------------------------------------------------

	void Light::SetDiffuseColour(const Colour& colour)
	{
		mDiffuse = colour;
	}
	//-----------------------------------------------------------------------

	const Colour& Light::GetDiffuseColour(void) const
	{
		return mDiffuse;
	}
	//-----------------------------------------------------------------------

	void Light::SetSpecularColour(f32 red, f32 green, f32 blue)
	{
		mSpecular.mRed = red;
		mSpecular.mBlue = blue;
		mSpecular.mGreen = green;
	}
	//-----------------------------------------------------------------------

	void Light::SetSpecularColour(const Colour& colour)
	{
		mSpecular = colour;
	}
	//-----------------------------------------------------------------------

	const Colour& Light::GetSpecularColour(void) const
	{
		return mSpecular;
	}
	//-----------------------------------------------------------------------

	void Light::SetAttenuation(f32 range, f32 constant,
								f32 linear, f32 quadratic)
	{
		mRange = range;
		mAttenuationConst = constant;
		mAttenuationLinear = linear;
		mAttenuationQuad = quadratic;
	}
	//-----------------------------------------------------------------------

	f32 Light::GetAttenuationRange(void) const
	{
		return mRange;
	}
	//-----------------------------------------------------------------------

	f32 Light::GetAttenuationConstant(void) const
	{
		return mAttenuationConst;
	}
	//-----------------------------------------------------------------------

	f32 Light::GetAttenuationLinear(void) const
	{
		return mAttenuationLinear;
	}
	//-----------------------------------------------------------------------

	f32 Light::GetAttenuationQuadric(void) const
	{
		return mAttenuationQuad;
	}
	//-----------------------------------------------------------------------

	void Light::SetPowerScale(f32 power)
	{
		mPowerScale = power;
	}
	//-----------------------------------------------------------------------

	f32 Light::GetPowerScale(void) const
	{
		return mPowerScale;
	}
}
