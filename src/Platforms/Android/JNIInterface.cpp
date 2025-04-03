#include <jni.h>
#include <GLES/gl.h>
#include <android/log.h>
#include <android/asset_manager_jni.h>
#include <echo/Platforms/Android/AndroidSupport.h>
#include <iostream>

//This redirects stdout to the logcat view
class androidbuf: public std::streambuf {
public:
	enum { bufsize = 128 }; // ... or some other suitable buffer size
	androidbuf() { this->setp(buffer, buffer + bufsize - 1); }
private:
	int overflow(int c) {
		if (c == traits_type::eof()) {
			*this->pptr() = traits_type::to_char_type(c);
			this->sbumpc();
		}
		return this->sync()? traits_type::eof(): traits_type::not_eof(c);
	}
	int sync() {
		int rc = 0;
		if (this->pbase() != this->pptr()) {
			__android_log_print(ANDROID_LOG_INFO,
							   "Native",
							   "%s",
							   std::string(this->pbase(),
										   this->pptr() - this->pbase()).c_str());
			rc = 0;
			this->setp(buffer, buffer + bufsize - 1);
		}
		return rc;
	}
	char buffer[bufsize];
};

// The AndroidSupport object is the interface between JNI and Echo.
namespace Echo
{
	extern AndroidSupport* androidSupport;
}
using namespace Echo;

androidbuf* redirectBuffer = 0;
extern shared_ptr<Kernel> EchoInitialise();

extern "C"
{
	JNIEXPORT void JNICALL Java_au_com_emblementertainment_echo_Echo_Initialise(JNIEnv * env, jobject obj, jint width, jint height, jfloat xdpi, jfloat ydpi, jobject assetManager, jstring internalApplicationDirectory)
	{
		redirectBuffer = new androidbuf();
		std::cout.rdbuf(redirectBuffer);

		androidSupport = new AndroidSupport(width,height);
		//Acquire the asset manager.
		androidSupport->SetAssetManager(AAssetManager_fromJava(env, assetManager));

		//Convert the internal application directory java string to a C++ string.
		const char* internalApplicationDirectoryTemp = env->GetStringUTFChars(internalApplicationDirectory,0);
		androidSupport->SetInternalApplicationDirectory(std::string(internalApplicationDirectoryTemp));
		env->ReleaseStringUTFChars(internalApplicationDirectory,internalApplicationDirectoryTemp);

		shared_ptr<Kernel> kernel = EchoInitialise();
		if(androidSupport->InitialiseEcho(kernel))
		{
			androidSupport->SetDeviceScreenResolution(width,height,xdpi,ydpi);
			if(!androidSupport->Start())
			{
				androidSupport->CleanUp();
			}
		}
	}

	//cleanUp
	JNIEXPORT void JNICALL Java_au_com_emblementertainment_echo_Echo_CleanUp(JNIEnv * env, jobject obj)
	{
		if(androidSupport)
		{
			androidSupport->CleanUp();
		}
	}

	//isInitialised
	JNIEXPORT bool JNICALL Java_au_com_emblementertainment_echo_Echo_IsInitialised(JNIEnv * env, jobject obj)
	{
		if(androidSupport)
		{
			return androidSupport->GetInitialised();
		}
		return false;
	}

	//step
	JNIEXPORT bool JNICALL Java_au_com_emblementertainment_echo_Echo_Update(JNIEnv * env, jobject obj)
	{
		if(androidSupport)
		{
			return androidSupport->Update();
		}
		return false;
	}

	//pause
	JNIEXPORT void JNICALL Java_au_com_emblementertainment_echo_Echo_Pause(JNIEnv * env, jobject obj)
	{
		if(androidSupport)
		{
			androidSupport->Pause();
		}
	}

	//resume
	JNIEXPORT void JNICALL Java_au_com_emblementertainment_echo_Echo_Resume(JNIEnv * env, jobject obj)
	{
		if(androidSupport)
		{
			androidSupport->Resume();
		}
	}

	//graphicsRestored
	JNIEXPORT void JNICALL Java_au_com_emblementertainment_echo_Echo_GraphicsRestored(JNIEnv * env, jobject obj)
	{
		if(androidSupport)
		{
			androidSupport->GraphicsRestored();
		}
	}

	//graphicsLost
	JNIEXPORT void JNICALL Java_au_com_emblementertainment_echo_Echo_GraphicsLost(JNIEnv * env, jobject obj)
	{
		if(androidSupport)
		{
			androidSupport->GraphicsLost();
		}
	}

	//setResolution
	JNIEXPORT void JNICALL Java_au_com_emblementertainment_echo_Echo_SetResolution(JNIEnv * env, jobject obj, jint width, jint height, jfloat xDPI, jfloat yDPI)
	{
		if(androidSupport)
		{
			androidSupport->SetDeviceScreenResolution(width,height, xDPI, yDPI);
		}
	}

	//setMouse
	JNIEXPORT void JNICALL Java_au_com_emblementertainment_echo_Echo_SetMouse(JNIEnv * env, jobject obj, jfloat x, jfloat y, jint pressed)
	{
		if(androidSupport)
		{
			androidSupport->SetMouse(x,y, (pressed==1));
		}
	}
	
	JNIEXPORT void JNICALL Java_au_com_emblementertainment_echo_Echo_SetLinearAcceleration(JNIEnv * env, jobject obj, jfloat x, jfloat y, jfloat z)
	{
		if(androidSupport)
		{
			androidSupport->SetLinearAcceleration(x,y,z);
		}
	}

	JNIEXPORT void JNICALL Java_au_com_emblementertainment_echo_Echo_SetGyroscope(JNIEnv * env, jobject obj, jfloat w, jfloat x, jfloat y, jfloat z)
	{
		if(androidSupport)
		{
			androidSupport->SetGyroscope(w,x,y,z);
		}
	}	

	JNIEXPORT void JNICALL Java_au_com_emblementertainment_echo_Echo_SetGravity(JNIEnv * env, jobject obj, jfloat x, jfloat y, jfloat z)
	{
		if(androidSupport)
		{
			androidSupport->SetGravity(x,y,z);
		}
	}
	
	JNIEXPORT void JNICALL Java_au_com_emblementertainment_echo_Echo_SetGenericDigitalInput(JNIEnv * env, jobject obj, jstring name, jboolean state)
	{
		if(androidSupport)
		{
			//Convert the internal application directory java string to a C++ string.
			const char* nameTemp = env->GetStringUTFChars(name,0);
//			androidSupport->SetGenericDigitalInput(std::string(nameTemp),state);
			env->ReleaseStringUTFChars(name,nameTemp);
		}
	}

	JNIEXPORT void JNICALL Java_au_com_emblementertainment_echo_Echo_AddSpeechRecognitionPartialResult(JNIEnv * env, jobject obj, jstring partialResult, jfloat confidence, jint resultNumber, jint totalResults)
	{
		if(androidSupport)
		{
			//Convert the internal application directory java string to a C++ string.
			const char* partialResultTemp = env->GetStringUTFChars(partialResult,0);
			androidSupport->AddSpeechRecognitionPartialResult(std::string(partialResultTemp),confidence,resultNumber,totalResults);
			env->ReleaseStringUTFChars(partialResult,partialResultTemp);
		}
	}
	JNIEXPORT void JNICALL Java_au_com_emblementertainment_echo_Echo_AddSpeechRecognitionFinalResult(JNIEnv * env, jobject obj, jstring finalResult, jfloat confidence, jint resultNumber, jint totalResults)
	{
		if(androidSupport)
		{
			//Convert the internal application directory java string to a C++ string.
			const char* finalResultTemp = env->GetStringUTFChars(finalResult,0);
			androidSupport->AddSpeechRecognitionFinalResult(std::string(finalResultTemp),confidence,resultNumber,totalResults);
			env->ReleaseStringUTFChars(finalResult,finalResultTemp);
		}
	}

	//GetConfigurationBool
	JNIEXPORT bool JNICALL Java_au_com_emblementertainment_echo_Echo_GetConfigurationBool(JNIEnv * env, jobject obj, jstring optionName, jboolean defaultValue)
	{
		if(androidSupport)
		{
			//Convert the internal application directory java string to a C++ string.
			const char* optionNameTemp = env->GetStringUTFChars(optionName,0);
			bool optionValue=androidSupport->GetConfigurationBool(std::string(optionNameTemp),defaultValue);
			env->ReleaseStringUTFChars(optionName,optionNameTemp);
			return optionValue;
		}
		return false;
	}
}
