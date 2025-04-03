#ifndef _ECHOLIGHT_H__
#define _ECHOLIGHT_H__

#include <echo/Types.h>
#include <echo/Graphics/Colour.h>
#include <echo/Graphics/Node.h>
#include <echo/Maths/Vector4.h>
#include <echo/Maths/PlaneBoundedVolume.h>
#include <map>

namespace Echo
{
	class Camera;

	/** Representation of a dynamic light source in the scene.
		@remarks
			Lights are added to the scene like any other object. They contain various
			parameters like type, position, attenuation (how light intensity fades with
			distance), colour etc.
		@par
			The defaults when a light is created is pure white diffuse light, with no
			attenuation (does not decrease with distance) and a range of 1000 world units.
		@par
			Lights are created by using the Scene::CreateLight method. They can subsequently be
			added to a SceneNode if required to allow them to move relative to a node in the scene. A light attached
			to a SceneNode is assumed to have a base position of (0,0,0) and a direction of (0,0,1) before modification
			by the SceneNode's own orientation. If not attached to a SceneNode,
			the light's position and direction is as set using setPosition and SetDirection.
		@par
			Remember also that dynamic lights rely on modifying the colour of vertices based on the position of
			the light compared to an object's vertex normals. Dynamic lighting will only look good if the
			object being lit has a fair level of tessellation and the normals are properly set. This is particularly
			true for the spotlight which will only look right on highly tessellated models.
	*/
	class Light : public Node
	{
	public:
		/// Defines the type of light
		struct LightTypes
		{
			enum _
			{
				// Point light sources give off light equally in all directions, so require only position not direction
				POINT = 0,
				// Directional lights simulate parallel light beams from a distant source, hence have direction but no position
				DIRECTIONAL = 1,
				//	Spotlights simulate a cone of light from a source so require position and direction, plus extra values for falloff
				SPOTLIGHT = 2
			};
		};
		typedef LightTypes::_ LightType;

		//!Default constructor
		Light();

		//!Constructor
		Light(const std::string& name);

		//!Standard destructor.
		virtual ~Light();

		//!Sets the type of light - see LightTypes for more info.
		void SetType(LightType type);

		//!Returns the light type.
		LightType GetType(void) const;

		//!Sets the colour of the diffuse light given off by this source.
		//!Material objects have ambient, diffuse and specular values which indicate how much of each type of
		//			light an object reflects. This value denotes the amount and colour of this type of light the light
		//			exudes into the scene. The actual appearance of objects is a combination of the two.
		//
		//			Diffuse light simulates the typical light emanating from light sources and affects the base colour
		//			of objects together with ambient light.
		void SetDiffuseColour(f32 red, f32 green, f32 blue);

		//!Sets the colour of the diffuse light given off by this source.
		//!Material objects have ambient, diffuse and specular values which indicate how much of each type of
		//			light an object reflects. This value denotes the amount and colour of this type of light the light
		//			exudes into the scene. The actual appearance of objects is a combination of the two.
		//
		//			Diffuse light simulates the typical light emanating from light sources and affects the base colour
		//			of objects together with ambient light.
		void SetDiffuseColour(const Colour& colour);

		//!Returns the colour of the diffuse light given off by this light source (see SetDiffuseColour for more info).
		const Colour& GetDiffuseColour(void) const;

		//!Sets the colour of the specular light given off by this source.
		//!Material objects have ambient, diffuse and specular values which indicate how much of each type of
		//!			light an object reflects. This value denotes the amount and colour of this type of light the light
		//!			exudes into the scene. The actual appearance of objects is a combination of the two.
		//!
		//!			Specular light affects the appearance of shiny highlights on objects, and is also dependent on the
		//!			'shininess' Material value.
		void SetSpecularColour(f32 red, f32 green, f32 blue);

		//!Sets the colour of the specular light given off by this source.
		//!Material objects have ambient, diffuse and specular values which indicate how much of each type of
		//!			light an object reflects. This value denotes the amount and colour of this type of light the light
		//!			exudes into the scene. The actual appearance of objects is a combination of the two.
		//!
		//!			Specular light affects the appearance of shiny highlights on objects, and is also dependent on the
		//!			'shininess' Material value.
		void SetSpecularColour(const Colour& colour);

		//!Returns the colour of specular light given off by this light source.
		const Colour& GetSpecularColour(void) const;

		//!Sets the attenuation parameters of the light source i.e. how it diminishes with distance.
		//!Lights normally get fainter the further they are away. Also, each light is given a maximum range
		//!		beyond which it cannot affect any objects.
		//!
		//!		Light attenuation is not applicable to directional lights since they have an infinite range and
		//!		constant intensity.
		//!
		//!		This follows a standard attenuation approach - see any good 3D text for the details of what they mean
		//!		since i don't have room here!
		//!param range		The absolute upper range of the light in world units
		//!param constant	The constant factor in the attenuation formula: 1.0 means never attenuate, 0.0 is complete attenuation
		//!param linear	The linear factor in the attenuation formula: 1 means attenuate evenly over the distance
		//!paramquadratic	The quadratic factor in the attenuation formula: adds a curvature to the attenuation formula.
		void SetAttenuation(f32 range, f32 constant, f32 linear, f32 quadratic);

		//!Returns the absolute upper range of the light.
		f32 GetAttenuationRange(void) const;

		//!Returns the constant factor in the attenuation formula.
		f32 GetAttenuationConstant(void) const;

		//!Returns the linear factor in the attenuation formula.
		f32 GetAttenuationLinear(void) const;

		//!Returns the quadric factor in the attenuation formula.
		f32 GetAttenuationQuadric(void) const;

		/** Sets the range of a spotlight, i.e. the angle of the inner and outer cones and the rate of falloff between them.
			@param
				innerAngle Angle covered by the bright inner cone
				@node
					The inner cone applicable only to Direct3D, it'll always treat as zero in OpenGL.
			@param
				outerAngle Angle covered by the outer cone
			@param
				falloff The rate of falloff between the inner and outer cones. 1.0 means a linear falloff, less means slower falloff, higher means faster falloff.
		*/
		void SetSpotlightRange(const Radian& innerAngle, const Radian& outerAngle, f32 falloff = 1.0);

		/** Returns the angle covered by the spotlights inner cone.
		*/
		const Radian& GetSpotlightInnerAngle(void) const;

		/** Returns the angle covered by the spotlights outer cone.
		*/
		const Radian& GetSpotlightOuterAngle(void) const;

		/** Returns the falloff between the inner and outer cones of the spotlight.
		*/
		f32 GetSpotlightFalloff(void) const;

		/** Sets the angle covered by the spotlights inner cone.
		*/
		void SetSpotlightInnerAngle(const Radian& val);

		/** Sets the angle covered by the spotlights outer cone.
		*/
		void SetSpotlightOuterAngle(const Radian& val);

		/** Sets the falloff between the inner and outer cones of the spotlight.
		*/
		void SetSpotlightFalloff(f32 val);

		/** Set a scaling factor to indicate the relative power of a light.
		@remarks
			This factor is only useful in High Dynamic Range (HDR) rendering.
			You can bind it to a shader variable to take it into account,
			@see GpuProgramParameters
		@param power The power rating of this light, default is 1.0.
		*/
		void SetPowerScale(f32 power);

		/** Set the scaling factor which indicates the relative power of a 
			light.
		*/
		f32 GetPowerScale(void) const;
	protected:
		LightType mLightType;
		Colour mDiffuse;
		Colour mSpecular;

		Radian mSpotOuter;
		Radian mSpotInner;
		f32 mSpotFalloff;
		f32 mRange;
		f32 mAttenuationConst;
		f32 mAttenuationLinear;
		f32 mAttenuationQuad;
		f32 mPowerScale;
		bool mOwnShadowFarDist;
		f32 mShadowFarDist;
		f32 mShadowFarDistSquared;
		
		f32 mShadowNearClipDist;
		f32 mShadowFarClipDist;

		mutable PlaneBoundedVolume mNearClipVolume;
		mutable PlaneBoundedVolumeList mFrustumClipVolumes;
	};
}
#endif
