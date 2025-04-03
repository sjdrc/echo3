#ifndef _ECHOGTKKEYBOARD_H_
#define _ECHOGTKKEYBOARD_H_
#include <echo/Input/InputDevice.h>
#include <echo/Platforms/GTK/GTKWindow.h>

namespace Echo
{
	class GTKKeyboard : public InputDevice, public GTKWindowEventListener
	{
	private:
		shared_ptr<GTKWindow> mWindow;

		struct Key
		{
			shared_ptr<bool> mDown;
			std::string mValue;
		};
		std::map< u32, Key > mKeys;
		void AddKey(u32 code, const std::string& keyName);
	public:
		GTKKeyboard(shared_ptr<GTKWindow> window);
		~GTKKeyboard();

		void MouseMoved(f32 newX, f32 newY);
		void LeftButtonStateChanged(f32 atX, f32 atY, bool state);
		void RightButtonStateChanged(f32 atX, f32 atY, bool state);
		void MiddleButtonStateChanged(f32 atX, f32 atY, bool state);
		void KeyStateChanged(u32 key, bool state);
	};
}
#endif
