#ifndef _MyProjectApplication_H_
#define _MyProjectApplication_H_

#include <echo/Application.h>
#include <echo/Util/ContextSwitcher.h>

using namespace Echo;

namespace MyProject
{
	class MyProjectApplication : public Application
	{
	public:
		MyProjectApplication();
		~MyProjectApplication();

		bool Initialise();
	private:
		ContextSwitcher& GetContextSwitcher() { return mContextSwitcher; }
		ContextSwitcher mContextSwitcher;		/// Application context switcher
	};
}

#endif
