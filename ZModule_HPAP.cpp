#include "stdafx.h"
#include "ZModule_HPAP.h"
#include "ZGame.h"
#include "ZApplication.h"
#include "CheckReturnCallStack.h"

ZModule_HPAP::ZModule_HPAP() : m_LastAttacker(MUID(0,0)), m_bRealDamage_DebugRegister(false), m_fAccumulationDamage(0.f), m_bAccumulationDamage(false)
{
	float f = float(rand() % 1000); 
	f += float(rand() % 100) * 0.01f;
	m_fMask.Set_MakeCrc(f);

	m_bRealDamage.Set_MakeCrc(false);
	m_fHP.Set_MakeCrc(1000.f);
	m_fAP.Set_MakeCrc(1000.f);
	m_fMaxHP.Set_MakeCrc(100);
	m_fMaxAP.Set_MakeCrc(100);
}



ZModule_HPAP::~ZModule_HPAP()
{
}

float ZModule_HPAP::GetHP() 
{
	return  m_fHP.Ref() - GetMask();
}

float	ZModule_HPAP::GetAP() 
{
	return m_fAP.Ref() - GetMask();
}

void ZModule_HPAP::OnDamage(MUID uidAttacker,float fDamage, float fRatio, int Type)
{
	_ASSERT(fDamage >= 0);
	if (fDamage < 0) fDamage *= -1.f;	// 해킹으로 음수대미지를 넣어서 회복할 수 있으므로..

	m_LastAttacker = uidAttacker;
	// 퀘스트 테스트용 치트 체크
#ifndef _PUBLISH
	if (CheckQuestCheet() == true) return;
#endif
	//jintriple3 디버그 레지스터 핵 방지 코드
	//내 캐릭터가 아니거나 npc가 아니면....
	//m_bRealDamage의 경우 내 클라이언트에서 실제로 데미지를 적용하나 안하나를 체크하는 변수.
	//MyCharacter 이거나 NPC만 내 클라이언트에서 데미지 적용.
	if(!m_bRealDamage.Ref() && (ZDAMAGETYPE)Type != ZD_FALLING)	
		PROTECT_DEBUG_REGISTER(!m_bRealDamage_DebugRegister)	//이걸 위해 새로운 변수 하나 더 사용.
			return;
	// NPC의 난이도 조절계수때문에 넣음
	ZObject* pAttacker = ZGetObjectManager()->GetObject(uidAttacker);
	if ((pAttacker) && (!IsPlayerObject(pAttacker)))
	{
		ZActor* pActor = (ZActor*)pAttacker;
		//fDamage = (int)(fDamage * (pActor->GetTC()));
		fDamage = fDamage * (pActor->GetQL() * 0.2f + 1);
	}

	if(!ZGetGame()->m_pMyCharacter->IsObserverTarget() && ZGetGame()->GetMatch()->GetMatchType() != MMATCH_GAMETYPE_DEATHMATCH_TEAM2_NPC && !ZGetGame()->GetMatch()->IsQuestDrived() && !ZGetGame()->m_pMyCharacter->IsDie() && !ZGetGameClient()->IsDuelTournamentGame())
	{
	if(!pAttacker || !ZGetGame()->m_pMyCharacter) return;
		if(pAttacker->GetUID() != ZGetGame()->m_pMyCharacter->GetUID())
		{
			if( ZGetGameClient()->GetMatchStageSetting()->IsAntiLead() == false || ZGetGameClient()->GetMatchStageSetting()->IsAntiLead() == true && (ZDAMAGETYPE)Type != ZD_BULLET && (ZDAMAGETYPE)Type != ZD_BULLET_HEADSHOT)
			{
				if(ZGetGame()->DmgCounter->count(pAttacker->GetUID()) == 1)
				{
					DamageCounterMap::iterator itr = ZGetGame()->DmgCounter->find(pAttacker->GetUID());
					itr->second+= fDamage;
				} else {
					ZGetGame()->DmgCounter->insert(DamageCounterMap::value_type(pAttacker->GetUID(), fDamage));
				}
			}
		}
		if(!ZGetGame()->m_pMyCharacter->IsDie())
		{
			ZGetGame()->m_pMyCharacter->GetStatus().CheckCrc();
			ZGetGame()->m_pMyCharacter->GetStatus().Ref().nTakenDamage += fDamage;		
			ZGetGame()->m_pMyCharacter->GetStatus().Ref().nRoundTakenDamage += fDamage;
			ZGetGame()->m_pMyCharacter->GetStatus().MakeCrc();
		}
	}
	// 내가 죽기전까지 얻은 데미지를 누적한다(일단 듀얼토너먼트 전용임)
	if(IsAccumulationDamage())
	{
		AccumulateDamage(fDamage);
#ifndef _PUBLISH	// 내부빌드에서 누적 대미지 정보 출력
		char szAccumulationDamagePrint[256];
		sprintf(szAccumulationDamagePrint, "피해 당한 누적대미지[%2.1f]", GetAccumulationDamage());
		ZChatOutput(MCOLOR(255, 200, 100), szAccumulationDamagePrint);
#endif
	}        
	float fHPDamage = (float)((float)fDamage * fRatio);
	float fAPDamage = fDamage - fHPDamage;

	if ((GetAP() - fAPDamage) < 0)
	{
		fHPDamage += (fAPDamage - GetAP());
		fAPDamage -= (fAPDamage - GetAP());
	}

	SetHP(GetHP() - fHPDamage);
	SetAP(GetAP() - fAPDamage);
}


void ZModule_HPAP::InitStatus()
{
	if(ZGetGame() && ZGetGame()->m_pMyCharacter && ZGetGame()->m_pMyCharacter->GetUserGrade() != MMUG_ADMIN)
	m_LastAttacker = MUID(0,0);
}

bool ZModule_HPAP::CheckQuestCheet()
{
#ifdef _PUBLISH
	return false;
#endif

	// 퀘스트 테스트용 치트 체크
	if (IsMyCharacter((ZObject*)m_pContainer))
	{
		if ((ZIsLaunchDevelop()) && (ZGetGameClient()->GetServerMode() == MSM_TEST))
		{
			if (ZGetGameTypeManager()->IsQuestDerived(ZGetGameClient()->GetMatchStageSetting()->GetGameType()))
			{
				if (ZGetQuest()->GetCheet(ZQUEST_CHEET_GOD) == true) return true;
			}
		}
	}

	return false;
}

void ZModule_HPAP::ShiftFugitiveValues()
{
	m_fMask.ShiftHeapPos_CheckCrc();

	m_fMaxHP.ShiftHeapPos_CheckCrc();
	m_fMaxAP.ShiftHeapPos_CheckCrc();

	m_fHP.ShiftHeapPos_CheckCrc();
	m_fAP.ShiftHeapPos_CheckCrc();

	m_bRealDamage.ShiftHeapPos_CheckCrc();
}