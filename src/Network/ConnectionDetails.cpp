#include <echo/Network/ConnectionDetails.h>
#include <echo/Util/StringUtils.h>
#include <vector>

namespace Echo
{
	//////////////////////////////////////////////////////////////////////////

	ConnectionDetails::ConnectionDetails()
	{
		mHasSystem=false;
		mHasType=false;
		mHasAddress=false;
		mHasAdditionalInfo=false;
	}

	ConnectionDetails::ConnectionDetails( const std::string& connectionString )
	{
		ConvertConnectionDetails(connectionString);
	}

	void ConnectionDetails::ConvertConnectionDetails( const std::string& connectionString )
	{
		//WorkingCopy will be (System)Type:address:additionalOptions
		std::vector<std::string> outParams;
		Utils::String::Split(connectionString,":",outParams);

		mHasType=false;
		mHasAddress=false;
		mHasAdditionalInfo=false;
		mHasSystem=false;
		mSystem.clear();
		mAddress.clear();
		mAdditionalInfo.Clear();
		mType.clear();

		if(outParams.size()<2)
		{
			return;
		}

		//////////////////////////////////////////////////////////////////////////
		//determine if this has a system
		if(!outParams[0].empty())
		{
			std::string workingCopy=outParams[0];
			size_t obp=workingCopy.find_first_of('(');
			if(obp!=std::string::npos)
			{
				//there could be a system
				size_t cbp=workingCopy.find_first_of(')');
				if(obp<cbp && cbp!=std::string::npos)
				{		
					if(cbp-obp-1!=0)
					{
						mHasSystem=true;
						mSystem=workingCopy.substr(obp+1,cbp-obp-1);
					}
					if(cbp<workingCopy.length())
					{
						mHasType=true;
						mType=workingCopy.substr(cbp+1);
					}
				}
			}else
			{
				mHasType=true;
				mType=workingCopy;
			}
		}

		if(!outParams[1].empty())
		{
			mHasAddress=true;
			mAddress=outParams[1];
		}

		if((outParams.size()>=3 && mHasSystem && mHasType) || (outParams.size()>=2 && mHasType))
		{
			mHasAdditionalInfo=true;
			for(size_t i=2; i < outParams.size(); ++i)
			{
				mAdditionalInfo.ParseString(outParams[i]);
			}
		}
	}
	
	std::string ConnectionDetails::ToString() const
	{
		std::stringstream ss;
		if(HasSystem())
		{
			ss << "(" << GetSystem() << ")";
		}
		if(HasType())
		{
			ss << GetType();
		}
		if(HasAddress())
		{
			if(HasType() || HasSystem())
			{
				ss << ":";
			}
			ss << GetAddress();
		}
		if(HasAdditionalInfo())
		{
			// All index key types should go first
			std::vector<std::string> allIndexOptions = mAdditionalInfo.GetAllAsStrings("", true);
			for(Size i=0; i < allIndexOptions.size();++i)
			{
				ss << ":" << allIndexOptions[i];
			}
			std::vector<std::string> optionNames = mAdditionalInfo.GetAllOptionNames();
			for(Size o = 0; o < optionNames.size(); ++o)
			{
				const std::string& name = optionNames[o];
				if(!name.empty())
				{
					ss << ":" << name << "=" << mAdditionalInfo.Get(name,"");
				}
			}
		}
		return ss.str();
	}
}
