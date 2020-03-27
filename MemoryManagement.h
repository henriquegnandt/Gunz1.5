#include "Mutex.h"
class MemoryManagement
{
public:
	MemoryManagement(HMODULE baseAddress, unsigned int addressSize)
	{
#ifndef DEBUG
		_mutex = new Mutex();
#endif;
		if (baseAddress > 0 && addressSize > 0)
		{
			this->m_hBaseAddress = baseAddress;
			this->m_nAddressSize = addressSize;
			this->m_pMemory = new BYTE[addressSize];
			memcpy(this->m_pMemory, (LPVOID)baseAddress, addressSize);
			mlog ("Set buffer. Address(%X). Size(%X)\n", baseAddress, addressSize);
		}
		else
		{
			mlog ("Failed to set buffer. Address(%X). Size(%X)\n", baseAddress, addressSize);
		}
	}


	bool MemoryEdited()
	{
#ifndef DEBUG
		_mutex->Aquire();
#endif
		bool bRet = memcmp(this->m_pMemory, (LPVOID)this->m_hBaseAddress, this->m_nAddressSize);

#ifndef DEBUG
		_mutex->Release();
#endif
		return bRet;
	}

	void ModifyBuffer(DWORD address, PBYTE value, unsigned int valueSize)
	{
#ifndef DEBUG
		_mutex->Aquire();
#endif

		DWORD bytesWrote = 0;
		WriteProcessMemory(GetCurrentProcess(), (LPVOID)address, value, valueSize, &bytesWrote);

		//This should fix our RVA to array issue.
		address -= (unsigned int)this->m_hBaseAddress;
		
		for(unsigned int i = address, j = 0; i < (address + valueSize); ++i, j++)
		{
			this->m_pMemory[i] = value[j];
		}

#ifndef DEBUG
		_mutex->Release();
#endif		
	}


private:
	PBYTE m_pMemory;	
	HMODULE m_hBaseAddress;
	unsigned int m_nAddressSize;
#ifndef DEBUG
	Mutex* _mutex;
#endif
};