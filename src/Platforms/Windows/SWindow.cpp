#include <echo/Platforms/Windows/SWindow.h>
#include <tchar.h>

namespace Echo
{

	std::map< HWND, SWindow* > SWindow::mWindows;

	SWindow::SWindow()
	{
		mWindow=0;
		mOldWinProc=0;
		mPreTranslatFunc=0;
		mCallOldProc=true;
		//AddMessageHandler(WM_CLOSE,this,&SWindow::Close);
	}

	SWindow::~SWindow()
	{
		CleanUp();
	}

	void SWindow::CleanUp()
	{
		//Delete all our functors
		std::map< u32, SWindowFunctorBase* >::iterator it=mCommands.begin();
		std::map< u32, SWindowFunctorBase* >::iterator itEnd=mCommands.end();
		while(it!=itEnd)
		{
			delete it->second;
			++it;
		}
		mCommands.clear();

		it=mMessages.begin();
		itEnd=mMessages.end();
		while(it!=itEnd)
		{
			delete it->second;
			++it;
		}
		mMessages.clear();

		std::map< HWND, SWindow* >::iterator wit=mWindows.find(mWindow);
		if(wit!=mWindows.end())
		{
			mWindows.erase(wit);
		}
		if(mPreTranslatFunc)
		{
			mPreTranslatFunc=0;
			delete mPreTranslatFunc;
		}
		ReleaseTakenOverWindow();
	}

	HWND SWindow::GetChildWindow(int id)
	{
		return GetDlgItem(GetWindow(),id);
	}

	void SWindow::TakeOverWindow(HWND windtowToTakeOver)
	{
		if(windtowToTakeOver==0)
			return;
		if(mWindow)
			CleanUp();
		mOldWinProc=(WNDPROC)GetWindowLongPtr(windtowToTakeOver, GWLP_WNDPROC);
		SetLastError(0);
		SetWindowLongPtr(windtowToTakeOver,GWLP_WNDPROC,(LONG_PTR)WinProc);
		//SetWindowLongPtr(windtowToTakeOver, DWLP_DLGPROC, (LONG_PTR)YourProc).
		SetProp(windtowToTakeOver,_T("SWindow"),this);
		mWindow=windtowToTakeOver;
		mWindows[mWindow]=this;
	}

	void SWindow::ReleaseTakenOverWindow()
	{
		if(mOldWinProc!=0)
		{
			SetWindowLongPtr(mWindow,GWLP_WNDPROC,(LONG_PTR)mOldWinProc);
			RemoveProp(mWindow,_T("SWindow"));
			mOldWinProc=0;
		}
	}


	void SWindow::Show()
	{
		ShowWindow(mWindow,TRUE);
	}

	void SWindow::Hide()
	{
		ShowWindow(mWindow,FALSE);
	}

	HWND SWindow::CreateAndRegisterDialog(int id, HWND par)
	{
		HINSTANCE i=GetModuleHandle(NULL);
		return CreateAndRegisterDialog(i, id, par);
	}

	HWND SWindow::CreateAndRegisterDialog(HINSTANCE inst, int id, HWND par)
	{
		AddMessageHandler(WM_COMMAND,this,&SWindow::DlgCommand);
		mWindow=CreateDialog(inst,MAKEINTRESOURCE(id),par,DlgProc);
		if(mWindow)
			mWindows[mWindow]=this;
		return mWindow;
	}

	void SWindow::SetWindow(HWND windowHwnd)
	{
		AddMessageHandler(WM_COMMAND,this,&SWindow::Command);
		mWindow=windowHwnd;
		if(mWindow)
			mWindows[mWindow]=this;
	}

	int SWindow::Close(UINT message, u32 wParam, u32 lParam)
	{
		DestroyWindow(mWindow);
		return 0;
	}

	int SWindow::Command(UINT message, u32 wParam, u32 lParam)
	{
		u32 wmId = LOWORD(wParam);
		int wmEvent = HIWORD(wParam); 

		//Events first
		std::pair<u32, u32> handleEventPair(wmId,wmEvent);
		std::map< std::pair<u32, u32>, SWindowFunctorBase* >::iterator eit=mEvents.find(handleEventPair);
		if(eit!=mEvents.end())
		{
			return (*(eit->second))(message,wParam,lParam);
		}

		u32 wmID2 = LOWORD(wParam);
		std::map< u32, SWindowFunctorBase* >::iterator it=mCommands.find(wmID2);
		if(it!=mCommands.end())
		{
			return (*(it->second))(message,wParam,lParam);
		}
		return (int)DefWindowProc(mWindow,message,wParam,lParam);
	}

	int SWindow::DlgCommand(UINT message, u32 wParam, u32 lParam)
	{
		u32 wmId = LOWORD(wParam);
		int wmEvent = HIWORD(wParam); 

		//Events first
		std::pair<u32, u32> handleEventPair(wmId,wmEvent);
		std::map< std::pair<u32, u32>, SWindowFunctorBase* >::iterator eit=mEvents.find(handleEventPair);
		if(eit!=mEvents.end())
		{
			return (*(eit->second))(message,wParam,lParam);
		}

		u32 wmID2 = LOWORD(wParam);
		std::map< u32, SWindowFunctorBase* >::iterator it=mCommands.find(wmID2);
		if(it!=mCommands.end())
		{
			return (*(it->second))(message,wParam,lParam);
		}
		return 0;
	}

	bool SWindow::PreMessageTranslation(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if(mPreTranslatFunc)
			return ((*(mPreTranslatFunc))(message,wParam,lParam)!=0);
		return true;
	}

	//Call this before your message translation
	bool SWindow::ProvidePreTranslation(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		std::map< HWND, SWindow* >::iterator wit=mWindows.find(hWnd);
		if(wit!=mWindows.end())
		{
			return wit->second->PreMessageTranslation(hWnd,message,wParam,lParam);
		}
		return true;
	}

	//INT_PTR CALLBACK SWindow::WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	LRESULT CALLBACK SWindow::WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if(message==WM_CREATE)
			return 0;
		std::map< HWND, SWindow* >::iterator wit=mWindows.find(hWnd);
		if(wit!=mWindows.end())
		{
			//Then any other messages
			std::map< u32, SWindowFunctorBase* >::iterator it=wit->second->mMessages.find(message);
			if(it!=wit->second->mMessages.end())
			{
				return (*(it->second))(message,wParam,lParam);
			}
			if(wit->second->mOldWinProc && wit->second->mCallOldProc)
				return CallWindowProc(wit->second->mOldWinProc,hWnd,message,wParam,lParam);
		}
		return DefWindowProc(hWnd,message,wParam,lParam);
	}

	INT_PTR CALLBACK SWindow::DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		std::map< HWND, SWindow* >::iterator wit=mWindows.find(hWnd);
		if(wit!=mWindows.end())
		{
			//Then any other messages
			std::map< u32, SWindowFunctorBase* >::iterator it=wit->second->mMessages.find(message);
			if(it!=wit->second->mMessages.end())
			{
				return (*(it->second))(message,wParam,lParam);
			}
		}
		return 0;
	}
}
