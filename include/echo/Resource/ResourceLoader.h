#ifndef _ECHORESOURCELOADER_H_
#define _ECHORESOURCELOADER_H_
#include <echo/Types.h>

namespace Echo
{
	/**
	 * Interfaace for loading resources
	 */
	template< typename ResourceType >
	class ResourceLoader
	{
	public:
		virtual ~ResourceLoader(){}

		/**
		 * Load a resource into an object.
		 * @param resourceName The resource name to load. This usually refers to a resource in a resource manager
		 * @param resourceToLoadInto The object to load into. It can be assumed this object is in an initial state,
		 * i.e. the implementation does not need to clear this object.
		 * @return true on success, otherwise false. On failure the method should either restore the resource object
		 * back to the original state or have not modified it.
		 */
		virtual bool LoadIntoResource(const std::string& resourceName, ResourceType& resourceToLoadInto) = 0;
	};

}
#endif
