#include <echo/Platform.h>
#include <echo/Input/Input.h>
#include <echo/Input/PseudoMouse.h>
#include <echo/Input/PseudoKeyboard.h>

namespace Echo
{
	namespace Platform
	{
		shared_ptr<InputManager> CreateDefaultInputManager(shared_ptr<RenderTarget> target)
		{
			shared_ptr<InputManager> inputManager = make_shared<InputManager>();
			inputManager->InstallDevice("Mouse",make_shared<PseudoMouse>("Mouse"));
			inputManager->InstallDevice("Keyboard",make_shared<PseudoKeyboard>("Keyboard"));
			return inputManager;
		}
	}
}
