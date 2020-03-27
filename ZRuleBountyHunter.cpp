#include "stdafx.h"
#include "ZRuleBountyHunter.h"

ZRuleBounty::ZRuleBounty(ZMatch* pMatch) : ZRule(pMatch), m_uidBounty(0,0)
{

}

ZRuleBounty::~ZRuleBounty()
{


}

bool ZRuleBounty::OnCommand(MCommand* pCommand)
{
	if (!ZGetGame()) return false;

	switch (pCommand->GetID())
	{
	case MC_MATCH_ASSIGN_BERSERKER:
		{
			MUID uidBounty;
			pCommand->GetParameter(&uidBounty,		0, MPT_UID);
			AssignBounty(uidBounty);
		}
		break;
	}
	return false;
}

void ZRuleBounty::OnResponseRuleInfo(MTD_RuleInfo* pInfo)
{
	MTD_RuleInfo_Bounty* pBountyRule = (MTD_RuleInfo_Bounty*)pInfo;
	AssignBounty(pBountyRule->uidBounty);
}

void ZRuleBounty::onDraw(MDrawContext* pDC) 
{
	//ENCODE_START
	ZRuleBounty* pRule = (ZRuleBounty*)ZGetGame()->GetMatch()->GetRule();
	if(pRule) {
		for(ZCharacterManager::iterator itor = ZGetGame()->m_CharacterManager.begin(); itor != ZGetGame()->m_CharacterManager.end(); ++itor)
		{
			rvector pos, screen_pos;
			ZCharacter* pCharacter = (*itor).second;
			if (!pCharacter->IsVisible()) continue;
			if (pCharacter->IsDie()) continue;
			if (pCharacter->GetUID() != pRule->GetBountyUID()) continue;
			pos = pCharacter->GetPosition();
			RVisualMesh* pVMesh = pCharacter->m_pVMesh;
			RealSpace2::rboundingbox box;

			if (pVMesh == NULL) continue;
		
			box.vmax = pos + rvector(50.f, 50.f, 190.f);
			box.vmin = pos + rvector(-50.f, -50.f, 0.f);

			if (isInViewFrustum(&box, RGetViewFrustum()))
			{
				// 미니맵이면 z 값을 0에 맞춘다
				if(ZGetCamera()->GetLookMode()==ZCAMERA_MINIMAP) {
					rvector pos = pCharacter->GetPosition();	//mmemory proxy
					pos.z=0;
					screen_pos = RGetTransformCoord(pos);
				}else
					screen_pos = RGetTransformCoord(pCharacter->GetVisualMesh()->GetHeadPosition(!bMemEdited)+rvector(0,0,30.f));

				MFont *pFont=NULL;
				//Steven: Ugrade Unmask
				MCOLOR CharNameColor;
				ZGetGame()->GetUserGradeIDColor(pCharacter->GetUserGrade(), CharNameColor, "");
				pFont = MFontManager::Get("FONTa12_O1Blr");
				pDC->SetColor(CharNameColor);
				pDC->SetBitmap(NULL);
				if (pFont == NULL) _ASSERT(0);
				pDC->SetFont(pFont);
				int x = screen_pos.x - pDC->GetFont()->GetWidth(pCharacter->GetUserName()) / 2;
				char Buffer[60];
				sprintf(Buffer, "[Bounty]%s", pCharacter->GetUserName());
				pDC->Text(x, screen_pos.y - 12, Buffer);
			}
		}
	}
	//ENCODE_END
}

void ZRuleBounty::AssignBounty(MUID& uidBounty)
{
	if (!ZGetGame()) return;

	for (ZCharacterManager::iterator itor = ZGetGame()->m_CharacterManager.begin();
		itor != ZGetGame()->m_CharacterManager.end(); ++itor)
	{
		ZCharacter* pCharacter = (*itor).second;
		pCharacter->SetTagger(false);
	}

	ZCharacter* pBountyChar = ZGetGame()->m_CharacterManager.Find(uidBounty);
	if (pBountyChar)
	{
		ZGetEffectManager()->AddBerserkerIcon(pBountyChar, 2);
		pBountyChar->SetTagger(true);
		if (!pBountyChar->IsDie())
		{
			float fMaxHP = pBountyChar->GetMaxHP();
			float fMaxAP = pBountyChar->GetMaxAP();

			pBountyChar->SetHP(fMaxHP);
			pBountyChar->SetAP(fMaxAP);


			if ( uidBounty == ZGetMyUID())
				ZGetGameInterface()->PlayVoiceSound( VOICE_GOT_BOUNTY, 3000);
			else
				ZGetGameInterface()->PlayVoiceSound( VOICE_BOUNTY_DOWN, 1900);
		}

	}

	m_uidBounty = uidBounty;
	m_fElapsedHealthUpdateTime = 0.0f;
}