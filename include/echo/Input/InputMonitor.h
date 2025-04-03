#ifndef _ECHOINPUTMONITOR_H_
#define _ECHOINPUTMONITOR_H_

#include <echo/Input/Input.h>
#include <set>

namespace Echo
{
	class InputMonitorListener;

	class InputMonitor : public Task
	{
	public:
		InputMonitor(shared_ptr<InputManager> inputManager);
		~InputMonitor();
		void AddDeviceToIgnoreList(const std::string& deviceName);
		void AddListener(shared_ptr<InputMonitorListener> listener);
		void RemoveListener(shared_ptr<InputMonitorListener> listener);
		void Update(Seconds lastFrameTime);
	private:
		shared_ptr<InputManager> mInputManager;
		std::list< shared_ptr<InputMonitorListener> > mNextInputListeners;
		std::set< std::string > mDevicesToIgnore;
		void _NotifyDigitalInput(const std::string& deviceName, const std::string& inputName, shared_ptr< Input<bool> > input );
		void _NotifyAnalogInput(const std::string& deviceName, const std::string& inputName, shared_ptr< Input<f32> > input );
	};
}
#endif 
