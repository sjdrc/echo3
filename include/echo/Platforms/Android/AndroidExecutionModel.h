#include <echo/Kernel/ExecutionModel.h>

namespace Echo
{
	class AndroidExecutionModel : public ExecutionModel
	{
	public:
		AndroidExecutionModel();
		virtual ~AndroidExecutionModel();

		bool SupportsModel(Model model);
		bool ProcessEvents(f32 timeOutSuggestion);
		bool EnterSystemEventManager();
		bool SendUpdateRequest();
	};
}
