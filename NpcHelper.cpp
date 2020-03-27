#include "stdafx.h"
#include "ZNpcHelper.h"
#include "ZCommandTable.h"
#include "ZGlobal.h"
#include "ZScreenEffectManager.h"
#include "RNavigationMesh.h"
#include "RNavigationNode.h"
#include "MBaseQuest.h"
bool ZNpcHelper::OnGameCommand(MCommand* pCommand)
{
	if (!ZGetGame()) return false;

	switch (pCommand->GetID())
	{
	case MC_QUEST_NPC_SPAWN:
		{
			if (ZGetGame() == NULL) return false;
			MUID uidChar, uidNPC;
			unsigned char nNPCType, nPositionIndex;

			pCommand->GetParameter(&uidChar,			0, MPT_UID);
			pCommand->GetParameter(&uidNPC,				1, MPT_UID);
			pCommand->GetParameter(&nNPCType,			2, MPT_UCHAR);
			pCommand->GetParameter(&nPositionIndex,		3, MPT_UCHAR);


			MQUEST_NPC NPCType = MQUEST_NPC(nNPCType);

			ZMapSpawnType nSpawnType = ZMST_NPC_MELEE;

			ZMapSpawnManager* pMSM = ZGetGame()->GetMapDesc()->GetSpawnManager();
			
			MQuestNPCInfo* pNPCInfo = m_NPCCatalogue.GetInfo(NPCType);
			if (pNPCInfo == NULL) return false;

			switch (pNPCInfo->GetSpawnType())
			{
			case MNST_MELEE: nSpawnType = ZMST_NPC_MELEE; break;
			case MNST_RANGE: nSpawnType = ZMST_NPC_RANGE; break;
			case MNST_BOSS: nSpawnType = ZMST_NPC_BOSS; break;
			default: _ASSERT(0);
			};

			int TeamIndex = nPositionIndex;

			ZMapSpawnData* pSpawnData = ZGetGame()->GetMapDesc()->GetSpawnManager()->GetTeamData(TeamIndex, 0);//new ZMapSpawnData();
			pSpawnData->m_nType = ZMST_NPC_MELEE;
			sprintf(pSpawnData->m_szSpawnName, "spawn_npc_melee_0%d", TeamIndex);
			//pSpawnData->m_Pos = ZGetGame()->GetMapDesc()->GetSpawnManager()->GetTeamData(TeamIndex, 0)->m_Pos;
			MMatchTeam t = TeamIndex == 0 ? MMT_RED : MMT_BLUE;
			rvector NPCPos = rvector(0,0,0);
			rvector NPCDir = rvector(1,0,0);

			if (pSpawnData)
			{
				NPCPos = pSpawnData->m_Pos;
				NPCDir = pSpawnData->m_Dir;
			}
			RMesh* pNPCMesh = ZGetNpcMeshMgr()->Get(pNPCInfo->szMeshName);
			if (pNPCMesh)
			{
				if (!pNPCMesh->m_isMeshLoaded)
				{
					ZGetNpcMeshMgr()->Load(pNPCInfo->szMeshName);
					ZGetNpcMeshMgr()->ReloadAllAnimation();
				}
			}

			ZActor* pNewActor = ZActor::CreateActor(NPCType, 1.0f, 0, t);
			if (pNewActor)
			{
				pNewActor->SetUID(uidNPC);
				pNewActor->SetPosition(NPCPos);
				pNewActor->SetDirection(NPCDir);
				bool bMyControl = (uidChar == ZGetGameClient()->GetPlayerUID());
				pNewActor->SetMyControl(bMyControl);
		
				ZCharacter *pOwner = ZGetCharacterManager()->Find(uidChar);
				if(pOwner)
					pNewActor->SetOwner(pOwner->GetUserName());

				if(pNewActor->m_pVMesh) {
		
					D3DCOLORVALUE color;

					color.r = pNPCInfo->vColor.x;
					color.g = pNPCInfo->vColor.y;
					color.b = pNPCInfo->vColor.z;
					color.a = 1.f;

					pNewActor->m_pVMesh->SetNPCBlendColor(color);//색을 지정한 경우..
				}

				ZGetObjectManager()->Add(pNewActor);
				ZGetEffectManager()->AddReBirthEffect(NPCPos);
			}
		}
		break;

	case MC_QUEST_NPC_DEAD:
		{
			MUID uidPlayer, uidNPC;
			pCommand->GetParameter(&uidPlayer,	0, MPT_UID);
			pCommand->GetParameter(&uidNPC,		1, MPT_UID);
			ZActor* pActor = ZGetObjectManager()->GetNPCObject(uidNPC);
			if (pActor)
			{
				ZGetGame()->GetMatch()->AddTeamKills(pActor->GetTeam() == MMT_BLUE ? MMT_RED : MMT_BLUE);
				pActor->OnDie();
				ZGetObjectManager()->Delete(pActor);
			}
		}
		break;
	}
	return false;
}

ZNpcHelper::ZNpcHelper() : m_bLoaded(false), m_bCreatedOnce(false)
{

}

ZNpcHelper::~ZNpcHelper()
{
	OnDestroyOnce();
}

bool ZNpcHelper::OnCreate()
{
	m_fLastWeightTime = 0.0f;
	LoadNPCMeshes();
	LoadNPCSounds();

	return ZGetScreenEffectManager()->CreateQuestRes();
}

bool ZNpcHelper::OnCreateOnce()
{
	if (m_bCreatedOnce) return true;

	m_bCreatedOnce = true;
	return Load();
}

void ZNpcHelper::OnDestroyOnce()
{
	m_bCreatedOnce = false;
}

bool ZNpcHelper::Load()
{
	if (m_bLoaded) return true;
	string strFileNameZNPC(FILENAME_ZNPC_DESC);
#ifndef _DEBUG
	strFileNameZNPC += "";	
#endif	
	if (!m_NPCCatalogue.ReadXml(ZApplication::GetFileSystem(), strFileNameZNPC.c_str()))
	{
		mlog("Error while Read Item Descriptor %s", strFileNameZNPC);
		return false;
	}
//	ProcessNPCDropTableMatching();
	RAniEventMgr* AniEventMgr = ZGetAniEventMgr();
	if (!AniEventMgr->ReadXml(ZApplication::GetFileSystem(), "System/AnimationEvent.xml"))
	{
		mlog("Read Animation Event Failed");
		return false;
	}
	m_bLoaded = true;
	return true;
}

void ZNpcHelper::Reload()
{
	m_bLoaded = false;
#ifdef _DEBUG
	ZApplication::GetSkillManager()->Destroy();
	ZApplication::GetSkillManager()->Create();
#endif
	m_NPCCatalogue.Clear();

	RAniEventMgr* AniEventMgr = ZGetAniEventMgr();
	AniEventMgr->Destroy();
	Load();
}

void ZNpcHelper::OnGameCreate()
{
	OnCreateOnce();
}

void ZNpcHelper::OnGameDestroy()
{
}


void ZNpcHelper::OnGameUpdate(float fElapsed)
{
	UpdateNavMeshWeight(fElapsed);

}

void ZNpcHelper::UpdateNavMeshWeight(float fDelta)
{
	// NavMesh 가중치 업데이트
	if ((ZGetGame()->GetTime() - m_fLastWeightTime) >= 1.0f)
	{
#ifdef _DEBUG
		unsigned long int nLastTime = timeGetTime();
#endif

		RNavigationMesh* pNavMesh = ZGetGame()->GetWorld()->GetBsp()->GetNavigationMesh();		
		if (pNavMesh != NULL)
		{
			pNavMesh->ClearAllNodeWeight();
			ZNPCObjectMap* pNPCObjectMap = ZGetObjectManager()->GetNPCObjectMap();
			for(ZNPCObjectMap::iterator i = pNPCObjectMap->begin();i!=pNPCObjectMap->end();i++)
			{
				ZObject* pNPCObject = i->second;
				RNavigationNode* pNode = pNavMesh->FindClosestNode(pNPCObject->GetPosition());
				if (pNode)
				{
					float fWeight = pNode->GetWeight() + 1.0f;
					pNode->SetWeight(fWeight);
				}
			}
		}
		m_fLastWeightTime = ZGetGame()->GetTime();

	}

}
void ZNpcHelper::LoadNPCMeshes()
{
#ifdef _DEBUG
	// 혼자서 AI 테스트할 경우
	if ((ZApplication::GetInstance()->GetLaunchMode() == ZApplication::ZLAUNCH_MODE_STANDALONE_QUEST) || 
		(ZApplication::GetInstance()->GetLaunchMode() == ZApplication::ZLAUNCH_MODE_STANDALONE_AI))
	{
		ZGetNpcMeshMgr()->LoadAll();

		ZGetNpcMeshMgr()->ReloadAllAnimation();// 읽지 않은 에니메이션이 있다면 로딩
		return;
	}
#endif
	for (map<MQUEST_NPC, MQuestNPCInfo*>::iterator itor = m_NPCCatalogue.begin();  itor != m_NPCCatalogue.end();  itor++)
	{
		ZGetNpcMeshMgr()->Load(itor->second->szMeshName);
	}

	ZGetNpcMeshMgr()->ReloadAllAnimation();// 읽지 않은 에니메이션이 있다면 로딩
}
	
void ZNpcHelper::LoadNPCSounds()
{

	ZSoundEngine* pSE = ZApplication::GetSoundEngine();
	if (pSE == NULL) return;

		int nCount = 0;
	for (map<MQUEST_NPC, MQuestNPCInfo*>::iterator itor = m_NPCCatalogue.begin();  itor != m_NPCCatalogue.end();  itor++)
	{
		if (!pSE->LoadNPCResource(itor->first))
		{
			mlog("failed npc sound\n");
		}
	}
}
void ZNpcHelper::OnDestroy()
{
	ZGetNpcMeshMgr()->UnLoadAll();
	ZGetScreenEffectManager()->DestroyQuestRes();
}