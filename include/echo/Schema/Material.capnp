@0xaee48dd2ff328ee4;
using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("Echo::Resources");

struct Material
{
	struct Colour
	{
		r @0 :Float32 = 1.0;
		g @1 :Float32 = 1.0;
		b @2 :Float32 = 1.0;
		a @3 :Float32 = 1.0;
		s @4 :Text = "#FFFFFFFF";
	}

	enum BlendMode
	{
		notSet @0;
		none @1;
		transparent @2;
		additive @3;
		additiveColour @4;
		
		# These are additional presets for texture units.
		modulate @5;
		modulateAndBrighten @6;
		modulateAndBrightenMore @7;
		subtract @8;
	}

	enum MemoryMode
	{
		none @0;		# Use the system configured mode.
		fastReload @1;	# Attempts to enable fast resource reload at the cost of higher memory usage.
		lowMemory @2;	# Attempts to reduce the memory usage by reloading resources from the original source at the cost of slower reload.
	}

	struct Blend
	{
		enum BlendSource
		{
			notSet @0;
			current @1;
			texture @2;
			diffuse @3;
			specular @4;
			manual @5;
		}
		enum BlendOperation
		{
			notSet @0;
			add @1;
			addSigned @2;
			addSmooth @3;
			modulate @4;
			modulateX2 @5;
			modulateX4 @6; 
			subtract @7;
			source1 @8;
			source2 @9;
			blendDiffuseAlpha @10;
			blendTextureAlpha @11;
			blendCurrentAlpha @12;
			blendDiffuseColour @13;
			manual @14;
			blendDotProduct @15;
		}
		source1 @0 :BlendSource;
		source2 @1 :BlendSource;
		operation @2 :BlendOperation;
	}
	enum TextureWrapMode
	{
		notSet @0;
		clamp @1;
		clampToEdge @2;
		repeat @3;
	}
	enum TextureFilter
	{
		notSet @0;
		nearest @1;
		linear @2;
	}
	struct TextureGeneration
	{
		struct Plane
		{
			a @0 :Float32;
			b @1 :Float32;
			c @2 :Float32;
			d @3 :Float32;
		}		
		
		struct CubeMap
		{
			posX @0 :Text;
			posY @1 :Text;
			posZ @2 :Text;
			negX @3 :Text;
			negY @4 :Text;
			negZ @5 :Text;
		}
		enum Mode
		{
			notSet @0;
			none @1;
			linear @2;
			eyeLinear @3;
			sphere @4;
			cube @5;
		}
		mode @0 :Mode;
		qPlane @1 :Plane;
		rPlane @2 :Plane;
		sPlane @3 :Plane;
		tPlane @4 :Plane;
		cubeMap @5 :CubeMap;
	}
	enum DepthFunction
	{
		notSet @0;
		never @1;
		less @2;
		equal @3;
		lessOrEqual @4;
		greater @5;
		notEqual @6;
		greaterOrEqual @7;
		always @8;
	}
	enum AlphaTestFunction
	{
		notSet @0;
		never @1;
		less @2;
		equal @3;
		lessOrEqual @4;
		greater @5;
		notEqual @6;
		greaterOrEqual @7;
		always @8;
	}
	enum CullMode
	{
		notSet @0;
		none @1;
		front @2;
		back @3;
	}
	struct StripFrame
	{
		u @0 :Float32;
		v @1 :Float32;
		frameTime @2 :Float32;
	}
	struct AutoStripAnimation
	{
		startFrame @0 :UInt32;
		frameCount @1 :UInt32;
		frameWidthPercent @2 :Float32;
		frameHeightPercent @3 :Float32;
		frameTime @4 :Float32;
	}
	struct StripAnimation
	{
		frame @0 :List(StripFrame);
		loop @1 :Bool;
		randomise @2 :Bool;
		useTransform @3 :Bool;
		autoFrames @4 :AutoStripAnimation;
	}
	struct CubeMap
	{
		name @0 :Text;
		front @1 :Text;
		back @2 :Text;
		left @3 :Text;
		right @4 :Text;
		top @5 :Text;
		bottom @6 :Text;
	}
	struct TextureUnit
	{
		blendMode @0 :BlendMode = modulate;
		wrapU @1 :TextureWrapMode;
		wrapV @2 :TextureWrapMode;
		minFilter @3 :TextureFilter;
		magFilter @4 :TextureFilter;
		image @5 :Text;
		texGen @6 :TextureGeneration;
		uScroll @7 :Float32 = 0;
		vScroll @8 :Float32 = 0;
		uScale @9 :Float32 = 0;
		vScale @10 :Float32 = 0;
		xRot @11 :Float32 = 0;
		yRot @12 :Float32 = 0;
		zRot @13 :Float32 = 0;
		stripAnimation @14 :StripAnimation;
		alphaBlend @15 :Blend;
		colourBlend @16 :Blend;
		memoryMode @17 :MemoryMode;
		textureCoordinateSet @18 :UInt32 = 0;
		manualColour @19 :Colour;
		cubemap @20 :CubeMap;
	}
	struct ShaderProgram
	{
		geometryShader @0 :List(Text);
		vertexShader @1 :List(Text);
		fragmentShader @2 :List(Text);
		variable @3 :List(Text);
		matrixVariable @4 :Text;
		cameraPositionVariable @5 :Text;
		worldMatrix @6 :Text;
		viewMatrix @7 :Text;
		projectionMatrix @8 :Text;
	}
	struct Pass
	{
		blendMode @0 :BlendMode;
		depthFunction @1 :DepthFunction;
		cullMode @2 :CullMode;
		lighting @3 :Bool = false;
		depthWrite @4 :Bool = true;
		depthCheck @5 :Bool = true;
		ambient @6 :Colour;
		diffuse @7 :Colour;
		specular @8 :Colour;
		emissive @9 :Colour;
		shininess @10 :Float32 = 0;
		textureUnit @11 :List(TextureUnit);
		vertexColour @12 :Bool = false;
		active @13 :Bool = true;
		program @14 :ShaderProgram;
		alphaTestFunction @15 :AlphaTestFunction;
		alphaTestValue @16 :Float32;
		pointAndLineSize @17 :Float32 = 1;
	}
	pass @0 :List(Pass);
	name @1 :Text;
}

struct Materials
{
	material @0 :List(Material);
}
