#include <echo/Util/StringUtils.h>
#include <echo/UTF8String.h>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/regex.hpp>

namespace Echo
{
	namespace Utils
	{
		namespace String
		{
			bool SplitInTwo(const std::string& stringToSplit, std::string separators, std::string& left, std::string& right)
			{
				std::string temp;
				size_t divider=stringToSplit.find_first_of(separators);
				
				//If a divider isn't found then we've failed to split.
				if(divider==std::string::npos)
				{
					return false;
				}
				
				//No failure means we have at least left.
				left=stringToSplit.substr(0,divider);
				
				//if the divider was at the end of the string then right will be empty.
				if(divider+1==stringToSplit.length())
				{
					//Make sure it is empty.
					right.clear();
				}else
				{
					//Otherwise, right is the divider+1 to the end of the string.
					right=stringToSplit.substr(divider+1);
				}
				return true;
			}

			bool SplitInTwo(const std::string& stringToSplit, std::string separators, std::string& left, UTF8String& right)
			{
				std::string rightTemp;
				bool result=SplitInTwo(stringToSplit, separators, left, rightTemp);
				if(result)
				{
					right=rightTemp;
				}
				return result;
			}
			
			void Split(const std::string& stringToSplit, std::string separators, std::vector<std::string> &outStrings)
			{
				std::string temp;
				outStrings.clear();
				for(size_t i=0; i < stringToSplit.size(); ++i)
				{
					if(separators.find_first_of(stringToSplit[i])!=std::string::npos)
					{
						outStrings.push_back(temp);
						temp="";
						continue;
					}
					temp+=stringToSplit[i];
				}
				if(!temp.empty())
				{
					outStrings.push_back(temp);
				}
			}
			
			void Split(const std::string& stringToSplit, std::vector<std::string>& outStrings, char separator, char escape, char quote)
			{
				using namespace std;
				using namespace boost;
				typedef tokenizer< escaped_list_separator<char> > Tokeniser;
				Tokeniser tok(stringToSplit.begin(),stringToSplit.end(), escaped_list_separator<char>(escape,separator,quote));
				for(Tokeniser::iterator it=tok.begin(); it!=tok.end(); ++it)
				{
					outStrings.push_back(*it);
				}
			}
			
			std::string EscapeCharacters(const std::string& stringToEscape, const std::string& charactersToEscape, char escape)
			{
				std::string temp;
				temp.reserve(stringToEscape.size()*2);	// Worst case scenario
				for(size_t i=0; i < stringToEscape.size(); ++i)
				{
					if(charactersToEscape.find_first_of(stringToEscape[i])!=std::string::npos)
					{
						temp+=escape;
						temp+=stringToEscape[i];
					}else
					{
						temp+=stringToEscape[i];
					}
				}
				return temp;
			}

			std::string SplitLine(const std::string& line, std::string lineSeparators, std::string rightPartSeparators, std::vector<std::string>& splitOut)
			{
				size_t firstEq=line.find_first_of(lineSeparators);

				if(firstEq==std::string::npos)
					return line;

				std::string token=line.substr(0,firstEq);
				std::string ds;	
				ds=line.substr(firstEq+1,line.length()-firstEq);

				//split paramB up with ,
				size_t comma=0;
				comma=ds.find_first_of(rightPartSeparators);
				while(comma!=std::string::npos)
				{
					comma=ds.find_first_of(rightPartSeparators);
					if(comma==std::string::npos)
						continue;
					if(comma==ds.length())
					{
						comma=std::string::npos;
						continue;
					}
					splitOut.push_back( ds.substr(0,comma) );
					ds=ds.substr(comma+1,ds.length()-comma);
				}
				if(!ds.empty())
				{
					splitOut.push_back(ds);
				}
				return token;
			}

			std::string GetFileExtension(const std::string& fileName)
			{
				size_t slash = fileName.find_last_of('/');
				if(slash!=std::string::npos)
				{
					return GetFileExtension(fileName.substr(slash+1));
				}
				size_t dot = fileName.find_last_of('.');
				
				//If there is no '.' or it is the last character.
				if(dot==std::string::npos || dot==fileName.size()-1)
				{
					return std::string();
				}
				//Otherwise
				return fileName.substr(dot+1);
			}

			std::string GetPathFromFilename(const std::string& fileName)
			{
				size_t slash = fileName.find_last_of('/');
				if(slash==std::string::npos)
				{
					return std::string();
				}
				//Otherwise
				return fileName.substr(0,slash+1);
			}

			std::string ReducePath(const std::string& pathString)
			{
				std::vector<std::string> pathComponents;
				Split(pathString,"/",pathComponents);
				if(pathString.empty() || pathComponents.size()==0)
				{
					return pathString;
				}
				// We'll keep a flag for each element in the string. The flags will be used in a
				// second pass to determine whether or not to append that element.
				std::vector<bool> usePathComponents(true,pathComponents.size());

				// For the rest remove ".." and "." in a path
				size_t numberOfComponents=pathComponents.size();
				for (size_t i=0; i < numberOfComponents; ++i)
				{
					// Go up a level if the current directory is ..
					if (pathComponents[i] == "..")
					{
						if(i!=0)
						{
							// This one shouldn't be used, the previous one shouldn't either, but it might
							// also be multiple ../../.. so we need to loop back.
							usePathComponents[i]=false;
							
							//If .. exists one after the next then this
							size_t d=1;
							while(d<i && usePathComponents[i-d]==false)
							{
								d++;
							}
							usePathComponents[i-d]=false;
						}
					}else
					if (pathComponents[i] == ".")
					{
						//Mark this element not to use
						usePathComponents[i]=false;
					}
				}

				std::string result;
				//Check for absolute paths and add the initial slash.
				if(pathString[0]=='/')
				{
					result="/";
				}
				
				for (size_t i=0; i < numberOfComponents; ++i)
				{
					if(usePathComponents[i] && !pathComponents[i].empty())
					{
						if(result.empty() || result=="/")
						{
							//Don't include an initial / for the first one
							result+=pathComponents[i];
						}else
						{
							result+="/" + pathComponents[i];
						}
					}
				}
				return result;
			}
			
			template<>
			optional<bool> FromString<bool>(const std::string& v, Size streamPrecision)
			{
				std::stringstream ss(v);

				bool tempValue;

				// try with 1 and 0.
				ss >> std::noboolalpha >> tempValue;
				if(ss.fail())
				{
					ss.clear();

					// Re-assign the stream contents. Not doing this causes a failure on Android.
					ss.str(v);

					// try with true and false.
					ss >> std::boolalpha >> tempValue;
					if(ss.fail())
					{
						ECHO_LOG_ERROR("Failed to convert: \"" << v << "\" to type bool.");
						return none;
					}
				}
				return tempValue;
			}

			template<>
			optional<std::string> FromString<std::string>(const std::string& v, Size)
			{
				return v;
			}
			
			template<>
			optional<std::string> ToString<std::string>(const std::string& v, Size)
			{
				return v;
			}

			std::string RemoveURLSchemeAndCreds(std::string url)
			{
				if(url.substr(0,7)=="http://")
				{
					url = url.substr(7);
				}else
				if(url.substr(0,8)=="https://")
				{
					url = url.substr(8);
				}
				size_t pastCreds = url.find_first_of('@');
				if(pastCreds!=std::string::npos)
				{
					url = url.substr(pastCreds+1);
				}
				return url;
			}

			std::string RemoveURLParameters(std::string url)
			{
				size_t paramsStart = url.find_first_of('?');
				if(paramsStart!=std::string::npos)
				{
					url = url.substr(0,paramsStart);
				}
				return url;
			}

			std::string MakeURLDirectorySafe(std::string url)
			{
				boost::regex expr{"[^A-Za-z0-9.-/]"};
				url = RemoveURLSchemeAndCreds(url);
				boost::filesystem::path pathSection = RemoveURLParameters(url);
				boost::filesystem::path truncatedPath;

				// Iterate over each section and ensure the length doesn't exceed 255
				for(auto it = pathSection.begin(); it!=pathSection.end(); ++it)
				{
					if(it->string().length() >= 255)
					{
						size_t possibleExt = it->string().find_last_of(".");
						if(possibleExt!=std::string::npos)
						{
							std::string assumedExt = it->string().substr(possibleExt);
							truncatedPath /= (it->string().substr(0,253-assumedExt.length()) + "__" + assumedExt);
						}else
						{
							truncatedPath /= (it->string().substr(0,253) + "__");
						}
					}else
					{
						truncatedPath /= *it;
					}
				}
				std::string format("_");
				return boost::regex_replace(truncatedPath.string(), expr, format);
			}

		}
	}
}
