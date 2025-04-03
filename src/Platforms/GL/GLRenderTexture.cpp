#include <echo/Platforms/GL/GLRenderTexture.h>
#include <echo/Platforms/GL/GLTexture.h>
#include <echo/Maths/Length.h>

#ifndef ECHO_PLATFORM_WINDOWS
// NOTE: We need a refactor for this to be moved elsewhere
#include <gdkmm-2.4/gdkmm/event.h>
#include <gtkmm-2.4/gtkmm/widget.h>
#include <gdkmm-2.4/gdkmm/rectangle.h>

namespace Echo
{
	GLRenderTexture::GLRenderTexture(shared_ptr<GLContext> context, const std::string& title, u32 width, u32 height, Texture::Format format, Texture::Format depthFormat) :
		GLRenderTarget(context), Texture(width,height,format)
	{
		assert(width>=0 && height>=0 && "Cannot create texture with a width or height of 0.");
		mIsFullScreen = false;

		mName = title;
		mContextRef = 0;

		Gtk::GL::init(0, 0);

		mGTKWindow.reset(new Gtk::Window());
		mGTKWindow->set_title(title);
		mGTKWindow->set_default_size(width,height);

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
			mGLWidget->set_gl_capability(glconfig, mMainContext, true, Gdk::GL::RGBA_TYPE);
		}

		mGTKWindow->add(*mGLWidget);
		mGTKWindow->show_all();
		mGTKWindow->hide_all();

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
		glewInit();

		// This is a side effect of an earlier bad design decision. I'll sort it out one day.
		GLRenderTarget::mWidth = width;
		GLRenderTarget::mHeight = height;

		// This creates a GLTexture
		mGLTexture = GetGLTexture(this);

		mBytesPerPixel = Texture::GetBytesPerPixel();
		mFrameBuffer = 0;
		glGenFramebuffers(1, &mFrameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer);

		// Attach colour texture
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mGLTexture->GetGLReference(), 0);

		// Attach depth texture
		GLint depthAttachmentType = (depthFormat==Formats::DEPTH24_STENCIL8) ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT;
		mDepthTexture = make_shared<Texture>(GLRenderTarget::mWidth, GLRenderTarget::mHeight,depthFormat);
		mGLDepthTexture = GetGLTexture(mDepthTexture.get());
		glFramebufferTexture2D(GL_FRAMEBUFFER, depthAttachmentType, GL_TEXTURE_2D, mGLDepthTexture->GetGLReference(), 0);

		// Set the list of draw buffers for the frame buffer
		GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, drawBuffers);

		// Always check that our framebuffer is ok
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			ECHO_LOG_ERROR("There was an error setting up a frame buffer to render to");
			mReady = false;
		}else
		{
			mReady = true;
		}
		mMainWindow->gl_end();
	}

	GLRenderTexture::~GLRenderTexture()
	{
	}

	void GLRenderTexture::SetName(const std::string& name)
	{
		mName = name;
	}

	std::string GLRenderTexture::GetName()
	{
		return mName;
	}

	void GLRenderTexture::SetSize(u32 width, u32 height)
	{
	}

	void GLRenderTexture::ClearDepthTexture()
	{
		mDepthTexture.reset();
	}

	shared_ptr<Texture> GLRenderTexture::GetDepthTexture()
	{
		return mDepthTexture;
	}

	bool GLRenderTexture::Activate()
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

			// Begin
			glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer);
			GLRenderTarget::Activate();
		}
		return true;
	}

	void GLRenderTexture::Deactivate()
	{
		GLRenderTarget::Deactivate();
		--mContextRef;
		if(mContextRef < 0)
		{
			ECHO_LOG_ERROR("No context to deactivate");
		}
		if(mContextRef == 0)
		{
			// End
			if(mDepthTexture)
			{
				GLint type = GLTexture::GetGLTexelType(mDepthTexture->GetFormat());
				GLint texalFormat = GLTexture::GetGLTexelFormat(mDepthTexture->GetFormat());
				
				glReadPixels(0,0,GLRenderTarget::mWidth, GLRenderTarget::mHeight,texalFormat,type,mDepthTexture->GetBuffer().get());
				// We need to increment the version of this texture so anything using this resource knows it has updated.
				mDepthTexture->IncrementVersion();
				// Set the GLTexture version so it doesn't have to do a resource copy back to GPU in this GLContext
				mGLDepthTexture->SetVersion(Resource::GetVersion());
				EchoCheckOpenGLError();
			}
			if(mGLTexture->GetData(*this))
			{
				// We need to increment the version of this texture so anything using this resource knows it has updated.
				IncrementVersion();
				// Set the GLTexture version so it doesn't have to do a resource copy back to GPU in this GLContext
				mGLTexture->SetVersion(Resource::GetVersion());
			}else
			{
				ECHO_LOG_ERROR("Failed to copy data back to Texture buffer after rendering to texture");
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			mMainWindow->gl_end();
	}
	}

	void GLRenderTexture::SwapBuffers()
	{
		Glib::RefPtr<Gdk::GL::Window> glwindow = mGLWidget->get_gl_window();
		glwindow->swap_buffers();
	}
}
#endif
