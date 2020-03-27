#include "StdAfx.h"
#include "Psapi.h"
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
struct LANGANDCODEPAGE1 
{
                  WORD wLanguage;
                  WORD wCodePage;
}*lpTranslate1;

char* GetVersionInfo(char* filename)
{
    char strTmp[512];
    DWORD handle;
    int infoSize = (int) GetFileVersionInfoSize(filename, &handle);
    if(infoSize == 0) return "ERROR";
    LPVOID pBlock;
    pBlock = new BYTE[infoSize];
    int bResult = GetFileVersionInfo(filename, handle, infoSize, pBlock);
    if(bResult == 0) return "ERROR";
    LPVOID         lpBuffer;
    UINT         dwBytes;
    int                  i, langNumber;
    bResult = VerQueryValue(pBlock, TEXT("\\VarFileInfo\\Translation"), (LPVOID*)&lpTranslate1, &dwBytes);
    if(bResult == 0)
    return "ERROR";
    langNumber = dwBytes/sizeof(struct LANGANDCODEPAGE1);
    if(langNumber != 1){
            sprintf(strTmp, "Error! Languages number: %d.", langNumber);
            return "ERROR";
    }
    HRESULT hr;
    for(i=0; i<langNumber; i++)
	{
	hr =  sprintf_s(strTmp, 512, TEXT("\\StringFileInfo\\%04x%04x\\FileDescription"),
    lpTranslate1[i].wLanguage, lpTranslate1[i].wCodePage);
    if (FAILED(hr))return "ERROR";
    bResult = VerQueryValue(pBlock, strTmp, &lpBuffer, &dwBytes);
    if(bResult == 0)return "ERROR";
    }
    return (char*)lpBuffer;
}

char* getProcessModule(DWORD pid) 
{
  HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
  if (hProcess) 
  {
	const int size = 1000;
	DWORD bufSize = size;
	char buf[size];
	if (QueryFullProcessImageNameA(hProcess, 0, (LPSTR)&buf, &bufSize)) 
	return buf;
  }
  CloseHandle(hProcess);
  return NULL;
}

void AntiShotBotterGeez()
{
	while(true)
	{
		Sleep(120000);
		 HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		 if (hSnapshot == HANDLE(-1)) return;
		 PROCESSENTRY32 pe;
		 pe.dwSize = sizeof(PROCESSENTRY32);
		 BOOL retval = Process32First(hSnapshot, &pe);
		 while (retval) {
			DWORD pid = pe.th32ProcessID;
			char* name = getProcessModule(pid);
			if(name != NULL)
			{
				char*Vername = GetVersionInfo(name);
				if(strstr(Vername, "#fuckit"))
				{
					printf("Found\n");
				}
			}
			pe.dwSize = sizeof(PROCESSENTRY32);
			retval = Process32Next(hSnapshot, &pe);
		 }
		 CloseHandle(hSnapshot);
	}
}*/