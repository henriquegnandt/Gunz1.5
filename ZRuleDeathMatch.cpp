#include "stdafx.h"
#include "ZRuleDeathMatch.h"
#include "ZScreenEffectManager.h"
#include "MMatchUtil.h"
ZRuleSoloGunGame::ZRuleSoloGunGame(ZMatch* pMatch) : ZRule(pMatch)
{
		ZGetScreenEffectManager()->CreateQuestRes();
		TopPlayer = MUID(0,0);
}

void ZRuleSoloGunGame::DrawArrow()
{
	if(ZGetMyUID() == TopPlayer) return;
	ZCharacter* nub = ZGetGame()->m_CharacterManager.Find(TopPlayer);
	if(nub)
	{
		rvector Pos = nub->GetPosition();
		ZGetScreenEffectManager()->DrawArrow(Pos, 240.0f);
	}
}

bool ZRuleSoloGunGame::OnCommand(MCommand* pCommand)
{
	if (!ZGetGame()) return false;
	switch (pCommand->GetID())
	{
	case MC_MATCH_PEER_TOP_PLAYER:
		{	
			pCommand->GetParameter(&TopPlayer, 0, MPT_UID);			
		}
		break;
	case MC_MATCH_PEER_GUNGAME_SCORE:
		{
			MUID Player;
			int Type = 0;
			pCommand->GetParameter(&Player, 0, MPT_UID);
			pCommand->GetParameter(&Type, 1, MPT_INT);
			ZCharacter* pCharacter = ZGetCharacterManager()->Find(Player);
			if(pCharacter)
			{
				pCharacter->GetStatus().CheckCrc();
				switch(Type)
				{
				case 0:
					pCharacter->GetStatus().Ref().AddWepCount();
					break;
				case 1:
					if(pCharacter->GetStatus().Ref().nWepCount != 0)
					{
						pCharacter->GetStatus().Ref().AddSetbacks();
						pCharacter->GetStatus().Ref().MinusWepCount();
					}
					break;
				}
				pCharacter->GetStatus().MakeCrc();
			}
		}
		break;
	case MC_MATCH_PEER_WEAPON_CHANGE:
		{
			int itemid = 0;
			MUID Player;
			pCommand->GetParameter(&itemid, 0, MPT_INT);
			pCommand->GetParameter(&Player, 1, MPT_UID);
			ZCharacter* pChar = ZGetGame()->m_CharacterManager.Find(Player);
			if(pChar)
			{
				pChar->GetItems()->EquipItem(MMCIP_PRIMARY, itemid);
				pChar->ChangeWeapon(MMCIP_PRIMARY, false);
				pChar->InitItemBullet();
			}
		}
		break;
	}
	return false;
}

ZRuleSoloGunGame::~ZRuleSoloGunGame()
{
		ZGetScreenEffectManager()->DestroyQuestRes();
}

ZRuleSoloDeathMatch::ZRuleSoloDeathMatch(ZMatch* pMatch) : ZRule(pMatch)
{

}

ZRuleSoloDeathMatch::~ZRuleSoloDeathMatch()
{
}
/////////////////////////////////////////////////////////////////////////////////////////

ZRuleTeamDeathMatch::ZRuleTeamDeathMatch(ZMatch* pMatch) : ZRule(pMatch)
{

}

ZRuleTeamDeathMatch::~ZRuleTeamDeathMatch()
{

}

ZRuleTeamDeathMatch3::ZRuleTeamDeathMatch3(ZMatch* pMatch) : ZRule(pMatch)
{

}

ZRuleTeamDeathMatch3::~ZRuleTeamDeathMatch3()
{

}
/*
/////////////////////////////////////////////////////////////////////////////////////////

ZRuleTrippleTeamDeathMatch::ZRuleTrippleTeamDeathMatch(ZMatch* pMatch) : ZRule(pMatch)
{

}

ZRuleTrippleTeamDeathMatch::~ZRuleTrippleTeamDeathMatch()
{

}
*/
/////////////////////////////////////////////////////////////////////////////////////////

ZRuleTeamDeathMatch2::ZRuleTeamDeathMatch2(ZMatch* pMatch) : ZRule(pMatch)
{

}

ZRuleTeamDeathMatch2::~ZRuleTeamDeathMatch2()
{

}

bool ZRuleTeamDeathMatch2::OnCommand(MCommand* pCommand)
{
	if (!ZGetGame()) return false;

	switch (pCommand->GetID())
	{

	case MC_MATCH_GAME_DEAD:
		{
			OnGameDeath GameDeath;
			pCommand->GetParameter(&GameDeath,	0, MPT_BLOB);

			ZCharacter* pAttacker = ZGetGame()->m_CharacterManager.Find(GameDeath.Attacker);
			ZCharacter* pVictim = ZGetGame()->m_CharacterManager.Find(GameDeath.Victim);
			
			m_pMatch->AddTeamKills(pVictim->GetTeamID() == MMT_BLUE ? MMT_RED : MMT_BLUE);
		}
		break;
	}

	return false;
}
ZRuleTeamCTF::ZRuleTeamCTF(ZMatch* pMatch) : ZRule(pMatch)
{
	ZeroMemory(m_BlueFlagPos, sizeof(m_BlueFlagPos));
	ZeroMemory(m_RedFlagPos, sizeof(m_RedFlagPos));
	m_bRedFlagTaken = 0;
	m_bBlueFlagTaken = 0;
	ZGetScreenEffectManager()->CreateQuestRes();
}

ZRuleTeamCTF::~ZRuleTeamCTF()
{
	ZGetScreenEffectManager()->DestroyQuestRes();
}

bool ZRuleTeamCTF::OnCommand(MCommand* pCommand)
{
	if (!ZGetGame()) return false;

	switch (pCommand->GetID())
	{
		case MC_MATCH_FLAG_STATE:
		{
			int nItemID;
			MShortVector s_pos;
			int IsGone;
			MUID Carrier;
			pCommand->GetParameter(&nItemID, 0, MPT_INT);
			pCommand->GetParameter(&s_pos, 1, MPT_SVECTOR);
			pCommand->GetParameter(&IsGone, 2, MPT_INT);
			pCommand->GetParameter(&Carrier, 3, MPT_UID);
			switch(nItemID)
			{
			case CTF_RED_ITEM_ID:
				{
				SetRedCarrier(Carrier);
				SetRedFlagState(IsGone);
				SetRedFlagPos(rvector(s_pos.x, s_pos.y, s_pos.z));
				}
				break;
			case CTF_BLUE_ITEM_ID:
				{
				SetBlueCarrier(Carrier);
				SetBlueFlagState(IsGone);
				SetBlueFlagPos(rvector(s_pos.x, s_pos.y, s_pos.z));
				}
				break;
			}
		}
		break;

	case MC_MATCH_FLAG_CAP:
		{
			MMatchTeam nTeam;
			unsigned int Low;
			pCommand->GetParameter(&nTeam, 0, MPT_INT);
			pCommand->GetParameter(&Low, 1, MPT_UINT);
			m_pMatch->AddTeamKills(nTeam == MMT_BLUE ? MMT_BLUE : MMT_RED);
			ZCharacter* pCharacter = ZGetCharacterManager()->Find(MUID(0, Low));
			if(pCharacter)
			{
				pCharacter->GetStatus().CheckCrc();
				pCharacter->GetStatus().Ref().AddScores();
				pCharacter->GetStatus().MakeCrc();
				if(pCharacter->GetProperty() && ZGetGame()->m_pMyCharacter->GetTeamID() == nTeam)
				{
					char text[300];
					sprintf(text, "%s has scored!", pCharacter->GetProperty()->GetName());
					MLabel* pLab = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("CTFLine1");
					if(pLab) 
					{
						pLab->Show(true);
						pLab->SetText(text);
					}
				}
				if(pCharacter->GetProperty() && ZGetGame()->m_pMyCharacter->GetTeamID() != nTeam)
				{
					MLabel* pLab = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("CTFLine2");
					if(pLab) pLab->Show(false);
				}
			}
			if(nTeam == MMT_RED)
			{
				ZGetGameInterface()->PlayVoiceSound( VOICE_RED_TEAM_SCORE, 1600);
				ZGetScreenEffectManager()->AddScreenEffect("ctf_score_r");
				SetBlueFlagState(false);
				SetRedCarrier(MUID(0,0));					
				MLabel* pLab = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "CTFBlueSafe");
				if(pLab) pLab->Show(true);
				pLab = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "CTFBlueTaken");
				if(pLab) pLab->Show(false);
				pLab = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "CTFBlueTakenBy");
				if(pLab) pLab->Show(false);

			}
			else if(nTeam == MMT_BLUE)
			{
				ZGetGameInterface()->PlayVoiceSound( VOICE_BLUE_TEAM_SCORE, 1600);
				ZGetScreenEffectManager()->AddScreenEffect("ctf_score_b");
				SetBlueCarrier(MUID(0,0));
				SetRedFlagState(true);					
				MLabel* pLab = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "CTFRedSafe");
				if(pLab) pLab->Show(true);
				pLab = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "CTFRedTaken");
				if(pLab) pLab->Show(false);
				pLab = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "CTFRedTakenBy");
				if(pLab) pLab->Show(false);
			}

			for (ZCharacterManager::iterator itor = ZGetGame()->m_CharacterManager.begin();
			itor != ZGetGame()->m_CharacterManager.end(); ++itor)
			{
				ZCharacter* pCharacter = (*itor).second;
				if(pCharacter)
				{
					if(pCharacter->GetTeamID() == nTeam)
					{
						pCharacter->SetTagger(false);
					}
				}
			}
		}
		break;
	case MC_MATCH_FLAG_EFFECT:
		{
			MUID uidOwner;
			int nTeam;
			pCommand->GetParameter(&uidOwner,		0, MPT_UID);
			pCommand->GetParameter(&nTeam,		1, MPT_INT);
			ZCharacter* pCapper = ZGetGame()->m_CharacterManager.Find(uidOwner);
			if(pCapper)
			{
				if(!pCapper->IsDie())
				{
					AssignFlagEffect(uidOwner, nTeam);
				}
				else
				{
					if(ZGetGame()->m_pMyCharacter->GetTeamID() != nTeam)
					{
						MLabel* pLab = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("CTFLine2");
						if(pLab) pLab->Show(false);
					}
					if(ZGetGame()->m_pMyCharacter->GetTeamID() == nTeam)
					{
						MLabel* pLab = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("CTFLine1");
						if(pLab) pLab->Show(false);
					}
					for (ZCharacterManager::iterator itor = ZGetGame()->m_CharacterManager.begin();
					itor != ZGetGame()->m_CharacterManager.end(); ++itor)
					{
						ZCharacter* pCharacter = (*itor).second;
						if(pCharacter)
						{
							if(pCharacter->GetTeamID() == nTeam)
							{
								pCharacter->SetTagger(false);
							}
						}
					}
					if(nTeam == MMT_RED)
					{
						SetRedCarrier(MUID(0,0));
						SetBlueFlagState(false);
						ZGetScreenEffectManager()->AddScreenEffect("ctf_flagdrop_b");
						ZGetGameInterface()->PlayVoiceSound( VOICE_BLUE_FLAG_RETURN, 1600);
						MLabel* pLab = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "CTFBlueSafe");
						if(pLab) pLab->Show(true);
						pLab = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "CTFBlueTaken");
						if(pLab) pLab->Show(false);
						pLab = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "CTFBlueTakenBy");
						if(pLab) pLab->Show(false);
					}
					else if(nTeam == MMT_BLUE)
					{
						SetBlueCarrier(MUID(0,0));
						SetRedFlagState(false);
						ZGetScreenEffectManager()->AddScreenEffect("ctf_flagdrop_r");
						ZGetGameInterface()->PlayVoiceSound( VOICE_RED_FLAG_RETURN, 1600);
						MLabel* pLab = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "CTFRedSafe");
						if(pLab) pLab->Show(true);
						pLab = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "CTFRedTaken");
						if(pLab) pLab->Show(false);
						pLab = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "CTFRedTakenBy");
						if(pLab) pLab->Show(false);
					}
				}
			}
			else
			{
			if(ZGetGame()->m_pMyCharacter->GetTeamID() != nTeam)
			{
				MLabel* pLab = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("CTFLine2");
				if(pLab) pLab->Show(false);
			}
			if(ZGetGame()->m_pMyCharacter->GetTeamID() == nTeam)
					{
						MLabel* pLab = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("CTFLine1");
						if(pLab) pLab->Show(false);
					}
					for (ZCharacterManager::iterator itor = ZGetGame()->m_CharacterManager.begin();
					itor != ZGetGame()->m_CharacterManager.end(); ++itor)
					{
						ZCharacter* pCharacter = (*itor).second;
						if(pCharacter)
						{
							if(pCharacter->GetTeamID() == nTeam)
							{
								pCharacter->SetTagger(false);
							}
						}
					}
					if(nTeam == MMT_RED)
					{
					SetRedCarrier(MUID(0,0));
					SetBlueFlagState(false);
					ZGetScreenEffectManager()->AddScreenEffect("ctf_flagdrop_b");
					ZGetGameInterface()->PlayVoiceSound( VOICE_BLUE_FLAG_RETURN, 1600);
					MLabel* pLab = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "CTFBlueSafe");
					if(pLab) pLab->Show(true);
					pLab = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "CTFBlueTaken");
					if(pLab) pLab->Show(false);
					pLab = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "CTFBlueTakenBy");
					if(pLab) pLab->Show(false);
					}
					else if(nTeam == MMT_BLUE)
					{
					SetBlueCarrier(MUID(0,0));
					SetRedFlagState(false);
					ZGetScreenEffectManager()->AddScreenEffect("ctf_flagdrop_r");
					ZGetGameInterface()->PlayVoiceSound( VOICE_RED_FLAG_RETURN, 1600);					
					MLabel* pLab = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "CTFRedSafe");
					if(pLab) pLab->Show(true);
					pLab = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "CTFRedTaken");
					if(pLab) pLab->Show(false);
					pLab = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "CTFRedTakenBy");
					if(pLab) pLab->Show(false);
					}
			}
		}
		break;
	}

	return false;
}

void ZRuleTeamCTF::AssignFlagEffect(MUID& uidOwner, int nTeam)
{
	if (!ZGetGame()) return;

	for (ZCharacterManager::iterator itor = ZGetGame()->m_CharacterManager.begin();
	itor != ZGetGame()->m_CharacterManager.end(); ++itor)
	{
		ZCharacter* pCharacter = (*itor).second;
		if(pCharacter)
		{
			if(pCharacter->GetTeamID() == nTeam)
			{
				pCharacter->SetTagger(false);
			}
		}
	}
	ZCharacter* pFlagChar = ZGetGame()->m_CharacterManager.Find(uidOwner);
	if (pFlagChar)
	{		
		pFlagChar->SetTagger(true);
		if(pFlagChar->GetProperty() && ZGetGame()->m_pMyCharacter->GetTeamID() == nTeam)
		{
			char text[300];
			sprintf(text, "%s has captured the flag assist him!", pFlagChar->GetProperty()->GetName());
			MLabel* pLab = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("CTFLine1");
			if(pLab) 
			{
				pLab->Show(true);
				pLab->SetText(text);
			}
		}
		if(pFlagChar->GetProperty() && ZGetGame()->m_pMyCharacter->GetTeamID() != nTeam)
		{
			char text[300];
			sprintf(text, "%s has our flag Eliminate him!", pFlagChar->GetProperty()->GetName());
			MLabel* pLab = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("CTFLine2");
			if(pLab) 
			{
				pLab->Show(true);
				pLab->SetText(text);
			}
		}
		if(nTeam == MMT_BLUE)
		{
			ZGetEffectManager()->AddFlagOnBack(pFlagChar, 0);
			SetBlueCarrier(uidOwner);
			SetRedFlagState(true);
			ZGetScreenEffectManager()->AddScreenEffect("ctf_taken_r");
			ZGetGameInterface()->PlayVoiceSound( VOICE_BLUE_HAS_FLAG, 1600);
			MLabel* pLab = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "CTFRedSafe");
			if(pLab) pLab->Show(false);
			pLab = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "CTFRedTaken");
			if(pLab) pLab->Show(true);
			pLab = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "CTFRedTakenBy");
			if(pLab)
			{
				pLab->Show(true);
				if(pFlagChar->GetProperty())
					pLab->SetText(pFlagChar->GetProperty()->GetName());
			}
		}
		else if (nTeam == MMT_RED)
		{
			ZGetEffectManager()->AddFlagOnBack(pFlagChar, 1);
			SetRedCarrier(uidOwner);
			SetBlueFlagState(true);
			ZGetScreenEffectManager()->AddScreenEffect("ctf_taken_b");
			ZGetGameInterface()->PlayVoiceSound( VOICE_RED_HAS_FLAG, 1600);
			MLabel* pLab = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "CTFBlueSafe");
			if(pLab) pLab->Show(false);
			pLab = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "CTFBlueTaken");
			if(pLab) pLab->Show(true);
			pLab = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "CTFBlueTakenBy");
			if(pLab)
			{
				pLab->Show(true);
				if(pFlagChar->GetProperty())
					pLab->SetText(pFlagChar->GetProperty()->GetName());
			}
		}
	}
}

void ZRuleTeamCTF::DrawArrow()
{ 
	int screenx=0;
	switch(MGetWorkspaceWidth())
	{
	case 640:
		 screenx=0.18*MGetWorkspaceWidth();
		break;
	case 720:
		 if(MGetWorkspaceHeight() == 480)
			screenx=0.18*MGetWorkspaceWidth();
		 else
			screenx=0.15*MGetWorkspaceWidth();
		break;
	case 800:
			screenx=0.14*MGetWorkspaceWidth();
		break;
	case 1024:
		screenx=0.11*MGetWorkspaceWidth();
		break;
	case 1280:
		 if(MGetWorkspaceHeight() == 720)
			screenx=0.12*MGetWorkspaceWidth();
		 else
			screenx=0.11*MGetWorkspaceWidth();
		break;
	case 1360:
			screenx=0.11*MGetWorkspaceWidth();
		break;
	default:
			screenx=0.13*MGetWorkspaceWidth();
		break;
	}
	 
	int Team = 0;if(ZGetGame()->m_pMyCharacter->GetTeamID() == MMT_BLUE)Team = 0;else if(ZGetGame()->m_pMyCharacter->GetTeamID() == MMT_RED)Team = 1;
	if((Team ? GetRedCarrier() : GetBlueCarrier()) == ZGetGame()->m_pMyCharacter->GetUID() && (Team ? GetRedFlagState() : GetBlueFlagState()) == 0)
			ZGetScreenEffectManager()->DrawArrow((Team ? GetRedFlagPos() : GetBlueFlagPos()), 240.0f, screenx, Team);
		else if ((Team ? GetRedFlagState() : GetBlueFlagState()) == 1)
		{
			ZCharacter* nub = ZGetGame()->m_CharacterManager.Find((Team ? GetBlueCarrier() : GetRedCarrier()));
			if(nub)
			{
				rvector Pos = nub->GetPosition();
				ZGetScreenEffectManager()->DrawArrow(Pos, 240.0f, screenx, Team ? 0 : 1);
			}
		}
		else if((Team ? GetRedFlagState() : GetBlueFlagState()) == 0)
			ZGetScreenEffectManager()->DrawArrow((Team ? GetBlueFlagPos() : GetRedFlagPos()), 240.0f, screenx, Team);
}

/*
 * Npc TDM Extream
 */


ZRuleTeamDeathMatch2Npc::ZRuleTeamDeathMatch2Npc(ZMatch* pMatch) : ZRule(pMatch)
{
}

ZRuleTeamDeathMatch2Npc::~ZRuleTeamDeathMatch2Npc()
{
}

// 3 Team DM.
// ------------------------------------------------------------------------------------------
ZRuleTeamDeathMatchTriple::ZRuleTeamDeathMatchTriple(ZMatch* pMatch) : ZRule(pMatch)
{

}

ZRuleTeamDeathMatchTriple::~ZRuleTeamDeathMatchTriple()
{

}