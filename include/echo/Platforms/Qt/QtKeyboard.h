#ifndef _ECHOQGTKEYBOARD_H_
#define _ECHOQGTKEYBOARD_H_
#include <echo/Input/InputDevice.h>
#include <echo/Platforms/Qt/QtWindow.h>

namespace Echo
{
	class QtKeyboard : public InputDevice, public QtWindowEventListener
	{
	private:
		shared_ptr<QtWindow> mWindow;

		struct Key
		{
			shared_ptr<bool> mDown;
			std::string mValue;
		};
		std::map< u32, Key > mKeys;
		void AddKey(u32 code, const std::string& keyName);
	public:
		QtKeyboard(shared_ptr<QtWindow> window);
		~QtKeyboard();

		void MouseMoved(f32 newX, f32 newY);
		void LeftButtonStateChanged(f32 atX, f32 atY, bool state);
		void RightButtonStateChanged(f32 atX, f32 atY, bool state);
		void MiddleButtonStateChanged(f32 atX, f32 atY, bool state);
		void KeyStateChanged(u32 key, bool state);
	};
}
#endif
