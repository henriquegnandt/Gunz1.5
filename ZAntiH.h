#pragma once
#include "stdafx.h"
class MemoryManagement
{
public:
	MemoryManagement(HMODULE baseAddress, unsigned int addressSize)
	{
		if (baseAddress > 0 && addressSize > 0)
		{
			this->m_hBaseAddress = baseAddress;
			this->m_nAddressSize = addressSize;
			this->m_pMemory = new BYTE[addressSize];
			memcpy(this->m_pMemory, (LPVOID)baseAddress, addressSize);
		}
	}


	bool MemoryEdited()
	{
		bool bRet = memcmp(this->m_pMemory, (LPVOID)this->m_hBaseAddress, this->m_nAddressSize);

		return bRet;
	}


public:
	PBYTE m_pMemory;
	HMODULE m_hBaseAddress;
	unsigned int m_nAddressSize;
};

void MemChecker();