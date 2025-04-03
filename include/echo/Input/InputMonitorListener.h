#ifndef _ECHOINPUTMONITORLISTENER_H_
#define _ECHOINPUTMONITORLISTENER_H_

#include <echo/Input/Input.h>

namespace Echo
{
	class InputMonitorListener
	{
	private:
		weak_ptr<InputMonitor> mMonitor;
		friend class InputMonitor;
		void _SetMonitor(shared_ptr<InputMonitor> monitor)
		{
			mMonitor = monitor;
		}
	public:
		InputMonitorListener(){}
		virtual ~InputMonitorListener(){}
		shared_ptr<InputMonitor> GetMonitor() const {return mMonitor.lock();}
		virtual void OnDigitalInput(const std::string& deviceName, const std::string& inputName, shared_ptr< Input< bool > > input){}
		virtual void OnAnalogInput(const std::string& deviceName, const std::string& inputName, shared_ptr< Input< f32 > > input){}
	};
}
#endif 
