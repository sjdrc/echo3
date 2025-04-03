#include <echo/Util/Configuration.h>
#include <echo/Util/StringUtils.h>
#include <boost/foreach.hpp>
#include <echo/Util/FunctionBinder.h>
#include <echo/FileSystem/FileSystem.h>
#include <echo/Maths/EchoMaths.h>
#include <algorithm>

namespace Echo
{
	class FunctionBinder;
	
	Configuration::Configuration(shared_ptr<FileSystem> fileSystem, shared_ptr<FunctionBinder> functionBinder) :
		mFileSystem(fileSystem),
		mFunctionBinder(functionBinder),
		mFileDepth(0),
		mStreamPrecision(16),
		mStoreLinesOnLoad(true),
		mSectionsInheritVariables(true)
	{
		mParser.SetOutputErrors(false);
		mVariableSubstitutor.SetOutputErrors(false);
	}
	
	Configuration::~Configuration()
	{
	}

	bool Configuration::LoadFile(const std::string& fileName, bool parseLineThroughFunctionBinder)
	{
		if(!mFileSystem)
		{
			ECHO_LOG_ERROR("No FileSystem specified. Unable to open file. \"" << fileName << "\"");
			return false;
		}
		File file=mFileSystem->Open(fileName);
		if(!file.IsOpen())
		{
			ECHO_LOG_ERROR("Could not open file. \"" << fileName << "\"");
			return false;
		}
		
		ECHO_LOG_INFO("Loading configuration: \"" << fileName << "\"");

		std::vector<std::string> lines;
		file.Seek(0);
		file.ReadLines(lines,false);

		return ParseLines(lines,parseLineThroughFunctionBinder,fileName);
	}
	
	bool Configuration::ParseString(const std::string& content, bool parseLineThroughFunctionBinder, std::string parentFileName)
	{
		std::vector<std::string> lines;
		Utils::String::Split(content,"\n",lines);
		return ParseLines(lines,parseLineThroughFunctionBinder,parentFileName);
	}
	
	bool Configuration::ParseLines(const std::vector<std::string>& lines, bool parseLineThroughFunctionBinder, std::string parentFileName)
	{
		std::string currentSection;
		FunctionBinder internalFunctions;
		internalFunctions.Register("Remove",[this,&currentSection](const std::string& option){
			RemoveFromSection(currentSection,option);
		},false,boost::fusion::vector<std::string>());
		
		internalFunctions.Register("RemoveAtIndex",[this,&currentSection](const std::string& option, Size index){
			RemoveFromSectionAtIndex(currentSection,option,index);
		},false,boost::fusion::vector<std::string,Size>());
		
		internalFunctions.Register("RemoveFromSection",[this](const std::string& sectionName, const std::string& option){
			RemoveFromSection(sectionName,option);
		},false,boost::fusion::vector<std::string,std::string>());
		
		internalFunctions.Register("RemoveFromSectionAtIndex",[this](const std::string& sectionName, const std::string& option, Size index){
			RemoveFromSectionAtIndex(sectionName,option,index);
		},false,boost::fusion::vector<std::string,std::string,Size>());

		internalFunctions.Register("AddToSection",[this](const std::string& sectionName,const std::string& option,const std::string& value){
			AddToSection(sectionName,option,value);
		},false,boost::fusion::vector<std::string,std::string,std::string>());
		
		BOOST_FOREACH(const std::string& line, lines)
		{
			if(line.empty())
			{
				continue;
			}
			if(parseLineThroughFunctionBinder)
			{
				if(mFunctionBinder->Call(line).mStatus!=FunctionBinder::CallStatuses::FUNCTION_NOT_FOUND)
				{
					continue;
				}
			}

			// Attempt to call internal functions first
			if(internalFunctions.Call(line).mStatus!=FunctionBinder::CallStatuses::FUNCTION_NOT_FOUND)
			{
				continue;
			}
			
			// Check for section change
			if(line.length()>=2)
			{
				if(line.front()=='[' && line.back()==']')
				{
					currentSection=line.substr(1,line.length()-2);
					continue;
				}
			}
			
			std::vector<std::string> params;
			std::string left=Utils::String::SplitLine(line,"=","",params);
			if(params.size()>0)
			{
				if(left=="include")
				{
					if(!mFileSystem)
					{
						ECHO_LOG_ERROR("include line found but missing file system object.");
						return false;
					}
					std::vector<std::string> params2;
					Utils::String::Split(params[0],",",params2);
					bool optional=false;
					if(params2.size()==0)
					{
						ECHO_LOG_ERROR("config.file specified but missing value.");
						return false;
					}else
					{
						if(params2.size()>=2 && params2[1]=="optional")
						{
							optional=true;
						}

					}
					std::string configFile=mVariableSubstitutor.Parse(params2[0]);
					
					// Attempt to load it relative to the current file.
					std::string path = Utils::String::GetPathFromFilename(parentFileName);
					mFileDepth++;
					ECHO_LOG_INFO("Loading include...");
					if(mFileSystem->FileExists(path+configFile))
					{
						//Try and load it.
						if(!LoadFile(currentSection,path+configFile,parseLineThroughFunctionBinder))
						{
							if(optional)
							{
								ECHO_LOG_ERROR("Optional file \"" << (path+configFile) << "\" not loaded.");
							}else
							{
								mFileDepth=0;
								return false;
							}
						}
					}else
					{
						//File with path doesn't exist so try the specified name.
						if(!mFileSystem->FileExists(configFile))
						{
							if(optional)
							{
								ECHO_LOG_INFO("Optional file \"" << configFile << "\" not found.");
							}else
							{
								ECHO_LOG_ERROR("Unable to find file \"" << configFile << "\"");
								mFileDepth=0;
								return false;
							}
						} else
						{
							// File specified exists (apparently) so try to load it.
							if(!LoadFile(currentSection,configFile,parseLineThroughFunctionBinder))
							{
								if(optional)
								{
									ECHO_LOG_INFO("Optional file \"" << configFile << "\" not loaded.");
								}else
								{
									ECHO_LOG_ERROR("Unable to load file \"" << configFile << "\"");
									mFileDepth=0;
									return false;
								}
							}
						}
					}
						
					mFileDepth--;
					continue;
				}
				RegisterOptionWithParsers(currentSection,left);
				AddToSection(currentSection,left,params[0]);
				if(left=="sections.inherit")
				{
					mSectionsInheritVariables = GetFromSection(currentSection,left,true);
				}
			}else
			{
				if(mStoreLinesOnLoad)
				{
					//It is just a line, we'll add it to the empty key entry for lines.
					AddToSection(currentSection,"",left);
				}
			}
		}
		return true;
	}
	
	bool Configuration::LoadFile(const std::string& section, const std::string& fileName, bool parseLineThroughFunctionBinder)
	{
		if(section.empty())
		{
			return LoadFile(fileName, parseLineThroughFunctionBinder);
		}
		std::string subsections;
		Configuration& thisSection = GetOrCreateSection(section,subsections);
		return thisSection.LoadFile(subsections,fileName,parseLineThroughFunctionBinder);
	}
	
	bool Configuration::ParseLines(const std::string& section, const std::vector<std::string>& lines, bool parseLineThroughFunctionBinder, std::string parentFileName)
	{
		if(section.empty())
		{
			return ParseLines(lines, parseLineThroughFunctionBinder, parentFileName);
		}
		std::string subsections;
		Configuration& thisSection = GetOrCreateSection(section,subsections);
		return thisSection.ParseLines(subsections,lines, parseLineThroughFunctionBinder, parentFileName);
	}
	
	bool Configuration::ParseString(const std::string& section, const std::string& content, bool parseLineThroughFunctionBinder, std::string parentFileName)
	{
		if(section.empty())
		{
			return ParseString(content, parseLineThroughFunctionBinder, parentFileName);
		}
		std::string subsections;
		Configuration& thisSection = GetOrCreateSection(section,subsections);
		return thisSection.ParseString(subsections,content, parseLineThroughFunctionBinder, parentFileName);
	}
	
	bool Configuration::WriteFile(const std::string& fileName, File::OpenMode openMode) const
	{
		if(!mFileSystem)
		{
			ECHO_LOG_ERROR("No FileSystem specified. Unable to open file \"" << fileName << "\" to write configuration");
			return false;
		}
		File file=mFileSystem->Open(fileName,openMode);
		if(!file.IsOpen())
		{
			ECHO_LOG_ERROR("Could not open file. \"" << fileName << "\" to write configuration");
			return false;
		}
		
		ECHO_LOG_INFO("Writing configuration to: \"" << fileName << "\"");
		return WriteFile(file);
		
		file.Close();
		return true;
	}
	
	bool Configuration::WriteFile(File& file) const
	{
		if(!file.IsOpen())
		{
			return false;
		}
		bool errorFree = true;

		BOOST_FOREACH(const ConfigurationPair& configPair, mConfigurationOptions)
		{
			BOOST_FOREACH(const std::string& value, configPair.second)
			{
				std::string line;
				if(configPair.first.length()!=0)
				{
					line = configPair.first + "=" + value + "\n";
				}else
				{
					line = value + "\n";
				}
				
				if(file.Write(line)!=line.length())
				{
					ECHO_LOG_ERROR("Failed to write line. \"" << line << "\"");
					errorFree = false;
				}
			}
		}
		// Write sections
		BOOST_FOREACH(const SectionPair& sectionPair, mSections)
		{
			std::string sectionName = "[" + sectionPair.first + "]";
			if(file.Write(sectionName)!=sectionName.length())
			{
				ECHO_LOG_ERROR("Failed to write section name to \"" << sectionName << "\"");
				errorFree = false;
			}
			if(!sectionPair.second.WriteFile(file))
			{
				errorFree = false;
			}
		}
		return errorFree;
	}
	

	bool Configuration::ParseConfigurationScript(File file)
	{
		if(!mFunctionBinder)
		{
			ECHO_LOG_ERROR("No function binder is configured. Unable to parse configuration script.");
			return false;
		}
		if(!file.IsOpen())
		{
			ECHO_LOG_ERROR("File is not open.");
			return false;
		}
		file.Seek(0);

		ECHO_LOG_INFO("Loading configuration." << file.GetActualFileName());

		std::vector<std::string> lines;
		file.ReadLines(lines,false);

		BOOST_FOREACH(std::string& line, lines)
		{
			mFunctionBinder->Call(line);
		}
		return true;
	}
	
	size_t Configuration::GetNumberOfOptionsNamed(const std::string& optionName) const
	{
		ConfigurationMap::const_iterator it = mConfigurationOptions.find(optionName);
		if(it==mConfigurationOptions.end())
		{
			return 0;
		}
		return it->second.size();
	}

	bool Configuration::HasOption(const std::string& optionName) const
	{
		ConfigurationMap::const_iterator it = mConfigurationOptions.find(optionName);
		if(it==mConfigurationOptions.end())
		{
			return false;
		}
		return !(it->second.empty());
	}

	template<>
	bool Configuration::Get(const std::string& optionName, bool& outObject, const bool& defaultValue, size_t index, bool suppressWarning, bool substituteOnly) const
	{
		ConfigurationMap::const_iterator it = mConfigurationOptions.find(optionName);
		if (it ==mConfigurationOptions.end())
		{
			if (!suppressWarning)
			{
				ECHO_LOG_WARNING("Option '" << optionName << "' not found.");
			}
			outObject = defaultValue;
			return false;
		}
		if (index >= it->second.size())
		{
			if (!suppressWarning)
			{
				ECHO_LOG_WARNING("Option not found for '" << optionName << "' at index " << index << ".");
			}
			outObject = defaultValue;
			return false;
		}
		std::string parsed = mVariableSubstitutor.Parse(it->second[index]);
		if(!substituteOnly)
		{
			optional<double> result = mParser.Parse(parsed);
			if(result)
			{
				outObject = Maths::Abs<double>(*result)>0;
				return true;
			}
		}

		std::stringstream ss(parsed);
		// try with 1 and 0.
		ss >> std::noboolalpha >> outObject;
		if (ss.fail())
		{
			ss.clear();
			
			// Re-assign the stream contents. Not doing this causes a failure on Android.
			ss.str(parsed);
			
			// try with true and false.
			ss >> std::boolalpha >> outObject;
			if (ss.fail())
			{
				return false;
			}
		}
		return true;
	}
	
	template<>
	bool Configuration::Get(const std::string& configurationName, std::string& outObject, const std::string& defaultValue, size_t index, bool suppressWarning, bool substituteOnly) const
	{
		ConfigurationMap::const_iterator it = mConfigurationOptions.find(configurationName);
		if(it==mConfigurationOptions.end())
		{
			if(!suppressWarning)
			{
				ECHO_LOG_WARNING("Option '" << configurationName << "' not found.");
			}
			outObject = defaultValue;
			return false;
		}
		if(index>=it->second.size())
		{
			if(!suppressWarning)
			{
				ECHO_LOG_WARNING("Option not found for '" << configurationName << "' at index " << index << ".");
			}
			outObject = defaultValue;
			return false;
		}
		
		outObject = mVariableSubstitutor.Parse(it->second[index]);
		optional<double> result;
		if(!substituteOnly)
		{
			result = mParser.Parse(outObject);
		}
		if(result)
		{
			std::string processed;
			std::stringstream ss;
			ss << std::setprecision(mStreamPrecision) << result.value();
			ss >> std::setprecision(mStreamPrecision) >> processed;
			if(!(ss.fail()))
			{
				outObject = processed;
			}
		}
		return true;
	}
	
	template<>
	bool Configuration::GetOrFail<std::string>(const std::string& configurationName, std::string& outObject, size_t index, bool suppressWarning, bool substituteOnly) const
	{
		ConfigurationMap::const_iterator it = mConfigurationOptions.find(configurationName);
		if(it==mConfigurationOptions.end())
		{
			if(!suppressWarning)
			{
				ECHO_LOG_WARNING("Option '" << configurationName << "' not found.");
			}
			return false;
		}
		if(index>=it->second.size())
		{
			if(!suppressWarning)
			{
				ECHO_LOG_WARNING("Option not found for '" << configurationName << "' at index " << index << ".");
			}
			return false;
		}
		
		outObject = mVariableSubstitutor.Parse(it->second[index]);
		optional<double> result;
		if(!substituteOnly)
		{
			result = mParser.Parse(outObject);
		}
		if(result)
		{
			std::string processed;
			std::stringstream ss;
			ss << std::setprecision(mStreamPrecision) << result.value();
			ss >> std::setprecision(mStreamPrecision) >> processed;
			if(!(ss.fail()))
			{
				outObject = processed;
			}
		}
		return true;
	}

	std::vector<std::string> Configuration::GetAllAsStrings(const std::string& optionName, bool suppressWarning) const
	{
		ConfigurationMap::const_iterator it = mConfigurationOptions.find(optionName);
		if(it==mConfigurationOptions.end())
		{
			if(!suppressWarning)
			{
				ECHO_LOG_WARNING("Option '" << optionName << "' not found.");
			}
			return std::vector<std::string>();
		}
		return it->second;
	}
	
	std::vector< std::string > Configuration::GetAllOptionNames() const
	{
		std::vector< std::string > optionNames;
		ConfigurationMap::const_iterator it = mConfigurationOptions.begin();
		ConfigurationMap::const_iterator itEnd = mConfigurationOptions.end();
		while(it!=itEnd)
		{
			optionNames.push_back(it->first);
			++it;
		}
		return optionNames;
	}
	
	bool Configuration::Set(const std::string& optionName, const std::string& optionValue, bool overwriteExisting)
	{
		ConfigurationMap::const_iterator it = mConfigurationOptions.find(optionName);
		if(it!=mConfigurationOptions.end() && !overwriteExisting)
		{
		}
		if(it==mConfigurationOptions.end())
		{
			RegisterOptionWithParsers(optionName);
		}
		if(!overwriteExisting)
		{
			return false;
		}
		std::vector<std::string>& optionValues = mConfigurationOptions[optionName];
		if(overwriteExisting)
		{
			optionValues.clear();
		}
		optionValues.push_back(optionValue);
		return true;
	}
	
	void Configuration::Add(const std::string& optionName, const std::string& optionValue)
	{
		ConfigurationMap::iterator it = mConfigurationOptions.find(optionName);
		if(it==mConfigurationOptions.end())
		{
			RegisterOptionWithParsers(optionName);
			mConfigurationOptions[optionName].push_back(optionValue);
		}else
		{
			it->second.push_back(optionValue);
		}
	}
	
	void Configuration::AddOptions(const Configuration::SingleValueConfigurationMap& optionsMap)
	{
		BOOST_FOREACH(const SingleValueConfigurationMap::value_type& configPair, optionsMap)
		{
			Add(configPair.first,configPair.second);
		}
	}

	bool Configuration::Remove(const std::string& optionName)
	{
		ConfigurationMap::iterator it = mConfigurationOptions.find(optionName);
		if(it!=mConfigurationOptions.end())
		{
			mConfigurationOptions.erase(it);
			return true;
		}
		return false;
	}

	bool Configuration::RemoveAtIndex(const std::string& optionName, Size index)
	{
		ConfigurationMap::iterator it = mConfigurationOptions.find(optionName);
		if(it!=mConfigurationOptions.end())
		{
			if(index < it->second.size())
			{
				it->second.erase(it->second.begin()+index);
			}
			return true;
		}
		return false;
	}

	bool Configuration::RemoveFromSectionAtIndex(const std::string& sectionName, const std::string& optionName, Size index)
	{
		if(sectionName.empty())
		{
			return RemoveAtIndex(optionName, index);
		}

		std::string subsections;
		Configuration* thisSection = GetSection(sectionName,subsections);
		if(!thisSection)
		{
			return false;
		}
		return thisSection->RemoveFromSectionAtIndex(subsections, optionName, index);
	}


	void Configuration::Clear()
	{
		mConfigurationOptions.clear();
		mSections.clear();
	}

	size_t Configuration::GetNumberOfOptionsNamedInSection(const std::string& section, const std::string& optionName) const
	{
		if(section.empty())
		{
			return GetNumberOfOptionsNamed(optionName);
		}

		std::string subsections;
		const Configuration* thisSection = GetSection(section,subsections);
		if(!thisSection)
		{
			ECHO_LOG_WARNING("Unable to find section " << thisSection << " could not look up " << optionName);
			return 0;
		}

		// When a separator isn't found subsections will be empty causing a normal lookup to occur.
		return thisSection->GetNumberOfOptionsNamedInSection(subsections, optionName);
	}

	bool Configuration::HasOptionInSection(const std::string& section, const std::string& optionName) const
	{
		if(section.empty())
		{
			return HasOption(optionName);
		}

		std::string subsections;
		const Configuration* thisSection = GetSection(section,subsections);
		if(!thisSection)
		{
			ECHO_LOG_WARNING("Unable to find section " << thisSection << " could not look up " << optionName);
			return 0;
		}

		// When a separator isn't found subsections will be empty causing a normal lookup to occur.
		return thisSection->HasOptionInSection(subsections, optionName);
	}

	std::vector<std::string> Configuration::GetAllFromSectionAsStrings(const std::string& section, const std::string& optionName, bool suppressWarning) const
	{
		if(section.empty())
		{
			return GetAllAsStrings(optionName, suppressWarning);
		}

		std::string subsections;
		const Configuration* thisSection = GetSection(section,subsections);
		if(!thisSection)
		{
			ECHO_LOG_WARNING("Unable to find section " << thisSection << " could not look up " << optionName);
			return std::vector<std::string>();
		}

		// When a separator isn't found subsections will be empty causing a normal lookup to occur.
		return thisSection->GetAllFromSectionAsStrings(subsections, optionName, suppressWarning);
	}
	
	void Configuration::AddOptionsToSection(const std::string& section, const Configuration::SingleValueConfigurationMap& optionsMap)
	{
		if(section.empty())
		{
			return AddOptions(optionsMap);
		}

		std::string subsections;
		Configuration& thisSection = GetOrCreateSection(section,subsections);
		return thisSection.AddOptionsToSection(subsections, optionsMap);
	}

	bool Configuration::RemoveFromSection(const std::string& section, const std::string& optionName)
	{
		if(section.empty())
		{
			return Remove(optionName);
		}

		std::string subsections;
		Configuration* thisSection = GetSection(section,subsections);
		if(!thisSection)
		{
			return false;
		}
		return thisSection->RemoveFromSection(subsections, optionName);
	}
	
	void Configuration::ClearSection(const std::string& section)
	{
		if(section.empty())
		{
			return Clear();
		}

		std::string subsections;
		Configuration* thisSection = GetSection(section,subsections);
		if(!thisSection)
		{
			ECHO_LOG_WARNING("No section named " << thisSection);
			return;
		}
		return thisSection->ClearSection(subsections);
	}

	void Configuration::RegisterOptionWithParsers(const std::string& optionName)
	{
		mParser.RegisterVariableViaGetter(optionName,bind(static_cast<double(Configuration::*)(const std::string&, const double&) const>(&Configuration::Get<double>),this,optionName,0));
		mVariableSubstitutor.RegisterVariableViaGetter(optionName,bind(static_cast<std::string(Configuration::*)(const std::string&, const std::string&) const>(&Configuration::Get<std::string>),this,optionName,""));
		mVariableSubstitutor.RegisterVariableViaGetter("__"+optionName,bind(static_cast<std::string(Configuration::*)(const std::string&, const std::string&) const>(&Configuration::GetNoCalc<std::string>),this,optionName,""));
	}

	void Configuration::RegisterOptionWithParsers(const std::string& section, const std::string& optionName)
	{
		if(section.empty())
		{
			return RegisterOptionWithParsers(optionName);
		}
		std::string subsections;
		Configuration& thisSection = GetOrCreateSection(section,subsections);
		thisSection.RegisterOptionWithParsers(subsections,optionName);
	}

	Configuration* Configuration::GetSection(const std::string& sectionPath, std::string& subsections)
	{
		std::string thisSection;
		if(!Utils::String::SplitInTwo(sectionPath, ".", thisSection, subsections))
		{
			thisSection=sectionPath;
		}
		
		SectionMap::iterator it = mSections.find(thisSection);
		if(it==mSections.end())
		{
			return nullptr;
		}
		return &it->second;
	}
	
	const Configuration* Configuration::GetSection(const std::string& sectionPath, std::string& subsections) const
	{
		std::string thisSection;
		if(!Utils::String::SplitInTwo(sectionPath, ".", thisSection, subsections))
		{
			thisSection=sectionPath;
		}
		
		SectionMap::const_iterator it = mSections.find(thisSection);
		if(it==mSections.end())
		{
			return nullptr;
		}
		return &it->second;
	}
	
	Configuration& Configuration::GetOrCreateSection(const std::string& sectionPath, std::string& subsections)
	{
		std::string thisSection;
		if(!Utils::String::SplitInTwo(sectionPath, ".", thisSection, subsections))
		{
			thisSection=sectionPath;
		}
		
		SectionMap::iterator it = mSections.find(thisSection);
		if(it==mSections.end())
		{
			Configuration& section = mSections[thisSection];
			mSectionOrder.push_back(thisSection);
			section.SetFileSystem(GetFileSystem());
			section.SetFunctionBinder(GetFunctionBinder());

			if(mSectionsInheritVariables)
			{
				section.mParser.GetFunctionBinder()->SetFallback(mParser.GetFunctionBinder());
				section.mVariableSubstitutor.GetFunctionBinder()->SetFallback(mVariableSubstitutor.GetFunctionBinder());
			}
			return section;
		}
		return it->second;
	}
	
	Configuration* Configuration::GetSection(const std::string& sectonPath)
	{
		std::string subsections;
		return GetSection(sectonPath, subsections);
	}

	const Configuration* Configuration::GetSection(const std::string& sectonPath) const
	{
		std::string subsections;
		return Configuration::GetSection(sectonPath, subsections);
	}

	std::vector< Configuration::NamedSection > Configuration::GetAllSections() const
	{
		std::vector< Configuration::NamedSection > sections;
		sections.reserve(mSections.size());
		for(auto& section : mSectionOrder)
		{
			auto it = mSections.find(section);
			if(it!=mSections.end())
			{
				sections.push_back(NamedSection{it->first,&it->second});
			}else
			{
				ECHO_LOG_ERROR("Unable to find section \"" << section << "\" when building a section list. This should not happen with internal record keeping. Please report this error.");
			}
		}
		return sections;
			
	}

	std::vector< std::string > Configuration::GetAllSectionNames() const
	{
		std::vector< std::string > sections;
		sections.reserve(mSections.size());
		for(auto& section : mSectionOrder)
		{
			auto it = mSections.find(section);
			if(it!=mSections.end())
			{
				sections.push_back(it->first);
			}else
			{
				ECHO_LOG_ERROR("Unable to find section \"" << section << "\" when building a section list. This should not happen with internal record keeping. Please report this error.");
			}
		}
		return sections;
	}
	
	void Configuration::SetSectionsInheritVariables(bool sectionsInheritVariables)
	{
		mSectionsInheritVariables = sectionsInheritVariables;
	}
	
	bool Configuration::GetSectionsInheritVariables() const
	{
		return mSectionsInheritVariables;
	}

	template<>
	bool Configuration::ForEach<std::string>(const std::string& optionName, boost::function<bool(const std::string&)> operation, bool suppressWarning, bool substituteOnly) const
	{
		ConfigurationMap::const_iterator it = mConfigurationOptions.find(optionName);
		if(it==mConfigurationOptions.end())
		{
			if(!suppressWarning)
			{
				ECHO_LOG_WARNING("Option '" << optionName << "' not found.");
			}
			return false;
		}

		ConfigurationValueList::const_iterator itList = it->second.begin();
		ConfigurationValueList::const_iterator itListEnd = it->second.end();
		while(itList!=itListEnd)
		{
			std::string parsed = mVariableSubstitutor.Parse(*itList);
			optional<double> result;
			if(!substituteOnly)
			{
				result = mParser.Parse(parsed);
			}
			if(result)
			{
				std::string resultConverted;
				std::stringstream ss;
				ss << std::setprecision(mStreamPrecision) << result.value();
				ss >> std::setprecision(mStreamPrecision) >> resultConverted;
				if(!(ss.fail()))
				{
					parsed = resultConverted;
				}
			}

			if(!operation(parsed))
			{
				if(!suppressWarning)
				{
					ECHO_LOG_WARNING("Operation callback failed for '" << optionName << "' with data '" << (*itList) << "'");
					return false;
				}
			}
			itList++;
		}
		return true;
	}
}
