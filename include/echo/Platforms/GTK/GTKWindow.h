#ifndef _ECHOGTKWINDOW_H
#define _ECHOGTKWINDOW_H

#include <echo/Platforms/GL/GLRenderTarget.h>
#include <gtkmm/main.h>
#include <gtkmm/window.h>
#include <gtkglmm.h>

#include <echo/Types.h>

namespace Echo
{
	class GTKWindowEventListener
	{
	public:
		GTKWindowEventListener(){}
		virtual ~GTKWindowEventListener(){}
		virtual void MouseMoved(f32 newX, f32 newY) = 0;
		virtual void LeftButtonStateChanged(f32 atX, f32 atY, bool state) = 0;
		virtual void RightButtonStateChanged(f32 atX, f32 atY, bool state) = 0;
		virtual void MiddleButtonStateChanged(f32 atX, f32 atY, bool state) = 0;
		virtual void KeyStateChanged(u32 key, bool state) = 0;
	};

	class GTKWindow : public GLRenderTarget, public Gtk::Window
	{
	public:
		GTKWindow(shared_ptr<GLContext> context, Kernel& kernel, const std::string& title, u32 width,
				 u32 height, u8 pixelBitDepth, bool fullscreenflag, shared_ptr<Gtk::Main> gtkMain);
		~GTKWindow();
		Glib::RefPtr<Gdk::GL::Context> GetGLContext()
		{
			return mMainContext;
		}

		void InternalMouseMoved(f32 newX, f32 newY);
		void InternalLeftButtonStateChanged(f32 atX, f32 atY, bool state);
		void InternalRightButtonStateChanged(f32 atX, f32 atY, bool state);
		void InternalMiddleButtonStateChanged(f32 atX, f32 atY, bool state);
		void InternalKeyStateChanged(u32 key, bool state);

		void SetSize(u32 width, u32 height) override;

		void AddListener(GTKWindowEventListener* listener);
		void RemoveListener(GTKWindowEventListener* listener);
		bool Activate() override;
		void Deactivate() override;
		void SwapBuffers() override;
		
		void SetName(const std::string& name) override;
		std::string GetName() override;
	private:
		//Events
		bool on_delete_event(GdkEventAny* event) override;
		bool on_expose_event(GdkEventExpose* event) override;
		bool on_focus_in_event(GdkEventFocus* event) override;
		bool on_focus_out_event(GdkEventFocus* event) override;
		void on_size_request(Gtk::Requisition* requisition) override;
		void on_size_allocate(Gtk::Allocation& allocation) override;
		bool on_button_press_event(GdkEventButton* event) override;
		bool on_button_release_event(GdkEventButton* event) override;
		bool on_motion_notify_event(GdkEventMotion* event) override;
		bool on_key_press_event(GdkEventKey* event) override;
		bool on_key_release_event(GdkEventKey *event) override;
		
		Kernel& mKernel;
		std::list<GTKWindowEventListener*> mListeners;

		std::string mName;
		shared_ptr<Gtk::Main> mGTKMain;
		Gtk::GL::DrawingArea* mGLWidget;

		s32 mContextRef;
		Glib::RefPtr<Gdk::GL::Context> mMainContext;
		Glib::RefPtr<Gdk::GL::Window> mMainWindow;
		void CleanUp();
	};
}
#endif
