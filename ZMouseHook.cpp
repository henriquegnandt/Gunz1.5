#include "stdafx.h"
#include "ZMouseHook.h"

LRESULT CALLBACK ZMouseHook::MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam){
        
        PKBDLLHOOKSTRUCT k = (PKBDLLHOOKSTRUCT)(lParam);
        POINT p;
        int xMetric = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        
        xMetric -= 100;
        
        if(wParam == WM_RBUTTONDOWN)
        {      
        MessageBox(NULL,"Click!",NULL,MB_OK);
        }
        return CallNextHookEx(NULL,nCode,wParam,lParam);
 }

DWORD WINAPI ZMouseHook::StayAlive(LPVOID pJobContext)
{
    MSG message;
    while(GetMessage(&message,NULL,0,0)){
            TranslateMessage(&message);
            DispatchMessage(&message);
    }
	return 0;
}

void ZMouseHook::InstallHook( HINSTANCE a)
{
    MouseHook = SetWindowsHookEx(WH_MOUSE_LL,MouseHookProc,a,0);
	DWORD dwThreadId=0;
	HANDLE hThread = CreateThread(NULL, 0, StayAlive, this, 0, &dwThreadId);
}