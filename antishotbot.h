#include "StdAfx.h"
#include "Psapi.h"
#include "HMAC_SHA1.h"
#include "ZPost.h"
HHOOK MouseHook;

LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
if (nCode == HC_ACTION)
	{
		if(wParam == WM_RBUTTONDOWN || wParam == WM_LBUTTONDOWN || wParam == WM_MBUTTONDOWN || wParam == WM_XBUTTONDOWN)
		{
 			MSLLHOOKSTRUCT *info=(MSLLHOOKSTRUCT*)lParam;    
			if((info->flags & LLMHF_INJECTED) == LLMHF_INJECTED)
				ExitProcess(-1);
		}
	}
	return CallNextHookEx(MouseHook,nCode,wParam,lParam);
}

void AntiSB()
{
    HINSTANCE hInstance = GetModuleHandle(NULL);
    MouseHook = SetWindowsHookEx( WH_MOUSE_LL, MouseHookProc, hInstance, NULL );
    MSG message;
    while (GetMessage(&message,NULL,0,0)) {
        TranslateMessage( &message );
        DispatchMessage( &message );
    }
    UnhookWindowsHookEx(MouseHook);
    return;
}
/*
void AntiShit()
{
	VM_START
	unsigned long int Timer = timeGetTime();
	typedef BOOL (WINAPI *LPQueryFullProcessImageName)(
     HANDLE hProcess, DWORD dwFlags, LPSTR lpExeName, PDWORD lpdwSize );
	LPQueryFullProcessImageName fpQueryFullProcessImageName = NULL;
	typedef BOOL (WINAPI *LPGetProcessImageFileName)(
     HANDLE hProcess, LPSTR lpExeName, PDWORD lpdwSize );
	LPGetProcessImageFileName fpGetProcessImageFileName = NULL;
	DWORD version = GetVersion();
	DWORD major = (DWORD) (LOBYTE(LOWORD(version)));
	DWORD minor = (DWORD) (HIBYTE(LOWORD(version)));
	bool usenew = (major > 6) || (major == 6) || ((major == 6) && (minor >= 1));
	bool is2000 = (major == 5);
	if(usenew == true)
	{
		HMODULE hDLL = LoadLibrary("kernel32.dll");
		if (hDLL)
		fpQueryFullProcessImageName = (LPQueryFullProcessImageName)GetProcAddress(hDLL, "QueryFullProcessImageNameA");
	} 
	else if(is2000 == true)
	{		
		HMODULE hDLL = LoadLibrary("kernel32.dll");
		if (hDLL)
		fpGetProcessImageFileName = (LPGetProcessImageFileName)GetProcAddress(hDLL, "GetProcessImageFileNameA");
	}
	int TimeLimit = 60000;//RandomNumber(4, 14) * 60000;
	while (true)
	{	
		Sleep(1000);
		if(ZGetGame() && ZGetGame()->m_pMyCharacter)
		{
			unsigned long int nNowTime = timeGetTime();
			if ((nNowTime-Timer) >= TimeLimit) 
			{
				//TimeLimit = RandomNumber(4, 14) * 60000;
				HANDLE hcurrent=GetCurrentProcess();
				HANDLE hToken;
				BOOL bret=OpenProcessToken(hcurrent,40,&hToken);
				LUID luid;
				bret=LookupPrivilegeValue(NULL,"SeDebugPrivilege",&luid);
				TOKEN_PRIVILEGES NewState,PreviousState;
				DWORD ReturnLength;
				NewState.PrivilegeCount =1;
				NewState.Privileges[0].Luid =luid;
				NewState.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
				AdjustTokenPrivileges(hToken,FALSE,&NewState,28,&PreviousState,&ReturnLength);
				DWORD pids[1024] = {};
				DWORD bytesReturned = 0;
				if (EnumProcesses(pids, sizeof pids, &bytesReturned))
				{
					DWORD cProcesses = bytesReturned / sizeof *pids;
					for (DWORD i = 0; i < cProcesses; i++)
					{
						DWORD pid = pids[i];
						if (HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid))
						{
							const int size = 1200;
							DWORD bufSize = size;
							char buf[size];
							try 
							{
							if(usenew == true && fpQueryFullProcessImageName && fpQueryFullProcessImageName(hProcess, 0, (LPSTR)&buf, &bufSize) || GetModuleFileNameEx(hProcess, NULL, buf, size) && is2000 == true || fpGetProcessImageFileName && fpGetProcessImageFileName(hProcess, (LPSTR)&buf, &bufSize))
							{
								BYTE digest[20];
								BYTE Key[20];
								memset(Key, 0, 20);
								memcpy(Key, GUNZ_HMAC_KEY, strlen(GUNZ_HMAC_KEY));
								CHMAC_SHA1 HMAC_SHA1 ;
								HMAC_SHA1.HMAC_SHA1_file(buf, Key, GUNZ_HMAC_KEY_LENGTH, digest) ;
								int i, size1 = sizeof(digest) / sizeof(char);
								char *buf_str = (char*) malloc(3 * size1), *buf_ptr = buf_str;
								if (buf_str) {
									for (i = 0; i < size1; i++)
									buf_ptr += sprintf(buf_ptr, i < size - 1 ? "%02X" : "%02X\0", digest[i]);
									ZPOSTCMD2(MC_MATCH_REPORT_APP, MCommandParameterString(buf), MCommandParameterString(buf_str));
									Sleep(2000);
								}
							}
							} catch(...) { }
							CloseHandle(hProcess);
						}
					}
				}
				NewState.Privileges[0].Attributes=0;
				AdjustTokenPrivileges(hToken,FALSE,&NewState,28,&PreviousState,&ReturnLength);
				Timer = nNowTime;
			}
		}
	}
	VM_END
}*/