#ifndef ECHO_CONFIGURATION_H
#define ECHO_CONFIGURATION_H

#include <echo/Types.h>
#include <echo/FileSystem/File.h>
#include <echo/Util/Parsers.h>
#include <boost/lexical_cast.hpp>
#include <echo/Util/StringUtils.h>
#include <iostream>
#include <vector>
#include <map>
#include <iomanip>

namespace Echo
{
	class FunctionBinder;
	class FileSystem;
	
	/**
	 * Configuration objects are name mapped values.
	 * You can load mapped configuration options from a file or set them using the various Set() methods.
	 * Configuration objects can also take a FunctionBinder object and parse a file like a script. This can be useful
	 * if you want to load a configuration in and perform various function calls during loading. The function calls
	 * need to be set up in the FunctionBinder for this to work.
	 * 
	 * The option values are stored as strings and will be converted to the requested type (or at least it will be
	 * attempted) when calling Get<T>().
	 * 
	 * Options can have multiple values and they can be indexed from a zero base in the order they were added. In
	 * the case of loading options from a file, if there are multiple of the same option then they will be indexed
	 * in the order they appears in the file.
	 * 
	 * Sections
	 * 
	 * Configuration objects also support sections. Sections allow you to break up options into related groups.
	 * Accessing options within sections is accomplished through the GetFromSection() and similar methods. These
	 * are designed to take a section "path" which are section names separated by `.` for example:
	 * 
	 *	Colour textColour = GetFromSection<Colour>("gui","text.colour",Colours::BLACK);
	 * 
	 * ```
	 * [gui]
	 * text.colour=#aadf63
	 * ```
	 * 
	 * Here are a few notes on using sections including some things to consider:
	 * 
	 * - An empty section name refers to no section, that includes [] in a file and using GetFromSection() passing
	 * an empty section name.
	 * - Sub sections can be accessed from higher levels using `.` for example "gui.fonts" would refer to a section
	 * named "fonts" in the "gui" section from the root
	 * - It is possible to create a section tree, either by loading a file or programatically, however the tree
	 * structure is currently not preserved when writing to a file.
	 * - Sections, by default, will inherit the parent's variables. This can be controlled by setting sections.inherit
	 * (which is a bool) before the section is first declared in the file.
	 */
	class Configuration
	{
	public:
		typedef std::vector<std::string> ConfigurationValueList;
		typedef std::pair< std::string, ConfigurationValueList > ConfigurationPair;
		typedef std::map< std::string, ConfigurationValueList > ConfigurationMap;
		typedef std::map< std::string, std::string > SingleValueConfigurationMap;
		typedef std::map< std::string, Configuration > SectionMap;
		typedef std::pair< std::string, Configuration > SectionPair;

		/**
		 * Constructor.
		 * @param fileSystem FileSystem to use to load files. Can be null but LoadFile() will not be available.
		 * @param functionBinder If a valid pointer, the function binder can be used while loading to call each line of the
		 * file as a function.
		 */
		Configuration(shared_ptr<FileSystem> fileSystem=shared_ptr<FileSystem>(), shared_ptr<FunctionBinder> functionBinder = shared_ptr<FunctionBinder>());
		~Configuration();

		/**
		 * Load options from a file.
		 * Configuration files can also specify other configuration files to load too using "include=filename[,optional]" option. The
		 * function will attempt to load relative files first. You can specify ",optional" if the file is considered optional which
		 * prevents the LoadFile() call from failing.
		 * @note Non-key-value-pair lines are stored with an empty key by default. If you need to save some memory you can change this
		 * behaviour with SetStoreLinesOnLoad(false).
		 * @param fileName name of the configuration file to attempt to open and load.
		 * @param parseLineThroughFunctionBinder if true and a function binder has been provided, will pass each line to the
		 * function binder to perform a function call. The function binder must be configured in advanced for this to work.
		 * @return true upon successful load, false if the passed in File object is not opened.
		 */
		bool LoadFile(const std::string& fileName, bool parseLineThroughFunctionBinder = false);
		
		/**
		 * Parse options from a vector of strings.
		 * If you already have the file contents loaded you can use this method.
		 * @see LoadFile().
		 * @param parentFileName The parent file name that include lines can be relative to.
		 * @return true upon successful parse, false if parsing the string failed usually a result of an include failure.
		 */
		bool ParseLines(const std::vector<std::string>& lines, bool parseLineThroughFunctionBinder = false, std::string parentFileName = "");

		/**
		 * Parse options from a string.
		 * The string will be split into lines.
		 * @see LoadFile().
		 * @param parentFileName The parent file name that include lines can be relative to.
		 * @return true upon successful parse, false if parsing the string failed usually a result of an include failure.
		 */
		bool ParseString(const std::string& content, bool parseLineThroughFunctionBinder = false, std::string parentFileName = "");
		
		/**
		 * Version of LoadFile() that supports loading into a section.
		 */
		bool LoadFile(const std::string& section, const std::string& fileName, bool parseLineThroughFunctionBinder = false);
		
		/**
		 * Version of ParseLines() that supports sections.
		 */
		bool ParseLines(const std::string& section, const std::vector<std::string>& lines, bool parseLineThroughFunctionBinder = false, std::string parentFileName = "");

		/**
		 * Version of ParseString() that supports sections.
		 */
		bool ParseString(const std::string& section, const std::string& content, bool parseLineThroughFunctionBinder = false, std::string parentFileName = "");
		
		/**
		 * Write configuration to a file.
		 * @note The Configuration object must have a FileSystem object in order for this method to succeed.
		 * @note To include a line such as a comment or function call add it to the configuration object
		 * with an empty key. For example: Add("","#your line).
		 * @note Lines are read in the order they are added (for index purposes), but written all at once so if you have a
		 * configuration file:
		 *		a=b
		 *		AFunctionCall()
		 *		c=d
		 *		AFunctionCall2()
		 * If the file is loaded then written the output will be:
		 *		AFunctionCall()
		 *		AFunctionCall2()
		 *		a=b
		 *		c=d
		 * This is due to internal sorting of values. This should be considered when overwriting a file where order is
		 * important, such as when substitution is used. Sections are written in blocks but the section order may change.
		 * @param fileName the name of the file to write.
		 * @param openMode lets you specify the open mode of the file. Change to File::OpenModes::APPEND if
		 * you this if you want to append to a file rather than create a new or overwrite an existing file.
		 * @note It is recommended to only use includes in files you will not write back as an include to load to due to
		 * ordering and the loss of include information (as the include is merged with the object it is included from).
		 * @note Sub sections are flattened during a write and each sub section becomes a section of the parent. This
		 * is because Configuration objects can load multiple files into the same object which is how includes are
		 * processed.
		 * @return true if the configuration was written successfully, otherwise false.
		 */
		bool WriteFile(const std::string& fileName, File::OpenMode openMode=File::OpenModes::WRITE) const;
		
		/**
		 * Overload of WriteFile that takes an already open File object.
		 */
		bool WriteFile(File& file) const;
		
		/**
		 * Parses a file as a simple script where each line is passed to the function binder.
		 * @param file Already open file to processed.
		 * @return false if there isn't a function binder or if the file is not opened, otherwise true.
		 */
		bool ParseConfigurationScript(File file);
		
		/**
		 * Get the number of options with the specified name.
		 * This will return the number of times a given option is specified, for example if you query with "person" and there is at least one
		 * line in the configuration file with "person" then the value returned will be 1 or more. The value represents the maxIndex+1 for
		 * that option.
		 * @param optionName
		 * @return the number of entries for the specified option name.
		 */
		size_t GetNumberOfOptionsNamed(const std::string& optionName) const;

		/**
		 * Return whether an option exists.
		 * @param optionName 
		 * @return true if the specified option exists, otherwise false.
		 */
		bool HasOption(const std::string& optionName) const;
		
		/**
		 * Get a configuration option value.
		 * Use this version of Get() if you don't care if the option is available or not.
		 * @param  optionName   The name of the option to use as a key for look-up.
		 * @param  defaultValue If the configuration option cannot be found, or conversion fails, this will be the value returned.
		 * @return The configuration option if found, else the default value.
		 */
		template< typename T >
		T Get(const std::string& optionName, const T& defaultValue) const
		{
			T outObject;
			Get(optionName, outObject, defaultValue, 0, true, false);
			return outObject;
		}
		
		template<size_t size>
		std::string Get(const std::string& optionName, const char (&defaultValue)[size]) const
		{
			return Get(optionName, std::string(defaultValue));
		}

		template< typename T >
		T GetNoCalc(const std::string& optionName, const T& defaultValue) const
		{
			T outObject;
			Get(optionName, outObject, defaultValue, 0, true, true);
			return outObject;
		}
		
		template<size_t size>
		std::string GetNoCalc(const std::string& optionName, const char (&defaultValue)[size]) const
		{
			return GetNoCalc(optionName, std::string(defaultValue));
		}

		/**
		 * Get a configuration option value at the specified index.
		 * Use this version of Get() if you don't care if the option is available or not.
		 * @param configurationName name of the option.
		 * @param defaultValue if the configuration option cannot be found or conversion fails this will be the value of outObject.
		 * @param index The index of the value.
		 * @return The configuration option or the default value.
		 */		
		template< typename T >
		T GetAtIndex(const std::string& optionName, const T& defaultValue, size_t index, bool suppressWarning = false, bool substituteOnly = false) const
		{
			T outObject;
			Get(optionName, outObject, defaultValue, index, suppressWarning, substituteOnly);
			return outObject;
		}
		
		/**
		 * Get a configuration option value.
		 * @param optionName name of the option.
		 * @param outObject The object to assign the value.
		 * @param defaultValue if the configuration option cannot be found or conversion fails this will be the value of outObject.
		 * @param index index of the configuration value if there are multiple of the same line.
		 * @param suppressWarning if true will suppress the option not found warning.
		 * @return true if successful
		 */
		template< typename T >
		bool Get(const std::string& optionName, T& outObject, const T& defaultValue, size_t index = 0, bool suppressWarning = false, bool substituteOnly = false) const
		{
			ConfigurationMap::const_iterator it = mConfigurationOptions.find(optionName);
			if(it==mConfigurationOptions.end())
			{
				if(!suppressWarning)
				{
					ECHO_LOG_WARNING("Option '" << optionName << "' not found.");
				}
				outObject = defaultValue;
				return false;
			}
			if(index>=it->second.size())
			{
				if(!suppressWarning)
				{
					ECHO_LOG_WARNING("Option not found for '" << optionName << "' at index " << index);
				}
				outObject = defaultValue;
				return false;
			}
			
			// Get a variable replacement first so you can do things like:
			// x = 10
			// y = 2
			// z = $y * x
			std::string parsed = mVariableSubstitutor.Parse(it->second[index]);
			optional<double> result;
			if(!substituteOnly)
			{
				result = mParser.Parse(parsed);
			}
			if(result)
			{
				std::stringstream ss;
				ss << std::setprecision(mStreamPrecision) << result.value();
				ss >> std::setprecision(mStreamPrecision) >> outObject;
				if((ss.fail()))
				{
					outObject = defaultValue;
					return false;
				}
			}else
			{
				std::stringstream ss(parsed);
				ss >> std::setprecision(mStreamPrecision) >> outObject;
				if((ss.fail()))
				{
					outObject = defaultValue;
					return false;
				}
			}
			return true;
		}
		
		template<size_t size>
		bool Get(const std::string& optionName, std::string& outObject, const char (&defaultValue)[size], size_t index = 0, bool suppressWarning = false, bool substituteOnly = false) const
		{
			return Get(optionName, outObject, std::string(defaultValue), index, suppressWarning, substituteOnly);
		}
		
		/**
		 * Like GetOrFail() except it doesn't take a default value so the outObject is untouched if this method returns false.
		 */
		template< typename T >
		bool GetOrFail(const std::string& optionName, T& outObject, size_t index = 0, bool suppressWarning = false, bool substituteOnly = false) const
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
			if(index>=it->second.size())
			{
				if(!suppressWarning)
				{
					ECHO_LOG_WARNING("Option not found for '" << optionName << "' at index " << index);
				}
				return false;
			}
			std::string parsed = mVariableSubstitutor.Parse(it->second[index]);
			optional<double> result;
			if(!substituteOnly)
			{
				result = mParser.Parse(parsed);
			}
			if(result)
			{
				std::stringstream ss;
				ss << std::setprecision(mStreamPrecision) << result.value();
				ss >> std::setprecision(mStreamPrecision) >> outObject;
				if((ss.fail()))
				{
					return false;
				}
			}else
			{
				std::stringstream ss(parsed);
				ss >> std::setprecision(mStreamPrecision) >> outObject;
				if((ss.fail()))
				{
					return false;
				}
			}
			return true;
		}
		
		/**
		 * Get all of the option values in a vector using the specified key.
		 * This method will attempt to convert all of the values of all entries that have the specified key to the specified type.
		 * If any of the conversions fail the method will return an empty list.
		 * If the key isn't found the method will return an empty list.
		 * @param optionName The name, or key, to look up.
		 * @param suppressWarning if true any warnings will be suppressed
		 * @return An empty vector if the option name isn't found or if a conversion fails, otherwise a vector containing the converted values.
		 */
		template< typename T >
		std::vector<T> GetAll(const std::string& optionName, bool suppressWarning = false, bool substituteOnly = false) const
		{
			ConfigurationMap::const_iterator it = mConfigurationOptions.find(optionName);
			if(it==mConfigurationOptions.end())
			{
				if(!suppressWarning)
				{
					ECHO_LOG_WARNING("Option '" << optionName << "' not found.");
				}
				return std::vector<T>();
			}
			
			std::vector<T> outVector;
			Size numberOfFailedConversions = 0;
			Size numberOfOptions = GetNumberOfOptionsNamed(optionName);
			for(Size i = 0; i < numberOfOptions; ++i)
			{
				T converted;
				if(GetOrFail(optionName, converted, i, suppressWarning, substituteOnly))
				{
					outVector.push_back(converted);
				}else
				{
					// I've decided to continue so we can report all errors if suppressWarning is false
					++numberOfFailedConversions;
				}
			}

			if(numberOfFailedConversions>0)
			{
				if(!suppressWarning)
				{
					ECHO_LOG_WARNING("Failed to convert " << numberOfFailedConversions << " entries to " << typeid(T).name());
				}
				return std::vector<T>();
			}
			return outVector;
		}
		
		/**
		 * A version of the above method because strings do not need to be converted.
		 * We cannot overload this because the only thing that differs is the return type.
		 */
		std::vector<std::string> GetAllAsStrings(const std::string& optionName, bool suppressWarning = false) const;
		
		/**
		 * Get all option names
		 */
		std::vector< std::string > GetAllOptionNames() const;
		
		/**
		 * Set a configuration option value.
		 * Use this version of Get() if you don't care if the option is available or not.
		 * @param optionName name of the option.
		 * @param optionValue The value to set option.
		 * @return The configuration option or the default value.
		 */	
		template< typename T >
		bool Set(const std::string& optionName, const T& optionValue, bool overwriteExisting = true)
		{
			std::stringstream ss;
			ss << std::setprecision(mStreamPrecision) << optionValue;
			if(ss.fail())
			{
				return false;
			}
			return Set(optionName,ss.str(),overwriteExisting);
		}
		
		/**
		 * Set an option's value by name with the option to overwrite existing values.
		 * @param optionName The option's name, used to reference the option.
		 * @param optionValue The value of the option.
		 * @param overwriteExisting if true any options with the same name will be overwritten.
		 * @return If overwriteExisting is false and the option exists the method will return false,
		 * otherwise true.
		 */
		bool Set(const std::string& optionName, const std::string& optionValue, bool overwriteExisting = true);

		/**
		 * Add an option and attempt to automatically convert to a storage string.
		 * This method appends the value to the option name and never overwrites.
		 * The type should be convertible using a stringstream.
		 * @param optionName the name of the option.
		 * @param optionValue The value.
		 * @return true if the option is added, false if conversion failed.
		 */
		template< typename T >
		bool Add(const std::string& optionName, const T& optionValue)
		{
			std::stringstream ss;
			ss << std::setprecision(mStreamPrecision) << optionValue;
			if(ss.fail())
			{
				return false;
			}
			Add(optionName,ss.str());
			return true;
		}

		/**
		 * Add an option.
		 * This method appends the value to the option name and never overwrites.
		 * @param optionName the option name
		 * @param optionValue
		 */
		void Add(const std::string& optionName, const std::string& optionValue);

		/**
		 * Add a "line" to the configuration object.
		 * Lines are added as configuration options with empty keys so you can access them. Normally you won't
		 * bother using lines unless you are planning to write the configuration file out.
		 * Lines are written in the order they are added.
		 * @param line The contents of a line to include in the configuration file.
		 */
		void AddLine(const std::string& line)
		{
			Add("",line);
		}
		
		/**
		 * Add multiple options at once using a string-string map.
		 * @param optionsMap The options.
		 */
		void AddOptions(const SingleValueConfigurationMap& optionsMap);
		
		/**
		 * Remove an option by name.
		 * @param optionName
		 * @return false if the option doesn't exist, otherwise true if the option was found and removed.
		 */
		bool Remove(const std::string& optionName);

		/**
		 * Remove an option by name at index.
		 * @param optionName Option name
		 * @param index Index of option to remove
		 * @return false if the option or index doesn't exist, otherwise true if the option at index was found and removed.
		 */
		bool RemoveAtIndex(const std::string& optionName, Size index);

		/**
		 * Clear all values previously added/set, including all subsections.
		 */
		void Clear();
		
		/**
		 * Version of GetNumberOfOptionsNamed() that supports sections.
		 */
		size_t GetNumberOfOptionsNamedInSection(const std::string& section, const std::string& optionName) const;

		/**
		 * Version of HasOption() that supports sections.
		 */
		bool HasOptionInSection(const std::string& section, const std::string& optionName) const;
		
		/**
		 * Version of Get() supporting section access.
		 */
		template< typename T >
		T GetFromSection(const std::string& section, const std::string& optionName, const T& defaultValue, bool suppressWarning = false, bool substituteOnly = false) const
		{
			T outObject;
			GetFromSection(section, optionName, outObject, defaultValue, 0, suppressWarning, substituteOnly);
			return outObject;
		}
		
		template<size_t size>
		std::string GetFromSection(const std::string& section, const std::string& optionName, const char (&defaultValue)[size], bool suppressWarning = false, bool substituteOnly = false) const
		{
			return GetFromSection(section, optionName, std::string(defaultValue), suppressWarning, substituteOnly);
		}
		
		/**
		 * Version of GetAtIndex() that supports sections
		 */		
		template< typename T >
		T GetFromSectionAtIndex(const std::string& section, const std::string& optionName, const T& defaultValue, size_t index, bool suppressWarning = false, bool substituteOnly = false) const
		{
			T outObject;
			GetFromSection(section, optionName, outObject, defaultValue, index, suppressWarning, substituteOnly);
			return outObject;
		}

		/**
		 * Version of Get() that supports sections
		 */
		template< typename T >
		bool GetFromSection(const std::string& section, const std::string& optionName, T& outObject, const T& defaultValue, size_t index = 0, bool suppressWarning = false, bool substituteOnly = false) const
		{
			if(section.empty())
			{
				return Get(optionName, outObject, defaultValue, index, suppressWarning, substituteOnly);
			}
			
			std::string subsections;
			const Configuration* thisSection = GetSection(section,subsections);
			if(!thisSection)
			{
				ECHO_LOG_WARNING("Unable to find section " << thisSection << " could not look up " << optionName);
				return false;
			}
			// When a separator isn't found subsections will be empty causing a normal lookup to occur.
			return thisSection->GetFromSection<T>(subsections, optionName, outObject, defaultValue, index, suppressWarning, substituteOnly);
		}
		
		/**
		 * Version of GetOrFail() that supports sections
		 */
		template< typename T >
		bool GetFromSectionOrFail(const std::string& section, const std::string& optionName, T& outObject, size_t index = 0, bool suppressWarning = false, bool substituteOnly = false) const
		{
			if(section.empty())
			{
				return GetOrFail<T>(optionName, outObject, index, suppressWarning, substituteOnly);
			}
			
			std::string subsections;
			const Configuration* thisSection = GetSection(section,subsections);
			if(!thisSection)
			{
				ECHO_LOG_WARNING("Unable to find section " << thisSection << " could not look up " << optionName);
				return false;
			}

			// When a separator isn't found subsections will be empty causing a normal lookup to occur.
			return thisSection->GetFromSectionOrFail<T>(subsections, optionName, outObject, index, suppressWarning, substituteOnly);
		}

		/**
		 * Version of GetAll() that supports sections
		 */
		template< typename T >
		std::vector<T> GetAllFromSection(const std::string& section, const std::string& optionName, bool suppressWarning = false, bool substituteOnly = false) const
		{
			if(section.empty())
			{
				return GetAll<T>(optionName, suppressWarning, substituteOnly);
			}
			
			std::string subsections;
			const Configuration* thisSection = GetSection(section,subsections);
			if(!thisSection)
			{
				ECHO_LOG_WARNING("Unable to find section " << thisSection << " could not look up " << optionName);
				return false;
			}

			// When a separator isn't found subsections will be empty causing a normal lookup to occur.
			return thisSection->GetAllFromSection<T>(subsections, optionName, suppressWarning, substituteOnly);			
		}
		
		/**
		 * Version of GetAllAsStrings() that supports sections
		 */
		std::vector<std::string> GetAllFromSectionAsStrings(const std::string& section, const std::string& optionName, bool suppressWarning = false) const;
		
		/**
		 * Version of Set() that supports sections.
		 */	
		template< typename T >
		bool SetInSection(const std::string& section, const std::string& optionName, const T& optionValue, bool overwriteExisting = true)
		{
			if(section.empty())
			{
				return Set<T>(optionName, optionValue, overwriteExisting);
			}

			std::string subsections;
			Configuration& thisSection = GetOrCreateSection(section,subsections);
			return thisSection.SetInSection<T>(subsections, optionName, optionValue, overwriteExisting);
		}
		
		/**
		 * Version of Add() that supports sections.
		 */
		template< typename T >
		bool AddToSection(const std::string& section, const std::string& optionName, const T& optionValue)
		{
			if(section.empty())
			{
				return Add<T>(optionName, optionValue);
			}

			std::string subsections;
			Configuration& thisSection = GetOrCreateSection(section,subsections);
			return thisSection.AddToSection<T>(subsections, optionName, optionValue);			
		}

		/**
		 * Version of AddLine() that supports sections.
		 */
		void AddLineToSection(const std::string& section, const std::string& line)
		{
			AddToSection(section,"",line);
		}
		
		/**
		 * Version of AddOptions() that supports sections
		 */
		void AddOptionsToSection(const std::string& section, const SingleValueConfigurationMap& optionsMap);
		
		/**
		 * Version of Remove() that supports sections.
		 */
		bool RemoveFromSection(const std::string& section, const std::string& optionName);

		/**
		 * Remove option by name at index in section.
		 * @param sectionName Section name. If empty it will call remove on this object.
		 * @param optionName Option name
		 * @param index Index of option to remove
		 * @return false if the option or index doesn't exist, otherwise true if the option at index was found and removed.
		 */
		bool RemoveFromSectionAtIndex(const std::string& sectionName, const std::string& optionName, Size index);

		/**
		 * Clear all values previously added/set in the specified section.
		 * If section is empty the object is cleared, including all subsections.
		 */
		void ClearSection(const std::string& section);
		
		void SetFunctionBinder(shared_ptr<FunctionBinder> functionBinder) {mFunctionBinder=functionBinder;}
		shared_ptr<FunctionBinder> GetFunctionBinder() const {return mFunctionBinder;}

		void SetFileSystem(shared_ptr<FileSystem> fileSystem) {mFileSystem=fileSystem;}
		shared_ptr<FileSystem> GetFileSystem() const {return mFileSystem;}
		
		/**
		 * Set whether to store non-key-value-pair lines on file load.
		 */
		void SetStoreLinesOnLoad(bool store){mStoreLinesOnLoad=store;}

		/**
		 * Get whether to store non-key-value-pair lines on file load.
		 */
		bool GetStoreLinesOnLoad() const {return mStoreLinesOnLoad;}
		
		/**
		 * Sets the precision of the streams for reading and writing values.
		 * This affects any future reads and writes.
		 * @param streamPrecision The stream precision, see std::setprecision()
		 */
		void SetStreamPrecision(Size streamPrecision)
		{
			mStreamPrecision = streamPrecision;
		}

		/**
		 * Get the current precision used for stream operations.
		 * @see SetStreamPrecision()
		 */
		Size GetStreamPrecision() const {return mStreamPrecision;}

		Configuration* GetSection(const std::string& sectonPath);
		
		const Configuration* GetSection(const std::string& sectonPath) const;

		/**
		 * Named section.
		 * Because it is nicer to reference mName or mSection than first and second from a std::pair
		 */
		struct NamedSection
		{
			std::string mName;
			const Configuration* mSection;
		};
		
		/**
		 * Get all sections with their names
		 * @return a vector of NamedSections in the order they were created.
		 */
		std::vector< NamedSection > GetAllSections() const;
		
		/**
		 * Get all section names
		 * @return a vector of section names in the order the sections were created.
		 */
		std::vector< std::string > GetAllSectionNames() const;

		Configuration& GetOrCreateSection(const std::string& sectonPath);
		
		void SetSectionsInheritVariables(bool sectionsInheritVariables);
		bool GetSectionsInheritVariables() const;
		
		template< typename T >
		bool ForEach(const std::string& optionName, boost::function<bool(const T&)> operation, bool suppressWarning = false, bool substituteOnly = false) const
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
				T converted;
				std::string parsed = mVariableSubstitutor.Parse(*itList);
				optional<double> result;
				if(!substituteOnly)
				{
					mParser.Parse(parsed);
				}
				if(result)
				{
					std::stringstream ss;
					ss << std::setprecision(mStreamPrecision) << result.value();
					ss >> std::setprecision(mStreamPrecision) >> converted;
					if((ss.fail()))
					{
						if(!suppressWarning)
						{
							ECHO_LOG_WARNING("Failed to convert option '" << optionName << "' with data '" << (parsed) << "'");
						}
						return false;
					}
				}else
				{
					std::stringstream ss(parsed);
					ss >> std::setprecision(mStreamPrecision) >> converted;
					if((ss.fail()))
					{
						if(!suppressWarning)
						{
							ECHO_LOG_WARNING("Failed to convert option '" << optionName << "' with data '" << (parsed) << "'");
						}
						return false;
					}
				}
				
				if(!operation(converted))
				{
					if(!suppressWarning)
					{
						ECHO_LOG_WARNING("Operation callback failed for '" << optionName << "' with data '" << (parsed) << "'");
						return false;
					}
				}
				itList++;
			}
			return true;
		}
		
		template< typename T >
		bool ForEachInSection(const std::string& optionName, const std::string& sectionName, boost::function<bool(const T&)> operation , bool suppressWarning = false, bool substituteOnly = false) const
		{
			const Configuration* section = GetSection(sectionName);
			if(!section)
			{
				if(!suppressWarning)
				{
					ECHO_LOG_WARNING("Could not find section '" << sectionName << "'");
				}
				return false;
			}
			return section->ForEach<T>(optionName, operation, suppressWarning, substituteOnly);
		}
	private:
		Configuration* GetSection(const std::string& sectonPath, std::string& subsections);
		const Configuration* GetSection(const std::string& sectonPath, std::string& subsections) const;
		Configuration& GetOrCreateSection(const std::string& sectonPath, std::string& subsections);
		void RegisterOptionWithParsers(const std::string& optionName);
		void RegisterOptionWithParsers(const std::string& section, const std::string& optionName);
		ConfigurationMap mConfigurationOptions;
		SectionMap mSections;
		std::list< std::string > mSectionOrder;
		shared_ptr<FileSystem> mFileSystem;
		shared_ptr<FunctionBinder> mFunctionBinder;
		Parser::CalculatorWithVariables mParser;
		Parser::VariableSubstitutor mVariableSubstitutor;
		Size mFileDepth;
		Size mStreamPrecision;
		bool mStoreLinesOnLoad;
		bool mSectionsInheritVariables;
	};
	
	/**
	 * Specialisation of Get() for bool to support converting "1", "0", "true" and "false".
	 */
	template<>
	bool Configuration::Get<bool>(const std::string& optionName, bool& outObject, const bool& defaultValue, size_t index, bool suppressWarning, bool substituteOnly) const;

	/**
	 * Specialisation of Get() for std::string to prevent the string ending at whitespae.
	 */
	template<>
	bool Configuration::Get(const std::string& configurationName, std::string& outObject, const std::string& defaultValue, size_t index, bool suppressWarning, bool substituteOnly) const;

	/**
	 * Specialisation of GetOrFail() for std::string to prevent the string ending at whitespae.
	 */
	template<>
	bool Configuration::GetOrFail<std::string>(const std::string& optionName, std::string& outObject, size_t index, bool suppressWarning, bool substituteOnly) const;

	/**
	 * Specialisation of ForEach() for std::string to prevent the string ending at whitespae.
	 */
	template<>
	bool Configuration::ForEach<std::string>(const std::string& optionName, boost::function<bool(const std::string&)> operation, bool suppressWarning, bool substituteOnly) const;
}

#endif
