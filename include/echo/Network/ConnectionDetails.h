#ifndef _CONNECTIONDETAILS_H_
#define _CONNECTIONDETAILS_H_
#include <echo/Util/Configuration.h>

namespace Echo
{
	//When a connectionDetailsString is passed into Connect or Listen it is exploded into
	//one of these structures before being passed to the appropriate system.
	class ConnectionDetails
	{
	private:
		std::string mSystem;
		std::string mType;
		std::string mAddress;
		Configuration mAdditionalInfo;
		bool mHasSystem;
		bool mHasType;
		bool mHasAddress;
		bool mHasAdditionalInfo;
	public:
		ConnectionDetails();
		explicit ConnectionDetails(const std::string& connectionString);
		void ConvertConnectionDetails(const std::string& connectionString);
		const std::string& GetSystem() const {return mSystem;}
		const std::string& GetType() const {return mType;}
		const std::string& GetAddress() const {return mAddress;}
		
		const Configuration& GetAllAdditionalInfo() const {return mAdditionalInfo;}
		
		/**
		 * Get an additional option as a name.
		 * When options are supplied as "name=value" they can be accessed by name with this.
		 * @note If an option with the same name is specified multiple times it will not
		 * be accessible, only the first value will be used.
		 */
		template< typename T >
		T GetAdditionalInfo(const std::string& name, const T& defaultValue) const
		{
			return mAdditionalInfo.Get(name, defaultValue);
		}

		/**
		 * For each option that has not been specified as a key value pair they will be available at an index in the order they appear.
		 * @note The index count does not include the key value count, so it is not a good idea to mix non-key-value pairs and index
		 * additional info, unless all key value pairs are after index types.
		 */
		template< typename T >
		T GetAdditionalInfoAt(Size index, const T& defaultValue) const
		{
			return mAdditionalInfo.Get("", defaultValue);
		}

		/**
		 * Helper to get index or named info.
		 */
		template< typename T >
		T GetAdditionalInfoWithIndexFallback(const std::string& name, Size orAtIndex, const T& defaultValue) const
		{
			return mAdditionalInfo.Get(name, GetAdditionalInfoAt(orAtIndex, defaultValue));
		}
		
		bool HasType() const {return mHasType;}
		bool HasSystem() const {return mHasSystem;}
		bool HasAddress() const {return mHasAddress;}
		bool HasAdditionalInfo() const {return mHasAdditionalInfo;}
		std::string ToString() const;
	};
}
#endif
