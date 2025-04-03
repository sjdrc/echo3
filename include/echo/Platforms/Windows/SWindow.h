// A solution to Windows events in C++
//
//////////////////////////////////////////////////////////////////////////
// Most simple example usage
//////////////////////////////////////////////////////////////////////////
// SWindow window; 
//
// window.CreateAndRegisterDialog(IDD_DIALOG1);		//Or whatever IDD your Dialog is
// window.Show();
//
// MSG msg;
// BOOL bRet; 
// while(bRet = GetMessage( &msg, NULL, 0, 0 ))
// { 
//	 if(!IsDialogMessage(0,&msg))
//	 {
//		 TranslateMessage(&msg); 
//		 DispatchMessage(&msg);
//	 }
// }
//////////////////////////////////////////////////////////////////////////
//
// It performs all the magic for you, not too useful there cause you're not
// doing anything with the messages - it doesn't even post a quit message
// on WM_CLOSE. So lets say you wanted to do just that.
//
// You can derive from SWindow then implement a function of prototype
//
// int FunctionName(u32 wParam, u32 lParam);
//
// Then you can register it with a message.
//////////////////////////////////////////////////////////////////////////
// class Quiter : public SWindow
// {
// public:
//		Quiter();
//		int OnDestroy(u32 wParam, u32 lParam);
// };
//
// //We'll make this automatically register itself
// Quiter::Quiter()
// {
//		AddMessageHandler(WM_DESTROY,this,&Quiter::OnDestroy);
// }
//
// int Quiter::OnDestroy(u32 wParam, u32 lParam)
// {
//		PostQuitMessage(0);
//		return 0;
// }
//
//
// //Then somewhere else
//
// Quiter window; 
//
// window.CreateAndRegisterDialog(IDD_DIALOG1);		//Or whatever IDD your Dialog is
// window.Show();
//
// MSG msg;
// BOOL bRet; 
// while(bRet = GetMessage( &msg, NULL, 0, 0 ))
// { 
//	 if(!IsDialogMessage(0,&msg))
//	 {
//		 TranslateMessage(&msg); 
//		 DispatchMessage(&msg);
//	 }
// }
//////////////////////////////////////////////////////////////////////////
//
// You also have other options for adding message handlers
// AddCommandHandler();		//For WM_COMMAND messages to function handler
// AddMessageHandler();		//For ANY messages to function handler
// AddWindowEventHandler();	//For WM_COMMAND messages with specified eventID - depends on Window Control
//
// Enjoy!

#ifdef ECHO_PLATFORM_WINDOWS
#ifndef _SWINDOW_H_
#define _SWINDOW_H_
#include <echo/Types.h>
#include <windows.h>
#undef ERROR
#include <map>

namespace Echo
{
	class SWindowFunctorBase
	{
	public:
		virtual int operator()(UINT message, u32 param01, u32 param02)=0;
	};


	template< class T >
	class SWindowFunctor : public SWindowFunctorBase
	{
	public:
		typedef int (T::*Func)(UINT,u32, u32);
	private:
		T* mInstance;
		Func mFunc;
	public:
		SWindowFunctor(T* instance, Func func)
		{
			mInstance=instance;
			mFunc=func;
		}

		virtual int operator()(UINT message, u32 param01, u32 param02)
		{
			return (*mInstance.*mFunc)(message, param01, param02);
		}
	};

	class SWindow
	{
	private:
		static std::map< HWND, SWindow* > mWindows;						//for lookup from static DlgProc
		std::map< u32, SWindowFunctorBase* > mCommands;					//for WM_COMMAND messages
		std::map< u32, SWindowFunctorBase* > mMessages;					//for any message
		std::map< std::pair<u32, u32>, SWindowFunctorBase* > mEvents;	//for window event messages for specific windows
		HWND mWindow;
		WNDPROC mOldWinProc;						//Only relevant for SWindows that have taken control of external windows
		SWindowFunctorBase* mPreTranslatFunc;
		SWindowFunctorBase* mWinProcCall;
		bool mCallOldProc;
	public:
		SWindow();
		~SWindow();
		void CleanUp();

		//Intercepts a window's messages. This provides SWindow functionality
		//for windows created externally.
		void TakeOverWindow(HWND windtowToTakeOver);
		void ReleaseTakenOverWindow();
		void SetCallOldProc(bool callIt){mCallOldProc=callIt;}
		WNDPROC GetOldWinProc() const { return mOldWinProc; }

		HWND GetWindow() const {return mWindow;}
		HWND GetChildWindow(int id);
		//Useful if you are creating your own window instead of using dialogs
		void SetWindow(HWND windowHwnd);

		HWND CreateAndRegisterDialog(int id, HWND par=NULL);
		HWND CreateAndRegisterDialog(HINSTANCE inst, int id, HWND par);

		template< class T>
		void AddCommandHandler(u32 command, T* obj, typename SWindowFunctor<T>::Func func);
		template< class T>
		void AddMessageHandler(u32 message, T* obj, typename SWindowFunctor<T>::Func func);

		template< class T>
		void AddWindowEventHandler(u32 wid, u32 eventID, T* obj, typename SWindowFunctor<T>::Func func);

		void Show();
		void Hide();

		template< class T>
		void SetPreTranslationFunc(T* obj, typename SWindowFunctor<T>::Func func);

		//Some basic handlers
		virtual int Close(UINT message, u32 wParam, u32 lParam);
		virtual int Command(UINT message, u32 wParam, u32 lParam);
		virtual int DlgCommand(UINT message, u32 wParam, u32 lParam);

		//Return false if you don't want the translation to go ahead
		virtual bool PreMessageTranslation(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		//Call this before your message translation
		static bool ProvidePreTranslation(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		static LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		static INT_PTR CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	};

	template<class T>
	void SWindow::AddCommandHandler(u32 command, T* obj, typename SWindowFunctor<T>::Func func)
	{
		//Clean up existing functor
		std::map<u32, SWindowFunctorBase*>::iterator it=mCommands.find(command);
		if(it!=mCommands.end())
			if(it->second)
				delete it->second;
		mCommands[command]=new SWindowFunctor<T>(obj, func);
	}

	template<class T>
	void SWindow::AddMessageHandler(u32 message, T* obj, typename SWindowFunctor<T>::Func func)
	{	
		//Clean up existing functor
		std::map<u32, SWindowFunctorBase*>::iterator it=mMessages.find(message);
		if(it!=mMessages.end())
			if(it->second)
				delete it->second;
		mMessages[message]=new SWindowFunctor<T>(obj, func);
	}

	template< class T>
	void SWindow::AddWindowEventHandler(u32 wid, u32 eventID, T* obj, typename SWindowFunctor<T>::Func func)
	{	
		//Clean up existing functor
		std::pair<u32, u32> handleEventPair(wid, eventID);
		std::map< std::pair<u32, u32>, SWindowFunctorBase* >::iterator it=mEvents.find(handleEventPair);
		if(it!=mEvents.end())
			if(it->second)
				delete it->second;
		mEvents[handleEventPair]=new SWindowFunctor<T>(obj, func);
	}

	template< class T>
	void SWindow::SetPreTranslationFunc(T* obj, typename SWindowFunctor<T>::Func func)
	{
		if(mPreTranslatFunc)
			delete mPreTranslatFunc;
		mPreTranslatFunc=new SWindowFunctor<T>(obj, func);
	}

}
#endif
#endif
