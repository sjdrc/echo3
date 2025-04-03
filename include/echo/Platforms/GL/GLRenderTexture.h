#include <echo/Platforms/GL/GLRenderTarget.h>

#ifdef ECHO_RENDERER_GL
#ifndef _ECHOGLRENDERTEXTURE_H_
#define _ECHOGLRENDERTEXTURE_H_
#ifndef ECHO_PLATFORM_WINDOWS
#include <gtkmm/main.h>
#include <gtkmm/window.h>
#include <gtkglmm.h>

namespace Echo
{
	class GLTexture;
	class GLContext;

	/**
	 * Rendering to a Texture is accomplished by setting up a texture in the GL context that maps to the instance of GLRenderTexture.
	 * GLRenderTexture configures a frame buffer to be a target for rendering in Activate() and Deactivate().
	 */
	class GLRenderTexture : public GLRenderTarget, public Texture
	{
	public:
		GLRenderTexture(shared_ptr<GLContext> context, const std::string& title, u32 width, u32 height, Texture::Format format = Texture::Formats::R8G8B8A8, Texture::Format depthFormat = Texture::Formats::DEPTH24);
		~GLRenderTexture();

		void SetSize(u32 width, u32 height) override;

		bool Activate() override;
		void Deactivate() override;
		void SwapBuffers() override;
		void SetName(const std::string& name) override;
		std::string GetName() override;

		/**
		 * Create a Texture to copy the depth buffer into.
		 * If this is not set no depth buffer copy will occur. You can access the texture with GetDepthTexture(). The instance will not change from frame to
		 * frame so it can be used as a regular resource.
		 * The format can be modified and GL will convert from floating point 0.0 - 1.0 to the corresponding format as per this page
		 * https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glReadPixels.xhtml
		 */
		void CreateDepthTexture(Texture::Format format = Texture::Formats::GREYSCALE16);

		/**
		 * Clear the internal depth texture.
		 * This stops the depth texture copy.
		 */
		void ClearDepthTexture();

		/**
		 * Get the Depth Texture.
		 * @return nullptr if the depth texture is not set, otherwise the texture.
		 */
		shared_ptr<Texture> GetDepthTexture();
	private:
		std::string mName;
		shared_ptr<GLTexture> mGLTexture;
		shared_ptr<Texture> mDepthTexture;
		shared_ptr<GLTexture> mGLDepthTexture;

		GLuint mFrameBuffer;
		bool mReady;

		s32 mContextRef;
		void CleanUp();

		// This needs to go
		shared_ptr<Gtk::Main> mGTKMain;
		shared_ptr<Gtk::Window> mGTKWindow;
		Gtk::GL::DrawingArea* mGLWidget;

		Glib::RefPtr<Gdk::GL::Context> mMainContext;
		Glib::RefPtr<Gdk::GL::Window> mMainWindow;
	};
}
#endif
#endif
#endif
