#include <echo/Platform.h>
#include <echo/Kernel/SimpleExecutionModel.h>
#include <boost/make_shared.hpp>

namespace Echo
{
	namespace Platform
	{
		shared_ptr<ExecutionModel> CreateExecutionModel()
		{
			return shared_ptr<ExecutionModel>(new SimpleExecutionModel());
		}
	}
}
