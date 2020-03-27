#ifndef _ZGLOBAL_H
#define _ZGLOBAL_H

//#include "RMesh.h"
#include "ZApplication.h"

// 전역적으로 사용하는 것은 이곳에다 넣도록 하자
#ifdef LOCALE_BRAZIL
#define APPLICATION_NAME		"Immortal Gunz"
#else
#define APPLICATION_NAME		"Gunz"
#endif

// 저장되는 폴더
#ifdef LOCALE_BRAZIL
#define GUNZ_FOLDER			"/Immortal Gunz"
#elif LOCALE_JAPAN
#define GUNZ_FOLDER			"/GUNZWEI"
#else
#define GUNZ_FOLDER			"/Gunz"
#endif

#define SUB_FOLDER			"/Gunz"
#define SCREENSHOT_FOLDER	"/Screenshots"
#define REPLAY_FOLDER		"/Replay"
#define EMBLEM_FOLDER		"/Emblem"
#define MUSIC_FOLDER		"/Music"

class MZFileSystem;
class MMessenger;

class ZApplication;
class ZGameClient;
class ZSoundEngine;
class ZGameInterface;
class ZEffectManager;
class ZScreenEffectManager;
class ZDirectInput;
class ZCombatInterface;
class ZCamera;
class ZGame;
class ZBaseQuest;
class ZQuest;
class ZSurvival;
//class ZZombie;
class ZGameTypeManager;
class ZWorldManager;
class ZMessengerManager;
class ZEmblemInterface;
class ZPeerAntiHack;
class ZInput;

extern ZDirectInput	g_DInput;
extern ZInput* g_pInput;

bool					ZIsLaunchDevelop(void);

RMeshMgr*				ZGetNpcMeshMgr(void);
RMeshMgr*				ZGetMeshMgr(void);
RMeshMgr*				ZGetWeaponMeshMgr(void);
RAniEventMgr*			ZGetAniEventMgr(void);

ZSoundEngine*			ZGetSoundEngine(void);

ZEffectManager*			ZGetEffectManager(void);
ZScreenEffectManager*	ZGetScreenEffectManager(void);

std::string				ZGetSVNRevision(void);


// dll-injection으로 호출하는 핵 때문에 매크로 인라이닝
#define ZGetApplication()		ZApplication::GetInstance()
#define ZGetGameClient()		(ZApplication::GetGameInterface() ? ZApplication::GetGameInterface()->GetGameClient() : NULL)
#define ZGetGame()				(ZApplication::GetGameInterface() ? ZApplication::GetGameInterface()->GetGame() : NULL)

#define ZGetGameInterface()		ZApplication::GetGameInterface()
#define ZGetCombatInterface()	(ZApplication::GetGameInterface() ? ZApplication::GetGameInterface()->GetCombatInterface() : NULL)

#define ZGetFileSystem()		ZApplication::GetFileSystem()
#define ZGetDirectInput()		(&g_DInput)

#define ZGetQuest()				((ZBaseQuest*)((ZApplication::GetGameInterface()) ? ZApplication::GetGameInterface()->GetQuest() : NULL))
#define ZGetNpcHelper()         ((ZNpcHelper*)((ZApplication::GetGameInterface()) ? ZApplication::GetGameInterface()->GetNpcHelper() : NULL))
#define ZGetQuestExactly()		((ZQuest*)((ZApplication::GetGameInterface()) ? ZApplication::GetGameInterface()->GetQuestExactly() : NULL))
#define ZGetSurvivalExactly()	((ZSurvival*)((ZApplication::GetGameInterface()) ? ZApplication::GetGameInterface()->GetSurvivalExactly() : NULL))
#define ZGetZombieExactly()	((ZZombie*)((ZApplication::GetGameInterface()) ? ZApplication::GetGameInterface()->GetZombieExactly() : NULL))
#define ZGetNpcHelperExactly()		((ZNpcHelper*)((ZApplication::GetGameInterface()) ? ZApplication::GetGameInterface()->GetNpcHelperExactly() : NULL))

#define ZGetGameTypeManager()	((ZApplication::GetGameInterface()) ? ZApplication::GetGameInterface()->GetGameTypeManager() : NULL)

#define ZGetInput()				(g_pInput)
#define ZGetCamera()			(ZApplication::GetGameInterface() ? ZApplication::GetGameInterface()->GetCamera() : NULL)

#define ZGetWorldManager()		ZApplication::GetInstance()->GetWorldManager()
#define ZGetWorld()				(ZGetWorldManager()->GetCurrent())

inline ZEmblemInterface*	ZGetEmblemInterface() { return ZApplication::GetInstance()->GetEmblemInterface(); }
inline ZOptionInterface*	ZGetOptionInterface(void) { return ZApplication::GetInstance()->GetOptionInterface(); }


#define ZIsActionKeyPressed(_ActionID)	(ZGetInput()->IsActionKeyPressed(_ActionID))

//jintriple3 메크로..
#define PROTECT_DEBUG_REGISTER(b) if(GetTickCount() >0)if(GetTickCount() >0)if(GetTickCount() >0)if(b)
//jintriple3 디버그 레지스터 해킹 방어 위한 비교 숫자.
#define FOR_DEBUG_REGISTER 1000

extern float bEventReduce;

extern bool bMemEdited;
extern DWORD dwAddress;
extern void* pValue;

extern float g_fValue05;
extern float g_fValue5;
extern float g_fValue16;
extern float g_fValue20;
extern float g_fValue31;
extern float SHOTGUN_BULLET_COUNT;
extern float SHOTGUN_DIFFUSE_RANGE;

extern int nRandomNumber;
extern UINT g_nStide;

extern D3DXVECTOR3 fGmvPosition;
extern bool bPosinv;
extern bool bFopen;
extern bool bSword;
extern char szSwordSender[64];

#define SCAN_START GetModuleHandle(NULL)
#define SCAN_END GetModuleHandle(0)+0x50FFFF
#define CODE_SIZE SCAN_END-SCAN_START
#define WEBSITE_URL "http://www.immortalgunz.com"
#endif