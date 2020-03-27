#include "stdafx.h"
#include "ZRuleCTF.h"
ZRuleCTF::ZRuleCTF(ZMatch* pMatch) : ZRule(pMatch)
{

}

ZRuleCTF::~ZRuleCTF()
{

}

bool ZRuleCTF::OnCommand(MCommand* pCommand)
{
	if (!ZGetGame()) return false;

	switch (pCommand->GetID())
	{

	case MC_MATCH_GAME_DEAD:
		{
			/*MUID uidAttacker, uidVictim;

			pCommand->GetParameter(&uidAttacker, 0, MPT_UID);
			pCommand->GetParameter(&uidVictim, 2, MPT_UID);

			ZCharacter* pAttacker = ZGetGame()->m_CharacterManager.Find(uidAttacker);
			ZCharacter* pVictim = ZGetGame()->m_CharacterManager.Find(uidVictim);
			
			m_pMatch->AddTeamKills(pVictim->GetTeamID() == MMT_BLUE ? MMT_RED : MMT_BLUE);*/
		}
		break;
	}

	return false;
}
