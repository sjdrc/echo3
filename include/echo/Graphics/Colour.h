#ifndef _ECHOCOLOUR_H_
#define _ECHOCOLOUR_H_

#include <echo/Types.h>
#include <echo/Maths/Vector4.h>
#include <boost/algorithm/clamp.hpp>
#ifdef ECHO_OGRE_SUPPORT
#include <OgreColourValue.h>
#endif

namespace Echo
{
	union VertexColour
	{
		struct _RGBA
		{
			u8 r;
			u8 g;
			u8 b;
			u8 a;
		}_RGBA;
		u32 mRGBA;
	};

	/**
	 * Colour represents a colour made up red, green, blue and alpha components.
	 * Typically each component will have a value in the range of 0-1 but this class doesn't enforce that.
	 * You can clamp a colour within this range (or any other specified range) by calling Clamp().
	 * 
	 * If you want to interpolate between two colours use
	 * 
	 * Colour interpolatedColour = Maths::LinearInterpolate<Colour>(colourA,colourB,percent);
	 * 
	 * where percent has the range of 0-1. 0 being completely colourA and 1 completely colourB.
	 */
	class Colour
	{
	public:
		f32 mRed;
		f32 mGreen;
		f32 mBlue;
		f32 mAlpha;
		/**
		 * See operator>> for possible string conversion options.
		 * If the value fails the colour will be white.
		 */
		Colour(const std::string& valueAsString);
		Colour(f32 r=1.0f, f32 g=1.0f, f32 b=1.0f, f32 a=1.0f) : mRed(r),mGreen(g),mBlue(b),mAlpha(a){}
		~Colour(){}
		Colour(const Vector4& v)
		{
			mRed = v.x;
			mGreen = v.y;
			mBlue = v.z;
			mAlpha = v.w;
		}
		Colour(const Vector3& v)
		{
			mRed=v.x;
			mGreen=v.y;
			mBlue=v.z;
			mAlpha=1.0f;
		}
		Colour(const VertexColour& v)
		{
			mRed = v._RGBA.r / 255.0f;
			mGreen = v._RGBA.g / 255.0f;
			mBlue = v._RGBA.b / 255.0f;
			mAlpha = v._RGBA.a / 255.0f;
		}
		Colour(const Colour& i)
		{
			mRed=i.mRed;
			mGreen=i.mGreen;
			mBlue=i.mBlue;
			mAlpha=i.mAlpha;
		}
		Colour& operator=(const Colour& rhs)
		{
			mRed=rhs.mRed;
			mGreen=rhs.mGreen;
			mBlue=rhs.mBlue;
			mAlpha=rhs.mAlpha;
			return *this;
		}
		
		bool operator==(const Colour& rhs) const 
		{
			return (mRed==rhs.mRed &&
					mGreen==rhs.mGreen &&
					mBlue==rhs.mBlue &&
					mAlpha==rhs.mAlpha);
		}

		bool operator!=(const Colour& rhs) const 
		{
			return (mRed!=rhs.mRed ||
					mGreen!=rhs.mGreen ||
					mBlue!=rhs.mBlue ||
					mAlpha!=rhs.mAlpha);
		}

		//Addition
		Colour operator+(const Colour& rhs)
		{
			Colour c(*this);
			c+=rhs;
			return c;
		}
		//Subtract
		Colour operator-(const Colour& rhs) const
		{
			Colour c(*this);
			c-=rhs;
			return c;
		}
		//Multiply
		Colour operator*(const Colour& rhs) const
		{
			Colour c(*this);
			c*=rhs;
			return c;
		}
		//Multiply by scalar
		Colour operator*(const f32& rhs) const
		{
			Colour c(*this);
			c.mRed*=rhs;
			c.mGreen*=rhs;
			c.mBlue*=rhs;
			c.mAlpha*=rhs;
			return c;
		}		
		//Divide
		Colour operator/(const Colour& rhs) const
		{
			Colour c(*this);
			c/=rhs;
			return c;
		}

		Colour& operator+=(const Colour& rhs)
		{
			mRed+=rhs.mRed;
			mGreen+=rhs.mGreen;
			mBlue+=rhs.mBlue;
			mAlpha+=rhs.mAlpha;
			return *this;
		}
		
		Colour& operator-=(const Colour& rhs)
		{
			mRed-=rhs.mRed;
			mGreen-=rhs.mGreen;
			mBlue-=rhs.mBlue;
			mAlpha-=rhs.mAlpha;
			return *this;
		}
		
		Colour& operator*=(const Colour& rhs)
		{
			mRed*=rhs.mRed;
			mGreen*=rhs.mGreen;
			mBlue*=rhs.mBlue;
			mAlpha*=rhs.mAlpha;
			return *this;
		}
		
		Colour& operator/=(const Colour& rhs)
		{
			mRed/=rhs.mRed;
			mGreen/=rhs.mGreen;
			mBlue/=rhs.mBlue;
			mAlpha/=rhs.mAlpha;
			return *this;
		}
		
		inline operator VertexColour() const
		{
			VertexColour v;
			v._RGBA.r=(u8)(mRed*255.0f);
			v._RGBA.g=(u8)(mGreen*255.0f);
			v._RGBA.b=(u8)(mBlue*255.0f);
			v._RGBA.a=(u8)(mAlpha*255.0f);
			return v;
		}

		inline operator Vector4() const
		{
			Vector4 v;
			v.x=mRed;
			v.y=mGreen;
			v.z=mBlue;
			v.w=mAlpha;
			return v;
		}

		inline operator Vector3() const
		{
			Vector3 v;
			v.x=mRed;
			v.y=mGreen;
			v.z=mBlue;
			return v;
		}
		
		void Set(f32 r, f32 g, f32 b, f32 a)
		{
			mRed=r;
			mGreen=g;
			mBlue=b;
			mAlpha=a;
		}
		
		inline void CopyColourComponentsFrom(const Colour& target)
		{
			mRed = target.mRed;
			mGreen = target.mGreen;
			mBlue = target.mBlue;
		}

		inline void CopyAlphaFrom(const Colour& target)
		{
			mAlpha = target.mAlpha;
		}

		inline void CopyColourComponentsTo(Colour& target) const
		{
			target.mRed = mRed;
			target.mGreen = mGreen;
			target.mBlue = mBlue;
		}

		inline void CopyAlphaTo(Colour& target) const
		{
			target.mAlpha = mAlpha;
		}
		
		/**
		 * Clamp the colour between two other colours.
		 * @param lower
		 * @param upper
		 */
		void Clamp(Colour lower=Colour(0,0,0,0), Colour upper=Colour())
		{
			using boost::algorithm::clamp;
			mRed=clamp(mRed, lower.mRed, upper.mRed);
			mGreen=clamp(mGreen, lower.mGreen, upper.mGreen);
			mBlue=clamp(mBlue, lower.mBlue, upper.mBlue);
			mAlpha=clamp(mAlpha, lower.mAlpha, upper.mAlpha);
		}

	#ifdef ECHO_OGRE_SUPPORT
		//Ogre support
		inline operator Ogre::ColourValue() const
		{
			return Ogre::ColourValue(mRed,mGreen,mBlue,mAlpha);
		}
		inline Colour& operator=(const Ogre::ColourValue& rhs)
		{
			mRed=rhs.r;
			mGreen=rhs.g;
			mBlue=rhs.b;
			mAlpha=rhs.a;
			return *this;
		}
		inline Colour(const Ogre::ColourValue& rhs)
		{
			mRed=rhs.r;
			mGreen=rhs.g;
			mBlue=rhs.b;
			mAlpha=rhs.a;
		}
	#endif
		/** Function for writing to a stream.
		*/
		inline friend std::ostream& operator <<( std::ostream& o, const Colour& c )
		{
			o << "Colour(" << c.mRed << "," << c.mGreen << "," << c.mBlue << "," << c.mAlpha << ")";
			return o;
		}
		
		/**
		 * Provides conversion of colours in streams.
		 * Supported formats are:
		 * 
		 * - R,G,B,A
		 * - Colour(R,G,B,A)
		 * - Colour8(R,G,B,A)
		 * - #RRGGBBAA
		 * 
		 * Colour(R,G,B,A) and R,G,B,A support channel ranges of supports ranges of 0-1 and
		 * 0 - 255, representing 0% to 100% in both cases.. You can mix the format you use
		 * as range detection is per channel. 
		 * Detection is based on on the channel value, if a channel has a value greater than
		 * 1 it is assumed the 0-255 range is in use.
		 * This way of detection selection means that RGBA(1,1,1,1) will be opaque white,
		 * rather than almost transparent black since the check is channel>1 and not
		 * channel>=1.
		 * 
		 * If you need 0-255 ranges and want to include 1 as an option then use Colour8 as a
		 * prefix to your string, for example Colour8(1,1,1,1) would result in 0.0039215686
		 * for each channel.
		 * 
		 * Alternatively, you can use hexadecimal format, which is commonly found in web
		 * development and graphics applications along with 0-255 values.
		 */
		friend std::istream& operator >>(std::istream& i, Colour& c);
	};

	struct Colours
	{
		static Colour Random();		//Returns an opaque random colour.
		static Colour Random(Colour lowerBound, Colour upperBound);		//Returns a random colour within the rannge.
		
		//List of colours harvested from http://en.wikipedia.org/wiki/List_of_colors
		static const Colour AIR_FORCE_BLUE;
		static const Colour ALICE_BLUE;
		static const Colour ALIZARIN_CRIMSON;
		static const Colour ALMOND;
		static const Colour AMARANTH;
		static const Colour AMBER;
		static const Colour AMERICAN_ROSE;
		static const Colour AMETHYST;
		static const Colour ANDROID_GREEN;
		static const Colour ANTIFLASH_WHITE;
		static const Colour ANTIQUE_BRASS;
		static const Colour ANTIQUE_FUCHSIA;
		static const Colour ANTIQUE_WHITE;
		static const Colour AO;
		static const Colour APPLE_GREEN;
		static const Colour APRICOT;
		static const Colour AQUA;
		static const Colour AQUAMARINE;
		static const Colour ARMY_GREEN;
		static const Colour ARSENIC;
		static const Colour ARYLIDE_YELLOW;
		static const Colour ASH_GREY;
		static const Colour ASPARAGUS;
		static const Colour ATOMIC_TANGERINE;
		static const Colour AUBURN;
		static const Colour AUREOLIN;
		static const Colour AUROMETALSAURUS;
		static const Colour AWESOME;
		static const Colour AZURE;
		static const Colour AZURE_MIST;
		static const Colour BABY_BLUE;
		static const Colour BABY_BLUE_EYES;
		static const Colour BABY_PINK;
		static const Colour BALL_BLUE;
		static const Colour BANANA_MANIA;
		static const Colour BANANA_YELLOW;
		static const Colour BATTLESHIP_GREY;
		static const Colour BAZAAR;
		static const Colour BEAU_BLUE;
		static const Colour BEAVER;
		static const Colour BEIGE;
		static const Colour BISQUE;
		static const Colour BISTRE;
		static const Colour BITTERSWEET;
		static const Colour BLACK;
		static const Colour BLANCHED_ALMOND;
		static const Colour BLEU_DE_FRANCE;
		static const Colour BLIZZARD_BLUE;
		static const Colour BLOND;
		static const Colour BLUE;
		static const Colour BLUE_BELL;
		static const Colour BLUE_GRAY;
		static const Colour BLUE_GREEN;
		static const Colour BLUE_VIOLET;
		static const Colour BLUSH;
		static const Colour BOLE;
		static const Colour BONDI_BLUE;
		static const Colour BOSTON_UNIVERSITY_RED;
		static const Colour BRANDEIS_BLUE;
		static const Colour BRASS;
		static const Colour BRICK_RED;
		static const Colour BRIGHT_CERULEAN;
		static const Colour BRIGHT_GREEN;
		static const Colour BRIGHT_LAVENDER;
		static const Colour BRIGHT_MAROON;
		static const Colour BRIGHT_PINK;
		static const Colour BRIGHT_TURQUOISE;
		static const Colour BRIGHT_UBE;
		static const Colour BRILLIANT_LAVENDER;
		static const Colour BRILLIANT_ROSE;
		static const Colour BRINK_PINK;
		static const Colour BRITISH_RACING_GREEN;
		static const Colour BRONZE;
		static const Colour BROWN_TRADITIONAL;
		static const Colour BROWN_WEB;
		static const Colour BUBBLE_GUM;
		static const Colour BUBBLES;
		static const Colour BUFF;
		static const Colour BULGARIAN_ROSE;
		static const Colour BURGUNDY;
		static const Colour BURLYWOOD;
		static const Colour BURNT_ORANGE;
		static const Colour BURNT_SIENNA;
		static const Colour BURNT_UMBER;
		static const Colour BYZANTINE;
		static const Colour BYZANTIUM;
		static const Colour CADET;
		static const Colour CADET_BLUE;
		static const Colour CADET_GREY;
		static const Colour CADMIUM_GREEN;
		static const Colour CADMIUM_ORANGE;
		static const Colour CADMIUM_RED;
		static const Colour CADMIUM_YELLOW;
		static const Colour CAL_POLY_POMONA_GREEN;
		static const Colour CAMBRIDGE_BLUE;
		static const Colour CAMEL;
		static const Colour CAMOUFLAGE_GREEN;
		static const Colour CANARY_YELLOW;
		static const Colour CANDY_APPLE_RED;
		static const Colour CANDY_PINK;
		static const Colour CAPRI;
		static const Colour CAPUT_MORTUUM;
		static const Colour CARDINAL;
		static const Colour CARIBBEAN_GREEN;
		static const Colour CARMINE;
		static const Colour CARMINE_PINK;
		static const Colour CARMINE_RED;
		static const Colour CARNATION_PINK;
		static const Colour CARNELIAN;
		static const Colour CAROLINA_BLUE;
		static const Colour CARROT_ORANGE;
		static const Colour CEIL;
		static const Colour CELADON;
		static const Colour CELESTIAL_BLUE;
		static const Colour CERISE;
		static const Colour CERISE_PINK;
		static const Colour CERULEAN;
		static const Colour CERULEAN_BLUE;
		static const Colour CG_BLUE;
		static const Colour CG_RED;
		static const Colour CHAMOISEE;
		static const Colour CHAMPAGNE;
		static const Colour CHARCOAL;
		static const Colour CHARTREUSE_TRADITIONAL;
		static const Colour CHARTREUSE_WEB;
		static const Colour CHERRY_BLOSSOM_PINK;
		static const Colour CHESTNUT;
		static const Colour CHOCOLATE_TRADITIONAL;
		static const Colour CHOCOLATE_WEB;
		static const Colour CHROME_YELLOW;
		static const Colour CINEREOUS;
		static const Colour CINNABAR;
		static const Colour CINNAMON;
		static const Colour CITRINE;
		static const Colour CLASSIC_ROSE;
		static const Colour COBALT;
		static const Colour COCOA_BROWN;
		static const Colour COFFEE;
		static const Colour COLUMBIA_BLUE;
		static const Colour COOL_BLACK;
		static const Colour COOL_GREY;
		static const Colour COPPER;
		static const Colour COPPER_ROSE;
		static const Colour COQUELICOT;
		static const Colour CORAL;
		static const Colour CORAL_PINK;
		static const Colour CORAL_RED;
		static const Colour CORDOVAN;
		static const Colour CORN;
		static const Colour CORNELL_RED;
		static const Colour CORNFLOWER_BLUE;
		static const Colour CORNSILK;
		static const Colour COSMIC_LATTE;
		static const Colour COTTON_CANDY;
		static const Colour CREAM;
		static const Colour CRIMSON;
		static const Colour CRIMSON_GLORY;
		static const Colour CYAN;
		static const Colour CYAN_PROCESS;
		static const Colour DAFFODIL;
		static const Colour DANDELION;
		static const Colour DARK_BLUE;
		static const Colour DARK_BROWN;
		static const Colour DARK_BYZANTIUM;
		static const Colour DARK_CANDY_APPLE_RED;
		static const Colour DARK_CERULEAN;
		static const Colour DARK_CHAMPAGNE;
		static const Colour DARK_CHESTNUT;
		static const Colour DARK_CORAL;
		static const Colour DARK_CYAN;
		static const Colour DARK_ELECTRIC_BLUE;
		static const Colour DARK_GOLDENROD;
		static const Colour DARK_GRAY;
		static const Colour DARK_GREEN;
		static const Colour DARK_JUNGLE_GREEN;
		static const Colour DARK_KHAKI;
		static const Colour DARK_LAVA;
		static const Colour DARK_LAVENDER;
		static const Colour DARK_MAGENTA;
		static const Colour DARK_MIDNIGHT_BLUE;
		static const Colour DARK_OLIVE_GREEN;
		static const Colour DARK_ORANGE;
		static const Colour DARK_ORCHID;
		static const Colour DARK_PASTEL_BLUE;
		static const Colour DARK_PASTEL_GREEN;
		static const Colour DARK_PASTEL_PURPLE;
		static const Colour DARK_PASTEL_RED;
		static const Colour DARK_PINK;
		static const Colour DARK_POWDER_BLUE;
		static const Colour DARK_RASPBERRY;
		static const Colour DARK_RED;
		static const Colour DARK_SALMON;
		static const Colour DARK_SCARLET;
		static const Colour DARK_SEA_GREEN;
		static const Colour DARK_SIENNA;
		static const Colour DARK_SLATE_BLUE;
		static const Colour DARK_SLATE_GRAY;
		static const Colour DARK_SPRING_GREEN;
		static const Colour DARK_TAN;
		static const Colour DARK_TANGERINE;
		static const Colour DARK_TAUPE;
		static const Colour DARK_TERRA_COTTA;
		static const Colour DARK_TURQUOISE;
		static const Colour DARK_VIOLET;
		static const Colour DARTMOUTH_GREEN;
		static const Colour DAVYS_GREY;
		static const Colour DEBIAN_RED;
		static const Colour DEEP_CARMINE;
		static const Colour DEEP_CARMINE_PINK;
		static const Colour DEEP_CARROT_ORANGE;
		static const Colour DEEP_CERISE;
		static const Colour DEEP_CHAMPAGNE;
		static const Colour DEEP_CHESTNUT;
		static const Colour DEEP_FUCHSIA;
		static const Colour DEEP_JUNGLE_GREEN;
		static const Colour DEEP_LILAC;
		static const Colour DEEP_MAGENTA;
		static const Colour DEEP_PEACH;
		static const Colour DEEP_PINK;
		static const Colour DEEP_SAFFRON;
		static const Colour DEEP_SKY_BLUE;
		static const Colour DENIM;
		static const Colour DESERT;
		static const Colour DESERT_SAND;
		static const Colour DIM_GRAY;
		static const Colour DODGER_BLUE;
		static const Colour DOGWOOD_ROSE;
		static const Colour DOLLAR_BILL;
		static const Colour DRAB;
		static const Colour DUKE_BLUE;
		static const Colour EARTH_YELLOW;
		static const Colour ECRU;
		static const Colour EGGPLANT;
		static const Colour EGGSHELL;
		static const Colour EGYPTIAN_BLUE;
		static const Colour ELECTRIC_BLUE;
		static const Colour ELECTRIC_CRIMSON;
		static const Colour ELECTRIC_CYAN;
		static const Colour ELECTRIC_GREEN;
		static const Colour ELECTRIC_INDIGO;
		static const Colour ELECTRIC_LAVENDER;
		static const Colour ELECTRIC_LIME;
		static const Colour ELECTRIC_PURPLE;
		static const Colour ELECTRIC_ULTRAMARINE;
		static const Colour ELECTRIC_VIOLET;
		static const Colour ELECTRIC_YELLOW;
		static const Colour EMERALD;
		static const Colour ETON_BLUE;
		static const Colour FALLOW;
		static const Colour FALU_RED;
		static const Colour FANDANGO;
		static const Colour FASHION_FUCHSIA;
		static const Colour FAWN;
		static const Colour FELDGRAU;
		static const Colour FERN_GREEN;
		static const Colour FERRARI_RED;
		static const Colour FIELD_DRAB;
		static const Colour FIREBRICK;
		static const Colour FIRE_ENGINE_RED;
		static const Colour FLAME;
		static const Colour FLAMINGO_PINK;
		static const Colour FLAVESCENT;
		static const Colour FLAX;
		static const Colour FLORAL_WHITE;
		static const Colour FLUORESCENT_ORANGE;
		static const Colour FLUORESCENT_PINK;
		static const Colour FLUORESCENT_YELLOW;
		static const Colour FOLLY;
		static const Colour FOREST_GREEN_TRADITIONAL;
		static const Colour FOREST_GREEN_WEB;
		static const Colour FRENCH_BEIGE;
		static const Colour FRENCH_BLUE;
		static const Colour FRENCH_LILAC;
		static const Colour FRENCH_ROSE;
		static const Colour FUCHSIA;
		static const Colour FUCHSIA_PINK;
		static const Colour FULVOUS;
		static const Colour FUZZY_WUZZY;
		static const Colour GAINSBORO;
		static const Colour GAMBOGE;
		static const Colour GHOST_WHITE;
		static const Colour GINGER;
		static const Colour GLAUCOUS;
		static const Colour GLITTER;
		static const Colour GOLD_METALLIC;
		static const Colour GOLD_WEB;
		static const Colour GOLDEN_BROWN;
		static const Colour GOLDEN_POPPY;
		static const Colour GOLDEN_YELLOW;
		static const Colour GOLDENROD;
		static const Colour GRANNY_SMITH_APPLE;
		static const Colour GRAY;
		static const Colour GRAY_HTML_CSS_GRAY;
		static const Colour GRAY_X11;
		static const Colour GRAY_ASPARAGUS;
		static const Colour GREEN;
		static const Colour GREEN_YELLOW;
		static const Colour GRULLO;
		static const Colour GUPPIE_GREEN;
		static const Colour HALAYA_UBE;
		static const Colour HAN_BLUE;
		static const Colour HAN_PURPLE;
		static const Colour HANSA_YELLOW;
		static const Colour HARLEQUIN;
		static const Colour HARVARD_CRIMSON;
		static const Colour HARVEST_GOLD;
		static const Colour HEART_GOLD;
		static const Colour HELIOTROPE;
		static const Colour HOLLYWOOD_CERISE;
		static const Colour HONEYDEW;
		static const Colour HOOKERS_GREEN;
		static const Colour HOT_MAGENTA;
		static const Colour HOT_PINK;
		static const Colour HUNTER_GREEN;
		static const Colour ICEBERG;
		static const Colour ICTERINE;
		static const Colour INCHWORM;
		static const Colour INDIA_GREEN;
		static const Colour INDIAN_RED;
		static const Colour INDIAN_YELLOW;
		static const Colour INDIGO_DYE;
		static const Colour INDIGO_WEB;
		static const Colour INTERNATIONAL_KLEIN_BLUE;
		static const Colour INTERNATIONAL_ORANGE;
		static const Colour IRIS;
		static const Colour ISABELLINE;
		static const Colour ISLAMIC_GREEN;
		static const Colour IVORY;
		static const Colour JADE;
		static const Colour JASPER;
		static const Colour JASMINE;
		static const Colour JAZZBERRY_JAM;
		static const Colour JONQUIL;
		static const Colour JUNE_BUD;
		static const Colour JUNGLE_GREEN;
		static const Colour KELLY_GREEN;
		static const Colour KHAKI;
		static const Colour KU_CRIMSON;
		static const Colour LA_SALLE_GREEN;
		static const Colour LANGUID_LAVENDER;
		static const Colour LAPIS_LAZULI;
		static const Colour LASER_LEMON;
		static const Colour LAVA;
		static const Colour LAVENDER_FLORAL;
		static const Colour LAVENDER_WEB;
		static const Colour LAVENDER_BLUE;
		static const Colour LAVENDER_BLUSH;
		static const Colour LAVENDER_GRAY;
		static const Colour LAVENDER_INDIGO;
		static const Colour LAVENDER_MAGENTA;
		static const Colour LAVENDER_MIST;
		static const Colour LAVENDER_PINK;
		static const Colour LAVENDER_PURPLE;
		static const Colour LAVENDER_ROSE;
		static const Colour LAWN_GREEN;
		static const Colour LEMON;
		static const Colour LEMON_CHIFFON;
		static const Colour LIGHT_APRICOT;
		static const Colour LIGHT_BLUE;
		static const Colour LIGHT_BROWN;
		static const Colour LIGHT_CARMINE_PINK;
		static const Colour LIGHT_CORAL;
		static const Colour LIGHT_CORNFLOWER_BLUE;
		static const Colour LIGHT_CRIMSON;
		static const Colour LIGHT_CYAN;
		static const Colour LIGHT_FUCHSIA_PINK;
		static const Colour LIGHT_GOLDENROD_YELLOW;
		static const Colour LIGHT_GRAY;
		static const Colour LIGHT_GREEN;
		static const Colour LIGHT_KHAKI;
		static const Colour LIGHT_MAUVE;
		static const Colour LIGHT_PASTEL_PURPLE;
		static const Colour LIGHT_PINK;
		static const Colour LIGHT_SALMON;
		static const Colour LIGHT_SALMON_PINK;
		static const Colour LIGHT_SEA_GREEN;
		static const Colour LIGHT_SKY_BLUE;
		static const Colour LIGHT_SLATE_GRAY;
		static const Colour LIGHT_TAUPE;
		static const Colour LIGHT_THULIAN_PINK;
		static const Colour LIGHT_YELLOW;
		static const Colour LILAC;
		static const Colour LIME;
		static const Colour LIME_GREEN;
		static const Colour LINCOLN_GREEN;
		static const Colour LINEN;
		static const Colour LIVER;
		static const Colour LUST;
		static const Colour MAGENTA;
		static const Colour MAGENTA_DYE;
		static const Colour MAGENTA_PROCESS;
		static const Colour MAGIC_MINT;
		static const Colour MAGNOLIA;
		static const Colour MAHOGANY;
		static const Colour MAIZE;
		static const Colour MAJORELLE_BLUE;
		static const Colour MALACHITE;
		static const Colour MANATEE;
		static const Colour MANGO_TANGO;
		static const Colour MAROON;
		static const Colour MAROON_X11;
		static const Colour MAUVE;
		static const Colour MAUVE_TAUPE;
		static const Colour MAUVELOUS;
		static const Colour MAYA_BLUE;
		static const Colour MEAT_BROWN;
		static const Colour MEDIUM_AQUAMARINE;
		static const Colour MEDIUM_BLUE;
		static const Colour MEDIUM_CANDY_APPLE_RED;
		static const Colour MEDIUM_CARMINE;
		static const Colour MEDIUM_CHAMPAGNE;
		static const Colour MEDIUM_ELECTRIC_BLUE;
		static const Colour MEDIUM_JUNGLE_GREEN;
		static const Colour MEDIUM_LAVENDER_MAGENTA;
		static const Colour MEDIUM_ORCHID;
		static const Colour MEDIUM_PERSIAN_BLUE;
		static const Colour MEDIUM_PURPLE;
		static const Colour MEDIUM_RED_VIOLET;
		static const Colour MEDIUM_SEA_GREEN;
		static const Colour MEDIUM_SLATE_BLUE;
		static const Colour MEDIUM_SPRING_BUD;
		static const Colour MEDIUM_SPRING_GREEN;
		static const Colour MEDIUM_TAUPE;
		static const Colour MEDIUM_TEAL_BLUE;
		static const Colour MEDIUM_TURQUOISE;
		static const Colour MEDIUM_VIOLET_RED;
		static const Colour MELON;
		static const Colour MIDNIGHT_BLUE;
		static const Colour MIDNIGHT_GREEN;
		static const Colour MIKADO_YELLOW;
		static const Colour MINT;
		static const Colour MINT_CREAM;
		static const Colour MINT_GREEN;
		static const Colour MISTY_ROSE;
		static const Colour MOCCASIN;
		static const Colour MODE_BEIGE;
		static const Colour MOONSTONE_BLUE;
		static const Colour MORDANT_RED_19;
		static const Colour MOSS_GREEN;
		static const Colour MOUNTAIN_MEADOW;
		static const Colour MOUNTBATTEN_PINK;
		static const Colour MULBERRY;
		static const Colour MUSTARD;
		static const Colour MYRTLE;
		static const Colour MSU_GREEN;
		static const Colour NADESHIKO_PINK;
		static const Colour NAPIER_GREEN;
		static const Colour NAPLES_YELLOW;
		static const Colour NAVAJO_WHITE;
		static const Colour NAVY_BLUE;
		static const Colour NEON_CARROT;
		static const Colour NEON_FUCHSIA;
		static const Colour NEON_GREEN;
		static const Colour NON_PHOTO_BLUE;
		static const Colour OCEAN_BOAT_BLUE;
		static const Colour OCHRE;
		static const Colour OFFICE_GREEN;
		static const Colour OLD_GOLD;
		static const Colour OLD_LACE;
		static const Colour OLD_LAVENDER;
		static const Colour OLD_MAUVE;
		static const Colour OLD_ROSE;
		static const Colour OLIVE;
		static const Colour OLIVE_DRAB_3;
		static const Colour OLIVE_DRAB_7;
		static const Colour OLIVINE;
		static const Colour ONYX;
		static const Colour OPERA_MAUVE;
		static const Colour ORANGE;
		static const Colour ORANGE_RYB;
		static const Colour ORANGE_WEB;
		static const Colour ORANGE_PEEL;
		static const Colour ORANGE_RED;
		static const Colour ORCHID;
		static const Colour OTTER_BROWN;
		static const Colour OUTER_SPACE;
		static const Colour OUTRAGEOUS_ORANGE;
		static const Colour OXFORD_BLUE;
		static const Colour OU_CRIMSON_RED;
		static const Colour PAKISTAN_GREEN;
		static const Colour PALATINATE_BLUE;
		static const Colour PALATINATE_PURPLE;
		static const Colour PALE_AQUA;
		static const Colour PALE_BLUE;
		static const Colour PALE_BROWN;
		static const Colour PALE_CARMINE;
		static const Colour PALE_CERULEAN;
		static const Colour PALE_CHESTNUT;
		static const Colour PALE_COPPER;
		static const Colour PALE_CORNFLOWER_BLUE;
		static const Colour PALE_GOLD;
		static const Colour PALE_GOLDENROD;
		static const Colour PALE_GREEN;
		static const Colour PALE_MAGENTA;
		static const Colour PALE_PINK;
		static const Colour PALE_PLUM;
		static const Colour PALE_RED_VIOLET;
		static const Colour PALE_ROBIN_EGG_BLUE;
		static const Colour PALE_SILVER;
		static const Colour PALE_SPRING_BUD;
		static const Colour PALE_TAUPE;
		static const Colour PALE_VIOLET_RED;
		static const Colour PANSY_PURPLE;
		static const Colour PAPAYA_WHIP;
		static const Colour PARIS_GREEN;
		static const Colour PASTEL_BLUE;
		static const Colour PASTEL_BROWN;
		static const Colour PASTEL_GRAY;
		static const Colour PASTEL_GREEN;
		static const Colour PASTEL_MAGENTA;
		static const Colour PASTEL_ORANGE;
		static const Colour PASTEL_PINK;
		static const Colour PASTEL_PURPLE;
		static const Colour PASTEL_RED;
		static const Colour PASTEL_VIOLET;
		static const Colour PASTEL_YELLOW;
		static const Colour PATRIARCH;
		static const Colour PAYNES_GREY;
		static const Colour PEACH;
		static const Colour PEACH_ORANGE;
		static const Colour PEACH_PUFF;
		static const Colour PEACH_YELLOW;
		static const Colour PEAR;
		static const Colour PEARL;
		static const Colour PEARL_AQUA;
		static const Colour PERIDOT;
		static const Colour PERIWINKLE;
		static const Colour PERSIAN_BLUE;
		static const Colour PERSIAN_GREEN;
		static const Colour PERSIAN_INDIGO;
		static const Colour PERSIAN_ORANGE;
		static const Colour PERSIAN_PINK;
		static const Colour PERSIAN_PLUM;
		static const Colour PERSIAN_RED;
		static const Colour PERSIAN_ROSE;
		static const Colour PERSIMMON;
		static const Colour PHLOX;
		static const Colour PHTHALO_BLUE;
		static const Colour PHTHALO_GREEN;
		static const Colour PIGGY_PINK;
		static const Colour PINE_GREEN;
		static const Colour PINK;
		static const Colour PINK_ORANGE;
		static const Colour PINK_PEARL;
		static const Colour PINK_SHERBET;
		static const Colour PISTACHIO;
		static const Colour PLATINUM;
		static const Colour PLUM;
		static const Colour PLUM_WEB;
		static const Colour PORTLAND_ORANGE;
		static const Colour POWDER_BLUE;
		static const Colour PRINCETON_ORANGE;
		static const Colour PRUNE;
		static const Colour PRUSSIAN_BLUE;
		static const Colour PSYCHEDELIC_PURPLE;
		static const Colour PUCE;
		static const Colour PUMPKIN;
		static const Colour PURPLE;
		static const Colour PURPLE_MUNSELL;
		static const Colour PURPLE_X11;
		static const Colour PURPLE_HEART;
		static const Colour PURPLE_MOUNTAIN_MAJESTY;
		static const Colour PURPLE_PIZZAZZ;
		static const Colour PURPLE_TAUPE;
		static const Colour QUARTZ;
		static const Colour RADICAL_RED;
		static const Colour RASPBERRY;
		static const Colour RASPBERRY_GLACE;
		static const Colour RASPBERRY_PINK;
		static const Colour RASPBERRY_ROSE;
		static const Colour RAW_UMBER;
		static const Colour RAZZLE_DAZZLE_ROSE;
		static const Colour RAZZMATAZZ;
		static const Colour RED;
		static const Colour RED_MUNSELL;
		static const Colour RED_NCS;
		static const Colour RED_PIGMENT;
		static const Colour RED_RYB;
		static const Colour RED_BROWN;
		static const Colour RED_VIOLET;
		static const Colour REDWOOD;
		static const Colour REGALIA;
		static const Colour RICH_BLACK;
		static const Colour RICH_BRILLIANT_LAVENDER;
		static const Colour RICH_CARMINE;
		static const Colour RICH_ELECTRIC_BLUE;
		static const Colour RICH_LAVENDER;
		static const Colour RICH_LILAC;
		static const Colour RICH_MAROON;
		static const Colour RIFLE_GREEN;
		static const Colour ROBIN_EGG_BLUE;
		static const Colour ROSE;
		static const Colour ROSE_BONBON;
		static const Colour ROSE_EBONY;
		static const Colour ROSE_GOLD;
		static const Colour ROSE_MADDER;
		static const Colour ROSE_PINK;
		static const Colour ROSE_QUARTZ;
		static const Colour ROSE_TAUPE;
		static const Colour ROSE_VALE;
		static const Colour ROSEWOOD;
		static const Colour ROSSO_CORSA;
		static const Colour ROSY_BROWN;
		static const Colour ROYAL_AZURE;
		static const Colour ROYAL_BLUE;
		static const Colour ROYAL_BLUE_WEB;
		static const Colour ROYAL_FUCHSIA;
		static const Colour ROYAL_PURPLE;
		static const Colour RUBY;
		static const Colour RUDDY;
		static const Colour RUDDY_BROWN;
		static const Colour RUDDY_PINK;
		static const Colour RUFOUS;
		static const Colour RUSSET;
		static const Colour RUST;
		static const Colour SACRAMENTO_STATE_GREEN;
		static const Colour SADDLE_BROWN;
		static const Colour SAFETY_ORANGE;
		static const Colour SAFFRON;
		static const Colour ST_PATRICKS_BLUE;
		static const Colour SALMON;
		static const Colour SALMON_PINK;
		static const Colour SAND;
		static const Colour SAND_DUNE;
		static const Colour SANDSTORM;
		static const Colour SANDY_BROWN;
		static const Colour SANDY_TAUPE;
		static const Colour SANGRIA;
		static const Colour SAP_GREEN;
		static const Colour SAPPHIRE;
		static const Colour SATIN_SHEEN_GOLD;
		static const Colour SCARLET;
		static const Colour SCHOOL_BUS_YELLOW;
		static const Colour SCREAMIN_GREEN;
		static const Colour SEA_GREEN;
		static const Colour SEAL_BROWN;
		static const Colour SEASHELL;
		static const Colour SELECTIVE_YELLOW;
		static const Colour SEPIA;
		static const Colour SHADOW;
		static const Colour SHAMROCK_GREEN;
		static const Colour SHOCKING_PINK;
		static const Colour SIENNA;
		static const Colour SILVER;
		static const Colour SINOPIA;
		static const Colour SKOBELOFF;
		static const Colour SKY_BLUE;
		static const Colour SKY_MAGENTA;
		static const Colour SLATE_BLUE;
		static const Colour SLATE_GRAY;
		static const Colour SMALT;
		static const Colour SMOKEY_TOPAZ;
		static const Colour SMOKY_BLACK;
		static const Colour SNOW;
		static const Colour SPIRO_DISCO_BALL;
		static const Colour SPLASHED_WHITE;
		static const Colour SPRING_BUD;
		static const Colour SPRING_GREEN;
		static const Colour STEEL_BLUE;
		static const Colour STIL_DE_GRAIN_YELLOW;
		static const Colour STIZZA;
		static const Colour STRAW;
		static const Colour SUNGLOW;
		static const Colour SUNSET;
		static const Colour TAN;
		static const Colour TANGELO;
		static const Colour TANGERINE;
		static const Colour TANGERINE_YELLOW;
		static const Colour TAUPE;
		static const Colour TAUPE_GRAY;
		static const Colour TEA_GREEN;
		static const Colour TEA_ROSE_ORANGE;
		static const Colour TEA_ROSE;
		static const Colour TEAL;
		static const Colour TEAL_BLUE;
		static const Colour TEAL_GREEN;
		static const Colour TENNE_TAWNY;
		static const Colour TERRA_COTTA;
		static const Colour THISTLE;
		static const Colour THULIAN_PINK;
		static const Colour TICKLE_ME_PINK;
		static const Colour TIFFANY_BLUE;
		static const Colour TIGERS_EYE;
		static const Colour TIMBERWOLF;
		static const Colour TITANIUM_YELLOW;
		static const Colour TOMATO;
		static const Colour TOOLBOX;
		static const Colour TOPAZ;
		static const Colour TRACTOR_RED;
		static const Colour TROLLEY_GREY;
		static const Colour TROPICAL_RAIN_FOREST;
		static const Colour TRUE_BLUE;
		static const Colour TUFTS_BLUE;
		static const Colour TUMBLEWEED;
		static const Colour TURKISH_ROSE;
		static const Colour TURQUOISE;
		static const Colour TURQUOISE_BLUE;
		static const Colour TURQUOISE_GREEN;
		static const Colour TUSCAN_RED;
		static const Colour TWILIGHT_LAVENDER;
		static const Colour TYRIAN_PURPLE;
		static const Colour UA_BLUE;
		static const Colour UA_RED;
		static const Colour UBE;
		static const Colour UCLA_BLUE;
		static const Colour UCLA_GOLD;
		static const Colour UFO_GREEN;
		static const Colour ULTRAMARINE;
		static const Colour ULTRAMARINE_BLUE;
		static const Colour ULTRA_PINK;
		static const Colour UMBER;
		static const Colour UNITED_NATIONS_BLUE;
		static const Colour UNIVERSITY_OF_CALIFORNIA_GOLD;
		static const Colour UNMELLOW_YELLOW;
		static const Colour UP_FOREST_GREEN;
		static const Colour UP_MAROON;
		static const Colour UPSDELL_RED;
		static const Colour UROBILIN;
		static const Colour USC_CARDINAL;
		static const Colour USC_GOLD;
		static const Colour UTAH_CRIMSON;
		static const Colour VANILLA;
		static const Colour VEGAS_GOLD;
		static const Colour VENETIAN_RED;
		static const Colour VERDIGRIS;
		static const Colour VERMILION;
		static const Colour VERONICA;
		static const Colour VIOLET;
		static const Colour VIOLET_RYB;
		static const Colour VIOLET_WEB;
		static const Colour VIRIDIAN;
		static const Colour VIVID_AUBURN;
		static const Colour VIVID_BURGUNDY;
		static const Colour VIVID_CERISE;
		static const Colour VIVID_TANGERINE;
		static const Colour VIVID_VIOLET;
		static const Colour WARM_BLACK;
		static const Colour WENGE;
		static const Colour WHEAT;
		static const Colour WHITE;
		static const Colour WHITE_SMOKE;
		static const Colour WILD_BLUE_YONDER;
		static const Colour WILD_STRAWBERRY;
		static const Colour WILD_WATERMELON;
		static const Colour WINE;
		static const Colour WISTERIA;
		static const Colour XANADU;
		static const Colour YALE_BLUE;
		static const Colour YELLOW;
		static const Colour YELLOW_MUNSELL;
		static const Colour YELLOW_NCS;
		static const Colour YELLOW_PROCESS;
		static const Colour YELLOW_RYB;
		static const Colour YELLOW_GREEN;
		static const Colour YELLOW_ORANGE;
		static const Colour ZAFFRE;
		static const Colour ZINNWALDITE_BROWN;
	};

	inline Colour operator+(const Colour& v1, const Colour& v2) 
	{
		Colour v = v1;
		v = v + v2;
		return v;
	}
}
#endif 
