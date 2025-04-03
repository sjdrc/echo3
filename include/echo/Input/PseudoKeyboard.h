#ifndef _ECHOPSEUDOKEYBOARD_H_
#define _ECHOPSEUDOKEYBOARD_H_
#include <echo/Input/InputDevice.h>

namespace Echo
{
	class PseudoKeyboard : public InputDevice
	{
	public:
		PseudoKeyboard(const std::string& name);
		~PseudoKeyboard();
		
		void AddKey(const std::string& key, bool initialState = false);
		void SetKey(const std::string& key, bool state);
	private:
		typedef std::pair<const std::string, shared_ptr<bool> > KeyPair;
		typedef std::map< std::string, shared_ptr<bool> >::iterator KeyIterator;
		std::map< std::string, shared_ptr<bool> > mKeys;
	};
}
#endif
