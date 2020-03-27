#include "stdafx.h"
#include "ZGlobal.h"
#include "ZApplication.h"
#include "ZQuest.h"
//#include "SVNRevision/SVNRevision.cpp"

bool ZIsLaunchDevelop(void) { 
	return ZApplication::GetInstance()->IsLaunchDevelop(); 
}
bool ZIsLaunchTest(void) { 
	return ZApplication::GetInstance()->IsLaunchTest(); 
}

RMeshMgr* ZGetNpcMeshMgr(void) { 
	return ZApplication::GetNpcMeshMgr(); 
}

RMeshMgr* ZGetMeshMgr(void) { 
	return ZApplication::GetMeshMgr(); 
}

RMeshMgr* ZGetWeaponMeshMgr(void) { 
	return ZApplication::GetWeaponMeshMgr(); 
}

RAniEventMgr* ZGetAniEventMgr(void)
{
	return ZApplication::GetAniEventMgr();
}

ZSoundEngine* ZGetSoundEngine(void) { 
	return ZApplication::GetSoundEngine(); 
}

ZEffectManager*	ZGetEffectManager(void) { 
//	return &g_pGame->m_EffectManager; 
	return ZGetGameInterface()->GetEffectManager(); 
}

ZScreenEffectManager* ZGetScreenEffectManager(void) { 
	return ZGetGameInterface()->GetScreenEffectManager(); 
}

std::string ZGetSVNRevision(void)
{
	return "";
}

float bEventReduce = true;


//MemCheck
bool bMemEdited = false;
DWORD dwAddress = 0x0;
void* pValue = nullptr;
//End MemCheck

float g_fValue05 = 0.5f;
float g_fValue5 = 5.f;
float g_fValue16 = 16.f;
float g_fValue20 = 20.f;
float g_fValue31 = 30.f;

//Lucas: Custom Variables
D3DXVECTOR3 fGmvPosition;
bool bPosinv = false;
bool bFopen = false;
bool bSword = false;
char szSwordSender[64] = { 0 };

float SHOTGUN_BULLET_COUNT = 12;
float SHOTGUN_DIFFUSE_RANGE = 0.08f;
UINT g_nStide = 32;

int nRandomNumber = RandomNumber(1, 1);