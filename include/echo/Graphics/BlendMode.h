#ifndef _ECHOBLENDMODE_H_
#define _ECHOBLENDMODE_H_

#include <echo/Graphics/Colour.h>

//Because, Microsoft! =(
#ifdef TRANSPARENT
#undef TRANSPARENT
#endif

namespace Echo
{
	/**
	 * Simple blend modes.
	 * These are used to define how objects are blended with the scene but can also be used as presets
	 * for texture blend stages.
	 */
	struct BlendModes
	{
		enum BlendModeEnum
		{
			NONE = 0,
			TRANSPARENT = 1,
			ADDITIVE = 2,
			ADDITIVE_COLOUR = 3,
			
			//These blend modes aren't relevant to scene blending. They are additional presets for texture blending.
			MODULATE = 4,
			MODULATE_AND_BRIGHTEN = 5,
			MODULATE_AND_BRIGHTEN_MORE = 6,
			SUBTRACT = 7,
		};
	};
	typedef BlendModes::BlendModeEnum BlendMode;	
	
	/** Type of texture blend modes.
	*/
	struct LayerBlendTypes
	{
		enum _
		{
			COLOUR,
			ALPHA
		};
	};
	typedef LayerBlendTypes::_ LayerBlendType;
	
	/** List of valid texture blending operations, for use with TextureUnitState::setColourOperation.
		@remarks
			This list is a more limited list than LayerBlendOperationEx because it only
			includes operations that are supportable in both multipass and multitexture
			rendering and thus provides automatic fallback if multitexture hardware
			is lacking or insufficient.
	*/
	struct LayerBlendOperations
	{
		enum _
		{
			/// Replace all colour with texture with no adjustment
			REPLACE,
			/// Add colour components together.
			ADD,
			/// Multiply colour components together.
			MODULATE,
			/// Blend based on texture alpha
			ALPHA_BLEND
		};
	};
	typedef LayerBlendOperations::_ LayerBlendOperation;

	/** Expert list of valid texture blending operations, for use with TextureUnitState::setColourOperationEx
		and TextureUnitState::setAlphaOperation, and internally in the LayerBlendModeEx class. It's worth
		noting that these operations are for blending <i>between texture layers</i> and not between rendered objects
		and the existing scene. Because all of these modes are only supported in multitexture hardware it may be
		required to set up a fallback operation where this hardware is not available.
	*/
	
	struct LayerBlendOperationExs
	{
		enum _
		{
			/// use source1 without modification
			SOURCE1,
			/// use source2 without modification
			SOURCE2,
			/// multiply source1 and source2 together
			MODULATE,
			/// as MODULATE but brighten afterwards (x2)
			MODULATE_X2,
			/// as MODULATE but brighten more afterwards (x4)
			MODULATE_X4,
			/// add source1 and source2 together
			ADD,
			/// as ADD, but subtract 0.5 from the result
			ADD_SIGNED,
			/// as ADD, but subtract product from the sum
			ADD_SMOOTH,
			/// subtract source2 from source1
			SUBTRACT,
			/// use interpolated alpha value from vertices to scale source1, then add source2 scaled by (1-alpha)
			BLEND_DIFFUSE_ALPHA,
			/// as BLEND_DIFFUSE_ALPHA, but use alpha from texture
			BLEND_TEXTURE_ALPHA,
			/// as BLEND_DIFFUSE_ALPHA, but use current alpha from previous stages
			BLEND_CURRENT_ALPHA,
			/// as BLEND_DIFFUSE_ALPHA but use a constant manual blend value (0.0-1.0)
			BLEND_MANUAL,
			/// dot product of colour1 and colour2 
			DOTPRODUCT,
			/// use interpolated colour values from vertices to scale source1, then add source2 scaled by (1-colour)
			BLEND_DIFFUSE_COLOUR
		};
	};
	typedef LayerBlendOperationExs::_ LayerBlendOperationEx;
	
	/** List of valid sources of values for blending operations used
		in TextureUnitState::setColourOperation and TextureUnitState::setAlphaOperation,
		and internally in the LayerBlendModeEx class.
	*/
	struct LayerBlendSources
	{
		enum _
		{
			/// the colour as built up from previous stages
			CURRENT,
			/// the colour derived from the texture assigned to this layer
			TEXTURE,
			/// the interpolated diffuse colour from the vertices
			DIFFUSE,
			/// the interpolated specular colour from the vertices
			SPECULAR,
			/// a colour supplied manually as a separate argument
			MANUAL
		};
	};
	typedef LayerBlendSources::_ LayerBlendSource;
	/** Class which manages blending of both colour and alpha components.
		@remarks
			This class is a utility class used by both TextureUnitState and
			RenderSystem to wrap up the details of a blending operation. This blending
			operation could be used for blending colour or alpha in a texture layer.
			This class is really only for use by OGRE, since apps can deal with
			blending modes through the TextureUnitState class methods
			setColourOperation and setAlphaOperation.
		@par
			It's worth noting that these operations are for blending <i>between texture
			layers</i> and not between rendered objects and the existing scene. If
			you wish to make an object blend with others in the scene, e.g. to make
			transparent objects etc, use the Material::setSceneBlending method.
	*/
	class LayerBlendModeEx
	{
	public:
		/// The type of blending (colour or alpha)
		LayerBlendType blendType;
		/// The operation to be applied
		LayerBlendOperationEx operation;
		/// The first source of colour/alpha
		LayerBlendSource source1;
		/// The second source of colour/alpha
		LayerBlendSource source2;

		/// Manual colour value when using manual source.
		Colour colour;

		LayerBlendModeEx()
		{
			operation = LayerBlendOperationExs::SOURCE1;
			source1 = LayerBlendSources::TEXTURE;
			source2 = LayerBlendSources::CURRENT;
			colour = Colour();
		}

		bool operator==(const LayerBlendModeEx& rhs) const
		{
			if(blendType != rhs.blendType ||
				operation != rhs.operation ||
				source1 != rhs.source1 ||
				source2 != rhs.source2 ||
				colour != rhs.colour)
			{
				return false;
			}
			return true;
		}

		bool operator!=(const LayerBlendModeEx& rhs) const
		{
			return !(*this == rhs);
		}
	};
}
#endif
