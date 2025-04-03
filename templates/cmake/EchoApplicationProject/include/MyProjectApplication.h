#ifndef _MyProjectApplication_H_
#define _MyProjectApplication_H_

#include <echo/Application.h>
#include <echo/Graphics/Scene.h>

// For simplicity we'll specify that we are using the Echo namespace, however you should
// only do this if you're confident that names and types within the Echo namespace will
// not conflict with other namespaces. It is recommended that you either use
// 'using namespace Echo' in limited scopes (like specific functions), or even better
// define what you want to use from that scope. For example:
// 
//	void foo()
//	{
//		using Echo::Vector3;
//		Vector3 thing;
//	}
//
// Or optionally use the full name with scope if you're using the type once.
//
//	void foo()
//	{
//		Echo::Vector3 thing;
//	}
//
using namespace Echo;

namespace MyProject
{
	class MyProjectApplication : public Application
	{
	public:
		MyProjectApplication();
		~MyProjectApplication();

		bool Initialise();
		Scene mScene;
	};
}

#endif
