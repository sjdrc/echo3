@0x9c15fbfd7c25ca05;
using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("Echo::Resources");
using import "Material.capnp".Material;

struct Font
{
	struct CharacterRange
	{
		# The values should be in decimal. So U+2203 (∃) would be 8707 (hexidecimal to decimal conversion).
		# You may find http:#www.utf8-chartable.de useful for finding out code points.
		lowerCode @0 :UInt32;	# lower code of a range to include in a map (code is included) 
		upperCode @1 :UInt32;	# upper code of a range to include in a map (code is included) 
	}
	name @0 :Text;					# Name of the resource
	filename @1 :Text;				# The ttf file to load
	pointSize @2 :Int32;			# The default point size
	range @3 :List(CharacterRange);	# Character ranges to include in the character map.
	characters @4 :List(Text);		# UTF8 Characters to include in the character map (can be used instead of or as well as range)
	xDPI @5 :Int32;					# default x DPI used when rendering fonts (if values aren't specified via another mechanism).
	yDPI @6 :Int32;					# default y DPI used when rendering fonts (if values aren't specified via another mechanism).
	colour @7 :Material.Colour;							# The default colour of the font.
	textureMinFilter @8 :Material.TextureFilter;		# The default min texture filter.
	textureMagFilter @9 :Material.TextureFilter;		# The default max texture filter.
	replacementCharacter @10 :Int32 = 0xFFFD;			# Defaults to 0xFFFD
	loadReplacementCharacter @11 :Bool = true;			# Defaults to true, setting this to false can save some texture space.
	forceUseReplacementCharacter @12 :Bool = false;		# Defaults to false.
	warnOnReplacementCharacterUsage @13 :Bool = false;	# Defaults to false

	
	# Specifying a material overrides the a default material. If the other material properties above
	# are defined they will override the values in this material.
	material @14 :Material;
}

struct Fonts
{
	fonts @0 :List(Font);
}
