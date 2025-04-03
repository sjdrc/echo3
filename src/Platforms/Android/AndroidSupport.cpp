#include <echo/Platforms/Android/AndroidSupport.h>
#include <echo/Input/PseudoMouse.h>
#include <echo/Input/PseudoKeyboard.h>
#include <echo/Input/GenericInputDevice.h>
#include <echo/Speech/SpeechToTextReceiver.h>
#include <echo/Graphics/RenderTargetNotifier.h>
#include <echo/Graphics/RenderTarget.h>
#include <echo/Application.h>

struct AAssetManager;

namespace Echo
{
	AndroidSupport::AndroidSupport(u32 screenWidth, u32 screenHeight) :
		mScreenWidth(screenWidth),
		mScreenHeight(screenHeight)
	{
		mPseudoMouse = make_shared<PseudoMouse>("Mouse");
		mPseudoKeyboard = make_shared<PseudoKeyboard>("Keyboard");
		mAccelerometer = make_shared<OmniInputDevice<Vector3> >("Accelerometer","Vector3", Vector3::ZERO);
		mGyroscope = make_shared<OmniInputDevice<Quaternion> >("Gyroscope","Quaternion", Quaternion::ZERO);
		mGravity = make_shared<OmniInputDevice<Vector3> >("Gravity","Vector3", Vector3::ZERO);
		mRenderTargetNotifier = new RenderTargetNotifier();
		// The screen is 0-1 in X and Y so we'll just move the mouse way off screen to
		// an arbitrary position. If this needs to be configured at some point just add
		// a mutator method.
		mMouseNoTouchPosition = Vector2(100.f,100.f);
	}

	AndroidSupport::~AndroidSupport()
	{
		CleanUp();
		delete mRenderTargetNotifier;
		mRenderTargetNotifier = 0;
	}
	
	std::vector< shared_ptr< InputDevice > > AndroidSupport::GetInputDevices()
	{
		std::vector< shared_ptr< InputDevice > > devices;
		devices.push_back(mPseudoMouse);
		devices.push_back(mPseudoKeyboard);
		devices.push_back(mAccelerometer);
		devices.push_back(mGyroscope);
		devices.push_back(mGravity);
		return devices;
	}

	void AndroidSupport::GraphicsLost()
	{
		if(mRenderTargetNotifier)
		{
			mRenderTargetNotifier->GraphicsContextLost();
		}
	}
	
	void AndroidSupport::GraphicsRestored()
	{
		if(mRenderTargetNotifier)
		{
			mRenderTargetNotifier->GraphicsContextRestored();
		}
	}
	
	void AndroidSupport::SetMouse(float x, float y, bool pressed)
	{
		mPseudoMouse->MouseMove(x/static_cast<float>(mScreenWidth), y/static_cast<float>(mScreenHeight));
		mPseudoMouse->SetLeft(pressed);
	}
	
	void AndroidSupport::SetLinearAcceleration(float x, float y, float z)
	{
		static_cast<OmniInputDevice<Vector3>*>(mAccelerometer.get())->Set(Vector3((x,y,z)));
	}
	
	void AndroidSupport::SetGyroscope(float w, float x, float y, float z)
	{
		static_cast<OmniInputDevice<Quaternion>*>(mGyroscope.get())->Set(Quaternion(x,y,z,w));
	}

	void AndroidSupport::SetGravity(float x, float y, float z)
	{
		static_cast<OmniInputDevice<Vector3>*>(mGravity.get())->Set(Vector3((x,y,z)));
	}
	
	bool AndroidSupport::InitialiseEcho(shared_ptr<Kernel> kernel)
	{
		mKernel = kernel;
		return (mKernel != nullptr);
	}
	
	bool AndroidSupport::GetConfigurationBool(const std::string& optionName, bool defaultValue)
	{
		if(mKernel)
		{
			Application* application = dynamic_cast<Application*>(mKernel.get());
			if(application)
			{
				return application->GetConfiguration().Get(optionName, defaultValue);
			}
		}
		return false;
	}
	
	bool AndroidSupport::GetInitialised()
	{
		return (mKernel != nullptr);
	}
	
	bool AndroidSupport::Start()
	{
		if(!mKernel)
		{
			return false;
		}
		return (mKernel->Execute() == Kernel::ExecutionResults::INCOMPLETE);
	}
	
	void AndroidSupport::CleanUp()
	{
		mKernel.reset();
		mAssetManager = 0;
		mInternalApplicationDirectory.clear();
	}

	bool AndroidSupport::Update()
	{
		if(mKernel)
		{
			bool continueProcessing = mKernel->ProcessFrame();

			// Android devices usually have a touch screen so when the screen isn't
			// touched no touch event is received to update the position. This usually
			// leaves the cursor on screen in the last touch position but this
			// behaviour isn't desirable on touch screens as it makes it look like
			// you're performing input when you're not. An example is when an Element
			// release event causes a new Element to be displayed on screen where the
			// first Element was. The new Element receives the enter then hover events
			// which isn't what the user would expect since touch screens generally
			// don't have visible cursors.
			// Note: This implementation will probably need to change when adding
			// support for multi-touch.
			mPseudoMouse->MouseMove(mMouseNoTouchPosition.x, mMouseNoTouchPosition.y);
			mPseudoMouse->SetLeft(false);
			return continueProcessing;
		}
		return false;
	}

	void AndroidSupport::Pause()
	{
		if(mKernel)
		{
			mKernel->PauseKernel(true);
		}
	}

	void AndroidSupport::Resume()
	{
		if(mKernel)
		{
			mKernel->ResumeKernel(true);
		}
	}

	void AndroidSupport::SetDeviceScreenResolution(int width, int height, float xdpi, float ydpi)
	{
		if(!mRenderTargetNotifier)
		{
			return;
		}
		
		shared_ptr<RenderTarget> pTarget = mRenderTargetNotifier->GetPrimaryRenderTarget();
		if(pTarget)
		{
			ECHO_LOG_INFO("(AndroidSupport) Screen resolution: " << width << "x" << height);
			// This forces the screen resolution to the device size to make it full screen.
			// The render target ResizeMode should be used to control how the viewport scaling works.
			pTarget->SetSize(width,height);
			pTarget->SetDisplayXDPI(xdpi);
			pTarget->SetDisplayYDPI(ydpi);
		}
		mScreenWidth = (width == 0) ? 1 : width;
		mScreenHeight = (height== 0) ? 1 : height;
	}

	void AndroidSupport::SetSpeechToTextResultsInterface(shared_ptr<SpeechToTextReceiver> speechToTextReceiver)
	{
		mSpeechToTextReceiver = speechToTextReceiver;
	}

	void AndroidSupport::AddSpeechRecognitionPartialResult(const std::string& partialResult, float confidence, int resultNumber, int totalResults)
	{
		if(mSpeechToTextReceiver && partialResult.length()>0)
		{
			mSpeechToTextResults.push_back(std::make_pair(UTF8String(partialResult),confidence));
			//ECHO_LOG_DEBUG("Partial result is" << UTF8String(partialResult));
			//mSpeechToTextResults.push_back(std::make_pair(UTF8String("hello there"),1.f));
			if(resultNumber==totalResults)
			{
				mSpeechToTextReceiver->OnSpeechPartialResults(mSpeechToTextResults);
				mSpeechToTextResults.resize(0);
			}
		}
	}
	
	void AndroidSupport::AddSpeechRecognitionFinalResult(const std::string& finalResult, float confidence, int resultNumber, int totalResults)
	{
		if(mSpeechToTextReceiver && finalResult.length()>0)
		{
			mSpeechToTextResults.push_back(std::make_pair(UTF8String(finalResult),confidence));
			//ECHO_LOG_DEBUG("Final result is" << UTF8String(finalResult));
			//mSpeechToTextResults.push_back(std::make_pair(UTF8String("this is final"),1.f));
			if(resultNumber==totalResults)
			{
				mSpeechToTextReceiver->OnSpeechFinalResults(mSpeechToTextResults);
				mSpeechToTextResults.resize(0);
			}
		}
	}
}
