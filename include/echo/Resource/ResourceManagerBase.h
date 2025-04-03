#ifndef _ECHORESOURCEMANAGERBASE_H_
#define _ECHORESOURCEMANAGERBASE_H_

#include <echo/FileSystem/File.h>

namespace Echo
{
	/**
	 * Base class for ResourceManager<T> for generic functionality.
	 */
	class ResourceManagerBase
	{
	public:
		ResourceManagerBase(){}
		virtual ~ResourceManagerBase(){}
		virtual bool LoadList(File listFile) = 0;
		virtual const std::string& GetResourceTypeName() const = 0;
	};
}
#endif 

