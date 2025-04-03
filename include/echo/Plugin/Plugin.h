#ifndef _ECHOPLUGIN_H_
#define _ECHOPLUGIN_H_
#include <echo/Kernel/Kernel.h>

namespace Echo
{
	/**
	 * Plugin
	 * Plugins should be loaded via the Platform interface.
	 * Provides access to task creation and function registration.
	 */
	class Plugin
	{
	public:
		Plugin(){}
		virtual ~Plugin(){}
		
		bool HasTask(const std::string& taskName) const
		{
			return (mCreators.find(taskName)!=mCreators.end());
		}

		shared_ptr<Task> CreateTask(const std::string& taskName)
		{
			TaskCreatorMap::iterator it = mCreators.find(taskName);
			if(it==mCreators.end())
			{
				return shared_ptr<Task>();
			}
			return it->second();
		}

		virtual bool RegisterFunctions(FunctionBinder& functionBinder) = 0;
	protected:
		typedef function< shared_ptr<Task>() > TaskCreatorFunction;
		typedef std::map< std::string, TaskCreatorFunction > TaskCreatorMap;
		
		template< T >
		void RegisterTaskCreator(const std::string& taskName)
		{
			mCreators[taskName] = bind(&Plugin::CreateTaskT<T>,this,taskName);
		}

		template< T >
		shared_ptr<Task> CreateTaskT(const std::string& taskName)
		{
			return shared_ptr<T>(new T(taskName));
		}
		TaskCreatorMap mCreators;
	};
}
#endif
