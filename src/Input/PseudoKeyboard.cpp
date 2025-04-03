#include <echo/Input/PseudoKeyboard.h>
#include <boost/make_shared.hpp>

namespace Echo
{
	PseudoKeyboard::PseudoKeyboard(const std::string& name) : InputDevice("PseudoKeyboard")
	{
		AddKey("Left");
		AddKey("Right");
		AddKey("Up");
		AddKey("Down");
		AddKey("LCtrl");
		AddKey("LAlt");
		AddKey("Space");
		AddKey("LShift");
		AddKey("Backspace");
		AddKey("Tab");
		AddKey("Enter");
		AddKey("Pause");
		AddKey("Scroll Lock");
		AddKey("Esc");
		AddKey("Delete");
		AddKey("Home");
		AddKey("Left");
		AddKey("Up");
		AddKey("Right");
		AddKey("Down");
		AddKey("Page Up");
		AddKey("Page Down");
		AddKey("End");
		AddKey("Insert");
		AddKey("Break");
		AddKey("Num Lock");
		AddKey("F1");
		AddKey("F2");
		AddKey("F3");
		AddKey("F4");
		AddKey("F5");
		AddKey("F6");
		AddKey("F7");
		AddKey("F8");
		AddKey("F9");
		AddKey("F10");
		AddKey("F11");
		AddKey("F12");
		AddKey("Left Shift");
		AddKey("Right Shift");
		AddKey("Left Control");
		AddKey("Right Control");
		AddKey("Caps Lock");
		AddKey("Left Meta");
		AddKey("Right Meta");
		AddKey("Left Alt");
		AddKey("Right Alt");
		AddKey("Enter");
		AddKey("Space");
		AddKey("!");
		AddKey("\"");
		AddKey("#");
		AddKey("$");
		AddKey("%");
		AddKey("&");
		AddKey("'");
		AddKey("`");
		AddKey("(");
		AddKey(")");
		AddKey("*");
		AddKey("+");
		AddKey(",");
		AddKey("-");
		AddKey(".");
		AddKey("/");
		AddKey("0");
		AddKey("1");
		AddKey("2");
		AddKey("3");
		AddKey("4");
		AddKey("5");
		AddKey("6");
		AddKey("7");
		AddKey("8");
		AddKey("9");
		AddKey(":");
		AddKey(";");
		AddKey("<");
		AddKey("=");
		AddKey(">");
		AddKey("?");
		AddKey("@");
		AddKey("A");
		AddKey("B");
		AddKey("C");
		AddKey("D");
		AddKey("E");
		AddKey("F");
		AddKey("G");
		AddKey("H");
		AddKey("I");
		AddKey("J");
		AddKey("K");
		AddKey("L");
		AddKey("M");
		AddKey("N");
		AddKey("O");
		AddKey("P");
		AddKey("Q");
		AddKey("R");
		AddKey("S");
		AddKey("T");
		AddKey("U");
		AddKey("V");
		AddKey("W");
		AddKey("X");
		AddKey("Y");
		AddKey("Z");
		AddKey("[");
		AddKey("\\");
		AddKey("]");
		AddKey("_");
		AddKey("a");
		AddKey("b");
		AddKey("c");
		AddKey("d");
		AddKey("e");
		AddKey("f");

		AddKey("g");
		AddKey("h");
		AddKey("i");
		AddKey("j");
		AddKey("k");
		AddKey("l");
		AddKey("m");
		AddKey("n");
		AddKey("o");
		AddKey("p");
		AddKey("q");
		AddKey("r");
		AddKey("s");
		AddKey("t");
		AddKey("u");
		AddKey("v");
		AddKey("w");
		AddKey("x");
		AddKey("y");
		AddKey("z");
		AddKey("{");
		AddKey("|");
		AddKey("}");
		AddKey("~");
		AddKey("Backspace");
	}

	PseudoKeyboard::~PseudoKeyboard()
	{
	}

	void PseudoKeyboard::AddKey(const std::string& key, bool initialState)
	{
		std::pair<KeyIterator, bool > insertResult = mKeys.insert(KeyPair(key,make_shared<bool>(initialState)));
		if(insertResult.second)
		{
			AddInput<bool>(key,insertResult.first->second,false);
		}else
		{
			ECHO_LOG_DEBUG("PseudoKeyboard - Could not add key: " << key);
		}
	}

	void PseudoKeyboard::SetKey(const std::string& key, bool state)
	{
		KeyIterator it = mKeys.find(key);
		if(it!=mKeys.end())
		{
			*it->second = state;
		}else
		{
			AddKey(key,state);
		}
	}
}
