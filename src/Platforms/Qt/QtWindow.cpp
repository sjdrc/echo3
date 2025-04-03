#include <echo/Platforms/Qt/QtWindow.h>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLPaintDevice>
#include <QtGui/QMouseEvent>
#include <QtGui/QKeyEvent>
#include <QtGui/QResizeEvent>
#include <QtGui/QScreen>
#include <QtGui/QGuiApplication>
#include <echo/Maths/Length.h>
#include <iostream>

namespace Echo
{
	QtWindow::QtWindow(shared_ptr<GLContext> context, Kernel& kernel, const std::string& title, u32 width,
				 u32 height, u8 pixelBitDepth, bool fullscreenflag, shared_ptr<QGuiApplication> application, QWindow* parent) :
		GLRenderTarget(context),
		QWindow(parent),
		mKernel(kernel)
	{
		ECHO_ASSERT(width>=0 && height>=0, "Cannot create window with a width or height of 0.");
		mBytesPerPixel = pixelBitDepth / 8;
		mIsFullScreen = fullscreenflag;

		mName = title;
		mContextRef = 0;
		
		setSurfaceType(QWindow::OpenGLSurface);
		QSurfaceFormat format;
		format.setDepthBufferSize( 24 );
		format.setSamples( 16 );
		format.setProfile( QSurfaceFormat::CoreProfile );
		setFormat( format );
		create();

		setTitle(title.c_str());

		if(fullscreenflag)
		{
			ECHO_LOG_WARNING("Full screen using Qt is currently not supported when creating a QtWindow.");
		}

        mQOpenGLContext = new QOpenGLContext(this);
        mQOpenGLContext->setFormat(format);
        mQOpenGLContext->create();
		
		QScreen* screen = application->primaryScreen();
		if(screen)
		{
			QRect geometry = screen->geometry();
			SetDisplayXDPI(screen->physicalDotsPerInchX());
			SetDisplayYDPI(screen->physicalDotsPerInchY());

			ECHO_LOG_INFO("Screen width: " << geometry.width() << " (" << Millimetre(Inch(Scalar(screen->physicalDotsPerInchX()))) << " " << GetDisplayXDPI() << "DPI)");
			ECHO_LOG_INFO("Screen height: " << geometry.height() << " (" << Millimetre(Inch(Scalar(screen->physicalDotsPerInchY()))) << " " << GetDisplayYDPI() << "DPI)");
		}

		mContext->mContextReady = true;
		
		resize(width, height);
		show();

		Activate();
		DetermineCapabilities();
		Deactivate();

		mWidth = width;
		mHeight = height;
	}

	QtWindow::~QtWindow()
	{
		Deactivate();
	}

	void QtWindow::SetSize(u32 width, u32 height)
	{
		resize(width, height);
	}

	bool QtWindow::Activate()
	{
		if(!mContext->mContextReady)
		{
			mContextRef = 0;
			return false;
		}
		++mContextRef;
		if(mContextRef == 1)
		{
			if(!mQOpenGLContext->makeCurrent(this))
			{
				ECHO_LOG_ERROR("Failed to set current GL context");
				--mContextRef;
				return false;
			}
			glewInit();
			GLRenderTarget::Activate();
		}
		return true;
	}

	void QtWindow::Deactivate()
	{
		GLRenderTarget::Deactivate();
		--mContextRef;
		if(mContextRef < 0)
		{
			ECHO_LOG_WARNING("Deactivate on QtWindow called more times than Activate.");
			mContextRef = 0;
		}
		if(mContextRef == 0)
		{
			mQOpenGLContext->doneCurrent();
		}
	}
	
	void QtWindow::SwapBuffers()
	{
		mQOpenGLContext->swapBuffers(this);
	}
	
	bool QtWindow::event(QEvent *event)
	{
		switch (event->type())
		{
		case QEvent::UpdateRequest:
			break;
		case QEvent::ApplicationStateChange:
			break;
		case QEvent::Close:
			if(IsPrimary())	//This flag is false by default, the Application will set the appropriate window to be a primary window.
			{
				mKernel.RemoveAllTasks();
			}
			break;
		case QEvent::WindowDeactivate:
		case QEvent::FocusOut:
		case QEvent::Hide:	//Maybe for pausing rendi
			//This implementation supports disabling application pause/resume
			if(mKernel.GetPauseHint())
			{
				mKernel.PauseKernel(true);
			}
			break;
		case QEvent::Resize:
		{
			QResizeEvent* resizeEvent = dynamic_cast<QResizeEvent*>(event);
			if(resizeEvent)
			{
				mWidth = resizeEvent->size().width();
				mHeight = resizeEvent->size().height();
				if(mContext->mScissorEnabled)
				{
					mContext->mScissorRectangle=Viewport::Rectangle(0,0,1,1);
				}
			}else
			{
				ECHO_LOG_ERROR("QEvent::Resize passed an event not of type QResuzeEvent");
			}
		}break;
		case QEvent::WindowActivate:
		case QEvent::FocusIn:
		case QEvent::Show:
			//This implementation supports disabling application pause/resume
			if(mKernel.GetPauseHint())
			{
				mKernel.ResumeKernel(true);
			}
			break;
		//case QEvent::UpdateRequest:	// For Controlled applications this could be used to process a frame?
		//	mKernel.ProcessFrame();
		//	return true;
		case QEvent::Wheel:
			break;
		default:
			break;
		}
		return QWindow::event(event);
	}

	void QtWindow::AddListener(QtWindowEventListener* listener)
	{
		mListeners.push_back(listener);
	}

	void QtWindow::RemoveListener(QtWindowEventListener* listener)
	{
		mListeners.remove(listener);
	}

	void QtWindow::InternalLeftButtonStateChanged(f32 atX, f32 atY, bool state)
	{
		for(auto& listener : mListeners)
		{
			listener->LeftButtonStateChanged(atX, atY, state);
		}
	}

	void QtWindow::InternalRightButtonStateChanged(f32 atX, f32 atY, bool state)
	{
		for(auto& listener : mListeners)
		{
			listener->RightButtonStateChanged(atX, atY, state);
		}
	}

	void QtWindow::InternalMiddleButtonStateChanged(f32 atX, f32 atY, bool state)
	{
		for(auto& listener : mListeners)
		{
			listener->MiddleButtonStateChanged(atX, atY, state);
		}
	}

	void QtWindow::InternalKeyStateChanged(u32 key, bool state)
	{
		for(auto& listener : mListeners)
		{
			listener->KeyStateChanged(key, state);
		}
	}
	
	void QtWindow::SetName(const std::string& name)
	{
		mName = name;
		setTitle(mName.c_str());
	}
	
	std::string QtWindow::GetName()
	{
		return mName;
	}
	
	void QtWindow::mouseMoveEvent(QMouseEvent *event)
	{
		for(auto& listener : mListeners)
		{
			listener->MouseMoved(event->x(),event->y());
		}
	}
	
	void QtWindow::mousePressEvent(QMouseEvent *event)
	{
		Qt::MouseButton button = event->button();
		switch(button)
		{
			case Qt::LeftButton:
				InternalLeftButtonStateChanged(event->x(), event->y(), true);
			break;
			case Qt::MiddleButton:
				InternalMiddleButtonStateChanged(event->x(), event->y(), true);
			break;
			case Qt::RightButton:
				InternalRightButtonStateChanged(event->x(), event->y(), true);
			break;
			default:
			break;
		}
	}
	
	void QtWindow::mouseReleaseEvent(QMouseEvent *event)
	{
		Qt::MouseButton button = event->button();
		switch(button)
		{
			case Qt::LeftButton:
				InternalLeftButtonStateChanged(event->x(), event->y(), false);
			break;
			case Qt::MiddleButton:
				InternalMiddleButtonStateChanged(event->x(), event->y(), false);
			break;
			case Qt::RightButton:
				InternalRightButtonStateChanged(event->x(), event->y(), false);
			break;
			default:
			break;
		}
	}

	void QtWindow::keyPressEvent(QKeyEvent *event)
	{
		InternalKeyStateChanged(event->key(), true);
	}

	void QtWindow::keyReleaseEvent(QKeyEvent *event)
	{
		InternalKeyStateChanged(event->key(), false);
	}
}
