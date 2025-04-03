#ifndef _ECHOANDROIDWINDOW_H_
#define _ECHOANDROIDWINDOW_H_

#include <echo/Platforms/GL/GLRenderTarget.h>

namespace Echo
{
	/**
	 * Class to represent the GL rendering window on Android devices.
	 * This object is mainly managed externally by other code.
	 */
	class AndroidWindow : public GLRenderTarget
	{
	public:
		AndroidWindow(const std::string& title, u32 width, u32 height, u8 bits, bool fullscreenflag);
		~AndroidWindow();

		bool Activate();
		void Deactivate();
		
		void SetSize(u32 width, u32 height);

		void SetName(const std::string& name);
		std::string GetName();
	private:
		std::string mName;
	};
}
#endif
