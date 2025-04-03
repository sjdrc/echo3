#include <echo/Platforms/Android/AndroidWindow.h>
#include <iostream>

namespace Echo
{
	AndroidWindow::AndroidWindow(const std::string& title, u32 width, u32 height, u8 bits, bool fullscreenflag) : GLRenderTarget(make_shared<GLContext>()), mName(title)
	{
		assert(width>=0 && height>=0 && "Cannot create window with a width or height of 0.");
		mIsFullScreen = true;
		mContext->mContextReady = true;

		DetermineCapabilities();

		mWidth = width;
		mHeight = height;
	}

	AndroidWindow::~AndroidWindow()
	{

	}
	

	bool AndroidWindow::Activate()
	{
		return GLRenderTarget::Activate();
	}

	void AndroidWindow::Deactivate()
	{
		GLRenderTarget::Deactivate();
	}
	
	void AndroidWindow::SetSize(u32 width, u32 height)
	{
		mWidth = width;
		mHeight = height;
	}

	void AndroidWindow::SetName(const std::string& name)
	{
		mName = name;
	}
	
	std::string AndroidWindow::GetName()
	{
		return mName;
	}

}
