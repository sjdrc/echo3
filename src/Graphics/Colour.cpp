#include <echo/Graphics/Colour.h>
#include <echo/Util/StringUtils.h>
#include <echo/Util/Utils.h>
#include <echo/Maths/EchoMaths.h>
#include <sstream>

namespace Echo
{
	Colour::Colour(const std::string& valueAsString)
	{
		std::stringstream ss(valueAsString);
		ss >> *this;
	}
	
	std::istream& operator >>(std::istream& i, Colour& c)
	{
		using namespace Utils::String;
		std::string temp;
		i >> temp;
		
		// We want to support the following:
		// - Any channel can be 0-1 or 0-255, or
		// - The value can be #RRGGBBAA

		// Since we will implicitly allow 0-1 AND 0-255 we also want to allow an
		// explicit token for 0-255 to allow for 1,1,1,1 which can't work
		// implicitly.
		//
		// Implicit conversion issue:
		// If the value is >1 then conversion for that channel is specified in an 8bit value.
		// This almost allows the full range except for 1 as a value between 0 and 255 since
		// 1 should result in 100% and not 0.392156862745%.
		//
		// To work around this we'll first check for an explicit Colour8 token which forces
		// the 8 bit conversion.
		
		bool force8Bit=false;
		force8Bit = Utils::String::VerifyConstructorAndExtractParameters(temp,"Colour8",false);
				
		// If Colour8 succeeded then temp will be hex or r,g,b,a otherwise if it fails
		// it might still be Colour(r,g,b,a), if this fails - the string isn't a colour.
		if(!Utils::String::VerifyConstructorAndExtractParameters(temp,"Colour") || temp.empty())
		{
			c = Colours::WHITE;
			i.setstate(std::ios_base::failbit);
			return i;
		}
		
		// Now, before we go and convert our individual channels, we'll check if it is a hex
		// value, we support # and h because # is often used to denote a comment in some
		// formats.
		if(temp[0]=='#' || temp[0]=='h')
		{
			//Now we expect #RRGGBBAA
			if(temp.length()!=9)
			{
				c = Colours::WHITE;
				i.setstate(std::ios_base::failbit);
				return i;
			}
			
			//     RR						GG                       BB                       AA
			temp = temp.substr(1,2) + " " + temp.substr(3,2) + " " + temp.substr(5,2) + " " + temp.substr(7,2);
			std::stringstream ss(temp);
			ss << std::hex;	//Switch on hex processing.
			u32 inty;	//Mr. Inty the Integer.
			//I don't normally one line multiple statements but this is easy to follow.
			inty=0; ss >> inty; c.mRed = inty;
			inty=0; ss >> inty; c.mGreen = inty;
			inty=0; ss >> inty; c.mBlue = inty;
			inty=0; ss >> inty; c.mAlpha = inty;
			force8Bit=true;
		}else
		{
			if(!ConvertAndAssign(temp, c.mRed, c.mGreen, c.mBlue, c.mAlpha))
			{
				c = Colours::WHITE;
				i.setstate(std::ios_base::failbit);
				return i;
			}
		}
		
		if(force8Bit)
		{
			//Explicitly convert as though the value was an 8 bit channel.
			c.mRed=LimitOne(Ratio(c.mRed,255.f));
			c.mGreen=LimitOne(Ratio(c.mGreen,255.f));
			c.mBlue=LimitOne(Ratio(c.mBlue,255.f));
			c.mAlpha=LimitOne(Ratio(c.mAlpha,255.f));
		}else
		{
			//Implicit - 0-1 range always works (unchanged) but >1 will perform 8bit conversion.
			if(c.mRed>1)
			{
				c.mRed=LimitOne(Ratio(c.mRed,255.f));
			}
			if(c.mGreen>1)
			{
				c.mGreen=LimitOne(Ratio(c.mGreen,255.f));
			}
			if(c.mBlue>1)
			{
				c.mBlue=LimitOne(Ratio(c.mBlue,255.f));
			}
			if(c.mAlpha>1)
			{
				c.mAlpha=LimitOne(Ratio(c.mAlpha,255.f));
			}
		}
		return i;
	}		
	
	const Colour Colours::AIR_FORCE_BLUE = 			Colour(0.36f,	0.54f,	0.66f,	1.0f);
	const Colour Colours::ALICE_BLUE = 				Colour(0.94f,	0.97f,	1.0f,	1.0f);
	const Colour Colours::ALIZARIN_CRIMSON = 		Colour(0.89f,	0.15f,	0.21f,	1.0f);
	const Colour Colours::ALMOND = 					Colour(0.94f,	0.87f,	0.80f,	1.0f);
	const Colour Colours::AMARANTH = 				Colour(0.90f,	0.17f,	0.31f,	1.0f);
	const Colour Colours::AMBER = 					Colour(1.0f,	0.75f,	0.0f,	1.0f);
	const Colour Colours::AMERICAN_ROSE = 			Colour(1.0f,	0.1f,	0.24f,	1.0f);
	const Colour Colours::AMETHYST = 				Colour(0.60f,	0.40f,	0.80f,	1.0f);
	const Colour Colours::ANDROID_GREEN = 			Colour(0.64f,	0.78f,	0.22f,	1.0f);
	const Colour Colours::ANTIFLASH_WHITE = 		Colour(0.95f,	0.95f,	0.96f,	1.0f);
	const Colour Colours::ANTIQUE_BRASS = 			Colour(0.80f,	0.58f,	0.46f,	1.0f);
	const Colour Colours::ANTIQUE_FUCHSIA = 		Colour(0.57f,	0.36f,	0.51f,	1.0f);
	const Colour Colours::ANTIQUE_WHITE = 			Colour(0.98f,	0.92f,	0.84f,	1.0f);
	const Colour Colours::AO = 						Colour(0.0f,	0.50f,	0.0f,	1.0f);
	const Colour Colours::APPLE_GREEN = 			Colour(0.55f,	0.71f,	0.0f,	1.0f);
	const Colour Colours::APRICOT = 				Colour(0.98f,	0.81f,	0.69f,	1.0f);
	const Colour Colours::AQUA = 					Colour(0.0f,	1.0f,	1.0f,	1.0f);
	const Colour Colours::AQUAMARINE = 				Colour(0.50f,	1.0f,	0.83f,	1.0f);
	const Colour Colours::ARMY_GREEN = 				Colour(0.29f,	0.33f,	0.13f,	1.0f);
	const Colour Colours::ARSENIC = 				Colour(0.23f,	0.27f,	0.29f,	1.0f);
	const Colour Colours::ARYLIDE_YELLOW = 			Colour(0.91f,	0.84f,	0.42f,	1.0f);
	const Colour Colours::ASH_GREY = 				Colour(0.0f,	0.75f,	0.71f,	1.0f);
	const Colour Colours::ASPARAGUS = 				Colour(0.3f,	0.66f,	0.42f,	1.0f);
	const Colour Colours::ATOMIC_TANGERINE = 		Colour(1.0f,	0.60f,	0.40f,	1.0f);
	const Colour Colours::AUBURN = 					Colour(0.65f,	0.16f,	0.16f,	1.0f);
	const Colour Colours::AUREOLIN = 				Colour(0.99f,	0.93f,	0.0f,	1.0f);
	const Colour Colours::AUROMETALSAURUS = 		Colour(0.43f,	0.50f,	0.50f,	1.0f);
	const Colour Colours::AWESOME = 				Colour(1.0f,	0.13f,	0.32f,	1.0f);
	const Colour Colours::AZURE = 					Colour(0.0f,	0.50f,	1.0f,	1.0f);
	const Colour Colours::AZURE_MIST = 				Colour(0.94f,	1.0f,	1.0f,	1.0f);
	const Colour Colours::BABY_BLUE = 				Colour(0.47f,	0.81f,	0.94f,	1.0f);
	const Colour Colours::BABY_BLUE_EYES = 			Colour(0.63f,	0.79f,	0.95f,	1.0f);
	const Colour Colours::BABY_PINK = 				Colour(0.96f,	0.76f,	0.76f,	1.0f);
	const Colour Colours::BALL_BLUE = 				Colour(0.13f,	0.67f,	0.80f,	1.0f);
	const Colour Colours::BANANA_MANIA = 			Colour(0.98f,	0.91f,	0.71f,	1.0f);
	const Colour Colours::BANANA_YELLOW = 			Colour(1.0f,	0.88f,	0.21f,	1.0f);
	const Colour Colours::BATTLESHIP_GREY = 		Colour(0.52f,	0.52f,	0.51f,	1.0f);
	const Colour Colours::BAZAAR = 					Colour(0.60f,	0.47f,	0.48f,	1.0f);
	const Colour Colours::BEAU_BLUE = 				Colour(0.74f,	0.83f,	0.90f,	1.0f);
	const Colour Colours::BEAVER = 					Colour(0.62f,	0.51f,	0.44f,	1.0f);
	const Colour Colours::BEIGE = 					Colour(0.96f,	0.96f,	0.86f,	1.0f);
	const Colour Colours::BISQUE = 					Colour(1.0f,	0.89f,	0.77f,	1.0f);
	const Colour Colours::BISTRE = 					Colour(0.24f,	0.17f,	0.12f,	1.0f);
	const Colour Colours::BITTERSWEET = 			Colour(1.0f,	0.44f,	0.37f,	1.0f);
	const Colour Colours::BLACK = 					Colour(0.0f,	0.0f,	0.0f,	1.0f);
	const Colour Colours::BLANCHED_ALMOND = 		Colour(1.0f,	0.92f,	0.80f,	1.0f);
	const Colour Colours::BLEU_DE_FRANCE = 			Colour(0.19f,	0.55f,	0.91f,	1.0f);
	const Colour Colours::BLIZZARD_BLUE = 			Colour(0.67f,	0.90f,	0.93f,	1.0f);
	const Colour Colours::BLOND = 					Colour(0.98f,	0.94f,	0.75f,	1.0f);
	const Colour Colours::BLUE = 					Colour(0.0f,	0.0f,	1.0f,	1.0f);
	const Colour Colours::BLUE_BELL = 				Colour(0.64f,	0.64f,	0.82f,	1.0f);
	const Colour Colours::BLUE_GRAY = 				Colour(0.40f,	0.60f,	0.80f,	1.0f);
	const Colour Colours::BLUE_GREEN = 				Colour(0.0f,	0.87f,	0.87f,	1.0f);
	const Colour Colours::BLUE_VIOLET = 			Colour(0.54f,	0.17f,	0.89f,	1.0f);
	const Colour Colours::BLUSH = 					Colour(0.87f,	0.36f,	0.51f,	1.0f);
	const Colour Colours::BOLE = 					Colour(0.47f,	0.27f,	0.23f,	1.0f);
	const Colour Colours::BONDI_BLUE = 				Colour(0.0f,	0.58f,	0.71f,	1.0f);
	const Colour Colours::BOSTON_UNIVERSITY_RED = 	Colour(0.80f,	0.0f,	0.0f,	1.0f);
	const Colour Colours::BRANDEIS_BLUE = 			Colour(0.0f,	0.44f,	1.0f,	1.0f);
	const Colour Colours::BRASS = 					Colour(0.71f,	0.65f,	0.26f,	1.0f);
	const Colour Colours::BRICK_RED = 				Colour(0.80f,	0.25f,	0.33f,	1.0f);
	const Colour Colours::BRIGHT_CERULEAN = 		Colour(0.11f,	0.67f,	0.84f,	1.0f);
	const Colour Colours::BRIGHT_GREEN = 			Colour(0.40f,	1.0f,	0.0f,	1.0f);
	const Colour Colours::BRIGHT_LAVENDER = 		Colour(0.75f,	0.58f,	0.89f,	1.0f);
	const Colour Colours::BRIGHT_MAROON = 			Colour(0.76f,	0.13f,	0.28f,	1.0f);
	const Colour Colours::BRIGHT_PINK = 			Colour(1.0f,	0.0f,	0.50f,	1.0f);
	const Colour Colours::BRIGHT_TURQUOISE = 		Colour(0.3f,	0.91f,	0.87f,	1.0f);
	const Colour Colours::BRIGHT_UBE = 				Colour(0.82f,	0.62f,	0.91f,	1.0f);
	const Colour Colours::BRILLIANT_LAVENDER = 		Colour(0.96f,	0.73f,	1.0f,	1.0f);
	const Colour Colours::BRILLIANT_ROSE = 			Colour(1.0f,	0.33f,	0.64f,	1.0f);
	const Colour Colours::BRINK_PINK = 				Colour(0.98f,	0.38f,	0.50f,	1.0f);
	const Colour Colours::BRITISH_RACING_GREEN = 	Colour(0.0f,	0.26f,	0.15f,	1.0f);
	const Colour Colours::BRONZE = 					Colour(0.80f,	0.50f,	0.20f,	1.0f);
	const Colour Colours::BROWN_TRADITIONAL = 		Colour(0.59f,	0.29f,	0.0f,	1.0f);
	const Colour Colours::BROWN_WEB = 				Colour(0.65f,	0.16f,	0.16f,	1.0f);
	const Colour Colours::BUBBLE_GUM = 				Colour(0.99f,	0.76f,	0.80f,	1.0f);
	const Colour Colours::BUBBLES = 				Colour(0.91f,	1.0f,	1.0f,	1.0f);
	const Colour Colours::BUFF = 					Colour(0.94f,	0.86f,	0.51f,	1.0f);
	const Colour Colours::BULGARIAN_ROSE = 			Colour(0.28f,	0.2f,	0.3f,	1.0f);
	const Colour Colours::BURGUNDY = 				Colour(0.50f,	0.0f,	0.13f,	1.0f);
	const Colour Colours::BURLYWOOD = 				Colour(0.87f,	0.72f,	0.53f,	1.0f);
	const Colour Colours::BURNT_ORANGE = 			Colour(0.80f,	0.33f,	0.0f,	1.0f);
	const Colour Colours::BURNT_SIENNA = 			Colour(0.91f,	0.45f,	0.32f,	1.0f);
	const Colour Colours::BURNT_UMBER = 			Colour(0.54f,	0.20f,	0.14f,	1.0f);
	const Colour Colours::BYZANTINE = 				Colour(0.74f,	0.20f,	0.64f,	1.0f);
	const Colour Colours::BYZANTIUM = 				Colour(0.44f,	0.16f,	0.39f,	1.0f);
	const Colour Colours::CADET = 					Colour(0.33f,	0.41f,	0.47f,	1.0f);
	const Colour Colours::CADET_BLUE = 				Colour(0.37f,	0.62f,	0.63f,	1.0f);
	const Colour Colours::CADET_GREY = 				Colour(0.57f,	0.64f,	0.69f,	1.0f);
	const Colour Colours::CADMIUM_GREEN = 			Colour(0.0f,	0.42f,	0.24f,	1.0f);
	const Colour Colours::CADMIUM_ORANGE = 			Colour(0.93f,	0.53f,	0.18f,	1.0f);
	const Colour Colours::CADMIUM_RED = 			Colour(0.89f,	0.0f,	0.13f,	1.0f);
	const Colour Colours::CADMIUM_YELLOW = 			Colour(1.0f,	0.96f,	0.0f,	1.0f);
	const Colour Colours::CAL_POLY_POMONA_GREEN = 	Colour(0.12f,	0.30f,	0.17f,	1.0f);
	const Colour Colours::CAMBRIDGE_BLUE = 			Colour(0.64f,	0.76f,	0.68f,	1.0f);
	const Colour Colours::CAMEL = 					Colour(0.76f,	0.60f,	0.42f,	1.0f);
	const Colour Colours::CAMOUFLAGE_GREEN = 		Colour(0.47f,	0.53f,	0.42f,	1.0f);
	const Colour Colours::CANARY_YELLOW = 			Colour(1.0f,	0.94f,	0.0f,	1.0f);
	const Colour Colours::CANDY_APPLE_RED = 		Colour(1.0f,	0.3f,	0.0f,	1.0f);
	const Colour Colours::CANDY_PINK = 				Colour(0.89f,	0.44f,	0.48f,	1.0f);
	const Colour Colours::CAPRI = 					Colour(0.0f,	0.75f,	1.0f,	1.0f);
	const Colour Colours::CAPUT_MORTUUM = 			Colour(0.35f,	0.15f,	0.13f,	1.0f);
	const Colour Colours::CARDINAL = 				Colour(0.77f,	0.12f,	0.23f,	1.0f);
	const Colour Colours::CARIBBEAN_GREEN = 		Colour(0.0f,	0.80f,	0.60f,	1.0f);
	const Colour Colours::CARMINE = 				Colour(0.59f,	0.0f,	0.9f,	1.0f);
	const Colour Colours::CARMINE_PINK = 			Colour(0.92f,	0.30f,	0.26f,	1.0f);
	const Colour Colours::CARMINE_RED = 			Colour(1.0f,	0.0f,	0.22f,	1.0f);
	const Colour Colours::CARNATION_PINK = 			Colour(1.0f,	0.65f,	0.79f,	1.0f);
	const Colour Colours::CARNELIAN = 				Colour(0.70f,	0.11f,	0.11f,	1.0f);
	const Colour Colours::CAROLINA_BLUE = 			Colour(0.60f,	0.73f,	0.89f,	1.0f);
	const Colour Colours::CARROT_ORANGE = 			Colour(0.93f,	0.57f,	0.13f,	1.0f);
	const Colour Colours::CEIL = 					Colour(0.57f,	0.63f,	0.81f,	1.0f);
	const Colour Colours::CELADON = 				Colour(0.67f,	0.88f,	0.69f,	1.0f);
	const Colour Colours::CELESTIAL_BLUE = 			Colour(0.29f,	0.59f,	0.82f,	1.0f);
	const Colour Colours::CERISE = 					Colour(0.87f,	0.19f,	0.39f,	1.0f);
	const Colour Colours::CERISE_PINK = 			Colour(0.93f,	0.23f,	0.51f,	1.0f);
	const Colour Colours::CERULEAN = 				Colour(0.0f,	0.48f,	0.65f,	1.0f);
	const Colour Colours::CERULEAN_BLUE = 			Colour(0.16f,	0.32f,	0.75f,	1.0f);
	const Colour Colours::CG_BLUE = 				Colour(0.0f,	0.48f,	0.65f,	1.0f);
	const Colour Colours::CG_RED = 					Colour(0.88f,	0.24f,	0.19f,	1.0f);
	const Colour Colours::CHAMOISEE = 				Colour(0.63f,	0.47f,	0.35f,	1.0f);
	const Colour Colours::CHAMPAGNE = 				Colour(0.97f,	0.91f,	0.81f,	1.0f);
	const Colour Colours::CHARCOAL = 				Colour(0.21f,	0.27f,	0.31f,	1.0f);
	const Colour Colours::CHARTREUSE_TRADITIONAL = 	Colour(0.87f,	1.0f,	0.0f,	1.0f);
	const Colour Colours::CHARTREUSE_WEB = 			Colour(0.50f,	1.0f,	0.0f,	1.0f);
	const Colour Colours::CHERRY_BLOSSOM_PINK = 	Colour(1.0f,	0.72f,	0.77f,	1.0f);
	const Colour Colours::CHESTNUT = 				Colour(0.80f,	0.36f,	0.36f,	1.0f);
	const Colour Colours::CHOCOLATE_TRADITIONAL = 	Colour(0.48f,	0.25f,	0.0f,	1.0f);
	const Colour Colours::CHOCOLATE_WEB = 			Colour(0.82f,	0.41f,	0.12f,	1.0f);
	const Colour Colours::CHROME_YELLOW = 			Colour(1.0f,	0.65f,	0.0f,	1.0f);
	const Colour Colours::CINEREOUS = 				Colour(0.60f,	0.51f,	0.48f,	1.0f);
	const Colour Colours::CINNABAR = 				Colour(0.89f,	0.26f,	0.20f,	1.0f);
	const Colour Colours::CINNAMON = 				Colour(0.82f,	0.41f,	0.12f,	1.0f);
	const Colour Colours::CITRINE = 				Colour(0.89f,	0.82f,	0.4f,	1.0f);
	const Colour Colours::CLASSIC_ROSE = 			Colour(0.98f,	0.80f,	0.91f,	1.0f);
	const Colour Colours::COBALT = 					Colour(0.0f,	0.28f,	0.67f,	1.0f);
	const Colour Colours::COCOA_BROWN = 			Colour(0.82f,	0.41f,	0.12f,	1.0f);
	const Colour Colours::COFFEE = 					Colour(0.44f,	0.31f,	0.22f,	1.0f);
	const Colour Colours::COLUMBIA_BLUE = 			Colour(0.61f,	0.87f,	1.0f,	1.0f);
	const Colour Colours::COOL_BLACK = 				Colour(0.0f,	0.18f,	0.39f,	1.0f);
	const Colour Colours::COOL_GREY = 				Colour(0.55f,	0.57f,	0.67f,	1.0f);
	const Colour Colours::COPPER = 					Colour(0.72f,	0.45f,	0.20f,	1.0f);
	const Colour Colours::COPPER_ROSE = 			Colour(0.60f,	0.40f,	0.40f,	1.0f);
	const Colour Colours::COQUELICOT = 				Colour(1.0f,	0.22f,	0.0f,	1.0f);
	const Colour Colours::CORAL = 					Colour(1.0f,	0.50f,	0.31f,	1.0f);
	const Colour Colours::CORAL_PINK = 				Colour(0.97f,	0.51f,	0.47f,	1.0f);
	const Colour Colours::CORAL_RED = 				Colour(1.0f,	0.25f,	0.25f,	1.0f);
	const Colour Colours::CORDOVAN = 				Colour(0.54f,	0.25f,	0.27f,	1.0f);
	const Colour Colours::CORN = 					Colour(0.98f,	0.93f,	0.36f,	1.0f);
	const Colour Colours::CORNELL_RED = 			Colour(0.70f,	0.11f,	0.11f,	1.0f);
	const Colour Colours::CORNFLOWER_BLUE = 		Colour(0.39f,	0.58f,	0.93f,	1.0f);
	const Colour Colours::CORNSILK = 				Colour(1.0f,	0.97f,	0.86f,	1.0f);
	const Colour Colours::COSMIC_LATTE = 			Colour(1.0f,	0.97f,	0.91f,	1.0f);
	const Colour Colours::COTTON_CANDY = 			Colour(1.0f,	0.74f,	0.85f,	1.0f);
	const Colour Colours::CREAM = 					Colour(1.0f,	0.99f,	0.82f,	1.0f);
	const Colour Colours::CRIMSON = 				Colour(0.86f,	0.8f,	0.24f,	1.0f);
	const Colour Colours::CRIMSON_GLORY = 			Colour(0.75f,	0.0f,	0.20f,	1.0f);
	const Colour Colours::CYAN = 					Colour(0.0f,	1.0f,	1.0f,	1.0f);
	const Colour Colours::CYAN_PROCESS = 			Colour(0.0f,	0.72f,	0.92f,	1.0f);
	const Colour Colours::DAFFODIL = 				Colour(1.0f,	1.0f,	0.19f,	1.0f);
	const Colour Colours::DANDELION = 				Colour(0.94f,	0.88f,	0.19f,	1.0f);
	const Colour Colours::DARK_BLUE = 				Colour(0.0f,	0.0f,	0.55f,	1.0f);
	const Colour Colours::DARK_BROWN = 				Colour(0.40f,	0.26f,	0.13f,	1.0f);
	const Colour Colours::DARK_BYZANTIUM = 			Colour(0.36f,	0.22f,	0.33f,	1.0f);
	const Colour Colours::DARK_CANDY_APPLE_RED = 	Colour(0.64f,	0.0f,	0.0f,	1.0f);
	const Colour Colours::DARK_CERULEAN = 			Colour(0.3f,	0.27f,	0.49f,	1.0f);
	const Colour Colours::DARK_CHAMPAGNE = 			Colour(0.76f,	0.70f,	0.50f,	1.0f);
	const Colour Colours::DARK_CHESTNUT = 			Colour(0.60f,	0.41f,	0.38f,	1.0f);
	const Colour Colours::DARK_CORAL = 				Colour(0.80f,	0.36f,	0.27f,	1.0f);
	const Colour Colours::DARK_CYAN = 				Colour(0.0f,	0.55f,	0.55f,	1.0f);
	const Colour Colours::DARK_ELECTRIC_BLUE = 		Colour(0.33f,	0.41f,	0.47f,	1.0f);
	const Colour Colours::DARK_GOLDENROD = 			Colour(0.72f,	0.53f,	0.4f,	1.0f);
	const Colour Colours::DARK_GRAY = 				Colour(0.66f,	0.66f,	0.66f,	1.0f);
	const Colour Colours::DARK_GREEN = 				Colour(0.0f,	0.20f,	0.13f,	1.0f);
	const Colour Colours::DARK_JUNGLE_GREEN = 		Colour(0.10f,	0.14f,	0.13f,	1.0f);
	const Colour Colours::DARK_KHAKI = 				Colour(0.74f,	0.72f,	0.42f,	1.0f);
	const Colour Colours::DARK_LAVA = 				Colour(0.28f,	0.24f,	0.20f,	1.0f);
	const Colour Colours::DARK_LAVENDER = 			Colour(0.45f,	0.31f,	0.59f,	1.0f);
	const Colour Colours::DARK_MAGENTA = 			Colour(0.55f,	0.0f,	0.55f,	1.0f);
	const Colour Colours::DARK_MIDNIGHT_BLUE = 		Colour(0.0f,	0.20f,	0.40f,	1.0f);
	const Colour Colours::DARK_OLIVE_GREEN = 		Colour(0.33f,	0.42f,	0.18f,	1.0f);
	const Colour Colours::DARK_ORANGE = 			Colour(1.0f,	0.55f,	0.0f,	1.0f);
	const Colour Colours::DARK_ORCHID = 			Colour(0.60f,	0.20f,	0.80f,	1.0f);
	const Colour Colours::DARK_PASTEL_BLUE = 		Colour(0.47f,	0.62f,	0.80f,	1.0f);
	const Colour Colours::DARK_PASTEL_GREEN = 		Colour(0.1f,	0.75f,	0.24f,	1.0f);
	const Colour Colours::DARK_PASTEL_PURPLE = 		Colour(0.59f,	0.44f,	0.84f,	1.0f);
	const Colour Colours::DARK_PASTEL_RED = 		Colour(0.76f,	0.23f,	0.13f,	1.0f);
	const Colour Colours::DARK_PINK = 				Colour(0.91f,	0.33f,	0.50f,	1.0f);
	const Colour Colours::DARK_POWDER_BLUE = 		Colour(0.0f,	0.20f,	0.60f,	1.0f);
	const Colour Colours::DARK_RASPBERRY = 			Colour(0.53f,	0.15f,	0.34f,	1.0f);
	const Colour Colours::DARK_RED = 				Colour(0.55f,	0.0f,	0.0f,	1.0f);
	const Colour Colours::DARK_SALMON = 			Colour(0.91f,	0.59f,	0.48f,	1.0f);
	const Colour Colours::DARK_SCARLET = 			Colour(0.34f,	0.1f,	0.10f,	1.0f);
	const Colour Colours::DARK_SEA_GREEN = 			Colour(0.56f,	0.74f,	0.56f,	1.0f);
	const Colour Colours::DARK_SIENNA = 			Colour(0.24f,	0.8f,	0.8f,	1.0f);
	const Colour Colours::DARK_SLATE_BLUE = 		Colour(0.28f,	0.24f,	0.55f,	1.0f);
	const Colour Colours::DARK_SLATE_GRAY = 		Colour(0.18f,	0.31f,	0.31f,	1.0f);
	const Colour Colours::DARK_SPRING_GREEN = 		Colour(0.9f,	0.45f,	0.27f,	1.0f);
	const Colour Colours::DARK_TAN = 				Colour(0.57f,	0.51f,	0.32f,	1.0f);
	const Colour Colours::DARK_TANGERINE = 			Colour(1.0f,	0.66f,	0.7f,	1.0f);
	const Colour Colours::DARK_TAUPE = 				Colour(0.28f,	0.24f,	0.20f,	1.0f);
	const Colour Colours::DARK_TERRA_COTTA = 		Colour(0.80f,	0.31f,	0.36f,	1.0f);
	const Colour Colours::DARK_TURQUOISE = 			Colour(0.0f,	0.81f,	0.82f,	1.0f);
	const Colour Colours::DARK_VIOLET = 			Colour(0.58f,	0.0f,	0.83f,	1.0f);
	const Colour Colours::DARTMOUTH_GREEN = 		Colour(0.5f,	0.50f,	0.6f,	1.0f);
	const Colour Colours::DAVYS_GREY = 				Colour(0.33f,	0.33f,	0.33f,	1.0f);
	const Colour Colours::DEBIAN_RED = 				Colour(0.84f,	0.4f,	0.33f,	1.0f);
	const Colour Colours::DEEP_CARMINE = 			Colour(0.66f,	0.13f,	0.24f,	1.0f);
	const Colour Colours::DEEP_CARMINE_PINK = 		Colour(0.94f,	0.19f,	0.22f,	1.0f);
	const Colour Colours::DEEP_CARROT_ORANGE = 		Colour(0.91f,	0.41f,	0.17f,	1.0f);
	const Colour Colours::DEEP_CERISE = 			Colour(0.85f,	0.20f,	0.53f,	1.0f);
	const Colour Colours::DEEP_CHAMPAGNE = 			Colour(0.98f,	0.84f,	0.65f,	1.0f);
	const Colour Colours::DEEP_CHESTNUT = 			Colour(0.73f,	0.31f,	0.28f,	1.0f);
	const Colour Colours::DEEP_FUCHSIA = 			Colour(0.76f,	0.33f,	0.76f,	1.0f);
	const Colour Colours::DEEP_JUNGLE_GREEN = 		Colour(0.0f,	0.29f,	0.29f,	1.0f);
	const Colour Colours::DEEP_LILAC = 				Colour(0.60f,	0.33f,	0.73f,	1.0f);
	const Colour Colours::DEEP_MAGENTA = 			Colour(0.80f,	0.0f,	0.80f,	1.0f);
	const Colour Colours::DEEP_PEACH = 				Colour(1.0f,	0.80f,	0.64f,	1.0f);
	const Colour Colours::DEEP_PINK = 				Colour(1.0f,	0.8f,	0.58f,	1.0f);
	const Colour Colours::DEEP_SAFFRON = 			Colour(1.0f,	0.60f,	0.20f,	1.0f);
	const Colour Colours::DEEP_SKY_BLUE = 			Colour(0.0f,	0.75f,	1.0f,	1.0f);
	const Colour Colours::DENIM = 					Colour(0.8f,	0.38f,	0.74f,	1.0f);
	const Colour Colours::DESERT = 					Colour(0.76f,	0.60f,	0.42f,	1.0f);
	const Colour Colours::DESERT_SAND = 			Colour(0.93f,	0.79f,	0.69f,	1.0f);
	const Colour Colours::DIM_GRAY = 				Colour(0.41f,	0.41f,	0.41f, 	1.0f);
	const Colour Colours::DODGER_BLUE = 			Colour(0.12f,	0.56f,	1.0f,	1.0f);
	const Colour Colours::DOGWOOD_ROSE = 			Colour(0.84f,	0.9f,	0.41f,	1.0f);
	const Colour Colours::DOLLAR_BILL = 			Colour(0.52f,	0.73f,	0.40f,	1.0f);
	const Colour Colours::DRAB = 					Colour(0.59f,	0.44f,	0.9f,	1.0f);
	const Colour Colours::DUKE_BLUE = 				Colour(0.0f,	0.0f,	0.61f,	1.0f);
	const Colour Colours::EARTH_YELLOW = 			Colour(0.88f,	0.66f,	0.37f,	1.0f);
	const Colour Colours::ECRU = 					Colour(0.76f,	0.70f,	0.50f,	1.0f);
	const Colour Colours::EGGPLANT = 				Colour(0.38f,	0.25f,	0.32f,	1.0f);
	const Colour Colours::EGGSHELL = 				Colour(0.94f,	0.92f,	0.84f,	1.0f);
	const Colour Colours::EGYPTIAN_BLUE = 			Colour(0.6f,	0.20f,	0.65f,	1.0f);
	const Colour Colours::ELECTRIC_BLUE = 			Colour(0.49f,	0.98f,	1.0f,	1.0f);
	const Colour Colours::ELECTRIC_CRIMSON = 		Colour(1.0f,	0.0f,	0.25f,	1.0f);
	const Colour Colours::ELECTRIC_CYAN = 			Colour(0.0f,	1.0f,	1.0f,	1.0f);
	const Colour Colours::ELECTRIC_GREEN = 			Colour(0.0f,	1.0f,	0.0f,	1.0f);
	const Colour Colours::ELECTRIC_INDIGO = 		Colour(0.44f,	0.0f,	1.0f,	1.0f);
	const Colour Colours::ELECTRIC_LAVENDER = 		Colour(0.96f,	0.73f,	1.0f,	1.0f);
	const Colour Colours::ELECTRIC_LIME = 			Colour(0.80f,	1.0f,	0.0f,	1.0f);
	const Colour Colours::ELECTRIC_PURPLE = 		Colour(0.75f,	0.0f,	1.0f,	1.0f);
	const Colour Colours::ELECTRIC_ULTRAMARINE = 	Colour(0.25f,	0.0f,	1.0f,	1.0f);
	const Colour Colours::ELECTRIC_VIOLET = 		Colour(0.56f,	0.0f,	1.0f,	1.0f);
	const Colour Colours::ELECTRIC_YELLOW = 		Colour(1.0f,	1.0f,	0.0f,	1.0f);
	const Colour Colours::EMERALD = 				Colour(0.31f,	0.78f,	0.47f,	1.0f);
	const Colour Colours::ETON_BLUE = 				Colour(0.59f,	0.78f,	0.64f,	1.0f);
	const Colour Colours::FALLOW = 					Colour(0.76f,	0.60f,	0.42f,	1.0f);
	const Colour Colours::FALU_RED = 				Colour(0.50f,	0.9f,	0.9f,	1.0f);
	const Colour Colours::FANDANGO = 				Colour(0.71f,	0.20f,	0.54f,	1.0f);
	const Colour Colours::FASHION_FUCHSIA = 		Colour(0.96f,	0.0f,	0.63f,	1.0f);
	const Colour Colours::FAWN = 					Colour(0.90f,	0.67f,	0.44f,	1.0f);
	const Colour Colours::FELDGRAU = 				Colour(0.30f,	0.36f,	0.33f,	1.0f);
	const Colour Colours::FERN_GREEN = 				Colour(0.31f,	0.47f,	0.26f,	1.0f);
	const Colour Colours::FERRARI_RED = 			Colour(1.0f,	0.11f,	0.0f,	1.0f);
	const Colour Colours::FIELD_DRAB = 				Colour(0.42f,	0.33f,	0.12f,	1.0f);
	const Colour Colours::FIREBRICK = 				Colour(0.70f,	0.13f,	0.13f,	1.0f);
	const Colour Colours::FIRE_ENGINE_RED = 		Colour(0.81f,	0.9f,	0.13f,	1.0f);
	const Colour Colours::FLAME = 					Colour(0.89f,	0.35f,	0.13f,	1.0f);
	const Colour Colours::FLAMINGO_PINK = 			Colour(0.99f,	0.56f,	0.67f,	1.0f);
	const Colour Colours::FLAVESCENT = 				Colour(0.97f,	0.91f,	0.56f,	1.0f);
	const Colour Colours::FLAX = 					Colour(0.93f,	0.86f,	0.51f,	1.0f);
	const Colour Colours::FLORAL_WHITE = 			Colour(1.0f,	0.98f,	0.94f,	1.0f);
	const Colour Colours::FLUORESCENT_ORANGE = 		Colour(1.0f,	0.75f,	0.0f,	1.0f);
	const Colour Colours::FLUORESCENT_PINK = 		Colour(1.0f,	0.8f,	0.58f,	1.0f);
	const Colour Colours::FLUORESCENT_YELLOW = 		Colour(0.80f,	1.0f,	0.0f,	1.0f);
	const Colour Colours::FOLLY = 					Colour(1.0f,	0.0f,	0.31f,	1.0f);
	const Colour Colours::FOREST_GREEN_TRADITIONAL = Colour(0.0f,	0.27f,	0.13f,	1.0f);
	const Colour Colours::FOREST_GREEN_WEB = 		Colour(0.13f,	0.55f,	0.13f,	1.0f);
	const Colour Colours::FRENCH_BEIGE = 			Colour(0.65f,	0.48f,	0.36f,	1.0f);
	const Colour Colours::FRENCH_BLUE = 			Colour(0.0f,	0.45f,	0.73f,	1.0f);
	const Colour Colours::FRENCH_LILAC = 			Colour(0.53f,	0.38f,	0.56f,	1.0f);
	const Colour Colours::FRENCH_ROSE = 			Colour(0.96f,	0.29f,	0.54f,	1.0f);
	const Colour Colours::FUCHSIA = 				Colour(1.0f,	0.0f,	1.0f,	1.0f);
	const Colour Colours::FUCHSIA_PINK = 			Colour(1.0f,	0.47f,	1.0f,	1.0f);
	const Colour Colours::FULVOUS = 				Colour(0.86f,	0.52f,	0.0f,	1.0f);
	const Colour Colours::FUZZY_WUZZY = 			Colour(0.80f,	0.40f,	0.40f,	1.0f);
	const Colour Colours::GAINSBORO = 				Colour(0.86f,	0.86f,	0.86f,	1.0f);
	const Colour Colours::GAMBOGE = 				Colour(0.89f,	0.61f,	0.6f,	1.0f);
	const Colour Colours::GHOST_WHITE = 			Colour(0.97f,	0.97f,	1.0f,	1.0f);
	const Colour Colours::GINGER = 					Colour(0.69f,	0.40f,	0.0f,	1.0f);
	const Colour Colours::GLAUCOUS = 				Colour(0.38f,	0.51f,	0.71f,	1.0f);
	const Colour Colours::GLITTER = 				Colour(0.90f,	0.91f,	0.98f,	1.0f);
	const Colour Colours::GOLD_METALLIC = 			Colour(0.83f,	0.69f,	0.22f,	1.0f);
	const Colour Colours::GOLD_WEB = 				Colour(1.0f,	0.84f,	0.0f,	1.0f);
	const Colour Colours::GOLDEN_BROWN = 			Colour(0.60f,	0.40f,	0.8f,	1.0f);
	const Colour Colours::GOLDEN_POPPY = 			Colour(0.99f,	0.76f,	0.0f,	1.0f);
	const Colour Colours::GOLDEN_YELLOW = 			Colour(1.0f,	0.87f,	0.0f,	1.0f);
	const Colour Colours::GOLDENROD = 				Colour(0.85f,	0.65f,	0.13f,	1.0f);
	const Colour Colours::GRANNY_SMITH_APPLE = 		Colour(0.66f,	0.89f,	0.63f,	1.0f);
	const Colour Colours::GRAY = 					Colour(0.50f,	0.50f,	0.50f,	1.0f);
	const Colour Colours::GRAY_HTML_CSS_GRAY = 		Colour(0.50f,	0.50f,	0.50f,	1.0f);
	const Colour Colours::GRAY_X11 = 				Colour(0.75f,	0.75f,	0.75f,	1.0f);
	const Colour Colours::GRAY_ASPARAGUS = 			Colour(0.27f,	0.35f,	0.27f,	1.0f);
	const Colour Colours::GREEN = 					Colour(0.0f,	1.0f,	0.0f,	1.0f);
	const Colour Colours::GREEN_YELLOW = 			Colour(0.68f,	1.0f,	0.18f,	1.0f);
	const Colour Colours::GRULLO = 					Colour(0.66f,	0.60f,	0.53f,	1.0f);
	const Colour Colours::GUPPIE_GREEN = 			Colour(0.0f,	1.0f,	0.50f,	1.0f);
	const Colour Colours::HALAYA_UBE = 				Colour(0.40f,	0.22f,	0.33f,	1.0f);
	const Colour Colours::HAN_BLUE = 				Colour(0.27f,	0.42f,	0.81f,	1.0f);
	const Colour Colours::HAN_PURPLE = 				Colour(0.32f,	0.9f,	0.98f,	1.0f);
	const Colour Colours::HANSA_YELLOW = 			Colour(0.91f,	0.84f,	0.42f,	1.0f);
	const Colour Colours::HARLEQUIN = 				Colour(0.25f,	1.0f,	0.0f,	1.0f);
	const Colour Colours::HARVARD_CRIMSON = 		Colour(0.79f,	0.0f,	0.9f,	1.0f);
	const Colour Colours::HARVEST_GOLD = 			Colour(0.85f,	0.57f,	0.0f,	1.0f);
	const Colour Colours::HEART_GOLD = 				Colour(0.50f,	0.50f,	0.0f,	1.0f);
	const Colour Colours::HELIOTROPE = 				Colour(0.87f,	0.45f,	1.0f,	1.0f);
	const Colour Colours::HOLLYWOOD_CERISE = 		Colour(0.96f,	0.0f,	0.63f,	1.0f);
	const Colour Colours::HONEYDEW = 				Colour(0.94f,	1.0f,	0.94f,	1.0f);
	const Colour Colours::HOOKERS_GREEN = 			Colour(0.0f,	0.44f,	0.0f,	1.0f);
	const Colour Colours::HOT_MAGENTA =				Colour(1.0f,	0.11f,	0.81f,	1.0f);
	const Colour Colours::HOT_PINK = 				Colour(1.0f,	0.41f,	0.71f,	1.0f);
	const Colour Colours::HUNTER_GREEN = 			Colour(0.21f,	0.37f,	0.23f,	1.0f);
	const Colour Colours::ICEBERG = 				Colour(0.44f,	0.65f,	0.82f,	1.0f);
	const Colour Colours::ICTERINE = 				Colour(0.99f,	0.97f,	0.37f,	1.0f);
	const Colour Colours::INCHWORM = 				Colour(0.70f,	0.93f,	0.36f,	1.0f);
	const Colour Colours::INDIA_GREEN = 			Colour(0.7f,	0.53f,	0.3f,	1.0f);
	const Colour Colours::INDIAN_RED = 				Colour(0.80f,	0.36f,	0.36f,	1.0f);
	const Colour Colours::INDIAN_YELLOW = 			Colour(0.89f,	0.66f,	0.34f,	1.0f);
	const Colour Colours::INDIGO_DYE = 				Colour(0.0f,	0.25f,	0.42f,	1.0f);
	const Colour Colours::INDIGO_WEB = 				Colour(0.29f,	0.0f,	0.51f,	1.0f);
	const Colour Colours::INTERNATIONAL_KLEIN_BLUE =Colour(0.0f,	0.18f,	0.65f,	1.0f);
	const Colour Colours::INTERNATIONAL_ORANGE = 	Colour(1.0f,	0.31f,	0.0f,	1.0f);
	const Colour Colours::IRIS = 					Colour(0.35f,	0.31f,	0.81f,	1.0f);
	const Colour Colours::ISABELLINE = 				Colour(0.96f,	0.94f,	0.93f,	1.0f);
	const Colour Colours::ISLAMIC_GREEN = 			Colour(0.0f,	0.56f,	0.0f,	1.0f);
	const Colour Colours::IVORY = 					Colour(1.0f,	1.0f,	0.94f,	1.0f);
	const Colour Colours::JADE = 					Colour(0.0f,	0.66f,	0.42f,	1.0f);
	const Colour Colours::JASPER = 					Colour(0.84f,	0.23f,	0.24f,	1.0f);
	const Colour Colours::JASMINE = 				Colour(0.97f,	0.87f,	0.49f,	1.0f);
	const Colour Colours::JAZZBERRY_JAM = 			Colour(0.65f,	0.4f,	0.37f,	1.0f);
	const Colour Colours::JONQUIL = 				Colour(0.98f,	0.85f,	0.37f,	1.0f);
	const Colour Colours::JUNE_BUD = 				Colour(0.74f,	0.85f,	0.34f,	1.0f);
	const Colour Colours::JUNGLE_GREEN = 			Colour(0.16f,	0.67f,	0.53f,	1.0f);
	const Colour Colours::KELLY_GREEN = 			Colour(0.30f,	0.73f,	0.9f,	1.0f);
	const Colour Colours::KHAKI = 					Colour(0.76f,	0.69f,	0.57f,	1.0f);
	const Colour Colours::KU_CRIMSON = 				Colour(0.91f,	0.0f,	0.5f,	1.0f);
	const Colour Colours::LA_SALLE_GREEN = 			Colour(0.3f,	0.47f,	0.19f,	1.0f);
	const Colour Colours::LANGUID_LAVENDER = 		Colour(0.84f,	0.79f,	0.87f,	1.0f);
	const Colour Colours::LAPIS_LAZULI = 			Colour(0.15f,	0.38f,	0.61f,	1.0f);
	const Colour Colours::LASER_LEMON = 			Colour(1.0f,	1.0f,	0.13f,	1.0f);
	const Colour Colours::LAVA = 					Colour(0.81f,	0.6f,	0.13f,	1.0f);
	const Colour Colours::LAVENDER_FLORAL = 		Colour(0.71f,	0.49f,	0.86f,	1.0f);
	const Colour Colours::LAVENDER_WEB = 			Colour(0.90f,	0.90f,	0.98f,	1.0f);
	const Colour Colours::LAVENDER_BLUE = 			Colour(0.80f,	0.80f,	1.0f,	1.0f);
	const Colour Colours::LAVENDER_BLUSH = 			Colour(1.0f,	0.94f,	0.96f,	1.0f);
	const Colour Colours::LAVENDER_GRAY = 			Colour(0.77f,	0.76f,	0.82f,	1.0f);
	const Colour Colours::LAVENDER_INDIGO = 		Colour(0.58f,	0.34f,	0.92f,	1.0f);
	const Colour Colours::LAVENDER_MAGENTA = 		Colour(0.93f,	0.51f,	0.93f,	1.0f);
	const Colour Colours::LAVENDER_MIST = 			Colour(0.90f,	0.90f,	0.98f,	1.0f);
	const Colour Colours::LAVENDER_PINK = 			Colour(0.98f,	0.68f,	0.82f,	1.0f);
	const Colour Colours::LAVENDER_PURPLE = 		Colour(0.59f,	0.48f,	0.71f,	1.0f);
	const Colour Colours::LAVENDER_ROSE = 			Colour(0.98f,	0.63f,	0.89f,	1.0f);
	const Colour Colours::LAWN_GREEN = 				Colour(0.49f,	0.99f,	0.0f,	1.0f);
	const Colour Colours::LEMON = 					Colour(1.0f,	0.97f,	0.0f,	1.0f);
	const Colour Colours::LEMON_CHIFFON = 			Colour(1.0f,	0.98f,	0.80f,	1.0f);
	const Colour Colours::LIGHT_APRICOT = 			Colour(0.99f,	0.84f,	0.69f,	1.0f);
	const Colour Colours::LIGHT_BLUE = 				Colour(0.68f,	0.85f,	0.90f,	1.0f);
	const Colour Colours::LIGHT_BROWN = 			Colour(0.71f,	0.40f,	0.11f,	1.0f);
	const Colour Colours::LIGHT_CARMINE_PINK = 		Colour(0.90f,	0.40f,	0.38f,	1.0f);
	const Colour Colours::LIGHT_CORAL = 			Colour(0.94f,	0.50f,	0.50f,	1.0f);
	const Colour Colours::LIGHT_CORNFLOWER_BLUE = 	Colour(0.60f,	0.81f,	0.93f,	1.0f);
	const Colour Colours::LIGHT_CRIMSON = 			Colour(0.96f,	0.41f,	0.57f,	1.0f);
	const Colour Colours::LIGHT_CYAN = 				Colour(0.88f,	1.0f,	1.0f,	1.0f);
	const Colour Colours::LIGHT_FUCHSIA_PINK = 		Colour(0.98f,	0.52f,	0.94f,	1.0f);
	const Colour Colours::LIGHT_GOLDENROD_YELLOW = 	Colour(0.98f,	0.98f,	0.82f,	1.0f);
	const Colour Colours::LIGHT_GRAY = 				Colour(0.83f,	0.83f,	0.83f,	1.0f);
	const Colour Colours::LIGHT_GREEN = 			Colour(0.56f,	0.93f,	0.56f,	1.0f);
	const Colour Colours::LIGHT_KHAKI = 			Colour(0.94f,	0.90f,	0.55f,	1.0f);
	const Colour Colours::LIGHT_MAUVE = 			Colour(0.86f,	0.82f,	1.0f,	1.0f);
	const Colour Colours::LIGHT_PASTEL_PURPLE = 	Colour(0.69f,	0.61f,	0.85f,	1.0f);
	const Colour Colours::LIGHT_PINK = 				Colour(1.0f,	0.71f,	0.76f,	1.0f);
	const Colour Colours::LIGHT_SALMON = 			Colour(1.0f,	0.63f,	0.48f,	1.0f);
	const Colour Colours::LIGHT_SALMON_PINK = 		Colour(1.0f,	0.60f,	0.60f,	1.0f);
	const Colour Colours::LIGHT_SEA_GREEN = 		Colour(0.13f,	0.70f,	0.67f,	1.0f);
	const Colour Colours::LIGHT_SKY_BLUE = 			Colour(0.53f,	0.81f,	0.98f,	1.0f);
	const Colour Colours::LIGHT_SLATE_GRAY = 		Colour(0.47f,	0.53f,	0.60f,	1.0f);
	const Colour Colours::LIGHT_TAUPE = 			Colour(0.70f,	0.55f,	0.43f,	1.0f);
	const Colour Colours::LIGHT_THULIAN_PINK = 		Colour(0.90f,	0.56f,	0.67f,	1.0f);
	const Colour Colours::LIGHT_YELLOW = 			Colour(1.0f,	1.0f,	0.88f,	1.0f);
	const Colour Colours::LILAC = 					Colour(0.78f,	0.64f,	0.78f,	1.0f);
	const Colour Colours::LIME = 					Colour(0.75f,	1.0f,	0.0f,	1.0f);
	const Colour Colours::LIME_GREEN = 				Colour(0.20f,	0.80f,	0.20f,	1.0f);
	const Colour Colours::LINCOLN_GREEN = 			Colour(0.11f,	0.35f,	0.2f,	1.0f);
	const Colour Colours::LINEN = 					Colour(0.98f,	0.94f,	0.90f,	1.0f);
	const Colour Colours::LIVER = 					Colour(0.33f,	0.29f,	0.31f,	1.0f);
	const Colour Colours::LUST = 					Colour(0.90f,	0.13f,	0.13f,	1.0f);
	const Colour Colours::MAGENTA = 				Colour(1.0f,	0.0f,	1.0f,	1.0f);
	const Colour Colours::MAGENTA_DYE = 			Colour(0.79f,	0.8f,	0.48f,	1.0f);
	const Colour Colours::MAGENTA_PROCESS = 		Colour(1.0f,	0.0f,	0.56f,	1.0f);
	const Colour Colours::MAGIC_MINT = 				Colour(0.67f,	0.94f,	0.82f,	1.0f);
	const Colour Colours::MAGNOLIA = 				Colour(0.97f,	0.96f,	1.0f,	1.0f);
	const Colour Colours::MAHOGANY = 				Colour(0.75f,	0.25f,	0.0f,	1.0f);
	const Colour Colours::MAIZE = 					Colour(0.98f,	0.93f,	0.37f,	1.0f);
	const Colour Colours::MAJORELLE_BLUE = 			Colour(0.38f,	0.31f,	0.86f,	1.0f);
	const Colour Colours::MALACHITE = 				Colour(0.4f,	0.85f,	0.32f,	1.0f);
	const Colour Colours::MANATEE = 				Colour(0.59f,	0.60f,	0.67f,	1.0f);
	const Colour Colours::MANGO_TANGO = 			Colour(1.0f,	0.51f,	0.26f,	1.0f);
	const Colour Colours::MAROON = 					Colour(0.50f,	0.0f,	0.0f,	1.0f);
	const Colour Colours::MAROON_X11 = 				Colour(0.69f,	0.19f,	0.38f,	1.0f);
	const Colour Colours::MAUVE = 					Colour(0.88f,	0.69f,	1.0f,	1.0f);
	const Colour Colours::MAUVE_TAUPE = 			Colour(0.57f,	0.37f,	0.43f,	1.0f);
	const Colour Colours::MAUVELOUS = 				Colour(0.94f,	0.60f,	0.67f,	1.0f);
	const Colour Colours::MAYA_BLUE = 				Colour(0.45f,	0.76f,	0.98f,	1.0f);
	const Colour Colours::MEAT_BROWN = 				Colour(0.90f,	0.72f,	0.23f,	1.0f);
	const Colour Colours::MEDIUM_AQUAMARINE = 		Colour(0.40f,	0.80f,	0.67f,	1.0f);
	const Colour Colours::MEDIUM_BLUE = 			Colour(0.0f,	0.0f,	0.80f,	1.0f);
	const Colour Colours::MEDIUM_CANDY_APPLE_RED = 	Colour(0.89f,	0.2f,	0.17f,	1.0f);
	const Colour Colours::MEDIUM_CARMINE = 			Colour(0.69f,	0.25f,	0.21f,	1.0f);
	const Colour Colours::MEDIUM_CHAMPAGNE = 		Colour(0.95f,	0.90f,	0.67f,	1.0f);
	const Colour Colours::MEDIUM_ELECTRIC_BLUE = 	Colour(0.1f,	0.31f,	0.59f,	1.0f);
	const Colour Colours::MEDIUM_JUNGLE_GREEN = 	Colour(0.11f,	0.21f,	0.18f,	1.0f);
	const Colour Colours::MEDIUM_LAVENDER_MAGENTA = Colour(0.80f,	0.60f,	0.80f,	1.0f);
	const Colour Colours::MEDIUM_ORCHID = 			Colour(0.73f,	0.33f,	0.83f,	1.0f);
	const Colour Colours::MEDIUM_PERSIAN_BLUE = 	Colour(0.0f,	0.40f,	0.65f,	1.0f);
	const Colour Colours::MEDIUM_PURPLE = 			Colour(0.58f,	0.44f,	0.86f,	1.0f);
	const Colour Colours::MEDIUM_RED_VIOLET = 		Colour(0.73f,	0.20f,	0.52f,	1.0f);
	const Colour Colours::MEDIUM_SEA_GREEN = 		Colour(0.24f,	0.70f,	0.44f,	1.0f);
	const Colour Colours::MEDIUM_SLATE_BLUE = 		Colour(0.48f,	0.41f,	0.93f,	1.0f);
	const Colour Colours::MEDIUM_SPRING_BUD = 		Colour(0.79f,	0.86f,	0.54f,	1.0f);
	const Colour Colours::MEDIUM_SPRING_GREEN = 	Colour(0.0f,	0.98f,	0.60f,	1.0f);
	const Colour Colours::MEDIUM_TAUPE = 			Colour(0.40f,	0.30f,	0.28f,	1.0f);
	const Colour Colours::MEDIUM_TEAL_BLUE = 		Colour(0.0f,	0.33f,	0.71f,	1.0f);
	const Colour Colours::MEDIUM_TURQUOISE = 		Colour(0.28f,	0.82f,	0.80f,	1.0f);
	const Colour Colours::MEDIUM_VIOLET_RED = 		Colour(0.78f,	0.8f,	0.52f,	1.0f);
	const Colour Colours::MELON = 					Colour(0.99f,	0.74f,	0.71f,	1.0f);
	const Colour Colours::MIDNIGHT_BLUE = 			Colour(0.10f,	0.10f,	0.44f,	1.0f);
	const Colour Colours::MIDNIGHT_GREEN = 			Colour(0.0f,	0.29f,	0.33f,	1.0f);
	const Colour Colours::MIKADO_YELLOW = 			Colour(1.0f,	0.77f,	0.5f,	1.0f);
	const Colour Colours::MINT = 					Colour(0.24f,	0.71f,	0.54f,	1.0f);
	const Colour Colours::MINT_CREAM = 				Colour(0.96f,	1.0f,	0.98f,	1.0f);
	const Colour Colours::MINT_GREEN = 				Colour(0.60f,	1.0f,	0.60f,	1.0f);
	const Colour Colours::MISTY_ROSE = 				Colour(1.0f,	0.89f,	0.88f,	1.0f);
	const Colour Colours::MOCCASIN = 				Colour(0.98f,	0.92f,	0.84f,	1.0f);
	const Colour Colours::MODE_BEIGE = 				Colour(0.59f,	0.44f,	0.9f,	1.0f);
	const Colour Colours::MOONSTONE_BLUE = 			Colour(0.45f,	0.66f,	0.76f,	1.0f);
	const Colour Colours::MORDANT_RED_19 = 			Colour(0.68f,	0.5f,	0.0f,	1.0f);
	const Colour Colours::MOSS_GREEN = 				Colour(0.68f,	0.87f,	0.68f,	1.0f);
	const Colour Colours::MOUNTAIN_MEADOW = 		Colour(0.19f,	0.73f,	0.56f,	1.0f);
	const Colour Colours::MOUNTBATTEN_PINK = 		Colour(0.60f,	0.48f,	0.55f,	1.0f);
	const Colour Colours::MULBERRY = 				Colour(0.77f,	0.29f,	0.55f,	1.0f);
	const Colour Colours::MUSTARD = 				Colour(1.0f,	0.86f,	0.35f,	1.0f);
	const Colour Colours::MYRTLE = 					Colour(0.13f,	0.26f,	0.12f,	1.0f);
	const Colour Colours::MSU_GREEN = 				Colour(0.9f,	0.27f,	0.23f,	1.0f);
	const Colour Colours::NADESHIKO_PINK = 			Colour(0.96f,	0.68f,	0.78f,	1.0f);
	const Colour Colours::NAPIER_GREEN = 			Colour(0.16f,	0.50f,	0.0f,	1.0f);
	const Colour Colours::NAPLES_YELLOW = 			Colour(0.98f,	0.85f,	0.37f,	1.0f);
	const Colour Colours::NAVAJO_WHITE = 			Colour(1.0f,	0.87f,	0.68f,	1.0f);
	const Colour Colours::NAVY_BLUE = 				Colour(0.0f,	0.0f,	0.50f,	1.0f);
	const Colour Colours::NEON_CARROT = 			Colour(1.0f,	0.64f,	0.26f,	1.0f);
	const Colour Colours::NEON_FUCHSIA = 			Colour(1.0f,	0.25f,	0.39f,	1.0f);
	const Colour Colours::NEON_GREEN = 				Colour(0.22f,	0.88f,	0.8f,	1.0f);
	const Colour Colours::NON_PHOTO_BLUE = 			Colour(0.64f,	0.87f,	0.93f,	1.0f);
	const Colour Colours::OCEAN_BOAT_BLUE = 		Colour(0.0f,	0.47f,	0.75f,	1.0f);
	const Colour Colours::OCHRE = 					Colour(0.80f,	0.47f,	0.13f,	1.0f);
	const Colour Colours::OFFICE_GREEN = 			Colour(0.0f,	0.50f,	0.0f,	1.0f);
	const Colour Colours::OLD_GOLD = 				Colour(0.81f,	0.71f,	0.23f,	1.0f);
	const Colour Colours::OLD_LACE = 				Colour(0.99f,	0.96f,	0.90f,	1.0f);
	const Colour Colours::OLD_LAVENDER = 			Colour(0.47f,	0.41f,	0.47f,	1.0f);
	const Colour Colours::OLD_MAUVE = 				Colour(0.40f,	0.19f,	0.28f,	1.0f);
	const Colour Colours::OLD_ROSE = 				Colour(0.75f,	0.50f,	0.51f,	1.0f);
	const Colour Colours::OLIVE = 					Colour(0.50f,	0.50f,	0.0f,	1.0f);
	const Colour Colours::OLIVE_DRAB_3 =			Colour(0.42f,	0.56f,	0.14f,	1.0f);
	const Colour Colours::OLIVE_DRAB_7 =			Colour(0.24f,	0.20f,	0.12f,	1.0f);
	const Colour Colours::OLIVINE = 				Colour(0.60f,	0.73f,	0.45f,	1.0f);
	const Colour Colours::ONYX = 					Colour(0.6f,	0.6f,	0.6f,	1.0f);
	const Colour Colours::OPERA_MAUVE = 			Colour(0.72f,	0.52f,	0.65f,	1.0f);
	const Colour Colours::ORANGE = 					Colour(1.0f,	0.50f,	0.0f,	1.0f);
	const Colour Colours::ORANGE_RYB = 				Colour(0.98f,	0.60f,	0.1f,	1.0f);
	const Colour Colours::ORANGE_WEB = 				Colour(1.0f,	0.65f,	0.0f,	1.0f);
	const Colour Colours::ORANGE_PEEL = 			Colour(1.0f,	0.62f,	0.0f,	1.0f);
	const Colour Colours::ORANGE_RED = 				Colour(1.0f,	0.27f,	0.0f,	1.0f);
	const Colour Colours::ORCHID = 					Colour(0.85f,	0.44f,	0.84f,	1.0f);
	const Colour Colours::OTTER_BROWN = 			Colour(0.40f,	0.26f,	0.13f,	1.0f);
	const Colour Colours::OUTER_SPACE = 			Colour(0.25f,	0.29f,	0.30f,	1.0f);
	const Colour Colours::OUTRAGEOUS_ORANGE = 		Colour(1.0f,	0.43f,	0.29f,	1.0f);
	const Colour Colours::OXFORD_BLUE = 			Colour(0.0f,	0.13f,	0.28f,	1.0f);
	const Colour Colours::OU_CRIMSON_RED = 			Colour(0.60f,	0.0f,	0.0f,	1.0f);
	const Colour Colours::PAKISTAN_GREEN = 			Colour(0.0f,	0.40f,	0.0f,	1.0f);
	const Colour Colours::PALATINATE_BLUE = 		Colour(0.15f,	0.23f,	0.89f,	1.0f);
	const Colour Colours::PALATINATE_PURPLE = 		Colour(0.41f,	0.16f,	0.38f,	1.0f);
	const Colour Colours::PALE_AQUA = 				Colour(0.74f,	0.83f,	0.90f,	1.0f);
	const Colour Colours::PALE_BLUE = 				Colour(0.69f,	0.93f,	0.93f,	1.0f);
	const Colour Colours::PALE_BROWN = 				Colour(0.60f,	0.46f,	0.33f,	1.0f);
	const Colour Colours::PALE_CARMINE = 			Colour(0.69f,	0.25f,	0.21f,	1.0f);
	const Colour Colours::PALE_CERULEAN = 			Colour(0.61f,	0.77f,	0.89f,	1.0f);
	const Colour Colours::PALE_CHESTNUT = 			Colour(0.87f,	0.68f,	0.69f,	1.0f);
	const Colour Colours::PALE_COPPER = 			Colour(0.85f,	0.54f,	0.40f,	1.0f);
	const Colour Colours::PALE_CORNFLOWER_BLUE = 	Colour(0.67f,	0.80f,	0.94f,	1.0f);
	const Colour Colours::PALE_GOLD = 				Colour(0.90f,	0.75f,	0.54f,	1.0f);
	const Colour Colours::PALE_GOLDENROD = 			Colour(0.93f,	0.91f,	0.67f,	1.0f);
	const Colour Colours::PALE_GREEN = 				Colour(0.60f,	0.98f,	0.60f,	1.0f);
	const Colour Colours::PALE_MAGENTA = 			Colour(0.98f,	0.52f,	0.90f,	1.0f);
	const Colour Colours::PALE_PINK = 				Colour(0.98f,	0.85f,	0.87f,	1.0f);
	const Colour Colours::PALE_PLUM = 				Colour(0.80f,	0.60f,	0.80f,	1.0f);
	const Colour Colours::PALE_RED_VIOLET = 		Colour(0.86f,	0.44f,	0.58f,	1.0f);
	const Colour Colours::PALE_ROBIN_EGG_BLUE = 	Colour(0.59f,	0.87f,	0.82f,	1.0f);
	const Colour Colours::PALE_SILVER = 			Colour(0.79f,	0.75f,	0.73f,	1.0f);
	const Colour Colours::PALE_SPRING_BUD = 		Colour(0.93f,	0.92f,	0.74f,	1.0f);
	const Colour Colours::PALE_TAUPE = 				Colour(0.74f,	0.60f,	0.49f,	1.0f);
	const Colour Colours::PALE_VIOLET_RED = 		Colour(0.86f,	0.44f,	0.58f,	1.0f);
	const Colour Colours::PANSY_PURPLE = 			Colour(0.47f,	0.9f,	0.29f,	1.0f);
	const Colour Colours::PAPAYA_WHIP = 			Colour(1.0f,	0.94f,	0.84f,	1.0f);
	const Colour Colours::PARIS_GREEN = 			Colour(0.31f,	0.78f,	0.47f,	1.0f);
	const Colour Colours::PASTEL_BLUE = 			Colour(0.68f,	0.78f,	0.81f,	1.0f);
	const Colour Colours::PASTEL_BROWN = 			Colour(0.51f,	0.41f,	0.33f,	1.0f);
	const Colour Colours::PASTEL_GRAY = 			Colour(0.81f,	0.81f,	0.77f,	1.0f);
	const Colour Colours::PASTEL_GREEN = 			Colour(0.47f,	0.87f,	0.47f,	1.0f);
	const Colour Colours::PASTEL_MAGENTA = 			Colour(0.96f,	0.60f,	0.76f,	1.0f);
	const Colour Colours::PASTEL_ORANGE = 			Colour(1.0f,	0.70f,	0.28f,	1.0f);
	const Colour Colours::PASTEL_PINK = 			Colour(1.0f,	0.82f,	0.86f,	1.0f);
	const Colour Colours::PASTEL_PURPLE = 			Colour(0.70f,	0.62f,	0.71f,	1.0f);
	const Colour Colours::PASTEL_RED = 				Colour(1.0f,	0.41f,	0.38f,	1.0f);
	const Colour Colours::PASTEL_VIOLET = 			Colour(0.80f,	0.60f,	0.79f,	1.0f);
	const Colour Colours::PASTEL_YELLOW = 			Colour(0.99f,	0.99f,	0.59f,	1.0f);
	const Colour Colours::PATRIARCH = 				Colour(0.50f,	0.0f,	0.50f,	1.0f);
	const Colour Colours::PAYNES_GREY = 			Colour(0.25f,	0.25f,	0.28f,	1.0f);
	const Colour Colours::PEACH = 					Colour(1.0f,	0.90f,	0.71f,	1.0f);
	const Colour Colours::PEACH_ORANGE = 			Colour(1.0f,	0.80f,	0.60f,	1.0f);
	const Colour Colours::PEACH_PUFF = 				Colour(1.0f,	0.85f,	0.73f,	1.0f);
	const Colour Colours::PEACH_YELLOW = 			Colour(0.98f,	0.87f,	0.68f,	1.0f);
	const Colour Colours::PEAR = 					Colour(0.82f,	0.89f,	0.19f,	1.0f);
	const Colour Colours::PEARL = 					Colour(0.94f,	0.92f,	0.84f,	1.0f);
	const Colour Colours::PEARL_AQUA = 				Colour(0.53f,	0.85f,	0.75f,	1.0f);
	const Colour Colours::PERIDOT = 				Colour(0.90f,	0.89f,	0.0f,	1.0f);
	const Colour Colours::PERIWINKLE = 				Colour(0.80f,	0.80f,	1.0f,	1.0f);
	const Colour Colours::PERSIAN_BLUE = 			Colour(0.11f,	0.22f,	0.73f,	1.0f);
	const Colour Colours::PERSIAN_GREEN = 			Colour(0.0f,	0.65f,	0.58f,	1.0f);
	const Colour Colours::PERSIAN_INDIGO = 			Colour(0.20f,	0.7f,	0.48f,	1.0f);
	const Colour Colours::PERSIAN_ORANGE = 			Colour(0.85f,	0.56f,	0.35f,	1.0f);
	const Colour Colours::PERSIAN_PINK = 			Colour(0.97f,	0.50f,	0.75f,	1.0f);
	const Colour Colours::PERSIAN_PLUM = 			Colour(0.44f,	0.11f,	0.11f,	1.0f);
	const Colour Colours::PERSIAN_RED = 			Colour(0.80f,	0.20f,	0.20f,	1.0f);
	const Colour Colours::PERSIAN_ROSE = 			Colour(1.0f,	0.16f,	0.64f,	1.0f);
	const Colour Colours::PERSIMMON = 				Colour(0.93f,	0.35f,	0.0f,	1.0f);
	const Colour Colours::PHLOX = 					Colour(0.87f,	0.0f,	1.0f,	1.0f);
	const Colour Colours::PHTHALO_BLUE = 			Colour(0.0f,	0.6f,	0.54f,	1.0f);
	const Colour Colours::PHTHALO_GREEN = 			Colour(0.7f,	0.21f,	0.14f,	1.0f);
	const Colour Colours::PIGGY_PINK = 				Colour(0.99f,	0.87f,	0.90f,	1.0f);
	const Colour Colours::PINE_GREEN = 				Colour(0.0f,	0.47f,	0.44f,	1.0f);
	const Colour Colours::PINK = 					Colour(1.0f,	0.75f,	0.80f,	1.0f);
	const Colour Colours::PINK_ORANGE = 			Colour(1.0f,	0.60f,	0.40f,	1.0f);
	const Colour Colours::PINK_PEARL = 				Colour(0.91f,	0.67f,	0.81f,	1.0f);
	const Colour Colours::PINK_SHERBET = 			Colour(0.97f,	0.56f,	0.65f,	1.0f);
	const Colour Colours::PISTACHIO = 				Colour(0.58f,	0.77f,	0.45f,	1.0f);
	const Colour Colours::PLATINUM = 				Colour(0.90f,	0.89f,	0.89f,	1.0f);
	const Colour Colours::PLUM = 					Colour(0.56f,	0.27f,	0.52f,	1.0f);
	const Colour Colours::PLUM_WEB = 				Colour(0.80f,	0.60f,	0.80f,	1.0f);
	const Colour Colours::PORTLAND_ORANGE = 		Colour(1.0f,	0.35f,	0.21f,	1.0f);
	const Colour Colours::POWDER_BLUE = 			Colour(0.69f,	0.88f,	0.90f,	1.0f);
	const Colour Colours::PRINCETON_ORANGE = 		Colour(1.0f,	0.56f,	0.0f,	1.0f);
	const Colour Colours::PRUNE = 					Colour(0.44f,	0.11f,	0.11f,	1.0f);
	const Colour Colours::PRUSSIAN_BLUE = 			Colour(0.0f,	0.19f,	0.33f,	1.0f);
	const Colour Colours::PSYCHEDELIC_PURPLE = 		Colour(0.87f,	0.0f,	1.0f,	1.0f);
	const Colour Colours::PUCE = 					Colour(0.80f,	0.53f,	0.60f,	1.0f);
	const Colour Colours::PUMPKIN = 				Colour(1.0f,	0.46f,	0.9f,	1.0f);
	const Colour Colours::PURPLE = 					Colour(0.50f,	0.0f,	0.50f,	1.0f);
	const Colour Colours::PURPLE_MUNSELL = 			Colour(0.62f,	0.0f,	0.77f,	1.0f);
	const Colour Colours::PURPLE_X11 = 				Colour(0.63f,	0.36f,	0.94f,	1.0f);
	const Colour Colours::PURPLE_HEART = 			Colour(0.41f,	0.21f,	0.61f,	1.0f);
	const Colour Colours::PURPLE_MOUNTAIN_MAJESTY = Colour(0.59f,	0.47f,	0.71f,	1.0f);
	const Colour Colours::PURPLE_PIZZAZZ = 			Colour(1.0f,	0.31f,	0.85f,	1.0f);
	const Colour Colours::PURPLE_TAUPE = 			Colour(0.31f,	0.25f,	0.30f,	1.0f);
	const Colour Colours::QUARTZ = 					Colour(0.32f,	0.28f,	0.31f,	1.0f);
	const Colour Colours::RADICAL_RED = 			Colour(1.0f,	0.21f,	0.37f,	1.0f);
	const Colour Colours::RASPBERRY = 				Colour(0.89f,	0.4f,	0.36f,	1.0f);
	const Colour Colours::RASPBERRY_GLACE = 		Colour(0.57f,	0.37f,	0.43f,	1.0f);
	const Colour Colours::RASPBERRY_PINK = 			Colour(0.89f,	0.31f,	0.61f,	1.0f);
	const Colour Colours::RASPBERRY_ROSE = 			Colour(0.70f,	0.27f,	0.42f,	1.0f);
	const Colour Colours::RAW_UMBER = 				Colour(0.51f,	0.40f,	0.27f,	1.0f);
	const Colour Colours::RAZZLE_DAZZLE_ROSE = 		Colour(1.0f,	0.20f,	0.80f,	1.0f);
	const Colour Colours::RAZZMATAZZ = 				Colour(0.89f,	0.15f,	0.42f,	1.0f);
	const Colour Colours::RED = 					Colour(1.0f,	0.0f,	0.0f,	1.0f);
	const Colour Colours::RED_MUNSELL = 			Colour(0.95f,	0.0f,	0.24f,	1.0f);
	const Colour Colours::RED_NCS = 				Colour(0.77f,	0.1f,	0.20f,	1.0f);
	const Colour Colours::RED_PIGMENT = 			Colour(0.93f,	0.11f,	0.14f,	1.0f);
	const Colour Colours::RED_RYB = 				Colour(1.0f,	0.15f,	0.7f,	1.0f);
	const Colour Colours::RED_BROWN = 				Colour(0.65f,	0.16f,	0.16f,	1.0f);
	const Colour Colours::RED_VIOLET = 				Colour(0.78f,	0.8f,	0.52f,	1.0f);
	const Colour Colours::REDWOOD = 				Colour(0.67f,	0.31f,	0.32f,	1.0f);
	const Colour Colours::REGALIA = 				Colour(0.32f,	0.18f,	0.50f,	1.0f);
	const Colour Colours::RICH_BLACK = 				Colour(0.0f,	0.25f,	0.25f,	1.0f);
	const Colour Colours::RICH_BRILLIANT_LAVENDER = Colour(0.95f,	0.65f,	1.0f,	1.0f);
	const Colour Colours::RICH_CARMINE = 			Colour(0.84f,	0.0f,	0.25f,	1.0f);
	const Colour Colours::RICH_ELECTRIC_BLUE = 		Colour(0.3f,	0.57f,	0.82f,	1.0f);
	const Colour Colours::RICH_LAVENDER = 			Colour(0.67f,	0.38f,	0.80f,	1.0f);
	const Colour Colours::RICH_LILAC = 				Colour(0.71f,	0.40f,	0.82f,	1.0f);
	const Colour Colours::RICH_MAROON = 			Colour(0.69f,	0.19f,	0.38f,	1.0f);
	const Colour Colours::RIFLE_GREEN = 			Colour(0.25f,	0.28f,	0.20f,	1.0f);
	const Colour Colours::ROBIN_EGG_BLUE = 			Colour(0.0f,	0.80f,	0.80f,	1.0f);
	const Colour Colours::ROSE = 					Colour(1.0f,	0.0f,	0.50f,	1.0f);
	const Colour Colours::ROSE_BONBON = 			Colour(0.98f,	0.26f,	0.62f,	1.0f);
	const Colour Colours::ROSE_EBONY = 				Colour(0.40f,	0.30f,	0.28f,	1.0f);
	const Colour Colours::ROSE_GOLD = 				Colour(0.72f,	0.43f,	0.47f,	1.0f);
	const Colour Colours::ROSE_MADDER = 			Colour(0.89f,	0.15f,	0.21f,	1.0f);
	const Colour Colours::ROSE_PINK = 				Colour(1.0f,	0.40f,	0.80f,	1.0f);
	const Colour Colours::ROSE_QUARTZ = 			Colour(0.67f,	0.60f,	0.66f,	1.0f);
	const Colour Colours::ROSE_TAUPE = 				Colour(0.56f,	0.36f,	0.36f,	1.0f);
	const Colour Colours::ROSE_VALE = 				Colour(0.67f,	0.31f,	0.32f,	1.0f);
	const Colour Colours::ROSEWOOD = 				Colour(0.40f,	0.0f,	0.4f,	1.0f);
	const Colour Colours::ROSSO_CORSA = 			Colour(0.83f,	0.0f,	0.0f,	1.0f);
	const Colour Colours::ROSY_BROWN = 				Colour(0.74f,	0.56f,	0.56f,	1.0f);
	const Colour Colours::ROYAL_AZURE = 			Colour(0.0f,	0.22f,	0.66f,	1.0f);
	const Colour Colours::ROYAL_BLUE = 				Colour(0.0f,	0.14f,	0.40f,	1.0f);
	const Colour Colours::ROYAL_BLUE_WEB = 			Colour(0.25f,	0.41f,	0.88f,	1.0f);
	const Colour Colours::ROYAL_FUCHSIA = 			Colour(0.79f,	0.17f,	0.57f,	1.0f);
	const Colour Colours::ROYAL_PURPLE = 			Colour(0.47f,	0.32f,	0.66f,	1.0f);
	const Colour Colours::RUBY = 					Colour(0.88f,	0.7f,	0.37f,	1.0f);
	const Colour Colours::RUDDY = 					Colour(1.0f,	0.0f,	0.16f,	1.0f);
	const Colour Colours::RUDDY_BROWN = 			Colour(0.73f,	0.40f,	0.16f,	1.0f);
	const Colour Colours::RUDDY_PINK = 				Colour(0.88f,	0.56f,	0.59f,	1.0f);
	const Colour Colours::RUFOUS = 					Colour(0.66f,	0.11f,	0.3f,	1.0f);
	const Colour Colours::RUSSET = 					Colour(0.50f,	0.27f,	0.11f,	1.0f);
	const Colour Colours::RUST = 					Colour(0.72f,	0.25f,	0.5f,	1.0f);
	const Colour Colours::SACRAMENTO_STATE_GREEN = 	Colour(0.0f,	0.34f,	0.25f,	1.0f);
	const Colour Colours::SADDLE_BROWN = 			Colour(0.55f,	0.27f,	0.7f,	1.0f);
	const Colour Colours::SAFETY_ORANGE = 			Colour(1.0f,	0.40f,	0.0f,	1.0f);
	const Colour Colours::SAFFRON = 				Colour(0.96f,	0.77f,	0.19f,	1.0f);
	const Colour Colours::ST_PATRICKS_BLUE = 		Colour(0.14f,	0.16f,	0.48f,	1.0f);
	const Colour Colours::SALMON = 					Colour(1.0f,	0.55f,	0.41f,	1.0f);
	const Colour Colours::SALMON_PINK = 			Colour(1.0f,	0.57f,	0.64f,	1.0f);
	const Colour Colours::SAND = 					Colour(0.76f,	0.70f,	0.50f,	1.0f);
	const Colour Colours::SAND_DUNE = 				Colour(0.59f,	0.44f,	0.9f,	1.0f);
	const Colour Colours::SANDSTORM = 				Colour(0.93f,	0.84f,	0.25f,	1.0f);
	const Colour Colours::SANDY_BROWN = 			Colour(0.96f,	0.64f,	0.38f,	1.0f);
	const Colour Colours::SANDY_TAUPE = 			Colour(0.59f,	0.44f,	0.9f,	1.0f);
	const Colour Colours::SANGRIA = 				Colour(0.57f,	0.0f,	0.4f,	1.0f);
	const Colour Colours::SAP_GREEN = 				Colour(0.31f,	0.49f,	0.16f,	1.0f);
	const Colour Colours::SAPPHIRE = 				Colour(0.3f,	0.15f,	0.40f,	1.0f);
	const Colour Colours::SATIN_SHEEN_GOLD = 		Colour(0.80f,	0.63f,	0.21f,	1.0f);
	const Colour Colours::SCARLET = 				Colour(1.0f,	0.14f,	0.0f,	1.0f);
	const Colour Colours::SCHOOL_BUS_YELLOW = 		Colour(1.0f,	0.85f,	0.0f,	1.0f);
	const Colour Colours::SCREAMIN_GREEN = 			Colour(0.46f,	1.0f,	0.44f,	1.0f);
	const Colour Colours::SEA_GREEN = 				Colour(0.18f,	0.55f,	0.34f,	1.0f);
	const Colour Colours::SEAL_BROWN = 				Colour(0.20f,	0.8f,	0.8f,	1.0f);
	const Colour Colours::SEASHELL = 				Colour(1.0f,	0.96f,	0.93f,	1.0f);
	const Colour Colours::SELECTIVE_YELLOW = 		Colour(1.0f,	0.73f,	0.0f,	1.0f);
	const Colour Colours::SEPIA = 					Colour(0.44f,	0.26f,	0.8f,	1.0f);
	const Colour Colours::SHADOW = 					Colour(0.54f,	0.47f,	0.36f,	1.0f);
	const Colour Colours::SHAMROCK_GREEN = 			Colour(0.0f,	0.62f,	0.38f,	1.0f);
	const Colour Colours::SHOCKING_PINK = 			Colour(0.99f,	0.6f,	0.75f,	1.0f);
	const Colour Colours::SIENNA = 					Colour(0.53f,	0.18f,	0.9f,	1.0f);
	const Colour Colours::SILVER = 					Colour(0.75f,	0.75f,	0.75f,	1.0f);
	const Colour Colours::SINOPIA = 				Colour(0.80f,	0.25f,	0.4f,	1.0f);
	const Colour Colours::SKOBELOFF = 				Colour(0.0f,	0.48f,	0.45f,	1.0f);
	const Colour Colours::SKY_BLUE = 				Colour(0.53f,	0.81f,	0.92f,	1.0f);
	const Colour Colours::SKY_MAGENTA = 			Colour(0.81f,	0.44f,	0.69f,	1.0f);
	const Colour Colours::SLATE_BLUE = 				Colour(0.42f,	0.35f,	0.80f,	1.0f);
	const Colour Colours::SLATE_GRAY = 				Colour(0.44f,	0.50f,	0.56f,	1.0f);
	const Colour Colours::SMALT = 					Colour(0.0f,	0.20f,	0.60f,	1.0f);
	const Colour Colours::SMOKEY_TOPAZ = 			Colour(0.58f,	0.25f,	0.3f,	1.0f);
	const Colour Colours::SMOKY_BLACK = 			Colour(0.6f,	0.5f,	0.3f,	1.0f);
	const Colour Colours::SNOW = 					Colour(1.0f,	0.98f,	0.98f,	1.0f);
	const Colour Colours::SPIRO_DISCO_BALL = 		Colour(0.6f,	0.75f,	0.99f,	1.0f);
	const Colour Colours::SPLASHED_WHITE = 			Colour(1.0f,	0.99f,	1.0f,	1.0f);
	const Colour Colours::SPRING_BUD = 				Colour(0.65f,	0.99f,	0.0f,	1.0f);
	const Colour Colours::SPRING_GREEN = 			Colour(0.0f,	1.0f,	0.50f,	1.0f);
	const Colour Colours::STEEL_BLUE = 				Colour(0.27f,	0.51f,	0.71f,	1.0f);
	const Colour Colours::STIL_DE_GRAIN_YELLOW = 	Colour(0.98f,	0.85f,	0.37f,	1.0f);
	const Colour Colours::STIZZA = 					Colour(0.60f,	0.0f,	0.0f,	1.0f);
	const Colour Colours::STRAW = 					Colour(0.89f,	0.85f,	0.44f,	1.0f);
	const Colour Colours::SUNGLOW = 				Colour(1.0f,	0.80f,	0.20f,	1.0f);
	const Colour Colours::SUNSET = 					Colour(0.98f,	0.84f,	0.65f,	1.0f);
	const Colour Colours::TAN = 					Colour(0.82f,	0.71f,	0.55f,	1.0f);
	const Colour Colours::TANGELO = 				Colour(0.98f,	0.30f,	0.0f,	1.0f);
	const Colour Colours::TANGERINE = 				Colour(0.95f,	0.52f,	0.0f,	1.0f);
	const Colour Colours::TANGERINE_YELLOW = 		Colour(1.0f,	0.80f,	0.0f,	1.0f);
	const Colour Colours::TAUPE = 					Colour(0.28f,	0.24f,	0.20f,	1.0f);
	const Colour Colours::TAUPE_GRAY = 				Colour(0.55f,	0.52f,	0.54f,	1.0f);
	const Colour Colours::TEA_GREEN = 				Colour(0.82f,	0.94f,	0.75f,	1.0f);
	const Colour Colours::TEA_ROSE_ORANGE = 		Colour(0.97f,	0.51f,	0.47f,	1.0f);
	const Colour Colours::TEA_ROSE = 				Colour(0.96f,	0.76f,	0.76f,	1.0f);
	const Colour Colours::TEAL = 					Colour(0.0f,	0.50f,	0.50f,	1.0f);
	const Colour Colours::TEAL_BLUE = 				Colour(0.21f,	0.46f,	0.53f,	1.0f);
	const Colour Colours::TEAL_GREEN = 				Colour(0.0f,	0.51f,	0.50f,	1.0f);
	const Colour Colours::TENNE_TAWNY = 			Colour(0.80f,	0.34f,	0.0f,	1.0f);
	const Colour Colours::TERRA_COTTA = 			Colour(0.89f,	0.45f,	0.36f,	1.0f);
	const Colour Colours::THISTLE = 				Colour(0.85f,	0.75f,	0.85f,	1.0f);
	const Colour Colours::THULIAN_PINK = 			Colour(0.87f,	0.44f,	0.63f,	1.0f);
	const Colour Colours::TICKLE_ME_PINK = 			Colour(0.99f,	0.54f,	0.67f,	1.0f);
	const Colour Colours::TIFFANY_BLUE = 			Colour(0.4f,	0.73f,	0.71f,	1.0f);
	const Colour Colours::TIGERS_EYE = 				Colour(0.88f,	0.55f,	0.24f,	1.0f);
	const Colour Colours::TIMBERWOLF = 				Colour(0.86f,	0.84f,	0.82f,	1.0f);
	const Colour Colours::TITANIUM_YELLOW = 		Colour(0.93f,	0.90f,	0.0f,	1.0f);
	const Colour Colours::TOMATO = 					Colour(1.0f,	0.39f,	0.28f,	1.0f);
	const Colour Colours::TOOLBOX = 				Colour(0.45f,	0.42f,	0.75f,	1.0f);
	const Colour Colours::TOPAZ = 					Colour(1.0f,	0.78f,	0.49f,	1.0f);
	const Colour Colours::TRACTOR_RED = 			Colour(0.99f,	0.5f,	0.21f,	1.0f);
	const Colour Colours::TROLLEY_GREY = 			Colour(0.50f,	0.50f,	0.50f,	1.0f);
	const Colour Colours::TROPICAL_RAIN_FOREST = 	Colour(0.0f,	0.46f,	0.37f,	1.0f);
	const Colour Colours::TRUE_BLUE = 				Colour(0.0f,	0.45f,	0.81f,	1.0f);
	const Colour Colours::TUFTS_BLUE = 				Colour(0.28f,	0.57f,	0.81f,	1.0f);
	const Colour Colours::TUMBLEWEED = 				Colour(0.87f,	0.67f,	0.53f,	1.0f);
	const Colour Colours::TURKISH_ROSE = 			Colour(0.71f,	0.45f,	0.51f,	1.0f);
	const Colour Colours::TURQUOISE = 				Colour(0.19f,	0.84f,	0.78f,	1.0f);
	const Colour Colours::TURQUOISE_BLUE = 			Colour(0.0f,	1.0f,	0.94f,	1.0f);
	const Colour Colours::TURQUOISE_GREEN = 		Colour(0.63f,	0.84f,	0.71f,	1.0f);
	const Colour Colours::TUSCAN_RED = 				Colour(0.40f,	0.26f,	0.30f,	1.0f);
	const Colour Colours::TWILIGHT_LAVENDER = 		Colour(0.54f,	0.29f,	0.42f,	1.0f);
	const Colour Colours::TYRIAN_PURPLE = 			Colour(0.40f,	0.1f,	0.24f,	1.0f);
	const Colour Colours::UA_BLUE = 				Colour(0.0f,	0.20f,	0.67f,	1.0f);
	const Colour Colours::UA_RED = 					Colour(0.85f,	0.0f,	0.30f,	1.0f);
	const Colour Colours::UBE = 					Colour(0.53f,	0.47f,	0.76f,	1.0f);
	const Colour Colours::UCLA_BLUE = 				Colour(0.33f,	0.41f,	0.58f,	1.0f);
	const Colour Colours::UCLA_GOLD = 				Colour(1.0f,	0.70f,	0.0f,	1.0f);
	const Colour Colours::UFO_GREEN = 				Colour(0.24f,	0.82f,	0.44f,	1.0f);
	const Colour Colours::ULTRAMARINE = 			Colour(0.7f,	0.4f,	0.56f,	1.0f);
	const Colour Colours::ULTRAMARINE_BLUE = 		Colour(0.25f,	0.40f,	0.96f,	1.0f);
	const Colour Colours::ULTRA_PINK = 				Colour(1.0f,	0.44f,	1.0f,	1.0f);
	const Colour Colours::UMBER = 					Colour(0.39f,	0.32f,	0.28f,	1.0f);
	const Colour Colours::UNITED_NATIONS_BLUE = 	Colour(0.36f,	0.57f,	0.90f,	1.0f);
	const Colour Colours::UNIVERSITY_OF_CALIFORNIA_GOLD = Colour(0.72f,	0.53f,	0.15f,	1.0f);
	const Colour Colours::UNMELLOW_YELLOW = 		Colour(1.0f,	1.0f,	0.40f,	1.0f);
	const Colour Colours::UP_FOREST_GREEN = 		Colour(0.0f,	0.27f,	0.13f,	1.0f);
	const Colour Colours::UP_MAROON = 				Colour(0.48f,	0.7f,	0.7f,	1.0f);
	const Colour Colours::UPSDELL_RED = 			Colour(0.68f,	0.9f,	0.13f,	1.0f);
	const Colour Colours::UROBILIN = 				Colour(0.88f,	0.68f,	0.13f,	1.0f);
	const Colour Colours::USC_CARDINAL = 			Colour(0.60f,	0.0f,	0.0f,	1.0f);
	const Colour Colours::USC_GOLD = 				Colour(1.0f,	0.80f,	0.0f,	1.0f);
	const Colour Colours::UTAH_CRIMSON = 			Colour(0.83f,	0.0f,	0.25f,	1.0f);
	const Colour Colours::VANILLA = 				Colour(0.95f,	0.90f,	0.67f,	1.0f);
	const Colour Colours::VEGAS_GOLD = 				Colour(0.77f,	0.70f,	0.35f,	1.0f);
	const Colour Colours::VENETIAN_RED = 			Colour(0.78f,	0.3f,	0.8f,	1.0f);
	const Colour Colours::VERDIGRIS = 				Colour(0.26f,	0.70f,	0.68f,	1.0f);
	const Colour Colours::VERMILION = 				Colour(0.89f,	0.26f,	0.20f,	1.0f);
	const Colour Colours::VERONICA = 				Colour(0.63f,	0.36f,	0.94f,	1.0f);
	const Colour Colours::VIOLET = 					Colour(0.50f,	0.0f,	1.0f,	1.0f);
	const Colour Colours::VIOLET_RYB = 				Colour(0.53f,	0.0f,	0.69f,	1.0f);
	const Colour Colours::VIOLET_WEB = 				Colour(0.93f,	0.51f,	0.93f,	1.0f);
	const Colour Colours::VIRIDIAN = 				Colour(0.25f,	0.51f,	0.43f,	1.0f);
	const Colour Colours::VIVID_AUBURN = 			Colour(0.58f,	0.15f,	0.14f,	1.0f);
	const Colour Colours::VIVID_BURGUNDY = 			Colour(0.62f,	0.11f,	0.21f,	1.0f);
	const Colour Colours::VIVID_CERISE = 			Colour(0.85f,	0.11f,	0.51f,	1.0f);
	const Colour Colours::VIVID_TANGERINE = 		Colour(1.0f,	0.63f,	0.54f,	1.0f);
	const Colour Colours::VIVID_VIOLET = 			Colour(0.62f,	0.0f,	1.0f,	1.0f);
	const Colour Colours::WARM_BLACK = 				Colour(0.0f,	0.26f,	0.26f,	1.0f);
	const Colour Colours::WENGE = 					Colour(0.39f,	0.33f,	0.32f,	1.0f);
	const Colour Colours::WHEAT = 					Colour(0.96f,	0.87f,	0.70f,	1.0f);
	const Colour Colours::WHITE = 					Colour(1.0f,	1.0f,	1.0f,	1.0f);
	const Colour Colours::WHITE_SMOKE = 			Colour(0.96f,	0.96f,	0.96f,	1.0f);
	const Colour Colours::WILD_BLUE_YONDER = 		Colour(0.64f,	0.68f,	0.82f,	1.0f);
	const Colour Colours::WILD_STRAWBERRY = 		Colour(1.0f,	0.26f,	0.64f,	1.0f);
	const Colour Colours::WILD_WATERMELON = 		Colour(0.99f,	0.42f,	0.52f,	1.0f);
	const Colour Colours::WINE = 					Colour(0.45f,	0.18f,	0.22f,	1.0f);
	const Colour Colours::WISTERIA = 				Colour(0.79f,	0.63f,	0.86f,	1.0f);
	const Colour Colours::XANADU = 					Colour(0.45f,	0.53f,	0.47f,	1.0f);
	const Colour Colours::YALE_BLUE = 				Colour(0.6f,	0.30f,	0.57f,	1.0f);
	const Colour Colours::YELLOW = 					Colour(1.0f,	1.0f,	0.0f,	1.0f);
	const Colour Colours::YELLOW_MUNSELL = 			Colour(0.94f,	0.80f,	0.0f,	1.0f);
	const Colour Colours::YELLOW_NCS = 				Colour(1.0f,	0.83f,	0.0f,	1.0f);
	const Colour Colours::YELLOW_PROCESS = 			Colour(1.0f,	0.94f,	0.0f,	1.0f);
	const Colour Colours::YELLOW_RYB = 				Colour(1.0f,	1.0f,	0.20f,	1.0f);
	const Colour Colours::YELLOW_GREEN = 			Colour(0.60f,	0.80f,	0.20f,	1.0f);
	const Colour Colours::YELLOW_ORANGE = 			Colour(1.0f,	0.68f,	0.26f,	1.0f);
	const Colour Colours::ZAFFRE = 					Colour(0.0f,	0.8f,	0.66f,	1.0f);

	Colour Colours::Random()
	{
		return Colour(	Maths::RangeRandom<f32>(0.f,1.f),
						Maths::RangeRandom<f32>(0.f,1.f),
						Maths::RangeRandom<f32>(0.f,1.f));
	}

	Colour Colours::Random(Colour lowerBound, Colour upperBound)
	{
		return Colour(	Maths::RangeRandom<f32>(lowerBound.mRed,upperBound.mRed),
						Maths::RangeRandom<f32>(lowerBound.mGreen,upperBound.mGreen),
						Maths::RangeRandom<f32>(lowerBound.mBlue,upperBound.mBlue),
						Maths::RangeRandom<f32>(lowerBound.mAlpha,upperBound.mAlpha)
				);
	}
	
}
