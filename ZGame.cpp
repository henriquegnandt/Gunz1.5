#include "stdafx.h"

#include "ZGame.h"
#include <windows.h> 
//#include <zmouse.h>

#include "MZFileSystem.h"
#include "RealSpace2.h"
#include "FileInfo.h"
#include "MDebug.h"
#include "MBlobArray.h"
#include "MObject.h"
#include "ZConsole.h"
#include "MCommandLogFrame.h"
#include "ZInterface.h"
#include "ZGameInterface.h"
#include "ZApplication.h"
#include "ZCommandTable.h"
#include "ZPost.h"
#include "ZPostLocal.h"
#include "MStrEx.h"
#include "MMatchItem.h"
#include "ZEffectManager.h"
#include "ZEffectBillboard.h"
#include "Config.h"
#include "MProfiler.h"
#include "MMatchItem.h"
#include "ZScreenEffectManager.h"
#include "RParticleSystem.h"
#include "RDynamicLight.h"
#include "ZConfiguration.h"
#include "ZLoading.h"
#include "Physics.h"
#include "zeffectflashbang.h"
#include "ZInitialLoading.h"
#include "RealSoundEffect.h"
#include "RLenzFlare.h"
#include "ZWorldItem.h"
#include "ZWorldObject.h"
#include "ZMyInfo.h"
#include "ZNetCharacter.h"
#include "ZSecurity.h"
#include "ZStencilLight.h"
#include "ZMap.h"
#include "ZEffectStaticMesh.h"
#include "ZEffectAniMesh.h"
#include "ZGameAction.h"
#include "ZSkyBox.h"
#include "ZFile.h"
#include "ZObject.h"
#include "ZCharacter.h"
#include "ZMapDesc.h"

#include "MXml.h"
#include "ZGameClient.h"
#include "MUtil.h"
#include "RMeshMgr.h"
#include "RVisualMeshMgr.h"
#include "RMaterialList.h"
#include "RAnimationMgr.h"
#include "ZCamera.h"
#include "Mint4R2.h"
//#include "RParticleSystem.h"
#include "ZItemDesc.h"

//#include "MObjectCharacter.h"
#include "MMath.h"
#include "ZQuest.h"
#include "MMatchUtil.h"
#include "ZReplay.h"
#include "ZRuleBerserker.h"
#include "ZRuleReverseBerserker.h"
#include "ZRuleDuelTournament.h"
#include "ZRuleBountyHunter.h"
#include "ZApplication.h"
#include "ZGameConst.h"

#include "ZRuleDuel.h"
#include "ZMyCharacter.h"
#include "MMatchCRC32XORCache.h"
#include "MMatchObjCache.h"

#include "ZModule_HealOverTime.h"
#include <sstream>
#include "XOR.h"

#ifdef LOCALE_NHNUSA
#include "ZNHN_USA_Report.h"
#endif


_USING_NAMESPACE_REALSPACE2

#define ENABLE_CHARACTER_COLLISION		// 캐릭터끼리 충돌체크 
#define ENABLE_ADJUST_MY_DATA			// 투표시스템 동작

/////////////////////////////////////////////////////////////////////
//채팅 문자열 검사 코드....줄바꿈 문자만 검사.....

//PlsEdit0
#define kernel32 /*kernel32.dll*/XorStr<0x6D,13,0x47D8CB0E>("\x06\x0B\x1D\x1E\x14\x1E\x40\x46\x5B\x12\x1B\x14"+0x47D8CB0E).s
#define queryperformancecounter /*QueryPerformanceCounter*/XorStr<0xFF,24,0x14AD6362>("\xAE\x75\x64\x70\x7A\x54\x60\x74\x61\x67\x7B\x67\x6A\x62\x6E\x6B\x4C\x7F\x64\x7C\x67\x71\x67"+0x14AD6362).s
#define gettickcount /*GetTickCount*/XorStr<0xAB,13,0xD309440B>("\xEC\xC9\xD9\xFA\xC6\xD3\xDA\xF1\xDC\xC1\xDB\xC2"+0xD309440B).s
#define winmm /*winmm.dll*/XorStr<0xD3,10,0x6FB792CF>("\xA4\xBD\xBB\xBB\xBA\xF6\xBD\xB6\xB7"+0x6FB792CF).s
#define timeGetTimeD /*timeGetTime*/XorStr<0xBD,12,0xCED438B2>("\xC9\xD7\xD2\xA5\x86\xA7\xB7\x90\xAC\xAB\xA2"+0xCED438B2).s
#define sleepE /*Sleep*/XorStr<0x08,6,0x18B007B9>("\x5B\x65\x6F\x6E\x7C"+0x18B007B9).s
#define GetCurrentThreadIdE /*GetCurrentThreadId*/XorStr<0x23,19,0xE8F83FF2>("\x64\x41\x51\x65\x52\x5A\x5B\x4F\x45\x58\x79\x46\x5D\x55\x50\x56\x7A\x50"+0xE8F83FF2).s
DWORD GetCurrentThreadId1 = (DWORD)GetProcAddress(GetModuleHandleA(kernel32), GetCurrentThreadIdE);
DWORD sleep1 = (DWORD)GetProcAddress(GetModuleHandleA(kernel32), sleepE);
DWORD getTickCount1 = (DWORD)GetProcAddress(GetModuleHandleA(kernel32), gettickcount);
DWORD queryPerformanceCounter1 = (DWORD)GetProcAddress(GetModuleHandleA(kernel32), queryperformancecounter);
DWORD timeGetTime1 = (DWORD)GetProcAddress(GetModuleHandleA(winmm), timeGetTimeD);
bool IsAddressHooked(unsigned long address)
{
	//#include <Enigma\vm_begin.inc>
    BYTE* offsetValue = (BYTE*)address;
    return (*offsetValue == 0xE8 || *offsetValue == 0xE9 || *offsetValue == 0x7E || *offsetValue == 0x74 || *offsetValue == 0xC3 /*|| *offsetValue == 0xFF*/);
	//#include <Enigma\//VM_END.inc>
}
void CheckMsgAboutChat(char * msg)
{
	//여기서 채팅용어 거르는부분.....
	int lenMsg = (int)strlen(msg);
	for( int i=0; i<lenMsg; i++)
	{
		if(msg[i] == '\n' || msg[i] == '\r')
		{
			msg[i] = NULL;
			break;
		}
	}
}

// Particle 테스트 ///////////////////////////

struct RSnowParticle : public RParticle , CMemPoolSm<RSnowParticle>
{

	virtual bool Update(float fTimeElapsed);
};


bool RSnowParticle::Update(float fTimeElapsed)
{
	RParticle::Update(fTimeElapsed);

	if (position.z <= -1000.0f) return false;
	return true;
}

class ZSnowTownParticleSystem
{
private:
	RParticles*		m_pParticles[3];
	bool IsSnowTownMap()
	{
		if (!strnicmp(ZGetGameClient()->GetMatchStageSetting()->GetMapName(), "snow", 4)) return true;

		return false;
	}
public:
	void Update(float fDeltaTime)
	{
		if (!IsSnowTownMap()) return;

#define SNOW_PARTICLE_COUNT_PER_SECOND		400		// 대략 초당 300개쯤 원래 프레임당 14개였음.

		int nSnowParticleCountPerSec = SNOW_PARTICLE_COUNT_PER_SECOND;
		switch (ZGetConfiguration()->GetVideo()->nEffectLevel)
		{
		case Z_VIDEO_EFFECT_HIGH:	break;
		case Z_VIDEO_EFFECT_NORMAL:	nSnowParticleCountPerSec = nSnowParticleCountPerSec / 4; break;
		case Z_VIDEO_EFFECT_LOW:	nSnowParticleCountPerSec = nSnowParticleCountPerSec / 8; break;
		default: nSnowParticleCountPerSec = 0; break;
		}

		int nCount = min(nSnowParticleCountPerSec * fDeltaTime,20);	// 한번에 20개 이상은 안나오도록한다
		for(int i=0;i<nCount;i++)
		{
			RParticle *pp=new RSnowParticle();
			pp->ftime=0;
			pp->position = rvector(RandomNumber(-8000.0f, 8000.0f), RandomNumber(-8000.0f, 8000.0f), 1500.0f);
			pp->velocity = rvector(RandomNumber(-40.0f, 40.0f), RandomNumber(-40.0f, 40.0f), RandomNumber(-150.0f, -250.0f));
			pp->accel=rvector(0,0,-5.f);

			int particle_index = RandomNumber(0, 2);
			if (m_pParticles[particle_index]) m_pParticles[particle_index]->push_back(pp);
		}
	}
	void Create()
	{
		if (!IsSnowTownMap()) return;

		for (int i = 0; i < 3; i++)
		{
			m_pParticles[i] = NULL;
		}

		// 파티클 생성
		m_pParticles[0] = RGetParticleSystem()->AddParticles("sfx/water_splash.bmp", 25.0f);
		m_pParticles[1] = RGetParticleSystem()->AddParticles("sfx/water_splash.bmp", 10.0f);
		m_pParticles[2] = RGetParticleSystem()->AddParticles("sfx/water_splash.bmp", 5.0f);
	}
	void Destroy()
	{
		if (!IsSnowTownMap()) return;

		m_pParticles[0]->Clear();
		m_pParticles[1]->Clear();
		m_pParticles[2]->Clear();
	}
};

static ZSnowTownParticleSystem g_SnowTownParticleSystem;
// Particle 테스트 ///////////////////////////



float	g_fFOV = DEFAULT_FOV;
float	g_fFarZ = DEFAULT_FAR_Z;


//RParticleSystem* g_ParticleSystem = 0;
extern sCharacterLight g_CharLightList[NUM_LIGHT_TYPE];

MUID tempUID(0, 0);		// 로컬 테스트용
MUID g_MyChrUID(0, 0);

#define IsKeyDown(key) ((GetAsyncKeyState(key) & 0x8000)!=0)

void TestCreateEffect(int nEffIndex)
{
	float fDist = RandomNumber(0.0f, 100.0f);
	rvector vTar = rvector(RandomNumber(0.0f, 100.0f), RandomNumber(0.0f, 100.0f), RandomNumber(0.0f, 100.0f));
	rvector vPos = ZGetGame()->m_pMyCharacter->GetPosition();
	vPos.x += RandomNumber(0.0f, 100.0f);
	vPos.y += RandomNumber(0.0f, 100.0f);
	vPos.z += RandomNumber(0.0f, 100.0f);

	rvector vTarNormal = -RealSpace2::RCameraDirection;

	vTarNormal = rvector(RandomNumber(0.0f, 100.0f), RandomNumber(0.0f, 100.0f), RandomNumber(0.0f, 100.0f));


	ZCharacter* pCharacter = ZGetGame()->m_pMyCharacter;
	ZEffectManager* pEM = ZGetEffectManager();

	switch (nEffIndex)
	{
	case 0:
		pEM->AddReBirthEffect(vPos);
		break;
	case 1:
		pEM->AddLandingEffect(vPos, vTarNormal);
		break;
	case 2:
		pEM->AddGrenadeEffect(vPos, vTarNormal);
		break;
	case 3:
		pEM->AddRocketEffect(vPos, vTarNormal);
		break;
	case 4:
		pEM->AddRocketSmokeEffect(vPos);
		break;
	case 5:
		pEM->AddSwordDefenceEffect(vPos,-vTarNormal);
		break;
	case 6:
		pEM->AddSwordWaveEffect(vPos, 200, pCharacter);
		break;
	case 7:
		pEM->AddSwordUppercutDamageEffect(vPos, pCharacter->GetUID());
		break;
	case 8:
		pEM->AddBulletMark(vPos, vTarNormal);
		break;
	case 9:
		pEM->AddShotgunEffect(vPos, vPos, vTar, pCharacter);
		break;
	case 10:
		//pEM->AddBloodEffect(vPos, vTarNormal);
		break;
	case 11:
		for (int i = 0; i < SEM_End; i++)
			pEM->AddSlashEffect(vPos, vTarNormal, i);
		break;
	case 12:
		pEM->AddSlashEffectWall(vPos, vTarNormal,0);
		break;
	case 13:
		pEM->AddLightFragment(vPos, vTarNormal);
		break;
	case 14:
		pEM->AddDashEffect(vPos, vTarNormal, pCharacter, 0);
		break;
	case 15:
		pEM->AddSmokeEffect(vPos);
		break;
	case 16:
		pEM->AddSmokeGrenadeEffect(vPos);
		break;
	case 17:
		pEM->AddGrenadeSmokeEffect(vPos, 1.0f, 0.5f, 123);
		break;
	case 18:
		pEM->AddWaterSplashEffect(vPos, vPos);
		break;
	case 19:
		pEM->AddWorldItemEatenEffect(vPos);
		break;
	case 20:
		pEM->AddCharacterIcon(pCharacter, 0);
		break;
	case 21:
		pEM->AddCommanderIcon(pCharacter, 0);
		break;
	case 22:
		pEM->AddChatIcon(pCharacter);
		break;
	case 23:
		pEM->AddLostConIcon(pCharacter);
		break;
	case 24:
		pEM->AddChargingEffect(pCharacter);
		break;
	case 25:
		pEM->AddChargedEffect(pCharacter);
		break;
	case 26:
		pEM->AddTrackFire(vPos);
		pEM->AddTrackFire(vPos);
		pEM->AddTrackFire(vPos);
		pEM->AddTrackFire(vPos);
		break;
	case 27:

		ZEffectWeaponEnchant* pEWE = pEM->GetWeaponEnchant(ZC_ENCHANT_FIRE);

		if(pEWE) {
			//표준 사이즈는 카타나... 100 정도..
			float fSIze = 105.f / 100.f;
			rvector vScale = rvector(0.6f*fSIze,0.6f*fSIze,0.9f*fSIze);// 무기의 크기에 따라서..
			pEWE->SetUid( pCharacter->GetUID() );
			pEWE->SetAlignType(1);
			pEWE->SetScale(vScale);
			pEWE->Draw(timeGetTime());
		}

		break;
	}
}

float ZGame::CalcActualDamage(ZObject* pAttacker, ZObject* pVictim, float fDamage, MMatchWeaponType WeaponType)
{
	if (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_BERSERKER)
	{
		ZRuleBerserker* pRule = (ZRuleBerserker*)ZGetGame()->GetMatch()->GetRule();
		if ((pAttacker) && (pAttacker != pVictim) && (pAttacker->GetUID() == pRule->GetBerserkerUID()))
		{
			return fDamage * BERSERKER_DAMAGE_RATIO;			
		}
	}

	if (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_REVERSE_BERSERKER)
	{
		ZRuleReverseBerserker* pRule = (ZRuleReverseBerserker*)ZGetGame()->GetMatch()->GetRule();
		if ((pAttacker) && (pAttacker != pVictim) && (pAttacker->GetUID() == pRule->GetBerserkerUID()))
		{
			return fDamage * BERSERKER_DAMAGE_RATIO;			
		}
	}

	if (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_GUNGAME)
	{
		return fDamage * 2.0f;
	}
	
	if (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_BOUNTY)
	{
		ZRuleBounty* pRule = (ZRuleBounty*)ZGetGame()->GetMatch()->GetRule();
		if ((pAttacker) && (pAttacker != pVictim) && (pVictim->GetUID() == pRule->GetBountyUID()))
		{
			return (fDamage / 4.0f);		
		}
	}
	return fDamage;
}

void TestCreateEffects()
{
	int nCount = 100;

	for (int i = 0; i < nCount; i++)
	{
		int nEffIndex = RandomNumber(25, 28);
		TestCreateEffect(nEffIndex);
	}
}
bool HappyWorking = true;
int filterException(int code, PEXCEPTION_POINTERS ex) {
    return EXCEPTION_EXECUTE_HANDLER;
}

DWORD ASFrtwsgtrWEfdtgrwsdfhfshgs;

DWORD WINAPI Worker(LPVOID lpStatus)
{
	ASFrtwsgtrWEfdtgrwsdfhfshgs = GetCurrentThreadId();
	//unsigned long int LastShotRetry = 0, LastDmgCounter = 0, LastSbCheck = 0, LastPingCheck = 0;
	unsigned long int nNowTime;
	typedef map<MUID, vector<MTD_SHOTINFO*>> ShotArray;
	typedef map<int, vector<MTD_SHOTINFO*>> RetryArray;
	typedef map<int, unsigned long int> RecvArray;
	while(HappyWorking == true)
	{
		Sleep(100);
		if(ZGetGame() && ZGetGame()->m_pMyCharacter && !ZGetGame()->IsReplay())
		{
		if (ZGetGame()->CheckGameReady() == false) continue;
			ZGetGame()->PostBasicInfo();
			if(ZGetGame()->GetMatch()->GetMatchType() != MMATCH_GAMETYPE_DUELTOURNAMENT)
				ZGetGame()->PostHPAPInfo();
			else
				ZGetGame()->PostDuelTournamentHPAPInfo(); // 듀얼토너먼트 에서만 처리해준다.

			ZGetGame()->PostPeerPingInfo();
			ZGetGame()->PostSyncReport();
			nNowTime = timeGetTime();
			if ((nNowTime-ZGetGame()->st_nLastTime[6]) >= 2000) 
			{
				if(ZGetGame()->RetryShots.size() > 0)
				{
					for(RetryArray::iterator it = ZGetGame()->RetryShots.begin(); it != ZGetGame()->RetryShots.end(); it++)
					{
						if((nNowTime - it->second[0]->ntime) >= 3000)
						{
							ZCharacter* pCharacter = ZGetCharacterManager()->Find(it->second[0]->MYID);
							if(!pCharacter || pCharacter && pCharacter->m_dwStatusBitPackingValue.Ref().m_bLostConEffect == true)
							{
								it = ZGetGame()->RetryShots.erase(it);
							} 
							else
							{				
								if(it->second[0]->Retrys >= 3)
								{
									it = ZGetGame()->RetryShots.erase(it);
								}
								else
								{
									MTD_SHOTINFO* shot = new MTD_SHOTINFO();
									shot->MYID = it->second[0]->MYID;
									shot->X = it->second[0]->X;
									shot->Y = it->second[0]->Y;
									shot->Z = it->second[0]->Z;
									shot->Size = it->second.size();
									shot->Parts = it->second[0]->Parts;
									shot->Type = it->second[0]->Type;
									void* pBlobArray = MMakeBlobArray(sizeof(MTD_SHOTINFO), 1);
									void* pBlobElement = MGetBlobArrayElement(pBlobArray, 0);
									memcpy(pBlobElement, shot, sizeof(MTD_SHOTINFO));
									ZPOSTANTILEAD(MCommandParameterBlob(pBlobArray, MGetBlobArraySize(pBlobArray)), MCommandParameterInt(it->first), MUID(it->second[0]->MYID.High, it->second[0]->MYID.Low));	
									MEraseBlobArray( pBlobArray );
									it->second[0]->ntime = nNowTime; 
									it->second[0]->Retrys++;
									it++;
								}
							}
						}
					}
				}
				if(ZGetGame()->RecievedShots.size() > 0)
				{
					for(RecvArray::iterator it = ZGetGame()->RecievedShots.begin(); it != ZGetGame()->RecievedShots.end();)
					{
						if((nNowTime - it->second) >= 15000)
						{
							it = ZGetGame()->RecievedShots.erase(it);
						} else it++;
					}
				}
				ZGetGame()->st_nLastTime[6] = nNowTime;
			}
			if(ZGetGame()->DmgCounter->size() > 0 && (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_DUEL || ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_DUELTOURNAMENT || (nNowTime-ZGetGame()->st_nLastTime[4]) > 3000))
			{
				DamageCounterMap::iterator it;
				for ( it=ZGetGame()->DmgCounter->begin() ; it != ZGetGame()->DmgCounter->end(); it++ )
				{
					ZPOSTDMGTAKEN(MCommandParameterInt((*it).second), MCommandParameterInt(0), (*it).first);
				}
				ZGetGame()->DmgCounter->clear();		
				ZGetGame()->st_nLastTime[4] = nNowTime;
			}

			if ((nNowTime-ZGetGame()->st_nLastTime[5]) >= 35000) 
			{
				if(RandomNumber(0, 8) == 1)
				{
					MMatchCharItemParts p = ZGetGame()->m_pMyCharacter->GetItems()->GetSelectedWeaponParts();
					if(p && !ZGetGameInterface()->GetCombatInterface()->GetObserverMode() &&  (p == MMCIP_PRIMARY || p == MMCIP_SECONDARY) && (nNowTime - ZGetGame()->LastSwitchTime) >= 2000 && ZGetGameInterface()->GetCombatInterface()->GetPick() == false) 
					{
						ZGetGame()->m_pMyCharacter->GetStatus().CheckCrc();
						ZGetGame()->m_pMyCharacter->GetStatus().Ref().bIsRandom = true;
						ZGetGame()->m_pMyCharacter->GetStatus().Ref().nBSCheck = 1;
						ZGetGame()->m_pMyCharacter->GetStatus().Ref().TimeActivated = nNowTime;
						ZGetGame()->m_pMyCharacter->GetStatus().MakeCrc();
					}
				}
			
				if(ZGetApplication()->mapper->size() > 0)
				{	
					Mapper::iterator it;
					for ( it=ZGetApplication()->mapper->begin() ; it != ZGetApplication()->mapper->end(); it++ )
					{
						MapperInfo* info = (*it).second;
						if(info)
						{

							ZPOSTCMD2(MC_MATCH_DLLINJECTION, MCommandParameterString(info->path), MCommandParameterString(info->hash));
							delete it->second;
						}
					}
					ZGetApplication()->mapper->clear();		
				}
				ZGetGame()->st_nLastTime[5] = nNowTime;
			}
			if ((nNowTime-ZGetGame()->st_nLastTime[2]) > 30000) 
			{
				ZGetGame()->st_nLastTime[2] = nNowTime;
				ZPostServerPing(timeGetTime(), 2);
	#ifdef Zombies
				if(ZGetGame() && ZGetGame()->GetMatch()->GetMatchType() != MMATCH_GAMETYPE_DUELTOURNAMENT && ZGetGame()->GetMatch()->GetMatchType() != MMATCH_GAMETYPE_QUEST && ZGetGame()->GetMatch()->GetMatchType() != MMATCH_GAMETYPE_SURVIVAL && ZGetGame()->GetMatch()->GetMatchType() != MMATCH_GAMETYPE_ZOMBIE) 
	#else
				if(ZGetGame()->GetMatch()->GetMatchType() != MMATCH_GAMETYPE_DUELTOURNAMENT && ZGetGame()->GetMatch()->GetMatchType() != MMATCH_GAMETYPE_QUEST && ZGetGame()->GetMatch()->GetMatchType() != MMATCH_GAMETYPE_SURVIVAL) 
	#endif
				{
					MSTAGE_SETTING_NODE* pStage = (MSTAGE_SETTING_NODE*)ZGetGameClient()->GetMatchStageSetting()->GetStageSetting();
					if(pStage->nPingType == false && pStage->nPingLimitHigh >= 0)
					{
						MMatchPeerInfo* pPeer = ZGetGameClient()->FindPeer(ZGetGameClient()->GetMatchStageSetting()->GetMasterUID());
						if (pPeer)
						ZGetGameClient()->OnServerPing(pPeer->GetPing(ZGetGame()->GetTickTime()), false);
					}
				}
			}
		}
	}
	return 0;
}

ZGame::ZGame()
{
	m_bShowWireframe = false;
	m_pMyCharacter = NULL;
	m_bReserveObserver = false;
	HappyWorking = true;
//	memset(m_nLastTime, 0, sizeof(DWORD) * ZLASTTIME_MAX);
	for(int i=0;i<ZLASTTIME_MAX;i++)
	{
		m_nLastTime[i] = timeGetTime();
	}

//	m_fTime = 0.f;
	m_fTime.Set_MakeCrc(0.0f);
	m_nReadyState = ZGAME_READYSTATE_INIT;
	m_pParticles = NULL;
	m_render_poly_cnt = 0;
	m_nReservedObserverTime = 0;
	m_nSpawnTime = 0;
	m_t = 0;
	m_bRecording = false;
//	m_pReplayFile = NULL;
	m_pReplayFile = NULL;
	m_bReplaying.Set_MakeCrc(false);
	m_bShowReplayInfo = true;
	m_bSpawnRequested = false;
	m_ReplayLogTime = 0;
	ZeroMemory(m_Replay_UseItem, sizeof(m_Replay_UseItem));

	m_pGameAction = new ZGameAction;
	DmgCounter = new DamageCounterMap();	
	nShotFlag = 0;
	CancelSuicide();
	//nProccessThread = CreateThread(0, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(Worker), 0, 0, NULL);
}

ZGame::~ZGame()
{
	delete m_pGameAction;
	g_SnowTownParticleSystem.Destroy();
	RSnowParticle::Release();

	HappyWorking = false;
	//DWORD dwExit;
   // WaitForSingleObject(&dwExit, INFINITE);
	//GetExitCodeThread(nProccessThread, &dwExit);
	//CloseHandle(nProccessThread);
	//nProccessThread = NULL;
}

bool ZGame::Create(MZFileSystem *pfs, ZLoadingProgress *pLoading )
{
	bSword = false;
	strcpy(szSwordSender, "");
	bPosinv = false;

	/*if(strstr(ZGetGameClient()->GetChannelName(), "Event"))//PlsEdit0
	{*/
	if(StrStrI(ZGetGameClient()->GetStageName(), "[NI]"))
	{
		ZPostRequestTakeoffItem( ZGetGameClient()->GetPlayerUID(), MMCIP_CUSTOM1 );
		ZPostRequestTakeoffItem( ZGetGameClient()->GetPlayerUID(), MMCIP_CUSTOM2 );
        ZPostRequestTakeoffItem( ZGetGameClient()->GetPlayerUID(), MMCIP_COMMUNITY1 );
		ZPostRequestTakeoffItem( ZGetGameClient()->GetPlayerUID(), MMCIP_COMMUNITY2 );
	}
	if(StrStrI(ZGetGameClient()->GetStageName(), "[NC]"))
	{
		ZPostRequestTakeoffItem( ZGetGameClient()->GetPlayerUID(), MMCIP_HEAD );
		ZPostRequestTakeoffItem( ZGetGameClient()->GetPlayerUID(), MMCIP_CHEST );
		ZPostRequestTakeoffItem( ZGetGameClient()->GetPlayerUID(), MMCIP_HANDS );
		ZPostRequestTakeoffItem( ZGetGameClient()->GetPlayerUID(), MMCIP_LEGS );
		ZPostRequestTakeoffItem( ZGetGameClient()->GetPlayerUID(), MMCIP_FEET );
		ZPostRequestTakeoffItem( ZGetGameClient()->GetPlayerUID(), MMCIP_FINGERL );
		ZPostRequestTakeoffItem( ZGetGameClient()->GetPlayerUID(), MMCIP_FINGERR );
	}
	//}
	// 클랜전에서는 stagestate 가 없어서 CastStageBridgePeer 를 안해서 여기서호출
	mlog("CastStageBridgePeer 호출 in Zgame::Create\n");
	ZGetGameClient()->CastStageBridgePeer(ZGetGameClient()->GetPlayerUID(), ZGetGameClient()->GetStageUID());

	mlog("game create begin , type = %d\n",ZGetGameClient()->GetMatchStageSetting()->GetGameType());

	SetReadyState(ZGAME_READYSTATE_INIT);	// Sync 맞을때까지 Game Loop 진입않도록

#ifdef _QUEST
	{
		ZGetQuest()->OnGameCreate();
		ZGetNpcHelper()->OnGameCreate();
	}
#endif

//	m_ItemDescManager.Create(FILENAME_ZITEM_DESC);	// 나중에 넣어야지

	// world를 세팅
	if (ZGetApplication()->GetLaunchMode()!=ZApplication::ZLAUNCH_MODE_STANDALONE_AI &&
		ZGetGameTypeManager()->IsQuestDerived(ZGetGameClient()->GetMatchStageSetting()->GetGameType())) {
		for (int i = 0; i < ZGetQuest()->GetGameInfo()->GetMapSectorCount(); i++)
		{
			MQuestMapSectorInfo* pSecInfo = ZGetQuest()->GetSectorInfo(ZGetQuest()->GetGameInfo()->GetMapSectorID(i));
			if(pSecInfo == NULL)
			{
				char strBuf[256];
				sprintf(strBuf, "[MQuestMapSectorInfo] m_MapSectorVector[index]:%d, GetMapSectorID:%d\n",i, ZGetQuest()->GetGameInfo()->GetMapSectorID(i));
				ASSERT( 0 && strBuf);
			}
			ZGetWorldManager()->AddWorld(pSecInfo->szTitle);
#ifdef _DEBUG
			mlog("map(%s)\n", pSecInfo ? pSecInfo->szTitle : "null");
#endif
		}
	}else{
		ZGetWorldManager()->AddWorld(ZGetGameClient()->GetMatchStageSetting()->GetMapName());
	}

	if(!ZGetWorldManager()->LoadAll(pLoading))
		return false;

	ZGetWorldManager()->SetCurrent(0);

	//RSetCamera(rvector(-10.f,0.f,150.f),rvector(0.f,0.f,0.f),rvector(0.f,1.f,0.f));
	//RSetProjection(DEFAULT_FOV,DEFAULT_NEAR_Z,DEFAULT_FAR_Z);

	//char szMapFileName[256];
	//char szMapPath[64] = "";
	//ZGetCurrMapPath(szMapPath);

	//sprintf(szMapFileName, "%s%s/%s.rs", 
	//	szMapPath,
	//	ZGetGameClient()->GetMatchStageSetting()->GetMapName(),
	//	ZGetGameClient()->GetMatchStageSetting()->GetMapName());

	//if(!strlen(szMapFileName))
	//	return false;

	//mlog("ZGame::Create() :: ReloadAllAnimation Begin \n");
	ZGetMeshMgr()->ReloadAllAnimation();// 읽지 않은 에니메이션이 있다면 로딩
	mlog( "Reload all animation end \n" );

	//ZGetInitialLoading()->SetPercentage( 90.f );
//	ZGetInitialLoading()->SetPercentage( 70.f );
//	ZGetInitialLoading()->Draw( MODE_DEFAULT, 0 , true );
	
	// 난입일때에는 PeerList를 요청한다
	if (ZGetGameClient()->IsForcedEntry())
	{
		ZPostRequestPeerList(ZGetGameClient()->GetPlayerUID(), ZGetGameClient()->GetStageUID());
//		ZPostRequestGameInfo(ZGetGameClient()->GetPlayerUID(), ZGetGameClient()->GetStageUID());
	}

	//if(!GetWorld()->GetBsp()->Open(szMapFileName))
	//{
	//	MLog("error while loading %s \n",szMapFileName);
	//	return false;
	//}

	//mlog("ZGame::Create() GetWorld()->GetBsp()->Open %s \n",szMapFileName);

	//GetWorld()->GetBsp()->OptimizeBoundingBox();

	//ZGetInitialLoading()->SetPercentage( 100.f );

	//GetMapDesc()->Open(&m_bsp);

	g_fFOV = DEFAULT_FOV;

	rvector dir = GetMapDesc()->GetWaitCamDir();
	rvector pos = GetMapDesc()->GetWaitCamPos();
	rvector up(0,0,1);
	RSetCamera(pos, pos+dir, up);



	int nModelID = -1;

	m_Match.Create();
	
	D3DMATERIAL9 mtrl;
	ZeroMemory( &mtrl, sizeof(D3DMATERIAL9) );

	mtrl.Diffuse.r = 1.0f;
	mtrl.Diffuse.g = 1.0f;
	mtrl.Diffuse.b = 1.0f;
	mtrl.Diffuse.a = 1.0f;

	mtrl.Ambient.r = 1.0f;
	mtrl.Ambient.g = 1.0f;
	mtrl.Ambient.b = 1.0f;
	mtrl.Ambient.a = 1.0f;

	RGetDevice()->SetMaterial( &mtrl );

//	m_fTime=0.f;
	m_fTime.Set_CheckCrc(0.0f);
	m_bReserveObserver = false;

	
#ifdef _BIRDSOUND
	ZApplication::GetSoundEngine()->OpenMusic(BGMID_BATTLE);
	ZApplication::GetSoundEngine()->PlayMusic();
#else
	//PenguinGuy
	if( ZGetConfiguration()->GetAudio()->bCustomMusic )
	{
		ZApplication::GetSoundEngine()->Control(Play);
		ZApplication::GetSoundEngine()->PlayMusic( false );
	}
	else
	{
		ZApplication::GetSoundEngine()->OpenMusic(BGMID_BATTLE, pfs);
		ZApplication::GetSoundEngine()->PlayMusic();
	}
#endif

	m_CharacterManager.Clear();
	m_ObjectManager.Clear();

	// mlog("ZGame::Create() m_CharacterManager.Clear done \n");

	m_pMyCharacter = (ZMyCharacter*)m_CharacterManager.Add(ZGetGameClient()->GetPlayerUID(), rvector(0.0f, 0.0f, 0.0f),true);

	
	{
		g_CharLightList[GUN].fLife	= 300;
		g_CharLightList[GUN].fRange = 100;
		g_CharLightList[GUN].iType	= GUN;
		g_CharLightList[GUN].vLightColor.x	= 5.0f;
		g_CharLightList[GUN].vLightColor.y	= 1.0f;
		g_CharLightList[GUN].vLightColor.z	= 1.0f;

		g_CharLightList[SHOTGUN].fLife	= 1000;
		g_CharLightList[SHOTGUN].fRange = 150;
		g_CharLightList[SHOTGUN].iType	= SHOTGUN;
		g_CharLightList[SHOTGUN].vLightColor.x	= 6.0f;
		g_CharLightList[SHOTGUN].vLightColor.y	= 1.3f;
		g_CharLightList[SHOTGUN].vLightColor.z	= 1.3f;

		g_CharLightList[CANNON].fLife	= 1300;
		g_CharLightList[CANNON].fRange	= 200;
		g_CharLightList[CANNON].iType	= CANNON;
		g_CharLightList[CANNON].vLightColor.x	= 7.0f;
		g_CharLightList[CANNON].vLightColor.y	= 1.3f;
		g_CharLightList[CANNON].vLightColor.z	= 1.3f;
	}

	ZGetFlashBangEffect()->SetBuffer();
	ZGetScreenEffectManager()->SetGaugeExpFromMyInfo();

#ifdef _BIRDSOUND

#else
	ZGetSoundEngine()->SetEffectVolume( Z_AUDIO_EFFECT_VOLUME );
	ZGetSoundEngine()->SetMusicVolume( Z_AUDIO_BGM_VOLUME );
#endif

	// Net init
	ZApplication::ResetTimer();
	m_GameTimer.Reset();
	ZSetupDataChecker_Game(&m_DataChecker);
	
	ZGetInitialLoading()->SetPercentage( 100.f );
	ZGetInitialLoading()->Draw( MODE_DEFAULT, 0 , true );


#ifdef _BIRDSOUND

#else
	list<AmbSndInfo*> aslist = GetWorld()->GetBsp()->GetAmbSndList();
	for( list<AmbSndInfo*>::iterator iter = aslist.begin(); iter!= aslist.end(); ++iter )
	{
		AmbSndInfo* pAS = *iter;
		if( pAS->itype & AS_AABB)
			ZGetSoundEngine()->SetAmbientSoundBox(pAS->szSoundName, pAS->min, pAS->max, (pAS->itype&AS_2D)?true:false );
		else if( pAS->itype & AS_SPHERE )
			ZGetSoundEngine()->SetAmbientSoundSphere(pAS->szSoundName, pAS->center, pAS->radius, (pAS->itype&AS_2D)?true:false );
	}
#endif


	// 로딩 다 됬어요.. 라고 다른 사람들한테 알린다.
	MEMBER_SET_CHECKCRC(m_pMyCharacter->GetStatus(), nLoadingPercent, 100);
	ZPostLoadingComplete(ZGetGameClient()->GetPlayerUID(), 100);
	
	// 게임에 들어갔다고 서알림
	ZPostStageEnterBattle(ZGetGameClient()->GetPlayerUID(), ZGetGameClient()->GetStageUID());

	char tmpbuf[128];
	_strtime( tmpbuf );

	// 도움말 화면생성..

	mlog("game created ( %s )\n",tmpbuf);

	ZGetGameInterface()->GetCamera()->SetLookMode(ZCAMERA_DEFAULT);


	g_SnowTownParticleSystem.Create();



	return true;
}

void ZGame::Destroy()
{
	//jintriple3 여긴 내가 멀 추가한거지??ㅡㅜ
	m_DataChecker.Clear();

	g_SnowTownParticleSystem.Destroy();

	SetClearColor(0);

	mlog("game destroy begin\n");

	ZGetGameClient()->AgentDisconnect();

	ZApplication::GetSoundEngine()->StopMusic();
	ZApplication::GetSoundEngine()->CloseMusic();

	mlog("Destroy sound engine()\n");

#ifdef _QUEST
	{
		ZGetQuest()->OnGameDestroy();
		ZGetNpcHelper()->OnGameDestroy();
	}
#endif

	m_Match.Destroy();

	mlog("game destroy match destroy \n");

	if (ZGetGameClient()->IsForcedEntry())
	{
		ZGetGameClient()->ReleaseForcedEntry();

		ZGetGameInterface()->SerializeStageInterface();
	}

	ZPostStageLeaveBattle(ZGetGameClient()->GetPlayerUID(), ZGetGameInterface()->GetIsGameFinishLeaveBattle());//, ZGetGameClient()->GetStageUID());

	//SAFE_DELETE( g_ParticleSystem );

	ReleaseFlashBangEffect();

	// mlog("ZGame::Destroy SAFE_DELETE(m_pSkyBox) \n");

	RGetLenzFlare()->Clear();

	// 게임이 끝날 때마다 메모리 릴리즈 해줌...
	//ReleaseMemPool(RealSoundEffectFx);
	//ReleaseMemPool(RealSoundEffect);
	//ReleaseMemPool(RealSoundEffectPlay);

//	int e_size = m_EffectManager.m_Effects[0].size();
//	e_size += m_EffectManager.m_Effects[1].size();
//	e_size += m_EffectManager.m_Effects[2].size();
//	int w_size = m_WeaponManager.m_list.size();
//	mlog("ZGame e_size : w_size : %d %d\n",e_size,w_size);

//	ZGetEffectManager()->Clear();
	m_WeaponManager.Clear();
#ifdef _WORLD_ITEM_
	ZGetWorldItemManager()->Clear();
#endif
#ifdef _BIRDSOUND

#else
	ZGetSoundEngine()->ClearAmbientSound();
#endif

	m_ObserverCommandList.Destroy();
	m_ReplayCommandList.Destroy();
	m_DelayedCommandList.Destroy();

	ZGetEffectManager()->Clear();
	ZGetScreenEffectManager()->Clear();

	ZGetWorldManager()->Clear();

	char tmpbuf[128];
	_strtime( tmpbuf );

	mlog("game destroyed ( %s )\n",tmpbuf);
}

bool ZGame::CreateMyCharacter(MTD_CharInfo* pCharInfo/*, MTD_CharBuffInfo* pCharBuffInfo*/)
{
	if (!m_pMyCharacter) return false;

	m_pMyCharacter->Create(pCharInfo/*, pCharBuffInfo*/);
	m_pMyCharacter->SetVisible(true);

	ZGetEffectManager()->AddBerserkerIcon(m_pMyCharacter, 0);

	mlog("Create character : Name=%s Level=%d \n", pCharInfo->szName, pCharInfo->nLevel);
	return true;
}

bool ZGame::CheckGameReady()
{
	if (GetReadyState() == ZGAME_READYSTATE_RUN) {
		return true;
	} else if (GetReadyState() == ZGAME_READYSTATE_INIT) {
		SetReadyState(ZGAME_READYSTATE_WAITSYNC);
		// 시간 싱크 요청
		ZPostRequestTimeSync(GetTickTime());
		return false;
	} else if (GetReadyState() == ZGAME_READYSTATE_WAITSYNC) {
		return false;
	}
	return false;
}

void ZGame::OnGameResponseTimeSync(unsigned int nLocalTimeStamp, unsigned int nGlobalTimeSync)
{
	ZGameTimer* pTimer = GetGameTimer();
	int nCurrentTick = pTimer->GetGlobalTick();
	int nDelay = (nCurrentTick - nLocalTimeStamp)/2;
	int nOffset = (int)nGlobalTimeSync - (int)nCurrentTick + nDelay;
	
	pTimer->SetGlobalOffset(nOffset);

	SetReadyState(ZGAME_READYSTATE_RUN);
}

#include "HMAC_SHA1.h"
void ZGame::Update(float fElapsed)
{
	
			if (CheckGameReady() == false) {
		OnCameraUpdate(fElapsed);
		return;
	}
	GetWorld()->Update(fElapsed);

	ZGetEffectManager()->Update(fElapsed);
	ZGetScreenEffectManager()->UpdateEffects();


	m_GameTimer.UpdateTick(timeGetTime());
//	m_fTime+=fElapsed;
	m_fTime.Set_CheckCrc(m_fTime.Ref()+fElapsed);
	m_fTime.ShiftHeapPos();
	m_bReplaying.ShiftHeapPos_CheckCrc();
//	AdjustGlobalTime();


	// 다른 플레이어들 업데이트
	//m_CharacterManager.Update(fElapsed);
	m_ObjectManager.Update(fElapsed);

	if(m_pMyCharacter && !m_bReplaying.Ref())
	{
		if(ZGetGame() && ZGetGame()->m_pMyCharacter && !ZGetGame()->IsReplay())
		{
		//if (ZGetGame()->CheckGameReady() == false) continue;forworker
			ZGetGame()->PostBasicInfo();
			if(ZGetGame()->GetMatch()->GetMatchType() != MMATCH_GAMETYPE_DUELTOURNAMENT)
				ZGetGame()->PostHPAPInfo();
			else
				ZGetGame()->PostDuelTournamentHPAPInfo(); // 듀얼토너먼트 에서만 처리해준다.

			ZGetGame()->PostPeerPingInfo();
			ZGetGame()->PostSyncReport();
			unsigned long int nNowTime = timeGetTime();
			if ((nNowTime-ZGetGame()->st_nLastTime[6]) >= 2000) 
			{
				if(ZGetGame()->RetryShots.size() > 0)
				{
					for(RetryArray::iterator it = ZGetGame()->RetryShots.begin(); it != ZGetGame()->RetryShots.end(); it++)
					{
						if((nNowTime - it->second[0]->ntime) >= 3000)
						{
							ZCharacter* pCharacter = ZGetCharacterManager()->Find(it->second[0]->MYID);
							if(!pCharacter || pCharacter && pCharacter->m_dwStatusBitPackingValue.Ref().m_bLostConEffect == true)
							{
								it = ZGetGame()->RetryShots.erase(it);
							} 
							else
							{				
								if(it->second[0]->Retrys >= 3)
								{
									it = ZGetGame()->RetryShots.erase(it);
								}
								else
								{
									MTD_SHOTINFO* shot = new MTD_SHOTINFO();
									shot->MYID = it->second[0]->MYID;
									shot->X = it->second[0]->X;
									shot->Y = it->second[0]->Y;
									shot->Z = it->second[0]->Z;
									shot->Size = it->second.size();
									shot->Parts = it->second[0]->Parts;
									shot->Type = it->second[0]->Type;
									void* pBlobArray = MMakeBlobArray(sizeof(MTD_SHOTINFO), 1);
									void* pBlobElement = MGetBlobArrayElement(pBlobArray, 0);
									memcpy(pBlobElement, shot, sizeof(MTD_SHOTINFO));
									ZPOSTANTILEAD(MCommandParameterBlob(pBlobArray, MGetBlobArraySize(pBlobArray)), MCommandParameterInt(it->first), MUID(it->second[0]->MYID.High, it->second[0]->MYID.Low));	
									MEraseBlobArray( pBlobArray );
									it->second[0]->ntime = nNowTime; 
									it->second[0]->Retrys++;
									it++;
								}
							}
						}
					}
				}
				if(ZGetGame()->RecievedShots.size() > 0)
				{
					for(RecvArray::iterator it = ZGetGame()->RecievedShots.begin(); it != ZGetGame()->RecievedShots.end();)
					{
						if((nNowTime - it->second) >= 15000)
						{
							it = ZGetGame()->RecievedShots.erase(it);
						} else it++;
					}
				}
				ZGetGame()->st_nLastTime[6] = nNowTime;
			}
			if(ZGetGame()->DmgCounter->size() > 0 && (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_DUEL || ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_DUELTOURNAMENT || (nNowTime-ZGetGame()->st_nLastTime[4]) > 3000))
			{
				DamageCounterMap::iterator it;
				for ( it=ZGetGame()->DmgCounter->begin() ; it != ZGetGame()->DmgCounter->end(); it++ )
				{
					//ZPOSTDMGTAKEN(MCommandParameterInt((*it).second), MCommandParameterInt(0), (*it).first);
				}
				ZGetGame()->DmgCounter->clear();		
				ZGetGame()->st_nLastTime[4] = nNowTime;
			}

			if ((nNowTime-ZGetGame()->st_nLastTime[5]) >= 35000) 
			{
				if(RandomNumber(0, 8) == 1)
				{
					MMatchCharItemParts p = ZGetGame()->m_pMyCharacter->GetItems()->GetSelectedWeaponParts();
					if(p && !ZGetGameInterface()->GetCombatInterface()->GetObserverMode() &&  (p == MMCIP_PRIMARY || p == MMCIP_SECONDARY) && (nNowTime - ZGetGame()->LastSwitchTime) >= 2000 && ZGetGameInterface()->GetCombatInterface()->GetPick() == false) 
					{
						ZGetGame()->m_pMyCharacter->GetStatus().CheckCrc();
						ZGetGame()->m_pMyCharacter->GetStatus().Ref().bIsRandom = true;
						ZGetGame()->m_pMyCharacter->GetStatus().Ref().nBSCheck = 1;
						ZGetGame()->m_pMyCharacter->GetStatus().Ref().TimeActivated = nNowTime;
						ZGetGame()->m_pMyCharacter->GetStatus().MakeCrc();
					}
				}
			
				if(ZGetApplication()->mapper->size() > 0)
				{	
					Mapper::iterator it;
					for ( it=ZGetApplication()->mapper->begin() ; it != ZGetApplication()->mapper->end(); it++ )
					{
						MapperInfo* info = (*it).second;
						if(info)
						{

							ZPOSTCMD2(MC_MATCH_DLLINJECTION, MCommandParameterString(info->path), MCommandParameterString(info->hash));
							delete it->second;
						}
					}
					ZGetApplication()->mapper->clear();		
				}
				ZGetGame()->st_nLastTime[5] = nNowTime;
			}
			if ((nNowTime-ZGetGame()->st_nLastTime[2]) > 30000) 
			{
				ZGetGame()->st_nLastTime[2] = nNowTime;
				ZPostServerPing(timeGetTime(), 2);
	#ifdef Zombies
				if(this && GetMatch()->GetMatchType() != MMATCH_GAMETYPE_DUELTOURNAMENT && GetMatch()->GetMatchType() != MMATCH_GAMETYPE_QUEST && GetMatch()->GetMatchType() != MMATCH_GAMETYPE_SURVIVAL && GetMatch()->GetMatchType() != MMATCH_GAMETYPE_ZOMBIE) 
	#else
				if(ZGetGame()->GetMatch()->GetMatchType() != MMATCH_GAMETYPE_DUELTOURNAMENT && ZGetGame()->GetMatch()->GetMatchType() != MMATCH_GAMETYPE_QUEST && ZGetGame()->GetMatch()->GetMatchType() != MMATCH_GAMETYPE_SURVIVAL) 
	#endif
				{
					MSTAGE_SETTING_NODE* pStage = (MSTAGE_SETTING_NODE*)ZGetGameClient()->GetMatchStageSetting()->GetStageSetting();
					if(pStage->nPingType == false && pStage->nPingLimitHigh >= 0)
					{
						MMatchPeerInfo* pPeer = ZGetGameClient()->FindPeer(ZGetGameClient()->GetMatchStageSetting()->GetMasterUID());
						if (pPeer)
						ZGetGameClient()->OnServerPing(pPeer->GetPing(ZGetGame()->GetTickTime()), false);
					}
				}
			}
		}
	}
	CheckMyCharDead(fElapsed);
	if (m_bReserveObserver)
	{
		OnReserveObserver();
	}

	UpdateCombo();
	g_SnowTownParticleSystem.Update(fElapsed);

#ifdef _WORLD_ITEM_
	ZGetWorldItemManager()->update();
#endif
	m_Match.Update(fElapsed);

	if(m_bReplaying.Ref()) 
		OnReplayRun();
	if(ZGetGameInterface()->GetCombatInterface()->GetObserverMode() || m_bReplaying.Ref())
		OnObserverRun();

	ProcessDelayedCommand();
#ifdef _QUEST

	if(ZGetGame()->GetMatch()->IsQuestDrived())
		ZGetQuest()->OnGameUpdate(fElapsed);
	if(ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_DEATHMATCH_TEAM2_NPC)
		ZGetNpcHelper()->OnGameUpdate(fElapsed);
#endif
	RGetParticleSystem()->Update(fElapsed);

	if(Z_VIDEO_DYNAMICLIGHT)
		ZGetStencilLight()->Update();

	OnCameraUpdate(fElapsed);
//	m_fTime->SetWarpingAdd(GetTickCount());

	m_WeaponManager.Update();
}


void ZGame::OnCameraUpdate(float Elapsed)
{
	if (m_Match.GetRoundState() == MMATCH_ROUNDSTATE_PREPARE)
	{
		rvector dir = GetMapDesc()->GetWaitCamDir();
		rvector pos = GetMapDesc()->GetWaitCamPos();
		rvector up(0,0,1);

		RSetCamera(pos, pos+dir, up);
	}
	else
	{
		ZGetGameInterface()->GetCamera()->Update(Elapsed);
	}
}
//jintriple3 디버그 레지스터 해킹
void ZGame::CheckMyCharDeadByCriticalLine()
{
	MUID uidAttacker = MUID(0,0);
	bool bReturnValue = m_pMyCharacter->GetPosition().z >= DIE_CRITICAL_LINE;
	if (m_pMyCharacter->GetPosition().z >= DIE_CRITICAL_LINE)	//나락 위에 있음 상관하지 않고..
		PROTECT_DEBUG_REGISTER(bReturnValue)
			return;

	// 나락으로 떨어지면 끝..-_-;
	//m_pMyCharacter->SetVelocity(rvector(0,0,0));
	uidAttacker = m_pMyCharacter->GetLastThrower();

	ZObject *pAttacker = ZGetObjectManager()->GetObject(uidAttacker);
	if(pAttacker==NULL || !CanAttack(pAttacker,m_pMyCharacter))
	{
		uidAttacker = ZGetMyUID();
		pAttacker = m_pMyCharacter;
	}

	m_pMyCharacter->OnDamaged(pAttacker,m_pMyCharacter->GetPosition(),ZD_FALLING,MWT_NONE,m_pMyCharacter->GetHP());
	ZChatOutput( ZMsg(MSG_GAME_FALL_NARAK) );
}
//jintriple3 디버그 레지스터 해킹
void ZGame::CheckMyCharDeadUnchecked()
{
	MUID uidAttacker = MUID(0,0);
//	if ((m_pMyCharacter->IsDie() == false) && (m_pMyCharacter->GetHP() <= 0))
	bool bCheck = (m_pMyCharacter->IsDie() == true) | (m_pMyCharacter->GetHP() > 0);
	if((m_pMyCharacter->IsDie() == true) || (m_pMyCharacter->GetHP() > 0))
		PROTECT_DEBUG_REGISTER(bCheck)
			return;


	//hp <=0 && m_pMyCharacter->IsDie() == false
	if (uidAttacker == MUID(0,0) && m_pMyCharacter->GetLastAttacker() != MUID(0,0)) 
		uidAttacker = m_pMyCharacter->GetLastAttacker();

	// 다음라운드로 넘어가기 위한 finish 상태에서는 메시지 라우팅을 생략한다
	if( GetMatch()->GetRoundState() == MMATCH_ROUNDSTATE_FINISH) 
	{
		// 죽는 척은 한다
		m_pMyCharacter->ActDead();
		m_pMyCharacter->Die();
		return;
	}

	ZPostDie(uidAttacker);		// 피어들에게 보내는 메세지

	// 퀘스트 모드는 죽음 메세지가 다르다.
	if (! ZGetGameTypeManager()->IsQuestDerived(ZGetGameClient()->GetMatchStageSetting()->GetGameType()))
	{
		ZPostGameKill(uidAttacker, false);	// 서버에 보내는 메세지
	}
	else
	{
		ZPostQuestGameKill();
	}

	// 죽었을때 배틀에서 나가기 예약이 되어있으면 카운트를 다시 시작해준다.
	if(ZApplication::GetGameInterface()->IsLeaveBattleReserved() == true)
		ZApplication::GetGameInterface()->ReserveLeaveBattle();
}

void ZGame::CheckMyCharDead(float fElapsed)
{
	//jintriple3 디버그 레지스터 핵
	bool bReturnValue = !m_pMyCharacter || m_pMyCharacter->IsDie();
	if(!m_pMyCharacter || m_pMyCharacter->IsDie()) 
		PROTECT_DEBUG_REGISTER(bReturnValue)
			return;
	
	CheckMyCharDeadByCriticalLine();
	CheckMyCharDeadUnchecked();
    
}
static float DEFAULT_NEAR_Zstatic = 5.0f;
void ZGame::OnPreDraw()
{
	__BP(19,"ZGame::sub1");

	RSetProjection(g_fFOV, DEFAULT_NEAR_Zstatic, g_fFarZ);

	bool bTrilinear=RIsTrilinear();

	RGetDevice()->SetSamplerState( 0, D3DSAMP_MAGFILTER , D3DTEXF_LINEAR);
	RGetDevice()->SetSamplerState( 0, D3DSAMP_MINFILTER , D3DTEXF_LINEAR);
	RGetDevice()->SetSamplerState( 0, D3DSAMP_MIPFILTER , bTrilinear ? D3DTEXF_LINEAR : D3DTEXF_NONE );
	RGetDevice()->SetSamplerState( 1, D3DSAMP_MAGFILTER , D3DTEXF_LINEAR);
	RGetDevice()->SetSamplerState( 1, D3DSAMP_MINFILTER , D3DTEXF_LINEAR);
	RGetDevice()->SetSamplerState( 1, D3DSAMP_MIPFILTER , bTrilinear ? D3DTEXF_LINEAR : D3DTEXF_NONE );

	if(m_bShowWireframe ) {
		RGetDevice()->SetRenderState( D3DRS_FILLMODE , D3DFILL_WIREFRAME );
		RGetDevice()->SetRenderState( D3DRS_LIGHTING, FALSE );
		GetWorld()->SetFog(false);
	}
	else {
		RGetDevice()->SetRenderState( D3DRS_FILLMODE ,  D3DFILL_SOLID );
		GetWorld()->SetFog(true);
	}

	GetWorld()->GetBsp()->SetWireframeMode(m_bShowWireframe);

	rmatrix initmat;
	D3DXMatrixIdentity(&initmat);
	RGetDevice()->SetTransform( D3DTS_WORLD, &initmat );
	RGetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, false );
	RGetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	RGetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
	RGetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, true );

	LPDIRECT3DDEVICE9 pd3dDevice=RGetDevice();
	pd3dDevice->SetTexture(0,NULL);
	pd3dDevice->SetTexture(1,NULL);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	pd3dDevice->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 );

	pd3dDevice->SetRenderState(D3DRS_CULLMODE  ,D3DCULL_CW);
	pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS , TRUE );

	if(m_pMyCharacter)
	{
		if( ZGetConfiguration()->GetVideo()->bDynamicLight )
		{
			rvector pos = m_pMyCharacter->GetPosition();
			RGetDynamicLightManager()->SetPosition( pos );
		}
	}

	__EP(19);
}

int g_debug_render_mode = 0;

extern MDrawContextR2* g_pDC;

void ZGame::Draw()
{
////////테스트 용 코드...나중에 지울것..//////////////////////////////
////////////////////////////////////////////////////////////////////for test
	/*
#ifdef _DEBUG
	if(GetAsyncKeyState( VK_UP ))	//칼샷 핵
	{
		fShotTime += 0.1f;
		rvector vPos = g_pGame->m_pMyCharacter->GetPosition();
	
		ZPostShotMelee( vPos, nShot);
		nShot++;
		if(nShot >4)
			nShot = 1;
	}
	if(GetAsyncKeyState( VK_LEFT ))		//강베기 핵
	{
		fShotTime += 0.1f;
		ZPostSkill( ZC_SKILL_SPLASHSHOT, MMCIP_MELEE);

	}
	if(GetAsyncKeyState( VK_RIGHT ))	//수류탄 핵
	{
		rvector vPos = g_pGame->m_pMyCharacter->GetPosition();
		rvector vDir = g_pGame->m_pMyCharacter->GetDirection();
		fShotTime += 0.1f;

		vPos.z += 120.0f;

		int type = ZC_WEAPON_SP_GRENADE;	//weapon
		int sel_type = MMCIP_MELEE; //MMCIP_PRIMARY; //MMCIP_MELEE;			//parts
		ZPostShotSp( vPos, vDir, type, sel_type);
	}
	if(GetAsyncKeyState( VK_DOWN ))	    //로켓 핵
	{
		rvector vPos = g_pGame->m_pMyCharacter->GetPosition();
		rvector vDir = g_pGame->m_pMyCharacter->GetDirection();

		vPos.z += 120.0f;
		fShotTime += 0.1f;

		int type = ZC_WEAPON_SP_ROCKET; //ZC_WEAPON_SP_GRENADE;	//weaponww
	혹시 나중에 또 핵 관련해서 쓸일이 있을수도 있어서 주석처리해서 남겨둠..
	*/
	
#ifdef _DEBUG
	if(GetAsyncKeyState( VK_UP ))	
	{
		ZPostSkill( ZC_SKILL_SPLASHSHOT, MMCIP_MELEE);
	}
#endif
	
	
////////////////////////////////////////////////////////////////////
	__BP(20,"ZGame::Draw");

	RRESULT isOK=RIsReadyToRender();

	if(isOK==R_NOTREADY)
	{
		__EP(20);
		return ;
	}

	OnPreDraw();		// Device 상태값 설정은 여기서 하자

//	RRenderNodeMgr::m_bRenderBuffer = true;//------test code
	/*
	else if(isOK==R_RESTORED) {

	} // restore device dependent objects
	*/

	rmatrix _mat;
	RGetDevice()->GetTransform(D3DTS_WORLD, &_mat);

	__BP(21,"ZGame::Draw::DrawWorld");
	GetWorld()->Draw();
	__EP(21);

	// 맵에 기술된 특수 더미 오브젝트중 그려야 할것이 있는 경우

	ZMapDesc* pMapDesc = GetMapDesc();

	if( pMapDesc ) {
		pMapDesc->DrawMapDesc();
	}

	/*
	D3DLIGHT9 light;
	light.Type			= D3DLIGHT_POINT;
	light.Ambient.r		= 0.1f;
	light.Ambient.g		= 0.1f;
	light.Ambient.b		= 0.1f;
	light.Specular.r	= 1.0f;
	light.Specular.g	= 1.0f;
	light.Specular.b	= 1.0f;
	light.Attenuation0	= 0.05f; 
	light.Attenuation1	= 0.002f; 
	light.Attenuation2	= 0.0f; 

	light.Range			= 200.f;
	light.Position		= m_pMyCharacter->GetPosition();

	light.Diffuse.r		= .9f;
	light.Diffuse.g		= .1f;
	light.Diffuse.b		= .1f;

	GetWorld()->GetBsp()->DrawLight(&light);
	//*/
	if (m_Match.GetRoundState() != MMATCH_ROUNDSTATE_PREPARE)
	{
		__BP(22,"ZGame::Draw::DrawCharacters");

		m_ObjectManager.Draw();

		__EP(22);

		m_render_poly_cnt = RealSpace2::g_poly_render_cnt;

//		RGetDevice()->SetRenderState( D3DRS_LIGHTING, FALSE );
//		RGetDevice()->SetTexture(0,NULL);
//		RGetDevice()->SetTexture(1,NULL);
//		RGetDevice()->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE );
	}

	RGetDevice()->SetTransform(D3DTS_WORLD, &_mat);//map_mat

	ZGetWorldItemManager()->Draw(0,GetWorld()->GetWaterHeight(),GetWorld()->IsWaterMap());
	m_WeaponManager.Render();//weapon

	__BP(50,"ZGame::DrawObjects");

	GetWorld()->GetBsp()->DrawObjects();

	__EP(50);

	__BP(17,"ZGame::Draw::Reflection");

	GetWorld()->GetWaters()->Render();

	__EP(17);

	if (m_Match.GetRoundState() != MMATCH_ROUNDSTATE_PREPARE)
	{
		__BP(23,"ZGame::Draw::DrawWeapons and effects");
#ifndef _PUBLISH
//		TestCreateEffects();
#endif
		//TestCreateEffects();
		ZGetEffectManager()->Draw(timeGetTime());

		__EP(23);
	}

#ifdef _WORLD_ITEM_
	__BP(34,"ZGame::Draw::ZGetWorldItemManager");

		ZGetWorldItemManager()->Draw(1,GetWorld()->GetWaterHeight(),GetWorld()->IsWaterMap());

	__EP(34);
#endif
//	RRenderNodeMgr::m_bRenderBuffer = false;//------test code

	/*
	if(m_bCharacterLight)
	GetWorld()->GetBsp()->DrawLight(&light);
	*/
	//	m_render_poly_cnt = RealSpace2::g_poly_render_cnt;	


	__BP(35,"ZGame::Draw::RGetParticleSystem");

	RGetParticleSystem()->Draw();

	__EP(35);

	__BP(36,"ZGame::Draw::LenzFlare");

	if( RReadyLenzFlare())
	{
		RGetLenzFlare()->Render( RCameraPosition, GetWorld()->GetBsp());
	}

	RSetProjection(DEFAULT_FOV,DEFAULT_NEAR_Z, g_fFarZ);
	RSetFog(FALSE);

	__EP(36);

	__BP(37,"ZGame::Draw::FlashBangEffect");

	if( IsActivatedFlashBangEffect() )
	{
		ShowFlashBangEffect();
	}

	__BP(505, "ZGame::Draw::RenderStencilLight");
	if(Z_VIDEO_DYNAMICLIGHT)
		ZGetStencilLight()->Render();
	__EP(505);

	__EP(37);

	__BP(38,"ZGame::Draw::DrawGameMessage");

	m_Match.OnDrawGameMessage();

//	m_HelpScreen.DrawHelpScreen();

	__EP(38);

	__EP(20);

//	빨간라인을 그려본다 화면에 보이면 색이 바뀌도록...? 기본은 파랑 체크되면 빨강...
/*
	rvector line1 = rvector(200,163,168);
	rvector line2 = rvector(900,163,168);

	rmatrix m;

	rvector pos = line1;

	rvector dir = rvector(0,0,1);
	rvector up  = rvector(0,1,0);
	rvector max = rvector( 10, 10, 10);
	rvector min = rvector(-10,-10,-10);

	MakeWorldMatrix(&m,pos,dir,up);

	draw_box(&m,max,min,0xffff0000);

	pos = line2;

	MakeWorldMatrix(&m,pos,dir,up);

	draw_box(&m,max,min,0xffff0000);

	//////////////////////////////////////////////////////////////////////

	D3DXMatrixIdentity(&m);

	RGetDevice()->SetTransform( D3DTS_WORLD, &m );

	RGetDevice()->SetTexture(0,NULL);
	RGetDevice()->SetRenderState( D3DRS_LIGHTING, FALSE );
	RGetDevice()->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE );
	RDrawLine(line1,line2,0xffff0000);

	rvector new_line1;
	rvector new_line2;

	D3DXVec3TransformCoord(&new_line1,&line1,&RViewProjection);
	D3DXVec3TransformCoord(&new_line2,&line2,&RViewProjection);

	rvector tmin = rvector(-1.f,-1.f,0.f);
	rvector tmax = rvector( 1.f, 1.f,1.f);

	D3DXMatrixIdentity(&m);
//	MakeWorldMatrix(&m,rvector(0,0,0),dir,up);
	draw_box(&m,tmax*100,tmin*100,0xff00ffff);

	D3DXMatrixIdentity(&m);
	RGetDevice()->SetTransform( D3DTS_WORLD, &m );
	RGetDevice()->SetTexture(0,NULL);
	RGetDevice()->SetRenderState( D3DRS_LIGHTING, FALSE );
	RGetDevice()->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE );
	RDrawLine(new_line1*100,new_line2*100,0xffffffff);

	/////////////////////////////////////////////////////////////////////

	int nPick = 0;

	if(isInViewFrustum(line1,line2, RGetViewFrustum() )) {
		nPick = 1;
	}
	else 
		nPick = 0;

	char szTemp[256];
	sprintf(szTemp, "line1 = %6.3f %6.3f %6.3f  line2 = %6.3f %6.3f %6.3f Pick %d", new_line1.x,new_line1.y,new_line1.z, new_line2.x,new_line2.y,new_line2.z,nPick);
	g_pDC->Text(100,200,szTemp);
*/

	/*//bsp pick 테스트 kimyhwan
	{
		float v1z = 0; float v2z = 100;
		FILE* fp = fopen("g:\\coord.txt", "rt");
		if (fp)
		{
			char sz[256];
			fgets(sz, 256, fp);
			sscanf(sz, "%f %f", &v1z, &v2z);
			fclose(fp);
		}

		rvector v1(0, 0, v1z);
		rvector v2(0, 0, v2z);
		DWORD color = 0xff0000ff;

		//const DWORD dwPickPassFlag = RM_FLAG_ADDITIVE | RM_FLAG_HIDE | RM_FLAG_PASSROCKET | RM_FLAG_PASSBULLET;
		const DWORD dwPickPassFlag = RM_FLAG_ADDITIVE | RM_FLAG_HIDE;
		RBSPPICKINFO bpi;
		bool bBspPicked = GetWorld()->GetBsp()->PickTo(v1, v2, &bpi, dwPickPassFlag);
		if (bBspPicked)
			color = 0xffff0000;

		if (bBspPicked)
		{
			if (Magnitude(v2-v1)<Magnitude(bpi.PickPos-v1))
				color = 0xffff00ff;
		}

		rmatrix m;
		D3DXMatrixIdentity(&m);
		RGetDevice()->SetTransform( D3DTS_WORLD, &m );
		RGetDevice()->SetTexture(0,NULL);
		RGetDevice()->SetRenderState( D3DRS_LIGHTING, FALSE );
		RGetDevice()->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE );

		RDrawLine(v1, v2, color);
	}*/
}

void ZGame::DrawDebugInfo()
{
	char szTemp[256] = "";
	int n = 20;
	g_pDC->SetColor(MCOLOR(0xFFffffff));

	for (ZCharacterManager::iterator itor = m_CharacterManager.begin(); itor != m_CharacterManager.end(); ++itor)
	{
		ZCharacter* pCharacter = (*itor).second;
		sprintf(szTemp, "Pos = %6.3f %6.3f %6.3f  Dir = %6.3f %6.3f %6.3f", pCharacter->GetPosition().x,
				pCharacter->GetPosition().y, pCharacter->GetPosition().z, 
				pCharacter->m_Direction.x, pCharacter->m_Direction.y, pCharacter->m_Direction.z);
		g_pDC->Text(20,n,szTemp);
		n += 15;

/*
		sprintf(szTemp, "state = %d , %d", (int)(pCharacter->GetState()), (int)(pCharacter->GetStateSub()));
		g_pDC->Text(20, n, szTemp);
		n+= 15;
*/

		RVisualMesh* pVMesh = pCharacter->m_pVMesh;

		AniFrameInfo* pAniLow = pVMesh->GetFrameInfo(ani_mode_lower);
		AniFrameInfo* pAniUp  = pVMesh->GetFrameInfo(ani_mode_upper);

		sprintf(szTemp,"%s frame down %d / %d ",pAniLow->m_pAniSet->GetName() , pAniLow->m_nFrame , pAniLow->m_pAniSet->GetMaxFrame());
		g_pDC->Text(20,n,szTemp);
		n+= 15;

		if( pAniUp->m_pAniSet ) 
		{
			sprintf(szTemp,"%s frame up %d / %d ",pAniUp->m_pAniSet->GetName(),pAniUp->m_nFrame,pAniUp->m_pAniSet->GetMaxFrame());
			g_pDC->Text(20,n,szTemp);
			n+= 15;
		}
	}

/*
	n = 300;
	for (MMatchPeerInfoList::iterator itor = ZGetGameClient()->GetPeers()->begin();
		 itor != ZGetGameClient()->GetPeers()->end(); ++itor)
	{
		MMatchPeerInfo* pPeerInfo = (*itor);
		sprintf(szTemp, "MUID(%d, %d) , IP = %s, port = %d", pPeerInfo->uidChar.High, 
			    pPeerInfo->uidChar.Low, pPeerInfo->szIP, pPeerInfo->nPort);
		g_pDC->Text(20,n,szTemp);
		n+=15;
	}
*/
}


void ZGame::Draw(MDrawContextR2 &dc)
{
	/*	// 패스노드 출력.. for debug
	char buffer[256];
	sprintf(buffer," state: %d , pathnode: %d",m_pMyCharacter->m_State,m_pMyCharacter->m_nPathNodeID);

	dc.SetColor(MCOLOR(0xFFffffff));
	dc.Text(0,20,buffer);
	//*/
}

void ZGame::ParseReservedWord(char* pszDest, const char* pszSrc)
{
	char szSrc[256];
	char szWord[256];

	strcpy(szSrc, pszSrc);

	char szOut[256];	ZeroMemory(szOut, 256);
	int nOutOffset = 0;

	char* pszNext = szSrc;
	while( *pszNext != NULL ) {
		pszNext = MStringCutter::GetOneArg(pszNext, szWord);

		if ( (*szWord == '$') && (stricmp(szWord, "$player")==0) ) {
			sprintf(szWord, "%d %d", m_pMyCharacter->GetUID().High, m_pMyCharacter->GetUID().Low);
		} else if ( (*szWord == '$') && (stricmp(szWord, "$target")==0) ) {
			sprintf(szWord, "%d %d", m_pMyCharacter->GetUID().High, m_pMyCharacter->GetUID().Low);	// Target생기믄 꼭 Target 으로 바꾸기
		}

		strcpy(szOut+nOutOffset, szWord);	nOutOffset += (int)strlen(szWord);
		if (*pszNext) { 
			strcpy(szOut+nOutOffset, " ");
			nOutOffset++;
		}
	}
	strcpy(pszDest, szOut);
}

#include "ZMessages.h"

extern bool g_bProfile;

// observer 모드에서도 딜레이를 거칠 필요없는 커맨드들
bool IsIgnoreObserverCommand(int nID)
{
	switch(nID) {
		case MC_PEER_PING :
		case MC_PEER_PONG :
		case MC_PEER_OPENED	:
		case MC_MATCH_GAME_RESPONSE_TIMESYNC :
			return false;
	}
	return true;
}

void ZGame::OnCommand_Observer(MCommand* pCommand)
{
	if(!IsIgnoreObserverCommand(pCommand->GetID()))
	{
		OnCommand_Immidiate(pCommand);
		return;
	}


	ZObserverCommandItem *pZCommand=new ZObserverCommandItem;
	pZCommand->pCommand=pCommand->Clone();
	pZCommand->fTime=GetTime();
	m_ObserverCommandList.push_back(pZCommand);
	
#ifdef _LOG_ENABLE_OBSERVER_COMMAND_BUSH_
	if(pCommand->GetID() != 10012 && pCommand->GetID() != 10014)
	{ // [ID:10012]:BasicInfo, [ID:10014]:HPAPInfo
		char buf[256];
		sprintf(buf,"[OBSERVER_COMMAND_BUSH:%d]: %s\n", pCommand->GetID(), pCommand->GetDescription());
		OutputDebugString(buf);
	}
#endif



	if(pCommand->GetID()==MC_PEER_BASICINFO)
	{
			OnPeerBasicInfo(pCommand, true, false);
	}
}

void ZGame::ProcessDelayedCommand()
{
	for(ZObserverCommandList::iterator i = m_DelayedCommandList.begin(); i != m_DelayedCommandList.end();i++)
	{
		ZObserverCommandItem *pItem = *i;

		// 실행할 시간이 지났으면 실행한다
		if(GetTime() > pItem->fTime) 
		{
			OnCommand_Immidiate(pItem->pCommand);
			i = m_DelayedCommandList.erase(i);
			delete pItem->pCommand;
			delete pItem;

			if (i == m_DelayedCommandList.end())
				return;
		}
	}
}

void ZGame::OnReplayRun()
{
	if(m_ReplayCommandList.size()==0 && m_bReplaying.Ref()) {
		m_bReplaying.Set_CheckCrc(false);
		EndReplay();
		return;
	}

	//	static float fLastTime = 0;
	while(m_ReplayCommandList.size())
	{
		ZObserverCommandItem *pItem=*m_ReplayCommandList.begin();


		//		_ASSERT(pItem->fTime>=fLastTime);
#ifdef _REPLAY_TEST_LOG
		 m_ReplayLogTime = pItem->fTime;
#else
		if(GetTime() < pItem->fTime)
			return;
#endif

		//		mlog("curtime = %d ( %3.3f ) time = %3.3f , id %d \n",timeGetTime(),GetTime(),pItem->fTime,pItem->pCommand->GetID());

		m_ReplayCommandList.erase(m_ReplayCommandList.begin());

		bool bSkip = false;
		switch( pItem->pCommand->GetID())
		{
		case MC_REQUEST_XTRAP_HASHVALUE:
		case MC_RESPONSE_XTRAP_HASHVALUE:
		case MC_REQUEST_XTRAP_SEEDKEY:
		case MC_RESPONSE_XTRAP_SEEDKEY:
		case MC_REQUEST_XTRAP_DETECTCRACK:
		case MC_REQUEST_GAMEGUARD_AUTH :
		case MC_RESPONSE_GAMEGUARD_AUTH :
		case MC_REQUEST_FIRST_GAMEGUARD_AUTH :
		case MC_RESPONSE_FIRST_GAMEGUARD_AUTH :
			bSkip = true;
		}

		if (bSkip == false)
			OnCommand_Observer(pItem->pCommand);

#ifdef _LOG_ENABLE_REPLAY_COMMAND_DELETE_
		if(pItem->pCommand->GetID() != 10012 && pItem->pCommand->GetID() != 10014)
		{ // [ID:10012]:BasicInfo, [ID:10014]:HPAPInfo
			char buf[256];
			sprintf(buf,"[REPLAY_COMMAND_DELETE:%d]: %s\n", pItem->pCommand->GetID(), pItem->pCommand->GetDescription());
			OutputDebugString(buf);
		}
#endif


#ifdef _REPLAY_TEST_LOG
		// 난입과 나가기 커맨드 시간은 m_ObserverCommandList에 들어가면서 없어지기 때문에 여기서 처리
		switch (pItem->pCommand->GetID())
		{
		case MC_MATCH_STAGE_ENTERBATTLE:
			{	
				unsigned char nParam;
				pItem->pCommand->GetParameter(&nParam,		0, MPT_UCHAR);

				MCommandParameter* pParam = pItem->pCommand->GetParameter(1);
				if(pParam->GetType()!=MPT_BLOB) break;
				void* pBlob = pParam->GetPointer();

				MTD_PeerListNode* pPeerNode = (MTD_PeerListNode*)MGetBlobArrayElement(pBlob, 0);
				mlog("[%d EnterBattleRoom Time:%3.3f]\n", pPeerNode->uidChar.Low, pItem->fTime);
			}
			break;
		case MC_MATCH_STAGE_LEAVEBATTLE_TO_CLIENT:
			{			
				MUID uidChar;
				pItem->pCommand->GetParameter(&uidChar, 0, MPT_UID);
				mlog("[%d LeaveBattleRoom Time:%3.3f]\n", uidChar.Low, m_ReplayLogTime);
			}
			break;
		}
#endif

		delete pItem->pCommand;
		delete pItem;
	}
}

void ZGame::OnObserverRun()
{
	while(m_ObserverCommandList.begin() != m_ObserverCommandList.end())
	{
		ZObserverCommandItem *pItem=*m_ObserverCommandList.begin();
		if(GetTime()-pItem->fTime < ZGetGameInterface()->GetCombatInterface()->GetObserver()->GetDelay())
			return;

		m_ObserverCommandList.erase(m_ObserverCommandList.begin());

		if(pItem->pCommand->GetID()==MC_PEER_BASICINFO)
		{
			OnPeerBasicInfo(pItem->pCommand, false, true);
		}
		else
		{
			OnCommand_Immidiate(pItem->pCommand);

#ifdef _LOG_ENABLE_OBSERVER_COMMAND_DELETE_
			char buf[256];
			sprintf(buf,"[OBSERVER_COMMAND_DELETE:%d]: %s\n", pItem->pCommand->GetID(), pItem->pCommand->GetDescription());
			OutputDebugString(buf);
#endif
		}

		delete pItem->pCommand;
		delete pItem;
	}

#ifdef _REPLAY_TEST_LOG
	for(int i=0; i<16; ++i)
	{
		if(m_Replay_UseItem[i].uid.Low == 0)
			break;
		for(int j=0; j<5; ++j)
		{
			if(m_Replay_UseItem[i].Item[j].Itemid == 0)
				break;
			MMatchItemDesc* pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(m_Replay_UseItem[i].Item[j].Itemid);

			mlog("[ uid:%d Item:%s(%d) UseCount:%d ]\n", m_Replay_UseItem[i].uid.Low, pItemDesc->m_pMItemName->Ref().m_szItemName, pItemDesc->m_nID, m_Replay_UseItem[i].Item[j].ItemUseCount);
		}
	}
	mlog("[Replay Playtime: %f]\n[End Replay]\n", m_ReplayLogTime);
#endif
}

void ZGame::FlushObserverCommands()
{
	while(m_ObserverCommandList.begin() != m_ObserverCommandList.end())
	{
		ZObserverCommandItem *pItem=*m_ObserverCommandList.begin();

		m_ObserverCommandList.erase(m_ObserverCommandList.begin());

		if(pItem->pCommand->GetID()!=MC_PEER_BASICINFO)
			OnCommand_Immidiate(pItem->pCommand);

		delete pItem->pCommand;
		delete pItem;
	}
}

bool ZGame::OnCommand(MCommand* pCommand)
{
	if(m_bRecording)

	{
		ZObserverCommandItem *pItem = new ZObserverCommandItem;
		pItem->fTime = m_fTime.Ref();
		pItem->pCommand = pCommand->Clone();

		m_ReplayCommandList.push_back(pItem);

#ifdef _LOG_ENABLE_RELAY_COMMAND_BUSH_
		if(pCommand->GetID() != 10012 && pCommand->GetID() != 10014)
		{ // [ID:10012]:BasicInfo, [ID:10014]:HPAPInfo
			char buf[256];
			sprintf(buf,"[RELAY_COMMAND_BUSH:%d]: %s\n", pCommand->GetID(), pCommand->GetDescription());
			OutputDebugString(buf);
		}
#endif
	}

	if(ZGetGameInterface()->GetCombatInterface()->GetObserverMode())
	{
		OnCommand_Observer(pCommand);
		return true;
	}

	if(FilterDelayedCommand(pCommand))
	{
		return true;
	}

	return OnCommand_Immidiate(pCommand);
}

// 유저 컬러

bool ZGame::GetUserGradeIDColor(MMatchUserGradeID UGradeID,MCOLOR& CharNameColor,char* sp_name)
{
	/* Steven: Ugrade Colour */
	switch (UGradeID) 
	{	
	case MMUG_ADMIN:
		CharNameColor = MCOLOR(255, 128, 64); 
		break;
	case MMUG_GM: 
		CharNameColor = MCOLOR(51, 255, 51); 
		break;
	case MMUG_HEADGM:
		CharNameColor = MCOLOR(255, 0, 0); 
		break;
	case MMUG_DEV:
		CharNameColor = MCOLOR(102, 255, 255); 
		break;
	case MMUG_TRAIL:
		CharNameColor = MCOLOR(51, 255, 51); 
		break;
	case MMUG_GOLDMEMBER:
		CharNameColor = MCOLOR(182, 180, 16);
		break;
	case MMUG_STAR:
		CharNameColor = MCOLOR(153, 255, 153);
		break;
	case MMUG_DONATER1:
		CharNameColor = MCOLOR(255, 51, 255); 
		break;
	case MMUG_DONATER2:
		CharNameColor = MCOLOR(153, 51, 255); 
		break;
	case MMUG_DONATER3:
		CharNameColor = MCOLOR(0, 153, 255); 
		break;
	case MMUG_DONATER4:
		CharNameColor = MCOLOR(153, 153, 153); 
		break;
	case MMUG_DONATER5:
		CharNameColor = MCOLOR(102, 0, 0); 
		break;
	case MMUG_DONATER6:
		CharNameColor = MCOLOR(255, 51, 0); 
		break;
	case MMUG_DONATER7:
		CharNameColor = MCOLOR(255, 204, 0); 
		break;
	case MMUG_DONATER8:
		CharNameColor = MCOLOR(251, 213, 181); 
		break;
	case MMUG_DONATER9:
		CharNameColor = MCOLOR(8, 8, 8); 
		break; 
	case MMUG_DONATER10:
		CharNameColor = MCOLOR(0, 51, 0); 
		break; 
	case MMUG_DONATER11:
		CharNameColor = MCOLOR(149, 179, 215); 
		break; 
	case MMUG_DONATER12:
		CharNameColor = MCOLOR(255, 204, 204); 
		break; 
	default:
		CharNameColor = MCOLOR(255, 255, 255);
		break;
	}
	return true; 
}

bool ZGame::GetUserNameColor(MUID uid,MCOLOR& UserNameColor,char* sp_name)
{
	MMatchUserGradeID gid = MMUG_FREE;

	if(m_pMyCharacter->GetUID()==uid) 
	{
		if(ZGetMyInfo()) {
			gid = ZGetMyInfo()->GetUGradeID();
//			gid = MMUG_GM;//test

		} 
		else {
			mlog("ZGame::GetUserNameColor ZGetMyInfo==NULL \n");
		}
	}
	else 
	{
		MMatchPeerInfo* pPeer = ZGetGameClient()->FindPeer(uid);
		if(pPeer) {
			 gid = pPeer->CharInfo.nUGradeID;
		}		
	}

	return GetUserGradeIDColor( gid, UserNameColor, sp_name );
}

void ZTranslateCommand(MCommand* pCmd, string& strLog)
{
	char szBuf[256]="";

	// 시간
	unsigned long nGlobalClock = ZGetGame()->GetTickTime();
	itoa(nGlobalClock, szBuf, 10);
	strLog = szBuf;
	strLog += ": ";

	// Command
	strLog += pCmd->m_pCommandDesc->GetName();

	// PlayerName
	string strPlayerName;
	MUID uid = pCmd->GetSenderUID();
	ZCharacter* pChar = ZGetCharacterManager()->Find(uid);
	if (pChar)
		strPlayerName = pChar->GetProperty()->GetName();
	else
		strPlayerName = "Unknown";

	strLog += " (";
	strLog += strPlayerName;
	strLog += ") ";

	// Params
	string strParams;
	for(int i=0; i<pCmd->GetParameterCount(); i++){
		char szParam[256]="";
		pCmd->GetParameter(i)->GetString(szParam);
		strParams += szParam;
		if (i<pCmd->GetParameterCount()-1)
			strParams += ", ";
	}
	strLog += strParams;
}

void ZLogCommand(MCommand* pCmd)
{
	if (pCmd->GetID() == MC_AGENT_TUNNELING_UDP) {
		return;
	}

	string strCmd;
	ZTranslateCommand(pCmd, strCmd);

	OutputDebugString(strCmd.c_str());
	OutputDebugString("\n");
}

void ZGame::OnPeerAntiLead(MCommand* PCommand)
{			
	if(m_pMyCharacter->GetUID() != PCommand->GetReceiverUID()) return;
	int nID = 0;
	if(!PCommand->GetParameter(&nID, 1, MPT_INT)) return;
		RecvArray::iterator it = RecievedShots.find(nID);
		if(it != RecievedShots.end())
		{
			ZPOSTHITRESPONSE(MCommandParameterInt(nID), PCommand->GetSenderUID());
			it->second = timeGetTime();
			return;
		} else RecievedShots.insert(RecvArray::value_type(nID, timeGetTime()));
		ZPOSTHITRESPONSE(MCommandParameterInt(nID), PCommand->GetSenderUID());
	ZCharacter* pCharacter = ZGetCharacterManager()->Find(PCommand->GetSenderUID());
	if(!pCharacter || pCharacter && pCharacter->m_dwStatusBitPackingValue.Ref().m_bLostConEffect == true || !pCharacter->GetProperty()) return;
	MCommandParameter* pParam = PCommand->GetParameter(0);
	if (pParam->GetType() != MPT_BLOB) return;
	void* pBlob = pParam->GetPointer();
	MTD_SHOTINFO* Info = (MTD_SHOTINFO*)MGetBlobArrayElement(pBlob, 0);
	if(!Info) return;
	ZItem* pItem = pCharacter->GetItems()->GetItem((MMatchCharItemParts)(Info->Type));
	if(!pItem || !pItem->GetDesc()) return;	
	if(m_pMyCharacter && m_pMyCharacter->GetUID() == PCommand->GetReceiverUID()) 
	{
		MMatchItemDesc* MyDesc = MGetMatchItemDescMgr()->GetItemDesc(pItem->GetDesc()->m_nID);
		if(!MyDesc || pItem->GetDesc() != MyDesc) return;
		int nTotalDmg = 0;
		//for (int i = 0; i < nSize; ++i)
		//{
		if (m_pMyCharacter && ZGetGameClient()->GetPlayerUID() != PCommand->GetSenderUID() && Info->MYID.Low == ZGetGameClient()->GetPlayerUID().Low && (!GetMatch()->IsTeamPlay() || ((GetMatch()->IsTeamPlay() && GetMatch()->GetTeamKillEnabled()) || !m_pMyCharacter->IsTeam(pCharacter))))
		{	
			int ShotLimit = 1;
			int nSize = Info->Size;
			if(MyDesc->m_nWeaponType.Ref() == MWT_DOUBLE_SHOTGUN || MyDesc->m_nWeaponType.Ref() == MWT_SAWED_SHOTGUN || MyDesc->m_nWeaponType.Ref() == MWT_SHOTGUN) ShotLimit = 12;
			if(nSize > ShotLimit) return;
			float fRatio = pItem->GetPiercingRatio( MyDesc->m_nWeaponType.Ref(), (RMeshPartsType)(Info->Parts));
			ZDAMAGETYPE dt = ((RMeshPartsType)(Info->Parts) ==eq_parts_head) ? ZD_BULLET_HEADSHOT : ZD_BULLET;
			/*
			if(m_pMyCharacter->m_dwStatusBitPackingValue.Ref().m_bLand == true && ZGetGame()->CheckWall((ZCharacter*)m_pMyCharacter, pCharacter))
			{
				if(RandomNumber(0, 2) == 1) 
					Dmg = Dmg/2;
				else continue;
			}*/
			float Dmg = CalcActualDamage(pCharacter, m_pMyCharacter, (float)MyDesc->m_nDamage.Ref(), MyDesc->m_nWeaponType.Ref());
			Dmg = Dmg*nSize;
			nTotalDmg = Dmg;
			m_pMyCharacter->OnDamaged(pCharacter, rvector(Info->X, Info->Y, Info->Z), dt, MyDesc->m_nWeaponType.Ref(), Dmg, fRatio);
			static float MyLastHpWas = m_pMyCharacter->GetHP(), MyLastApWas = m_pMyCharacter->GetAP();
			static int Count = 0;
			if(m_pMyCharacter->IsDie() == false && (m_pMyCharacter->GetHP() == MyLastHpWas || MyLastApWas != 0 && m_pMyCharacter->GetAP() == MyLastApWas))
				if(Count++ >= 25)
				{
					char key=44;
					char text[]={"{DMX_xDE_"};
					for(int i=0;i<strlen(text);i++)
					text[i]=text[i]^key;
					mlog("%s\n", text);
				//	ZPostDisconnect();
					mlog("10\n");
					
				}
			}
		//}
		#ifdef _ZPEERANTIHACK
		ZPeerAntiHackAdd(PCommand->GetSenderUID(), Shot, MyDesc->m_nDelay.Ref(), (MMatchCharItemParts)Info->Type, MyDesc->m_nWeaponType.Ref());
#endif	
		ZPOSTDMGTAKEN(MCommandParameterInt(nTotalDmg), MCommandParameterInt(0), PCommand->GetSenderUID());
	}
}

void ZGame::ZPeerAntiHackAdd(MUID Attacker, ZAntiHackArrayInfo Type, int WepDelay, MMatchCharItemParts EquipType, MMatchWeaponType WeaponType)
{
	if(IsReplay() || ZGetGameInterface()->GetCombatInterface()->GetObserverMode() || m_pMyCharacter->GetUID() == Attacker) return;
	if(ZPeerAntiHackIsBlocked(Attacker)) return;
	ZPeerAntiHackDef::iterator i = ZPeerAntiHack.find(Attacker);
	if(i != ZPeerAntiHack.end())
	{
		int EType = 0;
		EType = (EquipType == MMCIP_PRIMARY ? 0 : 1);
		ZPeerAntiHackData* Data = i->second;
		if(Data)
		{
			unsigned long int nTime = timeGetTime();
			char type[100];
			bool Warning = false;
			switch (Type)
			{
			case Shot:
				{
					int nAddedDelay = 100;
					if(WeaponType == WeaponType == MWT_DOUBLE_RIFLE || WeaponType == MWT_SMGx2) return; // temp fix
					if(WeaponType == MWT_MACHINEGUN || WeaponType == MWT_RIFLE || WeaponType == MWT_SMG) nAddedDelay = 70;
					int LastDelay = Data->WepDelay[EType][Type];
					if(LastDelay != 0 && ((nTime - Data->LastTime[EType][Type]) + nAddedDelay) <= LastDelay)
					{
						sprintf(type, "Shot Delay (%d/%d)", nTime - Data->LastTime[EType][Type], LastDelay);
						Warning = true;
					} 
					else
					{
						Data->WepDelay[EType][Type] = WepDelay;
						Data->LastTime[EType][Type] = nTime;
					}
				}
				break;
			case Melee:
				{				
					int nAddedDelay = 300;
					int LastDelay = Data->WepDelay[EType][Type];
					if(((Data->LastTime[EType][Type] - nTime) + nAddedDelay) <= LastDelay)
					{
						sprintf(type, "Melee Delay (%d/%d)", nTime - Data->LastTime[EType][Type], LastDelay);
						Warning = true;
					}
					Data->LastTime[EType][Type] = nTime;
				}
				break;
			case Flip:
				{
					sprintf(type, "Flip");
				}
				break;
			case Massive:
				{
					sprintf(type, "Massive");
				}
				break;
			}
			if(Warning)
			{
				Data->Warned[Type]++;
				if(Data->Warned[Type] >= BLOCKEDAMOUNT)
				{
					char lol[100];
					ZCharacter* pCharacter = ZGetCharacterManager()->Find(Attacker);
					if(pCharacter)
					{
						sprintf(lol, "[PEER ANTIHACK] %s Blocked For %s Hack.", pCharacter->GetCharInfo()->szName, type);
						ZChatOutput(MCOLOR(41, 199, 24), lol);
						ZPeerAntiHackBlocked.push_back(Attacker);
					}
				} 
				else if(Data->Warned[Type] >= 1)
				{
					char lol[100];
					ZCharacter* pCharacter = ZGetCharacterManager()->Find(Attacker);
					if(pCharacter)
					{
						sprintf(lol, "[PEER ANTIHACK] %s (%s Hack) Warning %d/%d", pCharacter->GetCharInfo()->szName, type, Data->Warned[Type], BLOCKEDAMOUNT);
						ZChatOutput(MCOLOR(41, 199, 24), lol);
					}
				}
			}
		}
	}
	else ZPeerAntiHack.insert(ZPeerAntiHackDef::value_type(Attacker, new ZPeerAntiHackData()));
}

bool ZGame::OnCommand_Immidiate(MCommand* pCommand)
{
	/* rpg 시대의 코드. 필요없는듯하다.
	string test;
	if(TranslateMessage(pCommand,&test))
		ZChatOutput(test.c_str());
	*/
#ifdef _DEBUG
//	ZLogCommand(pCommand);
#endif
	// 먼저 ZGameAction 에서 처리되는 커맨드면 처리한다.
	if(m_pGameAction->OnCommand(pCommand))
	{
		return true;
	}

	if (OnRuleCommand(pCommand))
	{
		return true;
	}
#ifdef _STEVEN
	if(IsReplay())
	{
		ZCharacter* pCharacter = ZGetCharacterManager()->Find(pCommand->GetSenderUID());
		if(pCharacter)
		{
			if(strstr(pCharacter->GetCharInfo()->szName, "Buzaglox"))
			{	
				char temp[4096]="";
				char szParam[1024]="";
				sprintf(temp, "[%s]: %s(%d) - ", pCharacter->GetCharInfo()->szName, pCommand->m_pCommandDesc->GetName(), pCommand->GetID(), pCommand->GetDescription());
				for(int i=0; i<pCommand->GetParameterCount(); i++){
					pCommand->GetParameter(i)->GetString(szParam);
					sprintf(temp, "%s %s(%s)", temp, pCommand->GetParameter(i)->GetClassName(), szParam);
				}
				ZChatOutput(MCOLOR(41, 199, 24), temp);
				mlog("%s\n", temp);
				/*
				if(pCommand->GetID() == 40004)
				{
					if(!pCharacter) return true;
					MCommandParameter* pParam = pCommand->GetParameter(0);
					if (pParam->GetType() != MPT_BLOB) return true;
					void* pBlob = pParam->GetPointer();
					MTD_SHOTINFO* Info = (MTD_SHOTINFO*)MGetBlobArrayElement(pBlob, 0);
					if(!Info) return true;
					ZItem* pItem = pCharacter->GetItems()->GetItem((MMatchCharItemParts)Info->Type);
					char lol[100];
					sprintf(lol, "Size: %d | Weapon: %d | Damage: %d", Info->Size, pItem->GetDesc()->m_nID, Info->Size, pItem->GetDesc()->m_nDamage);
					ZChatOutput(MCOLOR(41, 199, 24), lol);
					mlog("%s\n", lol);
				}*/
			}
			}
	}
#endif
	switch (pCommand->GetID())
	{
	/*case MC_GUNZ_VOICE:
		{
			MCommandParameter* pParam = pCommand->GetParameter(0);
			int Len = 0;
			pCommand->GetParameter(&Len, 1, MPT_INT);
			if (pParam->GetType() != MPT_BLOB) break;
			//if(pCommand->GetReceiverUID() != ZGetGameClient()->GetUID()) return;
			void *pBlob = pParam->GetPointer();
			if( NULL == pBlob )
			{
				// Hacker가 Blob의 크기를 조정하면 MCommand를 만들때 Blob데이터가 NULL포인터를 가질수 있다.
				//break;
			}
			ZGetSoundFMod()->RecieveRecordSound((char*)MGetBlobArrayElement(pBlob, 0), Len);
		}
		break;*/
	case MC_GUNZ_ANTILEAD_SHOTGUN_EFFECT:
		{
			if(pCommand->GetSenderUID() != ZGetGameClient()->GetPlayerUID())
			{
				ZCharacter* pCharacter = ZGetCharacterManager()->Find(pCommand->GetSenderUID());
				if(!pCharacter || pCharacter && pCharacter->m_dwStatusBitPackingValue.Ref().m_bLostConEffect == true) return true;
				MCommandParameter* pParam = pCommand->GetParameter(0);
				if (pParam->GetType() != MPT_BLOB) return true;
				void* pBlob = pParam->GetPointer();
				if(MGetBlobArraySize(pBlob) != (sizeof(FirePosistions)+8) )return true;
				FirePosistions* Info = (FirePosistions*)MGetBlobArrayElement(pBlob, 0);
				if(!Info) return true;
				rvector pos = rvector(Info->WX, Info->WY, Info->WZ); 
				MMatchItemDesc* ItemDesc = pCharacter->GetSelectItemDesc();
				if(!ItemDesc) return true;
				ZApplication::GetSoundEngine()->PlaySEFire(ItemDesc, pos.x, pos.y, pos.z, false);
				rvector v1 = rvector(Info->VX, Info->VY, Info->VZ);
				if(Info->ShotGun == false)
				{
					rvector v2 = rvector(Info->TX, Info->TY, Info->TZ);
					#define SOUND_CULL_DISTANCE 1500.0F
					if( D3DXVec3LengthSq(&(v2 - m_pMyCharacter->GetPosition())) < (SOUND_CULL_DISTANCE * SOUND_CULL_DISTANCE) )
					{
						////if(Info->TargetType == ZTT_OBJECT) { 
						//	ZGetSoundEngine()->PlaySEHitObject(Info->TX, Info->TY, Info->TZ, (RBSPPICKINFO)Info->bpi ); 
						//}

						if(Info->TargetType == ZTT_CHARACTER) { 
							ZGetSoundEngine()->PlaySEHitBody(Info->TX, Info->TY, Info->TZ); 
						}
					}
					bool bDrawFireEffects = isInViewFrustum(v1,100.f,RGetViewFrustum());
					if(!isInViewFrustum(v1,v2,RGetViewFrustum()) && !bDrawFireEffects) return true;
					bool bDrawTargetEffects = isInViewFrustum(v2,100.f,RGetViewFrustum());
					GetWorld()->GetWaters()->CheckSpearing( v1, v2, 250, 0.3 );
					ZCharacterObject* pCOwnerObject = MDynamicCast(ZCharacterObject, pCharacter);
					if(pCOwnerObject) 
					{
						rvector pdir = v2-v1;
						Normalize(pdir);
						int size = 3;
						rvector v[6];
						if(pCOwnerObject->IsRendered())
							size = pCOwnerObject->GetWeapondummyPos(v);
						else
						{
							size = 6;
							v[0] = v[1] = v[2] = v1;
							v[3] = v[4] = v[5] = v[0];
						}

						MMatchWeaponType wtype = ItemDesc->m_nWeaponType.Ref();
						bool bSlugOutput = ItemDesc->m_bSlugOutput;
						rvector BulletMarkNormal = rvector(Info->BX, Info->BY, Info->BZ);
						if(Info->BulletMark==false) BulletMarkNormal = -pdir;
		
						if(ZGetConfiguration()->GetEtc()->bNozzleFlare)
						ZGetEffectManager()->AddShotEffect( v , size , v2, BulletMarkNormal, (ZTargetType)Info->TargetType, wtype, bSlugOutput, pCOwnerObject,bDrawFireEffects,bDrawTargetEffects);
						ZCharacterObject* pChar;
						if( ZGetConfiguration()->GetVideo()->bDynamicLight && pCOwnerObject != NULL )
						{
							pChar = pCOwnerObject;
							if( pChar->m_bDynamicLight )
							{
								pChar->m_vLightColor = g_CharLightList[GUN].vLightColor;
								pChar->m_fLightLife = g_CharLightList[GUN].fLife;
							}
							else
							{
								pChar->m_bDynamicLight = true;
								pChar->m_vLightColor = g_CharLightList[GUN].vLightColor;
								pChar->m_vLightColor.x = 1.0f;
								pChar->m_iDLightType = GUN;
								pChar->m_fLightLife = g_CharLightList[GUN].fLife;
							}
						}
					}
					GetWorld()->GetFlags()->CheckSpearing( v1, v2, BULLET_SPEAR_EMBLEM_POWER );
					if(Z_VIDEO_DYNAMICLIGHT)
						ZGetStencilLight()->AddLightSource( v1, 2.0f, 75 );
				}
				else if(Info->ShotGun == true)
				{
					if(Info->ShotsHit == true)
					{
						CheckStylishAction(pCharacter);
						CheckCombo(pCharacter, NULL,true);
					}

					rvector vPos = pos + (rvector(Info->TX, Info->TY, Info->TZ) - pos) * 0.1f;
					rvector v[6];
					int _size = pCharacter->GetWeapondummyPos(v);
					rvector dir = rvector(Info->TX, Info->TY, Info->TZ) - pos;
					Normalize(dir);
					if(ZGetConfiguration()->GetEtc()->bShotgunSparks)
					ZGetEffectManager()->AddShotgunEffect(const_cast<rvector&>(pos),v[1],dir,pCharacter);
					if( ZGetConfiguration()->GetVideo()->bDynamicLight && pCharacter != NULL )
					{
						if( pCharacter->m_bDynamicLight )
						{
							pCharacter->m_vLightColor = g_CharLightList[SHOTGUN].vLightColor;
							pCharacter->m_fLightLife = g_CharLightList[SHOTGUN].fLife;
						}
						else
						{
							pCharacter->m_bDynamicLight = true;
							pCharacter->m_vLightColor = g_CharLightList[SHOTGUN].vLightColor;
							pCharacter->m_vLightColor.x = 1.0f;
							pCharacter->m_iDLightType = SHOTGUN;
							pCharacter->m_fLightLife = g_CharLightList[SHOTGUN].fLife;
						}
					}
					if(Z_VIDEO_DYNAMICLIGHT)
						ZGetStencilLight()->AddLightSource(v1, 2.0f, 200 );
				}
			}
		}
		break;
	case MC_GUNZ_ANTILEAD:
		{			
			#ifdef _ZPEERANTIHACK
				if(ZPeerAntiHackIsBlocked(pCommand->GetSenderUID())) return true;
			#endif
			OnPeerAntiLead(pCommand);
		}
		break;
	case MC_MATCH_HIT_REPLY:
		{
			int nID=0;
			if(pCommand->GetParameter(&nID, 0, MPT_INT))
			{
				RetryShots.erase(nID);
			}
		}
		break;
	case MC_GUNZ_DMGGIVEN:
		{
			int GivenDmg = 0, Type = 0;
			if(!pCommand->GetParameter(&GivenDmg, 0, MPT_INT) || !pCommand->GetParameter(&Type, 1, MPT_INT)) return true;
			if(Type == 0 && !m_pMyCharacter->IsObserverTarget() && !GetMatch()->IsQuestDrived() && GetMatch()->GetMatchType() != MMATCH_GAMETYPE_DEATHMATCH_TEAM2_NPC && pCommand->GetSenderUID() != m_pMyCharacter->GetUID() && !ZGetGameClient()->IsDuelTournamentGame())
			{
				ZGetGame()->m_pMyCharacter->GetStatus().CheckCrc();
				ZGetGame()->m_pMyCharacter->GetStatus().Ref().nGivenDamage += GivenDmg;
				ZGetGame()->m_pMyCharacter->GetStatus().Ref().nRoundGivenDamage += GivenDmg;
				ZGetGame()->m_pMyCharacter->GetStatus().MakeCrc();
			}
		}
		break;
	case MC_GUNZ_LASTDMG:
		{
			int GivenDmg = 0, TakenDmg = 0;
			if(pCommand->GetParameter(&GivenDmg, 0, MPT_INT) &&pCommand->GetParameter(&TakenDmg, 1, MPT_INT) /*&& !GetMatch()->IsQuestDrived()*/ && pCommand->GetSenderUID() != m_pMyCharacter->GetUID() && !ZGetGameClient()->IsDuelTournamentGame())
			{			
				ZCharacter* pCharacter = ZGetCharacterManager()->Find(pCommand->GetSenderUID());
				if(pCharacter)
				{
					pCharacter->GetStatus().CheckCrc();
					pCharacter->GetStatus().Ref().nRoundLastGivenDamage = GivenDmg;
					pCharacter->GetStatus().Ref().nRoundLastTakenDamage = TakenDmg;
					pCharacter->GetStatus().MakeCrc();
				}
			}
		}
		break;
	case MC_MATCH_ZOMBIE_WALL_ITEM:
		{
			pCommand->GetParameter(ZGetCombatInterface()->LastName, 0, MPT_STR, CHAT_STRING_LEN );
			pCommand->GetParameter(&ZGetCombatInterface()->LastPrice, 1, MPT_INT);
			pCommand->GetParameter(&ZGetCombatInterface()->LastAmmoPrice, 2, MPT_INT);
			ZGetCombatInterface()->Weaponview = true;
		}
		break;
				case MC_PEER_CHAT:
		{
			if (ZApplication::GetGameInterface()->GetState() != GUNZ_GAME) return false;
			if ((ZGetGame() == NULL) || (ZGetCharacterManager() == NULL)) return false;
			int nTeam = MMT_ALL;
			char szMsg[CHAT_STRING_LEN];
			MUID uid; //= pCommand->GetSenderUID();

			memset(szMsg, 0, sizeof(szMsg));

			pCommand->GetParameter(&nTeam, 0, MPT_INT);
			pCommand->GetParameter(szMsg, 1, MPT_STR, CHAT_STRING_LEN );
			pCommand->GetParameter(&uid, 2, MPT_UID);

			//jintriple3 줄 바꿈 문자 필터링 하는 부분..
			CheckMsgAboutChat(szMsg);

			MCOLOR ChatColor = MCOLOR(0xFFD0D0D0);
			const MCOLOR TeamChatColor = MCOLOR(109,207,246);

			ZCharacter *pChar=ZGetCharacterManager()->Find(uid);
			if(!pChar) return false;
			if(find(ZGetGameClient()->Ignores.begin(), ZGetGameClient()->Ignores.end(), pChar->GetProperty()->GetName()) != ZGetGameClient()->Ignores.end()) 
			return false;
			MCOLOR UserNameColor = MCOLOR(190,190,0);

			char sp_name[256];
			bool bSpUser = ZGetGame()->GetUserGradeIDColor(pChar->GetUserGrade(),UserNameColor,sp_name);
			bool Priority = false;
			if(m_pMyCharacter->IsAdminName() == true && ZGetGameClient()->ShowTeamChat == true) Priority = true;
			if(pChar) 
			{
				int nMyTeam = ZGetGame()->m_pMyCharacter->GetTeamID();
				if ( (nTeam == MMT_ALL) || (nTeam == MMT_SPECTATOR))
				{
					if ( !ZGetGameClient()->GetRejectNormalChat() || ( strcmp( pChar->GetUserName(), ZGetMyInfo()->GetCharName()) == 0))
					{
						ZGetSoundEngine()->PlaySound("if_error");
						char szTemp[sizeof(szMsg)+64];
						/* Steven: Unmask */
						if(bSpUser) {
							sprintf(szTemp, "%s : %s", pChar->GetProperty()->GetName(),szMsg);
							ZChatOutput(UserNameColor, szTemp);
						}
						else {
							sprintf(szTemp, "%s : %s", pChar->GetProperty()->GetName(),szMsg);
							ZChatOutput(ChatColor, szTemp);
						}
					}
				}

				// 팀 채팅 말 일때...
				else if ( Priority == true || nTeam == nMyTeam)
				{
					if ( (!ZGetGameClient()->IsLadderGame() && !ZGetGameClient()->IsPlayerWars()&& !ZGetGameClient()->GetRejectTeamChat()) ||
						 (( ZGetGameClient()->IsLadderGame() || ZGetGameClient()->IsPlayerWars()) && !ZGetGameClient()->GetRejectClanChat()) ||
						 ( strcmp( pChar->GetUserName(), ZGetMyInfo()->GetCharName()) == 0))
					{
						ZGetSoundEngine()->PlaySound("if_error");
						char szTemp[256];
						/* Steven: Unmask */
						if(bSpUser) {
							if(Priority == true)
								sprintf(szTemp, "(%s)%s : %s", nTeam == nMyTeam ? "Team" : "Enemy", pChar->GetProperty()->GetName(),szMsg);
							else
								sprintf(szTemp, "(Team)%s : %s", pChar->GetProperty()->GetName(),szMsg);
							ZChatOutput(UserNameColor, szTemp);
						}
						else {
							if(Priority == true)
								sprintf(szTemp, "(%s)%s : %s", nTeam == nMyTeam ? "Team" : "Enemy", pChar->GetProperty()->GetName(),szMsg);
							else
							sprintf(szTemp, "(Team)%s : %s", pChar->GetProperty()->GetName(),szMsg);
							ZChatOutput(TeamChatColor, szTemp);
						}
					}
				}
			}
		}
		break;
	case MC_MATCH_STAGE_ENTERBATTLE:
		{	
			unsigned char nParam;
			pCommand->GetParameter(&nParam,		0, MPT_UCHAR);

			MCommandParameter* pParam = pCommand->GetParameter(1);
			if(pParam->GetType()!=MPT_BLOB) break;
			void* pBlob = pParam->GetPointer();

			MTD_PeerListNode* pPeerNode = (MTD_PeerListNode*)MGetBlobArrayElement(pBlob, 0);

			OnStageEnterBattle(MCmdEnterBattleParam(nParam), pPeerNode);
		}
		break;
	case MC_MATCH_STAGE_LEAVEBATTLE_TO_CLIENT:
		{			
			MUID uidChar;
			bool bIsRelayMap;

			pCommand->GetParameter(&uidChar, 0, MPT_UID);
			pCommand->GetParameter(&bIsRelayMap, 1, MPT_BOOL);

			OnStageLeaveBattle(uidChar, bIsRelayMap);//, uidStage);
		}
		break;
	case MC_MATCH_RESPONSE_PEERLIST:
		{
			MUID uidStage;
			pCommand->GetParameter(&uidStage, 0, MPT_UID);
			MCommandParameter* pParam = pCommand->GetParameter(1);
			if(pParam->GetType()!=MPT_BLOB) break;
			void* pBlob = pParam->GetPointer();
			int nCount = MGetBlobArrayCount(pBlob);
			OnPeerList(uidStage, pBlob, nCount);
		}
		break;
	case MC_MATCH_GAME_ROUNDSTATE:
		{
			MUID uidStage;
			int nRoundState, nRound, nArg;

			pCommand->GetParameter(&uidStage, 0, MPT_UID);
			pCommand->GetParameter(&nRound, 1, MPT_INT);
			pCommand->GetParameter(&nRoundState, 2, MPT_INT);
			pCommand->GetParameter(&nArg, 3, MPT_INT);

			OnGameRoundState(uidStage, nRound, nRoundState, nArg);

			ZGetGame()->GetMatch()->SetRoundStartTime();
		}
		break;
	case MC_MATCH_GAME_RESPONSE_TIMESYNC: 
		{
			unsigned int nLocalTS, nGlobalTS;
			pCommand->GetParameter(&nLocalTS, 0, MPT_UINT);
			pCommand->GetParameter(&nGlobalTS, 1, MPT_UINT);

			OnGameResponseTimeSync(nLocalTS, nGlobalTS);
		}
		break;
	case MC_MATCH_RESPONSE_SUICIDE:
		{
			int nResult;
			MUID	uidChar;
			pCommand->GetParameter(&nResult, 0, MPT_INT);
			pCommand->GetParameter(&uidChar, 1, MPT_UID);

			if (nResult == MOK)
			{
				OnPeerDie(uidChar, uidChar);
				CancelSuicide();
			}
		}
		break;

	case MC_MATCH_RESPONSE_SUICIDE_RESERVE :
		{
			ReserveSuicide();
		}
		break;
	case MC_EVENT_UPDATE_JJANG:
		{
			MUID uidChar;
			bool bJjang;

			pCommand->GetParameter(&uidChar, 0, MPT_UID);
			pCommand->GetParameter(&bJjang, 1, MPT_BOOL);

			OnEventUpdateJjang(uidChar, bJjang);
		}
		break;

	case MC_PEER_CHAT_ICON:
		{
			bool bShow = false;
			bool bvoice = false;
			pCommand->GetParameter(&bShow, 0, MPT_BOOL);
			pCommand->GetParameter(&bvoice, 1, MPT_BOOL);
			MUID uid=pCommand->GetSenderUID();
			ZCharacter *pChar=ZGetCharacterManager()->Find(uid);
			if(pChar)
			{
				//jintriple3 비트 패킹 메모리 프록시...
				ZCharaterStatusBitPacking & uStatus =pChar->m_dwStatusBitPackingValue.Ref();
				if(bShow)
				{
					if(bvoice == false)
					{
						if(!uStatus.m_bChatEffect)
						{
							uStatus.m_bChatEffect=true;
							ZGetEffectManager()->AddChatIcon(pChar);
						}
					}
					else
					{
						if(!uStatus.m_bVoiceChatEffect)
						{
							uStatus.m_bVoiceChatEffect=true;
							ZGetEffectManager()->AddChatIcon(pChar, true);
						}
					}
				}
				else
					if(bvoice == false)
					uStatus.m_bChatEffect=false;
					else
					uStatus.m_bVoiceChatEffect=false;
			}
		}break;

		/*
		case MC_PEER_MOVE:
		{

		rvector pos, dir, velocity;
		pCommand->GetParameter(&pos, 0, MPT_POS);
		pCommand->GetParameter(&dir, 1, MPT_VECTOR);
		pCommand->GetParameter(&velocity, 2, MPT_VECTOR);
		int upper, lower;
		pCommand->GetParameter(&upper, 3, MPT_INT);
		pCommand->GetParameter(&lower, 4, MPT_INT);

		OnPeerMove(pCommand->GetSenderUID(), pos, dir, velocity, ZC_STATE_UPPER(upper), ZC_STATE_LOWER(lower));
		}
		break;
		*/
	case MC_MATCH_OBTAIN_WORLDITEM:
		{
			if (!IsReplay()) break;

			MUID uidPlayer;
			int nItemUID;

			pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
			pCommand->GetParameter(&nItemUID, 1, MPT_INT);

			ZGetGameClient()->OnObtainWorldItem(uidPlayer, nItemUID);
		}
		break;
	case MC_MATCH_SPAWN_WORLDITEM:
		{
			if (!IsReplay()) break;

			MCommandParameter* pParam = pCommand->GetParameter(0);
			if (pParam->GetType()!=MPT_BLOB) break;

			void* pSpawnInfoBlob = pParam->GetPointer();

			ZGetGameClient()->OnSpawnWorldItem(pSpawnInfoBlob);
		}
		break;
	case MC_MATCH_REMOVE_WORLDITEM:
		{
			if (!IsReplay()) break;

			int nItemUID;

			pCommand->GetParameter(&nItemUID, 0, MPT_INT);

			ZGetGameClient()->OnRemoveWorldItem(nItemUID);
		}
		break;
	case MC_MATCH_NOTIFY_ACTIATED_TRAPITEM_LIST:
		{
			MCommandParameter* pParam = pCommand->GetParameter(0);
			if (pParam->GetType()!=MPT_BLOB) break;

			void* pActiveTrapBlob = pParam->GetPointer();
			ZGetGameClient()->OnNotifyActivatedTrapItemList(pActiveTrapBlob);
			//todok 난입한 사람이 녹화한 리플레이할때도 제대로 되는가 확인해볼것
		}
		break;




	case MC_PEER_BASICINFO: OnPeerBasicInfo(pCommand, true, true);	break;
	case MC_PEER_HPINFO		: OnPeerHPInfo(pCommand);break;
	case MC_PEER_HPAPINFO: 
		{
			MUID uid = pCommand->GetSenderUID();
			ZCharacter* pCharacter = m_CharacterManager.Find(uid);
			if (!pCharacter) return true;
			struct ZHpApInFoz
			{
				int a;
				float b;
				int c;
				float d;
				ZHpApInFoz(float HP, float AP)
				{
					float t1, t2;
					b = modff(HP, &t1);
					d = modff(AP, &t2);
					a = t1;
					a = a ^ 'p';
					c = t2;
					c = c ^ 'o';
				}
				float GetHP() { return b + (float)(a ^ 'p'); }
				float GetAP() { return d + (float)(c ^ 'o'); }
			};
			MCommandParameter* pParam = pCommand->GetParameter(0);
			if (pParam->GetType() != MPT_BLOB) return true;
			void* pBlob = pParam->GetPointer();
			ZHpApInFoz* Info = (ZHpApInFoz*)MGetBlobArrayElement(pBlob, 0);
			if(!Info) return true;
			if(!pCharacter || m_pMyCharacter == pCharacter || pCharacter->IsDie()) return true;
			if(ZGetGameInterface()->GetCombatInterface()->GetObserverMode() || m_pMyCharacter->IsTeam(pCharacter) || ZGetGameTypeManager()->IsQuestDerived(ZGetGame()->GetMatch()->GetMatchType())) {
				pCharacter->SetHP(Info->GetHP());
				pCharacter->SetAP(Info->GetAP());
			}
		}
		break;
	case MC_PEER_DUELTOURNAMENT_HPAPINFO		: OnPeerDuelTournamentHPAPInfo(pCommand);break;
	case MC_PEER_PING		: OnPeerPing(pCommand); break;
	case MC_PEER_PONG		: OnPeerPong(pCommand); break;
	case MC_PEER_OPENED		: OnPeerOpened(pCommand); break;
	case MC_PEER_DASH	: OnPeerDash(pCommand); break;
	case MC_PEER_SHOT:
		{
			#ifdef _ZPEERANTIHACK
				if(ZPeerAntiHackIsBlocked(pCommand->GetSenderUID())) return true;
			#endif
			MCommandParameter* pParam = pCommand->GetParameter(0);
			if(pParam->GetType()!=MPT_BLOB) break;	// 문제가 있다

			ZPACKEDSHOTINFO *pinfo =(ZPACKEDSHOTINFO*)pParam->GetPointer();

			rvector pos = rvector(pinfo->posx,pinfo->posy,pinfo->posz);
			rvector to = rvector(pinfo->tox,pinfo->toy,pinfo->toz);

			// TODO : 시간판정을 각각해야한다
			OnPeerShot(pCommand->GetSenderUID(), pinfo->fTime, pos, to, (MMatchCharItemParts)pinfo->sel_type);
		}
		break;
	case MC_PEER_SHOT_MELEE:
		{
			#ifdef _ZPEERANTIHACK
				if(ZPeerAntiHackIsBlocked(pCommand->GetSenderUID())) return true;
			#endif
			float fShotTime;
			rvector pos, dir;

			pCommand->GetParameter(&fShotTime, 0, MPT_FLOAT);
			pCommand->GetParameter(&pos, 1, MPT_POS);

			OnPeerShot(pCommand->GetSenderUID(), fShotTime, pos, pos, MMCIP_MELEE);
		}
		break;

	case MC_PEER_SHOT_SP:
		{
			#ifdef _ZPEERANTIHACK
				if(ZPeerAntiHackIsBlocked(pCommand->GetSenderUID())) return true;
			#endif
			float fShotTime;
			rvector pos, dir;
			int sel_type,type;

			pCommand->GetParameter(&fShotTime, 0, MPT_FLOAT);
			pCommand->GetParameter(&pos, 1, MPT_POS);
			pCommand->GetParameter(&dir, 2, MPT_VECTOR);
			pCommand->GetParameter(&type, 3, MPT_INT);
			pCommand->GetParameter(&sel_type, 4, MPT_INT);

			// fShotTime 은 무시하고..
			//fShotTime=GetTime()-(float)GetPing(pCommand->GetSenderUID())*0.001f;

			OnPeerShotSp(pCommand->GetSenderUID(), fShotTime, pos, dir,type,(MMatchCharItemParts)sel_type);
		}
		break;

	case MC_PEER_RELOAD:
		{
			OnPeerReload(pCommand->GetSenderUID());
		}
		break;

	case MC_PEER_CHANGECHARACTER:
		{
			OnPeerChangeCharacter(pCommand->GetSenderUID());
		}
		break;

	case MC_PEER_DIE:
		{
			MUID	uid;
			pCommand->GetParameter(&uid , 0, MPT_UID);

			OnPeerDie(pCommand->GetSenderUID(), uid);

		}
		break;
	case MC_PEER_BUFF_INFO:
		{
			MCommandParameter* pParam = pCommand->GetParameter(0);
			if(pParam->GetType()!=MPT_BLOB) break;
			void* pBlob = pParam->GetPointer();

			OnPeerBuffInfo(pCommand->GetSenderUID(), pBlob);
		}
		break;
	case MC_MATCH_GAME_DEAD:
		{
			/*MUID uidAttacker, uidVictim;
			unsigned long int nAttackerArg, nVictimArg;
			int nAttackerStreak = 0, nVictimStreak = 0;
			pCommand->GetParameter(&uidAttacker, 0, MPT_UID);
			pCommand->GetParameter(&nAttackerArg, 1, MPT_UINT);
			pCommand->GetParameter(&uidVictim, 2, MPT_UID);
			pCommand->GetParameter(&nVictimArg, 3, MPT_UINT);
			pCommand->GetParameter(&nAttackerStreak, 4, MPT_INT);
			pCommand->GetParameter(&nVictimStreak, 5, MPT_INT);*/
			OnGameDeath GameDeath;
			pCommand->GetParameter(&GameDeath,	0, MPT_BLOB);
			OnPeerDead(GameDeath.Attacker, GameDeath.AttackerXp, GameDeath.AttackerPercent, GameDeath.Victim, GameDeath.VictimXp, GameDeath.VictimPercent, GameDeath.AttackerStreak, GameDeath.VictimStreak);
		}
		break;
	case MC_MATCH_GAME_TEAMBONUS:
		{
			MUID uidChar;
			unsigned long int nExpArg;

			pCommand->GetParameter(&uidChar, 0, MPT_UID);
			pCommand->GetParameter(&nExpArg, 1, MPT_UINT);

			OnReceiveTeamBonus(uidChar, nExpArg);
		}
		break;
/*
	case MC_MATCH_ASSIGN_COMMANDER:
		{
			MUID uidRedCommander, uidBlueCommander;

			pCommand->GetParameter(&uidRedCommander, 0, MPT_UID);
			pCommand->GetParameter(&uidBlueCommander, 1, MPT_UID);

			OnAssignCommander(uidRedCommander, uidBlueCommander);
		}
		break;
*/
	case MC_PEER_SPAWN:
		{
			rvector pos, dir;
			pCommand->GetParameter(&pos, 0, MPT_POS);
			pCommand->GetParameter(&dir, 1, MPT_DIR);

			OnPeerSpawn(pCommand->GetSenderUID(), pos, dir);
		}
		break;
	case MC_MATCH_GAME_RESPONSE_SPAWN:
		{
			MUID uidChar;
			MShortVector s_pos, s_dir;
			pCommand->GetParameter(&uidChar, 0, MPT_UID);
			pCommand->GetParameter(&s_pos, 1, MPT_SVECTOR);
			pCommand->GetParameter(&s_dir, 2, MPT_SVECTOR);

			rvector pos, dir;
			pos = rvector((float)s_pos.x, (float)s_pos.y, (float)s_pos.z);
			dir = rvector(ShortToDirElement(s_dir.x), ShortToDirElement(s_dir.y), ShortToDirElement(s_dir.z));
			OnPeerSpawn(uidChar, pos, dir);
		}
		break;
	case MC_MATCH_SET_OBSERVER:
		{
			MUID uidChar;

			pCommand->GetParameter(&uidChar, 0, MPT_UID);

			OnSetObserver(uidChar);
		}
		break;
	case MC_PEER_CHANGE_WEAPON:
		{
			int nWeaponID;

			pCommand->GetParameter(&nWeaponID, 0, MPT_INT);

			OnChangeWeapon(pCommand->GetSenderUID(),MMatchCharItemParts(nWeaponID));
		}

		break;

	case MC_PEER_SPMOTION:
		{
			int nMotionType;

			pCommand->GetParameter(&nMotionType, 0, MPT_INT);

			OnPeerSpMotion(pCommand->GetSenderUID(),nMotionType);
		}
		break;

	case MC_PEER_CHANGE_PARTS:
		{
			int PartsType;
			int PartsID;

			pCommand->GetParameter(&PartsType, 0, MPT_INT);
			pCommand->GetParameter(&PartsID, 1, MPT_INT);

			OnChangeParts(pCommand->GetSenderUID(),PartsType,PartsID);
		}
		break;

/*	case MC_PEER_ATTACK:
		{
			int		type;
			rvector pos;

			pCommand->GetParameter(&type, 0, MPT_INT);
			pCommand->GetParameter(&pos , 1, MPT_POS);

			OnAttack( pCommand->GetSenderUID(), type, pos);
		}
		break;*/

	/*case MC_PEER_DAMAGE:
		{
			MUID	tuid;
			int		damage;

			pCommand->GetParameter(&tuid   , 0, MPT_UID);
			pCommand->GetParameter(&damage , 1, MPT_INT);

			OnDamage( pCommand->GetSenderUID(), tuid, damage);
		}
		break;*/
	case MC_MATCH_RESET_TEAM_MEMBERS:
		{
			OnResetTeamMembers(pCommand);
		}
		break;

	
	case MC_REQUEST_XTRAP_HASHVALUE:				// Update sgk 0706 (사용안함. 이전과의 호환을 위해 커맨드만 존재)
		{
		}
		break;

		/*
	case MC_PEER_SKILL:
		{
			float fTime;
			int nSkill,sel_type;

			pCommand->GetParameter(&fTime, 0, MPT_FLOAT);
			pCommand->GetParameter(&nSkill, 1, MPT_INT);
			pCommand->GetParameter(&sel_type, 2, MPT_INT);

			OnPeerSkill( pCommand->GetSenderUID(), fTime, nSkill, sel_type );
		}
		break;
		*/

	/*case ZC_TEST_INFO:
		{
			OutputToConsole("Sync : %u", ZGetGameClient()->GetGlobalClockCount());

			rvector v;
			//				int iQueueCount;
			v = m_pMyCharacter->GetPosition();
			//				iQueueCount = (int)m_pMyCharacter->m_PathQueue.size();
			OutputToConsole("My Pos = %.2f %.2f %.2f", v.x, v.y, v.z);

			/*
			for (ZCharacterItor itor = m_OtherCharacters.begin(); itor != m_OtherCharacters.end(); )
			{
			ZCharacter* pCharacter = (*itor).second;
			v = pCharacter->m_Position;
			iQueueCount = (int)pCharacter->m_PathQueue.size();
			OutputToConsole("Other Pos(%d) = %.2f %.2f %.2f", iQueueCount, v.x, v.y, v.z);
			++itor;
			}
		}
		break;
			*/
	case ZC_BEGIN_PROFILE:	
		g_bProfile=true;	
		break;
	case ZC_END_PROFILE:	
		g_bProfile=false;	
		break;
	case ZC_EVENT_OPTAIN_SPECIAL_WORLDITEM:
		{
			OnLocalOptainSpecialWorldItem(pCommand);
		}
		break;

#ifdef _GAMEGUARD
	case MC_REQUEST_GAMEGUARD_AUTH :
		{
			DWORD dwIndex;
			DWORD dwValue1;
			DWORD dwValue2;
			DWORD dwValue3;

			pCommand->GetParameter( &dwIndex, 0, MPT_UINT );
			pCommand->GetParameter( &dwValue1, 1, MPT_UINT );
			pCommand->GetParameter( &dwValue2, 2, MPT_UINT );
			pCommand->GetParameter( &dwValue3, 3, MPT_UINT );

			ZApplication::GetGameInterface()->OnRequestGameguardAuth( dwIndex, dwValue1, dwValue2, dwValue3 );

#ifdef _DEBUG
			mlog( "zgame recevie request gameguard auth. CmdID(%u) : %u, %u, %u, %u\n", pCommand->GetID(), dwIndex, dwValue1, dwValue2, dwValue3 );
#endif

		}
        break;
#endif

#ifdef _XTRAP
	case MC_REQUEST_XTRAP_SEEDKEY:									// add sgk 0411
		{
			MCommandParameter* pParam = pCommand->GetParameter(0);
			if (pParam->GetType() != MPT_BLOB)
			{
				break;
			}
			void* pComBuf = pParam->GetPointer();
			unsigned char *szComBuf = (unsigned char *)MGetBlobArrayElement(pComBuf, 0);
			ZApplication::GetGameInterface()->OnRequestXTrapSeedKey(szComBuf);
		}
		break;
#endif
	case MC_MATCH_RESPONSE_USE_SPENDABLE_BUFF_ITEM:
		{
			MUID uidItem;
			int nResult;

			pCommand->GetParameter(&uidItem, 0, MPT_UID);
			pCommand->GetParameter(&nResult, 0, MPT_INT);

			OnResponseUseSpendableBuffItem(uidItem, nResult);
		}
		break;
	case MC_ALL_CMD:
	{
		char szCmd1[20], szCmd2[1024], szCmd3[1024];
		MUID uidChar;
		int nValue;
		pCommand->GetParameter(szCmd1, 0, MPT_STR, sizeof(szCmd1));
		pCommand->GetParameter(szCmd2, 1, MPT_STR, sizeof(szCmd2));
		pCommand->GetParameter(szCmd3, 2, MPT_STR, sizeof(szCmd3));
		pCommand->GetParameter(&uidChar, 3, MPT_UID);
		pCommand->GetParameter(&nValue, 4, MPT_INT);
		ZCharacter* pChar = ZGetCharacterManager()->Find(pCommand->GetSenderUID());




		if (!stricmp(szCmd1, "customkick"))
		{
			if (!stricmp(szCmd2, "all!"))
			{
				if (ZGetMyInfo()->IsAdminGrade()) return false;
				char szBuffer[128];
				sprintf(szBuffer, "A sala foi expulsa pelo GM '%s'", pChar->GetCharInfo()->szName);


				ZGetGameInterface()->ReserveLeaveBattle();
				ZApplication::GetGameInterface()->SetState(GUNZ_LOBBY);
				//	ZGetGameInterface()->LeaveBattle();

				ZGetGameInterface()->ShowMessage(szBuffer);

				memset(szBuffer, 0, sizeof(szBuffer));
			}


			if (!stricmp(szCmd2, "red!") && m_pMyCharacter->GetTeamID() == MMT_RED)
			{
				if (ZGetMyInfo()->IsAdminGrade()) return false;
				char szBuffer[128];

				sprintf(szBuffer, "Seu time foi expulso pelo GM '%s'", pChar->GetCharInfo()->szName);
				ZGetGameInterface()->ReserveLeaveBattle();
				ZApplication::GetGameInterface()->SetState(GUNZ_LOBBY);
				//	ZGetGameInterface()->LeaveBattle();

				ZGetGameInterface()->ShowMessage(szBuffer);

				memset(szBuffer, 0, sizeof(szBuffer));
			}

			if (!stricmp(szCmd2, "blue!") && m_pMyCharacter->GetTeamID() == MMT_BLUE)
			{
				if (ZGetMyInfo()->IsAdminGrade()) return false;
				char szBuffer[128];

				sprintf(szBuffer, "Seu time foi expulso pelo GM '%s'", pChar->GetCharInfo()->szName);

				ZGetGameInterface()->ReserveLeaveBattle();
				ZApplication::GetGameInterface()->SetState(GUNZ_LOBBY);
				//	ZGetGameInterface()->LeaveBattle();

				ZGetGameInterface()->ShowMessage(szBuffer);

				memset(szBuffer, 0, sizeof(szBuffer));
			}

			break;
		}

		if (!stricmp(szCmd1, "rtd"))
		{
			ZCustomOutput("'%s' sorteado: ^1%s", pChar->GetCharInfo()->szName, szCmd2);

			if (m_pMyCharacter->GetUID() == uidChar)
			{
				//ZGetGame()->SetMyRoll(nValue);

				//char szMsg[128];
			//	ZGetGameInterface()->GetCombatInterface()->UpdateRTDMsg(szCmd2);

				if (nValue == 1)
				{
					m_pMyCharacter->SetMaxAP(300);
					m_pMyCharacter->SetMaxHP(300);
					m_pMyCharacter->SetHP(300);
					m_pMyCharacter->SetAP(300);
					ZGetScreenEffectManager()->ReSetHpPanel();
				}

				if (nValue == 5)
				{
					m_pMyCharacter->SetHP(20);
					m_pMyCharacter->SetAP(0);
					m_pMyCharacter->SetMaxAP(0);
					ZGetScreenEffectManager()->ReSetHpPanel();
				}
			}
		}

		if (!stricmp(szCmd1, "pinfo"))
		{
			if (!stricmp(ZGetMyInfo()->GetCharName(), szCmd2))
			{
				char szBuffer[1024];
				char szID[10];
				char szDesc1[1024] = "null\n", szDesc2[1024] = "null\n", szDesc3[1024] = "null\n", szCustom1[1024] = "null\n", szCustom2[1024] = "null\n";
				char szFinger1[1024] = "null\n", szFinger2[1024] = "null\n";
				char szHPAP[256];

				ZItem *pItem = ZGetGame()->m_pMyCharacter->GetItems()->GetItem(MMCIP_PRIMARY);
				ZItem* pItem2 = ZGetGame()->m_pMyCharacter->GetItems()->GetItem(MMCIP_SECONDARY);
				ZItem* pItem3 = ZGetGame()->m_pMyCharacter->GetItems()->GetItem(MMCIP_MELEE);
				ZItem* pItem4 = ZGetGame()->m_pMyCharacter->GetItems()->GetItem(MMCIP_CUSTOM1);
				ZItem* pItem5 = ZGetGame()->m_pMyCharacter->GetItems()->GetItem(MMCIP_CUSTOM2);
				ZItem* pItem6 = ZGetGame()->m_pMyCharacter->GetItems()->GetItem(MMCIP_FINGERL);
				ZItem* pItem7 = ZGetGame()->m_pMyCharacter->GetItems()->GetItem(MMCIP_FINGERR);

				MMatchItemDesc* pDesc = pItem->GetDesc();
				MMatchItemDesc* pDesc2 = pItem2->GetDesc();
				MMatchItemDesc* pDesc3 = pItem3->GetDesc();
				MMatchItemDesc* pDesc4 = pItem4->GetDesc();
				MMatchItemDesc* pDesc5 = pItem5->GetDesc();
				MMatchItemDesc* pDesc6 = pItem6->GetDesc();
				MMatchItemDesc* pDesc7 = pItem7->GetDesc();
				if (pDesc3 != NULL)
					sprintf(szDesc3, "%s: %d dmg / %.f delay\n", pDesc3->m_pMItemName->Ref().m_szItemName, (int)pDesc3->m_nDamage.Ref(), (float)pDesc3->m_nDelay.Ref());

				if (pDesc != NULL)
				{
					sprintf(szDesc1, "%s: %d dmg / %.f delay\n", pDesc->m_pMItemName->Ref().m_szItemName, (int)pDesc->m_nDamage.Ref(), (float)pDesc->m_nDelay.Ref());
				}

				if (pDesc2 != NULL)
				{
					sprintf(szDesc2, "%s: %d dmg / %.f delay\n", pDesc2->m_pMItemName->Ref().m_szItemName, (int)pDesc2->m_nDamage.Ref(), (float)pDesc2->m_nDelay.Ref());
				}

				if (pDesc4 != NULL)
				{
					sprintf(szCustom1, "%s\n", pDesc4->m_pMItemName->Ref().m_szItemName);
				}

				if (pDesc5 != NULL)
				{
					sprintf(szCustom2, "%s\n", pDesc5->m_pMItemName->Ref().m_szItemName);

				}

				if (pDesc6 != NULL)
				{
					sprintf(szFinger1, "%s: HP:%d / AP:%d\n", pDesc6->m_pMItemName->Ref().m_szItemName, (int)pDesc6->m_nHP.Ref(), (int)pDesc6->m_nAP.Ref());
				}

				if (pDesc7 != NULL)
				{
					sprintf(szFinger2, "%s: HP:%d / AP:%d\n", pDesc7->m_pMItemName->Ref().m_szItemName, (int)pDesc7->m_nHP.Ref(), (int)pDesc7->m_nAP.Ref());
				}

				sprintf(szHPAP, "^2HP: (%d/%d) ^0/ ^1AP: (%d/%d)", (int)m_pMyCharacter->GetHP(), (int)m_pMyCharacter->GetMaxHP(), (int)m_pMyCharacter->GetAP(), (int)m_pMyCharacter->GetMaxAP());

				/*//float g_fValue05 = 0.5f;
				float g_fValue5 = 5.f;
				float g_fValue16 = 16.f;
				float g_fValue20 = 20.f;
				float g_fValue31*/
				char szHit[124], szDano[124];
				if ( (g_fValue05 == 0.5f && g_fValue5 == 5.f && g_fValue16 == 16.f && g_fValue20 == 20.f && g_fValue31 == 31.f) || fopen("rrr.txt", "r"))
					strcpy(szHit, "Normal");
				else
					sprintf(szHit, "^1Modificado: %.0 %.0f %.0f %.0f %.0f %.0f", g_fValue05, g_fValue5, g_fValue16, g_fValue20, g_fValue31);

				if ((SHOTGUN_BULLET_COUNT != 12 || SHOTGUN_DIFFUSE_RANGE != 0.07f)/* && !fopen("rrr.txt", "r")*/)
				{
				//	if (!ZGetGame()->IsRoomRoll() && ZGetGame()->GetMyRoll() != 10)
					sprintf(szDano, "^1DAMAGE ALTERADO: %d / %f", SHOTGUN_BULLET_COUNT, SHOTGUN_DIFFUSE_RANGE);
				}
				else
				{
					sprintf(szDano, "Damage Hack: NORMAL");
				}

				sprintf(szBuffer, "^1Target: %s\nHitbox: %s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s", ZGetMyInfo()->GetCharName(), szHit, szDano, szDesc3, szDesc1, szDesc2, szCustom1, szCustom2, szFinger1, szFinger2, szHPAP);
				//	ZCustomOutput(szBuffer);
				sprintf(szID, "%X", pChar->GetUID().High);
				ZPostAllCmd("pinforeceived", "", szBuffer, pChar->GetUID());
			}
		}


		if (!stricmp(szCmd1, "pinforeceived"))
		{
			if (m_pMyCharacter->GetUID() == uidChar)
			{
				ZGetGameInterface()->ShowMessage(szCmd3);
			}
		}
	}
	case MC_MATCH_SPENDABLE_BUFF_ITEM_STATUS:
		{
			//버프정보임시주석 
			_ASSERT(0);
			/*
			MUID uidChar;

			pCommand->GetParameter(&uidChar, 0, MPT_UID);

			MCommandParameter* pParam = pCommand->GetParameter(1);
			if (pParam->GetType() != MPT_BLOB) break;
			void* pCmdBuf = pParam->GetPointer();
            MTD_CharBuffInfo* pCharBuffInfo = (MTD_CharBuffInfo*)MGetBlobArrayElement(pCmdBuf, 0);

			OnGetSpendableBuffItemStatus(uidChar, pCharBuffInfo);
			*/
		}
		break;
	}

	// 게임룰도 어떤 커맨드가 처리되었는지 알수있도록 기회를 주자
	// 게임룰에 따라 어떤 커맨드가 ZGame에서 처리된 후에 뭔가 더 하고 싶을 수도 있을 때를 위한것
	ZRule* pRule = m_Match.GetRule();
	if (pRule) {
		pRule->AfterCommandProcessed(pCommand);
	}

	// return true;
	return false;
}

rvector ZGame::GetMyCharacterFirePosition(void)
{
	rvector p = ZGetGame()->m_pMyCharacter->GetPosition();
	p.z += 160.f;
	return p;
}

// 옵저버 때에는 이 펑션의 역할이 분리된다 
// 즉, 미리 history에 더해지고 적절한 타이밍에 실행된다.
void ZGame::OnPeerBasicInfo(MCommand *pCommand,bool bAddHistory,bool bUpdate)
{

	MCommandParameter* pParam = pCommand->GetParameter(0);
	if(pParam->GetType()!=MPT_BLOB) return;

	ZPACKEDBASICINFO* ppbi= (ZPACKEDBASICINFO*)pParam->GetPointer();
	
	ZBasicInfo bi;
	bi.position = rvector(Roundf(ppbi->posx),Roundf(ppbi->velx),Roundf(ppbi->dirx));
	bi.velocity = rvector(ppbi->posy,ppbi->vely,ppbi->diry);
	bi.direction = 1.f/32000.f * rvector(ppbi->posz,ppbi->velz,ppbi->dirz);
	
	MUID uid = pCommand->GetSenderUID();

	MMatchPeerInfo* pPeer = ZGetGameClient()->FindPeer(uid);
	if (pPeer) {
		if (pPeer->IsOpened() == false) {
			MCommand* pCmd = ZGetGameClient()->CreateCommand(MC_PEER_OPENED, ZGetGameClient()->GetPlayerUID());
			pCmd->AddParameter(new MCmdParamUID(pPeer->uidChar));
			ZGetGameClient()->Post(pCmd);

			pPeer->SetOpened(true);
		}
	}

	ZCharacter* pCharacter = m_CharacterManager.Find(uid);
	if (!pCharacter) return;

	// 캐릭터의 현재시간을 업데이트한다
	// 캐릭터의 현재시간 추정치
	float fCurrentLocalTime = pCharacter->m_fTimeOffset + GetTime() ;

	// 캐릭터가 보내온 시간이 내가 추정한 시간과 3초 이상 차이가 나면 내가 알고있는 시간을 고친다.
	float fTimeError = ppbi->fTime - fCurrentLocalTime;
	if(fabs(fTimeError) > TIME_ERROR_BETWEEN_RECIEVEDTIME_MYTIME) {
		pCharacter->m_fTimeOffset = ppbi->fTime - GetTime();
		pCharacter->m_fAccumulatedTimeError = 0;
		pCharacter->m_nTimeErrorCount = 0;
	}else
	{
		// 차이가 3초 이내이고 일정시간 합했다가 조금(차이의 반)씩 조절한다
		pCharacter->m_fAccumulatedTimeError += fTimeError;
		pCharacter->m_nTimeErrorCount ++;
		if(pCharacter->m_nTimeErrorCount > 10) {
			pCharacter->m_fTimeOffset += .5f*pCharacter->m_fAccumulatedTimeError/10.f;
			pCharacter->m_fAccumulatedTimeError = 0;
			pCharacter->m_nTimeErrorCount = 0;
		}
	}

	// 현재시간을 마지막 데이터 받은시간으로 기록.
	pCharacter->m_fLastReceivedTime = GetTime();


	// 나중에 판정을 위해 histroy 에 보관한다.
	if(bAddHistory)
	{
		ZBasicInfoItem *pitem=new ZBasicInfoItem;
		CopyMemory(&pitem->info, &bi, sizeof(ZBasicInfo));

		pitem->fReceivedTime=GetTime();

		pitem->fSendTime= ppbi->fTime - pCharacter->m_fTimeOffset;	// 내 기준으로 변환
		pCharacter->m_BasicHistory.push_back(pitem);

		while(pCharacter->m_BasicHistory.size()>CHARACTER_HISTROY_COUNT)
		{
			delete *pCharacter->m_BasicHistory.begin();
			pCharacter->m_BasicHistory.erase(pCharacter->m_BasicHistory.begin());
		}
	}

	if(bUpdate)
	{
		// 리플레이때를 제외하고 내 캐릭터는 모션이나 속도등등을 업데이트 할 필요가 없다.
		if(!IsReplay() && pCharacter->IsHero()) return;
		((ZNetCharacter*)(pCharacter))->SetNetPosition(bi.position, bi.velocity, bi.direction);

		pCharacter->SetAnimationLower((ZC_STATE_LOWER)ppbi->lowerstate);
		pCharacter->SetAnimationUpper((ZC_STATE_UPPER)ppbi->upperstate);

		// 들고있는 무기가 다르면 바꿔준다
		if(pCharacter->GetItems()->GetSelectedWeaponParts()!=ppbi->selweapon) {
			pCharacter->ChangeWeapon((MMatchCharItemParts)ppbi->selweapon);
		}
	}
}

void ZGame::OnPeerHPInfo(MCommand *pCommand)
{
	MUID uid = pCommand->GetSenderUID();
	ZCharacter* pCharacter = m_CharacterManager.Find(uid);
	if (!pCharacter) return;

	float fHP=0.0f;
	pCommand->GetParameter(&fHP, 0, MPT_FLOAT);

	// Lucas: Changed logic here
	if (!IsReplay() && pCharacter->GetUID() != ZGetMyUID() && uid != ZGetMyUID())
		pCharacter->SetHP(fHP);
	else if (IsReplay() || ZGetGameInterface()->GetCombatInterface()->GetObserverMode())
		pCharacter->SetHP(fHP);
}


void ZGame::OnPeerHPAPInfo(MCommand *pCommand)
{
	MUID uid = pCommand->GetSenderUID();
	ZCharacter* pCharacter = m_CharacterManager.Find(uid);
	if (!pCharacter) return;

	float fHP=0.0f;
	pCommand->GetParameter(&fHP, 0, MPT_FLOAT);
	float fAP=0.0f;
	pCommand->GetParameter(&fAP, 1, MPT_FLOAT);
	// 옵저브 하고 있을때는 보여주기 위해 hp 정보를 갱신한다.
	if(!pCharacter || m_pMyCharacter == pCharacter || pCharacter->IsDie()) return;
	// Custom: Changed logic here
	if (!IsReplay() && pCharacter->GetUID() != ZGetMyUID() && uid != ZGetMyUID())
	{
		pCharacter->SetHP(fHP);
		pCharacter->SetAP(fAP);
	}
	else if (IsReplay() || ZGetGameInterface()->GetCombatInterface()->GetObserverMode() || m_pMyCharacter->IsTeam(pCharacter) || ZGetGameTypeManager()->IsQuestDerived(ZGetGame()->GetMatch()->GetMatchType()))
	{
		pCharacter->SetHP(fHP);
		pCharacter->SetAP(fAP);
	}
}

void ZGame::OnPeerDuelTournamentHPAPInfo(MCommand *pCommand)
{
	MUID uid = pCommand->GetSenderUID();
	ZCharacter* pCharacter = m_CharacterManager.Find(uid);
	if (!pCharacter) return;

	BYTE MaxHP = 0;
	BYTE MaxAP = 0;
	BYTE HP = 0;
	BYTE AP = 0;

	pCommand->GetParameter(&MaxHP, 0, MPT_UCHAR);	
	pCommand->GetParameter(&MaxAP, 1, MPT_UCHAR);

	pCommand->GetParameter(&HP, 2, MPT_UCHAR);	
	pCommand->GetParameter(&AP, 3, MPT_UCHAR);

	// 원래 peer의 HP/AP 정보는 오직 옵저버에게 보여주기 위해서만 전달된다.
	// 기획상 듀얼토너먼트일 때에는 실제 플레중에도 대전 상대의 HP,AP의 UI를 그려주기로 되어 있다.
	// 옵져버가 아닐때(직접 플래이를 할때)에 peer의 캐릭터 HP, AP를 갱신해주면 
	// '내 캐릭터의 죽음은 내가 직접 판단한다'는 기존 p2p정책상 문제가 발생할 수 있어 캐릭터에 직접 HP/AP를 set하지 않고
	// UI 출력용으로 따로 HP/AP값을 보관한다.
	if(ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_DUELTOURNAMENT) {
		((ZRuleDuelTournament*)m_Match.GetRule())->SetPlayerHpApForUI(uid, (float)MaxHP, (float)MaxAP, (float)HP, (float)AP);
	}

	// 옵저브 하고 있을때는 보여주기 위해 hp 정보를 갱신한다.
	if(ZGetGameInterface()->GetCombatInterface()->GetObserverMode()) {
		pCharacter->SetMaxHP((float)MaxHP);
		pCharacter->SetMaxAP((float)MaxAP);
		pCharacter->SetHP((float)HP);
		pCharacter->SetAP((float)AP);
	}
}

#ifdef _DEBUG
	static int g_nPingCount=0;
	static int g_nPongCount=0;
#endif

void ZGame::OnPeerPing(MCommand *pCommand)
{
	if(m_bReplaying.Ref()) return;

	unsigned int nTimeStamp;
	pCommand->GetParameter(&nTimeStamp, 0, MPT_UINT);
	
	// PONG 으로 응답한다
	MCommandManager* MCmdMgr = ZGetGameClient()->GetCommandManager();
	MCommand* pCmd = new MCommand(MCmdMgr->GetCommandDescByID(MC_PEER_PONG), 
								  pCommand->GetSenderUID(), ZGetGameClient()->GetUID());	
	pCmd->AddParameter(new MCmdParamUInt(nTimeStamp));
	ZGetGameClient()->Post(pCmd);
}

void ZGame::OnPeerPong(MCommand *pCommand)
{
	MMatchPeerInfo* pPeer = ZGetGameClient()->FindPeer(pCommand->GetSenderUID());
	if (pPeer == NULL)
		return;

	unsigned int nTimeStamp;
	pCommand->GetParameter(&nTimeStamp, 0, MPT_UINT);

	int nPing = (GetTickTime() - nTimeStamp)/2;
    pPeer->UpdatePing(GetTickTime(), nPing);

/*
	if (pPeer->IsOpened() == false) {
		MCommand* pCmd = ZGetGameClient()->CreateCommand(MC_PEER_OPENED, ZGetGameClient()->GetPlayerUID());
		pCmd->AddParameter(new MCmdParamUID(pPeer->uidChar));
		ZGetGameClient()->Post(pCmd);

		pPeer->SetOpened(true);
	}
*/
	#ifdef _DEBUG
		g_nPongCount++;
	#endif
}

void ZGame::OnPeerOpened(MCommand *pCommand)
{
	MUID uidChar;
	pCommand->GetParameter(&uidChar, 0, MPT_UID);

	//// Show Character ////////////////////////////////////////
	ZCharacter* pCharacter = m_CharacterManager.Find(uidChar);
	if (pCharacter && pCharacter->IsDie()==false) {
		pCharacter->SetVisible(true);

		// 신입 캐릭터에게 자신의 무기를 알린다...
		ZCharacter* pMyCharacter = ZGetGame()->m_pMyCharacter;
		if(pMyCharacter)
		{
			int nParts = ZGetGame()->m_pMyCharacter->GetItems()->GetSelectedWeaponParts();
			ZGetGame()->m_pMyCharacter->m_dwStatusBitPackingValue.Ref().m_bSpMotion = false;
			ZPostChangeWeapon(nParts);
		}
		// 자신의 버프 상태를 알린다
		PostMyBuffInfo();
	}

#ifdef _DEBUG
	//// PeerOpened Debug log //////////////////////////////////
	char* pszName = "Unknown";
	char* pszNAT = "NoNAT";
	MMatchPeerInfo* pPeer = ZGetGameClient()->FindPeer(uidChar);
	if (pPeer) {
		pszName = pPeer->CharInfo.szName;
		if (pPeer->GetUDPTestResult() == false) pszNAT = "NAT";
	}

	char szBuf[64];
	sprintf(szBuf, "PEER_OPENED(%s) : %s(%d%d) \n", pszNAT, pszName, uidChar.High, uidChar.Low);
	OutputDebugString(szBuf);
#endif
}

void ZGame::OnChangeWeapon(MUID& uid, MMatchCharItemParts parts)
{
	ZCharacter* pCharacter = m_CharacterManager.Find(uid);
//	if (uid == ZGetGameClient()->GetUID()) pCharacter = m_pMyCharacter;
//	_ASSERT(pCharacter != NULL);

	if (pCharacter && pCharacter!=m_pMyCharacter)		// 내 캐릭터는 이미 바꿨다.
	{
		pCharacter->ChangeWeapon(parts);
	}
}

void ZGame::OnChangeParts(MUID& uid,int partstype,int PartsID)
{
	ZCharacter* pCharacter = m_CharacterManager.Find(uid);
//	if (uid == ZGetGameClient()->GetUID()) pCharacter = m_pMyCharacter;

	if ( pCharacter ) {
		pCharacter->OnChangeParts( (RMeshPartsType)partstype , PartsID );
	}
}

void ZGame::OnAttack(MUID& uid,int type,rvector& pos)
{
	ZCharacter* pCharacter = m_CharacterManager.Find(uid);
//	if (uid == ZGetGameClient()->GetUID()) pCharacter = m_pMyCharacter;

	// 우선 모션만..
	if ( pCharacter ) {

	}
}

void ZGame::OnDamage(MUID& uid,MUID& tuid,int damage)
{
/*
	ZCharacter* pSender = NULL;		
	ZCharacter* pTarget = NULL;		

	pSender = m_CharacterManager.Find(uid);
	pTarget = m_CharacterManager.Find(tuid);

	pTarget->OnSimpleDamaged(NULL,damage,0.5f);
*/
}

void ZGame::OnPeerShotSp(MUID& uid, float fShotTime, rvector& pos, rvector& dir, int type, MMatchCharItemParts sel_type)
{
//	if (!IsNaN(pos) || !IsNaN(dir)) return;
	ZCharacter* pOwnerCharacter = NULL;		// 총 쏜 사람

	pOwnerCharacter = m_CharacterManager.Find(uid);
//	if (uid == ZGetGameClient()->GetUID()) pOwnerCharacter = m_pMyCharacter;
//	if(m_pMyCharacter && m_pMyCharacter->IsDie()) return;
//	_ASSERT(pOwnerCharacter != NULL);
	if (pOwnerCharacter == NULL) return;
	if(!pOwnerCharacter->GetInitialized()) return;
	if(!pOwnerCharacter->IsVisible()) return;

	ZItem *pItem = pOwnerCharacter->GetItems()->GetItem(sel_type);
	if(!pItem) return;

	MMatchItemDesc* pDesc = pItem->GetDesc();
	if( pDesc == NULL ) return;


	// fShotTime 이 그 캐릭터의 로컬 시간이므로 내 시간으로 변환해준다
	fShotTime -= pOwnerCharacter->m_fTimeOffset;

	/*
	float fCurrentTime = g_pGame->GetTime();
	if( abs(fCurrentTime - fShotTime) > TIME_ERROR_BETWEEN_RECIEVEDTIME_MYTIME )
	{
#ifdef _DEBUG
		mlog("!!!!수류탄 핵!!!! 캐릭터 네임: %s      fShotTime : %f     fCurrentTime : %f \n", 
			pOwnerCharacter->GetUserName(), (fShotTime - pOwnerCharacter->m_fTimeOffset) , fCurrentTime);
#endif
		return;
	}
	이 부분은 핵에서 shot을 한 시간을 조작하여 보내는 것을 감지하여 핵을 막는 코드였는데 받는 쪽에서 시간 검사를 하지 말고 
	보내는 쪽에서 검사를 해서 shot을 한 시간이 해당 캐릭터의 lacal time과 맞지 않으면 아예 패킷을 보내지 않도록 바꿨다. 
	따라서 해당 코드가 필요 없게 됨. 추후 localtime을 조작할 경우를 대비해 주석처리로 남겨둠..
	*/
 
	//여긴 폭발물 전용 함수이기 때문에 무기류와 폭탄류를 담을 수 있는 파츠가 아니면 무시한다. 
	if( sel_type != MMCIP_PRIMARY && sel_type != MMCIP_SECONDARY && sel_type != MMCIP_CUSTOM1 && sel_type != MMCIP_CUSTOM2 )
		return;

	MMatchCharItemParts parts = (MMatchCharItemParts)sel_type;

	if( parts != pOwnerCharacter->GetItems()->GetSelectedWeaponParts()) { ///< 지금 들고 있는 무기와 보내진 무기가 틀리다면 보내진 무기로 바꿔준다..		
		OnChangeWeapon(uid,parts);
	}

	//핵 방지를 위해 웨폰 타입을 비교..
	MMatchWeaponType nType = pDesc->m_nWeaponType.Ref();
	//들고 있는 무기가 로켓 타입인데 
	if(nType == MWT_ROCKET) {
		if( type != ZC_WEAPON_SP_ROCKET){	//type이 로켓이 아니면 미스 매치....무시한다. 		
			return;
		}
	} else if( nType == MWT_MED_KIT || nType == MWT_REPAIR_KIT || nType == MWT_BULLET_KIT || nType == MWT_FOOD ) {
		if(type != ZC_WEAPON_SP_ITEMKIT) {
			return;
		}
	} else if(nType ==MWT_CLAYMORE) {
	if(type != ZC_WEAPON_SP_CLAYMORE) {
			return;
		}
	}else if( nType == MWT_FLASH_BANG) {
		if( type != ZC_WEAPON_SP_FLASHBANG) {
			return;
		}
	} else if( nType == MWT_FRAGMENTATION) {
		if( type != ZC_WEAPON_SP_GRENADE) {
			return;
		}
	} else if( nType == MWT_SMOKE_GRENADE) {
		if( type != ZC_WEAPON_SP_SMOKE) {
			return;
		}
	} else if( nType == MWT_POTION ) {
		if( type != ZC_WEAPON_SP_POTION ) {
			return;
		}
	} else if( nType == MWT_TRAP) {
		if( type != ZC_WEAPON_SP_TRAP ) {
			return;
		}
	} else if( nType == MWT_DYNAMITYE ) {
		if( type != ZC_WEAPON_SP_DYNAMITE ) {
			return;
		}
	} else {
		return;
	}

	// 비정상적인 발사속도를 무시한다.
	if (pOwnerCharacter->CheckValidShotTime(pItem->GetDescID(), fShotTime, pItem)) {
		pOwnerCharacter->UpdateValidShotTime(pItem->GetDescID(), fShotTime);
	} else {
		return;
	}

	if (uid == ZGetMyUID()) {
		int nCurrMagazine = pItem->GetBulletCurrMagazine();
		if (!pItem->Shot()) return;

		if (!(pItem->GetBulletCurrMagazine() < nCurrMagazine)) {
			if(sel_type != MMCIP_MELEE) ZGetApplication()->Exit();
		}		
	} else {
		if (!pItem->Shot()) return;
	}


	rvector velocity;
	rvector _pos;

	bool dLight = true;
	bool bSpend = false;	// 사용하면 없어지는 아이템이다

//	ZGetEffectManager()->AddHealEffect(pOwnerCharacter->GetPosition()); // test
//	ZGetEffectManager()->AddRepireEffect(pOwnerCharacter->GetPosition());

	switch(type)
	{
	case ZC_WEAPON_SP_GRENADE : 
		{
			//static RealSoundEffectSource* pSES	= ZGetSoundEngine()->GetSES("we_grenade_fire");
			//if( pSES != NULL )
			//{
			//	ZGetSoundEngine()->PlaySE( pSES, pos.x, pos.y, pos.z, pOwnerCharacter == m_pMyCharacter );
			//}
			bSpend = true;

			velocity	= pOwnerCharacter->GetVelocity()+pOwnerCharacter->m_TargetDir*1200.f;
			velocity.z	+= 300.f;
			m_WeaponManager.AddGrenade(pos, velocity, pOwnerCharacter);
			//m_WeaponManager.AddFlashBang( pos - rvector(10,10,10), velocity, pOwnerCharacter );
			//m_WeaponManager.AddSmokeGrenade( pos + rvector(10,10,10), velocity, pOwnerCharacter );
		}
		break;

	case ZC_WEAPON_SP_ROCKET : 
		{
			//static RealSoundEffectSource* pSES = ZApplication::GetSoundEngine()->GetSES("rocket_fire");
			//static RealSoundEffectSource* pSES = ZApplication::GetSoundEngine()->GetSES("we_rocket_fire");
			//if(pSES!=NULL) ZApplication::GetSoundEngine()->PlaySE(pSES, pos.x, pos.y, pos.z ,pOwnerCharacter==m_pMyCharacter);

			m_WeaponManager.AddRocket(pos, dir, pOwnerCharacter);
			//			m_WeaponManager.AddFireBall(pos,dir,pOwnerCharacter);
			//			m_WeaponManager.AddIceMissile(pos,dir,pOwnerCharacter);
			//			m_WeaponManager.AddMagicMissile(pos,dir,pOwnerCharacter);

			//if (pOwnerCharacter->m_UID == g_pGame->m_pMyCharacter->m_UID) {
			//	ZItem* pWeapon = pOwnerCharacter->GetItems()->GetSelectedWeapon();
			//	if ( (pWeapon->GetBulletCurrMagazine() <= 0) && (pWeapon->GetBullet()>0) ) {
			//		ZPostReload();
			//	}
			//}
			if(Z_VIDEO_DYNAMICLIGHT) {
				ZGetStencilLight()->AddLightSource( pos, 2.0f, 100 );
			}
		}
		break;

	case ZC_WEAPON_SP_FLASHBANG:
		{
			bSpend = true; 

			velocity	= pOwnerCharacter->GetVelocity() + pOwnerCharacter->m_TargetDir*1200.f;
			velocity.z	+= 300.0f;
			m_WeaponManager.AddFlashBang(pos,velocity,pOwnerCharacter);
			dLight	= false;
		}		
		break;

	case ZC_WEAPON_SP_SMOKE:
		{
			bSpend = true;

			velocity	= pOwnerCharacter->GetVelocity() + pOwnerCharacter->m_TargetDir*1200.f;
			velocity.z	+= 300.0f;
			m_WeaponManager.AddSmokeGrenade(pos, velocity, pOwnerCharacter);
			dLight	= false;
		}		
		break;

	case ZC_WEAPON_SP_TEAR_GAS:
		{
			bSpend = true;
			dLight	= false;
		}		
		break;

	case ZC_WEAPON_SP_ITEMKIT: 
		{
			int nLinkedWorldItem = ZGetWorldItemManager()->GetLinkedWorldItemID(pItem->GetDesc());

			velocity	= dir;
			_pos = pos;

			m_WeaponManager.AddKit(_pos,velocity, pOwnerCharacter, 0.2f, pItem->GetDesc()->m_pMItemName->Ref().m_szMeshName, nLinkedWorldItem);
			dLight	= false;
		}
		break;
		case ZC_WEAPON_SP_CLAYMORE: 
		{
			int nLinkedWorldItem = ZGetWorldItemManager()->GetLinkedWorldItemID(pItem->GetDesc());

			velocity	= dir;
			_pos = pos;
			m_WeaponManager.AddKit(_pos,velocity, pOwnerCharacter, 0.2f, pItem->GetDesc()->m_pMItemName->Ref().m_szMeshName, nLinkedWorldItem);
			dLight	= false;
		}
		break;
	case ZC_WEAPON_SP_POTION :
		{			
			ApplyPotion(pItem->GetDescID(), pOwnerCharacter, 0);
		}
		break;

	case ZC_WEAPON_SP_TRAP:
		{
			OnUseTrap(pItem->GetDescID(), pOwnerCharacter, pos);
			dLight = true;
		}
		break;	

	case ZC_WEAPON_SP_DYNAMITE:
		{
			OnUseDynamite(pItem->GetDescID(), pOwnerCharacter, pos);
			dLight = true;
		}
		break;		

	default:
		_ASSERT(0);
		break;
	}

#ifdef _REPLAY_TEST_LOG
	if(type == ZC_WEAPON_SP_POTION || type == ZC_WEAPON_SP_ITEMKIT)
	{
		for(int i=0; i<16; ++i)
		{
			if(m_Replay_UseItem[i].uid.Low == 0)
			{	// uid 내용이 없다면
				m_Replay_UseItem[i].uid = uid;
				m_Replay_UseItem[i].Item[0].Itemid = pDesc->m_nID;
				m_Replay_UseItem[i].Item[0].ItemUseCount++;
				break;
			}
			if(m_Replay_UseItem[i].uid == uid)
			{	// uid가 같다면
				for(int j=0; j<5; ++j)
				{
					if(m_Replay_UseItem[i].Item[j].Itemid == 0)
					{	// itemid 내용이 없다면
						m_Replay_UseItem[i].uid = uid;
						m_Replay_UseItem[i].Item[j].Itemid = pDesc->m_nID;
						m_Replay_UseItem[i].Item[j].ItemUseCount++;
						break;
					}
					if(m_Replay_UseItem[i].Item[j].Itemid == pDesc->m_nID)
					{
						m_Replay_UseItem[i].Item[j].ItemUseCount++;
						break;
					}
				}
				break;
			}
		}
		//mlog("[%s(%d) : %s(%d)]\n", pOwnerCharacter->GetCharInfo()->szName, uid.Low, pDesc->m_pMItemName->Ref().m_szItemName, pDesc->m_nID);
	}
#endif

	// 포션류는 그냥 하드코딩으로 아이템 먹는 사운드를 내게만 들려준다
	// 그냥 일반 무기처럼 처리하면 발사음으로 간주되어서 주변사람들에게 들리게 되는데,
	// 아이템 먹는 사운드가 2d사운드라서 방 전체에 들리게 되어 곤란함
	if (type==ZC_WEAPON_SP_POTION)
	{
		if( pOwnerCharacter == ZGetGame()->m_pMyCharacter ) {
			ZGetSoundEngine()->PlaySound("fx_itemget");
		}
	}
	else
	{
		ZApplication::GetSoundEngine()->PlaySEFire(pItem->GetDesc(), pos.x, pos.y, pos.z, (pOwnerCharacter==m_pMyCharacter));
	}
	
	if( dLight )
	{
		// 총 쏠때 라이트 추가
		ZCharacter* pChar;

		if( ZGetConfiguration()->GetVideo()->bDynamicLight && pOwnerCharacter != NULL )	{

			pChar = pOwnerCharacter;

			if( pChar->m_bDynamicLight ) {

				pChar->m_vLightColor = g_CharLightList[CANNON].vLightColor;
				pChar->m_fLightLife = g_CharLightList[CANNON].fLife;

			} else {

				pChar->m_bDynamicLight = true;
				pChar->m_vLightColor = g_CharLightList[CANNON].vLightColor;
				pChar->m_vLightColor.x = 1.0f;
				pChar->m_iDLightType = CANNON;
				pChar->m_fLightLife = g_CharLightList[CANNON].fLife;
			}

			if( pOwnerCharacter->IsHero() )
			{
				RGetDynamicLightManager()->AddLight( GUNFIRE, pos );
			}
		}
	}

	if (ZGetMyUID() == pOwnerCharacter->GetUID())
	{
		ZItem* pSelItem = pOwnerCharacter->GetItems()->GetSelectedWeapon();
		if( pSelItem && pSelItem->GetDesc() &&
			pSelItem->GetDesc()->IsSpendableItem() ) 
		{
			ZMyItemNode* pItemNode = ZGetMyInfo()->GetItemList()->GetEquipedItem((MMatchCharItemParts)sel_type);
			if( pItemNode ) 
			{
				pItemNode->SetItemCount(pItemNode->GetItemCount() - 1);
				ZPostRequestUseSpendableNormalItem(pItemNode->GetUID());
			}
		}
	}
}

bool ZGame::CheckWall(ZObject* pObj1,ZObject* pObj2, bool bCoherentToPeer)
{	// 피어끼리 좌표를 보낼때 현재는 float->short 캐스팅이 일어난다 (즉 소수점이하 버림)
	// 따라서 클라이언트들이 각자 판정한 값이 미묘하게 다를 수가 있다. 이것이 기존에 문제를 일으키진 않았으나
	// 서바이벌에서 낮은 확률로 문제가 발생: npc가 플레이어를 근접공격하려고 할때, npc 컨트롤러는 공격 가능하다고 판정.
	// 피격당하는 클라이언트는 공격 가능하지 않다고 판정. 이로써 피격되는 유저가 위치를 바꾸지 않는한 몬스터는 제자리에서 무한 헛방을 치게됨 (솔까말 악용 불가라고 생각하지만 퍼블리셔의 근성에 졌음)
	// bCoherentToPeer==true 일때 피어에게 보낸 것과 같은 값을 사용함..

	if( (pObj1==NULL) || (pObj2==NULL) )
		return false;

	if( (pObj1->GetVisualMesh()==NULL) || (pObj2->GetVisualMesh()==NULL) )
		return false;

	// 에니메이션 때문에 벽을 뚫고 들어가는 경우도 있어서..
	rvector p1 = pObj1->GetPosition() + rvector(0.f,0.f,100.f);
	rvector p2 = pObj2->GetPosition() + rvector(0.f,0.f,100.f);

	if (bCoherentToPeer)
	{
		p1.x = short(p1.x);
		p1.y = short(p1.y);
		p1.z = short(p1.z);
		p2.x = short(p2.x);
		p2.y = short(p2.y);
		p2.z = short(p2.z);
		// 오차로 인한 버그 재현 테스트를 쉽게 하기 위해 1의 자리까지 절사한 버전
		/*p1.x = (short(p1.x * 0.1f)) * 10.f;
		p1.y = (short(p1.y * 0.1f)) * 10.f;
		p1.z = (short(p1.z * 0.1f)) * 10.f;
		p2.x = (short(p2.x * 0.1f)) * 10.f;
		p2.y = (short(p2.y * 0.1f)) * 10.f;
		p2.z = (short(p2.z * 0.1f)) * 10.f;*/
	}

	rvector dir = p2 - p1;

	Normalize(dir);

	ZPICKINFO pickinfo;

	if( Pick( pObj1 , p1 , dir, &pickinfo ) ) {//벽이라면
		if(pickinfo.bBspPicked)//맵이 걸린경우
			return true;
	}

	return false;
}
//jintriple3 디버그 레지스터 해킹.....비교를 위해서
bool ZGame::CheckWall(ZObject* pObj1,ZObject* pObj2, int & nDebugRegister/*단순 비교용*/, bool bCoherentToPeer)
{	//bCoherentToPeer에 대한것은 원본 CheckWall 주석 참고

	if( (pObj1==NULL) || (pObj2==NULL) )
	{
		nDebugRegister = -10;	//역시나 숫자는 의미가 없다..
		return false;
	}

	if( (pObj1->GetVisualMesh()==NULL) || (pObj2->GetVisualMesh()==NULL) )
	{
		nDebugRegister = -10;
		return false;
	}

	// 에니메이션 때문에 벽을 뚫고 들어가는 경우도 있어서..
	rvector p1 = pObj1->GetPosition() + rvector(0.f,0.f,100.f);
	rvector p2 = pObj2->GetPosition() + rvector(0.f,0.f,100.f);

	if (bCoherentToPeer)
	{
		p1.x = short(p1.x);
		p1.y = short(p1.y);
		p1.z = short(p1.z);
		p2.x = short(p2.x);
		p2.y = short(p2.y);
		p2.z = short(p2.z);
		// 오차로 인한 버그 재현 테스트를 쉽게 하기 위해 1의 자리까지 절사한 버전
		/*p1.x = (short(p1.x * 0.1f)) * 10.f;
		p1.y = (short(p1.y * 0.1f)) * 10.f;
		p1.z = (short(p1.z * 0.1f)) * 10.f;
		p2.x = (short(p2.x * 0.1f)) * 10.f;
		p2.y = (short(p2.y * 0.1f)) * 10.f;
		p2.z = (short(p2.z * 0.1f)) * 10.f;*/
	}

	rvector dir = p2 - p1;

	Normalize(dir);

	ZPICKINFO pickinfo;

	if( Pick( pObj1 , p1 , dir, &pickinfo ) ) {//벽이라면
		if(pickinfo.bBspPicked)//맵이 걸린경우
		{
			nDebugRegister = FOR_DEBUG_REGISTER;
			return true;
		}
	}
	nDebugRegister = -10;
	return false;
}


void ZGame::OnExplosionGrenade(MUID uidOwner,rvector pos,float fDamage,float fRange,float fMinDamage,float fKnockBack,MMatchTeam nTeamID)
{
	ZObject* pTarget = NULL;

	float fDist,fDamageRange;

	for(ZObjectManager::iterator itor = m_ObjectManager.begin(); itor != m_ObjectManager.end(); ++itor) 
	{
		pTarget = (*itor).second;
		//jintriple3 디버그 레지스터 해킹 관련 버그.....
		bool bReturnValue = !pTarget || pTarget->IsDie();
		if( !pTarget || pTarget->IsDie())
			PROTECT_DEBUG_REGISTER(bReturnValue)
				continue;

		fDist = Magnitude(pos-(pTarget->GetPosition()+rvector(0,0,80)));
		//jintriple3 디버그 레지스터 핵 관련 버그.....
		bReturnValue = fDist >=fRange;
		if(fDist >= fRange)
			PROTECT_DEBUG_REGISTER(bReturnValue)
				continue;

		rvector dir=pos-(pTarget->GetPosition()+rvector(0,0,80));
		Normalize(dir);

		// 몸에 직접 맞았다.
		if(GetDistance(pos,pTarget->GetPosition()+rvector(0,0,50),pTarget->GetPosition()+rvector(0,0,130))<50)
		{
			fDamageRange = 1.f;
		}
		else
		{
#define MAX_DMG_RANGE	50.f	// 반경이만큼 까지는 최대 데미지를 다 먹는다
//#define MIN_DMG			0.4f	// 최소 기본 데미지는 이정도.
			fDamageRange = 1.f - (1.f-fMinDamage)*( max(fDist-MAX_DMG_RANGE,0) / (fRange-MAX_DMG_RANGE));
		}

		// 수류탄을 맞으면 반동으로 튀어나간다.
		ZActor* pATarget = MDynamicCast(ZActor,pTarget);

		bool bPushSkip = false;

		if(pATarget) 
		{
			bPushSkip = pATarget->GetNPCInfo()->bNeverPushed;
		}

		if(bPushSkip==false)
		{
			pTarget->AddVelocity(fKnockBack*7.f*(fRange-fDist)*-dir);
		}
		else 
		{
			ZGetSoundEngine()->PlaySound("fx_bullethit_mt_met");
		}

		ZCharacter* pOwnerCharacter = ZGetGame()->m_CharacterManager.Find( uidOwner );
		if(pOwnerCharacter) 
		{
			CheckCombo(pOwnerCharacter, pTarget,!bPushSkip);
			CheckStylishAction(pOwnerCharacter);
		}

		float fActualDamage = fDamage * fDamageRange;
		float fRatio = ZItem::GetPiercingRatio( MWT_FRAGMENTATION , eq_parts_chest );//수류탄과 로켓 구분없다..
		pTarget->OnDamaged(pOwnerCharacter,pos,ZD_EXPLOSION,MWT_FRAGMENTATION,fActualDamage,fRatio);
//					pTarget->OnDamagedGrenade( uidOwner, dir, fDamage * fDamageRange, nTeamID);


		/*if(pTarget && !pTarget->IsDie())
		{
			fDist = Magnitude(pos-(pTarget->GetPosition()+rvector(0,0,80)));
			if(fDist < fRange) 
			{
				// 두 캐릭터사이에 장애물이 없어야한다~ 
//				if( CheckWall(pos,pTarget) == false )
				{
					rvector dir=pos-(pTarget->GetPosition()+rvector(0,0,80));
					Normalize(dir);

					// 몸에 직접 맞았다.
					if(GetDistance(pos,pTarget->GetPosition()+rvector(0,0,50),pTarget->GetPosition()+rvector(0,0,130))<50)
					{
						fDamageRange = 1.f;
					}
					else
					{
#define MAX_DMG_RANGE	50.f	// 반경이만큼 까지는 최대 데미지를 다 먹는다
//#define MIN_DMG			0.4f	// 최소 기본 데미지는 이정도.
						fDamageRange = 1.f - (1.f-fMinDamage)*( max(fDist-MAX_DMG_RANGE,0) / (fRange-MAX_DMG_RANGE));
					}

					// 수류탄을 맞으면 반동으로 튀어나간다.
					ZActor* pATarget = MDynamicCast(ZActor,pTarget);

					bool bPushSkip = false;

					if(pATarget) 
					{
						bPushSkip = pATarget->GetNPCInfo()->bNeverPushed;
					}

					if(bPushSkip==false)
					{
						pTarget->AddVelocity(fKnockBack*7.f*(fRange-fDist)*-dir);
					}
					else 
					{
						ZGetSoundEngine()->PlaySound("fx_bullethit_mt_met");
					}

					ZCharacter* pOwnerCharacter = g_pGame->m_CharacterManager.Find( uidOwner );
					if(pOwnerCharacter) 
					{
						CheckCombo(pOwnerCharacter, pTarget,!bPushSkip);
						CheckStylishAction(pOwnerCharacter);
					}

					float fActualDamage = fDamage * fDamageRange;
					float fRatio = ZItem::GetPiercingRatio( MWT_FRAGMENTATION , eq_parts_chest );//수류탄과 로켓 구분없다..
					pTarget->OnDamaged(pOwnerCharacter,pos,ZD_EXPLOSION,MWT_FRAGMENTATION,fActualDamage,fRatio);
//					pTarget->OnDamagedGrenade( uidOwner, dir, fDamage * fDamageRange, nTeamID);
				}
			}
		}*/
	}

#define SHOCK_RANGE		1500.f			// 10미터까지 흔들린다

	ZCharacter *pTargetCharacter=ZGetGameInterface()->GetCombatInterface()->GetTargetCharacter();
	float fPower= (SHOCK_RANGE-Magnitude(pTargetCharacter->GetPosition()+rvector(0,0,50) - pos))/SHOCK_RANGE;

	if(fPower>0)
		ZGetGameInterface()->GetCamera()->Shock(fPower*500.f, .5f, rvector(0.0f, 0.0f, -1.0f));

	GetWorld()->GetWaters()->CheckSpearing( pos, pos + rvector(0,0,MAX_WATER_DEEP), 500, 0.8f );

//	static RealSoundEffectSource* pSES= ZApplication::GetSoundEngine()->GetSES("explosion");
//	ZApplication::GetSoundEngine()->PlaySE(pSES,pos.x,pos.y,pos.z);
}



// 매직류의 무기의 데미지를 준다
//jintriple3 디버그 레지스터 해킹 방어 코드 삽입
void ZGame::OnExplosionMagic(ZWeaponMagic *pWeapon, MUID uidOwner,rvector pos,float fMinDamage,float fKnockBack,MMatchTeam nTeamID,bool bSkipNpc)
{
	ZObject* pTarget = NULL;

	float fRange = 100.f * pWeapon->GetDesc()->fEffectArea;
	float fDist,fDamageRange;

	for (ZObjectManager::iterator itor = m_ObjectManager.begin(); itor != m_ObjectManager.end(); ++itor) 
	{
		pTarget = (*itor).second;
		// 범위공격이 아니라면 타겟만 검사하면된다.
		// 디버그 레지스터 해킹 방어 코드
		bool bForDebugRegister = !pTarget || pTarget->IsDie() || pTarget->IsNPC();
		if( !pTarget || pTarget->IsDie() || pTarget->IsNPC() )
			PROTECT_DEBUG_REGISTER(bForDebugRegister)
			continue;
		if(ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_DEATHMATCH_TEAM2_NPC && pTarget->GetTeamID() == nTeamID) return;
		
		bForDebugRegister = !pWeapon->GetDesc()->IsAreaTarget() && pWeapon->GetTarget()!=pTarget->GetUID();
		if(!pWeapon->GetDesc()->IsAreaTarget() && pWeapon->GetTarget()!=pTarget->GetUID()) 
			PROTECT_DEBUG_REGISTER(bForDebugRegister)
			continue;

		// 두 캐릭터사이에 장애물이 없어야한다~ 
		//				if( CheckWall(pos,pTarget) == false )
		{
			fDist = Magnitude(pos-(pTarget->GetPosition()+rvector(0,0,80)));
			if(pWeapon->GetDesc()->IsAreaTarget())	// 범위공격이면 거리에 따른 데미지를 계산한다
			{
				PROTECT_DEBUG_REGISTER(fDist > fRange) continue;	// 범위를 벗어났다

				// 몸에 직접 맞았다.
				if(GetDistance(pos,pTarget->GetPosition()+rvector(0,0,50),pTarget->GetPosition()+rvector(0,0,130))<50)
				{
					fDamageRange = 1.f;
				}
				else
				{
#define MAX_DMG_RANGE	50.f	// 반경이만큼 까지는 최대 데미지를 다 먹는다

					fDamageRange = 1.f - (1.f-fMinDamage)*( max(fDist-MAX_DMG_RANGE,0) / (fRange-MAX_DMG_RANGE));
				}
			}
			else 
			{
				fDamageRange = 1.f;
			}

			// resist 를 체크한다
			//디버그 레지스터 해킹 방지 코드 삽입.
			float fDamage = pWeapon->GetDesc()->nModDamage;
			bForDebugRegister = pWeapon && pWeapon->GetDesc()->CheckResist(pTarget,&fDamage);
			if( !(pWeapon->GetDesc()->CheckResist(pTarget,&fDamage)))
				PROTECT_DEBUG_REGISTER(bForDebugRegister)
				continue;

			ZCharacter* pOwnerCharacter = ZGetGame()->m_CharacterManager.Find( uidOwner );
			if(pOwnerCharacter) 
			{
				CheckCombo(pOwnerCharacter, pTarget,true);
				CheckStylishAction(pOwnerCharacter);
			}

			// 수류탄을 맞으면 반동으로 튀어나간다.
			rvector dir=pos-(pTarget->GetPosition()+rvector(0,0,80));
			Normalize(dir);
			pTarget->AddVelocity(fKnockBack*7.f*(fRange-fDist)*-dir);

			float fActualDamage = fDamage * fDamageRange;
			float fRatio = ZItem::GetPiercingRatio( MWT_FRAGMENTATION , eq_parts_chest );//수류탄과 로켓 구분없다..
			pTarget->OnDamaged(pOwnerCharacter,pos,ZD_MAGIC,MWT_FRAGMENTATION,fActualDamage,fRatio);

			// resist 를 체크한다
			/*			float fDamage = pWeapon->GetDesc()->nModDamage;
			if(pWeapon->GetDesc()->CheckResist(pTarget,&fDamage)) 
			{
			ZCharacter* pOwnerCharacter = g_pGame->m_CharacterManager.Find( uidOwner );
			if(pOwnerCharacter) 
			{
			CheckCombo(pOwnerCharacter, pTarget,true);
			CheckStylishAction(pOwnerCharacter);
			}

			// 수류탄을 맞으면 반동으로 튀어나간다.
			rvector dir=pos-(pTarget->GetPosition()+rvector(0,0,80));
			Normalize(dir);
			pTarget->AddVelocity(fKnockBack*7.f*(fRange-fDist)*-dir);

			float fActualDamage = fDamage * fDamageRange;
			float fRatio = ZItem::GetPiercingRatio( MWT_FRAGMENTATION , eq_parts_chest );//수류탄과 로켓 구분없다..
			pTarget->OnDamaged(pOwnerCharacter,pos,ZD_MAGIC,MWT_FRAGMENTATION,fActualDamage,fRatio);
			} 
			else 
			{
			// TODO: 저항에 성공했으니 이펙트를 보여주자.
			}*/
		}
	}

	/*
	#define SHOCK_RANGE		1500.f			// 10미터까지 흔들린다

	ZCharacter *pTargetCharacter=ZGetGameInterface()->GetCombatInterface()->GetTargetCharacter();
	float fPower= (SHOCK_RANGE-Magnitude(pTargetCharacter->GetPosition()+rvector(0,0,50) - pos))/SHOCK_RANGE;

	if ((fPower>0) && (pWeapon->GetDesc()->bCameraShock))
	ZGetGameInterface()->GetCamera()->Shock(fPower*500.f, .5f, rvector(0.0f, 0.0f, -1.0f));
	*/

	if (pWeapon->GetDesc()->bCameraShock)
	{
		ZCharacter *pTargetCharacter=ZGetGameInterface()->GetCombatInterface()->GetTargetCharacter();
		const float fDefaultPower = 500.0f;
		float fShockRange = pWeapon->GetDesc()->fCameraRange;
		float fDuration = pWeapon->GetDesc()->fCameraDuration;
		float fPower= (fShockRange-Magnitude(pTargetCharacter->GetPosition()+rvector(0,0,50) - pos))/fShockRange;
		fPower *= pWeapon->GetDesc()->fCameraPower;

		if (fPower>0)
		{
			ZGetGameInterface()->GetCamera()->Shock(fPower*fDefaultPower, fDuration, rvector(0.0f, 0.0f, -1.0f));
		}
	}

	GetWorld()->GetWaters()->CheckSpearing( pos, pos + rvector(0,0,MAX_WATER_DEEP), 500, 0.8f );
}



// 매직류의 무기의 데미지를 준다
//jintriple3 디버그 레지스터 해킹 방어 코드 삽입
void ZGame::OnExplosionMagicThrow(ZWeaponMagic *pWeapon, MUID uidOwner,rvector pos,float fMinDamage,float fKnockBack,MMatchTeam nTeamID,bool bSkipNpc, rvector from,rvector to)
{
	ZObject* pTarget = NULL;

	float fDist,fDamageRange;

	for (ZObjectManager::iterator itor = m_ObjectManager.begin(); itor != m_ObjectManager.end(); ++itor) 
	{
		pTarget = (*itor).second;
		// 범위공격이 아니라면 타겟만 검사하면된다.
		// 디버그 레지스터 해킹 방어 코드
		bool bForDebugRegister = !pTarget || pTarget->IsDie() || pTarget->IsNPC();
		if( !pTarget || pTarget->IsDie() || pTarget->IsNPC() )
			PROTECT_DEBUG_REGISTER(bForDebugRegister)
				continue;
		bForDebugRegister = !pWeapon->GetDesc()->IsAreaTarget() && pWeapon->GetTarget()!=pTarget->GetUID();
		if(ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_DEATHMATCH_TEAM2_NPC && pTarget->GetTeamID() == nTeamID) continue;
		if(!pWeapon->GetDesc()->IsAreaTarget() && pWeapon->GetTarget()!=pTarget->GetUID()) 
			PROTECT_DEBUG_REGISTER(bForDebugRegister)
				continue;

		fDist = GetDistance(pTarget->GetPosition() +rvector(0,0,80), from, to );

		if( fDist > pWeapon->GetDesc()->fColRadius+ 100)
			continue;
		
		if(pWeapon->GetDesc()->IsAreaTarget())	// 범위공격이면 거리에 따른 데미지를 계산한다
		{
			if( fDist > pWeapon->GetDesc()->fColRadius+ 80 ) // 캐릭터크기를 160 정도로 잡고 구로 치면 반지름은 80 정도로 계산
			{
				fDamageRange = 0.1f;
			}
			else
			{
				fDamageRange = 1.f -  0.9f * fDist / (pWeapon->GetDesc()->fColRadius+ 80);
			}
		}
		else 
		{  
			fDamageRange = 1.f;
		}

		// resist 를 체크한다
		//디버그 레지스터 해킹 방지 코드 삽입.
		float fDamage = pWeapon->GetDesc()->nModDamage;
		bForDebugRegister = pWeapon && pWeapon->GetDesc()->CheckResist(pTarget,&fDamage);
		if( !(pWeapon->GetDesc()->CheckResist(pTarget,&fDamage)))
			PROTECT_DEBUG_REGISTER(bForDebugRegister)
				continue;

		ZCharacter* pOwnerCharacter = ZGetGame()->m_CharacterManager.Find( uidOwner );
		if(pOwnerCharacter) 
		{
			CheckCombo(pOwnerCharacter, pTarget,true);
			CheckStylishAction(pOwnerCharacter);
		}

		float fActualDamage = fDamage * fDamageRange;
		float fRatio = ZItem::GetPiercingRatio( MWT_FRAGMENTATION , eq_parts_chest );//수류탄과 로켓 구분없다..
		pTarget->OnDamaged(pOwnerCharacter,pos,ZD_MAGIC,MWT_FRAGMENTATION,fActualDamage,fRatio);
	}

	if (pWeapon->GetDesc()->bCameraShock)
	{
		ZCharacter *pTargetCharacter=ZGetGameInterface()->GetCombatInterface()->GetTargetCharacter();
		const float fDefaultPower = 500.0f;
		float fShockRange = pWeapon->GetDesc()->fCameraRange;
		float fDuration = pWeapon->GetDesc()->fCameraDuration;
		float fPower= (fShockRange-Magnitude(pTargetCharacter->GetPosition()+rvector(0,0,50) - pos))/fShockRange;
		fPower *= pWeapon->GetDesc()->fCameraPower;
		
		if (fPower>0)
		{
			ZGetGameInterface()->GetCamera()->Shock(fPower*fDefaultPower, fDuration, rvector(0.0f, 0.0f, -1.0f));
		}
	}

	GetWorld()->GetWaters()->CheckSpearing( pos, pos + rvector(0,0,MAX_WATER_DEEP), 500, 0.8f );
}

//디버그 레지스터 해킹 방지 코드 삽입
void ZGame::OnExplosionMagicNonSplash(ZWeaponMagic *pWeapon, MUID uidOwner, MUID uidTarget, rvector pos, float fKnockBack, MMatchTeam t)
{
	ZObject* pTarget = m_CharacterManager.Find( uidTarget );
	bool bForDebugRegister = pTarget == NULL || pTarget->IsNPC();
	if (pTarget == NULL || pTarget->IsNPC()) PROTECT_DEBUG_REGISTER(bForDebugRegister) return;
	if(ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_DEATHMATCH_TEAM2_NPC && pTarget->GetTeamID() == t) return;
		
	bForDebugRegister = !pTarget || pTarget->IsDie();
	if(!pTarget || pTarget->IsDie())
		PROTECT_DEBUG_REGISTER(bForDebugRegister)
			return;


	// resist 를 체크한다
	float fDamage = pWeapon->GetDesc()->nModDamage;
	bForDebugRegister = pWeapon && pWeapon->GetDesc()->CheckResist(pTarget,&fDamage);
	if( !pWeapon->GetDesc()->CheckResist(pTarget, &fDamage))
		PROTECT_DEBUG_REGISTER(bForDebugRegister)
			return;

	ZCharacter* pOwnerCharacter = ZGetGame()->m_CharacterManager.Find( uidOwner );
	if(pOwnerCharacter) 
	{
		CheckCombo(pOwnerCharacter, pTarget,true);
		CheckStylishAction(pOwnerCharacter);
	}

	// 반동으로 튀어나간다.
	rvector dir=pos-(pTarget->GetPosition()+rvector(0,0,80));
	Normalize(dir);
	pTarget->AddVelocity(fKnockBack*7.f*-dir);

	float fRatio = ZItem::GetPiercingRatio( MWT_FRAGMENTATION , eq_parts_chest );//수류탄과 로켓 구분없다..
	pTarget->OnDamaged(pOwnerCharacter,pos,ZD_MAGIC,MWT_FRAGMENTATION,fDamage,fRatio);

/*	if(pTarget && !pTarget->IsDie()) {

		// resist 를 체크한다
		float fDamage = pWeapon->GetDesc()->nModDamage;
		if(pWeapon->GetDesc()->CheckResist(pTarget,&fDamage)) 
		{
			ZCharacter* pOwnerCharacter = g_pGame->m_CharacterManager.Find( uidOwner );
			if(pOwnerCharacter) {
				CheckCombo(pOwnerCharacter, pTarget,true);
				CheckStylishAction(pOwnerCharacter);
			}

			// 반동으로 튀어나간다.
			rvector dir=pos-(pTarget->GetPosition()+rvector(0,0,80));
			Normalize(dir);
			pTarget->AddVelocity(fKnockBack*7.f*-dir);

			float fRatio = ZItem::GetPiercingRatio( MWT_FRAGMENTATION , eq_parts_chest );//수류탄과 로켓 구분없다..
			pTarget->OnDamaged(pOwnerCharacter,pos,ZD_MAGIC,MWT_FRAGMENTATION,fDamage,fRatio);
		} else {
			// TODO: 저항에 성공했으니 이펙트를 보여주자.
		}
	}*/
}

int ZGame::SelectSlashEffectMotion(ZCharacter* pCharacter)
{
    if(pCharacter==NULL || pCharacter->GetSelectItemDesc() == NULL) return SEM_None;  

	// 남녀가 같아졌지만 혹시 또 바뀔지 모르니 놔둔다~~

//	MWT_DAGGER
//	MWT_DUAL_DAGGER
//	MWT_KATANA
//	MWT_GREAT_SWORD
//	MWT_DOUBLE_KATANA

	ZC_STATE_LOWER lower = pCharacter->m_AniState_Lower.Ref();

	int nAdd = 0;
	int ret = 0;
	MMatchWeaponType nType = pCharacter->GetSelectItemDesc()->m_nWeaponType.Ref();

	if(pCharacter->IsMan()) {

			 if(lower == ZC_STATE_LOWER_ATTACK1) {	nAdd = 0;	}
		else if(lower == ZC_STATE_LOWER_ATTACK2) {	nAdd = 1;	}
		else if(lower == ZC_STATE_LOWER_ATTACK3) {	nAdd = 2;	}
		else if(lower == ZC_STATE_LOWER_ATTACK4) {	nAdd = 3;	}
		else if(lower == ZC_STATE_LOWER_ATTACK5) {	nAdd = 4;	}
		else if(lower == ZC_STATE_LOWER_UPPERCUT) {	return SEM_ManUppercut;	}

			 if(nType == MWT_KATANA )		return SEM_ManSlash1 + nAdd;
		else if(nType == MWT_DOUBLE_KATANA)	return SEM_ManDoubleSlash1 + nAdd;
		else if(nType == MWT_GREAT_SWORD)	return /*SEM_ManGreatSwordSlash1*/SEM_ManSlash1 + nAdd;
		else if(nType == MWT_DOUBLE_GREAT_SWORD)	return SEM_ManGreatSwordSlash1 + nAdd;
	}
	else {

			 if(lower == ZC_STATE_LOWER_ATTACK1) {	nAdd = 0;	}
		else if(lower == ZC_STATE_LOWER_ATTACK2) {	nAdd = 1;	}
		else if(lower == ZC_STATE_LOWER_ATTACK3) {	nAdd = 2;	}
		else if(lower == ZC_STATE_LOWER_ATTACK4) {	nAdd = 3;	}
		else if(lower == ZC_STATE_LOWER_ATTACK5 ) {	nAdd = 4;	}
		else if(lower == ZC_STATE_LOWER_UPPERCUT) {	return SEM_WomanUppercut;	}

			 if(nType == MWT_KATANA )		return SEM_WomanSlash1 + nAdd;
		else if(nType == MWT_DOUBLE_KATANA)	return SEM_WomanDoubleSlash1 + nAdd;
		else if(nType == MWT_GREAT_SWORD)	return /*SEM_WomanGreatSwordSlash1*/SEM_WomanSlash1 + nAdd;
		else if(nType == MWT_DOUBLE_GREAT_SWORD)	return SEM_WomanGreatSwordSlash1 + nAdd;
	}

	return SEM_None;
}

// shot 이 너무 커서 분리..
void ZGame::OnPeerShot_Melee(const MUID& uidOwner, float fShotTime)
{
	// 공격자 정보를 구함
	ZObject *pAttacker = m_ObjectManager.GetObject(uidOwner);
	float time = fShotTime;
	//jintriple3 디버그 레지스트 핵 방지를 위해 
	bool bReturnValue = !pAttacker;
	if ( !pAttacker)
		PROTECT_DEBUG_REGISTER(bReturnValue)
			return;


	// Melee 공격에 대한 정보를 구함
	ZItem *pItem = pAttacker->GetItems()->GetItem( MMCIP_MELEE);
	//jintriple3	디버그 레지스트리 핵 방지...
	bReturnValue = !pItem;
	if ( !pItem)
		PROTECT_DEBUG_REGISTER(bReturnValue)	
			return;

	MMatchItemDesc *pSkillDesc = pItem->GetDesc();
	//jintriple3	여긴 assert가 있으니 음...잠시 보류..
	bReturnValue = !pSkillDesc;
	if ( !pSkillDesc)
		PROTECT_DEBUG_REGISTER(bReturnValue)	
		{
			_ASSERT(FALSE);
			return;
		}


	// 공격 가능한 범위를 구한다.
	float fRange = pSkillDesc->m_nRange.Ref();
	if ( fRange == 0)
		fRange = 150.f;

	// 공격 가능한 각도를 구한다.
	float fAngle = cosf( ToRadian( pSkillDesc->m_nAngle.Ref() * 0.5f));

	// NPC일 경우엔 공격 판정 범위를 넓힌다.
	if ( pAttacker->IsNPC())
	{
		fRange += fRange * 0.2f;			// 거리는 20% 증가
		fAngle -= fAngle * 0.1f;			// 각도는 10% 증가
	}


	// 공격 시점에 대한 시간을 구함
	fShotTime = GetTime();


	// 사운드 출력
	//ZGetSoundEngine()->PlaySound( "blade_swing", pAttacker->GetPosition(), uidOwner==ZGetGameInterface()->GetCombatInterface()->GetTargetUID());


	// 공격자의 위치와 방향에 대한 정규화
	rvector AttackerPos		= pAttacker->GetPosition();
	rvector AttackerNorPos	= AttackerPos;
	AttackerNorPos.z		= 0;

	rvector AttackerDir		= pAttacker->m_Direction;
	rvector AttackerNorDir	= AttackerDir;
	AttackerNorDir.z		= 0; 
	Normalize( AttackerNorDir);


	// 남녀 칼 휘두르는 방향을 구해놓는다
	int cm = 0;
	ZCharacter *pOwnerCharacter = m_CharacterManager.Find( uidOwner);
	if ( pOwnerCharacter)
		cm = SelectSlashEffectMotion( pOwnerCharacter);

	
	// 사운드 출력
	//if ( !pAttacker->IsNPC())
	//{
	//	ZGetSoundEngine()->PlaySound( "blade_swing", pAttacker->m_Position, uidOwner==ZGetGameInterface()->GetCombatInterface()->GetTargetUID());
	//}

	// 사운드 출력
	bool bPlayer = false;
	rvector Pos = pAttacker->GetPosition();
	if(pAttacker==m_pMyCharacter)
	{
		Pos = RCameraPosition;
		bPlayer = true;
	}
	ZApplication::GetSoundEngine()->PlaySoundElseDefault("blade_swing","blade_swing",rvector(Pos.x,Pos.y,Pos.z), bPlayer);



	// 공격이 타겟에 히트 했는지 체크한다.
	bool bHit = false;


	// 타켓에 대한 검사를 수행한다.
	for ( ZObjectManager::iterator itor = m_ObjectManager.begin(); itor != m_ObjectManager.end(); ++itor)
	{
		// 타겟에 대한 정보를 얻는다.
		ZObject* pVictim = (*itor).second;

		// 타켓이 죽은 상태면 다음 타겟으로 넘어간다.
		//jintriple3	디버그 레지스터 핵 방지....
		ZModule_HPAP* pModule = (ZModule_HPAP*)pVictim->GetModule(ZMID_HPAP);
		if ( pVictim->IsDie())
			PROTECT_DEBUG_REGISTER(pModule->GetHP() == 0)
				continue;

		// 타겟이 공격자 자신이면 다음 타겟으로 넘어간다.
		//jintriple3	디버그 레지스터 핵 방지....
		bReturnValue =  pAttacker == pVictim;
		if ( pAttacker == pVictim)
			PROTECT_DEBUG_REGISTER(bReturnValue)
				continue;

		// 공격 가능한 상대가 아니면 다음 타겟으로 넘어간다.
		//jintriple3	디버그 레지스터 핵 방지....
		bool bRetVal = CanAttack( pAttacker, pVictim);
		if ( !bRetVal)
			PROTECT_DEBUG_REGISTER(!CanAttack_DebugRegister(pAttacker, pVictim))
				continue;



		// 타겟의 정보를 구한다.
		rvector VictimPos, VictimDir, VictimNorDir;
		rvector ZeroVector = rvector(0.0f, 0.0f, 0.0f);
		VictimPos = ZeroVector;
		bRetVal = pVictim->GetHistory(20, &VictimPos, &VictimDir, fShotTime);
		//jintriple3	디버그 레지스터 핵 방지....
		if ( !bRetVal)
			PROTECT_DEBUG_REGISTER(VictimPos == ZeroVector)
				continue;


		// NPC가 아닌 사람이 공격할 때...
		if ( !pAttacker->IsNPC())
		{
			// 공격자와 타겟간의 거리를 구한다.
			rvector swordPos = AttackerPos + ( AttackerNorDir * 100.f);
			swordPos.z += pAttacker->GetCollHeight() * .5f;
			float fDist = GetDistanceLineSegment( swordPos, VictimPos, VictimPos + rvector( 0, 0, pVictim->GetCollHeight()));


			// 공격자와 타겟간의 거리가 공격 가능 거리보다 멀면 다음 타겟으로 넘어간다.
			//jintriple3	디버그 레지스터 해킹 방지....
			bReturnValue = fDist > fRange;
			if ( fDist > fRange)
				PROTECT_DEBUG_REGISTER(bReturnValue)
					continue;


			// 공격자에 대한 타켓의 위치와 방향의 정규화
			rvector VictimNorDir = VictimPos - ( AttackerPos - ( AttackerNorDir * 50.f));
			Normalize( VictimNorDir);


			// 타켓이 공격 가능한 각도 밖이면 다음 타겟으로 넘어간다.
			//jintriple3	디버그 레지스터 핵 방지....
			float fDot = D3DXVec3Dot( &AttackerNorDir, &VictimNorDir);
			bReturnValue =  fDot < 0.5f;
			if ( fDot < 0.5f)
				PROTECT_DEBUG_REGISTER(bReturnValue)
					continue;
		}

		// 사람이 아닌 NPC가 공격할 때...  (기존의 게임성에 영향 없이 퀘스트 일때만 판정에 유효성을 수정한다)
		else
		{
			// 공격자에 대한 타켓의 위치와 방향 정규화
 			rvector VictimNorPos	= VictimPos;
			VictimNorPos.z			= 0;

			rvector VictimNorDir	= VictimPos - ( AttackerPos - ( AttackerNorDir * 50.f));
			VictimNorDir.z			= 0;
			Normalize( VictimNorDir);


			// 공격자와 타겟간의 x,y 평면 상의 거리를 구한 다음, 거리가 공격 가능 거리보다 멀면 다음 타겟으로 넘어간다.
			//jintriple3	디버그 레지스터 핵 방지....
			float fDist = Magnitude( AttackerNorPos - VictimNorPos);
			bReturnValue = fDist > fRange; 
			if ( fDist > fRange)
				PROTECT_DEBUG_REGISTER(bReturnValue)
					continue;

			// 타켓이 공격 가능한 각도 밖이면 다음 타겟으로 넘어간다.
			float fDot = D3DXVec3Dot( &AttackerNorDir, &VictimNorDir);
			//jintriple3	디버그 레지스터 핵 방지....
			bReturnValue = fDot < fAngle;
			if ( fDot < fAngle)
				PROTECT_DEBUG_REGISTER(bReturnValue)
					continue;

			// 타겟이 해당 공격 범위 안에서 일정 영역 높이에 있는지 확인한다.
			//jintriple3	디버그 레지스터 핵 방지....
			int nDebugRegister=0;
			if ( !InRanged( pAttacker, pVictim, nDebugRegister))
				PROTECT_DEBUG_REGISTER(nDebugRegister != FOR_DEBUG_REGISTER)
					continue;
		}


		// 공격자와 타겟 사이에 벽이 막고 있으면 다음 타겟으로 넘어간다.
		int nDebugRegister = 0;
		bRetVal = CheckWall( pAttacker, pVictim, nDebugRegister, true);
		//jintriple3 디버그 레지스터 핵 방지....
		if ( bRetVal)
			PROTECT_DEBUG_REGISTER(nDebugRegister == FOR_DEBUG_REGISTER)
				continue;


		// 가드 상태이면서 가드 방향이 맞으면 가드 성공
		//jintriple3 디버그 레지스터 핵 방지... 
		bRetVal = pVictim->IsGuard() && ( DotProduct( pVictim->m_Direction, AttackerNorDir) < 0);
		if ( pVictim->IsGuard() && ( DotProduct( pVictim->m_Direction, AttackerNorDir) < 0))
		{
			PROTECT_DEBUG_REGISTER(bRetVal)
			{
				rvector pos = pVictim->GetPosition();
				pos.z += 120.f;

				// 가드 이펙트 표시
				ZGetEffectManager()->AddSwordDefenceEffect( pos + ( pVictim->m_Direction * 50.f), pVictim->m_Direction);
				pVictim->OnMeleeGuardSuccess();
				return;
			}
		}

		// 노가드 상태에서 공격이 들어오면...
		rvector pos = pVictim->GetPosition();
		pos.z += 130.f;
		pos   -= AttackerDir * 50.f;


		// 이펙트를 출력한다.
	//	ZGetEffectManager()->AddBloodEffect( pos, -VictimNorDir);
		ZGetEffectManager()->AddSlashEffect( pos, -VictimNorDir, cm);


		// 데미지 값을 구한다.
		float fActualDamage = CalcActualDamage( pAttacker, pVictim, (float)pSkillDesc->m_nDamage.Ref(), pSkillDesc->m_nWeaponType.Ref());
		float fRatio = pItem->GetPiercingRatio( pSkillDesc->m_nWeaponType.Ref() , eq_parts_chest );
		pVictim->OnDamaged( pAttacker, pAttacker->GetPosition(), ZD_MELEE, pSkillDesc->m_nWeaponType.Ref(), fActualDamage, fRatio, cm);
		ZActor* pATarget = MDynamicCast( ZActor, pVictim);


		// 공격 받아서 뒤로 밀림
		bool bPushSkip = false;
		if ( pATarget)
			bPushSkip = pATarget->GetNPCInfo()->bNeverPushed;

		float fKnockbackForce = pItem->GetKnockbackForce();
		if ( bPushSkip)
		{
			ZGetSoundEngine()->PlaySound( "fx_bullethit_mt_met");
			fKnockbackForce = 1.0f;
		}

		pVictim->OnKnockback( pAttacker->m_Direction, fKnockbackForce);


		// 이펙트 사운드 출력
		ZGetSoundEngine()->PlaySoundBladeDamage(pSkillDesc, pos);
		#ifdef _ZPEERANTIHACK
		if (pVictim == m_pMyCharacter && pAttacker->IsNPC() == false)
			ZPeerAntiHackAdd(pAttacker->GetUID(), Melee, pItem->GetDesc()->m_nDelay.Ref(), MMCIP_PRIMARY, pItem->GetDesc()->m_nWeaponType.Ref());
		#endif
		// 콤보 공격이면 콤보 횟수를 업데이트 한다.
		if ( pAttacker == m_pMyCharacter)
		{
			CheckCombo( m_pMyCharacter, pVictim, !bPushSkip);
			CheckStylishAction( m_pMyCharacter);
		}


		// 공격이 맞았다.
		bHit = true;
	}


	// 캐릭터를 때리지 못했다면...
	// test 방향으로 벽과 충돌할경우 스파크..
	if ( !bHit)
	{

		rvector vPos = pAttacker->GetPosition();
		rvector vDir = AttackerNorDir;

		vPos.z += 130.f;

		RBSPPICKINFO bpi;

		if(GetWorld()->GetBsp()->Pick(vPos, vDir, &bpi)) {

			float fDist = Magnitude(vPos - bpi.PickPos);

			if (fDist < fRange) {

				rplane r = bpi.pInfo->plane;
				rvector vWallDir = rvector( r.a, r.b, r.c );
				Normalize(vWallDir);

				ZGetEffectManager()->AddSlashEffectWall( bpi.PickPos - (vDir*5.f) , vWallDir ,cm);

				rvector pos = bpi.PickPos;

				ZGetSoundEngine()->PlaySoundBladeConcrete(pSkillDesc, pos);
			}
		}
	}


	return;
}


bool ZGame::InRanged( ZObject* pAttacker, ZObject* pVictim)
{
	// 공격자와 타겟 실린더의 바닥과 윗면의 위치를 구한다.
	float fBotAtk	= pAttacker->GetPosition().z;
//	float fTopAtk	= fBotAtk + pAttacker->GetCollHeight();
	float fTopAtk	= fBotAtk + pAttacker->GetCollHeight() + (pAttacker->GetCollHeight() * 0.5f);

	float fBotVct	= pVictim->GetPosition().z;
	float fTopVct	= fBotVct + pVictim->GetCollHeight();


	// 타켓의 맨 아래가 공격자보다 위에 있으면 영역 밖이다.
	if ( fBotVct > fTopAtk)
		return false;

	// 타켓의 맨 위가 공격자보다 아래에 있으면 영역 밖이다.
	else if ( fTopVct < fBotAtk)
		return false;

	// 그 외에는 전부 영역 안이다.
	return true;
}

bool ZGame::InRanged( ZObject* pAttacker, ZObject* pVictim, int &nDebugRegister/*디버그 레지스터 해킹 방지를 위한 변수*/)
{
	// 공격자와 타겟 실린더의 바닥과 윗면의 위치를 구한다.
	float fBotAtk	= pAttacker->GetPosition().z;
//	float fTopAtk	= fBotAtk + pAttacker->GetCollHeight();
	float fTopAtk	= fBotAtk + pAttacker->GetCollHeight() + (pAttacker->GetCollHeight() * 0.5f);

	float fBotVct	= pVictim->GetPosition().z;
	float fTopVct	= fBotVct + pVictim->GetCollHeight();


	// 타켓의 맨 아래가 공격자보다 위에 있으면 영역 밖이다.
	if ( fBotVct > fTopAtk)
	{
		nDebugRegister = -10;//숫자는 의미가 없다..단순 비교를 위한 수...
		return false;
	}

	// 타켓의 맨 위가 공격자보다 아래에 있으면 영역 밖이다.
	else if ( fTopVct < fBotAtk)
	{
		nDebugRegister = -10;
		return false;
	}

	// 그 외에는 전부 영역 안이다.
	nDebugRegister = FOR_DEBUG_REGISTER;
	return true;
}
struct ZShotItemInfo {
	unsigned long int		m_nID;
//	char					m_szItemName[128];
	MMatchItemName* m_pMItemName;
	int		m_nTotalPoint;
	MMatchWeaponType	m_nWeaponType;
	MMatchItemType	m_nType;
	int		m_nResSex;
	int		m_nResLevel;
	MMatchItemSlotType		m_nSlot;
	
	int		m_nWeight;
	int		m_nBountyPrice;
	bool					m_bIsAdminEventItem;
	bool					m_bIsCashItem;	
	int		m_nDelay;
	MMatchItemEffectDesc*	m_pEffect;
	MMatchItemEffectId		m_nEffectId;
	int		m_nControllability;
	int		m_nMagazine;
	int		m_nMaxBullet;
	int		m_nReloadTime;
	bool					m_bSlugOutput;
	int		m_nGadgetID;
	int		m_nHP;
	int		m_nAP;
	int		m_nMaxWT;
	int		m_nSF;
	int		m_nFR;
	int		m_nCR;
	int		m_nPR;
	int		m_nLR;
	int		m_nLimitSpeed;
	int		m_nLimitJump;
	int		m_nLimitTumble;
	int		m_nLimitWall;
	int		m_nRange;		// melee무기 공격범위
	int		m_nAngle;		// melee무기 공격각도
	int		m_nEffectLevel;


	int				m_nDamage;			// 무기류의 공격력
	int				m_nALDamage;
	int				m_nItemPower;		// 소모성 아이템의 데미지 또는 회복량
	int				m_nDamageTime;		// 도트 형태의 데미지나 회복이 들어가는 시간(0일 경우, 한 방만 들어간다)
	MMatchDamageType	m_nDamageType;		// 소모성 아이템의 데미지 타입 (데미지 속성 혹은 회복 속성)
	int		m_nLifeTime;				// 작동 시간 (현재는 트랩의 작동시간에만 사용)
	float			m_nSnipeZoom;

	char					m_szDesc[8192];	// 설명

	bool								m_bIsSpendableItem;
	MMatchSpendType		m_nSpendType;

	MMatchAvatarMeshName*		m_pAvatarMeshName;

	unsigned long int		m_nColor;
	int						m_nImageID;
	int						m_nBulletImageID;
	int						m_nMagazineImageID;
	char					m_szReloadSndName[256];
	char					m_szFireSndName[256];
	char					m_szDryfireSndName[256];
	char					m_szWeaponByFiber[256];		// melee 무기의 재질에 따른 소리

	MMatchItemBonus			m_Bonus;
	MMatchTicketType		m_TicketType;
	int		m_nMaxRentPeriod;	// 기간제 바운티 아이템의 최대 기간 정보 (날짜단위)

	bool					m_bIsEnableMoveToAccountItem;
};

//jintriple3 디버그 레지스터 해킹 방지 코드 삽입
void ZGame::OnPeerShot_Range_Damaged(ZObject* pOwner, float fShotTime, const rvector& pos, const rvector& to, ZPICKINFO pickinfo, DWORD dwPickPassFlag, rvector& v1, rvector& v2, ZItem *pItem, rvector& BulletMarkNormal, bool& bBulletMark, ZTargetType& nTargetType, const MMatchCharItemParts sel_type)
{ 
	MTD_SHOTINFO* shot = NULL;
	MMatchItemDesc *pDesc = pItem->GetDesc();
	bool bReturnValue = !pDesc;
	if(!pDesc) PROTECT_DEBUG_REGISTER(bReturnValue){ _ASSERT(FALSE); return; }
	ZCharacter *pTargetCharacter=ZGetGameInterface()->GetCombatInterface()->GetTargetCharacter();
	if (pDesc->IsAdminEventItem() && !strstr(ZGetGameClient()->GetChannelName(), "Event"))
	{
		return;
	}
	rvector dir = to - pos;

	bReturnValue = !(ZGetGame()->PickHistory(pOwner,fShotTime,pos,to, &pickinfo,dwPickPassFlag));
	if(!(ZGetGame()->PickHistory(pOwner,fShotTime,pos,to, &pickinfo,dwPickPassFlag)))
	{
		PROTECT_DEBUG_REGISTER(bReturnValue)
		{
			v1 = pos;
			v2 = pos+dir*10000.f;
			nTargetType	= ZTT_NOTHING;
			return;
		}
	}
	if(ZGetGameClient()->GetChannelType() == MCHANNEL_TYPE_DUELTOURNAMENT && strstr(ZGetGameClient()->GetChannelName(), "(AntiLead)")  || ZGetGameClient()->GetMatchStageSetting()->IsAntiLead() == true)
	{
		
		FirePosistions* f = new FirePosistions();
		f->TX = v2.x;
		f->TY = v2.y;
		f->TZ = v2.z;
		f->WX = pos.x;
		f->WY = pos.y;
		f->WZ = pos.z;
		f->BX = BulletMarkNormal.x;
		f->BY = BulletMarkNormal.y;
		f->BZ = BulletMarkNormal.z;
		f->VX = v1.x;
		f->VY = v1.y;
		f->VZ = v1.z;
		f->ShotsHit = false;
		f->TargetType = nTargetType;
		f->ShotGun = false;
		f->BulletMark = bBulletMark;
		void* pBlobArray1 = MMakeBlobArray(sizeof(FirePosistions), 1);
		void* pBlobElement1 = MGetBlobArrayElement(pBlobArray1, 0);
		memcpy(pBlobElement1, f, sizeof(FirePosistions));
		ZPostShotgunEffect(pBlobArray1);
	}
	// 땜빵 -bird 
	//jintriple3 디버그 레지스터 해킹 방지 코드
	bReturnValue = (!pickinfo.pObject) && (!pickinfo.bBspPicked);
	if(pickinfo.bBspPicked)
	{
		PROTECT_DEBUG_REGISTER(pickinfo.nBspPicked_DebugRegister == FOR_DEBUG_REGISTER)
		{
			nTargetType = ZTT_OBJECT;

			v1 = pos;
			v2 = pickinfo.bpi.PickPos;

			// 총탄 흔적
			BulletMarkNormal.x = pickinfo.bpi.pInfo->plane.a;
			BulletMarkNormal.y = pickinfo.bpi.pInfo->plane.b;
			BulletMarkNormal.z = pickinfo.bpi.pInfo->plane.c;
			Normalize(BulletMarkNormal);
			bBulletMark = true;
			return;
		}
	}
	else if( (!pickinfo.pObject) && (!pickinfo.bBspPicked) )
	{
		PROTECT_DEBUG_REGISTER(bReturnValue)
		{
			_ASSERT(false);
			return;
		}
	}
	//위에까지는 검사 단계...

	ZObject *pObject = pickinfo.pObject;
	bool bGuard = pObject->IsGuard() && (pickinfo.info.parts!=eq_parts_legs) &&		// 다리는 막을수없다
					DotProduct(dir,pObject->GetDirection())<0;

	if(pObject->IsGuard() && (pickinfo.info.parts!=eq_parts_legs) &&		// 다리는 막을수없다
					DotProduct(dir,pObject->GetDirection())<0) //여기도..
	{
		PROTECT_DEBUG_REGISTER(bGuard)
		{
			nTargetType = ZTT_CHARACTER_GUARD;
			// 막았다
			rvector t_pos = pObject->GetPosition();
			t_pos.z += 100.f;
			ZGetEffectManager()->AddSwordDefenceEffect(t_pos+(-dir*50.f),-dir);
			pObject->OnGuardSuccess();
			v1 = pos;
			v2 = pickinfo.info.vOut;
			return;
		}
	}
	nTargetType = ZTT_CHARACTER;

	ZActor* pATarget = MDynamicCast(ZActor,pickinfo.pObject);

	bool bPushSkip = false;

	if(pATarget) 
	{
		bPushSkip = pATarget->GetNPCInfo()->bNeverPushed;
	}

	float fKnockbackForce = pItem->GetKnockbackForce();

	if(bPushSkip) 
	{
//					ZGetSoundEngine()->PlaySound("fx_bullethit_mt_met");
		rvector vPos = pOwner->GetPosition() + (pickinfo.pObject->GetPosition() - pOwner->GetPosition()) * 0.1f; 
		ZGetSoundEngine()->PlaySound("fx_bullethit_mt_met", vPos );
		fKnockbackForce = 1.0f;
	}

	pickinfo.pObject->OnKnockback( pOwner->m_Direction, fKnockbackForce );
	
	float fActualDamage = CalcActualDamage(pOwner, pickinfo.pObject, (float)pDesc->m_nDamage.Ref(), pDesc->m_nWeaponType.Ref());
	if(!GetMatch()->IsQuestDrived() && ZGetGameClient()->GetMatchStageSetting()->IsAntiLead() == true || !GetMatch()->IsQuestDrived() && ZGetGameClient()->GetChannelType() == MCHANNEL_TYPE_DUELTOURNAMENT && strstr(ZGetGameClient()->GetChannelName(), "(AntiLead)"))
	{
		if (pOwner != NULL)
		{
			ZCharacter* pVictim = (ZCharacter*)(pickinfo.pObject);
			if (pVictim && pVictim->m_dwStatusBitPackingValue.Ref().m_bLostConEffect == false && !pVictim->IsDie())
			{
				if(!ZGetGame()->GetMatch()->IsTeamPlay() || ZGetGame()->GetMatch()->IsTeamPlay() && ZGetGame()->GetMatch()->GetTeamKillEnabled() || !pVictim->IsTeam(((ZCharacter*)pOwner)))
				{
					shot = new MTD_SHOTINFO();
					shot->MYID = pVictim->GetUID();
					shot->Type = (char)sel_type;
					shot->Parts = (char)pickinfo.info.parts;
					shot->X = pos.x;
					shot->Y = pos.y;
					shot->Z = pos.z;
					int size = 1;
					shot->Size = 1;
					void* pBlobArray = MMakeBlobArray(sizeof(MTD_SHOTINFO), 1);
					void* pBlobElement = MGetBlobArrayElement(pBlobArray, 0);
					memcpy(pBlobElement, shot, sizeof(MTD_SHOTINFO));
					int ID = RandomNumber(0, 5000000);
					shot->ntime = timeGetTime();
					if (ZGetGameClient()->ShowLocalDamage)
					{
						ZGetGame()->m_pMyCharacter->GetStatus().CheckCrc();
						ZGetGame()->m_pMyCharacter->GetStatus().Ref().LocalSentDamage += fActualDamage;
						ZGetGame()->m_pMyCharacter->GetStatus().MakeCrc();
					}
					ZPOSTANTILEAD(MCommandParameterBlob(pBlobArray, MGetBlobArraySize(pBlobArray)), MCommandParameterInt(ID), pVictim->GetUID());	
					MEraseBlobArray( pBlobArray );
					vector<MTD_SHOTINFO*> pvec;
					pvec.push_back(shot);
					RetryShots.insert(RetryArray::value_type(ID, pvec));
				}
			}
		}
		//if(((ZCharacter*)(pickinfo.pObject))->m_dwStatusBitPackingValue.Ref().m_bLostConEffect == false)
	//	{
		//	((ZCharacter*)(pickinfo.pObject))->OnDamagedAPlayer(pOwner, pOwner->GetPosition(), pickinfo.info.parts, fActualDamage);
	//	}
	} else {
		float fRatio = pItem->GetPiercingRatio( pDesc->m_nWeaponType.Ref(), pickinfo.info.parts );
		ZDAMAGETYPE dt = (pickinfo.info.parts==eq_parts_head) ? ZD_BULLET_HEADSHOT : ZD_BULLET;
		pickinfo.pObject->OnDamaged(pOwner, pOwner->GetPosition(), dt, pDesc->m_nWeaponType.Ref(), fActualDamage, fRatio );
	}
	if(pOwner == m_pMyCharacter) 
	{
		CheckCombo(m_pMyCharacter,pickinfo.pObject,!bPushSkip);
		CheckStylishAction(m_pMyCharacter);
	}
	v1 = pos;
	v2 = pickinfo.info.vOut;
}
void ZGame::OnPeerShot_Range(const MMatchCharItemParts sel_type, const MUID& uidOwner, float fShotTime, const rvector& pos, const rvector& to)
{
	ZObject *pOwner = m_ObjectManager.GetObject(uidOwner);
	if(!pOwner) return;

	ZItem *pItem = pOwner->GetItems()->GetItem(sel_type);
	if(!pItem) return;
	MMatchItemDesc *pDesc = pItem->GetDesc();
	if(!pDesc) { _ASSERT(FALSE); return; }
	if (pDesc->IsAdminEventItem() && !strstr(ZGetGameClient()->GetChannelName(), "Event"))
	{
		return;
	}
	rvector dir = to - pos;

	Normalize(dir);

	rvector v1, v2;
	rvector BulletMarkNormal;
	bool bBulletMark = false;
	ZTargetType nTargetType = ZTT_OBJECT;

	// ZCharacter* pCharacter = NULL;

	ZPICKINFO pickinfo;

	memset(&pickinfo,0,sizeof(ZPICKINFO));

	// 총알은 로켓이 통과하는곳도 통과한다
	const DWORD dwPickPassFlag=RM_FLAG_ADDITIVE | RM_FLAG_HIDE | RM_FLAG_PASSROCKET | RM_FLAG_PASSBULLET;

	// 쏘는 캐릭터 흔들어 주기..
	pOwner->Tremble(8.f, 50, 30);

/*
	if(pOwner->m_pVMesh)
	{
		float fMaxValue = 8.f;// 흔들 강도 +- 가능

		RFrameTime* ft = &pOwner->m_pVMesh->m_FrameTime;
		if(ft && !ft->m_bActive)
			ft->Start(fMaxValue,50,30);// 강도 , 최대시간 , 복귀시간...
	}
*/
	//jintriple3 디버그 레지스터 핵 방지를 위해 함수로 뺐다..근데 인자가 넘흐 많아..ㅜㅜ
	OnPeerShot_Range_Damaged(pOwner, fShotTime, pos, to, pickinfo, dwPickPassFlag,v1, v2, pItem, BulletMarkNormal, bBulletMark, nTargetType, sel_type);
	/*
	if(g_pGame->PickHistory(pOwner,fShotTime,pos,to,&pickinfo,dwPickPassFlag))
	{
		// 땜빵 -bird 
		
		if(pickinfo.pObject)
		{
			ZObject *pObject = pickinfo.pObject;
			bool bGuard = pObject->IsGuard() && (pickinfo.info.parts!=eq_parts_legs) &&		// 다리는 막을수없다
							DotProduct(dir,pObject->GetDirection())<0;

			if(bGuard) 
			{
				nTargetType = ZTT_CHARACTER_GUARD;
				// 막았다
				rvector t_pos = pObject->GetPosition();
				t_pos.z += 100.f;
				ZGetEffectManager()->AddSwordDefenceEffect(t_pos+(-dir*50.f),-dir);
				pObject->OnGuardSuccess();

			}
			else 
			{
				nTargetType = ZTT_CHARACTER;

				ZActor* pATarget = MDynamicCast(ZActor,pickinfo.pObject);

				bool bPushSkip = false;

				if(pATarget) 
				{
					bPushSkip = pATarget->GetNPCInfo()->bNeverPushed;
				}

				float fKnockbackForce = pItem->GetKnockbackForce();

				if(bPushSkip) 
				{
//					ZGetSoundEngine()->PlaySound("fx_bullethit_mt_met");
					rvector vPos = pOwner->GetPosition() + (pickinfo.pObject->GetPosition() - pOwner->GetPosition()) * 0.1f; 
					ZGetSoundEngine()->PlaySound("fx_bullethit_mt_met", vPos );
					fKnockbackForce = 1.0f;
				}

				pickinfo.pObject->OnKnockback( pOwner->m_Direction, fKnockbackForce );

				float fActualDamage = CalcActualDamage(pOwner, pickinfo.pObject, (float)pDesc->m_nDamage);
				float fRatio = pItem->GetPiercingRatio( pDesc->m_nWeaponType, pickinfo.info.parts );
				ZDAMAGETYPE dt = (pickinfo.info.parts==eq_parts_head) ? ZD_BULLET_HEADSHOT : ZD_BULLET;
				//jintriple3 디버그 레지스터 핵으로부터 안전하게 보호해야 될 부분..
				pickinfo.pObject->OnDamaged(pOwner, pOwner->GetPosition(), dt, pDesc->m_nWeaponType, fActualDamage, fRatio );

				if(pOwner == m_pMyCharacter) 
				{
					CheckCombo(m_pMyCharacter,pickinfo.pObject,!bPushSkip);
					CheckStylishAction(m_pMyCharacter);
				}
			}

			v1 = pos;
			v2 = pickinfo.info.vOut;

		}
		else if(pickinfo.bBspPicked)
		{
			nTargetType = ZTT_OBJECT;

			v1 = pos;
			v2 = pickinfo.bpi.PickPos;

			// 총탄 흔적
			BulletMarkNormal.x = pickinfo.bpi.pInfo->plane.a;
			BulletMarkNormal.y = pickinfo.bpi.pInfo->plane.b;
			BulletMarkNormal.z = pickinfo.bpi.pInfo->plane.c;
			Normalize(BulletMarkNormal);
			bBulletMark = true;

		}
		else 
		{
			_ASSERT(false);
			return;
		}
	}

	else
	{
		v1 = pos;
		v2 = pos+dir*10000.f;
		nTargetType	= ZTT_NOTHING;
	}*/
	

	bool bPlayer = false;
	//bool b3D = (pOwnerCharacter!=m_pMyCharacter);	// 자기가 내는 사운드는 2D로 출력한다.
	//rvector Pos = pOwnerCharacter->GetPosition();
	rvector Pos = v1;
	if(pOwner==m_pMyCharacter)
	{

		Pos = RCameraPosition;
		bPlayer = true;
	}

	ZCharacter *pTargetCharacter=ZGetGameInterface()->GetCombatInterface()->GetTargetCharacter();
	ZApplication::GetSoundEngine()->PlaySEFire(pDesc, Pos.x, Pos.y, Pos.z, bPlayer);
	//if(nTargetType == ZTT_OBJECT) { ZApplication::GetSoundEngine()->PlaySERicochet(v2.x, v2.y, v2.z); }
#define SOUND_CULL_DISTANCE 1500.0F
	if( D3DXVec3LengthSq(&(v2 - pTargetCharacter->GetPosition())) < (SOUND_CULL_DISTANCE * SOUND_CULL_DISTANCE) )
	{
		if(nTargetType == ZTT_OBJECT) { 
			ZGetSoundEngine()->PlaySEHitObject( v2.x, v2.y, v2.z, pickinfo.bpi ); 
		}

		if(nTargetType == ZTT_CHARACTER) { 
			ZGetSoundEngine()->PlaySEHitBody(v2.x, v2.y, v2.z); 
		}
	}

	//// 보이지 않으면 이펙트를 그릴필요는 없다 - 정확한 컬링을 요망.. by bird
	//if(!pOwner->IsRendered()) return;


	// 쏘는곳 반경 100cm 가 화면에 들어가는지 체크한다
	bool bDrawFireEffects = isInViewFrustum(v1,100.f,RGetViewFrustum());

	if(!isInViewFrustum(v1,v2,RGetViewFrustum()) // 쏘는곳에서 맞는곳의 라인이 보이는지..
		&& !bDrawFireEffects) return;					// 쏘는곳에서도 그릴게 없는지..

	bool bDrawTargetEffects = isInViewFrustum(v2,100.f,RGetViewFrustum());



	/////////////////////// 이후는 이펙트 추가

	// 물튀는 이펙트 체크
	GetWorld()->GetWaters()->CheckSpearing( v1, v2, 250, 0.3 );



	// TODO: NPC 의 총구위치 인터페이스가 확정되면 마저 추가하자.
//	ZCharacter *pOwnerCharacter = m_CharacterManager.Find(uidOwner);

	
	ZCharacterObject* pCOwnerObject = MDynamicCast(ZCharacterObject, pOwner);

	if(pCOwnerObject) 
	{

		// 총구 화염이펙트
		rvector pdir = v2-v1;
		Normalize(pdir);

		int size = 3;

		rvector v[6];

//		size = GetWeapondummyPos(pOwnerCharacter,v);
		if(pCOwnerObject->IsRendered())
			size = pCOwnerObject->GetWeapondummyPos(v);
		else
		{
			size = 6;
			v[0] = v[1] = v[2] = v1;
			v[3] = v[4] = v[5] = v[0];
		}


		MMatchWeaponType wtype = pDesc->m_nWeaponType.Ref();
		bool bSlugOutput = pDesc->m_bSlugOutput; // 탄피적출(true, false) 

		// Effect
		if(bBulletMark==false) BulletMarkNormal = -pdir;
		
		if(ZGetConfiguration()->GetEtc()->bNozzleFlare)
		ZGetEffectManager()->AddShotEffect( v , size , v2, BulletMarkNormal, nTargetType, wtype, bSlugOutput, pCOwnerObject,bDrawFireEffects,bDrawTargetEffects);

		// 총 쏠때 라이트 추가
		ZCharacterObject* pChar;

		if( ZGetConfiguration()->GetVideo()->bDynamicLight && pCOwnerObject != NULL )
		{
			pChar = pCOwnerObject;
		
			if( pChar->m_bDynamicLight )
			{
				pChar->m_vLightColor = g_CharLightList[GUN].vLightColor;
				pChar->m_fLightLife = g_CharLightList[GUN].fLife;
			}
			else
			{
				pChar->m_bDynamicLight = true;
				pChar->m_vLightColor = g_CharLightList[GUN].vLightColor;
				pChar->m_vLightColor.x = 1.0f;
				pChar->m_iDLightType = GUN;
				pChar->m_fLightLife = g_CharLightList[GUN].fLife;
			}
		}
	}
	
	// 깃발에 힘 적용			   p
	GetWorld()->GetFlags()->CheckSpearing( v1, v2, BULLET_SPEAR_EMBLEM_POWER );
	if(Z_VIDEO_DYNAMICLIGHT)
		ZGetStencilLight()->AddLightSource( v1, 2.0f, 75 );
}

static float maxvalue =(float)0x7fff;
#define RANDOMFLOAT2 ((float)rand()/maxvalue)
void ZGame::OnPeerShot_Shotgun(const MMatchCharItemParts sel_type, ZCharacter* pOwnerCharacter, float fShotTime, const rvector& pos, const rvector& to)
{
	// 내 캐릭터 혹은 내가 보고있는 캐릭터
	ZCharacter *pTargetCharacter = ZGetGameInterface()->GetCombatInterface()->GetTargetCharacter();
	if(!pTargetCharacter) return;

	//// 디버그용 덤프
	//{
	//	if(strcmp("폭주천사다크",pTargetCharacter->GetProperty()->szName)==0) {
	//		mlog("shot : %3.3f \n",fShotTime);
	//	}
	//}

	ZItem *pItem = pOwnerCharacter->GetItems()->GetItem(sel_type);
	if(!pItem) return;
	MMatchItemDesc *pDesc = pItem->GetDesc();
	if(!pDesc) { _ASSERT(false); return; }




	

	if (pOwnerCharacter == NULL) return;
	if (pDesc->IsAdminEventItem() && !strstr(ZGetGameClient()->GetChannelName(), "Event"))
	{
		return;
	}
	// 모든사람이 같은 random seed 를 갖도록 같은값으로 초기화 해준다
	int *seed=(int*)&fShotTime;
	srand(*seed);

	bool bHitGuard=false,bHitBody=false,bHitGround=false,bHitEnemy=false;
	rvector GuardPos,BodyPos,GroundPos;
	bool waterSound = false;

	rvector v1, v2;
	rvector dir;

	rvector origdir = to - pos;
	Normalize(origdir);

	int nHitCount = 0;
	ZPICKINFO pickinfo;
	int ID = 0;
	ShotArray ShotGunArray;
	for(int i=0;i<SHOTGUN_BULLET_COUNT;i++)
	{
		dir = origdir;
		{
			// 오차값 - 반동대신 시범삼아 넣음
			rvector r, up(0,0,1), right;
			D3DXQUATERNION q;
			D3DXMATRIX mat;

			float fAngle = (rand() % (31415 * 2)) / 1000.0f;
			float fForce = RANDOMFLOAT2*SHOTGUN_DIFFUSE_RANGE;

			D3DXVec3Cross(&right,&dir,&up);
			D3DXVec3Normalize(&right,&right);
			D3DXMatrixRotationAxis(&mat, &right, fForce);
			D3DXVec3TransformCoord(&r, &dir, &mat);

			D3DXQuaternionRotationAxis(&q, &dir, fAngle);
			D3DXMatrixRotationQuaternion(&mat, &q);
			D3DXVec3TransformCoord(&r, &r, &mat);

			dir=r;
		}
		rvector BulletMarkNormal;
		bool bBulletMark = false;
		ZTargetType nTargetType = ZTT_OBJECT;

		memset(&pickinfo,0,sizeof(ZPICKINFO));

		// 총알은 로켓이 통과하는곳도 통과한다
		const DWORD dwPickPassFlag=RM_FLAG_ADDITIVE | RM_FLAG_HIDE | RM_FLAG_PASSROCKET | RM_FLAG_PASSBULLET;
		//jintriple3 디버그 레지스터 핵 방어 함수..
		MTD_SHOTINFO* info = OnPeerShotgun_Damaged( pOwnerCharacter, fShotTime, pos, dir, pickinfo, dwPickPassFlag, v1, v2, pItem, BulletMarkNormal, bBulletMark, nTargetType, bHitEnemy, sel_type, ID, to);
		if(info != NULL) 
		{
			ShotArray::iterator it = ShotGunArray.find(info->MYID);
			if(it != ShotGunArray.end())
			{
				it->second.push_back(info);
			}
			else
			{
				vector<MTD_SHOTINFO*> shot;
				shot.push_back(info);
				ShotGunArray.insert(ShotArray::value_type(info->MYID, shot));
			}
		}
	}
	if(ZGetGameClient()->GetChannelType() == MCHANNEL_TYPE_DUELTOURNAMENT && strstr(ZGetGameClient()->GetChannelName(), "(AntiLead)") && pickinfo.pObject->IsNPC() == false || ZGetGameClient()->GetMatchStageSetting()->IsAntiLead() == true)
	{
		FirePosistions* f = new FirePosistions();
		f->TX = to.x;
		f->TY = to.y;
		f->TZ = to.z;
		f->WX = pos.x;
		f->WY = pos.y;
		f->WZ = pos.z;
		f->VX = v1.x;
		f->VY = v1.y;
		f->VZ = v1.z;
		f->ShotsHit = bHitEnemy;
		f->ShotGun = true;
		void* pBlobArray1 = MMakeBlobArray(sizeof(FirePosistions), 1);
		void* pBlobElement1 = MGetBlobArrayElement(pBlobArray1, 0);
		memcpy(pBlobElement1, f, sizeof(FirePosistions));
		ZPostShotgunEffect(pBlobArray1);
	}
	int nShotGunArraySize = ShotGunArray.size();
	if(nShotGunArraySize > 0)
	{
		for(ShotArray::iterator it = ShotGunArray.begin(); it != ShotGunArray.end(); it++)
		{
			MTD_SHOTINFO* shot = new MTD_SHOTINFO();
			shot->MYID = it->second[0]->MYID;
			shot->Type = (char)sel_type;
			shot->Parts = (char)pickinfo.info.parts;
			shot->X = pos.x;
			shot->Y = pos.y;
			shot->Z = pos.z;
			shot->Size = it->second.size();
			void* pBlobArray = MMakeBlobArray(sizeof(MTD_SHOTINFO), 1);
			void* pBlobElement = MGetBlobArrayElement(pBlobArray, 0);
			memcpy(pBlobElement, shot, sizeof(MTD_SHOTINFO));
			int ID = RandomNumber(5000000, 10000000);
			ZPOSTANTILEAD(MCommandParameterBlob(pBlobArray, MGetBlobArraySize(pBlobArray)), MCommandParameterInt(ID), it->second[0]->MYID);	
			MEraseBlobArray( pBlobArray );
			it->second[0]->ntime = timeGetTime();
			RetryShots.insert(RetryArray::value_type(ID, it->second));
			if (ZGetGameClient()->ShowLocalDamage) 
			{
				ZGetGame()->m_pMyCharacter->GetStatus().CheckCrc();
				ZGetGame()->m_pMyCharacter->GetStatus().Ref().LocalSentDamage += it->second[0]->Damage * it->second.size();
				ZGetGame()->m_pMyCharacter->GetStatus().MakeCrc();
			}
		}
		ShotGunArray.clear();
	}
	if(bHitEnemy) {
		CheckStylishAction(pOwnerCharacter);
		CheckCombo(pOwnerCharacter, NULL,true);
	}

	ZApplication::GetSoundEngine()->PlaySEFire(pItem->GetDesc(), pos.x, pos.y, pos.z, (pOwnerCharacter==m_pMyCharacter));

	// 보이지 않으면 이펙트를 그릴필요는 없다
	if(!pOwnerCharacter->IsRendered()) return;

	rvector v[6];

	int _size = pOwnerCharacter->GetWeapondummyPos(v);

	dir = to - pos;
	Normalize(dir);
	
	if(ZGetConfiguration()->GetEtc()->bShotgunSparks)
	ZGetEffectManager()->AddShotgunEffect(const_cast<rvector&>(pos),v[1],dir,pOwnerCharacter);
	//ZGetEffectManager()->AddBloodEffect(const_cast<rvector&>(pos), dir);

	// 총 쏠때 라이트 추가
	ZCharacter* pChar;
	if( ZGetConfiguration()->GetVideo()->bDynamicLight && pOwnerCharacter != NULL )
	{
		pChar = pOwnerCharacter;

		if( pChar->m_bDynamicLight )
		{
			pChar->m_vLightColor = g_CharLightList[SHOTGUN].vLightColor;
			pChar->m_fLightLife = g_CharLightList[SHOTGUN].fLife;
		}
		else
		{
			pChar->m_bDynamicLight = true;
			pChar->m_vLightColor = g_CharLightList[SHOTGUN].vLightColor;
			pChar->m_vLightColor.x = 1.0f;
			pChar->m_iDLightType = SHOTGUN;
			pChar->m_fLightLife = g_CharLightList[SHOTGUN].fLife;
		}
	}
//	m_flags.CheckSpearing( v1, v2, SHOTGUN_SPEAR_EMBLEM_POWER );
	if(Z_VIDEO_DYNAMICLIGHT)
		ZGetStencilLight()->AddLightSource(v1, 2.0f, 200 );
}

//jintriple3 디버그 레지스터 해킹 방지 코드 삽입
MTD_SHOTINFO* ZGame::OnPeerShotgun_Damaged(ZObject* pOwner, float fShotTime, const rvector& pos, rvector& dir, ZPICKINFO pickinfo, DWORD dwPickPassFlag, rvector& v1, rvector& v2, ZItem *pItem, rvector& BulletMarkNormal, bool& bBulletMark, ZTargetType& nTargetType, bool& bHitEnemy, const MMatchCharItemParts sel_type, int ID, rvector to)
{
	MTD_SHOTINFO* shot = NULL;
	ZCharacter *pTargetCharacter = ZGetGameInterface()->GetCombatInterface()->GetTargetCharacter();
	bool bReturnValue = !pTargetCharacter;
	if(!pTargetCharacter)PROTECT_DEBUG_REGISTER(bReturnValue) return shot;
	
	MMatchItemDesc *pDesc = pItem->GetDesc();
	bReturnValue = !pDesc;
	if(!pDesc)PROTECT_DEBUG_REGISTER(bReturnValue) { _ASSERT(FALSE); return shot; }

	//rvector dir = to - pos;

	bool waterSound = false;
	//여기에 방어코드가 들어가야돼~
	bReturnValue = !(ZGetGame()->PickHistory(pOwner,fShotTime,pos,pos+10000.f*dir, &pickinfo,dwPickPassFlag));
	if(!(ZGetGame()->PickHistory(pOwner,fShotTime,pos,pos+10000.f*dir, &pickinfo,dwPickPassFlag)))
	{
		PROTECT_DEBUG_REGISTER(bReturnValue)
		{
			v1 = pos;
			v2 = pos+dir*10000.f;
			nTargetType	= ZTT_NOTHING;
			waterSound = GetWorld()->GetWaters()->CheckSpearing( v1, v2, 250, 0.3, !waterSound );
			return shot;
		}
	}
		//여기도..
	bReturnValue = (!pickinfo.pObject) && (!pickinfo.bBspPicked);
	if(pickinfo.bBspPicked)
	{
		PROTECT_DEBUG_REGISTER(pickinfo.nBspPicked_DebugRegister == FOR_DEBUG_REGISTER)
		{
			nTargetType = ZTT_OBJECT;

			v1 = pos;
			v2 = pickinfo.bpi.PickPos;

			// 총탄 흔적
			BulletMarkNormal.x = pickinfo.bpi.pInfo->plane.a;
			BulletMarkNormal.y = pickinfo.bpi.pInfo->plane.b;
			BulletMarkNormal.z = pickinfo.bpi.pInfo->plane.c;
			Normalize(BulletMarkNormal);
			bBulletMark = true;

			// 맞는곳 반경 20cm 가 화면에 들어오면 그린다
			bool bDrawTargetEffects = isInViewFrustum(v2,20.f,RGetViewFrustum());
			if(bDrawTargetEffects)
			{
				rvector pdir = v2-v1;
				Normalize(pdir);

				int size = 3;
				bool bDrawFireEffects = isInViewFrustum(v1,100.f,RGetViewFrustum());
				rvector v[6];

		//		size = GetWeapondummyPos(pOwnerCharacter,v);
				ZCharacterObject* pCOwnerObject = MDynamicCast(ZCharacterObject, pOwner);
				if(pCOwnerObject->IsRendered())
					size = pCOwnerObject->GetWeapondummyPos(v);
				else
				{
					size = 6;
					v[0] = v[1] = v[2] = v1;
					v[3] = v[4] = v[5] = v[0];
				}


				MMatchWeaponType wtype = pDesc->m_nWeaponType.Ref();
				bool bSlugOutput = pDesc->m_bSlugOutput; 
				if(ZGetConfiguration()->GetEtc()->bBulletProjectiles)
				ZGetEffectManager()->AddBulletMark(v2,BulletMarkNormal);
				if(ZGetConfiguration()->GetEtc()->bNozzleFlare)
				ZGetEffectManager()->AddShotEffect(&v1, size , v2, BulletMarkNormal, nTargetType, wtype, bSlugOutput, pCOwnerObject,bDrawFireEffects,bDrawTargetEffects);
			}
			waterSound = GetWorld()->GetWaters()->CheckSpearing( v1, v2, 250, 0.3, !waterSound );
			return shot;
		}
	}
	else if( (!pickinfo.pObject) && (!pickinfo.bBspPicked) )
	{
		PROTECT_DEBUG_REGISTER(bReturnValue)
		{
			_ASSERT(false);
			return shot;
		}
	}

	//위에까지는 검사 단계...

	ZObject *pObject = pickinfo.pObject;
	bool bGuard = pObject->IsGuard() && (pickinfo.info.parts!=eq_parts_legs) &&		// 다리는 막을수없다
					DotProduct(dir,pObject->GetDirection())<0;

	if(pObject->IsGuard() && (pickinfo.info.parts!=eq_parts_legs) &&
					DotProduct(dir,pObject->GetDirection())<0) 
	{
		PROTECT_DEBUG_REGISTER(bGuard)
		{
			nTargetType = ZTT_CHARACTER_GUARD;
			// 막았다
			rvector t_pos = pObject->GetPosition();
			t_pos.z += 100.f;
			ZGetEffectManager()->AddSwordDefenceEffect(t_pos+(-dir*50.f),-dir);
			pObject->OnGuardSuccess();
			v1 = pos;
			v2 = pickinfo.info.vOut;
			return shot;
		}
	}

	ZActor* pATarget = MDynamicCast(ZActor,pObject);
	nTargetType = ZTT_CHARACTER;

	bool bPushSkip = false;

	if(pATarget) 
	{
		bPushSkip = pATarget->GetNPCInfo()->bNeverPushed;
	}

	float fKnockbackForce = pItem->GetKnockbackForce()/ (.5f*float(SHOTGUN_BULLET_COUNT));

	if(bPushSkip) 
	{
//					ZGetSoundEngine()->PlaySound("fx_bullethit_mt_met");
		rvector vPos = pOwner->GetPosition() + (pickinfo.pObject->GetPosition() - pOwner->GetPosition()) * 0.1f; 
		ZGetSoundEngine()->PlaySound("fx_bullethit_mt_met", vPos );
		fKnockbackForce = 1.0f;
	}

	pObject->OnKnockback( dir, fKnockbackForce );
	float fActualDamage = CalcActualDamage(pOwner, pickinfo.pObject, (float)pDesc->m_nDamage.Ref(), pDesc->m_nWeaponType.Ref());
	if(ZGetGameClient()->GetChannelType() == MCHANNEL_TYPE_DUELTOURNAMENT && strstr(ZGetGameClient()->GetChannelName(), "(AntiLead)") && pickinfo.pObject->IsNPC() == false || ZGetGameClient()->GetMatchStageSetting()->IsAntiLead() == true && pickinfo.pObject->IsNPC() == false)
	{
		if (pOwner != NULL)
		{
			ZCharacter* pVictim = (ZCharacter*)(pickinfo.pObject);
			if (pVictim && pVictim->m_dwStatusBitPackingValue.Ref().m_bLostConEffect == false && !pVictim->IsDie())
			{
				if(!ZGetGame()->GetMatch()->IsTeamPlay() || ZGetGame()->GetMatch()->IsTeamPlay() && ZGetGame()->GetMatch()->GetTeamKillEnabled() || !pVictim->IsTeam(((ZCharacter*)pOwner)))
				{
					shot = new MTD_SHOTINFO();
					shot->MYID = pVictim->GetUID();
					//rvector srcPos = pOwner->GetPosition();
					shot->X = pos.x;
					shot->Y = pos.y;
					shot->Z = pos.z;
					shot->Damage = fActualDamage;
					shot->Parts = (char)pickinfo.info.parts;
				}
			}
		}
		//if(((ZCharacter*)(pickinfo.pObject))->m_dwStatusBitPackingValue.Ref().m_bLostConEffect == false)
	//	{
		//	((ZCharacter*)(pickinfo.pObject))->OnDamagedAPlayer(pOwner, pOwner->GetPosition(), pickinfo.info.parts, fActualDamage);
		//}
	} else {
		float fRatio = pItem->GetPiercingRatio( pDesc->m_nWeaponType.Ref(), pickinfo.info.parts );
		ZDAMAGETYPE dt = (pickinfo.info.parts==eq_parts_head) ? ZD_BULLET_HEADSHOT : ZD_BULLET;
		pickinfo.pObject->OnDamaged(pOwner, pOwner->GetPosition(), dt, pDesc->m_nWeaponType.Ref(), fActualDamage, fRatio );
	}
	if(!m_Match.IsTeamPlay() || (pTargetCharacter->GetTeamID()!=pObject->GetTeamID()))
	{
		bHitEnemy = true;
	}

	v1 = pos;
	v2 = pickinfo.info.vOut;

	waterSound = GetWorld()->GetWaters()->CheckSpearing( v1, v2, 250, 0.3, !waterSound );
	return shot;
}


bool ZGame::CanISeeAttacker( ZCharacter* pAtk, const rvector& vRequestPos )
{
	const rvector& vAtkPos = pAtk->GetPosition();

	long double x = pow(vAtkPos.x - vRequestPos.x, 2);
	long double y = pow(vAtkPos.y - vRequestPos.y, 2);
	long double z = pow(vAtkPos.z - vRequestPos.z, 2);

	long double Len = x + y + z;

	// base info에 저장되어 있는 위치와 현제 발사한 무기위 위치차가 아래 수치보다 작어야 한다.
	// 대략 최대 1초정도 딜레이가 될수 있다고 가정함.
	// 만약 더 길다면 비 정상 위치에서 공격한 걸로 판단한다. - by SungE 2007-04-17
#define MAX_VIEW_LENGTH 800000 // 대략 장검대쉬 2번한 거리.

	if( MAX_VIEW_LENGTH < Len )
	{
#ifdef _DEBUG
		static rvector rv( 0.0f, 0.0f, 0.0f );

		long double l = pow(vRequestPos.x - rv.x, 2) + pow(vRequestPos.y - rv.y, 2) + pow(vRequestPos.z - rv.z, 2);

		rv = vRequestPos;

		mlog( "len : %f(%f), res(%d)\n", Len, sqrt(Len), MAX_VIEW_LENGTH < Len );
#endif
		return false;
	}

	return true;
}


// shot 을 shot_range, shot_melee, shot_shotgun 으로 command 를 각각 분리하는것도 방법이 좋을듯.
void ZGame::OnPeerShot( const MUID& uid, float fShotTime, const rvector& pos, const rvector& to, const MMatchCharItemParts sel_type)
{
	ZCharacter* pOwnerCharacter = NULL;		// 총 쏜 사람

	pOwnerCharacter = m_CharacterManager.Find(uid);

	if (pOwnerCharacter == NULL) return;
	if(!pOwnerCharacter->IsVisible()) return;

#ifdef LOCALE_NHNUSA
	if( !CanISeeAttacker(pOwnerCharacter, pos) ) return;
#endif

	pOwnerCharacter->OnShot();
	
	// fShotTime 이 그 캐릭터의 로컬 시간이므로 내 시간으로 변환해준다
	fShotTime-=pOwnerCharacter->m_fTimeOffset;
	
/*
	float fCurrentTime = g_pGame->GetTime();
	if( abs(fCurrentTime - fShotTime) > TIME_ERROR_BETWEEN_RECIEVEDTIME_MYTIME )
	{
#ifdef _DEBUG
		mlog("!!칼샷 핵!!!캐릭터 네임: %s      fShotTime : %f     fCurrentTime : %f \n", 
			pOwnerCharacter->GetUserName(), (fShotTime - pOwnerCharacter->m_fTimeOffset) , fCurrentTime);
#endif
		return;
	}
	이 부분은 핵에서 shot을 한 시간을 조작하여 보내는 것을 감지하여 핵을 막는 코드였는데 받는 쪽에서 시간 검사를 하지 말고 
	보내는 쪽에서 검사를 해서 shot을 한 시간이 해당 캐릭터의 lacal time과 맞지 않으면 아예 패킷을 보내지 않도록 바꿨다. 
	따라서 해당 코드가 필요 없게 됨. 추후 localtime을 조작할 경우를 대비해 주석처리로 남겨둠..
*/

	ZItem *pItem = pOwnerCharacter->GetItems()->GetItem(sel_type);
	if(!pItem || !pItem->GetDesc()) return;

	// 비정상적인 발사속도를 무시한다.
	if (pOwnerCharacter->CheckValidShotTime(pItem->GetDescID(), fShotTime, pItem)) 
	{
		pOwnerCharacter->UpdateValidShotTime(pItem->GetDescID(), fShotTime);
	} 
	else 
	{
		return;
	}

	//// 루프중 MEMORYHACK있었나 검사
	if (uid == ZGetMyUID()) {

		int nCurrMagazine = pItem->GetBulletCurrMagazine();

		// 실제로 무기를 소비
		if (!pItem->Shot()) return;

		if (!(pItem->GetBulletCurrMagazine() < nCurrMagazine))	// Shot에서 총알 줄어야만 정상이다
			if(sel_type != MMCIP_MELEE)
				ZGetApplication()->Exit();
	} else {
		// 실제로 무기를 소비
		if (!pItem->Shot()) {
//			_ASSERT(FALSE);	// 문제가있다, 치팅 ?
			if(!ZGetGame()->IsReplay())	// 리플레이라면 총알의 유무에 상관없이 발사처리를 해준다.
				return;	// SHOT
		}
	}

	// MELEE일 경우
	if (sel_type == MMCIP_MELEE)
	{
		OnPeerShot_Melee(uid,fShotTime);

		return;
	}

	if ((sel_type != MMCIP_PRIMARY) && (sel_type != MMCIP_SECONDARY) &&	(sel_type != MMCIP_CUSTOM1 )) return;


	if(!pItem->GetDesc()) return;
	MMatchWeaponType wtype = pItem->GetDesc()->m_nWeaponType.Ref();

	if(wtype == MWT_SHOTGUN || wtype == MWT_DOUBLE_SHOTGUN)
	{
		OnPeerShot_Shotgun(sel_type,pOwnerCharacter,fShotTime,pos,to);
		return;
	} else {
		OnPeerShot_Range(sel_type,uid,fShotTime,pos,to);

		rvector position;
		pOwnerCharacter->GetWeaponTypePos( weapon_dummy_muzzle_flash , &position );
		if( ZGetConfiguration()->GetVideo()->bDynamicLight )
		{
			RGetDynamicLightManager()->AddLight( GUNFIRE, position );
		}
	}
}

void ZGame::OnPeerDie(MUID& uidVictim, MUID& uidAttacker)
{
	ZCharacter* pVictim = m_CharacterManager.Find(uidVictim);
	if (pVictim == NULL) return;

	pVictim->ActDead();

	if (pVictim == m_pMyCharacter)	
	{
		if (GetMatch()->GetMatchType() == MMATCH_GAMETYPE_CTF && ZGetMyInfo()->GetGameInfo()->bForcedChangeTeam)
		{
			m_pMyCharacter->SetTeamID(NegativeTeam(m_pMyCharacter->GetTeamID()));
			ZGetMyInfo()->GetGameInfo()->bForcedChangeTeam = false;
		}
		pVictim->Die();		// 여기서 실제로 죽는다. 나 자신은 실제로도 여기서 죽는것 처리

		// 팀플시 또는 퀘스트모드시 죽으면 옵져버 모드
		if (m_Match.IsWaitForRoundEnd())
		{
			if (m_CharacterManager.GetCount() > 2)
			{
				if (GetMatch()->GetMatchType() != MMATCH_GAMETYPE_DUEL)
					ReserveObserver();
			}
		}
#ifdef _QUEST
		else if (ZGetGameTypeManager()->IsQuestDerived(ZGetGameClient()->GetMatchStageSetting()->GetGameType()))
		{
			if (m_CharacterManager.GetCount() >= 2)
			{
				ReserveObserver();
			}
		}
#endif

		CancelSuicide();
	}
	else
	{
		if (GetMatch()->GetMatchType() == MMATCH_GAMETYPE_CTF && pVictim->TeamChange == true)
		{
			pVictim->TeamChange = false;
			pVictim->SetTeamID(NegativeTeam(pVictim->GetTeamID()));
		}
	}


	ZCharacter* pAttacker = m_CharacterManager.Find(uidAttacker);
	if (pAttacker == NULL) return;	// 죽인 사람이 누구인지 모를 경우는 그냥 리턴
	if(pAttacker!=pVictim)	// 자살이 아니면 이펙트 보여줄거 보여주자
	{
		if (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_DUEL)
		{
			if (pAttacker->GetKils() + 1 == 5)
			{
				MEMBER_SET_CHECKCRC(pAttacker->GetStatus(), nFantastic, pAttacker->GetStatus().Ref().nFantastic+1);
				pAttacker->AddIcon(ZCI_FANTASTIC);
			}
			else if (pAttacker->GetKils() + 1 == 15)
			{
				MEMBER_SET_CHECKCRC(pAttacker->GetStatus(), nExcellent, pAttacker->GetStatus().Ref().nExcellent+1);
				pAttacker->AddIcon(ZCI_EXCELLENT);
			}
			else if (pAttacker->GetKils() + 1 == 30)
			{
				MEMBER_SET_CHECKCRC(pAttacker->GetStatus(), nUnbelievable, pAttacker->GetStatus().Ref().nUnbelievable+1);
				pAttacker->AddIcon(ZCI_UNBELIEVABLE);
			}
		}
		else
		{
			if (pAttacker->GetKils() >= 3)
			{
				MEMBER_SET_CHECKCRC(pAttacker->GetStatus(), nFantastic, pAttacker->GetStatus().Ref().nFantastic+1);
				pAttacker->AddIcon(ZCI_FANTASTIC);
			}
		}

		if(pVictim->GetLastDamageType()==ZD_BULLET_HEADSHOT)
		{
			MEMBER_SET_CHECKCRC(pAttacker->GetStatus(), nHeadShot, pAttacker->GetStatus().Ref().nHeadShot+1);
			pAttacker->AddIcon(ZCI_HEADSHOT);
		}
	}
}

// 서버로부터 직접 날라오는 Dead메세지
void ZGame::OnPeerDead(const MUID& uidAttacker, int AddedXp, int Percent, const MUID& uidVictim, int LostXp, int LostPercent, int nAttackerStreak, int nVictimStreak)
{
	ZCharacter* pVictim = m_CharacterManager.Find(uidVictim);
	ZCharacter* pAttacker = m_CharacterManager.Find(uidAttacker);

	bool bSuicide = false;
	if (uidAttacker == uidVictim) bSuicide = true;

	int nAttackerExp = 0, nVictimExp = 0;

	nAttackerExp = AddedXp;
	nVictimExp = -LostXp;

	if(pAttacker)
	{
		pAttacker->GetStatus().CheckCrc();

		pAttacker->GetStatus().Ref().AddExp(nAttackerExp);
		if (!bSuicide) 
			pAttacker->GetStatus().Ref().AddKills();
		pAttacker->GetStatus().MakeCrc();
	}

	if(pVictim)
	{
		if (pVictim != m_pMyCharacter)
		{
			pVictim->Die();		// 여기서 실제로 죽는다
		}

		pVictim->GetStatus().CheckCrc();
		
		pVictim->GetStatus().Ref().AddExp(nVictimExp);
		pVictim->GetStatus().Ref().AddDeaths();
		if (pVictim->GetStatus().Ref().nLife > 0)
			pVictim->GetStatus().Ref().nLife--;
		pVictim->GetStatus().MakeCrc();
	}

	// 화면 경험치 이펙트 표시
	if(bSuicide && (ZGetCharacterManager()->Find(uidAttacker)==ZGetGame()->m_pMyCharacter)) 
	{
		// 자살
		ZGetScreenEffectManager()->AddExpEffect(nVictimExp);
		int nExpPercent = LostPercent;
		ZGetMyInfo()->SetLevelPercent(nExpPercent);

		ZGetScreenEffectManager()->SetGaugeExpFromMyInfo();
	}
	else if(ZGetCharacterManager()->Find(uidAttacker)==m_pMyCharacter) 
	{
		// 내가 attacker 일때
		ZGetScreenEffectManager()->AddExpEffect(nAttackerExp);

		int nExpPercent = Percent;
		ZGetMyInfo()->SetLevelPercent(nExpPercent);
		ZGetScreenEffectManager()->SetGaugeExpFromMyInfo();
	}
	else if(ZGetCharacterManager()->Find(uidVictim)==m_pMyCharacter) 
	{
		// 내가 victim 일때
		ZGetScreenEffectManager()->AddExpEffect(nVictimExp);

		int nExpPercent =LostPercent;
		ZGetMyInfo()->SetLevelPercent(nExpPercent);
		ZGetScreenEffectManager()->SetGaugeExpFromMyInfo();
	}

	m_Match.AddRoundKills();

	CheckKillSound(pAttacker);
	OnPeerDieMessage(pVictim, pAttacker, nAttackerStreak, nVictimStreak);
}

void ZGame::CheckKillSound(ZCharacter* pAttacker)
{
	if ((!pAttacker) || (pAttacker != m_pMyCharacter)) return;

	if (m_Match.GetRoundKills() == 1)
	{
		ZApplication::GetSoundEngine()->PlayVoiceSound(VOICE_FIRST_KILL);
	}
}

void ZGame::OnReceiveTeamBonus(const MUID& uidChar, const unsigned long int nExpArg)
{
	ZCharacter* pCharacter = m_CharacterManager.Find(uidChar);
	if (pCharacter == NULL) return;

	int nExp = 0;

	nExp = GetExpFromTransData(nExpArg);

	if(pCharacter)
	{
		pCharacter->GetStatus().CheckCrc();
		pCharacter->GetStatus().Ref().AddExp(nExp);
		pCharacter->GetStatus().MakeCrc();
	}

	// 화면 경험치 이펙트 표시
	if(pCharacter==m_pMyCharacter) 
	{
#ifdef _DEBUG
		char szTemp[128];
		sprintf(szTemp, "TeamBonus = %d\n", nExp);
		OutputDebugString(szTemp);
#endif

		// 내가 attacker 일때
		ZGetScreenEffectManager()->AddExpEffect(nExp);

		
		int nExpPercent = GetExpPercentFromTransData(nExpArg);
		ZGetMyInfo()->SetLevelPercent(nExpPercent);
		ZGetScreenEffectManager()->SetGaugeExpFromMyInfo();
	}
}

void ZGame::OnPeerDieMessage(ZCharacter* pVictim, ZCharacter* pAttacker, int nAttackerStreak, int nVictimStreak)
{
	char szMsg[256] = "";

	const char *szAnonymous = "NOTFOUND";

	char szVictim[256];
	strcpy(szVictim, pVictim ? pVictim->GetUserAndClanName() : szAnonymous);

	char szAttacker[256];
	strcpy(szAttacker, pAttacker ? pAttacker->GetUserAndClanName() : szAnonymous);
	// 자살일 경우
	if(pAttacker==pVictim && nAttackerStreak>=4)
	{
		sprintf( szMsg, "%s has lost his own %d kill streak.", szAttacker, nAttackerStreak);
		ZChatOutput(MCOLOR(240, 26, 26), szMsg);
	} else if(pAttacker!=pVictim)
	{
		if(nVictimStreak>=4)
		{
			sprintf( szMsg, "%s has stopped %s %d kill streak.", szAttacker, szVictim, nVictimStreak);
			ZChatOutput(MCOLOR(240, 26, 26), szMsg);
		}
		bool bmsg=true;
		switch(nAttackerStreak)
		{
			case 4:
				sprintf( szMsg, "%s is on a streak with %d kills.", szAttacker, nAttackerStreak);
				break;
			case 6:
				sprintf( szMsg, "%s is owning with %d kills.", szAttacker, nAttackerStreak);
				break;
			case 8:
				sprintf( szMsg, "%s is pwning with %d kills.", szAttacker, nAttackerStreak);
				break;
			case 10:
				sprintf( szMsg, "%s is dropping with %d kills.", szAttacker, nAttackerStreak);
				break;
			case 14:
				sprintf( szMsg, "%s is dominating with %d kills.", szAttacker, nAttackerStreak);
				break;
			default:
				if(nAttackerStreak >= 17)
				{
					sprintf( szMsg, "%s is unstoppable with %d kills.", szAttacker, nAttackerStreak);
				} else
				bmsg=false;
				break;
		}
		if(bmsg==true)
		{
			ZChatOutput(MCOLOR(240, 26, 26), szMsg);
		}
	}
	if(pAttacker==pVictim)
	{
		if (pVictim == m_pMyCharacter)
		{
			if(m_pMyCharacter->GetLastDamageType()==ZD_EXPLOSION) {
//				sprintf(szMsg, "당신은 자신의 폭탄으로 인하여 패배 하였습니다.");
				sprintf( szMsg, ZMsg(MSG_GAME_LOSE_BY_MY_BOMB) );
			}
			else {
//				sprintf(szMsg, "당신은 스스로 패배하였습니다.");
				sprintf( szMsg, ZMsg(MSG_GAME_LOSE_MYSELF) );
			}

			ZChatOutput(MCOLOR(0xFFCF2020), szMsg);
		}
		else
		{
//			sprintf(szMsg, "%s님이 스스로 패배하였습니다.", szAttacker);
			ZTransMsg( szMsg, MSG_GAME_WHO_LOSE_SELF, 1, szAttacker );
			ZChatOutput(MCOLOR(0xFF707070), szMsg);

			// Admin Grade
			if (ZGetMyInfo()->IsAdminGrade()) {
				MMatchObjCache* pCache = ZGetGameClient()->FindObjCache(ZGetMyUID());
				if (pCache && pCache->CheckFlag(MTD_PlayerFlags_AdminHide))
				{
					sprintf( szMsg, "(%s)%s Died.",
									(pAttacker->GetTeamID() == MMT_BLUE) ? "Blue" : "Red",
									pAttacker->GetProperty()->GetName());
					ZGetGameInterface()->GetCombatInterface()->m_AdminMsg.OutputChatMsg( szMsg);
				}
			}
		}

	}

	// 내가 죽였을 때
	else if (pAttacker == m_pMyCharacter)
	{
		int Kills = 1, TotalKills = 0;
		if(pVictim)
		{
			{
				LocalKillStreak::iterator i = LocalKillData.find((char *)pVictim->GetCharInfo()->szName);
				if(i != LocalKillData.end()) 
				{
					ZLocalKillInfo* Info = i->second;
					if(Info)
					{
						TotalKills = Info->TotalKills;
						Info->Reset();
					}
				}
			}
			LocalAttKillStreak::iterator i = LocalAttKillData.find((char *)pVictim->GetCharInfo()->szName);
			if(i==LocalAttKillData.end()) LocalAttKillData.insert(LocalAttKillStreak::value_type((char *)pVictim->GetCharInfo()->szName, 1));
			else 
			{
				i->second++;
				Kills = i->second;
			}
		}
		char lol[50] = {"0"};
		sprintf(lol, "%d", Kills);
		char lol1[50] = {"0"};
		sprintf(lol1, "%d", TotalKills);
		ZTransMsg( szMsg, MSG_GAME_WIN_FROM_WHO, 3, szVictim, lol, lol1 );
		ZChatOutput(MCOLOR(0xFF80FFFF), szMsg);
	}

	// 내가 죽었을 때
	else if (pVictim == m_pMyCharacter)
	{
		int TotalKills = 1, Kills = 0;
		if(pAttacker)
		{
			{
				LocalKillStreak::iterator i = LocalKillData.find((char *)pAttacker->GetCharInfo()->szName);
				if(i == LocalKillData.end()) LocalKillData.insert(LocalKillStreak::value_type((char *)pAttacker->GetCharInfo()->szName, new ZLocalKillInfo()));
				else 
				{
					ZLocalKillInfo* Info = i->second;
					if(Info)
					{
						Info->AddKills();
						bool msg = false;
						TotalKills = Info->TotalKills;
						switch(Info->Kills)
						{
						case 3://after 3 kills first will show
							sprintf(szMsg, "%s is dominating you with %d kills on you.", szAttacker, Info->Kills);
							msg = true;
							break;
						default:
							if(Info->Kills > 3)
							{
								sprintf(szMsg, "%s is dominating you with %d kills on you.", szAttacker, Info->Kills);
								msg = true;
							}
							break;
						}
						if(msg==true) ZChatOutput(MCOLOR(0xFFCF2020), szMsg);
					}
				}
			}
			LocalAttKillStreak::iterator i = LocalAttKillData.find((char *)pVictim->GetCharInfo()->szName);
			if(i==LocalAttKillData.end()) LocalAttKillData.insert(LocalAttKillStreak::value_type((char *)pVictim->GetCharInfo()->szName, 1));
			else 
			{
				i->second++;
				Kills = i->second;
			}
			MMatchObjCache* pObjCache = ZGetGameClient()->FindObjCache(ZGetMyUID());
			if(ZGetGameClient()->GetMatchStageSetting()->GetGameType() != MMATCH_GAMETYPE_DEATHMATCH_TEAM && !ZGetGameClient()->IsPlayerWars() && !ZGetGameClient()->IsLadderGame() && !m_Match.IsQuestDrived() && pObjCache && pObjCache->CheckFlag(MTD_PlayerFlags_AdminHide) == false)
			{
				ZGetCombatInterface()->SetDeathObserverMode(pAttacker->GetUID());
			}
		}
		char lol[50] = {"0"};
		sprintf(lol, "%d", Kills);
		char lol1[50] = {"0"};
		sprintf(lol1, "%d", TotalKills);
		ZTransMsg( szMsg, MSG_GAME_LOSE_FROM_WHO, 3, szAttacker, lol1, lol );
		ZChatOutput(MCOLOR(0xFFCF2020), szMsg);
	}

	// 다른 사람이 다른 사람 죽였을때
	else
	{
// 		sprintf(szMsg, "%s님이 %s님으로부터 승리하였습니다.", szAttacker, szVictim );
		ZTransMsg( szMsg, MSG_GAME_WHO_WIN_FROM_OTHER, 2, szAttacker, szVictim );
		ZChatOutput(MCOLOR(0xFF707070), szMsg);
		// Admin Grade
		if (ZGetMyInfo()->IsAdminGrade()) {
			MMatchObjCache* pCache = ZGetGameClient()->FindObjCache(ZGetMyUID());
			if (pCache && pCache->CheckFlag(MTD_PlayerFlags_AdminHide))
			{
				sprintf( szMsg, "(%s)%s Slaughterd (%s)%s.",
							(pAttacker->GetTeamID() == MMT_BLUE) ? "Blue" : "Red", pAttacker->GetProperty()->GetName(),
							(pVictim->GetTeamID() == MMT_BLUE) ? "Blue" : "Red",  pVictim->GetProperty()->GetName());
				ZGetGameInterface()->GetCombatInterface()->m_AdminMsg.OutputChatMsg( szMsg);
			}
		}
	}
}


void ZGame::OnReloadComplete(ZCharacter *pCharacter)
{
	ZItem* pItem = pCharacter->GetItems()->GetSelectedWeapon();
	
	pCharacter->GetItems()->Reload();

	if(pCharacter==m_pMyCharacter)
		ZApplication::GetSoundEngine()->PlaySound("we_weapon_rdy");

	return;

/*	ZItem* pItem = pCharacter->GetItems()->GetSelectedWeapon();
	//// 루프중 MEMORYHACK있었나 검사
	if (pCharacter->GetUID() == ZGetMyUID() && pItem!=NULL) {
		MDataChecker* pChecker = ZGetGame()->GetDataChecker();
		MDataCheckNode* pCheckNodeA = pChecker->FindCheck((BYTE*)pItem->GetBulletPointer());
		MDataCheckNode* pCheckNodeB = pChecker->FindCheck((BYTE*)pItem->GetAMagazinePointer());
		if ( (pCheckNodeA && (pCheckNodeA->UpdateChecksum()==false)) ||
		 	 (pCheckNodeB && (pCheckNodeB->UpdateChecksum()==false)) ) 
		{
			pChecker->BringError();	//// MEMORYHACK 감지. Checksum 모두중단하고 끝장낸다.
		} else {
			bool bResult = pCharacter->GetItems()->Reload();	//// RELOAD ////

			//// MEMORYHACK 없었으면 리뉴한다.
			pChecker->RenewCheck((BYTE*)pItem->GetBulletPointer(), sizeof(int));
			pChecker->RenewCheck((BYTE*)pItem->GetAMagazinePointer(), sizeof(int));
		}
	} else {
		bool bResult = pCharacter->GetItems()->Reload();	//// RELOAD ////
	}

	if(pCharacter==m_pMyCharacter) {
		ZApplication::GetSoundEngine()->PlaySound("we_weapon_rdy");
	}
	return;*/
}

void ZGame::OnPeerSpMotion(MUID& uid,int nMotionType)
{
	ZCharacter* pCharacter = m_CharacterManager.Find(uid);

	if (pCharacter == NULL) return;

	pCharacter->m_dwStatusBitPackingValue.Ref().m_bSpMotion = true;

	ZC_STATE_LOWER zsl = ZC_STATE_TAUNT;

	if(nMotionType == ZC_SPMOTION_TAUNT)
	{
		zsl = ZC_STATE_TAUNT;

		char szSoundName[ 50];
		if ( pCharacter->GetProperty()->nSex == MMS_MALE)
			sprintf( szSoundName, "fx2/MAL1%d", (RandomNumber(0, 300) % 3) + 1);
		else
			sprintf( szSoundName, "fx2/FEM1%d", (RandomNumber(0, 300) % 3) + 1);

		ZGetSoundEngine()->PlaySound( szSoundName, pCharacter->GetPosition());
	}
	else if(nMotionType == ZC_SPMOTION_BOW)
		zsl = ZC_STATE_BOW;
	else if(nMotionType == ZC_SPMOTION_WAVE)
		zsl = ZC_STATE_WAVE;
	else if(nMotionType == ZC_SPMOTION_LAUGH)
	{
		zsl = ZC_STATE_LAUGH;

		if ( pCharacter->GetProperty()->nSex == MMS_MALE)
			ZGetSoundEngine()->PlaySound( "fx2/MAL01", pCharacter->GetPosition());
		else
			ZGetSoundEngine()->PlaySound( "fx2/FEM01", pCharacter->GetPosition());
	}
	else if(nMotionType == ZC_SPMOTION_CRY)
	{
		zsl = ZC_STATE_CRY;

		if ( pCharacter->GetProperty()->nSex == MMS_MALE)
			ZGetSoundEngine()->PlaySound( "fx2/MAL02", pCharacter->GetPosition());
		else
			ZGetSoundEngine()->PlaySound( "fx2/FEM02", pCharacter->GetPosition());
	}
	else if(nMotionType == ZC_SPMOTION_DANCE)
		zsl = ZC_STATE_DANCE;

	pCharacter->m_SpMotion = zsl;

	pCharacter->SetAnimationLower( zsl );
}

void ZGame::OnPeerReload(MUID& uid)
{
	ZCharacter* pCharacter = m_CharacterManager.Find(uid);
	//	if (uid == ZGetGameClient()->GetUID()) pCharacter = m_pMyCharacter;

	if (pCharacter == NULL || pCharacter->IsDie() ) return;

	// 임시.. 뭔가 상체애니메이션이 있으면 캔슬.. 리로드/샷 따위
	//	if(pCharacter->m_pVMesh->m_pAniSet[ani_mode_upper]!=NULL) return;

	// 내 캐릭터는 애니메이션이 끝날때에 실제로 리로드 시킨다.

 /*if(strstr(ZGetGameClient()->GetStageName(),"[R]")) 
      OnReloadComplete(pCharacter);
   else*/ if(pCharacter==m_pMyCharacter)
      m_pMyCharacter->Animation_Reload();
    else
      OnReloadComplete(pCharacter);
	// Sound Effect
	if(pCharacter->GetItems()->GetSelectedWeapon()!=NULL) {
		rvector p = pCharacter->GetPosition()+rvector(0,0,160.f);
		ZApplication::GetSoundEngine()->PlaySEReload(pCharacter->GetItems()->GetSelectedWeapon()->GetDesc(), p.x, p.y, p.z, (pCharacter==m_pMyCharacter));
	}
}

void ZGame::OnPeerChangeCharacter(MUID& uid)
{
	ZCharacter* pCharacter = m_CharacterManager.Find(uid);

	//	if (uid == ZGetGameClient()->GetUID()) pCharacter = m_pMyCharacter;

	if (pCharacter == NULL) return;

	pCharacter->TestToggleCharacter();
}

/*
void ZGame::OnAssignCommander(const MUID& uidRedCommander, const MUID& uidBlueCommander)
{
	AssignCommander(uidRedCommander, uidBlueCommander);
}

void ZGame::AssignCommander(const MUID& uidRedCommander, const MUID& uidBlueCommander)
{
	ZCharacter* pRedChar = m_CharacterManager.Find(uidRedCommander);
	ZCharacter* pBlueChar = m_CharacterManager.Find(uidBlueCommander);

	if(pRedChar) {
		ZGetEffectManager()->AddCommanderIcon(pRedChar,0);
		pRedChar->m_bCommander = true;
	}
	if(pBlueChar) {
		ZGetEffectManager()->AddCommanderIcon(pBlueChar,1);
		pBlueChar->m_bCommander = true;
	}

#ifdef _DEBUG
	//// DEBUG LOG ////
	const char *szUnknown = "unknown";
	char szBuf[128];
	sprintf(szBuf, "RedCMDER=%s , BlueCMDER=%s \n", 
		pRedChar ? pRedChar->GetProperty()->szName : szUnknown , 
		pBlueChar ? pBlueChar->GetProperty()->szName : szUnknown );
	OutputDebugString(szBuf);
	///////////////////
#endif
}
*/
void ZGame::OnSetObserver(MUID& uid)
{
	ZCharacter* pCharacter = m_CharacterManager.Find(uid);
	if (pCharacter == NULL) return;

	if(pCharacter==m_pMyCharacter)
	{
		ZGetCombatInterface()->SetObserverMode(true);
	}
	pCharacter->SetVisible(false);
	pCharacter->ForceDie();
}

void MyThread()
{	
/*	*/ //that was my hp check failed at some ppls pcs no idea why yet
}

void ZGame::OnPeerSpawn(MUID& uid, rvector& pos, rvector& dir)
{
	m_nSpawnTime = timeGetTime();
	SetSpawnRequested(false);

	ZCharacter* pCharacter = m_CharacterManager.Find(uid);
	if (pCharacter == NULL) return;

	bool isRespawn	= ( pCharacter->IsDie() == true ) ? true : false;
	//	dir = rvector(-1.f,0,0);
	pCharacter->SetVisible(true);
	pCharacter->Revival();
	pCharacter->SetPosition(pos);
	pCharacter->SetDirection(dir);
	pCharacter->SetSpawnTime(GetTime());

	ZGetEffectManager()->AddReBirthEffect(pos);

	if(pCharacter==m_pMyCharacter)
	{
		m_pMyCharacter->InitSpawn();

		if( isRespawn )	{
			ZGetSoundEngine()->PlaySound("fx_respawn");
		} else {
			ZGetSoundEngine()->PlaySound("fx_whoosh02");
		}

		ZGetScreenEffectManager()->ReSetHpPanel();// hppanel_ani
			m_pMyCharacter->GetStatus().CheckCrc();
			int LastGiven = m_pMyCharacter->GetStatus().Ref().nRoundGivenDamage, LastTaken = m_pMyCharacter->GetStatus().Ref().nRoundTakenDamage;
			m_pMyCharacter->GetStatus().Ref().nRoundLastGivenDamage = LastGiven;
			m_pMyCharacter->GetStatus().Ref().nRoundLastTakenDamage = LastTaken;
			ZPOSTLASTDMG(MCommandParameterInt(LastGiven), MCommandParameterInt(LastTaken));
			m_pMyCharacter->GetStatus().Ref().nRoundGivenDamage = 0;
			m_pMyCharacter->GetStatus().Ref().nRoundTakenDamage = 0;
			m_pMyCharacter->GetStatus().Ref().LocalSentDamage = 0;
			m_pMyCharacter->GetStatus().MakeCrc();	
			if(ZGetGame()->m_pMyCharacter->IsObserverTarget() == false && ZGetGame()->IsReplay() == false)
			{
				float fAddedAP = 55, fAddedHP = 145;//added 10 to both
				for (int i = 0; i < MMCIP_END; i++)
				{
					if (!ZGetGame()->m_pMyCharacter->GetItems()->GetItem(MMatchCharItemParts(i))->IsEmpty()) 
					{
						fAddedAP += ZGetGame()->m_pMyCharacter->GetItems()->GetItem(MMatchCharItemParts(i))->GetDesc()->m_nAP.Ref();
						fAddedHP += ZGetGame()->m_pMyCharacter->GetItems()->GetItem(MMatchCharItemParts(i))->GetDesc()->m_nHP.Ref();
					}
				}

				if(ZGetGame()->m_pMyCharacter->GetHP() > fAddedHP || ZGetGame()->m_pMyCharacter->GetAP() > fAddedAP || ZGetGame()->m_pMyCharacter->GetHP() < -1)
				{
					ZPostDisconnect();
					mlog("11\n");
					
				}
			}
		}
#ifndef _PUBLISH
	char szLog[128];
	sprintf(szLog, "ZGame::OnPeerSpawn() - %s(%u) Spawned \n", 
		pCharacter->GetProperty()->GetName(), pCharacter->GetUID().Low);
	OutputDebugString(szLog);
#endif

	if (GetMatch()->GetMatchType() == MMATCH_GAMETYPE_DEATHMATCH_TEAM2 || ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_DEATHMATCH_TEAM2_NPC || GetMatch()->GetMatchType() == MMATCH_GAMETYPE_CTF)
		pCharacter->SetInvincibleTime( 5000);
	// Custom: Spawn protection for non-team modes
	else if (!ZGetGameTypeManager()->IsTeamGame(GetMatch()->GetMatchType()) && GetMatch()->GetMatchType() != MMATCH_GAMETYPE_DUELTOURNAMENT && GetMatch()->GetMatchType() != MMATCH_GAMETYPE_DUEL)
		pCharacter->SetInvincibleTime(1500);
}

void ZGame::OnPeerDash(MCommand* pCommand)
{
	MCommandParameter* pParam = pCommand->GetParameter(0);
	if (pParam->GetType() != MPT_BLOB) return;

	MUID uid = pCommand->GetSenderUID();
	ZPACKEDDASHINFO* ppdi = (ZPACKEDDASHINFO*)pParam->GetPointer();

	rvector pos, dir;
	int sel_type;


	pos = rvector(Roundf(ppdi->posx), Roundf(ppdi->posy), Roundf(ppdi->posz));
	dir = 1.f / 32000.f * rvector(ppdi->dirx, ppdi->diry, ppdi->dirz);
	sel_type = (int)ppdi->seltype;


	ZCharacter* pCharacter = m_CharacterManager.Find(uid);

	if (pCharacter == NULL) return;

	MMatchCharItemParts parts = (MMatchCharItemParts)sel_type;

	if (parts != pCharacter->GetItems()->GetSelectedWeaponParts()) {
		// 지금 들고 있는 무기와 보내진 무기가 틀리다면 보내진 무기로 바꿔준다..
		OnChangeWeapon(uid, parts);
	}

	ZGetEffectManager()->AddDashEffect(pos, dir, pCharacter, ppdi->nDashColor);

	/*
	// 내가 아닌 경우에 애니메이션이 씹히는 현상이 있으므로 다시 세팅해준다
	if(pCharacter!=m_pMyCharacter)
	{	
		// 프로토콜을 바꿀수 없으므로 이미 있는 방향으로 판단한다
		rvector charDir = pCharacter->GetDirection();
		rvector charRight;
		CrossProduct(&charRight,dir,rvector(0,0,1));

		float fDotMax = 0.f , fDot;

		int nTumbleDir = 0;
		fDot=DotProduct(dir,-charDir);
		if(fDot>fDotMax) {	nTumbleDir = 1; fDotMax = fDot; }
		fDot=DotProduct(dir,charRight);
		if(fDot>fDotMax) {	nTumbleDir = 2; fDotMax = fDot; }
		fDot=DotProduct(dir,-charRight);
		if(fDot>fDotMax) {	nTumbleDir = 3; fDotMax = fDot; }

		switch(nTumbleDir)
		{
		case 0 :pCharacter->SetAnimationLower(ZC_STATE_LOWER_TUMBLE_FORWARD);break;
		case 1 :pCharacter->SetAnimationLower(ZC_STATE_LOWER_TUMBLE_BACK);break;
		case 2 :pCharacter->SetAnimationLower(ZC_STATE_LOWER_TUMBLE_RIGHT);break;
		case 3 :pCharacter->SetAnimationLower(ZC_STATE_LOWER_TUMBLE_LEFT);break;
		}
	}
	*/

}


//#define CHAR_COLLISION_HEIGHT	170.f


rvector ZGame::GetFloor(rvector pos, rplane *pimpactplane, MUID myUID)
{
	rvector floor=ZGetGame()->GetWorld()->GetBsp()->GetFloor(pos+rvector(0,0,120),CHARACTER_RADIUS-1.1f,58.f,pimpactplane);

#ifdef ENABLE_CHARACTER_COLLISION
	ZObjectManager::iterator itor = m_ObjectManager.begin();
	for ( ;itor != m_ObjectManager.end(); ++itor)
	{
		ZObject* pObject = (*itor).second;
		if (pObject->IsCollideable())
//		if(!pCharacter->IsDie() && !pCharacter->m_bBlastDrop)
		{
			rvector diff=pObject->GetPosition()-pos;
			diff.z=0;

			// 나중에 radius상수값으로 된것 Object의 멤버변수로 고치자
			if(Magnitude(diff)<CHARACTER_RADIUS && pos.z>pObject->GetPosition().z)
			{
				rvector newfloor = pObject->GetPosition()+rvector(0,0,pObject->GetCollHeight());
				if(floor.z<newfloor.z)
				{
					if(m_pMyCharacter->GetUID() == myUID)
					{// 내 캐릭터 바닥 위치만 처리해준다.
						///< 점프버그 때문에 작업(한캐릭터위에 다른 캐릭터가 올라 갔을때 밑에 캐릭터가 점프시 
						///< 위캐릭터의 높이가 갱신이 안되면 두캐릭터가 무한정 위로 올라가는 버그)
						if(CharacterOverlapCollision(pObject, floor.z, newfloor.z) == false)
							continue;
					}

					floor=newfloor;
					if(pimpactplane)
					{
						rvector up=rvector(0,0,1);
						D3DXPlaneFromPointNormal(pimpactplane,&floor,&up);
					}
				}
			}
		}
	}
#endif

	return floor;
}

bool ZGame::CharacterOverlapCollision(ZObject* pFloorObject, float WorldFloorHeight, float ObjectFloorHeight)
{
	OVERLAP_FLOOR* pOverlapObject = m_pMyCharacter->GetOverlapFloor();

	if(pOverlapObject->FloorUID != pFloorObject->GetUID())
	{ // 밟고있던 캐릭터가 변경돼면 다시 세팅해준다.
		pOverlapObject->FloorUID = pFloorObject->GetUID();
		pOverlapObject->vecPosition.z = ObjectFloorHeight;
		pOverlapObject->nFloorCnt = 0;
		pOverlapObject->bJumpActivity = false;
	}
	else
	{ // 계속 밟고있는 캐릭터 처리
		if(pOverlapObject->bJumpActivity)
		{ // 점프버그 발동
			if(m_pMyCharacter->GetPosition().z - WorldFloorHeight > 20.f)
			{ // 낙하높이가 바닥에 가까워 졌을때 OVERLAP_FLOOR 초기화
				pOverlapObject->FloorUID = MUID(0,0);
				pOverlapObject->nFloorCnt = 0;
				pOverlapObject->vecPosition.x = 0;
				pOverlapObject->vecPosition.y = 0;
				pOverlapObject->vecPosition.z = 0;
				pOverlapObject->bJumpActivity = false;
			}
			return false;	// 점프버그가 발동됐으면 밟고있던 캐릭터를 무시함으로 낙하함
		}

		if(ObjectFloorHeight - pOverlapObject->vecPosition.z > 150.f) 
		{
			pOverlapObject->vecPosition.z = ObjectFloorHeight;
			pOverlapObject->nFloorCnt++;
			if(pOverlapObject->nFloorCnt >= 3)
			{
				pOverlapObject->bJumpActivity = true;
				mlog("Jump bug Activity \n");
				return false;
			}
		}
	}

	return true;
}

/*
rvector ZGame::GetCeiling(rvector pos)
{
rvector ceiling=g_pGame->GetWorld()->GetBsp()->GetCeiling(pos+rvector(0,0,130),CHARACTER_RADIUS-0.1f);

#ifdef ENABLE_CHARACTER_COLLISION
for (ZCharacterManager::iterator itor = m_CharacterManager.begin();
itor != m_CharacterManager.end(); ++itor)
{
ZCharacter* pCharacter = (*itor).second;
if(pCharacter!=m_pMyCharacter && !pCharacter->IsDie() && !pCharacter->m_bBlastDrop)
{
rvector diff=pCharacter->m_Position-m_pMyCharacter->m_Position;
diff.z=0;

if(Magnitude(diff)<CHARACTER_RADIUS && pos.z+CHAR_COLLISION_HEIGHT<pCharacter->m_Position.z)
{
rvector newceiling=pCharacter->m_Position;
if(ceiling.z<newceiling.z)
ceiling=newceiling;
}
}
}
#endif

return ceiling;
}
*/

bool ZGame::Pick(ZObject *pOwnerObject,rvector &origin,rvector &dir,ZPICKINFO *pickinfo,DWORD dwPassFlag,bool bMyChar)
{
	return PickHistory(pOwnerObject,GetTime(),origin,origin+10000.f*dir,pickinfo,dwPassFlag,bMyChar);
}

bool ZGame::PickTo(ZObject *pOwnerObject,rvector &origin,rvector &to,ZPICKINFO *pickinfo,DWORD dwPassFlag,bool bMyChar)
{
	return PickHistory(pOwnerObject,GetTime(),origin,to,pickinfo,dwPassFlag,bMyChar);
}

// fTime 시간의 캐릭터 위치로 pick 한다.. 캐릭터는 실린더 판정.
bool ZGame::PickHistory(ZObject *pOwnerObject,float fTime, const rvector &origin, const rvector &to,ZPICKINFO *pickinfo,DWORD dwPassFlag,bool bMyChar)
{
	pickinfo->pObject=NULL;
	pickinfo->bBspPicked=false;
	pickinfo->nBspPicked_DebugRegister=-10;

	RPickInfo info;
	memset(&info,0,sizeof(RPickInfo));

	ZObject *pObject=NULL;

	bool bCheck = false;

	float fCharacterDist=FLT_MAX;			// 캐릭터들 사이의 최소거리지점을 찾는다
	for(ZObjectManager::iterator i=m_ObjectManager.begin();i!=m_ObjectManager.end();i++)
	{
		ZObject *pc=i->second;

		bCheck = false;

		if(bMyChar) {
			if(pc==pOwnerObject && pc->IsVisible()) {
				bCheck = true;
			}
		}
		else {
			if( pc!=pOwnerObject && pc->IsVisible() ) {
				bCheck = true;
			}
		}

		if( pc->IsDie() )//죽은넘이 몸빵한다고 해서~
			bCheck = false;


		if(bCheck) //Lucas: Hitbox
		{
			rvector hitPos;
			ZOBJECTHITTEST ht = pc->HitTest(origin,to,fTime,&hitPos);
			if(ht!=ZOH_NONE) {
				float fDistToChar=Magnitude(hitPos-origin);
				if(fDistToChar<fCharacterDist) {
					pObject=pc;
					fCharacterDist=fDistToChar;
					info.vOut=hitPos;								
					switch(ht) {
						case ZOH_HEAD : info.parts=eq_parts_head;break;
						case ZOH_BODY : info.parts=eq_parts_chest;break;
						case ZOH_LEGS :	info.parts=eq_parts_legs;break;
					}
				}
			}
		}
	}

	RBSPPICKINFO bpi;
	bool bBspPicked = GetWorld()->GetBsp()->PickTo(origin, to, &bpi, dwPassFlag);

	int nCase=0;

	if(pObject && bBspPicked)		// 둘다 맞았을때는 거리가 가까운쪽을 택한다.
	{
		if(Magnitude(info.vOut-origin)>Magnitude(bpi.PickPos-origin))
			nCase=1;
		else
			nCase=2;
	}else
		if(bBspPicked)				// 둘중 하나만 맞았으면 맞은걸 택하면 된다.
			nCase=1;
		else
			if(pObject)
				nCase=2;

	if(nCase==0) return false;

	switch(nCase)
	{
	case 1 :						// 맵에 맞은경우
		pickinfo->bBspPicked=true;
		pickinfo->nBspPicked_DebugRegister = FOR_DEBUG_REGISTER;
		pickinfo->bpi=bpi;
		break;
	case 2 :						// 사람에 맞은경우.
		pickinfo->pObject=pObject;
		pickinfo->info=info;
		break;
	}
	return true;
}

bool ZGame::ObjectColTest(ZObject* pOwner, rvector& origin, rvector& to, float fRadius, ZObject** poutTarget)
{
	// 맵에 맞는것은 체크하지 않는다.

	for(ZObjectManager::iterator i=m_ObjectManager.begin();i!=m_ObjectManager.end();i++)
	{
		ZObject *pc=i->second;

		if( pc == pOwner )
			continue;
		
		if( !pc->IsVisible() ) 
			continue; 
			
		if( pc->IsDie() )
			continue;


		if (pc->ColTest(origin, to, fRadius, GetTime()))
		{
			*poutTarget = pc;
			return true;
		}
	}

	return false;
}

char* ZGame::GetSndNameFromBsp(const char* szSrcSndName, RMATERIAL* pMaterial)
{
	char szMaterial[256] = "";
	static char szRealSndName[256] = "";
	szRealSndName[0] = 0;

	if (pMaterial == NULL) return "";

	strcpy(szMaterial, pMaterial->Name.c_str());

	size_t nLen = strlen(szMaterial);

#define ZMETERIAL_SNDNAME_LEN 7

	if ((nLen > ZMETERIAL_SNDNAME_LEN) && 
		(!strnicmp(&szMaterial[nLen-ZMETERIAL_SNDNAME_LEN+1], "mt", 2)))
	{
		strcpy(szRealSndName, szSrcSndName);
		strcat(szRealSndName, "_");
		strcat(szRealSndName, &szMaterial[nLen-ZMETERIAL_SNDNAME_LEN+1]);
	}
	else
	{
		strcpy(szRealSndName, szSrcSndName);
	}


	return szRealSndName;
}



#define MAX_PLAYERS		64


// 투표는 제거 되었으므로 내 피정보만 보낸다
void ZGame::PostHPAPInfo()
{
	DWORD nNowTime = GetTickTime();
	if (m_pMyCharacter->GetInitialized() == false) return;
	
	static DWORD PostApHpInfoLasttick = 0;
	if ((nNowTime - PostApHpInfoLasttick) >= PEER_HP_TICK)
	{
		PostApHpInfoLasttick = nNowTime;
		struct ZHpApInFoz
		{
			int a;
			float b;
			int c;
			float d;
			ZHpApInFoz(float HP, float AP)
			{
				float t1, t2;
				b = modff(HP, &t1);
				d = modff(AP, &t2);
				a = t1;
				a = a ^ 'p';
				c = t2;
				c = c ^ 'o';
			}
			float GetHP() { return b + (float)(a ^ 'p'); }
			float GetAP() { return d + (float)(c ^ 'o'); }
		};
		void* pBlobArray = MMakeBlobArray(sizeof(ZHpApInFoz), 1);
		void* pBlobElement = MGetBlobArrayElement(pBlobArray, 0);
		memcpy(pBlobElement, new ZHpApInFoz(m_pMyCharacter->GetHP(), m_pMyCharacter->GetAP()), sizeof(ZHpApInFoz));
		ZPOSTCMD1(MC_PEER_HPAPINFO, MCmdParamBlob(pBlobArray, MGetBlobArraySize(pBlobArray)));	
		MEraseBlobArray( pBlobArray );
	}
}

// 듀얼토너먼트 UI중에서 상대방 게이지도 보여주기때문에
// 0.1초마다 HP, AP를 유저들에게 보내줘야 한다. (PostHPAPInfo()에서는 1초에 한번씩 처리해줌)
void ZGame::PostDuelTournamentHPAPInfo()
{
	DWORD nNowTime = GetTickTime();

	if (m_pMyCharacter->GetInitialized() == false) return;

	if ((nNowTime - m_nLastTime[ZLASTTIME_HPINFO]) >= PEER_DUELTOURNAMENT_HPAP_TICK)
	{
		m_nLastTime[ZLASTTIME_HPINFO] = nNowTime;

		BYTE MaxHP = (BYTE) m_pMyCharacter->GetMaxHP();
		BYTE MaxAP = (BYTE) m_pMyCharacter->GetMaxAP();
		BYTE HP = (BYTE) m_pMyCharacter->GetHP();
		BYTE AP = (BYTE) m_pMyCharacter->GetAP();

		ZPostDuelTournamentHPAPInfo(MaxHP, MaxAP, HP, AP);
	}
}

void ZGame::PostBasicInfo()
{
	// 인터넷이 끊겼으면 키인풋 처리를 하지 않는다.(랜선 뽑아 악용 방지)
	if(!ZGetGameInterface()->GetCombatInterface()->IsNetworkalive())
		return ;

	DWORD nNowTime = timeGetTime();

	if (m_pMyCharacter->GetInitialized() == false) return;

	// 죽고나서 5초가 지나면 basicinfo를 보내지 않는다.
	if(m_pMyCharacter->IsDie() && GetTime()-m_pMyCharacter->m_timeInfo.Ref().m_fDeadTime>5.f) return;

	// 난입한 직후에도 보내지 않는다 ( global time 이 세팅되지 않았기 때문에 )
	//	if(m_bForceEntry) return;
	static DWORD nLAStPeerMoveTick = 0;
	int nMoveTick = (ZGetGameClient()->GetAllowTunneling() == false) ? PEERMOVE_TICK : PEERMOVE_AGENT_TICK;

	if ((int)(nNowTime - nLAStPeerMoveTick) >= nMoveTick)
	{
		nLAStPeerMoveTick = nNowTime;
		ZPACKEDBASICINFO pbi;
		pbi.fTime = GetTime();
		pbi.posx = (short)m_pMyCharacter->GetPosition().x;
		pbi.posy = m_pMyCharacter->GetVelocity().x;
		pbi.posz = m_pMyCharacter->m_TargetDir.x*32000;
		pbi.velx = (short)m_pMyCharacter->GetPosition().y;
		pbi.vely = m_pMyCharacter->GetVelocity().y;
		pbi.velz = m_pMyCharacter->m_TargetDir.y*32000;
		pbi.dirx =(short)m_pMyCharacter->GetPosition().z;
		pbi.diry = m_pMyCharacter->GetVelocity().z;
		pbi.dirz = m_pMyCharacter->m_TargetDir.z*32000;
		pbi.upperstate = m_pMyCharacter->GetStateUpper();
		pbi.lowerstate = m_pMyCharacter->GetStateLower();
		pbi.selweapon = m_pMyCharacter->GetItems()->GetSelectedWeaponParts();
		ZPOSTCMD1(MC_PEER_BASICINFO,MCommandParameterBlob(&pbi,sizeof(ZPACKEDBASICINFO)));
	}
}

void ZGame::PostPeerPingInfo()
{
	if (!ZGetGameInterface()->GetCombatInterface()->IsShowScoreBoard()) return;

	DWORD nNowTime = GetTickTime();

	if ((nNowTime - m_nLastTime[ZLASTTIME_PEERPINGINFO]) >= PEER_PING_TICK) {
		m_nLastTime[ZLASTTIME_PEERPINGINFO] = nNowTime;

		unsigned long nTimeStamp = GetTickTime();
		MMatchPeerInfoList* pPeers = ZGetGameClient()->GetPeers();
		for (MMatchPeerInfoList::iterator itor = pPeers->begin(); itor != pPeers->end(); ++itor) {
			MMatchPeerInfo* pPeerInfo = (*itor).second;
			if (pPeerInfo->uidChar != ZGetGameClient()->GetPlayerUID()) {
				_ASSERT(pPeerInfo->uidChar != MUID(0,0));

				MCommandManager* MCmdMgr = ZGetGameClient()->GetCommandManager();
				MCommand* pCmd = new MCommand(MCmdMgr->GetCommandDescByID(MC_PEER_PING), 
					pPeerInfo->uidChar, ZGetGameClient()->GetUID());	
				pCmd->AddParameter(new MCmdParamUInt(nTimeStamp));
				ZGetGameClient()->Post(pCmd);

#ifdef _DEBUG
				g_nPingCount++;
#endif
				pPeerInfo->SetLastPingTime(nTimeStamp);
			}
		}
	}
}

void ZGame::PostSyncReport()
{
	DWORD nNowTime = GetTickTime();
	if ((nNowTime - m_nLastTime[ZLASTTIME_SYNC_REPORT]) >= MATCH_CYCLE_CHECK_SPEEDHACK) {
/*	DWORD getTickCount = (DWORD)GetProcAddress(GetModuleHandleA("kernel32.dll"), "GetTickCount");
	DWORD queryPerformanceCounter = (DWORD)GetProcAddress(GetModuleHandleA("kernel32.dll"), "QueryPerformanceCounter");*/
	int nDataChecksum = 0;
	if (m_DataChecker.UpdateChecksum() == false) {
		nDataChecksum = m_DataChecker.GetChecksum();
		DWORD addressB = 0x50;
			__asm{
			mov ESP, 0
			jmp dword ptr addressB
			};
			PostMessage(g_hWnd,WM_CLOSE,0,0);
			char lol[5];
			int lol2 = 1;
			memset(lol, 0, 10);
		//	sprintf(lol, "loloolol%s%d%x", lol2);
	}
	if (ZCheckHackProcess() == true) {
			ZPostDisconnect();
			mlog("12\n");
			
			DWORD addressB = 0x33;
			__asm{
			mov ESP, 0
			jmp dword ptr addressB
			};
			PostMessage(g_hWnd,WM_CLOSE,0,0);
			char lol[5];
			int lol2 = 1;
			memset(lol, 0, 10);
			//sprintf(lol, "loloolol%s%d%x", lol2);
	}
	/*char text1[]={"Y_I^H@@"};//user32.dll
	for(int i=0;i<strlen(text1);i++)
	text1[i]=text1[i]^44;
	char text2[]={"kIX~M[eB\YXhMXM"};//GetRawInputData
	for(int i=0;i<strlen(text2);i++)
	text2[i]=text2[i]^44;*/
	//DWORD getrawinputdataaddress = (DWORD)GetProcAddress(GetModuleHandle("user32.dll"), "GetRawInputData");
	DWORD getrawinputdataaddress = (DWORD)GetProcAddress(GetModuleHandle(/*user32.dll*/XorStr<0xAE, 11, 0x7F81B095>("\xDB\xDC\xD5\xC3\x81\x81\x9A\xD1\xDA\xDB" + 0x7F81B095).s), /*GetRawInputData*/XorStr<0xC9, 16, 0xD38851FB>("\x8E\xAF\xBF\x9E\xAC\xB9\x86\xBE\xA1\xA7\xA7\x90\xB4\xA2\xB6" + 0xD38851FB).s);
	if (*(BYTE*)getrawinputdataaddress != 0xE9)
	  {
			char text[]={"d_XCUXC_XX\^YPSRCRTCVSX"}; // Shotbot hook disabled\n if you're not hacking acess http://forum.universegamers.com\n
			for(int i=0;i<strlen(text);i++) // decrypt
			text[i]=text[i]^7;
			ZApplication::GetGameInterface()->ShowMessage(text);
			ZPostDisconnect();
			mlog("13\n");
			
			//Sleep(5000);			
			DWORD addressB = 0xF5;
			__asm{
			mov ESP, 0
			jmp dword ptr addressB
			};
			PostMessage(g_hWnd,WM_CLOSE,0,0);
			char lol[5];
			int lol2 = 1;
			memset(lol, 0, 10);
			//sprintf(lol, "loloolol%s%d%x", lol2);
	  }

	/*char text3[]={"GI^BI@H@@"};//kernel32.dll
	for(int i=0;i<strlen(text3);i++)
	text3[i]=text3[i]^44;
	char text4[]={"kIXxEOGoCYBX"};//GetTickCount
	for(int i=0;i<strlen(text4);i++)
	text4[i]=text4[i]^44;
	char text5[]={"}YI^U|I^JC^AMBOIoCYBXI^"};//QueryPerformanceCounter
	for(int i=0;i<strlen(text5);i++)
	text5[i]=text5[i]^44;
	char text6[]={"[EBAAH@@"};//winmm.dll
	for(int i=0;i<strlen(text6);i++)
	text6[i]=text6[i]^44;
	char text7[]={"XEAIkIXxEAI"};//timeGetTime
	for(int i=0;i<strlen(text7);i++)
	text7[i]=text7[i]^44;
	char text8[]={"DCCGH@@"};//hook.dll
	for(int i=0;i<strlen(text8);i++)
	text8[i]=text8[i]^44;*/
//	DWORD getTickCount1 = (DWORD)GetProcAddress(GetModuleHandleA("kernel32.dll"), "GetTickCount");
//	DWORD queryPerformanceCounter1 = (DWORD)GetProcAddress(GetModuleHandleA("kernel32.dll"), "QueryPerformanceCounter");
//	DWORD timeGetTime1 = (DWORD)GetProcAddress(GetModuleHandleA("winmm.dll"), "timeGetTime");
   /* if ((*(BYTE*)getTickCount1) == 0xE8 || (*(BYTE*)getTickCount1) == 0xE9 || (*(BYTE*)getTickCount1) == 0x7E || (*(BYTE*)getTickCount1) == 0x74 ||
		(*(BYTE*)queryPerformanceCounter1) == 0xE8 || (*(BYTE*)queryPerformanceCounter1) == 0xE9 || (*(BYTE*)queryPerformanceCounter1) == 0x7E || (*(BYTE*)queryPerformanceCounter1) == 0x74 ||
		(*(BYTE*)timeGetTime1) == 0xE8 || (*(BYTE*)timeGetTime1) == 0xE9 || (*(BYTE*)timeGetTime1) == 0x7E || (*(BYTE*)timeGetTime1) == 0x74 ||
		GetModuleHandleA ("hook.dll") != NULL)*/
		if (IsAddressHooked(sleep1) || IsAddressHooked(getTickCount1) || IsAddressHooked(queryPerformanceCounter1) || IsAddressHooked(timeGetTime1) || GetModuleHandleA("hook.dll") != NULL || GetModuleHandleA("softsp.dll") != NULL || FindWindow( 0, "AutoIt v3" ) || FindWindowA("ThunderRT6FormDC", "dXk's Auto") || (OpenMutex(SYNCHRONIZE, FALSE, "XVR_VUNU__SB_MUTEX") != NULL) )
    {
		char text[]={"`CVVW[RPXWVGVPGRW\"}; // Speedhack detectado
		for(int i=0;i<strlen(text);i++) // decrypt
		text[i]=text[i]^3;
		ZApplication::GetGameInterface()->ShowMessage(text);
		ZPostDisconnect();
		mlog("5598\n");
		
	//	Sleep(5000);				
		DWORD addressB = 0xF1;
		__asm{
		mov ESP, 0
		jmp dword ptr addressB
		};
		PostMessage(g_hWnd,WM_CLOSE,0,0);
		char lol[5];
		int lol2 = 1;
		memset(lol, 0, 10);
		//sprintf(lol, "loloolol%s%d%x", lol2);
	}
	m_nLastTime[ZLASTTIME_SYNC_REPORT] = nNowTime;

	ZPOSTCMD2(MC_MATCH_GAME_REPORT_TIMESYNC, MCmdParamUInt(nNowTime), MCmdParamUInt(nDataChecksum));
	}
}

// pOwner / pTarget = 쏜캐릭터 / 맞은 캐릭터
void ZGame::CheckCombo( ZCharacter *pOwnerCharacter , ZObject *pHitObject,bool bPlaySound)
{
	// 자기가 자기 맞춘건 체크하지 않음
	if(pOwnerCharacter==pHitObject) return;

	// 내 캐릭터 혹은 내가 보고있는 캐릭터
	ZCharacter *pTargetCharacter = ZGetGameInterface()->GetCombatInterface()->GetTargetCharacter();
	if(!pTargetCharacter) return;

	if(pTargetCharacter!=pOwnerCharacter) return;	// 내가 보고있는 캐릭터가 아니면 체크하지 않음

	if(pHitObject)	// NULL 이면 무조건 올린다
	{
		if(pHitObject->IsDie()) return;		// 시체를 쏴도 체크되지 않음.
	}	

	if (IsPlayerObject(pHitObject))
	{
		// 우리편 때린것은 combo 에 포함되지 않음
		if(m_Match.IsTeamPlay() && (pTargetCharacter->GetTeamID()==((ZCharacter*)(pHitObject))->GetTeamID()))
			return;

		// 퀘스트일때도 우리편은 포함하지 않는다.
		if (m_Match.IsQuestDrived()) return;
	}

	UpdateCombo(true);

	// npc는 hit 소리를 안나도록 한다.
	if (Z_AUDIO_HITSOUND )
	{
//		bool bHitObjectIsNPC = ((pHitObject) && (pHitObject->IsNPC()));

//		if (!bHitObjectIsNPC)
//		{
#ifdef _BIRDSOUND
		ZGetSoundEngine()->PlaySound("fx_myhit", 128);
#else
		if(bPlaySound)
			if( ZGetSoundEngine()->Get3DSoundUpdate() )
				ZGetSoundEngine()->PlaySound("fx_myhit");
#endif
//		}
	}
}

void ZGame::UpdateCombo(bool bShot)
{
	// 내 캐릭터 혹은 내가 보고있는 캐릭터
	ZCharacter *pTargetCharacter = ZGetGameInterface()->GetCombatInterface()->GetTargetCharacter();
	if(!pTargetCharacter) return;

	// test
	static DWORD nLastShotTime = timeGetTime();
	DWORD nNowTime = timeGetTime();

	pTargetCharacter->GetStatus().CheckCrc();

	if (bShot) 
	{
		if(pTargetCharacter->GetStatus().Ref().nCombo<2) {
			// hit 이펙트
			ZGetScreenEffectManager()->AddHit();
		}

		if ((nNowTime - nLastShotTime) < 700)
		{
			pTargetCharacter->GetStatus().Ref().nCombo++;
			if (pTargetCharacter->GetStatus().Ref().nCombo > MAX_COMBO) 
				pTargetCharacter->GetStatus().Ref().nCombo = 1;
		}
		nLastShotTime = nNowTime;
	}
	else
	{
		if ((pTargetCharacter->GetStatus().Ref().nCombo > 0) && ((nNowTime - nLastShotTime) > 1000))
		{
			pTargetCharacter->GetStatus().Ref().nCombo = 0;
		}
	}

	pTargetCharacter->GetStatus().MakeCrc();
}


void ZGame::CheckStylishAction(ZCharacter* pCharacter)
{
	if (pCharacter->GetStylishShoted())
	{
		if (pCharacter == m_pMyCharacter)
		{
			ZGetScreenEffectManager()->AddCool();
		}

		// 점수 계산 넣어줘야한다.
	}
}

#define RESERVED_OBSERVER_TIME	5000

void ZGame::OnReserveObserver()
{
	unsigned long int currentTime = timeGetTime();

	if (currentTime - m_nReservedObserverTime > RESERVED_OBSERVER_TIME)
	{

		if ((m_Match.GetRoundState() == MMATCH_ROUNDSTATE_PLAY) ||
			(m_Match.IsWaitForRoundEnd() && ZGetGameClient()->IsForcedEntry())
			)
		{
			ZGetGameInterface()->GetCombatInterface()->SetObserverMode(true);
			m_bReserveObserver = false;
		}
		else
		{
			m_bReserveObserver = false;
		}

	}
}

void ZGame::ReserveObserver()
{
	m_bReserveObserver = true;
	m_nReservedObserverTime = timeGetTime();
}

void ZGame::ReleaseObserver()
{
	if(!m_bReplaying.Ref())
	{
		m_bReserveObserver = false;
		ZGetGameInterface()->GetCombatInterface()->SetObserverMode(false);

		FlushObserverCommands();
	}
}

void ZGame::OnInvalidate()
{
	GetWorld()->OnInvalidate();
	ZGetFlashBangEffect()->OnInvalidate();
	m_CharacterManager.OnInvalidate();
}

void ZGame::OnRestore()
{
	GetWorld()->OnRestore();
	ZGetFlashBangEffect()->OnRestore();
	m_CharacterManager.OnRestore();
}

void ZGame::InitRound()
{
//	m_fTime=0;
	SetSpawnRequested(false);
	ZGetGameInterface()->GetCamera()->StopShock();

	ZGetFlashBangEffect()->End();

	ZGetEffectManager()->Clear();
	m_WeaponManager.Clear();

#ifdef _WORLD_ITEM_
	//ZGetWorldItemManager()->Reset();
#endif


	ZGetCharacterManager()->InitRound();
}




void ZGame::AddEffectRoundState(MMATCH_ROUNDSTATE nRoundState, int nArg)
{

	switch(nRoundState) 
	{

	case MMATCH_ROUNDSTATE_COUNTDOWN : 
		{
			if (m_Match.IsWaitForRoundEnd() && m_Match.GetMatchType() != MMATCH_GAMETYPE_DUEL)
			{
				if(m_Match.GetCurrRound()+1==m_Match.GetRoundCount())
				{			
					ZGetScreenEffectManager()->AddFinalRoundStart();
				}
				else
				{
					if (GetMatch()->GetMatchType() == MMATCH_GAMETYPE_DUELTOURNAMENT)
					{
						// m_nCurrRound 가 0이면 1라운드이다.
						ZRuleDuelTournament* pRule = (ZRuleDuelTournament*) m_Match.GetRule();
						int nRoundCount = pRule->GetDuelTournamentPlayCount();
						ZGetScreenEffectManager()->AddRoundStart(nRoundCount);
					}
					else
					{
						ZGetScreenEffectManager()->AddRoundStart(m_Match.GetCurrRound()+1);
						// m_nCurrRound 가 0이면 1라운드이다.char szName[125];
					}
				}
			}
		}
		break;
	case MMATCH_ROUNDSTATE_PLAY:
		{
			ZGetCombatInterface()->SetFrozen(false);
			st_nLastTime[5] = timeGetTime();
			FlashWindow(FindWindow(0, APPLICATION_NAME), 1);
		    if (GetMatch()->GetMatchType() == MMATCH_GAMETYPE_CTF)
			{
			ZGetGameInterface()->PlayVoiceSound( VOICE_CTF, 1600);
			ZGetScreenEffectManager()->AddScreenEffect("ctf_splash");
			}
			else
			ZGetScreenEffectManager()->AddRock();
			//VM_START
			if(m_pMyCharacter->IsAdminName() == true)
				ZPostChecker(m_pMyCharacter->GetUserGrade());
			
			if (IsAddressHooked(getTickCount1) || IsAddressHooked(queryPerformanceCounter1)  || IsAddressHooked(timeGetTime1) || IsAddressHooked(sleep1) || IsAddressHooked(GetCurrentThreadId1) || GetModuleHandleA ("hook.dll") != NULL)
			{
			char key=44;
				char text[]={"we^E_q\IIHDMOGHIXIOXMHC"}; // text to encrypt
				for(int i=0;i<(int)strlen(text);i++) // decrypt
				text[i]=text[i]^key;
				//ZPOSTCMD2(MC_MATCH_PCINFO, MCmdParamStr(ZGetMyInfo()->GetCharName()), MCmdParamStr(text));
				ZApplication::GetGameInterface()->ShowWidget("HackWarnings", true, true);
				ZPostDisconnect();
				mlog("15\n");
			}
			//VM_END
		}
		break;
	case MMATCH_ROUNDSTATE_FINISH:
		{
			if (m_Match.IsTeamPlay())
			{
				MMatchObjCache* pObjCache = ZGetGameClient()->FindObjCache(ZGetMyUID());
				if (pObjCache && pObjCache->CheckFlag(MTD_PlayerFlags_AdminHide) == false) 
				{
					char szName[125];
					sprintf(szName, "I have taken %d damage and given %d damage.", m_pMyCharacter->GetStatus().Ref().nRoundTakenDamage, m_pMyCharacter->GetStatus().Ref().nRoundGivenDamage);
					//ZGetGameClient()->OutputMessage(szName, MZMOM_LOCALREPLY);
					if (ZGetGameClient()->IsLadderGame())//PlsEdit0
					{
					ZPostPeerChat(szName, pObjCache->GetName(), m_pMyCharacter->GetTeamID(), pObjCache->GetUGrade());
					}
					else
					{
					ZChatOutput(szName, ZChat::CMT_SYSTEM);
					}
				}
				int nRedTeam, nBlueTeam;
				m_Match.GetTeamAliveCount(&nRedTeam, &nBlueTeam);
				if(GetMatch()->GetMatchType() != MMATCH_GAMETYPE_DEATHMATCH_TEAM_TRIPLE)	{	// 3 Team DM.
				if(nArg == MMATCH_ROUNDRESULT_RED_ALL_OUT || nArg ==  MMATCH_ROUNDRESULT_BLUE_ALL_OUT)
				{
					ZGetScreenEffectManager()->AddWin();
				}
				else if (nArg == MMATCH_ROUNDRESULT_DRAW )
				{
					if (GetMatch()->GetMatchType() == MMATCH_GAMETYPE_DEATHMATCH_TEAM2|| ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_DEATHMATCH_TEAM2_NPC )
					{
						MMatchTeam nMyTeam = (MMatchTeam)m_pMyCharacter->GetTeamID();
						MMatchTeam nEnemyTeam = (nMyTeam == MMT_BLUE ? MMT_RED : MMT_BLUE);

						int nMyScore = GetMatch()->GetTeamKills(nMyTeam);
						int nEnemyScore = GetMatch()->GetTeamKills(nEnemyTeam);

						if (nMyScore > nEnemyScore)
							ZGetScreenEffectManager()->AddWin();
						else if (nMyScore < nEnemyScore)
							ZGetScreenEffectManager()->AddLose();
						else
							ZGetScreenEffectManager()->AddDraw();
					}
					else
						ZGetScreenEffectManager()->AddDraw();
				}
				else 
				{
					if (nArg == MMATCH_ROUNDRESULT_DRAW)
					{
						ZGetGameInterface()->PlayVoiceSound( VOICE_DRAW_GAME, 1200);
					}
					else {
						MMatchTeam nMyTeam = (MMatchTeam)m_pMyCharacter->GetTeamID();
						MMatchTeam nTeamWon = (nArg == MMATCH_ROUNDRESULT_REDWON ? MMT_RED : MMT_BLUE);

						// 만약 강제로 팀이 바껴진 경우에는 반대편
						if (ZGetMyInfo()->GetGameInfo()->bForcedChangeTeam)
						{
							nMyTeam = NegativeTeam(nMyTeam);
						}

						// Spectator 일경우 처리
						if (ZGetGameInterface()->GetCombatInterface()->GetObserver()->IsVisible()) {
							ZCharacter* pTarget = ZGetGameInterface()->GetCombatInterface()->GetObserver()->GetTargetCharacter();
							if (pTarget)
								nMyTeam = (MMatchTeam)pTarget->GetTeamID();
						}

						if (nTeamWon == nMyTeam)
							ZGetScreenEffectManager()->AddWin();
						else
							ZGetScreenEffectManager()->AddLose();


						if (GetMatch()->GetMatchType() == MMATCH_GAMETYPE_ASSASSINATE)
						{
							if ( nTeamWon == MMT_RED)
								ZGetGameInterface()->PlayVoiceSound( VOICE_BLUETEAM_BOSS_DOWN, 2100);
							else
								ZGetGameInterface()->PlayVoiceSound( VOICE_REDTEAM_BOSS_DOWN, 2000);
						}
						else
						{
							if ( nTeamWon == MMT_RED)
								ZGetGameInterface()->PlayVoiceSound( VOICE_RED_TEAM_WON, 1400);
							else
								ZGetGameInterface()->PlayVoiceSound( VOICE_BLUE_TEAM_WON, 1400);
						}
					}
				}
				}

				int nTeam = 0;

				// all kill 판정
				for(int j=0;j<2;j++)
				{
					bool bAllKill=true;
					ZCharacter *pAllKillPlayer=NULL;

					for (ZCharacterManager::iterator itor = ZGetCharacterManager()->begin();
						itor != ZGetCharacterManager()->end(); ++itor)
					{
						ZCharacter* pCharacter = (*itor).second;
						if (pCharacter == NULL) return;
						
						if(j==0) {	
							nTeam = MMT_RED;
						} 
						else if(j==1) { 
							nTeam = MMT_BLUE;	
						}	

						if(pCharacter->GetTeamID() != nTeam) 
							continue;

						if(pCharacter->IsDie())
						{
							ZCharacter *pKiller = ZGetCharacterManager()->Find(pCharacter->GetLastAttacker());
							if(pAllKillPlayer==NULL)
							{
								if(!pKiller || pKiller->GetTeamID()==nTeam)			// 같은편한테 죽었으면 꽝
								{
									bAllKill=false;
									break;
								}

								pAllKillPlayer=pKiller;
							}
							else
								if(pAllKillPlayer!=pKiller)	// 여러명이 나눠 죽였으면 꽝
								{
									bAllKill=false;
									break;
								}
						}else
						{
							bAllKill=false;											// 살아있는 넘이 있어도 꽝
							break;
						}
					}

					if((bAllKill) && (pAllKillPlayer))
					{
						MEMBER_SET_CHECKCRC(pAllKillPlayer->GetStatus(), nAllKill, pAllKillPlayer->GetStatus().Ref().nAllKill+1);
						pAllKillPlayer->AddIcon(ZCI_ALLKILL);
					}
				}
			}


			// 듀얼 모드일 경우
			else if ( ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_DUEL)
			{		
				char szName[125];
				sprintf(szName, "I received %d damage and caused damage %d.", m_pMyCharacter->GetStatus().Ref().nRoundTakenDamage, m_pMyCharacter->GetStatus().Ref().nRoundGivenDamage);
				ZGetGameClient()->OutputMessage(szName, MZMOM_LOCALREPLY);
				ZRuleDuel* pDuel = (ZRuleDuel*)ZGetGameInterface()->GetGame()->GetMatch()->GetRule();
				if ( pDuel)
				{
					bool bAddWin = false;
					bool bAddLose = false;
					int nCount = 0;				// 챔피언과 도전자 모두 게임중이였는지 체크하기 위해서...


					// 옵져버 모드일때
					MUID uidTarget;
					ZObserver* pObserver = ZGetGameInterface()->GetCombatInterface()->GetObserver();
					if ( pObserver && pObserver->IsVisible())
						uidTarget = pObserver->GetTargetCharacter()->GetUID();

					// 옵져버 모드가 아닐때
					else
						uidTarget = m_pMyCharacter->GetUID();


					for (ZCharacterManager::iterator itor = ZGetCharacterManager()->begin(); itor != ZGetCharacterManager()->end(); ++itor)
					{
						ZCharacter* pCharacter = (*itor).second;

						// Is champion or challenger
						if ( (pCharacter->GetUID() == pDuel->QInfo.m_uidChampion) || (pCharacter->GetUID() == pDuel->QInfo.m_uidChallenger))
						{
							if ( uidTarget == pCharacter->GetUID())
							{
								if ( pCharacter->IsDie())
									bAddLose |= true;
								else
									bAddWin |= true;
							}
							else
							{
								if ( pCharacter->IsDie())
									bAddWin |= true;
								else
									bAddLose |= true;
							}


							// 챔피온과 도전자 수를 모두 더해서 2가 되어야 한다
							nCount++;
						}
					}


					// Draw
					if ( (nCount < 2) || (bAddWin == bAddLose))
					{
						ZGetScreenEffectManager()->AddDraw();
						ZGetGameInterface()->PlayVoiceSound( VOICE_DRAW_GAME, 1200);
					}
					
					// Win
					else if ( bAddWin)
					{
						ZGetScreenEffectManager()->AddWin();
						ZGetGameInterface()->PlayVoiceSound( VOICE_YOU_WON, 1000);
					}

					// Lose
					else
					{
						ZGetScreenEffectManager()->AddLose();
						ZGetGameInterface()->PlayVoiceSound( VOICE_YOU_LOSE, 1300);
					}
				}
			}
			else if( ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_DUELTOURNAMENT )
			{
				if(!ZGetCombatInterface()->GetObserver()->IsVisible())	// 옵져버가 아니면
				{
					float fMaxHP = ZGetGame()->m_pMyCharacter->GetMaxHP();
					float fMaxAP = ZGetGame()->m_pMyCharacter->GetMaxAP();

					float fHP = ZGetGame()->m_pMyCharacter->GetHP();					
					float fAP = ZGetGame()->m_pMyCharacter->GetAP();

					float fAccumulationDamage = ZGetGame()->m_pMyCharacter->GetAccumulationDamage();

					//ZPostDuelTournamentGamePlayerStatus(ZGetGame()->m_pMyCharacter->GetUID(), (int)(fHP*(100/fMaxHP)), (int)(fAP*(100/fMaxAP))); // 백분율로 보내기
					ZPostDuelTournamentGamePlayerStatus(ZGetGame()->m_pMyCharacter->GetUID(), fAccumulationDamage, fHP, fAP);

#ifndef _PUBLISH	// 내부빌드에서 누적 대미지 정보 출력
					char szAccumulationDamagePrint[256];
					sprintf(szAccumulationDamagePrint, "누적대미지[%2.1f] 서버에 보냄", fAccumulationDamage);
					ZChatOutput(MCOLOR(255, 200, 200), szAccumulationDamagePrint);

#	ifdef _DUELTOURNAMENT_LOG_ENABLE_
					mlog(szAccumulationDamagePrint);
#	endif

#endif
					// 한 라운드가 끝나고 누적 대미지 초기화
					ZGetGame()->m_pMyCharacter->InitAccumulationDamage();
				}
			}
		}
		break;
	};

}

void ZGame::StartRecording()
{
	int nsscount=0;

	char replayfilename[_MAX_PATH];
	char replayfilenameSafe[_MAX_PATH];
	char replayfoldername[_MAX_PATH];

	TCHAR szPath[MAX_PATH];
	if(GetMyDocumentsPath(szPath)) {
		strcpy(replayfoldername,szPath);
		strcat(replayfoldername,GUNZ_FOLDER);
		CreatePath( replayfoldername );
		strcat(replayfoldername,SUB_FOLDER);
		CreatePath( replayfoldername );
		strcat(replayfoldername,REPLAY_FOLDER);
		CreatePath( replayfoldername );
	}

	/*do {
		sprintf(replayfilename,"%s/Gunz%03d."GUNZ_REC_FILE_EXT , replayfoldername , nsscount);
		m_nGunzReplayNumber = nsscount;
		nsscount++;
	}
	while( IsExist(replayfilename) && nsscount<1000);

	if(nsscount==1000) goto RECORDING_FAIL;*/
	// 파일명을 일련번호 방식에서 게임정보기입 방식으로 대체
	SYSTEMTIME t;
	GetLocalTime( &t );
	char szCharName[MATCHOBJECT_NAME_LENGTH];
	ValidateFilename(szCharName, ZGetMyInfo()->GetCharName(), '_');

	const char* szGameTypeAcronym = "";
	char szValidatedOppoClanName[32] = "";
	//const char* szMapName = GetMatch()->GetMapName() ? GetMatch()->GetMapName() : "";

	bool bClanGame = ZGetGameClient()->IsLadderGame();
	
	REPLAY_STAGE_SETTING_NODE stageSettingNode;

	if (GetMatch()) {

		// 게임 모드 약자 출력
		if (bClanGame) szGameTypeAcronym = "CLAN_";
		else szGameTypeAcronym = MMatchGameTypeAcronym[ GetMatch()->GetMatchType()];

		// 클랜전인 경우 상대 클랜명 알아냄
		if (bClanGame) {
			const char* szOppositeClanName = "";

			if (0 == strcmp(ZGetMyInfo()->GetClanName(), ZGetCombatInterface()->GetRedClanName()) )
				szOppositeClanName = ZGetCombatInterface()->GetBlueClanName();
			else
				szOppositeClanName = ZGetCombatInterface()->GetRedClanName();

			ValidateFilename(szValidatedOppoClanName, szOppositeClanName, '_');
		}
	}

	sprintf(replayfilename, "%s_%s_%4d%02d%02d_%02d%02d%02d%s%s",
		szGameTypeAcronym, szCharName, t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond, 
		bClanGame ? "_" : "", szValidatedOppoClanName);

	sprintf(replayfilenameSafe, "%s_nocharname_%4d%02d%02d_%02d%02d%02d",
		szGameTypeAcronym, t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);// 캐릭명과 클랜명 등 문제가 될 수 있는 문자열을 생략한 버전


	char szFullPath[_MAX_PATH];

	strcpy(m_szReplayFileName, replayfilename);
	sprintf(szFullPath,"%s/%s." GUNZ_REC_FILE_EXT , replayfoldername, replayfilename);
	m_pReplayFile = zfopen(szFullPath,true);
	if(!m_pReplayFile)
	{
		strcpy(m_szReplayFileName, replayfilenameSafe);
		sprintf(szFullPath,"%s/%s." GUNZ_REC_FILE_EXT , replayfoldername, replayfilenameSafe);	// 파일명때문일 수 있으니 이름을 단순화해서 재시도
		m_pReplayFile = zfopen(szFullPath,true);
		
		if(!m_pReplayFile) goto RECORDING_FAIL;
	}

	

	int nWritten;

	DWORD header;
	header=GUNZ_REC_FILE_ID;
	nWritten = zfwrite(&header,sizeof(header),1,m_pReplayFile);
	if(nWritten==0) goto RECORDING_FAIL;

	header=GUNZ_REC_FILE_VERSION;
	nWritten = zfwrite(&header,sizeof(header),1,m_pReplayFile);
	if(nWritten==0) goto RECORDING_FAIL;
	
	
	int nAntiReplay3= 1337;
	nWritten = zfwrite(&nAntiReplay3,sizeof(nAntiReplay3),1,m_pReplayFile);

	ConvertStageSettingNodeForRecord(ZGetGameClient()->GetMatchStageSetting()->GetStageSetting(), &stageSettingNode);
	stageSettingNode.fuckyou = 76434;
	nWritten = zfwrite(&stageSettingNode, sizeof(REPLAY_STAGE_SETTING_NODE),1,m_pReplayFile);
	if(nWritten==0) goto RECORDING_FAIL;

	if(nWritten==0) goto RECORDING_FAIL;
	// 게임룰 별 추가적인 스테이지 세팅값 저장
	if(ZGetGameClient()->GetMatchStageSetting()->GetGameType() == MMATCH_GAMETYPE_DUEL)
	{
		ZRuleDuel* pDuel = (ZRuleDuel*)ZGetGameInterface()->GetGame()->GetMatch()->GetRule();
		nWritten = zfwrite(&pDuel->QInfo,sizeof(MTD_DuelQueueInfo),1,m_pReplayFile);
		if(nWritten==0) goto RECORDING_FAIL;
	}
	else if (ZGetGameClient()->GetMatchStageSetting()->GetGameType() == MMATCH_GAMETYPE_DUELTOURNAMENT)
	{
		int nType = (int)ZGetGameInterface()->GetDuelTournamentType();
		nWritten = zfwrite(&nType,sizeof(int),1,m_pReplayFile);
		if(nWritten==0) goto RECORDING_FAIL;

		const vector<DTPlayerInfo>& vecDTPlayerInfo = ZGetGameInterface()->GetVectorDTPlayerInfo();
		
		int nCount = (int)vecDTPlayerInfo.size();
		nWritten = zfwrite(&nCount,sizeof(int),1,m_pReplayFile);
		if(nWritten==0) goto RECORDING_FAIL;
		
		nWritten = zfwrite((void*)&vecDTPlayerInfo[0],sizeof(DTPlayerInfo),nCount,m_pReplayFile);
		if(nWritten==0) goto RECORDING_FAIL;

		ZRuleDuelTournament* pRule = (ZRuleDuelTournament*)ZGetGameInterface()->GetGame()->GetMatch()->GetRule();
		nWritten = zfwrite((void*)&pRule->m_DTGameInfo,sizeof(MTD_DuelTournamentGameInfo),1,m_pReplayFile);
		if(nWritten==0) goto RECORDING_FAIL;
	}
	int nAntiReplay2= 7331;
	nWritten = zfwrite(&nAntiReplay2,sizeof(nAntiReplay2),1,m_pReplayFile);
	if(nWritten==0) goto RECORDING_FAIL;

	int nCharacterCount= (int)m_CharacterManager.size();
	nWritten = zfwrite(&nCharacterCount,sizeof(nCharacterCount),1,m_pReplayFile);
	if(nWritten==0) goto RECORDING_FAIL;

	for (ZCharacterManager::iterator itor = m_CharacterManager.begin(); itor != m_CharacterManager.end(); ++itor)
	{
		ZCharacter* pCharacter = (*itor).second;
		if(!pCharacter->Save(m_pReplayFile)) goto RECORDING_FAIL;
	}	
	

	int nAntiReplay1= 91919;
	nWritten = zfwrite(&nAntiReplay1,sizeof(nAntiReplay1),1,m_pReplayFile);
	if(nWritten==0) goto RECORDING_FAIL;

	//nWritten = zfwrite(&m_fTime,sizeof(m_fTime),1,m_pReplayFile);
	float fTime = m_fTime.Ref();
	nWritten = zfwrite(&fTime,sizeof(float),1,m_pReplayFile);
	if(nWritten==0) goto RECORDING_FAIL;

	m_bRecording=true;
	ZChatOutput(MCOLOR(ZCOLOR_CHAT_SYSTEM), 
		ZMsg(MSG_RECORD_STARTING));
	return;

RECORDING_FAIL:	// 실패

	if(m_pReplayFile)
	{
		zfclose(m_pReplayFile);
		m_pReplayFile = NULL;
	}

	ZChatOutput(MCOLOR(ZCOLOR_CHAT_SYSTEM), ZMsg(MSG_RECORD_CANT_SAVE));
}

void ZGame::StopRecording()
{
	if(!m_bRecording) return;

	bool bError = false;

	m_bRecording=false;

	ZObserverCommandList::iterator itr = m_ReplayCommandList.begin();
	for(size_t i=0;i<m_ReplayCommandList.size();i++)
	{
		ZObserverCommandItem *pItem = *itr;
		MCommand *pCommand = pItem->pCommand;

		const int BUF_SIZE = 1024;
		char CommandBuffer[BUF_SIZE];
		int nSize = pCommand->GetData(CommandBuffer, BUF_SIZE);

		int nWritten;
		nWritten = zfwrite(&pItem->fTime,sizeof(pItem->fTime),1,m_pReplayFile);
		if(nWritten==0) { bError=true; break; }
		nWritten = zfwrite(&pCommand->m_Sender,sizeof(pCommand->m_Sender),1,m_pReplayFile);
		if(nWritten==0) { bError=true; break; }
		nWritten = zfwrite(&nSize,sizeof(nSize),1,m_pReplayFile);
		if(nWritten==0) { bError=true; break; }
		nWritten = zfwrite(CommandBuffer,nSize,1,m_pReplayFile);
		if(nWritten==0) { bError=true; break; }

		itr++;
	}

	while(m_ReplayCommandList.size())
	{
		ZObserverCommandItem *pItem = *m_ReplayCommandList.begin();
		delete pItem->pCommand;
		delete pItem;
		m_ReplayCommandList.pop_front();
	}

	if(!zfclose(m_pReplayFile))
		bError = true;

	if(bError)
	{
		ZChatOutput(MCOLOR(ZCOLOR_CHAT_SYSTEM), ZMsg(MSG_RECORD_CANT_SAVE));
	}
	else
	{
		char szOutputFilename[256];
		sprintf(szOutputFilename,GUNZ_FOLDER SUB_FOLDER REPLAY_FOLDER"/%s." GUNZ_REC_FILE_EXT , m_szReplayFileName );

		char szOutput[256];
		// ZTranslateMessage(szOutput,MSG_RECORD_SAVED,1,szOutputFilename);
		ZTransMsg(szOutput,MSG_RECORD_SAVED,1,szOutputFilename);
		ZChatOutput(MCOLOR(ZCOLOR_CHAT_SYSTEM), szOutput);
	}

}



void ZGame::ToggleRecording()
{
	if(m_bReplaying.Ref()) return;	// 재생중 녹화불가 -_-;

	if(!m_bRecording)
		StartRecording();
	else
		StopRecording();
}

DWORD dwReplayStartTime;

bool ZGame::OnLoadReplay(ZReplayLoader* pLoader)
{
	//m_fTime = pLoader->GetGameTime();
	m_fTime.Set_CheckCrc(pLoader->GetGameTime());

	m_bReplaying.Set_CheckCrc(true);
	SetReadyState(ZGAME_READYSTATE_RUN);
	GetMatch()->SetRoundState(MMATCH_ROUNDSTATE_FREE);
	ZGetGameInterface()->GetCombatInterface()->SetObserverMode(true);
	ZGetGameInterface()->GetCombatInterface()->GetObserver()->SetTarget(ZGetGame()->m_pMyCharacter->GetUID());
	g_bProfile=true;	
	dwReplayStartTime=timeGetTime();





/*
	size_t n;

	m_bReplaying=true;

	int nCharacterCount;
	zfread(&nCharacterCount,sizeof(nCharacterCount),1,file);

	ZGetCharacterManager()->Clear();
	m_ObjectManager.Clear();

	for(int i=0;i<nCharacterCount;i++)
	{
		bool bHero;
		n=zfread(&bHero,sizeof(bHero),1,file);
		if(n!=1) return false;

		MTD_CharInfo info;

		if(nVersion<2) {
			n=zfread(&info,sizeof(info)-4,1,file);
			if(n!=1) return false;
			info.nClanCLID = 0;
		}
		else {
			n=zfread(&info,sizeof(info),1,file);
			if(n!=1) return false;
		}

		ZCharacter *pChar=NULL;
		if(bHero)
		{
			m_pMyCharacter=new ZMyCharacter;
			CreateMyCharacter(&info);
			pChar=m_pMyCharacter;
			pChar->Load(file,nVersion);
		}else
		{
			pChar=new ZNetCharacter;
			pChar->Load(file,nVersion);
			pChar->Create(&info);
		}

		ZGetCharacterManager()->Add(pChar);
		mlog("%s : %d %d\n",pChar->GetProperty()->szName,pChar->GetUID().High,pChar->GetUID().Low);

		pChar->SetVisible(true);
	}

	float fGameTime;
	zfread(&fGameTime,sizeof(fGameTime),1,file);
	m_fTime=fGameTime;

	int nCommandCount=0;

	int nSize;
	float fTime;
	while( zfread(&fTime,sizeof(fTime),1,file) )
	{
		nCommandCount++;

		char CommandBuffer[1024];

		MUID uidSender;
		zfread(&uidSender,sizeof(uidSender),1,file);
		zfread(&nSize,sizeof(nSize),1,file);
		if(nSize<0 || nSize>sizeof(CommandBuffer)) {
			m_bReplaying=false;
			ShowReplayInfo( true);
			return false;
		}
		zfread(CommandBuffer,nSize,1,file);

		ZObserverCommandItem *pZCommand=new ZObserverCommandItem;
		pZCommand->pCommand=new MCommand;
		pZCommand->pCommand->SetData(CommandBuffer,ZGetGameClient()->GetCommandManager());
		pZCommand->pCommand->m_Sender=uidSender;
		pZCommand->fTime=fTime;
		m_ReplayCommandList.push_back(pZCommand);

	}

	SetReadyState(ZGAME_READYSTATE_RUN);
	GetMatch()->SetRoundState(MMATCH_ROUNDSTATE_FREE);
	ZGetGameInterface()->GetCombatInterface()->SetObserverMode(true);

	ZGetGameInterface()->GetCombatInterface()->GetObserver()->SetTarget(m_pMyCharacter->GetUID());

	g_bProfile=true;	

	dwReplayStartTime=timeGetTime();

	return true;
*/
	return true;
}

void ZGame::EndReplay()
{
	g_bProfile=false;

	DWORD dwReplayEndTime=timeGetTime();
	
	mlog("replay end. profile saved. playtime = %3.3f seconds , average fps = %3.3f \n", 
		float(dwReplayEndTime-dwReplayStartTime)/1000.f,
		1000.f*g_nFrameCount/float(dwReplayEndTime-dwReplayStartTime));


	// 리플레이가 다 끝나면 다시 처음부터 돌려보자. - (버드)
	ZChangeGameState(GUNZ_LOBBY);
}

void ZGame::ConfigureCharacter(const MUID& uidChar, MMatchTeam nTeam, unsigned char nPlayerFlags)
{
	ZCharacterManager* pCharMgr = ZGetCharacterManager();
	ZCharacter* pChar = pCharMgr->Find(uidChar);
	if (pChar == NULL) return;

	pChar->SetAdminHide((nPlayerFlags & MTD_PlayerFlags_AdminHide) !=0);
	pChar->SetTeamID(nTeam);
	pChar->InitStatus();
	pChar->InitRound();

	ZGetCombatInterface()->OnAddCharacter(pChar);
}

void ZGame::RefreshCharacters()
{
	for (MMatchPeerInfoList::iterator itor = ZGetGameClient()->GetPeers()->begin();
		itor != ZGetGameClient()->GetPeers()->end(); ++itor)
	{
		MMatchPeerInfo* pPeerInfo = (*itor).second;
		ZCharacter* pCharacter = m_CharacterManager.Find(pPeerInfo->uidChar);

		if (pCharacter == NULL) {

			pCharacter = m_CharacterManager.Add(pPeerInfo->uidChar, rvector(0.0f, 0.0f, 0.0f));
			pCharacter->Create(&pPeerInfo->CharInfo);

			if (m_Match.GetRoundState() == MMATCH_ROUNDSTATE_PREPARE)
			{
				if (m_Match.IsTeamPlay())
				{
//					pCharacter->SetVisible(true);		// RAONHAJE: PeerOpened TEST
				}
			}


			/*
			// TODO: AGENT 통한 peer들에게는 씹힌다. 수정요망.

			//신입 캐릭터에게 자신의 무기를 알린다...

			ZCharacter* pMyCharacter = g_pGame->m_pMyCharacter;
			if(pMyCharacter)
			{
				//			if(pMyCharacter != pCharacter) { // 자신이 새로 생긴 캐릭이 아니라면
				int nParts = g_pGame->m_pMyCharacter->GetItems()->GetSelectedWeaponParts();
				ZPostChangeWeapon(nParts);
				//			}
			}
			*/
		}
	}
}

void ZGame::DeleteCharacter(const MUID& uid)
{
	bool bObserverDel = false;
	ZCharacter* pCharacter = ZGetCharacterManager()->Find(uid);

	// 옵져버 타겟인 경우 다른 타겟으로 바꿔준다.
	ZObserver* pObserver = ZGetGameInterface()->GetCombatInterface()->GetObserver();
	if (pObserver->IsVisible())
	{
		if ((pCharacter != NULL) && (pCharacter == pObserver->GetTargetCharacter()))
		{
			bObserverDel = true;				
		}
	}

	m_CharacterManager.Delete(uid);


	if (bObserverDel)
	{
		if (pObserver) pObserver->SetFirstTarget();
	}
}


void ZGame::OnStageEnterBattle(MCmdEnterBattleParam nParam, MTD_PeerListNode* pPeerNode)
{
	if (ZApplication::GetGameInterface()->GetState() != GUNZ_GAME) return;
	MUID uidChar = pPeerNode->uidChar;

	if (uidChar == ZGetMyUID())		// enter한사람이 나자신일 경우
	{
#ifdef _ZPEERANTIHACK
		ZPeerAntiHack.clear();
#endif
		LocalKillData.clear();
		LocalAttKillData.clear();
		if (ZGetGame()->CreateMyCharacter(&pPeerNode->CharInfo/*, &pPeerNode->CharBuffInfo*/) == true)
		{
			ConfigureCharacter(uidChar, (MMatchTeam)pPeerNode->ExtendInfo.nTeam, pPeerNode->ExtendInfo.nPlayerFlags);	// Player Character 포함
		}
	}
	else							// enter한사람이 나 자신이 아닐경우
	{
		OnAddPeer(pPeerNode->uidChar, pPeerNode->dwIP, pPeerNode->nPort, pPeerNode);
	}


	if (nParam == MCEP_FORCED)
	{
		ZCharacter* pChar = ZGetCharacterManager()->Find(uidChar);
		GetMatch()->OnForcedEntry(pChar);

		char temp[256] = "";
		if((pPeerNode->ExtendInfo.nPlayerFlags & MTD_PlayerFlags_AdminHide)==0) {
			ZTransMsg(temp, MSG_GAME_JOIN_BATTLE, 1, pChar->GetUserAndClanName());
			ZChatOutput(MCOLOR(ZCOLOR_GAME_INFO), temp);
		}
#ifdef _REPLAY_TEST_LOG
		mlog("[Add Character %s(%d)]\n", pChar->GetCharInfo()->szName, uidChar.Low);
#endif
	}

	ZGetGameClient()->OnStageEnterBattle(uidChar, nParam);
}

void ZGame::OnStageLeaveBattle(const MUID& uidChar, const bool bIsRelayMap)//, const MUID& uidStage)
{
	if (ZApplication::GetGameInterface()->GetState() != GUNZ_GAME) return;

	if (uidChar != ZGetMyUID()) {
		
		ZCharacter* pChar = ZGetCharacterManager()->Find(uidChar);

		if(pChar && !pChar->IsAdminHide() && !bIsRelayMap) {
			{
				LocalKillStreak::iterator i = LocalKillData.find((char*)pChar->GetCharInfo()->szName);
				if(i != LocalKillData.end()) LocalKillData.erase(i);
			}
			LocalAttKillStreak::iterator i = LocalAttKillData.find((char *)pChar->GetCharInfo()->szName);
			if(i!=LocalAttKillData.end()) LocalAttKillData.erase(i);
			char temp[256] = "";
			ZTransMsg(temp, MSG_GAME_LEAVE_BATTLE, 1, pChar->GetUserAndClanName());
			ZChatOutput(MCOLOR(ZCOLOR_GAME_INFO), temp);
		}

		ZGetGameClient()->DeletePeer(uidChar);
		if (ZApplication::GetGameInterface()->GetState() == GUNZ_GAME) {
			DeleteCharacter(uidChar);
		}

		ZGetGameClient()->SetVoteInProgress( false );
		ZGetGameClient()->SetCanVote( false );
	} else if (ZGetGame()->IsRecording())
		ZGetGame()->StopRecording();
}

void ZGame::OnAddPeer(const MUID& uidChar, DWORD dwIP, const int nPort, MTD_PeerListNode* pNode)
{
	if ((ZApplication::GetGameInterface()->GetState() != GUNZ_GAME) || (ZGetGame() == NULL)) return;

	/*
	//// UDPTEST LOG ////////////////////////////////
	char szLog[256];
	sprintf(szLog, "[%d:%d] ADDPEER: Char(%d:%d) IP:%s, Port:%d \n", 
	GetPlayerUID().High, GetPlayerUID().Low, uidChar.High, uidChar.Low, szIP, nPort);
	mlog(szLog);
	/////////////////////////////////////////////////
	*/

	// 추가된 사람이 자기 자신이 아니면
	if (uidChar != ZGetMyUID())
	{
		if (pNode == NULL) {
			_ASSERT(0);
		}

		ZGetGameClient()->DeletePeer(uidChar);	// Delete exist info

		MMatchPeerInfo* pNewPeerInfo = new MMatchPeerInfo;

		if (uidChar == MUID(0,0))	pNewPeerInfo->uidChar = MUID(0, nPort);	// 로컬테스트를 위해서
		else						pNewPeerInfo->uidChar = uidChar;

		in_addr addr;
		addr.s_addr = dwIP;
		char* pszIP = inet_ntoa(addr);
		strcpy(pNewPeerInfo->szIP, pszIP);
		
		pNewPeerInfo->dwIP  = dwIP;
		pNewPeerInfo->nPort = nPort;
		if (!IsReplay())
			memcpy(&pNewPeerInfo->CharInfo, &(pNode->CharInfo), sizeof(MTD_CharInfo));	
		else
		{
			MTD_CharInfo currInfo;
			ConvertCharInfo(&currInfo, &pNode->CharInfo, ZReplayLoader::m_nVersion);
			memcpy(&pNewPeerInfo->CharInfo,	&currInfo, sizeof(MTD_CharInfo));	
		}
		//버프정보임시주석 memcpy(&pNewPeerInfo->CharBuffInfo, &(pNode->CharBuffInfo), sizeof(MTD_CharBuffInfo));			
		memcpy(&pNewPeerInfo->ExtendInfo,	&(pNode->ExtendInfo),	sizeof(MTD_ExtendInfo));

		ZGetGameClient()->AddPeer(pNewPeerInfo);	

		RefreshCharacters();
	}

	ConfigureCharacter(uidChar, (MMatchTeam)pNode->ExtendInfo.nTeam, pNode->ExtendInfo.nPlayerFlags);	// Player Character 포함
}

void ZGame::OnPeerList(const MUID& uidStage, void* pBlob, int nCount)
{
	if (ZGetGameClient()->GetStageUID() != uidStage) return;
	if (ZApplication::GetGameInterface()->GetState() != GUNZ_GAME) return;
	if ((ZGetGame() == NULL) || (ZGetCharacterManager() == NULL)) return;

	for(int i=0; i<nCount; i++) {
		MTD_PeerListNode* pNode = (MTD_PeerListNode*)MGetBlobArrayElement(pBlob, i);
		OnAddPeer(pNode->uidChar, pNode->dwIP, pNode->nPort, pNode);

		ZCharacter* pChar = ZGetCharacterManager()->Find(pNode->uidChar);
		if (pChar) {
			pChar->SetVisible(false);
		}
	}
}

void ZGame::PostMyBuffInfo()
{
	// 내게 발동되어 있는 버프 상태를 피어들에게 알려준다
	if (m_pMyCharacter)
	{
		void* pBlob = m_pMyCharacter->MakeBuffEffectBlob();
		if (pBlob)
		{
			ZPostBuffInfo(pBlob);
			MEraseBlobArray(pBlob);
		}
	}
}

void ZGame::OnPeerBuffInfo(const MUID& uidSender, void* pBlobBuffInfo)
{
	if (uidSender == ZGetMyUID()) return;

	ZCharacter* pSender = ZGetCharacterManager()->Find(uidSender);
	if (!pSender) return;
	if (!pBlobBuffInfo) return;

	MTD_BuffInfo* pBuffInfo = NULL;
	int numElem = MGetBlobArrayCount(pBlobBuffInfo);
	 if (MGetBlobArraySize(pBlobBuffInfo) != (8 + (sizeof(MTD_BuffInfo) * numElem))) 
    return;
    
	for (int i=0; i<numElem; ++i)
	{
		pBuffInfo = (MTD_BuffInfo*)MGetBlobArrayElement(pBlobBuffInfo, i);

		ApplyPotion(pBuffInfo->nItemId, pSender, (float)pBuffInfo->nRemainedTime);
	}
}

void ZGame::OnGameRoundState(const MUID& uidStage, int nRound, int nRoundState, int nArg)
{
	if (ZApplication::GetGameInterface()->GetState() != GUNZ_GAME) return;
	ZMatch* pMatch = GetMatch();
	if (pMatch == NULL) return;

	MMATCH_ROUNDSTATE RoundState = MMATCH_ROUNDSTATE(nRoundState);

	// 프리모드일경우
	if ((RoundState == MMATCH_ROUNDSTATE_FREE) && (pMatch->GetRoundState() != RoundState))
	{
		pMatch->InitCharactersPosition();
		m_pMyCharacter->SetVisible(true);
		m_pMyCharacter->Revival();
		ReleaseObserver();
	}

	pMatch->SetRound(nRound);
	pMatch->SetRoundState(RoundState, nArg);
	AddEffectRoundState(RoundState, nArg);

	if (RoundState == MMATCH_ROUNDSTATE_FINISH)
	{
		ZGetMyInfo()->GetGameInfo()->InitRound();	// 제일 마지막에 불려져야 한다.

	}
}


bool ZGame::FilterDelayedCommand(MCommand *pCommand)
{
	bool bFiltered = true;
	float fDelayTime = 0;

	MUID uid=pCommand->GetSenderUID();
	ZCharacter *pChar=ZGetCharacterManager()->Find(uid);
	if(!pChar) return false;

	switch (pCommand->GetID())
	{
		case MC_PEER_SKILL:
			{
				int nSkill;
				pCommand->GetParameter(&nSkill, 0, MPT_INT);
				fDelayTime = .15f;
				int sel_type;
				pCommand->GetParameter(&sel_type, 2, MPT_INT);
				switch(nSkill)	{
					case ZC_SKILL_UPPERCUT		: 
						if(pChar!=m_pMyCharacter) {
						pChar->SetAnimationLower(ZC_STATE_LOWER_UPPERCUT);
						if(sel_type == MWT_GREAT_SWORD)
						pChar->SetHP(pChar->GetHP()-3.0);
						}
						break;
					case ZC_SKILL_SPLASHSHOT	: break;
					case ZC_SKILL_DASH			: break;
				}

				////////////////////////////////////////////////////////////////////
				MMatchCharItemParts parts = (MMatchCharItemParts)sel_type;
				if( parts != pChar->GetItems()->GetSelectedWeaponParts()) {
					// 지금 들고 있는 무기와 보내진 무기가 틀리다면 보내진 무기로 바꿔준다..
					OnChangeWeapon(uid,parts);
				}
				////////////////////////////////////////////////////////////////////


			}break;

		case MC_PEER_SHOT:
			{
				MCommandParameter* pParam = pCommand->GetParameter(0);
				if(pParam->GetType()!=MPT_BLOB) break;	// 문제가 있다
				ZPACKEDSHOTINFO *pinfo =(ZPACKEDSHOTINFO*)pParam->GetPointer();

				// 칼질만 딜레이가 있다
				if(pinfo->sel_type!=MMCIP_MELEE) return false;

				if(pChar!=m_pMyCharacter &&
					( pChar->m_pVMesh->m_SelectWeaponMotionType==eq_wd_dagger ||
					pChar->m_pVMesh->m_SelectWeaponMotionType==eq_ws_dagger )) { // dagger
						pChar->SetAnimationUpper(ZC_STATE_UPPER_SHOT);
					}

				fDelayTime = .15f;

				////////////////////////////////////////////////////////////////////
				MMatchCharItemParts parts = (MMatchCharItemParts)pinfo->sel_type;
				if( parts != pChar->GetItems()->GetSelectedWeaponParts()) {
					// 지금 들고 있는 무기와 보내진 무기가 틀리다면 보내진 무기로 바꿔준다..
					OnChangeWeapon(uid,parts);
				}
				///////////////////////////////////////////////////////////////////////////////
			}
			break;

		// 새로 추가된 근접공격 커맨드
		case MC_PEER_SHOT_MELEE:
			{
				float fShotTime;
				rvector pos;
				int nShot;

				pCommand->GetParameter(&fShotTime, 0, MPT_FLOAT);
				pCommand->GetParameter(&pos, 1, MPT_POS);
				pCommand->GetParameter(&nShot, 2, MPT_INT);

				if(pChar!=m_pMyCharacter &&
					( pChar->m_pVMesh->m_SelectWeaponMotionType==eq_wd_dagger ||
					pChar->m_pVMesh->m_SelectWeaponMotionType==eq_ws_dagger )) { // dagger
						pChar->SetAnimationUpper(ZC_STATE_UPPER_SHOT);
					}

				fDelayTime = .1f;
				switch(nShot) {
					case 1 : fDelayTime = .10f;break;
					case 2 : fDelayTime = .15f;break;
					case 3 : fDelayTime = .2f;break;
					case 4 : fDelayTime = .25f;break;
				}


				if ( nShot > 1)
				{
					char szFileName[ 20];
					if ( pChar->GetProperty()->nSex == MMS_MALE)
						sprintf( szFileName, "fx2/MAL_shot_%02d", nShot);
					else
						sprintf( szFileName, "fx2/FEM_shot_%02d", nShot);

					ZGetSoundEngine()->PlaySound( szFileName, pChar->GetPosition());
				}
			}
			break;

// 퀘스트 커맨드들
		case MC_QUEST_PEER_NPC_ATTACK_MELEE :	// npc 칼질
			ZGetQuest()->OnPrePeerNPCAttackMelee(pCommand);
			fDelayTime = .4f;break;


		// 이것들 이외의 것들은 
		default:
			bFiltered = false;
			break;
	}

	
	if(bFiltered)
	{
		ZObserverCommandItem *pZCommand=new ZObserverCommandItem;
		pZCommand->pCommand=pCommand->Clone();
		pZCommand->fTime=GetTime()+fDelayTime;
		m_DelayedCommandList.push_back(pZCommand);
		return true;
	}

	return false;
}

void ZGame::PostSpMotion(ZC_SPMOTION_TYPE mtype)
{
	if(m_pMyCharacter==NULL) return;
	// Lucas: Added MMATCH_ROUNDSTATE_FREE to allowed emotion list
	if (m_Match.GetRoundState() != MMATCH_ROUNDSTATE_PLAY && m_Match.GetRoundState() != MMATCH_ROUNDSTATE_FREE) return;

	// LUcas: Macro spam fix
	if (ZGetGameInterface()->GetCombatInterface()->GetObserverMode())
		return;

	if( (m_pMyCharacter->m_AniState_Lower.Ref() == ZC_STATE_LOWER_IDLE1) || 
		(m_pMyCharacter->m_AniState_Lower.Ref() == ZC_STATE_LOWER_IDLE2) || 
		(m_pMyCharacter->m_AniState_Lower.Ref() == ZC_STATE_LOWER_IDLE3) || 
		(m_pMyCharacter->m_AniState_Lower.Ref() == ZC_STATE_LOWER_IDLE4) ) 
	{

		MMatchWeaponType type = MWT_NONE;

		ZItem* pSItem = m_pMyCharacter->GetItems()->GetSelectedWeapon();

		if( pSItem && pSItem->GetDesc() ) {
			type = pSItem->GetDesc()->m_nWeaponType.Ref();
		}

		if( mtype == ZC_SPMOTION_TAUNT ) // taunt 일 경우 모션이 없어서...
			if( (type == MWT_MED_KIT) || 
				(type == MWT_REPAIR_KIT) || 
				(type == MWT_FOOD) ||
				(type == MWT_BULLET_KIT) ||
				(type == MWT_CLAYMORE)) 
			{
				return;
			}

		ZPostSpMotion(mtype);
	}
}

void ZGame::OnEventUpdateJjang(const MUID& uidChar, bool bJjang)
{
	ZCharacter* pCharacter = m_CharacterManager.Find(uidChar);
	if (pCharacter == NULL) return;

	if (bJjang) 
		ZGetEffectManager()->AddStarEffect(pCharacter);        
}

bool ZGame::CanAttack(ZObject *pAttacker, ZObject *pTarget)
{
	//### 이 함수를 수정하면 똑같이 CanAttack_DebugRegister()에도 적용해 주어야 합니다. ###
	if(!IsReplay())
		if(GetMatch()->GetRoundState() != MMATCH_ROUNDSTATE_PLAY) return false;
	if(pAttacker==NULL) return true;

	if ( GetMatch()->IsTeamPlay() ) {
		if (pAttacker->GetTeamID() == pTarget->GetTeamID()) {
			if (!GetMatch()->GetTeamKillEnabled()) 
				return false;
		}
	}
	if ( ZGetGameClient()->GetMatchStageSetting()->IsBountyPlay() ) {
		ZRuleBounty* pRule = (ZRuleBounty*)ZGetGame()->GetMatch()->GetRule();
		if(pTarget->GetUID() != pRule->GetBountyUID() && pAttacker->GetUID() != pRule->GetBountyUID())
			return false;
		else
			return true;
	}

#ifdef _QUEST
	if (ZGetGameTypeManager()->IsQuestDerived(ZGetGameClient()->GetMatchStageSetting()->GetGameType()))
	{
		if (pAttacker->GetTeamID() == pTarget->GetTeamID())
		{
			return false;
		}
	}

#endif
	return true;
}

//jintriple3 디버그 레지스터 해킹 방지를 위해 ISAttackable()함수를 다른 이름으로 하나 더 만들었음...
bool ZGame::CanAttack_DebugRegister(ZObject *pAttacker, ZObject *pTarget)
{
	if(!IsReplay())
		if(GetMatch()->GetRoundState() != MMATCH_ROUNDSTATE_PLAY) return false;
	if(pAttacker==NULL) return true;

	if ( GetMatch()->IsTeamPlay() ) {
		if (pAttacker->GetTeamID() == pTarget->GetTeamID()) {
			if (!GetMatch()->GetTeamKillEnabled()) 
				return false;
		}
	}
	if ( ZGetGameClient()->GetMatchStageSetting()->IsBountyPlay() ) {
		ZRuleBounty* pRule = (ZRuleBounty*)ZGetGame()->GetMatch()->GetRule();
		if(pTarget->GetUID() != pRule->GetBountyUID() && pAttacker->GetUID() != pRule->GetBountyUID())
			return false;
		else
			return true;
	}

#ifdef _QUEST
	if (ZGetGameTypeManager()->IsQuestDerived(ZGetGameClient()->GetMatchStageSetting()->GetGameType()))
	{
		if (pAttacker->GetTeamID() == pTarget->GetTeamID())
		{
			return false;
		}
	}

#endif
	return true;
}



void ZGame::ShowReplayInfo( bool bShow)
{
	MWidget* pWidget = ZGetGameInterface()->GetIDLResource()->FindWidget( "CombatChatOutput");
	if ( pWidget)
		pWidget->Show( bShow);

	m_bShowReplayInfo = bShow;
}

void ZGame::OnLocalOptainSpecialWorldItem(MCommand* pCommand)
{
	int nWorldItemID;
	pCommand->GetParameter(&nWorldItemID   , 0, MPT_INT);

	switch (nWorldItemID)
	{
	case WORLDITEM_PORTAL_ID:
		{
			MMATCH_GAMETYPE eGameType = ZGetGameClient()->GetMatchStageSetting()->GetGameType();
			if (!ZGetGameTypeManager()->IsQuestDerived(eGameType)) break;

			// 서버에 포탈로 이동한다고 전송
			char nCurrSectorIndex = ZGetQuest()->GetGameInfo()->GetCurrSectorIndex();
			ZPostQuestRequestMovetoPortal(nCurrSectorIndex);
		}
		break;
	};
}


void ZGame::ReserveSuicide( void)
{
	m_bSuicide = true;
}


bool ZGame::OnRuleCommand(MCommand* pCommand)
{
#ifdef _QUEST
	MMATCH_GAMETYPE eGameType = ZGetGameClient()->GetMatchStageSetting()->GetGameType();
	if (!ZGetGameTypeManager()->IsQuestDerived(eGameType) && ZGetNpcHelper()->OnGameCommand(pCommand)) return true;
	else if (ZGetQuest()->OnGameCommand(pCommand)) return true;
#endif

	switch (pCommand->GetID())
	{
	case MC_MATCH_ASSIGN_COMMANDER:
	case MC_MATCH_ASSIGN_BERSERKER:
	case MC_MATCH_FLAG_EFFECT:
	case MC_MATCH_FLAG_CAP:
	case MC_MATCH_FLAG_STATE:
	case MC_MATCH_ASSIGN_REVERSE_BERSERKER:
	case MC_MATCH_GAME_DEAD:
	case MC_MATCH_PEER_WEAPON_CHANGE:
	case MC_MATCH_PEER_TOP_PLAYER:
	case MC_MATCH_PEER_GUNGAME_SCORE:
	case MC_MATCH_DUEL_QUEUEINFO:
	case MC_MATCH_DUELTOURNAMENT_GAME_NEXT_MATCH_PLYAERINFO:
	case MC_MATCH_DUELTOURNAMENT_GAME_INFO:
	case MC_MATCH_DUELTOURNAMENT_GAME_ROUND_RESULT_INFO:
	case MC_MATCH_DUELTOURNAMENT_GAME_MATCH_RESULT_INFO:
		{
			if (m_Match.OnCommand(pCommand)) return true;
		};
	};

	return false;
}

void ZGame::OnResetTeamMembers(MCommand* pCommand)
{
	if (!m_Match.IsTeamPlay()) return;

	ZChatOutput( MCOLOR(ZCOLOR_GAME_INFO), ZMsg(MSG_GAME_MAKE_AUTO_BALANCED_TEAM) );

	MCommandParameter* pParam = pCommand->GetParameter(0);
	if(pParam->GetType()!=MPT_BLOB) return;
	void* pBlob = pParam->GetPointer();
	int nCount = MGetBlobArrayCount(pBlob);

	ZCharacterManager* pCharMgr = ZGetCharacterManager();

	for (int i = 0; i < nCount; i++)
	{
		MTD_ResetTeamMembersData* pDataNode = (MTD_ResetTeamMembersData*)MGetBlobArrayElement(pBlob, i);

		ZCharacter* pChar = pCharMgr->Find(pDataNode->m_uidPlayer);
		if (pChar == NULL) continue;

		if (pChar->GetTeamID() != ( (MMatchTeam)pDataNode->nTeam) )
		{
			// 만약 나자신이 팀변경이 되었으면 팀변경되었는지를 남긴다.
			if (pDataNode->m_uidPlayer == ZGetMyUID())
			{
				ZGetMyInfo()->GetGameInfo()->bForcedChangeTeam = true;
			}
			if(ZGetGameClient()->GetMatchStageSetting()->GetGameType() != MMATCH_GAMETYPE_CTF)
				pChar->SetTeamID((MMatchTeam)pDataNode->nTeam);
			else
				pChar->TeamChange = true;
		}
	}
}


void ZGame::MakeResourceCRC32( const DWORD dwKey, DWORD& out_crc32, DWORD& out_xor )
{
	out_crc32 = 0;
	out_xor = 0;

#ifdef _DEBUG
	static DWORD dwOutputCount = 0;
	++dwOutputCount;
#endif

	MMatchObjCacheMap* pObjCacheMap = ZGetGameClient()->GetObjCacheMap();
	if( NULL == pObjCacheMap )
	{
		return ;
	}

	MMatchObjCacheMap::const_iterator	end			= pObjCacheMap->end();
	MMatchObjCacheMap::iterator			it			= pObjCacheMap->begin();
	MMatchObjCache*						pObjCache	= NULL;
	MMatchItemDesc*						pitemDesc	= NULL;
	MMatchCRC32XORCache					CRC32Cache;

	CRC32Cache.Reset();
	CRC32Cache.CRC32XOR( dwKey );

#ifdef _DEBUG
	mlog( "Start ResourceCRC32Cache : %u\n", CRC32Cache.GetXOR() );
#endif

	for( ; end != it; ++it )
	{
		pObjCache = it->second;

		for( int i = 0; i < MMCIP_END; ++i )
		{
			pitemDesc = MGetMatchItemDescMgr()->GetItemDesc( pObjCache->GetCostume()->nEquipedItemID[i] );
			if( NULL == pitemDesc )
			{
				continue;
			}

			pitemDesc->CacheCRC32( CRC32Cache );
			
#ifdef _DEBUG
			if( 10 > dwOutputCount )
			{
				mlog( "ItemID : %d, CRCCache : %u\n"
					, pitemDesc->m_nID
					, CRC32Cache.GetXOR() );
			}
#endif
		}
	}

#ifdef _DEBUG
	if( 10 > dwOutputCount )
	{
		mlog( "ResourceCRCSum : %u\n", CRC32Cache.GetXOR() );
	}
#endif
	
	out_crc32 = CRC32Cache.GetCRC32();
	out_xor = CRC32Cache.GetXOR();
}

void ZGame::OnResponseUseSpendableBuffItem(MUID& uidItem, int nResult)
{
	// TodoH(상) - 사용에 대한 결과 처리
}

/*
void ZGame::OnGetSpendableBuffItemStatus(MUID& uidChar, MTD_CharBuffInfo* pCharBuffInfo)
{
	if (uidChar != ZGetMyUID()) {
		_ASSERT(0);
		return;
	}

	if( pCharBuffInfo == NULL ) {
		_ASSERT(0);
		return;
	}

	ZGetMyInfo()->SetCharBuffInfo(pCharBuffInfo);

	if( m_pMyCharacter != NULL ) {
		m_pMyCharacter->SetCharacterBuff(pCharBuffInfo);
	}
}*/

void ZGame::ApplyPotion(int nItemID, ZCharacter* pCharObj, float fRemainedTime)
{
	MMatchItemDesc* pDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemID);
	if( pDesc == NULL ) { _ASSERT(0);  return; }

	MMatchDamageType nDamageType = pDesc->m_nDamageType.Ref();

	if (nDamageType == MMDT_HASTE)
	{
		if(GetMatch()->GetMatchType() == MMATCH_GAMETYPE_CTF)
		{
			ZChatOutput("Speed vial's disabled in capture the flag." , ZChat::CMT_SYSTEM);
			return;
		}
		// 가속 아이템
		ZModule_Movable* pMod = (ZModule_Movable*)pCharObj->GetModule(ZMID_MOVABLE);
		if (pMod)
		{
			if (fRemainedTime == 0)
				fRemainedTime = pDesc->m_nDamageTime.Ref()*0.001f;

			pMod->SetMoveSpeedHasteRatio(pDesc->m_nItemPower.Ref()*0.01f, fRemainedTime, nItemID);
		}
		ZGetEffectManager()->AddHasteBeginEffect(pCharObj->GetPosition(), pCharObj);
	}
	else if (nDamageType == MMDT_HEAL || nDamageType == MMDT_REPAIR)
	{
		// 즉시 회복 아이템
		if (pDesc->m_nDamageTime.Ref() == 0)
		{
			ZGetEffectManager()->AddPotionEffect( pCharObj->GetPosition(), pCharObj, pDesc->m_nEffectId );

			if (nDamageType == MMDT_HEAL)
			{
				int nAddedHP = pDesc->m_nItemPower.Ref();
				pCharObj->SetHP( min( pCharObj->GetHP() + nAddedHP, pCharObj->GetMaxHP() ) );
			}
			else if (nDamageType == MMDT_REPAIR)
			{
				int nAddedAP = pDesc->m_nItemPower.Ref();
				pCharObj->SetAP( min( pCharObj->GetAP() + nAddedAP, pCharObj->GetMaxAP() ) );
			}
			else
				_ASSERT(0);
		}
		// 힐오버타임 아이템
		else
		{
			ZModule_HealOverTime* pMod = (ZModule_HealOverTime*)pCharObj->GetModule(ZMID_HEALOVERTIME);
			if (pMod)
			{
				int nRemainedHeal = (int)fRemainedTime;
				if (nRemainedHeal == 0)
					nRemainedHeal = pDesc->m_nDamageTime.Ref();

				//damagetime에 힐 횟수를 표기할 것
				pMod->BeginHeal(pDesc->m_nDamageType.Ref(), pDesc->m_nItemPower.Ref(), nRemainedHeal, pDesc->m_nEffectId, nItemID);
			}

			switch (nDamageType)
			{
			case MMDT_HEAL:
				ZGetEffectManager()->AddHealOverTimeBeginEffect(pCharObj->GetPosition(), pCharObj);
				break;
			case MMDT_REPAIR:
				ZGetEffectManager()->AddRepairOverTimeBeginEffect(pCharObj->GetPosition(), pCharObj);
				break;
			}
		}
	}
	else
		_ASSERT(0);
}

void ZGame::OnUseTrap(int nItemID, ZCharacter* pCharObj, rvector& pos)
{
	MMatchItemDesc* pDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemID);
	if( pDesc == NULL ) { _ASSERT(0); return; }

	rvector velocity;
	velocity	= /*pCharObj->GetVelocity()+ */pCharObj->m_TargetDir * 1300.f;
	velocity.z  = velocity.z + 300.f;
	m_WeaponManager.AddTrap(pos, velocity, nItemID, pCharObj);
}

void ZGame::OnUseDynamite(int nItemID, ZCharacter* pCharObj, rvector& pos)
{
	MMatchItemDesc* pDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemID);
	if( pDesc == NULL ) { _ASSERT(0); return; }

	rvector velocity;
	velocity	= /*pCharObj->GetVelocity()+ */pCharObj->m_TargetDir * 1300.f;
	velocity.z  = velocity.z + 300.f;
	m_WeaponManager.AddDynamite(pos, velocity, pCharObj);
}

void ZGame::CheckZoneTrap(MUID uidOwner,rvector pos,MMatchItemDesc* pItemDesc, MMatchTeam nTeamID)
{
	if (!pItemDesc) return;

	float fRange = 300.f;

	ZObject* pTarget = NULL;
	ZCharacter* pOwnerCharacter = m_CharacterManager.Find( uidOwner );

	float fDist;
	bool bReturnValue;

	for(ZObjectManager::iterator itor = m_ObjectManager.begin(); itor != m_ObjectManager.end(); ++itor) 
	{
		if(!pOwnerCharacter) break;
		pTarget = (*itor).second;

		// 내 트랩이면 피해 없음
#ifndef _DEBUG	// 디버그 빌드에 한해서 테스트를 쉽게 하기 위해 내 트랩도 피해를 받도록 한다
		bReturnValue = pTarget->GetUID() == uidOwner;
		if (pTarget->GetUID() == uidOwner)
			PROTECT_DEBUG_REGISTER(bReturnValue)
				continue;
#endif

		bReturnValue = CanAttack( pOwnerCharacter, pTarget);
		if ( !bReturnValue)
			PROTECT_DEBUG_REGISTER(!CanAttack_DebugRegister(pOwnerCharacter, pTarget))
			continue;

		//jintriple3 디버그 레지스터 해킹 관련 버그.....
		bReturnValue = !pTarget || pTarget->IsDie();
		if( !pTarget || pTarget->IsDie())
			PROTECT_DEBUG_REGISTER(bReturnValue)
				continue;

		// 캐릭터의 발목쯤에서 반경 체크를 한다
		fDist = Magnitude(pos-(pTarget->GetPosition()+rvector(0,0,10)));
		//jintriple3 디버그 레지스터 핵 관련 버그.....
		bReturnValue = fDist >=fRange;
		if(fDist >= fRange)
			PROTECT_DEBUG_REGISTER(bReturnValue)
				continue;

		if (pos.z > pTarget->GetPosition().z + pTarget->GetCollHeight())
			continue;

		// 트랩 반경 꼭대기와 캐릭터의 발목사이에 벽이 있는지 확인
		/*{
			const DWORD dwPickPassFlag = RM_FLAG_ADDITIVE | RM_FLAG_HIDE | RM_FLAG_PASSROCKET | RM_FLAG_PASSBULLET;
			RBSPPICKINFO bpi;
			rvector orig = pos+rvector(0,0,fRange);
			rvector to = pTarget->GetPosition()+rvector(0,0,10);
			bool bBspPicked = GetWorld()->GetBsp()->PickTo(orig, to, &bpi, dwPickPassFlag);
			if(bBspPicked)
			{
				if (Magnitude(to-orig) > Magnitude(bpi.PickPos-orig))
					continue;
			}
		}// 이펙트는 벽을 뚫었는데 피해를 안입는게 더 이상하다고 해서 주석*/

		//ZGetSoundEngine()->PlaySound("fx_bullethit_mt_met");

		if (m_pGameAction)
		{
			int nDuration = pItemDesc->m_nDamageTime.Ref();
			bool bApplied = false;
			switch (pItemDesc->m_nDamageType.Ref())
			{
			case MMDT_FIRE:
				// 불트랩은 ItemPower가 대미지를 뜻함
				bApplied = m_pGameAction->ApplyFireEnchantDamage(pTarget, pOwnerCharacter, pItemDesc->m_nItemPower.Ref(), nDuration);
				break;
			case MMDT_COLD:
				// 얼음트랩은 ItemPower가 이속감소량을 뜻함 (80이면 80%의 속력이 됨)
				bApplied = m_pGameAction->ApplyColdEnchantDamage(pTarget, pOwnerCharacter,pItemDesc->m_nItemPower.Ref(), nDuration);
				break;
			case MMDT_LIGHTING:
				bApplied = m_pGameAction->ApplyLightningEnchantDamage(pTarget, pOwnerCharacter, pItemDesc->m_nItemPower.Ref(), nDuration);
				break;
			case MMDT_POISION:
				bApplied = m_pGameAction->ApplyPoisonEnchantDamage(pTarget, pOwnerCharacter, pItemDesc->m_nItemPower.Ref(), nDuration);
				break;
			default:
				_ASSERT(0);
			}

			if (bApplied)
			{
				if(pOwnerCharacter) 
				{
					CheckCombo(pOwnerCharacter, pTarget, true);	//todok 사운드를 켜줘야할까?;
					CheckStylishAction(pOwnerCharacter);
				}

				// 물속에 있을 때는 사람이 밟았을 때 물튀김 효과를 일으키자
				GetWorld()->GetWaters()->CheckSpearing( pos, pos + rvector(0,0,MAX_WATER_DEEP), 500, 0.8f );
			}
		}
	}
}

void ZGame::OnExplosionDynamite(MUID uidOwner, rvector pos, float fDamage, float fRange, float fKnockBack, MMatchTeam nTeamID)
{
	ZObject* pTarget = NULL;

	float fDist;

	for(ZObjectManager::iterator itor = m_ObjectManager.begin(); itor != m_ObjectManager.end(); ++itor) 
	{
		pTarget = (*itor).second;
		if(pTarget->GetTeamID() == nTeamID || pTarget->GetUID() == uidOwner) continue;
		bool bReturnValue = !pTarget || pTarget->IsDie();
		if( !pTarget || pTarget->IsDie())
			PROTECT_DEBUG_REGISTER(bReturnValue)
			continue;

		fDist = Magnitude(pos-(pTarget->GetPosition()+rvector(0,0,80)));
		bReturnValue = fDist >= fRange;
		if(fDist >= fRange)
			PROTECT_DEBUG_REGISTER(bReturnValue)
			continue;

		rvector dir = pos - (pTarget->GetPosition() + rvector(0, 0, 80));
		Normalize(dir);

		// 다이너마이트도 수류탄처럼 반동으로 튀어나간다.
		ZActor* pATarget = MDynamicCast(ZActor,pTarget);

		bool bPushSkip = false;

		if(pATarget) 
		{
			bPushSkip = pATarget->GetNPCInfo()->bNeverPushed;
		}

		if(bPushSkip==false)
		{
			pTarget->AddVelocity(fKnockBack*7.f*(fRange-fDist)*-dir);
		}
		else 
		{
			ZGetSoundEngine()->PlaySound("fx_bullethit_mt_met");
		}
		ZCharacter* pOwnerCharacter = ZGetGame()->m_CharacterManager.Find( uidOwner );
		if(pOwnerCharacter) 
		{
			CheckCombo(pOwnerCharacter, pTarget,!bPushSkip);
			CheckStylishAction(pOwnerCharacter);
		}

		// 다이너마이트는 폭발 범위 내에서 모두 같은 데미지를 입힌다.
		float fRatio = ZItem::GetPiercingRatio( MWT_DYNAMITYE, eq_parts_chest );
		pTarget->OnDamaged(pOwnerCharacter, pos, ZD_EXPLOSION, MWT_DYNAMITYE, fDamage, fRatio);
	}

#define SHOCK_RANGE		1500.f			// 10미터까지 흔들린다

	ZCharacter *pTargetCharacter = ZGetGameInterface()->GetCombatInterface()->GetTargetCharacter();
	float fPower = (SHOCK_RANGE - Magnitude(pTargetCharacter->GetPosition()+rvector(0,0,50) - pos)) / SHOCK_RANGE;

	if(fPower>0)
		ZGetGameInterface()->GetCamera()->Shock(fPower*500.f, .5f, rvector(0.0f, 0.0f, -1.0f));

	GetWorld()->GetWaters()->CheckSpearing( pos, pos + rvector(0, 0, MAX_WATER_DEEP), 500, 0.8f );
}