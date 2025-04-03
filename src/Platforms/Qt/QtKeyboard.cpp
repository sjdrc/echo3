#include <echo/Platforms/Qt/QtKeyboard.h>
#include <iostream>
#include <Qt>

namespace Echo
{
	QtKeyboard::QtKeyboard(shared_ptr<QtWindow> window) : InputDevice("Keyboard"), mWindow(window)
	{
		mWindow->AddListener(this);
		AddKey(Qt::Key_Tab, "Tab");
		//AddKey(Qt::Key_Linefeed 0xff0a
		//AddKey(Qt::Key_Return 0xff0d
		AddKey(Qt::Key_Pause, "Pause");
		AddKey(Qt::Key_ScrollLock, "Scroll Lock");
		//AddKey(Qt::Key_Sys_Req 0xff15
		AddKey(Qt::Key_Escape, "Esc");
		AddKey(Qt::Key_Delete, "Delete");
		AddKey(Qt::Key_Home, "Home");
		AddKey(Qt::Key_Left, "Left");
		AddKey(Qt::Key_Up, "Up");
		AddKey(Qt::Key_Right, "Right");
		AddKey(Qt::Key_Down, "Down");
		AddKey(Qt::Key_PageUp, "Page Up");
		AddKey(Qt::Key_PageDown, "Page Down");
		AddKey(Qt::Key_End, "End");
		AddKey(Qt::Key_Insert, "Insert");
		AddKey(Qt::Key_Pause, "Break");
		AddKey(Qt::Key_NumLock, "Num Lock");
		AddKey(Qt::Key_F1, "F1");
		AddKey(Qt::Key_F2, "F2");
		AddKey(Qt::Key_F3, "F3");
		AddKey(Qt::Key_F4, "F4");
		AddKey(Qt::Key_F5, "F5");
		AddKey(Qt::Key_F6, "F6");
		AddKey(Qt::Key_F7, "F7");
		AddKey(Qt::Key_F8, "F8");
		AddKey(Qt::Key_F9, "F9");
		AddKey(Qt::Key_F10, "F10");
		AddKey(Qt::Key_F11, "F11");
		AddKey(Qt::Key_F12, "F12");
		AddKey(Qt::Key_Shift, "Left Shift");
		//AddKey(Qt::Key_Shift, "Right Shift");
		AddKey(Qt::Key_Control, "Control");
		AddKey(Qt::Key_Control, "Left Control");
		//AddKey(Qt::Key_Control, "Right Control");
		AddKey(Qt::Key_CapsLock, "Caps Lock");
		AddKey(Qt::Key_Meta, "Meta");
		AddKey(Qt::Key_Meta, "Left Meta");
		//AddKey(Qt::Key_Meta, "Right Meta");
		AddKey(Qt::Key_Alt, "Alt");
		AddKey(Qt::Key_Alt, "Left Alt");
		//AddKey(Qt::Key_Alt, "Right Alt");
		//	AddKey(Qt::Key_Super_L 0xffeb
		//	AddKey(Qt::Key_Super_R 0xffec
		//	AddKey(Qt::Key_Hyper_L 0xffed
		//	AddKey(Qt::Key_Hyper_R 0xffee
		AddKey(Qt::Key_Enter, "Enter");
		AddKey(Qt::Key_Space, "Space");
		AddKey(Qt::Key_Exclam, "!");
		AddKey(Qt::Key_QuoteDbl, "\"");
		AddKey(Qt::Key_NumberSign, "#");
		AddKey(Qt::Key_Dollar, "$");
		AddKey(Qt::Key_Percent, "%");
		AddKey(Qt::Key_Ampersand, "&");
		AddKey(Qt::Key_Apostrophe, "'");
		//AddKey(Qt::Key_quoteright, "`");
		AddKey(Qt::Key_ParenLeft, "P(");
		AddKey(Qt::Key_ParenRight, "P)");
		AddKey(Qt::Key_Asterisk, "*");
		AddKey(Qt::Key_Plus, "+");
		AddKey(Qt::Key_Comma, ",");
		AddKey(Qt::Key_Minus, "-");
		AddKey(Qt::Key_Period, ".");
		AddKey(Qt::Key_Slash, "/");
		AddKey(Qt::Key_0, "0");
		AddKey(Qt::Key_1, "1");
		AddKey(Qt::Key_2, "2");
		AddKey(Qt::Key_3, "3");
		AddKey(Qt::Key_4, "4");
		AddKey(Qt::Key_5, "5");
		AddKey(Qt::Key_6, "6");
		AddKey(Qt::Key_7, "7");
		AddKey(Qt::Key_8, "8");
		AddKey(Qt::Key_9, "9");
		AddKey(Qt::Key_Colon, ":");
		AddKey(Qt::Key_Semicolon, ";");
		AddKey(Qt::Key_Less, "<");
		AddKey(Qt::Key_Equal, "=");
		AddKey(Qt::Key_Greater, ">");
		AddKey(Qt::Key_Question, "?");
		AddKey(Qt::Key_At, "@");
		AddKey(Qt::Key_A, "A");
		AddKey(Qt::Key_B, "B");
		AddKey(Qt::Key_C, "C");
		AddKey(Qt::Key_D, "D");
		AddKey(Qt::Key_E, "E");
		AddKey(Qt::Key_F, "F");
		AddKey(Qt::Key_G, "G");
		AddKey(Qt::Key_H, "H");
		AddKey(Qt::Key_I, "I");
		AddKey(Qt::Key_J, "J");
		AddKey(Qt::Key_K, "K");
		AddKey(Qt::Key_L, "L");
		AddKey(Qt::Key_M, "M");
		AddKey(Qt::Key_N, "N");
		AddKey(Qt::Key_O, "O");
		AddKey(Qt::Key_P, "P");
		AddKey(Qt::Key_Q, "Q");
		AddKey(Qt::Key_R, "R");
		AddKey(Qt::Key_S, "S");
		AddKey(Qt::Key_T, "T");
		AddKey(Qt::Key_U, "U");
		AddKey(Qt::Key_V, "V");
		AddKey(Qt::Key_W, "W");
		AddKey(Qt::Key_X, "X");
		AddKey(Qt::Key_Y, "Y");
		AddKey(Qt::Key_Z, "Z");
		AddKey(Qt::Key_BracketLeft, "[");
		AddKey(Qt::Key_Backslash, "\\");
		AddKey(Qt::Key_BracketRight, "]");
		//AddKey(Qt::Key_asciicircum,"/");
		AddKey(Qt::Key_Underscore, "_");
		//AddKey(Qt::Key_grave,"/");
		//AddKey(Qt::Key_QuoteLeft,"");
		AddKey(Qt::Key_BraceLeft, "{");
		AddKey(Qt::Key_Bar, "|");
		AddKey(Qt::Key_BraceRight, "}");
		AddKey(Qt::Key_AsciiTilde, "~");
		AddKey(Qt::Key_Backspace, "Backspace");
	}

	QtKeyboard::~QtKeyboard()
	{
		mWindow->RemoveListener(this);
	}

	void QtKeyboard::AddKey(u32 code, const std::string& keyName)
	{
		Key key;
		key.mDown = make_shared<bool>(false);
		key.mValue = keyName;
		std::pair < std::map< u32, Key >::iterator, bool> insertResult = mKeys.insert(std::make_pair(code, key));
		if(insertResult.second)
		{
			AddInput<bool>(keyName, insertResult.first->second.mDown, false);
		} else
		{
			ECHO_LOG_ERROR("Error could not add key " << keyName << "\"");
		}
	}

	void QtKeyboard::MouseMoved(f32, f32)
	{
	}

	void QtKeyboard::LeftButtonStateChanged(f32, f32, bool)
	{
	}

	void QtKeyboard::RightButtonStateChanged(f32, f32, bool)
	{
	}

	void QtKeyboard::MiddleButtonStateChanged(f32, f32, bool)
	{
	}

	void QtKeyboard::KeyStateChanged(u32 key, bool state)
	{
		std::map< u32, Key >::iterator it = mKeys.find(key);
		if(it != mKeys.end())
		{
			*it->second.mDown = state;
		}
	}
}
