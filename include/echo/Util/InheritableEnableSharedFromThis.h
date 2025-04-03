#ifndef ECHOINHERITABLEENABLESHAREDFROMTHIS_H
#define ECHOINHERITABLEENABLESHAREDFROMTHIS_H

#include <echo/Types.h>

/*
 * The following is adapted from: http://stackoverflow.com/questions/16082785/use-of-enable-shared-from-this-with-multiple-inheritance
 */

namespace Echo
{
	class MultipleInheritableEnableSharedFromThis : public enable_shared_from_this<MultipleInheritableEnableSharedFromThis>
	{
	public:
		virtual ~MultipleInheritableEnableSharedFromThis(){}
	};
	
	/**
	 * A class used to ensure only one shared pointer is around when using enable_shared_from_this.
	 */
	template <class T>
	class InheritableEnableSharedFromThis : virtual public MultipleInheritableEnableSharedFromThis
	{
	public:
		shared_ptr<T> shared_from_this()
		{
			return dynamic_pointer_cast<T>(MultipleInheritableEnableSharedFromThis::shared_from_this());
		}
		
		/* Utility method to easily downcast.
		* Useful when a child doesn't inherit directly from enable_shared_from_this
		* but wants to use the feature.
		*/
		template <class DownType>
		shared_ptr<DownType> shared_from_this()
		{
			return dynamic_pointer_cast<DownType>(MultipleInheritableEnableSharedFromThis::shared_from_this());
		}
	};	
}

#endif
