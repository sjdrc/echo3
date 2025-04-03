#include <echo/Platforms/GTK/GTKKeyboard.h>
#include <iostream>
#include <gdk/gdkkeysyms.h> 
namespace Echo
{
	GTKKeyboard::GTKKeyboard(shared_ptr<GTKWindow> window) : InputDevice("Keyboard"), mWindow(window)
	{
		mWindow->AddListener(this);
		AddKey(GDK_KEY_Tab, "Tab");
		//AddKey(GDK_KEY_Linefeed 0xff0a
		//AddKey(GDK_KEY_Return 0xff0d
		AddKey(GDK_KEY_Pause, "Pause");
		AddKey(GDK_KEY_Scroll_Lock, "Scroll Lock");
		//AddKey(GDK_KEY_Sys_Req 0xff15
		AddKey(GDK_KEY_Escape, "Esc");
		AddKey(GDK_KEY_Delete, "Delete");
		AddKey(GDK_KEY_Home, "Home");
		AddKey(GDK_KEY_Left, "Left");
		AddKey(GDK_KEY_Up, "Up");
		AddKey(GDK_KEY_Right, "Right");
		AddKey(GDK_KEY_Down, "Down");
		AddKey(GDK_KEY_Page_Up, "Page Up");
		AddKey(GDK_KEY_Page_Down, "Page Down");
		AddKey(GDK_KEY_End, "End");
		AddKey(GDK_KEY_Insert, "Insert");
		AddKey(GDK_KEY_Break, "Break");
		AddKey(GDK_KEY_Num_Lock, "Num Lock");
		AddKey(GDK_KEY_F1, "F1");
		AddKey(GDK_KEY_F2, "F2");
		AddKey(GDK_KEY_F3, "F3");
		AddKey(GDK_KEY_F4, "F4");
		AddKey(GDK_KEY_F5, "F5");
		AddKey(GDK_KEY_F6, "F6");
		AddKey(GDK_KEY_F7, "F7");
		AddKey(GDK_KEY_F8, "F8");
		AddKey(GDK_KEY_F9, "F9");
		AddKey(GDK_KEY_F10, "F10");
		AddKey(GDK_KEY_F11, "F11");
		AddKey(GDK_KEY_F12, "F12");
		AddKey(GDK_KEY_Shift_L, "Left Shift");
		AddKey(GDK_KEY_Shift_R, "Right Shift");
		AddKey(GDK_KEY_Control_L, "Left Control");
		AddKey(GDK_KEY_Control_R, "Right Control");
		AddKey(GDK_KEY_Caps_Lock, "Caps Lock");
		AddKey(GDK_KEY_Meta_L, "Left Meta");
		AddKey(GDK_KEY_Meta_R, "Right Meta");
		AddKey(GDK_KEY_Alt_L, "Left Alt");
		AddKey(GDK_KEY_Alt_R, "Right Alt");
		//	AddKey(GDK_KEY_Super_L 0xffeb
		//	AddKey(GDK_KEY_Super_R 0xffec
		//	AddKey(GDK_KEY_Hyper_L 0xffed
		//	AddKey(GDK_KEY_Hyper_R 0xffee
		AddKey(GDK_KEY_ISO_Enter, "Enter");
		AddKey(GDK_KEY_space, "Space");
		AddKey(GDK_KEY_exclam, "!");
		AddKey(GDK_KEY_quotedbl, "\"");
		AddKey(GDK_KEY_numbersign, "#");
		AddKey(GDK_KEY_dollar, "$");
		AddKey(GDK_KEY_percent, "%");
		AddKey(GDK_KEY_ampersand, "&");
		AddKey(GDK_KEY_apostrophe, "'");
		//AddKey(GDK_KEY_quoteright, "`");
		AddKey(GDK_KEY_parenleft, "P(");
		AddKey(GDK_KEY_parenright, "P)");
		AddKey(GDK_KEY_asterisk, "*");
		AddKey(GDK_KEY_plus, "+");
		AddKey(GDK_KEY_comma, ",");
		AddKey(GDK_KEY_minus, "-");
		AddKey(GDK_KEY_period, ".");
		AddKey(GDK_KEY_slash, "/");
		AddKey(GDK_KEY_0, "0");
		AddKey(GDK_KEY_1, "1");
		AddKey(GDK_KEY_2, "2");
		AddKey(GDK_KEY_3, "3");
		AddKey(GDK_KEY_4, "4");
		AddKey(GDK_KEY_5, "5");
		AddKey(GDK_KEY_6, "6");
		AddKey(GDK_KEY_7, "7");
		AddKey(GDK_KEY_8, "8");
		AddKey(GDK_KEY_9, "9");
		AddKey(GDK_KEY_colon, ":");
		AddKey(GDK_KEY_semicolon, ";");
		AddKey(GDK_KEY_less, "<");
		AddKey(GDK_KEY_equal, "=");
		AddKey(GDK_KEY_greater, ">");
		AddKey(GDK_KEY_question, "?");
		AddKey(GDK_KEY_at, "@");
		AddKey(GDK_KEY_A, "A");
		AddKey(GDK_KEY_B, "B");
		AddKey(GDK_KEY_C, "C");
		AddKey(GDK_KEY_D, "D");
		AddKey(GDK_KEY_E, "E");
		AddKey(GDK_KEY_F, "F");
		AddKey(GDK_KEY_G, "G");
		AddKey(GDK_KEY_H, "H");
		AddKey(GDK_KEY_I, "I");
		AddKey(GDK_KEY_J, "J");
		AddKey(GDK_KEY_K, "K");
		AddKey(GDK_KEY_L, "L");
		AddKey(GDK_KEY_M, "M");
		AddKey(GDK_KEY_N, "N");
		AddKey(GDK_KEY_O, "O");
		AddKey(GDK_KEY_P, "P");
		AddKey(GDK_KEY_Q, "Q");
		AddKey(GDK_KEY_R, "R");
		AddKey(GDK_KEY_S, "S");
		AddKey(GDK_KEY_T, "T");
		AddKey(GDK_KEY_U, "U");
		AddKey(GDK_KEY_V, "V");
		AddKey(GDK_KEY_W, "W");
		AddKey(GDK_KEY_X, "X");
		AddKey(GDK_KEY_Y, "Y");
		AddKey(GDK_KEY_Z, "Z");
		AddKey(GDK_KEY_bracketleft, "[");
		AddKey(GDK_KEY_backslash, "\\");
		AddKey(GDK_KEY_bracketright, "]");
		//AddKey(GDK_KEY_asciicircum,"/");
		AddKey(GDK_KEY_underscore, "_");
		//AddKey(GDK_KEY_grave,"/");
		//AddKey(GDK_KEY_quoteleft,"");
		AddKey(GDK_KEY_a, "a");
		AddKey(GDK_KEY_b, "b");
		AddKey(GDK_KEY_c, "c");
		AddKey(GDK_KEY_d, "d");
		AddKey(GDK_KEY_e, "e");
		AddKey(GDK_KEY_f, "f");

		AddKey(GDK_KEY_g, "g");
		AddKey(GDK_KEY_h, "h");
		AddKey(GDK_KEY_i, "i");
		AddKey(GDK_KEY_j, "j");
		AddKey(GDK_KEY_k, "k");
		AddKey(GDK_KEY_l, "l");
		AddKey(GDK_KEY_m, "m");
		AddKey(GDK_KEY_n, "n");
		AddKey(GDK_KEY_o, "o");
		AddKey(GDK_KEY_p, "p");
		AddKey(GDK_KEY_q, "q");
		AddKey(GDK_KEY_r, "r");
		AddKey(GDK_KEY_s, "s");
		AddKey(GDK_KEY_t, "t");
		AddKey(GDK_KEY_u, "u");
		AddKey(GDK_KEY_v, "v");
		AddKey(GDK_KEY_w, "w");
		AddKey(GDK_KEY_x, "x");
		AddKey(GDK_KEY_y, "y");
		AddKey(GDK_KEY_z, "z");
		AddKey(GDK_KEY_braceleft, "{");
		AddKey(GDK_KEY_bar, "|");
		AddKey(GDK_KEY_braceright, "}");
		AddKey(GDK_KEY_asciitilde, "~");
		AddKey(GDK_KEY_BackSpace, "Backspace");
	}

	GTKKeyboard::~GTKKeyboard()
	{
		mWindow->RemoveListener(this);
	}

	void GTKKeyboard::AddKey(u32 code, const std::string& keyName)
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

	void GTKKeyboard::MouseMoved(f32, f32)
	{
	}

	void GTKKeyboard::LeftButtonStateChanged(f32, f32, bool)
	{
	}

	void GTKKeyboard::RightButtonStateChanged(f32, f32, bool)
	{
	}

	void GTKKeyboard::MiddleButtonStateChanged(f32, f32, bool)
	{
	}

	void GTKKeyboard::KeyStateChanged(u32 key, bool state)
	{
		std::map< u32, Key >::iterator it = mKeys.find(key);
		if(it != mKeys.end())
		{
			*it->second.mDown = state;
		}
	}
}
