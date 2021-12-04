#include "pubfunc.h"

bool CheckFullscreen()
{
    bool bFullScreen = false;
    HWND hWnd = GetForegroundWindow();
    if (hWnd != GetDesktopWindow() && hWnd != GetShellWindow())
    {
        RECT rcApp, rcDesk;
        GetWindowRect(GetDesktopWindow(), &rcDesk);
        GetWindowRect(hWnd, &rcApp);
        if (rcApp.left <= rcDesk.left
            && rcApp.top <= rcDesk.top
            && rcApp.right >= rcDesk.right
            && rcApp.bottom >= rcDesk.bottom)
        {
            bFullScreen = true;
        }
        if (bFullScreen){
            TCHAR cls[128] = { 0 };
            GetClassName(hWnd, cls, _countof(cls));
            if (_tcscmp(cls, _T("WorkerW")) == 0){
                bFullScreen = false;
            }
        }
    }
    return bFullScreen;
}
