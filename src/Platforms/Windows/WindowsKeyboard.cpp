#ifdef ECHO_PLATFORM_WINDOWS
#include <echo/Platforms/Windows/WindowsKeyboard.h>
#include <windows.h>
#undef ERROR
#include <iostream>
namespace Echo
{
	const u16 KEY_DOWN_MASK = (u16)0x8000U;

	struct KeyData
	{
		u16 virtualKey;
		const char* stringRep;
	};

	const KeyData virtualKeyMapping[] = 
	{
		{ VK_ESCAPE,   "Esc"       }, // KEY_ESCAPE

		{ VK_F1,       "F1"        }, // KEY_F1
		{ VK_F2,       "F2"        }, // KEY_F2
		{ VK_F3,       "F3"        }, // KEY_F3
		{ VK_F4,       "F4"        }, // KEY_F4
		{ VK_F5,       "F5"        }, // KEY_F5
		{ VK_F6,       "F6"        }, // KEY_F6
		{ VK_F7,       "F7"        }, // KEY_F7
		{ VK_F8,       "F8"        }, // KEY_F8 
		{ VK_F9,       "F9"        }, // KEY_F9
		{ VK_F10,      "F10"       }, // KEY_F10
		{ VK_F11,      "F11"       }, // KEY_F11
		{ VK_F12,      "F12"       }, // KEY_F12

		{ VK_SNAPSHOT, "PrintScreen"}, // KEY_PRINT_SCREEN, 
		{ VK_SCROLL,   "Scroll"    }, // KEY_SCROLL

		{ VK_OEM_3,    "`"         }, // KEY_APOSTROPHE
		{ VK_TAB,      "Tab"       }, // KEY_TAB
		{ VK_CAPITAL,  "CapsLock"  }, // KEY_CAPS
		{ VK_LSHIFT,   "LShift"    }, // KEY_LSHIFT
		{ VK_LCONTROL, "LCtrl"     }, // KEY_LCTRL
		{ VK_LMENU,    "LAlt"      }, // KEY_LALT 
		{ VK_SPACE,    "Space"     }, // KEY_SPACE 
		{ VK_RSHIFT,   "RShift"    }, // KEY_RSHIFT
		{ VK_RCONTROL, "RCtrl"     }, // KEY_RCTRL
		{ VK_RMENU,    "RAlt"      }, // KEY_RALT, 
		{ VK_OEM_MINUS,"-"         }, // KEY_SUBTRACT
		{ VK_OEM_PLUS, "="         }, // KEY_EQUALS
		{ VK_OEM_5,    "\\"        }, // KEY_BACKSLASH
		{ VK_BACK,     "Backspace" }, // KEY_BACKSPACE
		{ VK_RETURN,   "Enter"     }, // KEY_ENTER

		{ VK_OEM_4,    "["         }, // KEY_LSQUARE_BRACKET
		{ VK_OEM_6,    "]"         }, // KEY_RSQUARE_BRACKET
		{ VK_OEM_1,    ";"         }, // KEY_SEMICOLON
		{ VK_OEM_7,    "'"         }, // KEY_QUOTE
		{ VK_OEM_COMMA,","         }, // KEY_COMMA
		{ VK_OEM_PERIOD,"."        }, // KEY_FULLSTOP
		{ VK_OEM_2,    "/"         }, // KEY_FORWARD_SLASH

		{ '1',         "1"         }, // KEY_1
		{ '2',         "2"         }, // KEY_2
		{ '3',         "3"         }, // KEY_3
		{ '4',         "4"         }, // KEY_4
		{ '5',         "5"         }, // KEY_5
		{ '6',         "6"         }, // KEY_6
		{ '7',         "7"         }, // KEY_7
		{ '8',         "8"         }, // KEY_8
		{ '9',         "9"         }, // KEY_9
		{ '0',         "0"         }, // KEY_0

		{ 'A',         "A"         }, // KEY_A
		{ 'B',         "B"         }, // KEY_B
		{ 'C',         "C"         }, // KEY_C
		{ 'D',         "D"         }, // KEY_D
		{ 'E',         "E"         }, // KEY_E
		{ 'F',         "F"         }, // KEY_F
		{ 'G',         "G"         }, // KEY_G
		{ 'H',         "H"         }, // KEY_H
		{ 'I',         "I"         }, // KEY_I
		{ 'J',         "J"         }, // KEY_J
		{ 'K',         "K"         }, // KEY_K
		{ 'L',         "L"         }, // KEY_L
		{ 'M',         "M"         }, // KEY_M
		{ 'N',         "N"         }, // KEY_N
		{ 'O',         "O"         }, // KEY_O
		{ 'P',         "P"         }, // KEY_P
		{ 'Q',         "Q"         }, // KEY_Q
		{ 'R',         "R"         }, // KEY_R
		{ 'S',         "S"         }, // KEY_S
		{ 'T',         "T"         }, // KEY_T
		{ 'U',         "U"         }, // KEY_U
		{ 'V',         "V"         }, // KEY_V
		{ 'W',         "W"         }, // KEY_W
		{ 'X',         "X"         }, // KEY_X
		{ 'Y',         "Y"         }, // KEY_Y
		{ 'Z',         "Z"         }, // KEY_Z

		{ VK_INSERT,   "Insert"    }, // KEY_INSERT
		{ VK_HOME,     "Home"      }, // KEY_HOME
		{ VK_PRIOR,    "PageUp"    }, // KEY_PAGE_UP, 
		{ VK_DELETE,   "Delete"    }, // KEY_DELETE
		{ VK_END,      "End"       }, // KEY_END
		{ VK_NEXT,     "PageDown"  }, // KEY_PAGE_DOWN,

		{ VK_LEFT,     "Left"      }, // KEY_LEFT
		{ VK_UP,       "Up"        }, // KEY_UP
		{ VK_RIGHT,    "Right"     }, // KEY_RIGHT
		{ VK_DOWN,     "Down"      }, // KEY_DOWN

		{ VK_NUMLOCK,  "NumLock"   }, // KEY_NUM_LOCK

		{ VK_DIVIDE,   "Num/"      }, // KEY_NUM_DIVIDE
		{ VK_MULTIPLY, "Num*"      }, // KEY_NUM_MULTIPLY, 
		{ VK_SUBTRACT, "Num-"      }, // KEY_NUM_SUBTRACT 
		{ VK_ADD,      "Num+"      }, // KEY_NUM_ADD

		{ VK_NUMPAD1,  "Num1"      }, // KEY_NUM_1
		{ VK_NUMPAD2,  "Num2"      }, // KEY_NUM_2
		{ VK_NUMPAD3,  "Num3"      }, // KEY_NUM_3
		{ VK_NUMPAD4,  "Num4"      }, // KEY_NUM_4
		{ VK_NUMPAD5,  "Num5"      }, // KEY_NUM_5
		{ VK_NUMPAD6,  "Num6"      }, // KEY_NUM_6
		{ VK_NUMPAD7,  "Num7"      }, // KEY_NUM_7
		{ VK_NUMPAD8,  "Num8"      }, // KEY_NUM_8
		{ VK_NUMPAD9,  "Num9"      }, // KEY_NUM_9
		{ VK_NUMPAD0,  "Num0"      }, // KEY_NUM_0
		{ VK_DECIMAL,  "Num."      }, // KEY_NUM_DECIMAL
		{ VK_CLEAR,    "NumClear"  }, // KEY_NUM_CLEAR

		{ '\0',        "Invalid"   }
	};

	WindowsKeyboard::WindowsKeyboard() : InputDevice("Keyabord"), KEY_DOWN_MASK((u16)0x8000U)
	{
		SetName("Keyboard");
		AddKey(VK_ESCAPE,   "Esc"       ); // KEY_ESCAPE
		AddKey(VK_F1,       "F1"        ); // KEY_F1
		AddKey(VK_F2,       "F2"        ); // KEY_F2
		AddKey(VK_F3,       "F3"        ); // KEY_F3
		AddKey(VK_F4,       "F4"        ); // KEY_F4
		AddKey(VK_F5,       "F5"        ); // KEY_F5
		AddKey(VK_F6,       "F6"        ); // KEY_F6
		AddKey(VK_F7,       "F7"        ); // KEY_F7
		AddKey(VK_F8,       "F8"        ); // KEY_F8 
		AddKey(VK_F9,       "F9"        ); // KEY_F9
		AddKey(VK_F10,      "F10"       ); // KEY_F10
		AddKey(VK_F11,      "F11"       ); // KEY_F11
		AddKey(VK_F12,      "F12"       ); // KEY_F12
		AddKey(VK_SNAPSHOT, "PrintScreen"); // KEY_PRINT_SCREEN, 
		AddKey(VK_SCROLL,   "Scroll"    ); // KEY_SCROLL
		AddKey(VK_OEM_3,    "`"         ); // KEY_APOSTROPHE
		AddKey(VK_TAB,      "Tab"       ); // KEY_TAB
		AddKey(VK_CAPITAL,  "CapsLock"  ); // KEY_CAPS
		AddKey(VK_LSHIFT,   "LShift"    ); // KEY_LSHIFT
		AddKey(VK_LCONTROL, "LCtrl"     ); // KEY_LCTRL
		AddKey(VK_LMENU,    "LAlt"      ); // KEY_LALT 
		AddKey(VK_SPACE,    "Space"     ); // KEY_SPACE 
		AddKey(VK_RSHIFT,   "RShift"    ); // KEY_RSHIFT
		AddKey(VK_RCONTROL, "RCtrl"     ); // KEY_RCTRL
		AddKey(VK_RMENU,    "RAlt"      ); // KEY_RALT, 
		AddKey(VK_OEM_MINUS,"-"         ); // KEY_SUBTRACT
		AddKey(VK_OEM_PLUS, "="         ); // KEY_EQUALS
		AddKey(VK_OEM_5,    "\\"        ); // KEY_BACKSLASH
		AddKey(VK_BACK,     "Backspace" ); // KEY_BACKSPACE
		AddKey(VK_RETURN,   "Enter"     ); // KEY_ENTER
		AddKey(VK_OEM_4,    "["         ); // KEY_LSQUARE_BRACKET
		AddKey(VK_OEM_6,    "]"         ); // KEY_RSQUARE_BRACKET
		AddKey(VK_OEM_1,    ";"         ); // KEY_SEMICOLON
		AddKey(VK_OEM_7,    "'"         ); // KEY_QUOTE
		AddKey(VK_OEM_COMMA,","         ); // KEY_COMMA
		AddKey(VK_OEM_PERIOD,"."        ); // KEY_FULLSTOP
		AddKey(VK_OEM_2,    "/"         ); // KEY_FORWARD_SLASH
		AddKey('1',         "1"         ); // KEY_1
		AddKey('2',         "2"         ); // KEY_2
		AddKey('3',         "3"         ); // KEY_3
		AddKey('4',         "4"         ); // KEY_4
		AddKey('5',         "5"         ); // KEY_5
		AddKey('6',         "6"         ); // KEY_6
		AddKey('7',         "7"         ); // KEY_7
		AddKey('8',         "8"         ); // KEY_8
		AddKey('9',         "9"         ); // KEY_9
		AddKey('0',         "0"         ); // KEY_0
		AddKey('A',         "A"         ); // KEY_A
		AddKey('B',         "B"         ); // KEY_B
		AddKey('C',         "C"         ); // KEY_C
		AddKey('D',         "D"         ); // KEY_D
		AddKey('E',         "E"         ); // KEY_E
		AddKey('F',         "F"         ); // KEY_F
		AddKey('G',         "G"         ); // KEY_G
		AddKey('H',         "H"         ); // KEY_H
		AddKey('I',         "I"         ); // KEY_I
		AddKey('J',         "J"         ); // KEY_J
		AddKey('K',         "K"         ); // KEY_K
		AddKey('L',         "L"         ); // KEY_L
		AddKey('M',         "M"         ); // KEY_M
		AddKey('N',         "N"         ); // KEY_N
		AddKey('O',         "O"         ); // KEY_O
		AddKey('P',         "P"         ); // KEY_P
		AddKey('Q',         "Q"         ); // KEY_Q
		AddKey('R',         "R"         ); // KEY_R
		AddKey('S',         "S"         ); // KEY_S
		AddKey('T',         "T"         ); // KEY_T
		AddKey('U',         "U"         ); // KEY_U
		AddKey('V',         "V"         ); // KEY_V
		AddKey('W',         "W"         ); // KEY_W
		AddKey('X',         "X"         ); // KEY_X
		AddKey('Y',         "Y"         ); // KEY_Y
		AddKey('Z',         "Z"         ); // KEY_Z
		AddKey(VK_INSERT,   "Insert"    ); // KEY_INSERT
		AddKey(VK_HOME,     "Home"      ); // KEY_HOME
		AddKey(VK_PRIOR,    "PageUp"    ); // KEY_PAGE_UP, 
		AddKey(VK_DELETE,   "Delete"    ); // KEY_DELETE
		AddKey(VK_END,      "End"       ); // KEY_END
		AddKey(VK_NEXT,     "PageDown"  ); // KEY_PAGE_DOWN,
		AddKey(VK_LEFT,     "Left"      ); // KEY_LEFT
		AddKey(VK_UP,       "Up"        ); // KEY_UP
		AddKey(VK_RIGHT,    "Right"     ); // KEY_RIGHT
		AddKey(VK_DOWN,     "Down"      ); // KEY_DOWN
		AddKey(VK_NUMLOCK,  "NumLock"   ); // KEY_NUM_LOCK
		AddKey(VK_DIVIDE,   "Num/"      ); // KEY_NUM_DIVIDE
		AddKey(VK_MULTIPLY, "Num*"      ); // KEY_NUM_MULTIPLY, 
		AddKey(VK_SUBTRACT, "Num-"      ); // KEY_NUM_SUBTRACT 
		AddKey(VK_ADD,      "Num+"      ); // KEY_NUM_ADD
		AddKey(VK_NUMPAD1,  "Num1"      ); // KEY_NUM_1
		AddKey(VK_NUMPAD2,  "Num2"      ); // KEY_NUM_2
		AddKey(VK_NUMPAD3,  "Num3"      ); // KEY_NUM_3
		AddKey(VK_NUMPAD4,  "Num4"      ); // KEY_NUM_4
		AddKey(VK_NUMPAD5,  "Num5"      ); // KEY_NUM_5
		AddKey(VK_NUMPAD6,  "Num6"      ); // KEY_NUM_6
		AddKey(VK_NUMPAD7,  "Num7"      ); // KEY_NUM_7
		AddKey(VK_NUMPAD8,  "Num8"      ); // KEY_NUM_8
		AddKey(VK_NUMPAD9,  "Num9"      ); // KEY_NUM_9
		AddKey(VK_NUMPAD0,  "Num0"      ); // KEY_NUM_0
		AddKey(VK_DECIMAL,  "Num."      ); // KEY_NUM_DECIMAL
		AddKey(VK_CLEAR,    "NumClear"  ); // KEY_NUM_CLEAR
		//AddDataInput("KeyData",&mKeyDataInput);
		//AddDataInput("KeyStrokeData",&mKeyStrokeDataInput);
	}

	WindowsKeyboard::~WindowsKeyboard()
	{
	}

	void WindowsKeyboard::AddKey(u16 code, const std::string& keyName)
	{
		KeyImp* key=new KeyImp(code,keyName);
		*key->mDown = ((GetAsyncKeyState(code) & KEY_DOWN_MASK) != 0);
		mKeys.push_back(key);
		AddInput<bool>(keyName,key->mDown,false);
	}

	void WindowsKeyboard::Update(Seconds lastFrameTime)
	{
		//mKeyData.clear();
		//mKeyStrokeData.clear();
		for (int i = 0; i < Keys::KEY_COUNT; ++i)
		{
			KeyImp& key=*mKeys[i];
			bool keyCurrentlyDown = ((GetAsyncKeyState(key.mCode) & KEY_DOWN_MASK) != 0);
			//if(keyCurrentlyDown)
			//	mKeyData+=key.mValue.mHashString;
			if (keyCurrentlyDown != *key.mDown)
			{
				*key.mDown = keyCurrentlyDown;

				if (keyCurrentlyDown)
				{
					// key has just been pressed so set the hit flag
					//mKeyStrokeData+=key.mValue.mHashString;
					key.mHit = true;
					key.mLifted = false;
				}else
				{
					// key has just been lifted so set the lifted flag
					key.mLifted = true;
					*key.mDown = false;
				}
			}else
			{
				// key state is same as before so clear hit and lifted flags
				key.mHit = false;
				key.mLifted = false;
			}
		}
//		mKeyDataInput.mData=const_cast<u8*>(reinterpret_cast<const u8*>(mKeyData.c_str()));
//		mKeyDataInput.mDataSize=(u32)mKeyData.length();
//		mKeyStrokeDataInput.mData=const_cast<u8*>(reinterpret_cast<const u8*>(mKeyStrokeData.c_str()));
//		mKeyStrokeDataInput.mDataSize=(u32)mKeyStrokeData.length();
		InputDevice::Update(lastFrameTime);
	}
}
#endif
