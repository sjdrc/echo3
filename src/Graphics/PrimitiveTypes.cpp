#include <echo/Graphics/PrimitiveTypes.h>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <echo/Util/StringUtils.h>
#include <echo/Maths/EchoMaths.h>

namespace Echo
{
	std::istream& operator >>(std::istream& i, TextureUV& tuv)
	{
		using namespace Utils::String;
		std::string temp;
		i >> temp;
		if(!Utils::String::VerifyConstructorAndExtractParameters(temp,"TextureUV") ||
			!ConvertAndAssign(temp, tuv.u, tuv.v))
		{
			tuv = TextureUV(0,0);
			i.setstate(std::ios_base::failbit);
		}
		return i;
	}		

	std::istream& operator >>(std::istream& i, TextureUVPair& tuvPair)
	{
		using namespace Utils::String;
		std::string temp;
		i >> temp;
		if(!Utils::String::VerifyConstructorAndExtractParameters(temp,"TextureUVPair") ||
			!ConvertAndAssign(temp, tuvPair.first.u, tuvPair.first.v, tuvPair.second.u, tuvPair.second.v))
		{
			tuvPair.first = TextureUV(0,0);
			tuvPair.second = TextureUV(1,1);
			i.setstate(std::ios_base::failbit);
		}		
		return i;
	}		
}
