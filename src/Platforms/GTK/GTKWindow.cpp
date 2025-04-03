#include <echo/Platforms/GTK/GTKWindow.h>
#include <echo/Maths/Length.h>
#include <iostream>
#include <gdkmm-2.4/gdkmm/event.h>
#include <gtkmm-2.4/gtkmm/widget.h>
#include <gdkmm-2.4/gdkmm/rectangle.h>

namespace Echo
{
	GTKWindow::GTKWindow(shared_ptr<GLContext> context, Kernel& kernel, const std::string& title, u32 width, u32 height,
						 u8 pixelBitDepth, bool fullscreenflag, shared_ptr<Gtk::Main> gtkMain) :
		GLRenderTarget(context),
		mKernel(kernel),
		mGTKMain(gtkMain)
	{
		assert(width>=0 && height>=0 && "Cannot create window with a width or height of 0.");
		mBytesPerPixel = pixelBitDepth / 8;
		mIsFullScreen = fullscreenflag;

		mName = title;
		mContextRef = 0;
		
		Gtk::GL::init(0, 0);

		add_events(Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::KEY_PRESS_MASK);
		set_flags(Gtk::CAN_FOCUS);
		set_title(title);

		if(fullscreenflag)
		{
			set_decorated(false);
			fullscreen();
		} else
		{
			set_default_size(width, height);
		}

		//Create the GL Drawing Area Widget
		mGLWidget = Gtk::manage(new Gtk::GL::DrawingArea());
		mGLWidget->set_size_request(width, height);

		Gdk::GL::ConfigMode configMode=Gdk::GL::MODE_RGBA | Gdk::GL::MODE_DOUBLE | Gdk::GL::MODE_DEPTH;
		Glib::RefPtr<Gdk::GL::Config> glconfig = Gdk::GL::Config::create(configMode);
		if(!glconfig)
		{
			ECHO_LOG_ERROR("GTK - GLCONFIG invalid");
		}else
		{
			set_flags(Gtk::CAN_FOCUS);
			mGLWidget->set_gl_capability(glconfig, GetGLContext(), true, Gdk::GL::RGBA_TYPE);
		}

		add(*mGLWidget);
		show_all();

		mMainContext = mGLWidget->get_gl_context();
		mMainWindow = mGLWidget->get_gl_window();
		Glib::RefPtr<Gdk::Screen> screen = mGLWidget->get_screen();
		if(screen)
		{
			Millimetre xMMPD(Scalar(screen->get_width_mm()) / Scalar(screen->get_width()));
			SetDisplayXDPI(1/Inch(xMMPD).Count());
			Millimetre yMMPD(Scalar(screen->get_height_mm()) / Scalar(screen->get_height()));
			SetDisplayYDPI(1/Inch(yMMPD).Count());

			ECHO_LOG_INFO("Screen width: " << screen->get_width() << " (" << screen->get_width_mm() << "mm " << GetDisplayXDPI() << "DPI)");
			ECHO_LOG_INFO("Screen height: " << screen->get_height() << " (" << screen->get_height_mm() << "mm " << GetDisplayYDPI() << "DPI)");
		}

		mContext->mContextReady = true;

		mMainWindow->gl_begin(mMainContext);
		DetermineCapabilities();
		mMainWindow->gl_end();

		mWidth = width;
		mHeight = height;
	}

	GTKWindow::~GTKWindow()
	{
		mMainWindow->gl_end();
	}

	void GTKWindow::SetSize(u32 width, u32 height)
	{
		if(mGLWidget)
		{
			mGLWidget->set_size_request(width, height);
		}
	}

	bool GTKWindow::Activate()
	{
		if(!mContext->mContextReady)
		{
			mContextRef = 0;
			return false;
		}
		++mContextRef;
		if(mContextRef == 1)
		{
			mMainWindow->gl_begin(mMainContext);
			glewInit();
		}
		GLRenderTarget::Activate();
		return true;
	}

	void GTKWindow::Deactivate()
	{
		GLRenderTarget::Deactivate();
		--mContextRef;
		if(mContextRef < 0)
		{
			ECHO_LOG_ERROR("EchoGTKWindow::EndContenxt() : no contexts to destroy");
		}
		if(mContextRef == 0)
		{
			mMainWindow->gl_end();
		}
	}

	void GTKWindow::SwapBuffers()
	{
		Glib::RefPtr<Gdk::GL::Window> glwindow = mGLWidget->get_gl_window();
		glwindow->swap_buffers();
	}

	bool GTKWindow::on_delete_event(GdkEventAny* event)
	{
		if(IsPrimary())	//This flag is false by default, the Application will set the appropriate window to be a primary window.
		{
			mKernel.RemoveAllTasks();
		}
		return false;
	}

	bool GTKWindow::on_expose_event(GdkEventExpose* event)
	{
		return false;
	}

	bool GTKWindow::on_focus_in_event(GdkEventFocus* event)
	{
		//This implementation supports disabling application pause/resume
		if(mKernel.GetPauseHint())
		{
			mKernel.ResumeKernel(true);
		}
		return false;
	}

	bool GTKWindow::on_focus_out_event(GdkEventFocus* event)
	{
		//This implementation supports disabling application pause/resume
		if(mKernel.GetPauseHint())
		{
			mKernel.PauseKernel(true);
		}
		return false;
	}

	void GTKWindow::AddListener(GTKWindowEventListener* listener)
	{
		mListeners.push_back(listener);
	}

	void GTKWindow::RemoveListener(GTKWindowEventListener* listener)
	{
		mListeners.remove(listener);
	}

	void GTKWindow::InternalMouseMoved(f32 newX, f32 newY)
	{
		std::list<GTKWindowEventListener*>::iterator it = mListeners.begin();
		std::list<GTKWindowEventListener*>::iterator itEnd = mListeners.end();
		while(it != itEnd)
		{
			(*it)->MouseMoved(newX, newY);
			++it;
		}
	}

	void GTKWindow::InternalLeftButtonStateChanged(f32 atX, f32 atY, bool state)
	{
		std::list<GTKWindowEventListener*>::iterator it = mListeners.begin();
		std::list<GTKWindowEventListener*>::iterator itEnd = mListeners.end();
		while(it != itEnd)
		{
			(*it)->LeftButtonStateChanged(atX, atY, state);
			++it;
		}
	}

	void GTKWindow::InternalRightButtonStateChanged(f32 atX, f32 atY, bool state)
	{
		std::list<GTKWindowEventListener*>::iterator it = mListeners.begin();
		std::list<GTKWindowEventListener*>::iterator itEnd = mListeners.end();
		while(it != itEnd)
		{
			(*it)->RightButtonStateChanged(atX, atY, state);
			++it;
		}
	}

	void GTKWindow::InternalMiddleButtonStateChanged(f32 atX, f32 atY, bool state)
	{
		std::list<GTKWindowEventListener*>::iterator it = mListeners.begin();
		std::list<GTKWindowEventListener*>::iterator itEnd = mListeners.end();
		while(it != itEnd)
		{
			(*it)->MiddleButtonStateChanged(atX, atY, state);
			++it;
		}
	}

	void GTKWindow::InternalKeyStateChanged(u32 key, bool state)
	{
		std::list<GTKWindowEventListener*>::iterator it = mListeners.begin();
		std::list<GTKWindowEventListener*>::iterator itEnd = mListeners.end();
		while(it != itEnd)
		{
			(*it)->KeyStateChanged(key, state);
			++it;
		}
	}
	
	void GTKWindow::SetName(const std::string& name)
	{
		mName = name;
		set_title(name);
	}
	
	std::string GTKWindow::GetName()
	{
		return mName;
	}
	
	void GTKWindow::on_size_request(Gtk::Requisition* requisition)
	{
		//We might want to block the size request in the future.
		Gtk::Widget::on_size_request(requisition);
	}

	void GTKWindow::on_size_allocate(Gtk::Allocation& allocation)
	{
		mWidth = allocation.get_width();
		mHeight = allocation.get_height();
		if(mContext->mScissorEnabled)
		{
			mContext->mScissorRectangle=Viewport::Rectangle(0,0,1,1);
		}
		Gtk::Widget::on_size_allocate(allocation);
	}


	bool GTKWindow::on_button_press_event(GdkEventButton* event)
	{
		//the button which was pressed or released, numbered from 1 to 5.
		//Normally button	1 is the left mouse button
		//					2 is the middle button
		//					3 is the right button

		if(event->button == 1)
			InternalLeftButtonStateChanged(event->x, event->y, true);

		if(event->button == 2)
			InternalMiddleButtonStateChanged(event->x, event->y, true);

		if(event->button == 3)
			InternalRightButtonStateChanged(event->x, event->y, true);

		return true;
	}

	bool GTKWindow::on_button_release_event(GdkEventButton* event)
	{
		if(event->button == 1)
			InternalLeftButtonStateChanged(event->x, event->y, false);

		if(event->button == 2)
			InternalMiddleButtonStateChanged(event->x, event->y, false);

		if(event->button == 3)
			InternalRightButtonStateChanged(event->x, event->y, false);

		return true;
	}

	bool GTKWindow::on_motion_notify_event(GdkEventMotion* event)
	{
		int x, y;
		GdkModifierType state;

		if(event->is_hint)
			gdk_window_get_pointer(event->window, &x, &y, &state);
		else
		{
			x = event->x;
			y = event->y;
		}
		InternalMouseMoved(x, y);
		return true;
	}

	bool GTKWindow::on_key_press_event(GdkEventKey* event)
	{
		InternalKeyStateChanged(event->keyval, true);
		return true;
	}

	bool GTKWindow::on_key_release_event(GdkEventKey *event)
	{
		InternalKeyStateChanged(event->keyval, false);
		return true;
	}	
}
