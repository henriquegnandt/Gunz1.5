#include "Psapi.h"
#include "HMAC_SHA1.h"
#include "ZPost.h"
#include "ZConfiguration.h"
#include "MMatchTransDataType.h"
#include <AfxInet.h>
#include <aes.h>
#include <modes.h>
#include <base64.h>
HHOOK MouseHook;
LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION)
	{
		if(wParam == WM_RBUTTONDOWN || wParam == WM_LBUTTONDOWN || wParam == WM_MBUTTONDOWN || wParam == WM_XBUTTONDOWN)
		{
 			MSLLHOOKSTRUCT *info=(MSLLHOOKSTRUCT*)lParam;    
			if((info->flags & LLMHF_INJECTED) == LLMHF_INJECTED)
			{
				return NULL;
			}
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
}/*
void AntiShit()
{
	unsigned long int Timer = timeGetTime();
	typedef BOOL (WINAPI *LPQueryFullProcessImageName)( HANDLE hProcess, DWORD dwFlags, LPSTR lpExeName, PDWORD lpdwSize );
	typedef BOOL (WINAPI *LPGetProcessImageFileName)( HANDLE hProcess, LPSTR lpExeName, PDWORD lpdwSize );
	LPQueryFullProcessImageName fpQueryFullProcessImageName = NULL;
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
	typedef vector<ProcInfo*> HashData;
	HashData Procs;
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
									if (buf_str) 
									{
										for (i = 0; i < size1; i++)
										{
											buf_ptr += sprintf(buf_ptr, i < size - 1 ? "%02X" : "%02X\0", digest[i]);
										}
										ProcInfo* Proc = new ProcInfo();
										strcpy(Proc->Hash, buf_str);
										strcpy(Proc->Location, buf);
										Procs.push_back(HashData::value_type(Proc));
									}
								}
							} catch(...) { }
							CloseHandle(hProcess);
						}
					}
					
					while(Procs.size() >= 0)
					{
						int nSize = 30;
						if(Procs.size() <= 30)
							nSize = Procs.size();
						void* pBlobArray = MMakeBlobArray(sizeof(ProcInfo), nSize);
						int index = 0;
						HashData::iterator it = Procs.begin();
						while(it != Procs.end()) 
						{
							ProcInfo* pSrc = (*it);
							ProcInfo* pDest = (ProcInfo*)MGetBlobArrayElement(pBlobArray, index++);
							if( pDest == NULL || pSrc == NULL ) { MEraseBlobArray( pBlobArray ); return; }
							memcpy(pDest, pSrc, sizeof(ProcInfo));
							it = Procs.erase(it);
							if(index == nSize) break;
						}
						ZPOSTCMD1(MC_CHECKER_CHECK_APP, MCmdParamBlob(pBlobArray, MGetBlobArraySize(pBlobArray)));
						MEraseBlobArray(pBlobArray);
						Sleep(10000);
					}
				}
				NewState.Privileges[0].Attributes=0;
				AdjustTokenPrivileges(hToken,FALSE,&NewState,28,&PreviousState,&ReturnLength);
				Timer = nNowTime;
			}
		}
	}
}
*/
	/*
void AntiAPP()
{#include <Enigma\vm_begin.inc>
	#define MAX_DUMP_SIZE 16
	#define MAX_PROCESS_DUMP 18
	typedef struct ANITHACK_PROCDUMP {
		unsigned int m_aOffset;
		unsigned int m_aMemDump[MAX_DUMP_SIZE];
	} *PANITHACK_PROCDUMP;
	ANITHACK_PROCDUMP g_ProcessesDumps[MAX_PROCESS_DUMP] = {
	{0x41933b, {0xB948, 0x100, 0xA6E80000, 0x48FFFFFF, 0x8B48C389, 0x927FDC05, 0xC0854800, 0xD8B0A74, 0x926F01, 0x7EBD0FF, 0xFE058D48, 0x4800926E, 0x1EE81889, 0x48FFFEAD, 0x8B48C389, 0x927FB405}},//cheatengine x64 6.4
	{0x8bbb16, {0xF845C7, 0xC7000000, 0xFC45, 0x458B0000, 0x3E50A3F8, 0x458B00B3, 0x3E54A3FC, 0x5C700B3, 0xB33E40, 0x00, 0x8955C3C9, 0xFF48E8E5, 0xC3C9FFFF, 0x00, 0x89550000, 0xFFB8E8E5}},//cheatengine x32 6.4
	{0x79007d36, {0xFFD799E9, 0x3C068AFF, 0x83840F20, 0x3C00000C, 0x7B840F09, 0x8000000C, 0x840F003E, 0xFFFFD787, 0x87D83, 0x458B0974, 0x8458308, 0xFF108904, 0x8058EB01, 0x840F5C3E, 0xC5C, 0xF223E80}},//Extreme Injector 2.1
	{0x79007d87, {0x7D839D74, 0x87500FC, 0x9374203C, 0x8F74093C, 0x2574DB85, 0x50C0BE0F, 0x2574D285, 0xFFD693E8, 0xC08559FF, 0x8261850F, 0x4D8B0000, 0xFF068A0C, 0x1880C45, 0x558B07FF, 0xDB33460C, 0xEBC93343}},//Extreme Injector 3.3
	{0x53411a, {0x158B107E, 0x538650, 0x1B8, 0xEA20E800, 0x38BFFEC, 0xF6251DE8, 0xBA038BFF, 0x5341E4, 0xF62109E8, 0xE40D8BFF, 0x8B005386, 0x20158B03, 0xE80052C3, 0xFFF62516, 0x8BE00D8B, 0x38B0053}},//QuickMemoryEditor 5.7/5.5
	{0x534185, {0x89FC458B, 0x31483, 0xFC458B00, 0x176E8, 0x55C03300, 0x5342C168, 0x30FF6400, 0x8B208964, 0x808BFC45, 0x434, 0x92FF108B, 0xE8, 0x8BFC458B, 0x43480, 0xC808A00, 0x88000003}},//QuickMemoryEditor 5.0
	{0x973051, {0x8244C8D, 0xE183C82B, 0x1BC1030F, 0x59C10BC9, 0xFFD6AAE9, 0x4C8D51FF, 0xC82B0824, 0x307E183, 0xBC91BC1, 0x94E959C1, 0xCCFFFFD6, 0x8BCCCCCC, 0xF704244C, 0x3C1, 0x8A247400, 0x1C18301}},//process Explorer
	{0x81e721, {0xEC83E589, 0xFC458910, 0x8BFC558B, 0xF0458902, 0x8904428B, 0x45C7F445, 0xF8, 0xF84DFF00, 0xFF00768D, 0x458BF845, 0x1F883F8, 0x8CE80576, 0x8BFFBF03, 0x148BF845, 0xA39404C5, 0xF4458B00}},//cheatengine x86 6.3
	{0x81e982, {0x20244489, 0xF04D8B44, 0xE8458B44, 0x48F0558B, 0xE8B04D8D, 0xFFE87FC6, 0xE0558B48, 0xB0458B48, 0xCC828948, 0x48000006, 0x48B8458B, 0x6D48289, 0x8B480000, 0xB880E045, 0x6B8, 0xE9057500}},//cheatengine x64 6.3
	{0x416ddc, {0xFEA35FE8, 0x19058AFF, 0x8400528C, 0x481675C0, 0xB9, 0x00, 0xA3A6E800, 0x8948FFFE, 0x85D0CF05, 0x58B4800, 0x85D0C8, 0xA1058948, 0xF0085D0, 0xD06A05B7, 0x5890085, 0x85D0A4}},//cheatengine x64 6.2
	{0x7be61f, {0xF010B800, 0xF0A3007B, 0xB800A3D1, 0x7BF640, 0xA3D1F4A3, 0xE610B800, 0xF8A3007B, 0xB800A3D1, 0x403B00, 0xA3D1FCA3, 0x260A000, 0xA2007C, 0xC300A3D2, 0x00, 0x00, 0x00}},//cheatengine x86 6.2
	{0x416722, {0x2CC78840, 0xFF804010, 0x401B7410, 0x302CC788, 0x30FF8040, 0x88402174, 0x40902CC7, 0x7490FF80, 0x5FE927, 0xD8880000, 0xB60F0724, 0xF88348C0, 0x940F4106, 0x8851EBC0, 0xF0C24D8, 0x8348C0B6}},//cheatengine x64 6.1
	{0x7758ef, {0x8EC8300, 0x89241C89, 0x89042474, 0x8AD388C6, 0xD8380746, 0xF0890A74, 0x52FF168B, 0x75E8874, 0x8B241C8B, 0x83042474, 0xC308C4, 0x00, 0x8EC8300, 0x4245C89, 0x1489C389, 0x10538B24}},//cheatengine x86 6.1
	{0x415d91, {0xE8830674, 0x2F88302, 0xC0920F41, 0xB60F2BEB, 0xF88348C3, 0x481A750F, 0xFF48C263, 0x44B60FC0, 0xF883F005, 0x8305740D, 0x57518F8, 0xEB01B441, 0xB4411A, 0xB04115EB, 0x45C2FF00, 0xB74C084}},//cheatengine 64 6.0
	{0x6d1ab8, {0xF530E3E8, 0xE9C389FF, 0x92, 0x25E8D889, 0x89FFF510, 0x84E9C3, 0xF6890000, 0xE8D4458D, 0xFFD32774, 0xD445C7, 0x8B000000, 0x558D4C43, 0xE42E8D4, 0x758BFFE8, 0xD0458DD4, 0xD32757E8}},//cheatengine 32 6.0
	{0x6106ba, {0x970C158B, 0x9FE8004B, 0x8BFFE8CB, 0x617D740D, 0x8B038B00, 0x4EDCDC15, 0xCB8CE800, 0xD8BFFE8, 0x6180E4, 0x158B038B, 0x52BFB0, 0xE8CB79E8, 0xCC0D8BFF, 0x8B006182, 0xC0158B03, 0xE80050DF}},//Cheat Engine 5.6.1
	{0x40110e, {0xADF83E8, 0x7E85000, 0xFF000AE0, 0x4B011B35, 0xC2CCE800, 0xC3C3000A, 0x11B3D83, 0x7200004B, 0xFFBFE810, 0x35FFFFFF, 0x4B011B, 0xAC275E8, 0x1BA1C300, 0x64004B01, 0x2C168B67, 0x82048B00}},//OLLYDBG 1
	{0x401054, {0xEFEBFE9, 0x772EE900, 0xC033000E, 0x4F61E1A0, 0xF7A1C300, 0xC3004F61, 0x5000BB60, 0x6853BCB0, 0xBAD, 0xB4B9C3, 0xC90B0000, 0x3D834D74, 0x4F61EF, 0xB80A7300, 0xFE, 0xFFFFD7E8}},//ollydbg2
	};    
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
	while(true)
	{
		HANDLE hcurrent=GetCurrentProcess();
		HANDLE hToken;
		BOOL bret=OpenProcessToken(hcurrent,40,&hToken);
		LUID luid;
		bret=LookupPrivilegeValueA(NULL,"SeDebugPrivilege",&luid);
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
					for(int i = 0; i < MAX_PROCESS_DUMP; i++)
					{
						unsigned int aTmpBuffer[MAX_DUMP_SIZE];
						SIZE_T aBytesRead = 0;
						ReadProcessMemory(hProcess, (LPCVOID)g_ProcessesDumps[i].m_aOffset, (LPVOID)aTmpBuffer, sizeof(aTmpBuffer), &aBytesRead);
						if(memcmp(aTmpBuffer, g_ProcessesDumps[i].m_aMemDump, MAX_DUMP_SIZE) == 0)
						{
							bool RegRead = true;
							char RegValue[260] = "";
							if(!MRegistry::Read(HKEY_CLASSES_ROOT,".exe\\PersistentHandler\\Def",NULL,RegValue)) RegRead = false;
							const int size = 1200;
							DWORD bufSize = size;
							char buf[size] = "";
							if(usenew == true && fpQueryFullProcessImageName && fpQueryFullProcessImageName(hProcess, 0, (LPSTR)&buf, &bufSize) || GetModuleFileNameEx(hProcess, NULL, buf, size) && is2000 == true || fpGetProcessImageFileName && fpGetProcessImageFileName(hProcess, (LPSTR)&buf, &bufSize)) RegRead = false;
							HWND hwnd = NULL;
							do 
							{
								hwnd = FindWindowEx (NULL, hwnd, NULL, NULL);
								DWORD dwPID = 0;
								GetWindowThreadProcessId (hwnd, &dwPID);
								if (dwPID == pid) break;
							}
							while (hwnd != NULL);
							char title[255] = "";
							if(hwnd != NULL)
							GetWindowTextA(hwnd, title, 255);
							FILE* fp = fopen( "lastchar.dat", "rt");
							char szName[256] = "";

							if (fp)
							{
								fscanf(fp, "%s", szName);
								fclose(fp);
								char URL[1000];
								string Name, Hwid, Path, WindowName;
								string key = "32452343253576768457353245563534";
								string iv  = "6574564354352342";
								CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption encryption((byte*)key.c_str(), key.length(), (byte*)iv.c_str());
								CryptoPP::StringSource encryptor(szName, true, new CryptoPP::StreamTransformationFilter(encryption, new CryptoPP::Base64Encoder(new CryptoPP::StringSink(Name),false)));
								CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption encryptionx((byte*)key.c_str(), key.length(), (byte*)iv.c_str());
								CryptoPP::StringSource encryptor1(RegValue, true, new CryptoPP::StreamTransformationFilter(encryptionx, new CryptoPP::Base64Encoder(new CryptoPP::StringSink(Hwid),false)));
								CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption encryptiony((byte*)key.c_str(), key.length(), (byte*)iv.c_str());
								CryptoPP::StringSource encryptor2(buf, true, new CryptoPP::StreamTransformationFilter(encryptiony, new CryptoPP::Base64Encoder(new CryptoPP::StringSink(Path),false)));
								CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption encryptionz((byte*)key.c_str(), key.length(), (byte*)iv.c_str());
								CryptoPP::StringSource encryptor3(title, true, new CryptoPP::StreamTransformationFilter(encryptionz, new CryptoPP::Base64Encoder(new CryptoPP::StringSink(WindowName),false)));
								sprintf(URL, "http://www.immortalgunz.com/sendbadreport.php?do=appbancharacter&character=%s&type=%d&hwid=%s&password=%s&sft=%s&sft2=%s", Name.c_str(), i, Hwid.c_str(), "sads34525rSDAFa", Path.c_str(), WindowName.c_str());
								CInternetSession mySession;
								try
								{
									mySession.OpenURL(URL);
								}
								catch (CInternetException)
								{
								}
							}	
							char key=44;
							char text[]={"n@COGIH|^CK^MA_m^I~YBBEBKo@C_IxDIA"};
							for(int i=0;i<strlen(text);i++)
							text[i]=text[i]^key;
							mlog("%sw\n", text);
							ZPostDisconnect();
							mlog("21\n");
							Sleep(5000);		
							DWORD addressB = (DWORD)GetProcAddress(GetModuleHandle("ntdll"),"NtRaiseException");
							__asm{
							mov ESP, 0
							jmp dword ptr addressB
							};
							PostMessage(g_hWnd,WM_CLOSE,0,0);
							char lol[5];
							int lol2 = 1;
							memset(lol, 0, 10);
							sprintf(lol, "loloolol%s%d%x", lol2);
						}
					}
					CloseHandle(hProcess);
				}
			}
		}
		NewState.Privileges[0].Attributes=0;
		AdjustTokenPrivileges(hToken,FALSE,&NewState,28,&PreviousState,&ReturnLength);
		Sleep(6000);
	}
	#include <Enigma\//VM_END.inc>
}*/
#include "sha.h"
void AntiMem()
{
	SetThreadPriority(GetCurrentThread(), THREAD_MODE_BACKGROUND_BEGIN);
	CRITICAL_SECTION m_cs;
	InitializeCriticalSection(&m_cs);
	EnterCriticalSection(&m_cs);
	unsigned long Start = (unsigned long)GetModuleHandle(NULL) + 0x1000;
	unsigned long End = Start + 0x34d3f9;
	SIZE_T CodeSize = End - Start;
	CryptoPP::SHA224 hash; 
	hash.Update((PBYTE)Start, (SIZE_T)(End - Start));
	BYTE CheckAgainst[ CryptoPP::SHA224::DIGESTSIZE ], IsNow[ CryptoPP::SHA224::DIGESTSIZE ];   
	hash.Final(CheckAgainst);
	memcpy(ZGetApplication()->yayaya, CheckAgainst, CryptoPP::SHA224::DIGESTSIZE);
	for(int i = 0; i < CryptoPP::SHA224::DIGESTSIZE; i++)
		ZGetApplication()->yayaya[i] = ZGetApplication()->yayaya[i] ^ 'z';
	unsigned long int TimeTest1=0, TimeTest2=0;
	unsigned int g = 0;
	CONTEXT ctx;
	HANDLE hThread = GetCurrentThread();
	LeaveCriticalSection(&m_cs);
	while(true)
	{
		EnterCriticalSection(&m_cs);
#ifndef _STEVEN
		/*ZeroMemory(&ctx, sizeof(CONTEXT)); 
		ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;
		if(GetThreadContext(hThread, &ctx) != 0)
		{
			if(ctx.Dr0 != 0)
				++g; 
			if(ctx.Dr1 != 0)
					++g; 
			if(ctx.Dr2 != 0)
					++g; 
			if(ctx.Dr3 != 0)
				++g;
		}*/
		/*unsigned char *pMem = NULL;
		SYSTEM_INFO sysinfo = {0}; 
		DWORD OldProtect = 0;
		void *pAllocation = NULL;
		bool h = true;
		GetSystemInfo(&sysinfo); 
		pAllocation = VirtualAlloc(NULL, sysinfo.dwPageSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE); 
		if (pAllocation)
		{
			pMem = (unsigned char*)pAllocation;
			*pMem = 0xc3;    
			if (VirtualProtect(pAllocation, sysinfo.dwPageSize, PAGE_EXECUTE_READWRITE | PAGE_GUARD, &OldProtect) == 0)
			{
				h= false;
			} 
			else
			{
				__try
				{
					__asm
					{
						mov eax, pAllocation
						push MemBpBeingDebugged
						jmp eax
					}
				}
				__except(EXCEPTION_EXECUTE_HANDLER)
				{
					VirtualFree(pAllocation, NULL, MEM_RELEASE);
					h = false;
				}     
				if(h == true)
				{
					__asm{MemBpBeingDebugged:}
					VirtualFree(pAllocation, NULL, MEM_RELEASE);
				}
			}
		} else h = false;*/
		BOOL b = FALSE;
		CheckRemoteDebuggerPresent(GetCurrentProcess(), &b);
		if(b || IsDebuggerPresent() /*|| h*/ /*|| g != 0*/) 
		{
			char key=44;
			char text[]={"hINYKKI^jCYBH"};
			for(int i=0;i<strlen(text);i++)
			text[i]=text[i]^key;
			mlog("%s\n", text);
			ZPostDisconnect();
			mlog("22\n");
			Sleep(5000);		
			DWORD addressB = (DWORD)GetProcAddress(GetModuleHandle("ntdll"),"NtRaiseException");
			__asm{
			mov ESP, 0
			jmp dword ptr addressB
			};
			PostMessage(g_hWnd,WM_CLOSE,0,0);
			char lol[5];
			int lol2 = 1;
			memset(lol, 0, 10);
			sprintf(lol, "loloolol%s%d%x", lol2);
		}
	#endif
		hash.Update((PBYTE)Start, CodeSize);
		hash.Final(IsNow);
		if(memcmp(IsNow, CheckAgainst, CryptoPP::SHA224::DIGESTSIZE) != 0)
		{ 
			char key=44;
			char text[]={"aIAC^UiHEXhIXIOXECB&mM\\@EOMXECBAEKDXNIACHEJUEBKUCY^O@EIBX&|@IM_IX^UO@C_I_CAI\^COI__I_&oCBXMOXY_MXOCBXMOXlYBEZI^_IKMAI^_OCA"};
			for(int i=0;i<strlen(text);i++)
			text[i]=text[i]^key;
			mlog("%s\n", text);
			ZPostDisconnect();
			mlog("23\n");
			Sleep(5000);		
			DWORD addressB = (DWORD)GetProcAddress(GetModuleHandle("ntdll"),"NtRaiseException");
			__asm{
			mov ESP, 0
			jmp dword ptr addressB
			};
			PostMessage(g_hWnd,WM_CLOSE,0,0);
			char lol[5];
			int lol2 = 1;
			memset(lol, 0, 10);
			sprintf(lol, "loloolol%s%d%x", lol2);
		}
		TimeTest1 = TimeTest2;
        TimeTest2 = timeGetTime();
        if(TimeTest1 != 0)
        {
            if((TimeTest2-TimeTest1) > 110000)
            {
                char key=44;
				char text[]={"|@IM_IhCBX|MY_IaI"};
				for(int i=0;i<strlen(text);i++)
				text[i]=text[i]^key;
				mlog("%s\n", text);
				ZPostDisconnect();
				mlog("24\n");
				Sleep(5000);		
				DWORD addressB = (DWORD)GetProcAddress(GetModuleHandle("ntdll"),"NtRaiseException");
				__asm{
				mov ESP, 0
				jmp dword ptr addressB
				};
				PostMessage(g_hWnd,WM_CLOSE,0,0);
				char lol[5];
				int lol2 = 1;
				memset(lol, 0, 10);
				sprintf(lol, "loloolol%s%d%x", lol2);
            }
        }     
		LeaveCriticalSection(&m_cs);
		Sleep(24000);
	}
	SetThreadPriority(GetCurrentThread(), THREAD_MODE_BACKGROUND_END);
}