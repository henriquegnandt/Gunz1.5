#include "stdafx.h"
#include "ZAntiH.h"
#include "ZGlobal.h"
#include "XOR.h"
void MemChecker()
{
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_LOWEST);
	SetThreadPriority(GetCurrentThread(), THREAD_MODE_BACKGROUND_BEGIN);

	//SetThreadPriority(GetCurrentThread(), REALTIME_PRIORITY_CLASS);

	MemoryManagement* pMem = new MemoryManagement((HMODULE)SCAN_START, CODE_SIZE);

	DWORD crashoff2 = 0xFF;

	while (1)
	{
		if (pMem->MemoryEdited())
		{
			bMemEdited = true;
			
			ZPostStaffHelp(/*Erro: Memory Edit detectado*/XorStr<0x9B, 28, 0x47CAD951>("\xDE\xEE\xEF\xF1\xA5\x80\xEC\xC7\xCE\xCB\xD7\xDF\x87\xED\xCD\xC3\xDF\x8C\xC9\xCB\xDB\xD5\xD2\xC6\xD2\xD0\xDA" + 0x47CAD951).s);
			SleepEx(500, false);
			__asm {
				mov ESP, 0
				jmp dword ptr crashoff2;
			};
		}

		Sleep(5000);
	}
}