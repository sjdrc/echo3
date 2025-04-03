@0xc2fd36a8db0d443e;
using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("Echo::Resources");

enum SizeMode
{
	stretch @0;								# The set percent size will be used.
	fill @1;								# Fill the parent element using the aspect ratio of the element content. If the aspect ratio differs
											# from the parent then the element will overflow on which ever axis is needed to fill the parent.
	widthAndCalculateAspectRatio @2;		# Use the width and calculate the height based on the aspect ratio of the element content.
	heightAndCalculateAspectRatio @3;		# Use the height and calculate the width based on the aspect ratio of the element content.
	widthWithSetAspectRatio @4;				# Use the width and calculate height using the set aspect ratio to calculate the height based on the specified values.
	heightWithSetAspectRatio @5;			# Use the height and calculate width using the set aspect ratio to calculate the width based on the specified values.
	fillWithSetAspectRatio @6;				# Fill the parent element using the set aspect ratio of the element content. If the aspect ratio differs
											# from the parent then the element will overflow on which ever axis is needed to fill the parent.
	maxFitWithSetAspectRatio @7;			# Fit the Element to the parent on the smallest axis using the set aspect ratio to calculate the other axis.
}

enum Alignment
{
	centre @0;
	minimum @1;
	maximum @2;
}

enum ElementType
{
	container @0;
	image @1;
	text @2;
	button @3;
	layout @4;
	textbox @5;
}

struct ImageProperties
{
	material @0 :Text;
	flipHorizontally @1 :Bool = false;
	flipVertically @2 :Bool = false;
	# If you want to use only part of a texture for the image.
	# Minimum is top left, maximum is bottom right.
	minU @3 :Float32 = 0;
	minV @4 :Float32 = 0;
	maxU @5 :Float32 = 1;
	maxV @6 :Float32 = 1;
}

struct TextProperties
{
	text @0 :Text;
	font @1 :Text;
	pointSize @2 :Float32 = 0;
}

struct LayoutProperties
{
	enum Direction
	{
		notSet @0;
		horizontalMinToMax @1;	# Place elements horizontally from the minimum to maximum in the order they were added.
		horizontalMaxToMin @2;	# Place elements horizontally from the maximum to minimum in the order they were added.
		verticalMinToMax @3;	# Place elements vertically from the minimum to maximum in the order they were added.
		verticalMaxToMin @4;	# Place elements vertically from the maximum to minimum in the order they were added.
	}
	enum PlacementMode
	{
		notSet @0;
		fitContents @1;			# Contents will be scaled to fit the layout size, some child size modes have unspecified* behaviour,
								# For example: Horizontal directions do not support height first child element size modes as these
								# modes expect to be able to determine the width but the horizontal placement needs to be able to
								# scale by the width of the child element, which in doing so would break the height first rule.
								# Children using the max fit size modes 
		allowOverflow @2;		# Contents will be positioned and are allowed to be positioned outside of the layout's bounds. All size
								# modes are supported.
	}
	direction @0 :Direction;
 	placementMode @1 :PlacementMode;
	useOptimalSettings @2 :Bool = true;
}

struct TextBoxProperties
{
	text @0 :Text;
	font @1 :Text;
	lineSize @2 :Float32 = 1;
	pointSize @3 :Float32 = 0;
}

struct ButtonProperties
{
	idleMaterial @0 :Text;
	pressedMaterial @1 :Text;
	releasedMaterial @2 :Text;
	hoverMaterial @3 :Text;
}

struct Conditional
{
	condition @0 :Text;
	function @1 :List(Text);
}

struct Element
{
	type @0 :ElementType;
	width @1 :Float32 = 1;
	height @2 :Float32 = 1;
	aspectRatio @3 :Float32 = 0;
	x @4 :Float32 = 0.5;
	y @5 :Float32 = 0.5;
	z @6 :Float32 = 0.01;
	name @7 :Text;
	targetable @8 :Bool = true;
	sizeMode @9 :SizeMode = stretch;
	hAlign @10 :Alignment = centre;
	vAlign @11 :Alignment = centre;

	enterFunction @12 :List(Text);
	hoverFunction @13 :List(Text);
	leaveFunction @14 :List(Text);
	pressedFunction @15 :List(Text);
	holdFunction @16 :List(Text);
	releasedFunction @17 :List(Text);
	imageProperties @18 :ImageProperties;
	textProperties @19 :TextProperties;
	buttonProperties @20 :ButtonProperties;
	layoutProperties @21 :LayoutProperties;
	textboxProperties @22 :TextBoxProperties;
	element @23 :List(Element);
	guiFile @24 :List(Text);
	visible @25 :Bool = true;
	physicalWidth @26 :Text;
	physicalHeight @27 :Text;
	colour @28 :Text;
	inheritParentColour @29 :Bool = true;
	inheritParentAlpha @30 :Bool = true;
	property @31 :List(Text);
	conditional @32 :List(Conditional);
}

struct Cursor
{
 	element @0 :Element;
	xAxis @1 :Text;
	yAxis @2 :Text;
	button @3 :Text;
}

#See code documentation for the Viewport parameters.
struct Viewport
{
	left @0 :Float32 = 0;
	top @1 :Float32 = 0;
	right @2 :Float32 = 1;
	bottom @3 :Float32 = 1;
	mode @4 :SizeMode = stretch;
	displayAspectRatio @5 :Float32 = 0;
	verticalCentre @6 :Float32 = 0.5;
	horizontalCentre @7 :Float32 = 0.5;
}

#.gui files only contain elements and/or other gui files.
struct GUI
{
	element @0 :List(Element);
	guiFile @1 :List(Text);
}

#.screen files are similar to .gui files but contain other information for setting up a full GUI Scene.
struct Screen
{
	cursor @0 :Cursor;
	viewport @1 :Viewport;
	element @2 :List(Element);
	guiFile @3 :List(Text);
	conditional @4 :List(Conditional);
}
