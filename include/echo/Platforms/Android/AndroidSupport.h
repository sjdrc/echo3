#ifndef _ECHOANDROIDSUPPORT_H_
#define _ECHOANDROIDSUPPORT_H_

#include <echo/Types.h>
#include <echo/UTF8String.h>
#include <echo/Maths/Vector2.h>
#include <vector>
#include <string>

struct AAssetManager;

namespace Echo
{
	class PseudoMouse;
	class PseudoKeyboard;
	class RenderTargetNotifier;
	class Kernel;
	class InputDevice;
	class SpeechToTextReceiver;
	
	class AndroidSupport
	{
	public:
		AndroidSupport(u32 screenWidth, u32 screenHeight);
		~AndroidSupport();
		
		// This pointer is set by the AndroidGlue code. The lifetime of the object is determined by the Java objects.
		void GraphicsLost();
		void GraphicsRestored();
		void SetMouse(float x, float y, bool pressed);
		
		void SetAssetManager(AAssetManager* assetManager) {mAssetManager=assetManager;}
		AAssetManager* GetAssetManager() const {return mAssetManager;}
		
		void SetInternalApplicationDirectory(const std::string& internalApplicationDirectory) {mInternalApplicationDirectory=internalApplicationDirectory;}
		std::string GetInternalApplicationDirectory() const {return mInternalApplicationDirectory;}
		
		RenderTargetNotifier* GetRenderTargetNotifier(){return mRenderTargetNotifier;}
		
		bool InitialiseEcho(shared_ptr<Kernel> kernel);
		bool GetInitialised();
		bool Start();
		void CleanUp();
		bool Update();
		void Pause();
		void Resume();
		void SetDeviceScreenResolution(int width, int height, float xdpi, float ydpi);

		bool GetConfigurationBool(const std::string& optionName, bool defaultValue);
		
		void SetLinearAcceleration(float x, float y, float z);
		void SetGyroscope(float w, float x, float y, float z);
		void SetGravity(float x, float y, float z);

		std::vector< shared_ptr< InputDevice > > GetInputDevices();
		
		void SetSpeechToTextResultsInterface(shared_ptr<SpeechToTextReceiver> speechToTextReceiver);
		void AddSpeechRecognitionPartialResult(const std::string& partialResult, float confidence, int resultNumber, int totalResults);
		void AddSpeechRecognitionFinalResult(const std::string& finalResult, float confidence, int resultNumber, int totalResults);
		
		u32 GetScreenWidth() const {return mScreenWidth;}
		u32 GetScreenHeight() const {return mScreenHeight;}
	private:
		AAssetManager* mAssetManager;
		std::string mInternalApplicationDirectory;
		shared_ptr<PseudoMouse> mPseudoMouse;
		shared_ptr<PseudoKeyboard> mPseudoKeyboard;
		shared_ptr< InputDevice > mAccelerometer;
		shared_ptr< InputDevice > mGyroscope;
		shared_ptr< InputDevice > mGravity;
		RenderTargetNotifier* mRenderTargetNotifier;
		shared_ptr<Kernel> mKernel;
		shared_ptr<SpeechToTextReceiver> mSpeechToTextReceiver;
		std::vector<std::pair<UTF8String, float> > mSpeechToTextResults;
		u32 mScreenWidth;
		u32 mScreenHeight;
		Vector2 mMouseNoTouchPosition;
	};
}
#endif
