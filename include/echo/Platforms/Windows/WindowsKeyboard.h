#ifndef _WINDOWSKEYBORAD_H_
#define _WINDOWSKEYBORAD_H_
#ifdef ECHO_PLATFORM_WINDOWS
#include <echo/Input/InputDevice.h>

namespace Echo
{
	class WindowsKeyboard : public InputDevice
	{
	public:
		struct KeyImp
		{
			KeyImp(u16 code, const std::string& keyName) :
				mCode(code),
				mDown(new bool),
				mHit(false),
				mLifted(false),
				mValue(keyName)
			{
				*mDown=false;
			}
			u16 mCode;
			shared_ptr<bool> mDown;
			bool mHit;
			bool mLifted;
			std::string mValue;
		};
		struct Keys
		{
			enum KeyEnum
			{
				KEY_ESCAPE,

				KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, 
				KEY_F9, KEY_F10, KEY_F11, KEY_F12,

				KEY_PRINT_SCREEN, KEY_SCROLL, 

				// KEY_PAUSE
				// Left out deliberately. Doesn't work like other keys.

				KEY_APOSTROPHE, KEY_TAB, KEY_CAPS, 
				KEY_LSHIFT, KEY_LCONTROL, KEY_LALT, 
				KEY_SPACE, 
				KEY_RSHIFT, KEY_RCONTROL, KEY_RALT, 
				KEY_SUBTRACT, KEY_EQUALS, KEY_BACKSLASH, KEY_BACKSPACE, KEY_ENTER, 

				KEY_LSQUARE_BRACKET, KEY_RSQUARE_BRACKET, 
				KEY_SEMICOLON, KEY_QUOTE,
				KEY_COMMA, KEY_FULLSTOP, KEY_FORWARD_SLASH,

				KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0,

				KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J,
				KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T,
				KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,

				KEY_INSERT, KEY_HOME, KEY_PAGE_UP, 
				KEY_DELETE, KEY_END, KEY_PAGE_DOWN,

				KEY_LEFT, KEY_UP, KEY_RIGHT, KEY_DOWN,

				KEY_NUM_LOCK, 
				KEY_NUM_DIVIDE, KEY_NUM_MULTIPLY, KEY_NUM_MINUS, KEY_NUM_ADD,
				KEY_NUM_1, KEY_NUM_2, KEY_NUM_3,
				KEY_NUM_4, KEY_NUM_5, KEY_NUM_6,
				KEY_NUM_7, KEY_NUM_8, KEY_NUM_9,
				KEY_NUM_0, KEY_NUM_DECIMAL,
				KEY_NUM_CLEAR /* 5 on the Num Pad when Num Lock not on */,

				KEY_COUNT,  // Not an actual key, tells how many keys in the enum
				KEY_INVALID = KEY_COUNT,
			};
		};
		const u16 KEY_DOWN_MASK;
	private:
		std::vector<KeyImp*> mKeys;
		std::string mKeyData;
		std::string mKeyStrokeData;
		void AddKey(u16 code, const std::string& keyName);
	public:
		WindowsKeyboard();
		~WindowsKeyboard();

		void Update(Seconds lastFrameTime);
	};
}
#endif
#endif
