#include "stdafx.h"
#include "ZRule.h"
#include "ZMatch.h"
#include "ZRuleDeathMatch.h"
#include "ZRuleGladiator.h"
#include "ZRuleAssassinate.h"
#include "ZRuleTraining.h"
#include "ZRuleQuest.h"
#include "ZRuleSurvival.h"
#include "ZRuleZombies.h"
#include "ZRuleBerserker.h"
#include "ZRuleDuel.h"
#include "ZRuleDuelTournament.h"
#include "ZRuleClassic.h"
#include "ZRuleReverseBerserker.h"
#include "ZRuleBountyHunter.h"

ZRule::ZRule(ZMatch* pMatch)
{
	m_pMatch = pMatch;				// 초기화 =_=
}

ZRule::~ZRule()
{

}

void ZRule::Update(float fDelta)
{
	OnUpdate(fDelta);
}

ZRule* ZRule::CreateRule(ZMatch* pMatch, MMATCH_GAMETYPE nGameType)
{
	switch(nGameType)
	{
	case MMATCH_GAMETYPE_DEATHMATCH_SOLO:
		{
			return (new ZRuleSoloDeathMatch(pMatch));
		}
		break;
	case MMATCH_GAMETYPE_GUNGAME:
		{
			return (new ZRuleSoloGunGame(pMatch));
		}
		break;
	case MMATCH_GAMETYPE_DEATHMATCH_TEAM:
		{
			return (new ZRuleTeamDeathMatch(pMatch));
		}
		break;
		case MMATCH_GAMETYPE_DEATHMATCH_TRIPPLE_TEAM:
		{
			return (new ZRuleTeamDeathMatch3(pMatch));
		}
		break;
	case MMATCH_GAMETYPE_GLADIATOR_SOLO:
		{
			return (new ZRuleSoloGladiator(pMatch));
		}
		break;
	case MMATCH_GAMETYPE_GLADIATOR_TEAM:
		{
			return (new ZRuleTeamGladiator(pMatch));
		}
		break;
	case MMATCH_GAMETYPE_ASSASSINATE:
		{
			return (new ZRuleAssassinate(pMatch));
		}
		break;
	case MMATCH_GAMETYPE_TRAINING:
		{
			return (new ZRuleTraining(pMatch));
		}
		break;
#ifdef _QUEST
	case MMATCH_GAMETYPE_SURVIVAL:
		{
			return (new ZRuleSurvival(pMatch));
		}
		break;
#ifdef Zombies
	case MMATCH_GAMETYPE_ZOMBIE:
		{
			return (new ZRuleZombies(pMatch));
		}
		break;
#endif
	case MMATCH_GAMETYPE_QUEST:
		{
			return (new ZRuleQuest(pMatch));
		}
		break;
#endif
	case MMATCH_GAMETYPE_BERSERKER:
		{
			return (new ZRuleBerserker(pMatch));
		}
		break;
	case MMATCH_GAMETYPE_DEATHMATCH_TEAM2:
		{
			return (new ZRuleTeamDeathMatch2(pMatch));
		}
		break;
	case MMATCH_GAMETYPE_DEATHMATCH_TEAM2_NPC:
		{
			return (new ZRuleTeamDeathMatch2Npc(pMatch));
		}
		break;
	case MMATCH_GAMETYPE_DUEL:
		{
			return (new ZRuleDuel(pMatch));
		}
		break;

	case MMATCH_GAMETYPE_DUELTOURNAMENT:
		{
			return (new ZRuleDuelTournament(pMatch));
		}
		break;
	case MMATCH_GAMETYPE_CLASSIC_SOLO:
		{
			return (new ZRuleSoloClassic(pMatch));
		}
		break;

	case MMATCH_GAMETYPE_CLASSIC_TEAM:
		{
			return (new ZRuleTeamClassic(pMatch));
		}
		break;

	case MMATCH_GAMETYPE_REVERSE_BERSERKER:
		{
			return (new ZRuleReverseBerserker(pMatch));
		}
		break;
	case MMATCH_GAMETYPE_BOUNTY:
		{
			return (new ZRuleBounty(pMatch));
		}
		break;
	case MMATCH_GAMETYPE_CTF:
		{
			return (new ZRuleTeamCTF(pMatch));
		}
		break;	
		case MMATCH_GAMETYPE_DEATHMATCH_TEAM_TRIPLE:	// 3 Team DM.
		{
			return (new ZRuleTeamDeathMatchTriple(pMatch));
		}
		break;
	default:
		{
			// 게임 타입이 없습니다.
			_ASSERT(0);
		}
	}
	return NULL;

}

bool ZRule::OnCommand(MCommand* pCommand)
{
	return false;
}

void ZRule::AfterCommandProcessed( MCommand* pCommand )
{

}

void ZRule::OnResponseRuleInfo(MTD_RuleInfo* pInfo)
{

}