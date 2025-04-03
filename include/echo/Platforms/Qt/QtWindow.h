#ifndef ECHOQTWINDOW_H
#define ECHOQTWINDOW_H

#include <echo/Platforms/GL/GLRenderTarget.h>
#include <QtGui/QWindow>

#include <echo/Types.h>

class QOpenGLContext;
class QGuiApplication;

namespace Echo
{
	class QtWindowEventListener
	{
	public:
		QtWindowEventListener(){}
		virtual ~QtWindowEventListener(){}
		virtual void MouseMoved(f32 newX, f32 newY) = 0;
		virtual void LeftButtonStateChanged(f32 atX, f32 atY, bool state) = 0;
		virtual void RightButtonStateChanged(f32 atX, f32 atY, bool state) = 0;
		virtual void MiddleButtonStateChanged(f32 atX, f32 atY, bool state) = 0;
		virtual void KeyStateChanged(u32 key, bool state) = 0;
	};

	class QtWindow : public GLRenderTarget, public QWindow
	{
	public:
		QtWindow(shared_ptr<GLContext> context, Kernel& kernel, const std::string& title, u32 width,
				 u32 height, u8 pixelBitDepth, bool fullscreenflag, shared_ptr<QGuiApplication> application, QWindow* parent = 0);
		~QtWindow();
		
		QOpenGLContext* GetGLContext()
		{
			return mQOpenGLContext;
		}

		void AddListener(QtWindowEventListener* listener);
		void RemoveListener(QtWindowEventListener* listener);

		virtual void SetSize(u32 width, u32 height) override;
		virtual bool Activate() override;
		virtual void Deactivate() override;
		virtual void SwapBuffers() override;
		
		void SetName(const std::string& name) override;
		std::string GetName() override;
	protected:
		bool event(QEvent *event) override;
	private:
		Kernel& mKernel;
		std::list<QtWindowEventListener*> mListeners;
		void InternalLeftButtonStateChanged(f32 atX, f32 atY, bool state);
		void InternalRightButtonStateChanged(f32 atX, f32 atY, bool state);
		void InternalMiddleButtonStateChanged(f32 atX, f32 atY, bool state);
		void InternalKeyStateChanged(u32 key, bool state);
		
		virtual void mouseMoveEvent(QMouseEvent *event) override;
		virtual void mousePressEvent(QMouseEvent *event) override;
		virtual void mouseReleaseEvent(QMouseEvent *event) override;
		virtual void keyPressEvent(QKeyEvent *event) override;
		virtual void keyReleaseEvent(QKeyEvent *event) override;
		
		std::string mName;

		s32 mContextRef;
		QOpenGLContext* mQOpenGLContext;
	};
}
#endif
