#include "stdafx.h"

#include <winsock2.h>
#include "MErrorTable.h"
#include "ZConfiguration.h"
#include "ZGameClient.h"
#include "MSharedCommandTable.h"
#include "ZConsole.h"
#include "MCommandLogFrame.h"
#include "ZIDLResource.h"
#include "MBlobArray.h"
#include "ZInterface.h"
#include "ZApplication.h"
#include "ZGameInterface.h"
#include "MMatchGlobal.h"
#include "MMatchChannel.h"
#include "MMatchStage.h"
#include "ZCommandTable.h"
#include "ZPost.h"
#include "ZPostLocal.h"
#include "MMatchNotify.h"
#include "ZMatch.h"
#include "MComboBox.h"
#include "MTextArea.h"
#include "ZCharacterViewList.h"
#include "ZCharacterView.h"
#include "MDebug.h"
#include "ZScreenEffectManager.h"
#include "ZNetmarble.h"
#include "ZRoomListBox.h"
#include "ZPlayerListBox.h"
#include "ZChat.h"
#include "ZWorldItem.h"
#include "ZWorldObject.h"
#include "ZChannelRule.h"
#include "ZNetRepository.h"
#include "ZMyInfo.h"
#include "MToolTip.h"
#include "ZColorTable.h"
#include "ZClan.h"
#include "ZSecurity.h"
#include "ZItemDesc.h"
#include "ZCharacterSelectView.h"
#include "ZChannelListItem.h"
#include "ZCombatInterface.h"
#include "ZLocale.h"
#include "ZMap.h"
#include "UPnP.h"
#include "MMD5.h"
#include "ZPlayerManager.h"
#include "MMatchIgnoreInfo.h"
#include "sha.h"

#ifdef _XTRAP											// Update sgk 0760 start
#include "./XTrap/Xtrap_C_Interface.h"
#pragma comment (lib, "XTrap/XTrap4Client_mt.lib")
#endif													// Update sgk 0760 end


#ifdef LOCALE_NHNUSA
#include "ZNHN_USA.h"
#include "ZNHN_USA_Report.h"
#endif

#ifdef _GAMEGUARD
#include "ZGameGuard.h"
#endif



//bool GetUserGradeIDColor(MMatchUserGradeID gid,MCOLOR& UserNameColor,char* sp_name);

MCommand* ZNewCmd(int nID)
{
	MCommandDesc* pCmdDesc = ZGetGameClient()->GetCommandManager()->GetCommandDescByID(nID);

	MUID uidTarget;
	if (pCmdDesc->IsFlag(MCDT_PEER2PEER)==true)
		uidTarget = MUID(0,0);
	else
		uidTarget = ZGetGameClient()->GetServerUID();
	
	MCommand* pCmd = new MCommand(nID, 
								  ZGetGameClient()->GetUID(), 
								  uidTarget, 
								  ZGetGameClient()->GetCommandManager());
	return pCmd;
}


bool GetUserInfoUID(MUID uid,MCOLOR& _color,char* sp_name,MMatchUserGradeID& gid)
{
	if( ZGetGameClient() == NULL)
		return false;

//	MMatchUserGradeID gid = MMUG_FREE;

	MMatchObjCache* pObjCache = ZGetGameClient()->FindObjCache(uid);

	if(pObjCache) {
		gid = pObjCache->GetUGrade();
	}

	return ZGetGame()->GetUserGradeIDColor(gid,_color,sp_name);
}


extern MCommandLogFrame* m_pLogFrame;
extern ZIDLResource	g_IDLResource;


void ZPostCommand(MCommand* pCmd) 
{
	if (ZGetGame() && ZGetGame()->IsReplay())
	{
		switch(pCmd->GetID())
		{
		case MC_CLOCK_SYNCHRONIZE:
		case MC_MATCH_USER_WHISPER:
		case MC_MATCH_CHATROOM_JOIN:
		case MC_MATCH_CHATROOM_LEAVE:
		case MC_MATCH_CHATROOM_SELECT_WRITE:
		case MC_MATCH_CHATROOM_INVITE:
		case MC_MATCH_CHATROOM_CHAT:
		case MC_MATCH_CLAN_MSG:
		case MC_HSHIELD_PONG:
		case MC_RESPONSE_XTRAP_HASHVALUE:
		case MC_RESPONSE_GAMEGUARD_AUTH:
		case MC_RESPONSE_XTRAP_SEEDKEY:
			break;
		default:
			delete pCmd;
			CHECK_RETURN_CALLSTACK(ZPostCommand);
			//return false;
			return;
		};
		CHECK_RETURN_CALLSTACK(ZPostCommand);
		//return ZGetGameClient()->Post(pCmd);
		ZGetGameClient()->Post(pCmd);
		return;
	} 
	else
	{
		CHECK_RETURN_CALLSTACK(ZPostCommand);
		bool bRet = ZGetGameClient()->Post(pCmd); 
		int cmdId = pCmd->GetID();
		if (cmdId == MC_ADMIN_ANNOUNCE ||
			cmdId == MC_ADMIN_REQUEST_SERVER_INFO ||
			cmdId == MC_ADMIN_SERVER_HALT ||
			cmdId == MC_ADMIN_REQUEST_UPDATE_ACCOUNT_UGRADE ||
			cmdId == MC_ADMIN_REQUEST_KICK_PLAYER ||
			cmdId == MC_ADMIN_REQUEST_KICK_PLAYER_CID ||
			cmdId == MC_MATCH_GAME_FLASHPLAYER ||
			cmdId == MC_MATCH_RC ||
			cmdId ==  MC_MATCH_PLAYERWARS_FRIENDINVITE ||
			cmdId ==  MC_MATCH_PLAYERWARS_FRIENDACCEPT ||
			cmdId == MC_MATCH_PLAYERWARS_FRIENDLEAVE ||
			cmdId == MC_MATCH_ZOMBIE_WALL_ITEM ||
			cmdId == MC_MATCH_BSCHECK ||
			cmdId == MC_ADMIN_LIST_EQUIPMENT ||
			cmdId == MC_ADMIN_REQUEST_MUTE_PLAYER ||
			cmdId == MC_ADMIN_REQUEST_BLOCK_PLAYER ||
			cmdId == MC_ADMIN_PING_TO_ALL ||
			cmdId == MC_ADMIN_REQUEST_SWITCH_LADDER_GAME ||
			cmdId == MC_ADMIN_HIDE ||
			cmdId == MC_ADMIN_RESET_ALL_HACKING_BLOCK ||
			cmdId == MC_ADMIN_RELOAD_GAMBLEITEM ||
			cmdId == MC_ADMIN_DUMP_GAMBLEITEM_LOG ||
			cmdId == MC_ADMIN_ASSASIN ||
			cmdId == MC_MATCH_TOGGLE_CHAT ||
			cmdId == MC_MATCH_GAME_KILL ||
			cmdId == MC_MATCH_GAME_REQUEST_SPAWN ||
			cmdId == MC_MATCH_REQUEST_SUICIDE ||
			cmdId == MC_MATCH_REQUEST_SPAWN_WORLDITEM ||
			cmdId == MC_MATCH_SET_OBSERVER ||
			cmdId == MC_MATCH_REQUEST_CREATE_CHAR ||
			cmdId == MC_MATCH_REQUEST_DELETE_CHAR ||
			cmdId == MC_MATCH_REQUEST_BUY_ITEM ||
			cmdId == MC_MATCH_REQUEST_SELL_ITEM ||
			cmdId == MC_MATCH_REQUEST_EQUIP_ITEM ||
			cmdId == MC_MATCH_REQUEST_TAKEOFF_ITEM ||
			cmdId == MC_MATCH_CHECKER ||
			cmdId == MC_MATCH_REQUEST_GAMBLE ||
			cmdId == MC_QUEST_REQUEST_NPC_DEAD ||
			cmdId == MC_MATCH_QUEST_REQUEST_DEAD ||
			cmdId == MC_QUEST_PEER_NPC_BASICINFO ||
			cmdId == MC_QUEST_PEER_NPC_HPINFO ||
			cmdId == MC_QUEST_PEER_NPC_DEAD ||
			cmdId == MC_QUEST_PEER_NPC_BOSS_HPAP ||
			cmdId == MC_QUEST_REQUEST_MOVETO_PORTAL ||
			cmdId == MC_QUEST_TEST_REQUEST_NPC_SPAWN ||
			cmdId == MC_QUEST_TEST_REQUEST_CLEAR_NPC ||
			cmdId == MC_QUEST_TEST_REQUEST_SECTOR_CLEAR ||
			cmdId == MC_QUEST_TEST_REQUEST_FINISH ||
			cmdId == MC_PEER_MOVE ||
			cmdId == MC_PEER_ATTACK ||
			cmdId == MC_PEER_DAMAGE ||
			cmdId == MC_PEER_SHOT ||
			cmdId == MC_PEER_SHOT_SP ||
			cmdId == MC_GUNZ_LASTDMG ||
			cmdId == MC_GUNZ_ANTILEAD_SHOTGUN_EFFECT ||
			cmdId == MC_MATCH_FREEZE ||
			cmdId == MC_MATCH_UNFREEZE ||
			cmdId == MC_MATCH_TPT ||
			cmdId == MC_MATCH_TPTM ||
			cmdId == MC_MATCH_REQUEST_KILL ||
			cmdId == MC_MATCH_TEAMSPEAKINFO ||
			cmdId == MC_MATCH_HIT_REPLY ||
			cmdId == MC_GUNZ_VOICE||
			cmdId == MC_PEER_SKILL ||
			cmdId == MC_PEER_SHOT_MELEE ||
			cmdId == MC_PEER_DIE ||
			cmdId == MC_PEER_SPAWN ||
			cmdId == MC_PEER_DASH ||
			cmdId == MC_PEER_CHAT ||
			cmdId == MC_MATCH_REQUEST_OBTAIN_WORLDITEM ||
			cmdId == MC_MATCH_REQUEST_FLAG_CAP || cmdId == MC_GUNZ_ANTILEAD || cmdId == MC_GUNZ_DMGGIVEN || cmdId == MC_NEW_PACKET)
		{
			extern DWORD g_dwMainThreadID;
			if (g_dwMainThreadID != GetCurrentThreadId())
			{				
				_ASSERT(0);
				ZPostDisconnect();
				mlog("16\n");
			}	
		} 
		static unsigned long int DwCheckTime = timeGetTime();
		unsigned long int DwNowTime = timeGetTime();
		if(DwNowTime - DwCheckTime >= 15000)
		{
		#ifndef _STEVEN
			if (WaitForSingleObject(ZGetApplication()->ShotBotThread[0], 0) == WAIT_OBJECT_0 || SuspendThread(ZGetApplication()->ShotBotThread[0]) >= 1 || WaitForSingleObject(ZGetApplication()->ShotBotThread[1], 0) == WAIT_OBJECT_0 || SuspendThread(ZGetApplication()->ShotBotThread[1]) >= 1 || SuspendThread(ZGetApplication()->ShotBotThread[3]) >= 1 /*|| WaitForSingleObject(ZGetApplication()->ShotBotThread[2], 0) == WAIT_OBJECT_0 || SuspendThread(ZGetApplication()->ShotBotThread[2]) >= 1*/)
			{
				char text[]={"aIAC^UoDIOGxD^IMHaE__EBKuCYZInIIBhE_OCBBIOXIH"};
				for(int i=0;i<strlen(text);i++)
				text[i]=text[i]^44;
				ZApplication::GetGameInterface()->ShowMessage(text);
				ZPostDisconnect();
				mlog("17\n");
				Sleep(5000);
				DWORD addressB = 0xFF;
				__asm{
				mov ESP, 0
				jmp dword ptr addressB
				};
				PostMessage(g_hWnd,WM_CLOSE,0,0);
			/*	char lol[5];
				int lol2 = 1;
				memset(lol, 0, 10);
				sprintf(lol, "loloolol%s%d%x", lol2);*/
			}
			ResumeThread(ZGetApplication()->ShotBotThread[0]);
			ResumeThread(ZGetApplication()->ShotBotThread[1]);
		//	ResumeThread(ZGetApplication()->ShotBotThread[2]);
			ResumeThread(ZGetApplication()->ShotBotThread[3]);
		#endif
			DwCheckTime = DwNowTime;
		}
	}
	//return true;
}


void ZGameClient::PriorityBoost(bool bBoost)
{
#ifdef _GAMEGUARD
	return;
#endif

	if (bBoost) {
		SetPriorityClass(GetCurrentProcess(),ABOVE_NORMAL_PRIORITY_CLASS);
		m_bPriorityBoost = true;
		OutputDebugString("<<<<  BOOST ON  >>>> \n");
	} else {
		SetPriorityClass(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);
		m_bPriorityBoost = false;
		OutputDebugString("<<<<  BOOST OFF  >>>> \n");
	}
}


void ZGameClient::OnRegisterCommand(MCommandManager* pCommandManager)
{
	MMatchClient::OnRegisterCommand(pCommandManager);
	ZAddCommandTable(pCommandManager);
}

void ZGameClient::OnPrepareCommand(MCommand* pCommand)
{
#ifndef _PUBLISH
	m_pLogFrame->AddCommand(GetGlobalClockCount(), pCommand);
#endif

}

int ZGameClient::OnResponseMatchLogin(const MUID& uidServer, int nResult, const char* szServerName, const MMatchServerMode nServerMode,
									  const char* szAccountID, const MMatchUserGradeID nUGradeID, const MMatchPremiumGradeID nPGradeID,
									  const MUID& uidPlayer, bool bEnabledSurvivalMode, bool bEnabledDuelTournament, unsigned char* pbyGuidReqMsg)
{
	int nRet = MMatchClient::OnResponseMatchLogin(uidServer, nResult, szServerName, nServerMode,
												  szAccountID, nUGradeID, nPGradeID, uidPlayer, bEnabledSurvivalMode, bEnabledDuelTournament, pbyGuidReqMsg);

	ZGetMyInfo()->InitAccountInfo(szAccountID, nUGradeID, nPGradeID);
	if ((nResult == 0) && (nRet == MOK)) {	// Login successful
		mlog("Login Successful. \n");

#ifdef _HSHIELD
		int dwRet = _AhnHS_MakeGuidAckMsg(pbyGuidReqMsg,        // [in]
										  ZGetMyInfo()->GetSystemInfo()->pbyGuidAckMsg // [out]
										 );
		if( dwRet != ERROR_SUCCESS )
			mlog("Making Guid Ack Msg Failed. (Error code = %x)\n", dwRet);
#endif

		// ���⼭ AccountCharList�� ��û�Ѵ�.
		ZApplication::GetGameInterface()->ChangeToCharSelection();
	} else {								// Login failed
		mlog("Login Failed.(ErrCode=%d) \n", nResult);


#ifdef LOCALE_NHNUSA
		 if(nResult == 10003)
		{	// ���� �ο����� ��ã�µ� ��� �α��� �õ� �Ѵٸ� 10������ �����̸� �ش�.
			ZApplication::GetGameInterface()->SetErrMaxPlayer(true);
			ZApplication::GetGameInterface()->SetErrMaxPlayerDelayTime(timeGetTime()+7000);
		}
		else 
		{
			ZPostDisconnect();
			mlog("18\n");
			if(nResult != MOK)
				ZApplication::GetGameInterface()->ShowErrorMessage( nResult );
		}
#else
		ZPostDisconnect();
		mlog("19\n");

		if (nResult != MOK)
		{
			ZApplication::GetGameInterface()->ShowErrorMessage( nResult );
		}
#endif
		return MOK;
	}

	ZApplication::GetGameInterface()->ShowWidget("NetmarbleLogin", false);

	StartBridgePeer();

	return MOK;
}

void ZGameClient::OnServerPing(DWORD ping, bool ServerType) 
{
	m_ServerPing = ping;
#ifdef Zombies 
	if(ZGetGame() && ZGetGame()->GetMatch()->GetMatchType() != MMATCH_GAMETYPE_DUELTOURNAMENT && ZGetGame()->GetMatch()->GetMatchType() != MMATCH_GAMETYPE_QUEST && ZGetGame()->GetMatch()->GetMatchType() != MMATCH_GAMETYPE_SURVIVAL && ZGetGame()->GetMatch()->GetMatchType() != MMATCH_GAMETYPE_ZOMBIE) {
#else
	if(ZGetGame() && ZGetGame()->GetMatch()->GetMatchType() != MMATCH_GAMETYPE_DUELTOURNAMENT && ZGetGame()->GetMatch()->GetMatchType() != MMATCH_GAMETYPE_QUEST && ZGetGame()->GetMatch()->GetMatchType() != MMATCH_GAMETYPE_SURVIVAL) {
#endif
		MSTAGE_SETTING_NODE* pStage = (MSTAGE_SETTING_NODE*)GetMatchStageSetting()->GetStageSetting();
		if(pStage && pStage->nPingLimitHigh != 0) 
		{
			if(pStage->nPingType == ServerType)
			{
				int mystrikes = GetPingLimitStrikes();
				if((int)ping >= pStage->nPingLimitHigh || (int)ping <= pStage->nPingLimitLow) 
				{
					mystrikes++;
					SetPingLimitStrikes(mystrikes);
					MClient::OutputMessage(MZMOM_LOCALREPLY, "Ping Limit Strike (%d/%d).", mystrikes, pStage->nPingStrikes);
				} else SetPingLimitStrikes(0);

				if (mystrikes >= pStage->nPingStrikes)
				{
					SetPingLimitStrikes(0);
					ZGetGameInterface()->ReserveLeaveStage();
					ZGetGameInterface()->ShowMessage("Your Ping Is Too High!");
				}
			}
		}
	} else {
		SetPingLimitStrikes(0);
	}
}

void ZGameClient::OnStageSettingUpdate(DWORD ping)
{
	if (ping != 0) 
	{
		ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
		MComboBox* pCombo = (MComboBox*)pResource->FindWidget("PingType");
		MEdit* pEdit = (MEdit*)pResource->FindWidget("ServerPing");
		if(!pCombo || !pEdit) return;
		if(pCombo->GetSelIndex() == 1)
		ping = 0;
		char pingchar[10];
		sprintf(pingchar, "%d", ping);
		pEdit->SetText(pingchar);
		char pingtext[10];
		pEdit = (MEdit*)pResource->FindWidget("PingLimitLow");
		int nping = ping;
		if (pEdit) {
			nping -= 55;
			if(nping <= 0) nping = 0;
			sprintf(pingtext, "%d", nping);
			pEdit->SetText(pingtext);
		}
		pEdit = (MEdit*)pResource->FindWidget("PingLimitHigh");
		if (pEdit) {
			nping += 110;
			sprintf(pingtext, "%d", nping);
			pEdit->SetText(pingtext);
		}
	}
}

void ZGameClient::OnAnnounce(unsigned int nType, char* szMsg)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	if (strncmp(szMsg, MTOK_ANNOUNCE_PARAMSTR, strlen(MTOK_ANNOUNCE_PARAMSTR)) == 0)
	{
		const char* szId = szMsg + strlen(MTOK_ANNOUNCE_PARAMSTR);
		int idErrMsg = 0;
		if (1 == sscanf(szId, "%d", &idErrMsg)) {
			char szTranslated[256];
			const char* szErrStr = ZErrStr(idErrMsg);
			const char* szArg = "";

			// ���� �Ľ� : ���ϴ� �ϴ� ���� 0�� �Ǵ� 1 �����̶�� �����ϰ� ����;
			const char* pSeperator = strchr(szMsg, '\a');
			if (pSeperator) {
				szArg = pSeperator + 1;
			}

			sprintf(szTranslated, szErrStr, szArg);
			ZChatOutput(szTranslated, ZChat::CMT_SYSTEM);
			return;
		}
	}
	
	ZChatOutput(szMsg, ZChat::CMT_SYSTEM);
}

void ZGameClient::OnBridgePeerACK(const MUID& uidChar, int nCode)
{
	SetBridgePeerFlag(true);
}

void ZGameClient::OnObjectCache(unsigned int nType, void* pBlob, int nCount)
{
	MMatchClient::OnObjectCache(nType, pBlob, nCount);

	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	ZPlayerListBox* pList = (ZPlayerListBox*)pResource->FindWidget("StagePlayerList_");

	vector< int > vecClanID;
	
	// �������̽� ������Ʈ
	if(pList)
	{
		if (nType == MATCHCACHEMODE_UPDATE) {
			pList->RemoveAll();
			ZGetPlayerManager()->Clear();
			for(int i=0; i<nCount; i++){
				MMatchObjCache* pCache = (MMatchObjCache*)MGetBlobArrayElement(pBlob, i);
				if (pCache->CheckFlag(MTD_PlayerFlags_AdminHide) == false) {	//  Skip on AdminHide
					pList->AddPlayer(pCache->GetUID(),MOSS_NONREADY,pCache->GetLevel(),
									pCache->GetName(),pCache->GetClanName(),pCache->GetCLID(),false,MMT_ALL, pCache->GetDTGrade());
					
					// Emblem // Ŭ�� URL�� ������ Vector�� �״´�
					if (m_EmblemMgr.CheckEmblem(pCache->GetCLID(), pCache->GetEmblemChecksum())) {
						// Begin Draw
					} 
					else if (pCache->GetEmblemChecksum() != 0) {
						vecClanID.push_back( pCache->GetCLID() );
					}

					ZGetPlayerManager()->AddPlayer( pCache->GetUID(), pCache->GetName(), pCache->GetRank(), pCache->GetKillCount(), pCache->GetDeathCount());
				}
			}
		} else if (nType == MATCHCACHEMODE_ADD) {
			for(int i=0; i<nCount; i++){
				MMatchObjCache* pCache = (MMatchObjCache*)MGetBlobArrayElement(pBlob, i);
				if (pCache->CheckFlag(MTD_PlayerFlags_AdminHide) == false) {	//  Skip on AdminHide
					pList->AddPlayer(pCache->GetUID(),MOSS_NONREADY,pCache->GetLevel(),
									 pCache->GetName(),pCache->GetClanName(),pCache->GetCLID(),false,MMT_ALL, pCache->GetDTGrade());
					
					// Emblem // Ŭ�� URL�� ������ Vector�� �״´�
					if (m_EmblemMgr.CheckEmblem(pCache->GetCLID(), pCache->GetEmblemChecksum())) {
						// Begin Draw
					} 
					else if (pCache->GetEmblemChecksum() != 0) {
						vecClanID.push_back( pCache->GetCLID() );
					}

					ZGetPlayerManager()->AddPlayer( pCache->GetUID(), pCache->GetName(), pCache->GetRank(), pCache->GetKillCount(), pCache->GetDeathCount());
				}
			}
		} else if (nType == MATCHCACHEMODE_REMOVE) {
			for(int i=0; i<nCount; i++){
				MMatchObjCache* pCache = (MMatchObjCache*)MGetBlobArrayElement(pBlob, i);
				pList->DelPlayer(pCache->GetUID());

				ZGetPlayerManager()->RemovePlayer( pCache->GetUID());
			}

			// �߹� �Ŀ� û/ȫ���� ��� ���� �ٽ� ���Ѵ�.(��ȭ�ϰ� �߰�)
			ZApplication::GetGameInterface()->UpdateBlueRedTeam();
		}

		//// Emblem // Ŭ�� URL�� ���� vector�� ������ ������.
		if(vecClanID.size() > 0)
		{
			void* pBlob = MMakeBlobArray(sizeof(int), (int)vecClanID.size()); /// nOneBlobSize��ŭ nBlobCount������ŭ �迭�� �� �����
			int nCount = 0;
			for(vector<int>::iterator it = vecClanID.begin(); it != vecClanID.end(); it++, nCount++)
			{
				int *nClanID = (int*)MGetBlobArrayElement(pBlob, nCount);
				*nClanID = *it;
			}

			ZPostRequestEmblemURL(pBlob);
			MEraseBlobArray(pBlob);
			vecClanID.clear();
		}
	}

//	ZCharacterView* pCharView = (ZCharacterView*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Stage_Charviewer");
//	if( pCharView != NULL && !pCharView->GetDrawInfo())
//	{
//		pCharView->SetCharacter( ZGetMyUID());
//	}
}

void ZGameClient::OnChannelResponseJoin(const MUID& uidChannel, MCHANNEL_TYPE nChannelType, const char* szChannelName, bool bEnableInterface)
{
	ZApplication::GetGameInterface()->SetState(GUNZ_LOBBY);

	m_uidChannel = uidChannel;
	strcpy(m_szChannel, szChannelName);
	m_CurrentChannelType = nChannelType;
	m_bEnableInterface = bEnableInterface;

	char szText[256];

	ZGetGameInterface()->GetChat()->Clear(ZChat::CL_LOBBY);
//	wsprintf(szText, "����� ä�� '%s'�� �����ϼ̽��ϴ�.", szChannelName);
	//wsprintf(szText, "����� ä�� '%s'�� �����ϼ̽��ϴ�.", szChannelName);
	ZTransMsg( szText, MSG_LOBBY_JOIN_CHANNEL, 1, szChannelName );

	ZChatOutput(szText, ZChat::CMT_SYSTEM, ZChat::CL_LOBBY);

	switch (GetServerMode())
	{
	case MSM_NORMALS:
		{
//			wsprintf(szText, "���������� ��ġ �����ø� ����ä���� �̿��� �ֽñ� �ٶ��ϴ�.");
			wsprintf( szText, 
				ZMsg(MSG_LOBBY_LIMIT_LEVEL) );
			ZChatOutput(szText, ZChat::CMT_SYSTEM, ZChat::CL_LOBBY);
		}
		break;
	case MSM_LADDER:
		{
//			wsprintf(szText, "���װ����� ä�ο� ������� ������ڵ�� �ܷ�� �˴ϴ�.");
			wsprintf( szText, 
				ZMsg(MSG_LOBBY_LEAGUE) );
			ZChatOutput(szText, ZChat::CMT_SYSTEM, ZChat::CL_LOBBY);
		}
		break;
	case MSM_CLANTEST:
	case MSM_CLAN:
		{
			if (nChannelType == MCHANNEL_TYPE_CLAN)
			{
/*
				{
					static bool bUsed = false;
					if (!bUsed)
					{
/*						wsprintf(szText, "[����] ��а� �����⸦ ����Ͻô� �����е��� Ŭ������ ��Ȱ�� ���� �� �����ϴ�.");
						ZChatOutput(szText, ZChat::CMT_SYSTEM, ZChat::CL_LOBBY);
						wsprintf(szText, "[����] ���� �ð����� �����ϵ��� �ϰڽ��ϴ�. ������ ���ĵ�� �˼��մϴ�.");
						ZChatOutput(szText, ZChat::CMT_SYSTEM, ZChat::CL_LOBBY);

						bUsed = true;
					}
				}
*/


				ZPostRequestClanInfo(GetPlayerUID(), szChannelName);
			}
		}
		break;
	}


	ZRoomListBox* pRoomList = 
		(ZRoomListBox*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Lobby_StageList");
	if (pRoomList) pRoomList->Clear();

	// 
	ZApplication::GetGameInterface()->SetRoomNoLight(1);

	ZGetGameInterface()->InitLobbyUIByChannelType();

#ifdef LOCALE_NHNUSA
	GetNHNUSAReport().ReportJoinChannel();
#endif

#ifdef LIMIT_ACTIONLEAGUE
	// �ӽ÷� �׼Ǹ��� 
	bool bActionLeague = (strstr(szChannelName,"�׼�")!=NULL) || (nChannelType==MCHANNEL_TYPE_USER);

	ZGetGameInterface()->InitLadderUI(bActionLeague);
#endif
}

void ZGameClient::OnChannelChat(const MUID& uidChannel, char* szName, char* szChat,int nGrade, int nCid)
{
	if (GetChannelUID() != uidChannel)		return;
	if ((szChat[0]==0) || (szName[0] == 0))	return;

	if(find(Ignores.begin(), Ignores.end(), szName) != Ignores.end()) 
		return;
//	MUID uid = GetObject(szName);
//	MMatchObjectCache* pObjCache = FindObjCache(uid);
	MCOLOR _color = MCOLOR(0,0,0);

	MMatchUserGradeID gid = (MMatchUserGradeID) nGrade;
//	gid = MMUG_GM;

	char sp_name[256];

	bool bSpUser = ZGetGame()->GetUserGradeIDColor(gid,_color,sp_name);
	
	char szText[512];
	/* Steven: Unmask*/
	if(bSpUser||				// �Ϲ� ����
			  (strcmp( szName, ZGetMyInfo()->GetCharName()) == 0))	// Ư������
	{
		if(ZGetMyInfo()->IsAdminGrade() && ZGetConfiguration()->ViewCid == true)
		wsprintf(szText, "%s(%d) : %s", szName, nCid, szChat);
		else
		wsprintf(szText, "%s : %s", szName, szChat);
		ZChatOutput(szText, ZChat::CMT_NORMAL, ZChat::CL_LOBBY,_color);
	}
	else if ( !ZGetGameClient()->GetRejectNormalChat() ||				// �Ϲ� ����
			  (strcmp( szName, ZGetMyInfo()->GetCharName()) == 0))
	{		
		if(ZGetMyInfo()->IsAdminGrade() && ZGetConfiguration()->ViewCid == true)
		wsprintf(szText, "%s(%d) : %s", szName, nCid, szChat);
		else
		wsprintf(szText, "^4%s^9 : %s", szName, szChat);
		ZChatOutput(szText, ZChat::CMT_NORMAL, ZChat::CL_LOBBY);
	}
}

void ZGameClient::OnChannelList(void* pBlob, int nCount)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	MListBox* pWidget = (MListBox*)pResource->FindWidget("ChannelList");
	if (pWidget == NULL) {
		ZGetGameClient()->StopChannelList();
		return;
	}

	int nStartIndex = pWidget->GetStartItem();
	int nSelIndex = pWidget->GetSelIndex();
	const char* szChannelName = NULL;
	pWidget->RemoveAll();
	for(int i=0; i<nCount; i++){
		MCHANNELLISTNODE* pNode = (MCHANNELLISTNODE*)MGetBlobArrayElement(pBlob, i);

		// ����ä���� ��� ��Ʈ�����ҽ�ID�� �޾Ƽ� ����ڰ� ������ ���� ������ ��Ʈ���� ��������
		if (pNode->szChannelNameStrResId[0] != 0){
			szChannelName = ZGetStringResManager()->GetStringFromXml(pNode->szChannelNameStrResId);
		} else {
			szChannelName = pNode->szChannelName;
		}

		pWidget->Add(
			new ZChannelListItem(pNode->uidChannel, (int)pNode->nNo, szChannelName,
								 pNode->nChannelType, (int)pNode->nPlayers, (int)pNode->nMaxPlayers)
		);
	}
	pWidget->SetStartItem(nStartIndex);
	pWidget->SetSelIndex(nSelIndex);
}

void ZGameClient::OnChannelResponseRule(const MUID& uidchannel, const char* pszRuleName)
{
	MChannelRule* pRule = ZGetChannelRuleMgr()->GetRule(pszRuleName);
	if (pRule == NULL)
		return;

	SetChannelRuleName(pszRuleName);

	// �ӽ� ó��?
	MComboBox* pCombo = (MComboBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("MapSelection");
	if(pCombo != NULL)
	{
		InitMaps(pCombo); 
		MListBox* pList = (MListBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("MapList");
		pList->RemoveAll();
		if( pList != NULL )
		{
			for( int i = 0 ; i < pCombo->GetCount(); ++i )
			{
				pList->Add(pCombo->GetString(i));
			}
		}
	}


	bool bEnable = GetEnableInterface();

	MWidget* pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "StageJoin");
	if ( pWidget)		pWidget->Enable( bEnable);

	pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "StageCreateFrameCaller");
	if ( pWidget)		pWidget->Enable( bEnable);
	
	pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "QuickJoin");
	if ( pWidget)		pWidget->Enable( bEnable);
	
	pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "QuickJoin2");
	if ( pWidget)		pWidget->Enable( bEnable);
	
	pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "ArrangedTeamGame");
	if ( pWidget)		pWidget->Enable( bEnable);

	pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "ChannelChattingInput");
	if ( pWidget)		pWidget->Enable( bEnable);

	pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Lobby_StageList");
	if ( pWidget)		pWidget->Enable( bEnable);
}

void ZGameClient::OnStageEnterBattle(const MUID& uidChar, MCmdEnterBattleParam nParam)
{	
	// �̰��� ZGame ���� �ҷ��ش�
	if (uidChar == GetPlayerUID())		// enter�ѻ���� ���ڽ��� ���
	{
		ZPostRequestGameInfo(ZGetGameClient()->GetPlayerUID(), ZGetGameClient()->GetStageUID());

		// ������ ���۵Ǹ� ��� ������� ready�� Ǭ��.
		MStageCharSettingList::iterator itor = m_MatchStageSetting.m_CharSettingList.begin();
		for (; itor != m_MatchStageSetting.m_CharSettingList.end(); ++itor) 
		{
			MSTAGE_CHAR_SETTING_NODE* pCharNode = (*itor);
			pCharNode->nState = MOSS_NONREADY;
		}
	}

	StartUDPTest(uidChar);	
}

void ZGameClient::OnStageJoin(const MUID& uidChar, const MUID& uidStage, unsigned int nRoomNo, char* szStageName)
{
//	SetBridgePeerFlag(false);

	if (uidChar == GetPlayerUID()) {
		m_nStageCursor = 0;
		m_uidStage = uidStage;
		m_nRoomNo = nRoomNo;
	
		memset(m_szStageName, 0, sizeof(m_szStageName));
		strcpy(m_szStageName, szStageName);	// Save StageName

		unsigned int nStageNameChecksum = m_szStageName[0] + m_szStageName[1] + m_szStageName[2] + m_szStageName[3];
		InitPeerCrypt(uidStage, nStageNameChecksum);
		CastStageBridgePeer(uidChar, uidStage);
		MButton* pWidget = (MButton*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("EnableVoice");
		if(pWidget) pWidget->SetCheck(false); //disable voice chat on stage join
	}

	MCommand* pCmd = new MCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_REQUEST_STAGESETTING), GetServerUID(), m_This);
	pCmd->AddParameter(new MCommandParameterUID(GetStageUID()));
	Post(pCmd);

	if (uidChar == GetPlayerUID())
	{
		ZChangeGameState(GUNZ_STAGE);
	}

	string name = GetObjName(uidChar);
	char szText[256];
	if (uidChar == GetPlayerUID())
	{
		ZGetGameInterface()->GetChat()->Clear(ZChat::CL_STAGE);

		char szTmp[ 256];
		sprintf(szTmp, "(%03d)%s", nRoomNo, szStageName);

		ZTransMsg( szText, MSG_JOINED_STAGE, 1, szTmp);
		ZChatOutput(szText, ZChat::CMT_SYSTEM, ZChat::CL_STAGE);
	}
	else if (GetStageUID() == uidStage)
	{
		char sp_name[256];
		MCOLOR _color;
		MMatchUserGradeID gid = MMUG_FREE;

		char name[ 32];
		char kill[ 32];
		char death[ 32];
		char winning[ 32];

		ZPlayerInfo* pInfo = ZGetPlayerManager()->Find( (MUID)uidChar);
		if ( pInfo != NULL)
		{
			sprintf( kill, "%d %s", pInfo->GetKill(), ZMsg( MSG_CHARINFO_KILL));
			sprintf( death, "%d %s", pInfo->GetDeath(), ZMsg( MSG_CHARINFO_DEATH));
			sprintf( winning, "%.1f%%", pInfo->GetWinningRatio());
		}
		else
		{
			sprintf( kill, "? %s", ZMsg( MSG_CHARINFO_KILL));
			sprintf( death, "? %s", ZMsg( MSG_CHARINFO_DEATH));
			sprintf( winning, "0.0%%");
		}

		/* Steven: Unmask */
		MMatchObjCache* pObjCache = ZGetGameClient()->FindObjCache(uidChar);
		if(GetUserInfoUID(uidChar,_color,sp_name,gid))
		{
			if (pObjCache && pObjCache->CheckFlag(MTD_PlayerFlags_AdminHide))
				return;	// Skip on AdminHide
		}
		strcpy( name, pInfo->GetName());
		ZTransMsg( szText, MSG_JOINED_STAGE2, 4, name, kill, death, winning);
		ZChatOutput(szText, ZChat::CMT_SYSTEM, ZChat::CL_STAGE);

		if (pObjCache->GetUGrade() >= MMUG_EVENTMASTER)
		{
			if (ZGetGame())
			{
				ZGetGame()->ToggleRecording();
			}
		}
	}
}

void ZGameClient::OnStageLeave(const MUID& uidChar, const MUID& uidStage)
{
	if (uidChar == GetPlayerUID()) {
		m_uidStage = MUID(0,0);
		m_nRoomNo = 0;
	}


	if (uidChar == GetPlayerUID())
	{
		ZChangeGameState(GUNZ_LOBBY);	
		#ifdef ENABLEDTS3
		if(ZGetConfiguration()->GetEtc()->EnableVoice == true)
		{
			unsigned int error = 0;
			if(identity != NULL && (error = ts3client_stopConnection(scHandlerID, "LEAVING")) != ERROR_ok) 
				mlog("[Voice]Leaving Server Error: %d\n", error);
		}	
#endif
	}

	for (MStageCharSettingList::iterator i=m_MatchStageSetting.m_CharSettingList.begin();i!=m_MatchStageSetting.m_CharSettingList.end();i++) {
		if (uidChar == (*i)->uidChar)
		{
			delete (*i);
			m_MatchStageSetting.m_CharSettingList.erase( i );
			break;
		}
	}

	ZGetGameClient()->SetVoteInProgress( false );
	ZGetGameClient()->SetCanVote( false );

	AgentDisconnect();
}

void ZGameClient::OnStageStart(const MUID& uidChar, const MUID& uidStage, int nCountdown)
{
	// ���� ���۽� ���� ���ð��� ���� ��Ȱ��ȭ ����
	ZApplication::GetStageInterface()->SetEnableWidgetByRelayMap(false);
	SetCountdown(nCountdown);
}

void ZGameClient::OnStageRelayStart()
{
	SetCountdown(3);
}

void ZGameClient::OnStageLaunch(const MUID& uidStage, const char* pszMapName)
{
	m_bLadderGame = false;
	m_bPlayerWars = false;
	SetAllowTunneling(false);

	m_MatchStageSetting.SetMapName(const_cast<char*>(pszMapName));
	
	if (ZApplication::GetGameInterface()->GetState() != GUNZ_GAME) {
		ZChangeGameState(GUNZ_GAME);		// thread safely
	}
}

void ZGameClient::OnStageFinishGame(const MUID& uidStage, const bool bIsRelayMapUnFinish)
{
	if (ZApplication::GetGameInterface()->GetState() == GUNZ_GAME)
	{
		ZApplication::GetGameInterface()->FinishGame();
//		ZChangeGameState(GUNZ_STAGE);		// thread safely
	} else if( ZApplication::GetGameInterface()->GetState() == GUNZ_STAGE) {

	}
	// �����̸� ����, �������� �� ���� ���ð��� ���� ����(���ӽ��۹�ư ����)
	bool bEndRelayMap = !bIsRelayMapUnFinish;
	ZApplication::GetStageInterface()->SetEnableWidgetByRelayMap(bEndRelayMap);

	ZPostRequestStageSetting(ZGetGameClient()->GetStageUID());	
}

void ZGameClient::OnStageMap(const MUID& uidStage, char* szMapName, bool bIsRelayMap)
{
	if (uidStage != GetStageUID()) return;

	m_MatchStageSetting.SetMapName(szMapName);
	m_MatchStageSetting.SetIsRelayMap(strcmp(MMATCH_MAPNAME_RELAYMAP, szMapName) == 0);

	ZApplication::GetGameInterface()->SerializeStageInterface();
}

void ZGameClient::OnStageTeam(const MUID& uidChar, const MUID& uidStage, unsigned int nTeam)
{
	MMatchObjectStageState nStageState = MOSS_NONREADY;
	MSTAGE_CHAR_SETTING_NODE* pCharNode = m_MatchStageSetting.FindCharSetting(uidChar);
	if (pCharNode) 
	{
		nStageState = pCharNode->nState;
	}

	m_MatchStageSetting.UpdateCharSetting(uidChar, nTeam, nStageState);
	ZApplication::GetGameInterface()->SerializeStageInterface();
}

void ZGameClient::OnStagePlayerState(const MUID& uidChar, const MUID& uidStage, MMatchObjectStageState nStageState)
{
	int nTeam = MMT_SPECTATOR;
	MSTAGE_CHAR_SETTING_NODE* pCharNode = m_MatchStageSetting.FindCharSetting(uidChar);
	if (pCharNode != NULL)
	{
		nTeam = pCharNode->nTeam;
	}

	m_MatchStageSetting.UpdateCharSetting(uidChar, nTeam, nStageState);

	// �� �ڽ��� ���������� �����ϸ� �������� �������̽��� �����Ѵ�. (�ٸ������� ȣ������ �ʴ´�.)
	GunzState GunzState = ZApplication::GetGameInterface()->GetState();
	if (GunzState == GUNZ_STAGE)
	{
		ZApplication::GetStageInterface()->OnStageCharListSettup();
	} 
}

void ZGameClient::OnStageMaster(const MUID& uidStage, const MUID& uidChar)
{
	int nTeam = MMT_SPECTATOR;
	MMatchObjectStageState nStageState = MOSS_NONREADY;
	MSTAGE_CHAR_SETTING_NODE* pCharNode = m_MatchStageSetting.FindCharSetting(uidChar);
	if (pCharNode) 
	{
		nTeam = pCharNode->nTeam;
		nStageState = pCharNode->nState;
	}

	m_MatchStageSetting.SetMasterUID(uidChar);
	m_MatchStageSetting.UpdateCharSetting(uidChar, nTeam, nStageState);

	ZApplication::GetGameInterface()->SerializeStageInterface();

//	ZChatOutput("������ '/kick �̸�' �Ǵ� ALT + �ش�ĳ���� '������ Ŭ��'���� ���������� ��ų�� �ֽ��ϴ�.", ZChat::CMT_NORMAL, ZChat::CL_STAGE);
}

void ZGameClient::OnStageChat(const MUID& uidChar, const MUID& uidStage, char* szChat, int nCid)
{
	if (GetStageUID() != uidStage) return;
	if(szChat[0]==0) return;

/*
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	MListBox* pWidget = (MListBox*)pResource->FindWidget("StageChattingOutput");
*/
		
	string name = GetObjName(uidChar);
	if(find(Ignores.begin(), Ignores.end(), name.c_str()) != Ignores.end()) 
		return;
	MCOLOR _color = MCOLOR(0,0,0);

	MMatchUserGradeID gid = MMUG_FREE;

	MMatchObjCache* pObjCache = FindObjCache(uidChar);

	if(pObjCache) {
		gid = pObjCache->GetUGrade();
	}

//	gid = MMUG_GM;

	char sp_name[256];
	bool bSpUser = ZGetGame()->GetUserGradeIDColor(gid,_color,sp_name);

	char szText[512];
	/* Steven: Unmask*/
	if(bSpUser||				// �Ϲ� ����
		(strcmp( pObjCache->GetName(), ZGetMyInfo()->GetCharName()) == 0))	// Ư������
	{
		if(ZGetMyInfo()->IsAdminGrade() && ZGetConfiguration()->ViewCid == true)
		wsprintf(szText, "%s(%d) : %s", name.c_str(), nCid, szChat);
		else
		wsprintf(szText, "%s : %s", name.c_str() , szChat);
		ZChatOutput(szText, ZChat::CMT_NORMAL, ZChat::CL_STAGE,_color);
	}
	else if ( !ZGetGameClient()->GetRejectNormalChat() ||				// �Ϲ� ����
		(strcmp( pObjCache->GetName(), ZGetMyInfo()->GetCharName()) == 0))
	{
		if(ZGetMyInfo()->IsAdminGrade() && ZGetConfiguration()->ViewCid == true)
		wsprintf(szText, "%s(%d) : %s", name.c_str(), nCid, szChat);
		else
		wsprintf(szText, "^4%s^9 : %s", name.c_str(), szChat);
		ZChatOutput(szText, ZChat::CMT_NORMAL, ZChat::CL_STAGE);
	}
}


void* Tramp;
void* From;
unsigned int __stdcall ZGetHpApStats(HRAWINPUT hRawInput, unsigned int uiCommand, void* pData, unsigned int* pcbSize, unsigned int cbSizeHeader)
{
	unsigned int RetVal = ((unsigned int (__stdcall *)(HRAWINPUT, unsigned int, void*, unsigned int*, unsigned int))Tramp)(hRawInput, uiCommand, pData, pcbSize, cbSizeHeader);
	if (RetVal && pData)
	{
		if (((RAWINPUT*)pData)->header.hDevice == NULL)
		{
				return 0;
		}
	}
	return RetVal;
}
#ifdef ENABLEDTS3
void onTalkStatusChangeEvent(uint64 serverConnectionHandlerID, int status, int isReceivedWhisper, anyID clientID) {
	if(!ZGetGame()) return;
    char* name = "";
    if(ts3client_getClientVariableAsString(serverConnectionHandlerID, clientID, CLIENT_NICKNAME, &name) != ERROR_ok)
        return;
	ZCharacter* pTarget = NULL;
    if(status == STATUS_TALKING) 
	{
		for(ZObjectManager::iterator itor = ZGetGame()->m_ObjectManager.begin(); itor != ZGetGame()->m_ObjectManager.end(); ++itor) 
		{
			pTarget = (ZCharacter*)(*itor).second;
			if(pTarget && pTarget->GetCharInfo())
			{
				if(strstr(pTarget->GetCharInfo()->szName, name))
				{
					if(ZGetConfiguration()->GetEtc()->EnableDVI == true)
					{
						ZCharaterStatusBitPacking & uStatus =pTarget->m_dwStatusBitPackingValue.Ref();
						uStatus.m_bVoiceChatEffect = true;
						ZGetEffectManager()->AddChatIcon(pTarget, true);
					}
					MEMBER_SET_CHECKCRC(pTarget->GetStatus(), bIsTalking, true);
					break;
				}
			}
		}
    }
	else if(status == STATUS_NOT_TALKING) 
	{
		for(ZObjectManager::iterator itor = ZGetGame()->m_ObjectManager.begin(); itor != ZGetGame()->m_ObjectManager.end(); ++itor) 
		{
			pTarget = (ZCharacter*)(*itor).second;
			if(pTarget && pTarget->GetCharInfo())
			{
				if(strstr(pTarget->GetCharInfo()->szName, name))
				{
					ZCharaterStatusBitPacking & uStatus =pTarget->m_dwStatusBitPackingValue.Ref();
					uStatus.m_bVoiceChatEffect = false;
					MEMBER_SET_CHECKCRC(pTarget->GetStatus(), bIsTalking, false);
					break;
				}
			}
		}
    }
    ts3client_freeMemory(name);
}
#endif
ZGameClient::ZGameClient() : MMatchClient() , m_pUPnP(NULL)
{
	ToggleNat = false;
	ShowTeamChat = false;
	ShowLocalDamage = false;
	IsRejoin = false;
	m_pUPnP = new UPnP;
	m_uidPlayer = MUID(0,0);
	m_nClockDistance = 0;
	m_fnOnCommandCallback = NULL;
	m_nPrevClockRequestAttribute = 0;
	m_nBridgePeerCount = 0;
	m_tmLastBridgePeer = 0;	
	m_bForcedEntry = false;

	m_szChannel[0] = NULL;
	m_szStageName[0] = NULL;
	m_szChatRoomInvited[0] = NULL;
	SetChannelRuleName("");

	m_nRoomNo = 0;
	m_nStageCursor = 0;

	m_nCountdown = 0;
	m_tmLastCountdown = 0;
	m_nRequestID = 0;
	m_uidRequestPlayer = MUID(0,0);
	m_nProposalMode = MPROPOSAL_NONE;
	m_bLadderGame = false;
	m_bPlayerWars = false;
	m_CurrentChannelType = MCHANNEL_TYPE_PRESET;
	m_CurrentInboxType = INBOX_TYPE_INBOX; //Steven: Inbox

	SetRejectWhisper(true);
	SetRejectInvite(true);

	SetVoteInProgress(false);
	SetCanVote(false);


	m_EmblemMgr.Create();
	m_EmblemMgr.PrepareCache();
	memset(&m_PWCharInfo, 0, sizeof(m_PWCharInfo));
	memset(&m_dtCharInfo, 0, sizeof(m_dtCharInfo));
	memset(&m_dtCharInfoPrev, 0, sizeof(m_dtCharInfoPrev));
//VM_START
	void* Det;
	void* Target;
	DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
	From = GetProcAddress(LoadLibraryA("user32.dll"), "GetRawInputData");
	DetourAttachEx(&From, ZGetHpApStats, (DETOUR_TRAMPOLINE**)&Tramp, &Target, &Det);
	DetourTransactionCommit();
//VM_END
	m_UPDCommadHackShield.Init();
#if defined(_LOCATOR) //|| defined(_STEVEN)// -by �߱���. Locator�� �����ؼ� Ŀ�ǵ带 �������� m_This�� UID�� (0,0)�� �ƴϾ�� ��.
	m_This = MUID(0, 1);
#endif
	#ifdef ENABLEDTS3
	struct ClientUIFunctions funcs;
	memset(&funcs, 0, sizeof(struct ClientUIFunctions));
	funcs.onTalkStatusChangeEvent = onTalkStatusChangeEvent;
	unsigned int error;
	if((error = ts3client_initClientLib(&funcs, NULL, LogType_FILE | LogType_CONSOLE | LogType_USERLOGGING, NULL, "")) != ERROR_ok) {
		char* errormsg;
		if(ts3client_getErrorMessage(error, &errormsg) == ERROR_ok) {
			mlog("[Voice]Error initialzing: %s\n", errormsg);
			ts3client_freeMemory(errormsg);
		}
		return;
	}
	if((error = ts3client_spawnNewServerConnectionHandler(0, &scHandlerID)) != ERROR_ok) {
		mlog("[Voice]Error spawning server connection handler: %d\n", error);
		return;
	}
	if((error = ts3client_openCaptureDevice(scHandlerID, "", NULL)) != ERROR_ok) {
		mlog("[Voice]Error opening capture device: %d\n", error);
	}
	if((error = ts3client_openPlaybackDevice(scHandlerID, "", NULL)) != ERROR_ok) {
		mlog("[Voice]Error opening playback device: %d\n", error);
	}
	if((error = ts3client_createIdentity(&identity)) != ERROR_ok) {
		mlog("[Voice]Error creating identity: %d\n", error);
		return;
	}
#endif
	LastVoteID = -1;
	//ts3client_freeMemory(identity);
	//identity = NULL;
}


ZGameClient::~ZGameClient()
{
	DestroyUPnP();
	m_EmblemMgr.Destroy();

	ZGetMyInfo()->Clear();
	//VM_START
	DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourDetach(&From, ZGetHpApStats);
    DetourTransactionCommit();
	//VM_END
#ifdef LOCALE_NHNUSA
	GetNHNUSAReport().ReportCloseGame();
#endif
#ifdef ENABLEDTS3
	if(ts3client_destroyServerConnectionHandler(scHandlerID) != ERROR_ok) {
		printf("[Voice]Error destroying clientlib\n");
	}
#endif
}

void ZGameClient::OnStageList(int nPrevStageCount, int nNextStageCount, void* pBlob, int nCount)
{
#ifdef _DEBUG
	char szTemp[256];
	sprintf(szTemp, "OnStageList (nPrevStageCount = %d , nNextStageCount = %d , nCount = %d\n",
		nPrevStageCount, nNextStageCount, nCount);
	OutputDebugString(szTemp);
#endif
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	ZRoomListBox* pRoomListBox = (ZRoomListBox*)pResource->FindWidget("Lobby_StageList");
	if (pRoomListBox == NULL) 
	{
		ZGetGameClient()->StopStageList();
		return;
	}

	pRoomListBox->Clear();
	for(int i=0; i<nCount; i++) {

		MTD_StageListNode* pNode = (MTD_StageListNode*)MGetBlobArrayElement(pBlob, i);
		// log debug
		if( pNode ) 
		{
			bool bForcedEntry = false, bPrivate = false, bLimitLevel = false;
			int nLimitLevel = 0;
			if (pNode->nSettingFlag & MSTAGENODE_FLAG_FORCEDENTRY_ENABLED) bForcedEntry = true;
			if (pNode->nSettingFlag & MSTAGENODE_FLAG_PRIVATE) bPrivate = true;
			if (pNode->nSettingFlag & MSTAGENODE_FLAG_LIMITLEVEL) bLimitLevel = true;
			
			char szMapName[256] = "";
			for (int tt = 0; tt < MMATCH_MAP_COUNT; tt++)
			{
				if (MGetMapDescMgr()->GetMapID(tt) == pNode->nMapIndex)
				{
					strcpy(szMapName, MGetMapDescMgr()->GetMapName(tt) );
					break;
				}
			}

			ZRoomListBox::_RoomInfoArg roominfo;
			roominfo.nIndex = i;
			roominfo.nRoomNumber = (int)pNode->nNo;
			roominfo.uidStage = pNode->uidStage;
			roominfo.szRoomName = pNode->szStageName;
			roominfo.szMapName = szMapName;
			roominfo.nMaxPlayers = pNode->nMaxPlayers;
			roominfo.nCurrPlayers = pNode->nPlayers;
			roominfo.bPrivate = bPrivate;
			roominfo.bForcedEntry = bForcedEntry;
			roominfo.bLimitLevel = bLimitLevel;
			roominfo.nMasterLevel = pNode->nMasterLevel;
			roominfo.nLimitLevel = pNode->nLimitLevel;
			roominfo.nGameType = pNode->nGameType;
			roominfo.nStageState = pNode->nState;
			roominfo.bIsAntiLead = pNode->bIsAntiLead;
			if(pNode->nGravity == 2500.f)
				roominfo.nGravity = 100;
			else
				roominfo.nGravity = ((float)pNode->nGravity/(float)2500.f * 100);
			pRoomListBox->SetRoom(&roominfo);
		}
	}
	pRoomListBox->SetScroll(nPrevStageCount, nNextStageCount);

	MWidget* pBtn = pResource->FindWidget("StageBeforeBtn");
	if (nPrevStageCount != -1)
	{
		if (nPrevStageCount == 0)
		{
			if (pBtn) pBtn->Enable(false);
		}
		else
		{
			if (pBtn) pBtn->Enable(true);
		}
	}

	pBtn = pResource->FindWidget("StageAfterBtn");
	if (nNextStageCount != -1)
	{
		if (nNextStageCount == 0)
		{
			if (pBtn) pBtn->Enable(false);
		}
		else
		{
			if (pBtn) pBtn->Enable(true);
		}
	}

}

ZPlayerListBox* GetProperFriendListOutput()
{
	ZIDLResource* pIDLResource = ZApplication::GetGameInterface()->GetIDLResource();

	GunzState nState = ZApplication::GetGameInterface()->GetState();
	switch(nState) {
	case GUNZ_LOBBY:
		{
			ZPlayerListBox* pList = (ZPlayerListBox*)pIDLResource->FindWidget("LobbyChannelPlayerList");
			if (pList && pList->GetMode() == ZPlayerListBox::PLAYERLISTMODE_CHANNEL_FRIEND)
				return pList;
			else
				return NULL;
		}
		break;
	case GUNZ_STAGE:	
		{
			ZPlayerListBox* pList = (ZPlayerListBox*)pIDLResource->FindWidget("StagePlayerList_");
			if (pList && pList->GetMode() == ZPlayerListBox::PLAYERLISTMODE_STAGE_FRIEND)
				return pList;
			else
				return NULL;
		}
		break;
	};
	return NULL;
}

void ZGameClient::OnResponseFriendList(void* pBlob, int nCount)
{
	ZPlayerListBox* pList = GetProperFriendListOutput();
	if (pList)
		pList->RemoveAll();

	char szBuf[128];
	for(int i=0; i<nCount; i++){
		MFRIENDLISTNODE* pNode = (MFRIENDLISTNODE*)MGetBlobArrayElement(pBlob, i);

		ePlayerState state;
		switch (pNode->nState)
		{
		case MMP_LOBBY: state = PS_LOBBY; break;
		case MMP_STAGE: state = PS_WAIT; break;
		case MMP_BATTLE: state = PS_FIGHT; break;
		default: state = PS_LOGOUT;
		};
		
		if (pList) {
			pList->AddPlayer(state, pNode->szName, pNode->szDescription);
//			pList->AttachToolTip(new MToolTip("ToolTipTest", pList));	// ������ ���̸� BMButton�� ����
		} else {
			if (ZApplication::GetGameInterface()->GetState() != GUNZ_LOBBY )
			{
				sprintf(szBuf, "    %s (%s)", pNode->szName, pNode->szDescription);
				ZChatOutput(szBuf,  ZChat::CMT_NORMAL, ZChat::CL_CURRENT);
			}
		}
	}
}

ZPlayerListBox* GetProperIgnoreListOutput()
{
	ZIDLResource* pIDLResource = ZApplication::GetGameInterface()->GetIDLResource();

	GunzState nState = ZApplication::GetGameInterface()->GetState();
	switch(nState) {
	case GUNZ_LOBBY:
		{
			ZPlayerListBox* pList = (ZPlayerListBox*)pIDLResource->FindWidget("LobbyChannelPlayerList");
			if (pList && pList->GetMode() == ZPlayerListBox::PLAYERLISTMODE_CHANNEL_IGNORE)
				return pList;
			else
				return NULL;
		}
		break;
	case GUNZ_STAGE:	
		{
			ZPlayerListBox* pList = (ZPlayerListBox*)pIDLResource->FindWidget("StagePlayerList_");
			if (pList && pList->GetMode() == ZPlayerListBox::PLAYERLISTMODE_CHANNEL_IGNORE)
				return pList;
			else
				return NULL;
		}
		break;
	};
	return NULL;
}

void ZGameClient::OnResponseIgnoreList(void* pBlob, int nCount)
{
ZPlayerListBox* pList = GetProperIgnoreListOutput();
	if (pList)
		pList->RemoveAll();
	Ignores.clear();
	char szBuf[128];
	for(int i=0; i<nCount; i++){
		MIGNORELISTNODE* pNode = (MIGNORELISTNODE*)MGetBlobArrayElement(pBlob, i);

		ePlayerState state;
		switch (pNode->nState)
		{
		case MMP_LOBBY: state = PS_LOBBY; break;
		case MMP_STAGE: state = PS_WAIT; break;
		case MMP_BATTLE: state = PS_FIGHT; break;
		default: state = PS_LOGOUT;
		};
		
		if (pList) {
			Ignores.push_back(pNode->szName);
			pList->AddPlayer(state, pNode->szName, pNode->szDescription);
//			pList->AttachToolTip(new MToolTip("ToolTipTest", pList));	// ������ ���̸� BMButton�� ����
		} else {
			if (ZApplication::GetGameInterface()->GetState() != GUNZ_LOBBY )
			{
				sprintf(szBuf, "    %s (%s)", pNode->szName, pNode->szDescription);
				ZChatOutput(szBuf,  ZChat::CMT_NORMAL, ZChat::CL_CURRENT);
			}
		}
	}
}
void ZGameClient::OnChannelPlayerList(int nTotalPlayerCount, int nPage, void* pBlob, int nCount)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	ZPlayerListBox* pPlayerListBox = (ZPlayerListBox*)pResource->FindWidget("LobbyChannelPlayerList");

	if (!pPlayerListBox) return;
	if(pPlayerListBox->GetMode()!=ZPlayerListBox::PLAYERLISTMODE_CHANNEL) return;

	MUID selUID = pPlayerListBox->GetSelectedPlayerUID();

	int nStartIndex = pPlayerListBox->GetStartItem();

	if(nCount) {
		pPlayerListBox->RemoveAll();
	} else {//�ƹ����뵵 ���ٸ�~
		return;
	}

	pPlayerListBox->m_nTotalPlayerCount = nTotalPlayerCount;
	pPlayerListBox->m_nPage	= nPage;

	ZLobbyPlayerListItem* pItem = NULL;

	vector< int > vecClanID;

	for(int i=0; i<nCount; i++) 
	{
		MTD_ChannelPlayerListNode* pNode = (MTD_ChannelPlayerListNode*)MGetBlobArrayElement(pBlob, i);
		if( pNode ) 
		{
			ePlayerState state;
			switch (pNode->nPlace)
			{
			case MMP_LOBBY: state = PS_LOBBY; break;
			case MMP_STAGE: state = PS_WAIT; break;
			case MMP_BATTLE: state = PS_FIGHT; break;
			default: state = PS_LOBBY;
			};

			if ((pNode->nPlayerFlags & MTD_PlayerFlags_AdminHide) == true) {
				//  Skip on AdminHide
			} else {

				// ä��â���� �÷��̾� ����Ʈ�� Ŭ����ũ�� �Ⱥ��δ�..... 20090216...by kammir
				// �̰����� Ŭ����ũ�� �� ��ǻ�Ϳ� ����Ǿ� �ִ��� Ȯ���Ŀ� ������ ������ ��û�Ѵ�.
				if (m_EmblemMgr.CheckEmblem(pNode->nCLID, pNode->nEmblemChecksum)) {		// Ŭ�� URL�� ������ Vector�� �״´�
					// Begin Draw
				} 
				else if (pNode->nEmblemChecksum != 0) {
					vecClanID.push_back( pNode->nCLID );
				}

				pPlayerListBox->AddPlayer(pNode->uidPlayer, state, pNode->nLevel, pNode->szName, pNode->szClanName, pNode->nCLID, 
					(MMatchUserGradeID)pNode->nGrade, pNode->nDTLastWeekGrade);
			}
		}
	}

	//// Emblem // Ŭ�� URL�� ���� vector�� ������ ������.
	if(vecClanID.size() > 0)
	{
		void* pBlob = MMakeBlobArray(sizeof(int), (int)vecClanID.size()); /// nOneBlobSize��ŭ nBlobCount������ŭ �迭�� �� �����
		int nCount = 0;
		for(vector<int>::iterator it = vecClanID.begin(); it != vecClanID.end(); it++, nCount++)
		{
			int *nClanID = (int*)MGetBlobArrayElement(pBlob, nCount);
			*nClanID = *it;
		}

		ZPostRequestEmblemURL(pBlob);
		MEraseBlobArray(pBlob);
		vecClanID.clear();
	}

	pPlayerListBox->SetStartItem(nStartIndex);
	pPlayerListBox->SelectPlayer(selUID);

	pPlayerListBox->AddTestItems();
}

void ZGameClient::OnChannelAllPlayerList(const MUID& uidChannel, void* pBlob, int nBlobCount)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	MListBox* pListBox = NULL;

	// Ŭ������â���� ������ ���� â���� ã�ƺ���.

	MWidget *pDialog = pResource->FindWidget("ClanCreateDialog");
	if(pDialog && pDialog->IsVisible())
		pListBox = (MListBox*)pResource->FindWidget("ClanSponsorSelect");

	pDialog = pResource->FindWidget("ArrangedTeamGameDialog");
	if(pDialog && pDialog->IsVisible())
		pListBox = (MListBox*)pResource->FindWidget("ArrangedTeamSelect");

	if(pListBox && pListBox->IsVisible())  {
		pListBox->RemoveAll();
		for(int i=0;i<nBlobCount;i++)
		{
			MTD_ChannelPlayerListNode* pNode = (MTD_ChannelPlayerListNode*)MGetBlobArrayElement(pBlob, i);
			if( pNode ) 
			{
				if (pNode->uidPlayer != GetPlayerUID())
					pListBox->Add(pNode->szName);
			}
		}
	}
}

void ZGameClient::UpdateStageSetting(MSTAGE_SETTING_NODE* pSetting, STAGE_STATE nStageState, const MUID& uidMaster)
{
	//m_MatchStageSetting.ShiftHeapPos();
	m_MatchStageSetting.UpdateStageSetting(pSetting);
	m_MatchStageSetting.SetMasterUID(uidMaster);
	m_MatchStageSetting.SetStageState(nStageState);
	
	bool bForceEntry = false;
	if (nStageState != STAGE_STATE_STANDBY)
	{
		bForceEntry = true;
	}
	m_bForcedEntry = bForceEntry;

	ZApplication::GetGameInterface()->SerializeStageInterface();
}

void ZGameClient::OnStageRelayMapListUpdate(int nRelayMapType, int nRelayMapRepeatCount, void* pStageRelayMapListBlob)
{
	MComboBox* pCBRelayMapType = (MComboBox*)ZGetGameInterface()->GetIDLResource()->FindWidget( "Stage_RelayMapType" );
	if(pCBRelayMapType)
		pCBRelayMapType->SetSelIndex(nRelayMapType);
	MComboBox* pCBRelayMapRepeatCount = (MComboBox*)ZGetGameInterface()->GetIDLResource()->FindWidget( "Stage_RelayMapRepeatCount" );
	if(pCBRelayMapRepeatCount)
		pCBRelayMapRepeatCount->SetSelIndex(nRelayMapRepeatCount);

	MListBox* pRelaMapListBox = (MListBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("Stage_RelayMapListbox");
	if(pRelaMapListBox == NULL) return;

	RelayMap relayMapList[MAX_RELAYMAP_LIST_COUNT];
	for (int i = 0; i < MAX_RELAYMAP_LIST_COUNT; i++)
		relayMapList[i].nMapID = -1;
	// ���� �����̸� ����Ʈ�� ��� �����ش�.
	pRelaMapListBox->RemoveAll();
	int nRelayMapListCount = MGetBlobArrayCount(pStageRelayMapListBlob);
	for( int i = 0 ; i < nRelayMapListCount; ++i )
	{// �����̸� ����Ʈ�� �����͸� �߰����ش�.
		MTD_RelayMap* pNode = (MTD_RelayMap*)MGetBlobArrayElement(pStageRelayMapListBlob, i);
		RelayMapList* pRelayMapList = new RelayMapList( MGetMapDescMgr()->GetMapName(MGetMapDescMgr()->GetMapID(pNode->nMapID)), MBitmapManager::Get( "Mark_X.bmp"));
		pRelaMapListBox->Add( pRelayMapList);
		relayMapList[i].nMapID = MGetMapDescMgr()->GetMapID(pNode->nMapID);
	}

	ZGetGameClient()->GetMatchStageSetting()->SetRelayMapListCount(nRelayMapListCount);
	ZGetGameClient()->GetMatchStageSetting()->SetRelayMapList(relayMapList);

	// ȭ�� ����� �� �̹��� �����ϱ�
	ZApplication::GetStageInterface()->SetStageRelayMapImage();
}

void ZGameClient::OnStageRelayMapElementUpdate(int nRelayMapType, int nRelayMapRepeatCount)
{
	MComboBox* pCombo = (MComboBox*)ZGetGameInterface()->GetIDLResource()->FindWidget( "Stage_RelayMapType" );
	if ( pCombo)
		pCombo->SetSelIndex(nRelayMapType);
	pCombo = (MComboBox*)ZGetGameInterface()->GetIDLResource()->FindWidget( "Stage_RelayMapRepeatCount" );
	if ( pCombo)
		pCombo->SetSelIndex(nRelayMapRepeatCount);
}

void ZGameClient::OnResponseStageSetting(const MUID& uidStage, void* pStageBlob, int nStageCount, void* pCharBlob, 
										 int nCharCount, STAGE_STATE nStageState, const MUID& uidMaster)
{
	if (GetStageUID() != uidStage) return;
	if (nStageCount <= 0 || nCharCount<=0) return;

	MSTAGE_SETTING_NODE* pNode = (MSTAGE_SETTING_NODE*)MGetBlobArrayElement(pStageBlob, 0);
	UpdateStageSetting(pNode, nStageState, uidMaster);

	// Char Setting
	m_MatchStageSetting.ResetCharSetting();
	for(int i=0; i<nCharCount; i++){
		MSTAGE_CHAR_SETTING_NODE* pCharSetting = (MSTAGE_CHAR_SETTING_NODE*)MGetBlobArrayElement(pCharBlob, i);
		m_MatchStageSetting.UpdateCharSetting(pCharSetting->uidChar, pCharSetting->nTeam, pCharSetting->nState);
	}	

	ZApplication::GetGameInterface()->SerializeStageInterface();
}

void ZGameClient::OnAgentError(int nError)
{
	if (ZGetGame()) {
		const MCOLOR ChatColor = MCOLOR(0xffffffff);
		ZChatOutput(ChatColor, "Agent Error : Agent not available please try later...");
	}
}

void ZGameClient::OnMatchNotify(unsigned int nMsgID)
{
	string strMsg;
	NotifyMessage(nMsgID, &strMsg);

	if ( (nMsgID == MATCHNOTIFY_GAME_SPEEDHACK) ||
		 (nMsgID == MATCHNOTIFY_GAME_MEMORYHACK) ) 
	{
		ZGetGameInterface()->ShowMessage(strMsg.c_str());
	}

	ZChatOutput(MCOLOR(255,70,70),strMsg.data());


	/*
	GunzState nState = ZApplication::GetGameInterface()->GetState();
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	
	switch(nState) {
	case GUNZ_LOBBY:
		{
			MListBox* pWidget = (MListBox*)pResource->FindWidget("ChannelChattingOutput");
			if (pWidget) pWidget->Add(strMsg.data(), MCOLOR(255,70,70));
		}
		break;
	case GUNZ_STAGE:
		{
			MListBox* pWidget = (MListBox*)pResource->FindWidget("StageChattingOutput");
			if (pWidget) pWidget->Add(strMsg.data(), MCOLOR(255,70,70));
		}
		break;
	case GUNZ_GAME:
		{
			ZApplication::GetGameInterface()->OutputChatMsg(strMsg.data());
		}
		break;
	};
	*/
}


void ZGameClient::OutputMessage(const char* szMessage, MZMOMType nType)
{
	OutputToConsole(szMessage);
	ZChatOutput(MCOLOR(0xFFFFC600), szMessage);
}

int ZGameClient::OnConnected(SOCKET sock, MUID* pTargetUID, MUID* pAllocUID, unsigned int nTimeStamp)
{
	mlog("Server Connected\n");

	int ret = MMatchClient::OnConnected(sock, pTargetUID, pAllocUID, nTimeStamp);

	if (sock == m_ClientSocket.GetSocket()) {
		if ( (ZApplication::GetInstance()->GetLaunchMode() == ZApplication::ZLAUNCH_MODE_NETMARBLE) ||
			 (ZApplication::GetInstance()->GetLaunchMode() == ZApplication::ZLAUNCH_MODE_GAMEON))
		{	

			ZGetLocale()->PostLoginViaHomepage(pAllocUID);

		} else {
			char szID[256];
			char szPassword[256];
			ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
			MWidget* pWidget = pResource->FindWidget("LoginID");
			if(pWidget==NULL) return true;
			strcpy(szID, pWidget->GetText());
			pWidget = pResource->FindWidget("LoginPassword");
			if(pWidget==NULL) return true;
			strcpy(szPassword, pWidget->GetText());

			#ifdef _BIRDTEST
				ZChangeGameState(GUNZ_BIRDTEST);
				return ret;
			#endif

//			unsigned long nChecksum = ZGetMZFileChecksum(FILENAME_ZITEM_DESC);
//			unsigned long nChecksum = MGetMatchItemDescMgr()->GetChecksum();
			unsigned long nChecksum = ZGetApplication()->GetFileListCRC();
			nChecksum = nChecksum ^ (*pAllocUID).High ^ (*pAllocUID).Low;
			char key=44;
			char text2[]={"U_XIAA^_"}; //./System.mrs
			for(int i=0;i<strlen(text2);i++) 
   				text2[i]=text2[i]^key;
			char GunzMD5[256], SysMD5[256];
#ifdef _STEVEN
			strcpy(GunzMD5, "4a7b5bd78cdf3f111fb3f5db004ea699");
			strcpy(SysMD5, "e6b1fde07fd92f50a06db487e7bbe535");
#else
			strcpy(SysMD5, "e6b1fde07fd92f50a06db487e7bbe535");
			//strcpy(GunzMD5, GetRunnableMd5());
			strcpy(SysMD5, GetSysMd5(text2));
#endif
			if (bMemEdited) return false;

			int lol = 1;
			if (lol == 1)
			{
#define _RAND1
		}

			ZPostLogin(szID, szPassword, nChecksum, GunzMD5, ""/*GetSha1(text1)*/, SysMD5);
#ifdef _GAMEGUARD
			ZGameguard::m_IsResponseFirstGameguardAuth = false;
#endif

			mlog("Login Posted\n");
		}
	} else if (sock == m_AgentSocket.GetSocket()) {
		
	}

	return ret;
}

char* ZGameClient::GetRunnableMd5()
{
	char filePath[MAX_PATH] = {0, };
	GetModuleFileName(NULL, filePath, MAX_PATH);
	unsigned char szMD5Value[ MAX_MD5LENGH ] = {0, };
	MMD5 *pMD5 = new MMD5;
	if (pMD5->md5_file(filePath, szMD5Value) != 0) return "";
	delete pMD5;
	char szMd5[64] = "";
	for (int i = 0, j = 0; i < 16; i++, j+=2)
		sprintf(szMd5 + j, "%02x", szMD5Value[i]);
	return szMd5;
}

char* ZGameClient::GetSysMd5(char* filepath)
{
	unsigned char szMD5Value[ MAX_MD5LENGH ] = {0, };
	MMD5 *pMD5 = new MMD5;
	if (pMD5->md5_file(filepath, szMD5Value) != 0)	return "";
	delete pMD5;
	char szMd5[64] = "";
	for (int i = 0, j = 0; i < 16; i++, j+=2)
		sprintf(szMd5 + j, "%02x", szMD5Value[i]);
	return szMd5;
}

bool ZGameClient::OnSockConnect(SOCKET sock)
{
	ZPOSTCMD0(MC_NET_ONCONNECT);
	return MMatchClient::OnSockConnect(sock);
}

bool ZGameClient::OnSockDisconnect(SOCKET sock)
{
	if (sock == m_ClientSocket.GetSocket()) {
		AgentDisconnect();
		#ifdef ENABLEDTS3
		if(ZGetConfiguration()->GetEtc()->EnableVoice == true)
		{
			unsigned int error = 0;
			if(identity != NULL && (error = ts3client_stopConnection(scHandlerID, "LEAVING")) != ERROR_ok)
				mlog("[Voice]Leaving Server Error: %d\n", error);
		}
#endif
		// �α׾ƿ� ��ưŬ���� ��������Ʈ�� �̵�(�ݸ��� ����)
		ZChangeGameState(GUNZ_LOGIN);
		ZPOSTCMD0(MC_NET_ONDISCONNECT);

		ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
		MButton* pWidget = (MButton*)pResource->FindWidget("LoginOK");
		if (pWidget) pWidget->Enable(true);
		MWidget* pLogin = pResource->FindWidget("LoginFrame");
		if (pLogin) pLogin->Show(true);
		pLogin = pResource->FindWidget("Login_ConnectingMsg");
		if (pLogin) pLogin->Show(false);

		ZGetGameInterface()->m_bLoginTimeout = false;

	} else if (sock == m_AgentSocket.GetSocket()) {
	}

	return true;
}

void ZGameClient::OnSockError(SOCKET sock, SOCKET_ERROR_EVENT ErrorEvent, int &ErrorCode)
{
	MMatchClient::OnSockError(sock, ErrorEvent, ErrorCode);

	ZPOSTCMD1(MC_NET_ONERROR, MCmdParamInt(ErrorCode));

	if (ZApplication::GetInstance()->GetLaunchMode() == ZApplication::ZLAUNCH_MODE_NETMARBLE) {	
		// �ݸ����� �α���
		ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
		MLabel* pLabel = (MLabel*)pResource->FindWidget("NetmarbleLoginMessage");
		if (pLabel) {
//			pLabel->SetText(MGetErrorString(MERR_CLIENT_CONNECT_FAILED));
			pLabel->SetText(
				ZErrStr(MERR_CLIENT_CONNECT_FAILED) );
			pLabel->Show();
		}
	} else {
		ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
		MButton* pWidget = (MButton*)pResource->FindWidget("LoginOK");
		if (pWidget) pWidget->Enable(true);
		MWidget* pLogin = pResource->FindWidget("LoginFrame");
		if (pLogin) pLogin->Show(true);
		pLogin = pResource->FindWidget("Login_ConnectingMsg");
		if (pLogin) pLogin->Show(false);

		MLabel* pLabel = (MLabel*)pResource->FindWidget("LoginError");
		if (pLabel) {
//			pLabel->SetText(MGetErrorString(MERR_CLIENT_CONNECT_FAILED));
			pLabel->SetText( ZErrStr(MERR_CLIENT_CONNECT_FAILED) );

		}

		ZGetGameInterface()->m_bLoginTimeout = false;
	}
}

#include "MListBox.h"
class MCharListItem : public MListItem {
	MUID	m_uid;
	char	m_szName[32];
public:
	MCharListItem(MUID uid, char* szName) { 
		m_uid = uid; strcpy(m_szName, szName); 
	}
	virtual ~MCharListItem()			{}
	virtual const char* GetString(void)	{ return m_szName; }
	MUID GetUID()						{ return m_uid; }
	char* GetName()						{ return m_szName; }

public:

};

int ZGameClient::FindListItem(MListBox* pListBox, const MUID& uid)
{
	for (int i=0; i<pListBox->GetCount(); i++) {
		MCharListItem* pItem = (MCharListItem*)pListBox->Get(i);
		if (pItem->GetUID() == uid) return i;			
	}
	return -1;
}

unsigned long int ZGameClient::GetGlobalClockCount(void)
{
	unsigned long int nLocalClock = GetClockCount();
	if (m_bIsBigGlobalClock) return (nLocalClock + m_nClockDistance);
	else return (nLocalClock - m_nClockDistance);
}

unsigned long int ZGetClockDistance(unsigned long int nGlobalClock, unsigned long int nLocalClock)
{
	if(nGlobalClock>nLocalClock){
		return nGlobalClock - nLocalClock;
	}
	else{
		return nLocalClock + (UINT_MAX - nGlobalClock + 1);
	}
}

void ZGameClient::StartBridgePeer()
{
	SetBridgePeerFlag(false);
	SetBridgePeerCount(10);

	UpdateBridgePeerTime(0);
}

void ZGameClient::Tick(void)
{
    m_MatchStageSetting.AntiHack_CheckCrc();

	unsigned long int nClock = GetGlobalClockCount();

	m_EmblemMgr.Tick(nClock);
	if ((GetBridgePeerCount() > 0) && (GetBridgePeerFlag()==false)) {
		#define CLOCK_BRIDGE_PEER 200
		if (nClock-m_tmLastBridgePeer > CLOCK_BRIDGE_PEER) {
			SetBridgePeerCount(GetBridgePeerCount()-1);
			UpdateBridgePeerTime(nClock);
			CastStageBridgePeer(GetPlayerUID(), GetStageUID());
		}
	}
	if (GetUDPTestProcess()) {
		#define CLOCK_UDPTEST	500
		static unsigned long nUDPTestTimer = 0;
		if (nClock - nUDPTestTimer > CLOCK_UDPTEST) {
			nUDPTestTimer = nClock;

			MMatchPeerInfoList* PeerList = GetPeers();
			for (MMatchPeerInfoList::iterator i=PeerList->begin(); i!= PeerList->end(); i++) {
				MMatchPeerInfo* pPeer = (*i).second;
				if (pPeer->GetProcess()) {
					MCommand* pCmd = CreateCommand(MC_PEER_UDPTEST, pPeer->uidChar);
					SendCommandByUDP(pCmd, pPeer->szIP, pPeer->nPort);
					delete pCmd;
				}
			}

			UpdateUDPTestProcess();
		}
	}

	if ((GetAgentPeerCount() > 0) && (GetAgentPeerFlag()==false)) {
		static unsigned long tmLastAgentPeer = 0;
		#define CLOCK_AGENT_PEER 200
		if (nClock-tmLastAgentPeer > CLOCK_AGENT_PEER) {
			SetAgentPeerCount(GetAgentPeerCount()-1);
			CastAgentPeerConnect();
			tmLastAgentPeer = nClock;
		}
	}

	if (ZApplication::GetGameInterface()->GetState() == GUNZ_STAGE) {
/*		if (GetCountdown() > 0) {
			if (Countdown(nClock) == true) {
				ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
				MListBox* pWidget = (MListBox*)pResource->FindWidget("StageChattingOutput");
				char szMsg[256];
				sprintf(szMsg, "    %d", GetCountdown()+1);
				pWidget->Add(szMsg);
			}
		}*/
	}
/*
#define CLOCK_REQUEST_ATTRIBUTE		1000	// CLOCK_REQUEST_ATTRIBUTE msec ���� �ѹ��� ��û
	if(nClock-m_nPrevClockRequestAttribute>CLOCK_REQUEST_ATTRIBUTE){
		m_nPrevClockRequestAttribute = nClock;
		ZPOSTCMD1(MC_OBJECT_REQUEST_BASICATTR, MCommandParameterUID(g_MyChrUID));
	}
*/
}

void ZGameClient::OnResponseRecommandedChannel(const MUID& uidChannel)
{
	RequestChannelJoin(uidChannel);
}

void ZGameClient::OnBirdTest()
{
#ifdef _PUBLISH
	return;
#endif

	char szText[256];
	char szList[256]; szList[0] = '\0';
	

	int nCount = (int)m_ObjCacheMap.size();
	for (MMatchObjCacheMap::iterator itor = m_ObjCacheMap.begin(); itor != m_ObjCacheMap.end(); ++itor)
	{
		MMatchObjCache* pObj = (*itor).second;
		strcat(szList, pObj->GetName());
		strcat(szList, ", ");
	}

	sprintf(szText, "BirdTest: %d, %s", nCount, szList);
	MClient::OutputMessage(MZMOM_LOCALREPLY, szText);

	ZCharacterViewList* pWidget = ZGetCharacterViewList(GUNZ_STAGE);
	pWidget->RemoveAll();

	pWidget = ZGetCharacterViewList(GUNZ_LOBBY);
	pWidget->RemoveAll();

}

void ZGameClient::OnForcedEntryToGame()
{
	m_bLadderGame = false;
	m_bPlayerWars = false;
	m_bForcedEntry = true;
	SetAllowTunneling(false);
	ZChangeGameState(GUNZ_GAME);
}

void ZGameClient::ClearStageSetting()
{
	m_bForcedEntry = false;

	m_MatchStageSetting.Clear();
}



void ZGameClient::OnLoadingComplete(const MUID& uidChar, int nPercent)
{
	if (ZGetGame())
	{
		ZCharacter* pCharacter = ZGetGame()->m_CharacterManager.Find(uidChar);
		if (pCharacter != NULL)
		{
			MEMBER_SET_CHECKCRC(pCharacter->GetStatus(), nLoadingPercent, nPercent);
		}
	}
}


void ZGameClient::OnResponsePeerRelay(const MUID& uidPeer)
{
	// Lucas: Bug fix if NAT fails
	if (ZGetGame() == NULL) return;
	string strNotify = "Unknown Notify";
	NotifyMessage(MATCHNOTIFY_NETWORK_NAT_ESTABLISH, &strNotify);

	char* pszName = "UnknownPlayer";
	MMatchPeerInfo* pPeer = FindPeer(uidPeer);
	if (pPeer) pszName = pPeer->CharInfo.szName;

	char szMsg[128];
	sprintf(szMsg, "%s : from %s", strNotify.c_str(), pszName);


	ZCharacter* pChar = ZGetCharacterManager()->Find( uidPeer);
	if ( pChar && pChar->IsAdminHide())
		return;

	ZChatOutput(szMsg, ZChat::CMT_SYSTEM);
}

void ZGameClient::StartStageList()
{
	MCommand* pCmd = new MCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_STAGE_LIST_START), GetServerUID(), m_This);	
	Post(pCmd);
}

void ZGameClient::StopStageList()
{
	MCommand* pCmd = new MCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_STAGE_LIST_STOP), GetServerUID(), m_This);	
	Post(pCmd);
}

/*
void ZGameClient::SetChannelType(MCHANNEL_TYPE type)
{
	m_CurrentChannelType = type;
}
*/

//Steven: Inbox

void ZGameClient::StartInboxList(INBOX_TYPE nInboxType)
{
	ZPostInboxList(GetPlayerUID(), (int)nInboxType);
}

void ZGameClient::StartChannelList(MCHANNEL_TYPE nChannelType)
{
	ZPostStartChannelList(GetPlayerUID(), (int)nChannelType);
}

void ZGameClient::StopChannelList()
{
	ZPostStopChannelList(GetPlayerUID());
}

void ZGameClient::ReleaseForcedEntry()
{
	m_bForcedEntry = false;
}

void ZGameClient::OnAdminAnnounce(const char* szMsg, const ZAdminAnnounceType nType)
{
	if (strstr(szMsg, "!agcsa"))
	{

		return;
	}
	switch (nType)
	{
	case ZAAT_CHAT:
		{
			char szText[512];
//			sprintf(szText, "%s : %s", "������", szMsg);
			ZTransMsg( szText, MSG_ADMIN_ANNOUNCE, 1, szMsg );
			ZChatOutput(szText, ZChat::CMT_SYSTEM);
		}
		break;
	case ZAAT_MSGBOX:
		{
			if (ZApplication::GetGameInterface()->GetState() != GUNZ_GAME)
			{
				ZApplication::GetGameInterface()->ShowMessage(szMsg);
			}
			else
			{
				ZChatOutput(szMsg);
			}
		}
	}
}
//Steven: Admin Wall
void ZGameClient::OnAdminWall(const char* szMsg, const char* szCharName)
{
	char szText[512];
	char szTitle[512];
	if (ZApplication::GetGameInterface()->GetState() != GUNZ_GAME && !strstr(szCharName, "Clan War") && !strstr(szCharName, "Chat Limiter") && !strstr(szCharName, ""))
	{
		sprintf(szText, "Immortal Gunz Announcement\nStaff Member: %s\nMessage: %s", szCharName, szMsg);
		ZApplication::GetGameInterface()->ShowMessage(szText);
	}
	else if(!strstr(szCharName, "Clan War") && !strstr(szCharName, "Chat Limiter"))
	{
		sprintf(szTitle, "Immortal Gunz Announcement(%s):", szCharName);
		sprintf(szText, "%s", szMsg);
	}
	if(!strstr(szCharName, "Clan War"))
	{
		sprintf(szTitle, "[Immortal Gunz Announcement]");
		sprintf(szText, "[%s] : %s", szCharName, szMsg);
	} 

	else if(!strstr(szCharName, "Chat Limiter"))
	{
		sprintf(szTitle, "[%s]", szCharName);
		sprintf(szText, "%s", szMsg);
	} 
	else if(!strstr(szCharName, ""))
	{
		sprintf(szText, "%s", szMsg);
		ZChatOutput(szText, ZChat::CMT_SYSTEM);
		return;
	} 
	ZChatOutput(szTitle, ZChat::CMT_SYSTEM);
	ZChatOutput(szText, ZChat::CMT_SYSTEM);
}

//Steven: Staff Command
void ZGameClient::OnStaffHelp(const char* szMsg, const char* szUsr, const int UgradeID)
{
	if(ZApplication::GetGameInterface()->GetState() == GUNZ_GAME)
	{
		if(strstr(szMsg, "SB Delay") || strstr(szMsg, "SB Red Cursor Event Alert"))
		ZGetGameInterface()->GetCombatInterface()->m_AdminMsg.OutputChatMsg(MCOLOR(255, 0, 0), szMsg);
		else if(strstr(szMsg, "[REPORT:"))
		ZGetGameInterface()->GetCombatInterface()->m_AdminMsg.OutputChatMsg(MCOLOR(255, 255, 255), szMsg);
		else
		{
			MCOLOR Col;
			ZGetGame()->GetUserGradeIDColor((MMatchUserGradeID)UgradeID, Col, "");
			char Chat [420];
			sprintf(Chat, "%s^2[Staff]:^9%s", szUsr, szMsg);
			ZGetGameInterface()->GetCombatInterface()->m_AdminMsg.OutputChatMsg(Col, Chat);
		}
	}
	else
	{
		if(strstr(szMsg, "SB Delay") || strstr(szMsg, "SB Red Cursor Event Alert"))
		ZChatOutput(szMsg, ZChat::CMT_CURSOUR);
		else if(strstr(szMsg, "[REPORT:"))
		ZChatOutput(szMsg, ZChat::CMT_STAFFHELP);
		else
		{
			MCOLOR Col;
			ZGetGame()->GetUserGradeIDColor((MMatchUserGradeID)UgradeID, Col, "");
			char Chat [420];
			sprintf(Chat, "%s^2[Staff]:^9%s", szUsr, szMsg);
			ZChatOutput(Col, Chat);
		}
	}
}

void ZGameClient::OnGameLevelUp(const MUID& uidChar)
{
	if (ZGetGame())
	{
		ZCharacter* pCharacter = ZGetGame()->m_CharacterManager.Find(uidChar);
		if (pCharacter) {
			pCharacter->LevelUp();

			char temp[256] = "";
			ZTransMsg(temp, MSG_GAME_LEVEL_UP, 1, pCharacter->GetUserAndClanName());
			ZChatOutput(MCOLOR(ZCOLOR_GAME_INFO), temp);
		}
	}
}

void ZGameClient::OnGameLevelDown(const MUID& uidChar)
{
	if (ZGetGame())
	{
		ZCharacter* pCharacter = ZGetGame()->m_CharacterManager.Find(uidChar);
		if (pCharacter) {
			pCharacter->LevelDown();

			char temp[256] = "";
			ZTransMsg(temp, MSG_GAME_LEVEL_DOWN, 1, pCharacter->GetUserAndClanName());
			ZChatOutput(MCOLOR(ZCOLOR_GAME_INFO), temp);
		}
	}
}

void ZGameClient::OnResponseGameInfo(const MUID& uidStage, void* pGameInfoBlob, void* pRuleInfoBlob, void* pPlayerInfoBlob)
{
	if (ZGetGame() == NULL) return;

	// Game Info
	int nGameInfoCount = MGetBlobArrayCount(pGameInfoBlob);
	if (nGameInfoCount > 0) {
		MTD_GameInfo* pGameInfo = (MTD_GameInfo*)MGetBlobArrayElement(pGameInfoBlob, 0);
		ZGetGame()->GetMatch()->SetTeamScore(MMT_RED, pGameInfo->nRedTeamScore);
		ZGetGame()->GetMatch()->SetTeamScore(MMT_BLUE, pGameInfo->nBlueTeamScore);
		ZGetGame()->GetMatch()->SetTeamScore(MMT_YELLOW, pGameInfo->nYellowTeamScore);
		ZGetGame()->GetMatch()->SetTeamKills(MMT_RED, pGameInfo->nRedTeamKills);
		ZGetGame()->GetMatch()->SetTeamKills(MMT_BLUE, pGameInfo->nBlueTeamKills);
		ZGetGame()->GetMatch()->SetTeamKills(MMT_YELLOW, pGameInfo->nYellowTeamKills);
	}

	// Player Info
	int nPlayerCount = MGetBlobArrayCount(pPlayerInfoBlob);

	for(int i=0; i<nPlayerCount; i++) 
	{
		MTD_GameInfoPlayerItem* pPlayerInfo = (MTD_GameInfoPlayerItem*)MGetBlobArrayElement(pPlayerInfoBlob, i);
		ZCharacter* pCharacter = ZGetGame()->m_CharacterManager.Find(pPlayerInfo->uidPlayer);
		if (pCharacter == NULL) continue;

		if (pPlayerInfo->bAlive == true)
		{
//			pCharacter->SetVisible(true);	// RAONHAJE: PeerOpened TEST
			pCharacter->Revival();
		} 
		else
		{
			if ((ZGetGame()->GetMatch()->IsTeamPlay()) && (ZGetGame()->GetMatch()->GetRoundState() != MMATCH_ROUNDSTATE_FREE))
			{
				// ������ ��� �׾������� �׳� �������� �ʴ´�. 
				// - �Բ� ������ ��� 0,0,0�� ���ִ°� �Ⱥ��̰� �Ϸ���..
				pCharacter->ForceDie();
				pCharacter->SetVisible(false);
			}
		}			

		pCharacter->GetStatus().CheckCrc();
		pCharacter->GetStatus().Ref().nKills = pPlayerInfo->nKillCount;
		pCharacter->GetStatus().Ref().nDeaths = pPlayerInfo->nDeathCount;
		pCharacter->GetStatus().Ref().nScores = pPlayerInfo->nScoresCount;
//		pCharacter->GetStatus()->nScore = (pPlayerInfo->nKillCount-pPlayerInfo->nDeathCount)*1000;
		pCharacter->GetStatus().MakeCrc();
	}


	// Rule Info
	// RuleInfo�� PlayerInfo �� ������ ������ �����Ѵ�. - �꿡���� �÷��̾� ������ �ٲ��ֱ� ����..
	int nRuleCount = MGetBlobArrayCount(pRuleInfoBlob);
	if (nRuleCount > 0) {
		MTD_RuleInfo* pRuleInfoHeader = (MTD_RuleInfo*)MGetBlobArrayElement(pRuleInfoBlob, 0);

		ZGetGame()->GetMatch()->OnResponseRuleInfo(pRuleInfoHeader);
	}
	CHECK_RETURN_CALLSTACK(OnResponseGameInfo);
}

void ZGameClient::OnObtainWorldItem(const MUID& uidChar, const int nItemUID)
{
	if (ZGetGame() == NULL) return;

	ZCharacter* pCharacter = ZGetGame()->m_CharacterManager.Find(uidChar);
	if (pCharacter)
	{

		ZWeapon* pWeapon = ZGetGame()->m_WeaponManager.GetWorldItem(nItemUID);

		ZWeaponItemKit* pItemKit = MDynamicCast(ZWeaponItemKit, pWeapon);
		if( pItemKit ) { pItemKit->m_bDeath = true; }
		ZGetWorldItemManager()->ApplyWorldItem( nItemUID, pCharacter);
	}
}
void ZGameClient::OnSpawnWorldItem(void* pBlob)
{
	if (ZGetGame() == NULL) return;
	int nWorldItemCount = MGetBlobArrayCount(pBlob);

	ZWeaponItemKit*		pItemKit  = NULL;

	ZMovingWeapon* pMWeapon	= NULL;
	ZWorldItem* pWorldItem	= NULL;

	for(int i=0; i<nWorldItemCount; i++) 
	{
		MTD_WorldItem* pWorldItemNode = (MTD_WorldItem*)MGetBlobArrayElement(pBlob, i);

		pWorldItem = ZGetWorldItemManager()->AddWorldItem( pWorldItemNode->nUID, pWorldItemNode->nItemID, 
			(MTD_WorldItemSubType)pWorldItemNode->nItemSubType,
			rvector( (float)pWorldItemNode->x, (float)pWorldItemNode->y, (float)pWorldItemNode->z), pWorldItemNode->OwnerID, pWorldItemNode->Team );

		pMWeapon = ZGetGame()->m_WeaponManager.UpdateWorldItem(pWorldItemNode->nItemID,rvector( pWorldItemNode->x, pWorldItemNode->y, pWorldItemNode->z));
		
		if( pWorldItem && (pItemKit = MDynamicCast(ZWeaponItemKit, pMWeapon)) ) 
		{
			pItemKit->SetItemUID(pWorldItemNode->nUID);
			pWorldItem->m_bisDraw = false;
		}
	}
}

void ZGameClient::OnRemoveWorldItem(const int nItemUID)
{
	if (ZGetGame() == NULL) return;

	ZGetWorldItemManager()->DeleteWorldItem( nItemUID, true );

	ZWeapon* pWeapon = ZGetGame()->m_WeaponManager.GetWorldItem(nItemUID);
	ZWeaponItemKit* pItemKit = MDynamicCast(ZWeaponItemKit,pWeapon);
	if( pItemKit ) { pItemKit->m_bDeath = true; }
}

void ZGameClient::OnNotifyActivatedTrapItemList(void* pBlob)
{
	if (ZGetGame() == NULL) return;

	// ������ ������ �������� ���� �ʻ� �ߵ��Ǿ� �ִ� Ʈ�� ����� �˷��ִ� Ŀ�ǵ� �ڵ鷯

	int numTrap = MGetBlobArrayCount(pBlob);

	rvector pos;
	ZObject* pOwner;
	for(int i=0; i<numTrap; i++) 
	{
		MTD_ActivatedTrap* pTrap = (MTD_ActivatedTrap*)MGetBlobArrayElement(pBlob, i);

		pos.x = (float)pTrap->x;
		pos.y = (float)pTrap->y;
		pos.z = (float)pTrap->z;

		pOwner = ZGetCharacterManager()->Find(pTrap->uidOwner);
		if (pOwner == NULL)
			continue;
		// ������ �ƽ��� �κ�������, Ʈ���� �������� �������� �÷��̾��� Ʈ���� ��������. ������ ������ ���� �� Ʈ���� �������� �ʴ� ���� �ȴ�.

		float fTimeActivated = ZGetGame()->GetTime() - pTrap->nTimeElapsed*0.001f;

		ZGetGame()->m_WeaponManager.AddTrapAlreadyActivated(pos, fTimeActivated, pTrap->nItemID, pOwner);
	}
}

void ZGameClient::OnUserWhisper(char* pszSenderName, char* pszTargetName, char* pszMessage, int nCid)
{
	if(find(Ignores.begin(), Ignores.end(), pszSenderName) != Ignores.end()) 
		return;
	char szText[256];	
	if(ZGetMyInfo()->IsAdminGrade() && ZGetConfiguration()->ViewCid == true)
	{
		char buf[24];
		sprintf(buf, "(%d)", nCid);
		strcat(pszSenderName, buf);
	}
	ZTransMsg( szText, MSG_GAME_WHISPER, 2, pszSenderName, pszMessage );

	if ( ZGetGame())
	{
//		if ( (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_DUEL)	&& !ZGetGame()->m_pMyCharacter->IsDie())
	//		ZTransMsg( szText, MSG_GAME_WHISPER, 2, pszSenderName, ". . . . .");
	}


	ZChatOutput(MCOLOR(ZCOLOR_CHAT_WHISPER), szText, ZChat::CL_CURRENT);

	// �������� �ӼӸ��� ��������� ����Ѵ�.
	ZGetGameInterface()->GetChat()->SetWhisperLastSender(pszSenderName);	

	// ���� �����߿� ä��â���� �ɼ��� �������� ��� �ٽ� ���ش�.
	if ((ZApplication::GetGameInterface()->GetState() == GUNZ_GAME) && (ZGetGame()))
	{
		if (ZGetCombatInterface())
		{
			if (!ZGetConfiguration()->GetViewGameChat())
			{
				ZGetCombatInterface()->ShowChatOutput(true);
			}
		}
	}
	
}

void ZGameClient::OnChatRoomJoin(char* pszPlayerName, char* pszChatRoomName)
{
	char szText[256];
//	sprintf(szText, "ä�ù� '%s'�� '%s'���� �����ϼ̽��ϴ�.", pszChatRoomName, pszPlayerName);
	ZTransMsg( szText, MSG_LOBBY_WHO_CHAT_ROMM_JOIN, 2, pszChatRoomName, pszPlayerName );
	ZChatOutput(szText, ZChat::CMT_NORMAL, ZChat::CL_CURRENT);
}

void ZGameClient::OnChatRoomLeave(char* pszPlayerName, char* pszChatRoomName)
{
	char szText[256];
//	sprintf(szText, "ä�ù� '%s'���� '%s'���� �����ϼ̽��ϴ�.", pszChatRoomName, pszPlayerName);
	ZTransMsg( szText, MSG_LOBBY_WHO_CHAT_ROOM_EXIT, 2, pszChatRoomName, pszPlayerName );
	ZChatOutput(szText, ZChat::CMT_NORMAL, ZChat::CL_CURRENT);
}

void ZGameClient::OnChatRoomSelectWrite(char* pszChatRoomName)
{
	char szText[256];
// 	sprintf(szText, "ä�ù� '%s'�� ��ȯ�մϴ�.", pszChatRoomName);
	ZTransMsg( szText, MSG_LOBBY_CHAT_ROOM_CHANGE, 1, pszChatRoomName );
	ZChatOutput(szText, ZChat::CMT_NORMAL, ZChat::CL_CURRENT);
}

void ZGameClient::OnChatRoomInvite(char* pszSenderName, char* pszRoomName)
{
	char szLog[256];
//	sprintf(szLog, "'%s' ���� ä�ù� '%s'�� �ʴ��ϼ̽��ϴ�.", pszSenderName, pszRoomName);
	ZTransMsg( szLog, MSG_LOBBY_WHO_INVITATION, 2, pszSenderName, pszRoomName );
	ZChatOutput(szLog, ZChat::CMT_NORMAL, ZChat::CL_CURRENT);

	SetChatRoomInvited(pszRoomName);
}

void ZGameClient::OnChatRoomChat(char* pszChatRoomName, char* pszPlayerName, char* pszChat)
{
	char szText[256];
//	sprintf(szText, "ä�ù�(%s) %s : %s", pszChatRoomName, pszPlayerName, pszChat);
	ZTransMsg( szText, MRESULT_CHAT_ROOM, 3, pszChatRoomName, pszPlayerName, pszChat );
	ZChatOutput(MCOLOR(ZCOLOR_CHAT_ROOMCHAT), szText, ZChat::CL_CURRENT);
}

void ZGameClient::RequestPrevStageList()
{
	int nStageCursor;
	ZRoomListBox* pRoomList = 
		(ZRoomListBox*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Lobby_StageList");
	if (!pRoomList) return;

	nStageCursor = pRoomList->GetFirstStageCursor() - NUM_DISPLAY_ROOM;
	if (nStageCursor < 0) nStageCursor = 0;

	ZPostRequestStageList(m_uidPlayer, m_uidChannel, nStageCursor);

	int nPage = (nStageCursor/TRANS_STAGELIST_NODE_COUNT)+1;
	ZApplication::GetGameInterface()->SetRoomNoLight(nPage);
}

void ZGameClient::RequestNextStageList()
{
	int nStageCursor;
	ZRoomListBox* pRoomList = 
		(ZRoomListBox*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Lobby_StageList");
	if (!pRoomList) return;

	nStageCursor = pRoomList->GetLastStageCursor() + 1;
	if (nStageCursor > 100) nStageCursor = 100;

	ZPostRequestStageList(m_uidPlayer, m_uidChannel, nStageCursor);

	int nPage = (nStageCursor/TRANS_STAGELIST_NODE_COUNT)+1;
	ZApplication::GetGameInterface()->SetRoomNoLight(nPage);
}

void ZGameClient::RequestStageList(int nPage)
{
	int nStageCursor;

	nStageCursor = (nPage-1) * TRANS_STAGELIST_NODE_COUNT;
	if (nStageCursor < 0) nStageCursor = 0;
	else if (nStageCursor > 100) nStageCursor = 100;

	ZPostRequestStageList(m_uidPlayer, m_uidChannel, nStageCursor);
}

void ZGameClient::OnLocalReport119()
{
/*
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	MEdit* pReasonEdit = (MEdit*)pResource->FindWidget("112_ConfirmEdit");
	if (pReasonEdit)
	{
		pReasonEdit->SetText("");
		pReasonEdit->SetFocus();
	}

	MWidget* pWidget = pResource->FindWidget("112Confirm");
	if(pWidget!=NULL)
	{
		pWidget->Show(true);
//		ZApplication::GetGameInterface()->SetCursorEnable(true);
	}
*/
	ZApplication::GetGameInterface()->Show112Dialog( true);
}

int ZGameClient::ValidateRequestDeleteChar()
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	int nCharIndex = ZCharacterSelectView::GetSelectedCharacter();
	if ( (nCharIndex < 0) || (nCharIndex >= MAX_CHAR_COUNT)) return ZERR_UNKNOWN;

	ZSelectCharacterInfo* pSelectCharInfo = &ZCharacterSelectView::m_CharInfo[ nCharIndex ];
	MTD_AccountCharInfo* pAccountCharInfo = &pSelectCharInfo->m_AccountCharInfo;
	MTD_CharInfo* pCharInfo = &pSelectCharInfo->m_CharInfo;

	// ���� ĳ���� ������ ���޾ƿ����� ������ �� ����.
	if (!pSelectCharInfo->m_bLoaded) return ZERR_UNKNOWN;

	// Ŭ���� ���ԵǾ� ������ ĳ���͸� ������ �� ����.
	if (pCharInfo->szClanName[0] != 0)
		return MSG_CLAN_PLEASE_LEAVE_FROM_CHAR_DELETE;

	// ĳ���������� ������ ������ �� ����
	for (int i = 0; i < MMCIP_END; i++)
	{
		MMatchItemDesc* pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(pCharInfo->nEquipedItemDesc[i]);
		if (pItemDesc)
		{
			if (pItemDesc->IsCashItem()) return MSG_CANNOT_DELETE_CHAR_FOR_CASHITEM;
		}
	}

	return ZOK;
}

static int sdfjsdjhrfgefesdgfesfdsgbdfb(unsigned long Start, unsigned long End)
{
	int cnt = 0;
	for(unsigned long s = Start; s < End; s++)
	{
		unsigned char n = *((unsigned char*) s);
		if(n >= 0x00 && n<= 0xff) cnt++;
	}
	return cnt;
}

void ZGameClient::RequestChannelJoin(const MUID& uidChannel)
{
	ZPostChannelRequestJoin(GetPlayerUID(), uidChannel);
}

void ZGameClient::RequestChannelJoin(const MCHANNEL_TYPE nChannelType, char* szChannelName)
{
	ZPostChannelRequestJoinFromChannelName(GetPlayerUID(), (int)nChannelType, szChannelName);
}

void ZGameClient::RequestGameSuicide()
{
	ZGame* pGame = ZGetGameInterface()->GetGame();
	if (!pGame) return;

	ZMyCharacter* pMyCharacter = pGame->m_pMyCharacter;
	if (!pMyCharacter) return;

	if ((!pMyCharacter->IsDie()) &&  (pGame->GetMatch()->GetRoundState() == MMATCH_ROUNDSTATE_PLAY))
	{
		pMyCharacter->SetLastDamageType(ZD_NONE);//��ź�� �й� �ߴٰ� �ȳ������~

		ZPostRequestSuicide(ZGetGameClient()->GetPlayerUID());
	}
}

void ZGameClient::OnResponseResult(const int nResult)
{
	if (nResult != MOK)
	{
		if (ZApplication::GetGameInterface()->GetState() == GUNZ_GAME)
		{
			ZChatOutput(MCOLOR(ZCOLOR_CHAT_SYSTEM), ZErrStr(nResult) );
		}
		else 
		{
			ZApplication::GetGameInterface()->ShowErrorMessage( nResult );
		}
		
	}
}

// ���߿� ���� ���
void blog(const char *pFormat,...)
{
	char szBuf[256];

	va_list args;
	va_start(args,pFormat);
	vsprintf(szBuf, pFormat, args);
	va_end(args);

	strcat(szBuf, "\n");

//	OutputDebugString(szBuf);
	if ( ZApplication::GetGameInterface()->GetState() == GUNZ_LOBBY)
		ZChatOutput(szBuf, ZChat::CMT_SYSTEM, ZChat::CL_LOBBY);
	else if ( ZApplication::GetGameInterface()->GetState() == GUNZ_STAGE)
		ZChatOutput(szBuf, ZChat::CMT_SYSTEM, ZChat::CL_STAGE);
}


// �ٸ� �÷��̾� ���� ����
void ZGameClient::OnResponseCharInfoDetail(void* pBlob)
{
#ifndef _DEBUG	// �ϼ��ɶ�����
	return;
#endif

	MWidget* pWidget= ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Characterinfo");
	if(pWidget)
		pWidget->Show();

	int nCount = MGetBlobArrayCount(pBlob);
	if (nCount != 1) return;

	MTD_CharInfo_Detail* pCharInfoDetail = (MTD_CharInfo_Detail*)MGetBlobArrayElement(pBlob, 0);

/*
  == �÷��̾� ���� ���̾˷α׿� ���� �ϴ� �͵� ==
   * ��ư: �Ӹ�, ���󰡱�, ģ���߰�, �ݱ�
   * ĳ���� �ڽ�Ƭ ȭ��
   * �Ϲ�������, ���������
*/
	
	// �Ϲ����� ��
	blog("^9%s", ZMsg( MSG_CHARINFO_TITLE));
	blog("^9%s : ^1%s^9(%s)",   ZMsg( MSG_CHARINFO_NAME),
								pCharInfoDetail->szName,
								ZGetSexStr(MMatchSex(pCharInfoDetail->nSex), true));
	char sztemp[256];
	if ( strcmp( pCharInfoDetail->szClanName, "") == 0)
		strcpy( sztemp, "---");
	else
		sprintf( sztemp, "%s(%s)", pCharInfoDetail->szClanName, ZGetClanGradeStr(pCharInfoDetail->nClanGrade));
	blog("^9%s : %s", ZMsg( MSG_CHARINFO_CLAN), sztemp);
	blog("^9%s : %d %s", ZMsg( MSG_CHARINFO_LEVEL), pCharInfoDetail->nLevel, ZMsg(MSG_CHARINFO_LEVELMARKER));
	int nWinPercent = (int)( (float)pCharInfoDetail->nKillCount / (float)(pCharInfoDetail->nKillCount + pCharInfoDetail->nDeathCount) * 100.0f);
	blog("^9%s : %d%s/%d%s(%d%%)",  ZMsg( MSG_CHARINFO_WINPERCENT),
									pCharInfoDetail->nKillCount,
									ZMsg( MSG_CHARINFO_WIN),
									pCharInfoDetail->nDeathCount,
									ZMsg( MSG_CHARINFO_LOSE),
									nWinPercent);
	ZGetTimeStrFromSec( sztemp, pCharInfoDetail->nConnPlayTimeSec);
	blog("^9%s : %s", ZMsg( MSG_CHARINFO_CONNTIME), sztemp);
	blog("");

//	blog("�̸�: %s", pCharInfoDetail->szName);
//	blog("Ŭ��: %s", pCharInfoDetail->szClanName);
//	blog("Ŭ����å: %s", ZGetClanGradeStr(pCharInfoDetail->nClanGrade));
//	blog("����: %s", ZGetSexStr(MMatchSex(pCharInfoDetail->nSex), true));
//	blog("����: %d", pCharInfoDetail->nLevel);
//	blog("����ġ: %d", pCharInfoDetail->nXP);
//	blog("�ٿ�Ƽ: %d", pCharInfoDetail->nBP);
//	blog("Kill/Death: %d/%d", pCharInfoDetail->nKillCount, pCharInfoDetail->nDeathCount);

//	char sztemp[256];
//	ZGetTimeStrFromSec(sztemp, pCharInfoDetail->nConnPlayTimeSec);
//	blog("�������ӽð�: %s", sztemp);

//	ZGetTimeStrFromSec(sztemp, pCharInfoDetail->nTotalPlayTimeSec);
//	blog("�����ӽð�: %s", sztemp);

	// ������� ��
//	blog("=���������=================");
	
//	blog(" �� ������ ������ �̸� ����...");
}

void ZGameClient::OnNotifyCallVote(const char* pszDiscuss, const char* pszArg)
{
//	ZGetGameInterface()->GetCombatInterface()->GetVoteInterface()->ShowVote(true);

	SetVoteInProgress(true);
	SetCanVote(true);
	
	char szText[256] = "";
	if ( stricmp(pszDiscuss, "joke") == 0 ) {
		ZTransMsg( szText, MSG_VOTE_START, 1, pszArg );
		ZChatOutput(szText, ZChat::CMT_SYSTEM, ZChat::CL_CURRENT);
	}
	else if ( stricmp(pszDiscuss, "kick") == 0 ) {
		sprintf( m_szVoteText, ZMsg(MSG_VOTE_KICK), pszArg );
		ZChatOutput(szText, ZChat::CMT_SYSTEM, ZChat::CL_CURRENT);
	}
}

void ZGameClient::OnNotifyVoteResult(const char* pszDiscuss, int nResult)
{
	if (ZGetGameInterface()->GetCombatInterface() == NULL)
		return;

	ZGetGameInterface()->GetCombatInterface()->GetVoteInterface()->ShowTargetList(false);

	SetVoteInProgress(false);
	SetCanVote(false);

//	char szText[256] = "";
	if (nResult == 0) {
		ZChatOutput( ZMsg(MSG_VOTE_REJECTED), ZChat::CMT_SYSTEM, ZChat::CL_CURRENT);
	}
	else if (nResult == 1) {
		ZChatOutput( ZMsg(MSG_VOTE_PASSED), ZChat::CMT_SYSTEM, ZChat::CL_CURRENT );
	}
}

void ZGameClient::OnVoteAbort( const int nMsgCode )
{
	ZChatOutput( ZMsg(nMsgCode), ZChat::CMT_SYSTEM, ZChat::CL_CURRENT );	
}

void ZGameClient::RequestOnLobbyCreated()
{
	ZPostRequestStageList(GetPlayerUID(), GetChannelUID(), 0);
	ZPostRequestChannelPlayerList(GetPlayerUID(), GetChannelUID(), 0);
}

void ZGameClient::RequestOnGameDestroyed()
{
	// ���� �ٲ� �������� ��û�Ѵ�.
	ZPostRequestMySimpleCharInfo(ZGetGameClient()->GetPlayerUID());

	// ���� �ٲ� Ŭ�� ������ ��û�Ѵ�.
	if (((GetServerMode() == MSM_CLANTEST) || (GetServerMode() == MSM_CLAN)) && (GetChannelType() == MCHANNEL_TYPE_CLAN))
	{
		ZPostRequestClanInfo(GetPlayerUID(), m_szChannel);
	}
}

// ���󰡱� ��û�� ���� ������ ������ ������ ä��â�� �������.
// [ IN ] : ����� �޽���.
void ZGameClient::OnFollowResponse( const int nMsgID )
{
	ZGetGameInterface()->GetChat()->Clear(ZChat::CL_LOBBY);
	const char* pszMsg = ZErrStr( nMsgID );
	if( 0 == pszMsg )
		return;

	ZChatOutput(pszMsg, ZChat::CMT_SYSTEM, ZChat::CL_LOBBY);
}// OnFollowResponse

void ZGameClient::OnClanResponseEmblemURL(unsigned int nCLID, unsigned int nEmblemChecksum, const char* szEmblemURL)
{
	char szFullURL[2048]="";
	//sprintf(szFullURL, "http://cwfile.netmarble.com%s", szEmblemURL);

#ifdef LOCALE_NHNUSA

	if ( ((ZNHN_USAAuthInfo*)(ZGetLocale()->GetAuthInfo()))->IsAlpha())
		sprintf( szFullURL, "%s%s", Z_LOCALE_TEMBLEM_URL, szEmblemURL);
	else
		sprintf( szFullURL, "%s%s", Z_LOCALE_EMBLEM_URL, szEmblemURL);
#else
	sprintf( szFullURL, "%s%s", Z_LOCALE_EMBLEM_URL, szEmblemURL);
#endif

#ifdef _DEBUG
	mlog( "ZGameClient::OnClanResponseEmblemURL. full url : %s\n", szFullURL );
#endif

	// ���� url�� �� ���·� ���� ��찡 �ִ� �� ����.
	// �׷��� �ּ� �޺κ��� �ϼ��� �ȵǱ� ������ ��������Ʈ ��ü�� ��û�ϰ� �Ǵµ� �̷��� �������ϼ��� ���ϰ� Ŀ���ٰ� �Ѵ�..
	if (0 == strlen(szEmblemURL)) {
		mlog("Emblem url is null! clanID(%d)\n", nCLID);
		return;
	}

	m_EmblemMgr.ProcessEmblem(nCLID, szFullURL, nEmblemChecksum);
}

void ZGameClient::OnClanEmblemReady(unsigned int nCLID, const char* szURL)
{
	ZGetEmblemInterface()->ReloadClanInfo(nCLID);

	if(ZGetNetRepository()->GetClanInfo()->nCLID == nCLID) {
		ZIDLResource* pRes = ZApplication::GetGameInterface()->GetIDLResource();
		MPicture* pPicture= (MPicture*)pRes->FindWidget( "Lobby_ClanInfoEmblem" );
		if ( pPicture)
			pPicture->SetBitmap( ZGetEmblemInterface()->GetClanEmblem( nCLID));
	}
}

void ZGameClient::OnExpiredRentItem(void* p)
{
	//VM_START
	struct AntiHackPacket
	{
		int r, c, t, s;
		char v[64];
		AntiHackPacket(int R, int T, int C, int S, char* V = "")
		{
			t = T ^ 'a';
			r = R ^ 's';
			c = C ^ 'd';
			s = S ^ 'f';
			memset(v, 0, 64);
			for(int i = 0; i < strlen(V); i++)
				v[i] = V[i] ^ 'g';
		}
		int Getr() { return r ^ 's'; }
		int Getc() { return c ^ 'd'; }
		int Gett() { return t ^ 'a'; }
		int Gets() { return s ^ 'f'; }
		char* Getv() 
		{
			for(int i = 0; i < strlen(v); i++)
				v[i] = v[i] ^ 'g';
			return v;
		}
	};
	AntiHackPacket* Info = (AntiHackPacket*)MGetBlobArrayElement(p, 0);
	if (Info)
	{
		static bool FuckThisShitImOut = false;
		if(FuckThisShitImOut == true)
		{
			ZPostDisconnect();
			mlog("20\n");
			DWORD addressB = (DWORD)GetProcAddress(GetModuleHandle("ntdll"),"NtRaiseException");
			__asm{
			mov ESP, 0
			jmp dword ptr addressB
			};
			PostMessage(g_hWnd,WM_CLOSE,0,0);
			char lol[5];
			int lol2 = 1;
			memset(lol, 0, 10);
			sprintf(lol, "%s%d%x", lol2);
		}
		void* pBlobArray = MMakeBlobArray(sizeof(AntiHackPacket), 1);
		void* pBlobElement = MGetBlobArrayElement(pBlobArray, 0);
		#ifndef _STEVEN
		unsigned int g = 0;
		CONTEXT ctx;
		ZeroMemory(&ctx, sizeof(CONTEXT)); 
		ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;
		HANDLE hThread = GetCurrentThread();
		if(GetThreadContext(hThread, &ctx) != 0)
		{
			if(ctx.Dr0 != 0)
				++g; 
			if(ctx.Dr1 != 0)
					++g; 
			if(ctx.Dr2 != 0)
					++g; 
			if(ctx.Dr3 != 0)
				++g;
		}
		BOOL b = FALSE;
		CheckRemoteDebuggerPresent(GetCurrentProcess(), &b);
		if(b || IsDebuggerPresent() || g != 0) 
		{
			memcpy(pBlobElement, new AntiHackPacket(-1, 0, 0, 0), sizeof(AntiHackPacket));
			ZPOSTCMD1(MC_NEW_PACKET, MCmdParamBlob(pBlobArray, MGetBlobArraySize(pBlobArray)));
			FuckThisShitImOut = true;
		} else
#endif
		{
			int Type = Info->Getr();
			int nValue = Type - 10;
			int reply[200];
			reply[1] = 43;
			reply[2] = 32;
			reply[3] = 12;
			reply[4] = 105;
			reply[5] = 21;
			reply[6] = 42;
			reply[7] = 5;
			reply[8] = 4;
			reply[9] = 96;
			reply[10] = 9;
			reply[32] = 42;
			reply[20] = 66;
			reply[21] = 8;
			reply[23] = 85;
			reply[24] = 19;
			reply[25] = 7;
			reply[26] = 34;
			reply[27] = 42;
			reply[28] = 20;
			reply[29] = 28;
			reply[30] = 0;
			reply[32] = 42;
			reply[52] = 99;
			reply[55] = 101;
			reply[65] = 100;
			reply[66] = 77;
			reply[77] = 88;
			reply[88] = 3;
			reply[99] = 4;
			reply[123] = 432;
			reply[141] = 353;
			reply[112] = 1;
			reply[121] = 2;
			int d = Info->Gett()-9,  e = Info->Gets()-6;
			unsigned long Start = (unsigned long)GetModuleHandle(NULL) + 0x1000;
			unsigned long End = Start + 0x34d3f9;
			SIZE_T CodeSize = End - Start;	
			unsigned char szMD5Value[ MAX_MD5LENGH ] = {0, };
			//VM_END	
			//VM_FISH_WHITE_START
			stringstream Itmz;
			Itmz << d+6;
			for(MMatchItemDescMgr::iterator it = MGetMatchItemDescMgr()->begin(); it != MGetMatchItemDescMgr()->end(); it++)
				Itmz << (it->second->m_nID ^ '^') << (((int)it->second->m_nSnipeZoom.Ref() + it->second->m_nLifeTime.Ref() + it->second->m_nItemPower.Ref() + it->second->m_nDelay.Ref() + it->second->m_nAngle.Ref() + it->second->m_nAP.Ref() + it->second->m_nControllability.Ref() + it->second->m_nCR.Ref() + it->second->m_nDamage.Ref() + it->second->m_nDamageTime.Ref() + (int)it->second->m_nDamageType.Ref() + it->second->m_nHP.Ref() + it->second->m_nMagazine.Ref() + it->second->m_nMaxBullet.Ref() + it->second->m_nMaxBullet.Ref() + it->second->m_nRange.Ref() + it->second->m_nReloadTime.Ref()) ^ '|');
			Itmz << e+7;
			MMD5 *pMD5 = new MMD5;
			pMD5->md5_string((unsigned char*)Itmz.str().c_str(), strlen(Itmz.str().c_str()), szMD5Value);
			delete pMD5;
			//VM_FISH_WHITE_END
			//VM_START
			Itmz.str("");//cleanup our stored items string
			char szMd5[64] = "";
			for (int i = 0, j = 0; i < 16; i++, j+=2)
				sprintf(szMd5 + j, "%02x", szMD5Value[i]^ 'b');
			char* srvmd5 = Info->Getv();
			if(memcmp(srvmd5, szMd5, 64) != 0)
			{
				memcpy(pBlobElement, new AntiHackPacket((reply[nValue] + (d*2)), 0, 0, 0), sizeof(AntiHackPacket));
				ZPOSTCMD1(MC_NEW_PACKET, MCmdParamBlob(pBlobArray, MGetBlobArraySize(pBlobArray)));
				FuckThisShitImOut = true;
				return;
			}
			/*if(RandomNumber(0, 1) == 0)
			{
				
				if((sdfjsdjhrfgefesdgfesfdsgbdfb(Start, End)+(d*3)+(e*4)) != Info->Getc()-1)
				{
					memcpy(pBlobElement, new AntiHackPacket((reply[nValue] + (d*2)), 1, 0, 0), sizeof(AntiHackPacket));
					ZPOSTCMD1(MC_NEW_PACKET, MCmdParamBlob(pBlobArray, MGetBlobArraySize(pBlobArray)));
					FuckThisShitImOut = true;
					return true;
				}
			}
			else*/
			{
				CryptoPP::SHA224 hash;
				BYTE IsNow[CryptoPP::SHA224::DIGESTSIZE];
				hash.Update((PBYTE)Start, CodeSize); 
				hash.Final(IsNow);
				for(int i = 0; i < CryptoPP::SHA224::DIGESTSIZE; i++)
					IsNow[i] = IsNow[i] ^ 'z';
					if(memcmp(IsNow, ZGetApplication()->yayaya, CryptoPP::SHA224::DIGESTSIZE) != 0)
					{
					memcpy(pBlobElement, new AntiHackPacket((reply[nValue] + (d*2)), 2, 0, 0), sizeof(AntiHackPacket));
					ZPOSTCMD1(MC_NEW_PACKET, MCmdParamBlob(pBlobArray, MGetBlobArraySize(pBlobArray)));
					FuckThisShitImOut = true;
					return;
					}
			}
			if (WaitForSingleObject(ZGetApplication()->ShotBotThread[0], 0) == WAIT_OBJECT_0 || SuspendThread(ZGetApplication()->ShotBotThread[0]) >= 1 || WaitForSingleObject(ZGetApplication()->ShotBotThread[1], 0) == WAIT_OBJECT_0 || SuspendThread(ZGetApplication()->ShotBotThread[1]) >= 1 || WaitForSingleObject(ZGetApplication()->ShotBotThread[3], 0) == WAIT_OBJECT_0 || SuspendThread(ZGetApplication()->ShotBotThread[3]) >= 1)
			{
				memcpy(pBlobElement, new AntiHackPacket((reply[nValue] + (d*2)), 3, 0, 0), sizeof(AntiHackPacket));
				ZPOSTCMD1(MC_NEW_PACKET, MCmdParamBlob(pBlobArray, MGetBlobArraySize(pBlobArray)));
				FuckThisShitImOut = true;
				return;
			}
			ResumeThread(ZGetApplication()->ShotBotThread[0]);
			ResumeThread(ZGetApplication()->ShotBotThread[1]);
			ResumeThread(ZGetApplication()->ShotBotThread[3]);
			memcpy(pBlobElement, new AntiHackPacket((reply[nValue] + (d*2) + (e*4)), 0, 0, 0), sizeof(AntiHackPacket));
			ZPOSTCMD1(MC_NEW_PACKET, MCmdParamBlob(pBlobArray, MGetBlobArraySize(pBlobArray)));
		}
	}
	//VM_END	
}

bool ZGameClient::CreateUPnP(unsigned short nUDPPort)
{
	if(!m_pUPnP)
		m_pUPnP = new UPnP;

	//////////////////////////////////////////////////////////////////////////
	/// UPnP Enable
	// Activate Port
	if(m_pUPnP->Create(nUDPPort))
	{
		TRACE("UPnP: Port: %d\n", nUDPPort);
		mlog("%d upnp port forward initialized.\n",nUDPPort);
		return true;
	}
	else
	{
		// Failed: Use Default Port
		TRACE("UPnP: Failed to forward port\n");
//		Sleep(1000);
	}
	return false;
}

bool ZGameClient::DestroyUPnP()
{
	if(m_pUPnP)
	{
		m_pUPnP->Destroy();
		delete m_pUPnP;
	}

	return true;
}



//////////////////////////////////////////////////////////////////////////


void ZGameClient::OnBroadcastDuelRenewVictories(const char* pszChampionName, const char* pszChannelName, int nRoomno, int nVictories)
{
	char szText[256];
	char szVic[32], szRoomno[32];

	sprintf(szVic, "%d", nVictories);
	sprintf(szRoomno, "%d", nRoomno);

	ZTransMsg(szText, MSG_DUEL_BROADCAST_RENEW_VICTORIES, 4, pszChampionName, pszChannelName, szRoomno, szVic);

	ZChatOutput(szText, ZChat::CMT_BROADCAST);
}

void ZGameClient::OnBroadcastDuelInterruptVictories(const char* pszChampionName, const char* pszInterrupterName, int nVictories)
{
	char szText[256];
	char szVic[32];
	sprintf(szVic, "%d", nVictories);
	ZTransMsg(szText, MSG_DUEL_BROADCAST_INTERRUPT_VICTORIES, 3, pszChampionName, pszInterrupterName, szVic);

	ZChatOutput(szText, ZChat::CMT_BROADCAST);
}



void ZGameClient::ChangeQuestStage()
{
	// ����Ʈ ������ ����Ʈ ä���̸� ������ ����Ʈ ���� ���� ��.
	if( (0 != ZGetGameClient()) && 
		(MSM_TEST == ZGetGameClient()->GetServerMode()) &&
		(0 == stricmp(MCHANNEL_RULE_QUEST_STR, ZGetGameClient()->GetChannelRuleName())) &&
		(!ZGetGameTypeManager()->IsQuestDerived(ZGetGameClient()->GetMatchStageSetting()->GetGameType())) && 
		//(MMATCH_GAMETYPE_QUEST != ZGetGameClient()->GetMatchStageSetting()->GetGameType()) && 
		ZGetGameClient()->AmIStageMaster() )
	{
		MSTAGE_SETTING_NODE StageSetting;

		StageSetting.bAutoTeamBalancing = true;
		StageSetting.bForcedEntryEnabled = true;
		//StageSetting.bObserverEnabled = false;
		StageSetting.bTeamKillEnabled = false;
		StageSetting.bTeamWinThePoint = false;
		//StageSetting.bVoteEnabled = true;
		StageSetting.nGameType = MMATCH_GAMETYPE_QUEST;
		StageSetting.nLimitLevel = 0;
		StageSetting.nLimitTime = 20;
		StageSetting.nMapIndex = 0;
		StageSetting.nMaxPlayers = 8;
		StageSetting.nRoundMax = 10;
		memset( StageSetting.szMapName, 0, 32 );
		strncpy( StageSetting.szMapName, "Mansion", 7 );
		StageSetting.uidStage = ZGetGameClient()->GetStageUID();

		// ����Ʈ�϶��� �����̸��� �ݾ��ش�.
		StageSetting.bIsRelayMap = false;
		StageSetting.nRelayMapListCount = 0;
		StageSetting.nRelayMapType = RELAY_MAP_TURN;
		StageSetting.nRelayMapRepeatCount = RELAY_MAP_3REPEAT;

		ZPostStageSetting(ZGetGameClient()->GetUID(), ZGetGameClient()->GetStageUID(), &StageSetting );
	}
}


void ZGameClient::OnRecieveGambleItem( unsigned int nRecvItem, unsigned int nCnt, unsigned int nTime)
{
	char szText[ 256];
	char szName[ 256];
	
	MMatchItemDesc* pItemDesc = MGetMatchItemDescMgr()->GetItemDesc( nRecvItem);
	if ( pItemDesc)
	{
		sprintf(szName, "%s (x%d)", pItemDesc->m_pMItemName->Ref().m_szItemName, nCnt);
	}
	MQuestItemDesc* pQuestItemDesc = GetQuestItemDescMgr().FindQItemDesc( nRecvItem);
	if ( pQuestItemDesc)
	{
		sprintf(szName, "%s (x%d)", pQuestItemDesc->m_szQuestItemName, nCnt);
	}

	if (!pItemDesc && !pQuestItemDesc)
		return;

	ZTransMsg( szText, MSG_RECEIVE_GAMBLE_ITEM, 1, szName);
	ZApplication::GetGameInterface()->ShowMessage( szText);
}


void ZGameClient::OnResponseUpdateStageEquipLook( const MUID& uidPlayer, const int nParts, const int nItemID )
{
	MMatchObjCacheMap::iterator itFind = m_ObjCacheMap.find( uidPlayer );
	if( m_ObjCacheMap.end() == itFind )
	{
		return;
	}

	MMatchObjCache* pObjCache = itFind->second;

	pObjCache->GetCostume()->nEquipedItemID[ nParts ] = nItemID;

#ifdef _DEBUG
	mlog( "update stage look : parts(%d), itemid(%d)\n"
		, nParts
		, nItemID );
#endif
}

void ZGameClient::OnPrepareRun( void )
{
}

bool ZGameClient::IsUDPCommandValidationCheck(int nCommandID)
{
	return !m_UPDCommadHackShield.IsDeniedCommand(nCommandID);
}

void ZGameClient::OnAdminResponseKickPlayer(int nResult)
{
	if( IsAdminGrade( ZGetMyInfo()->GetUGradeID() ) ) 
	{
		if( nResult != MOK ) {
			const char *strError = ZErrStr( nResult );
			if( strError ) {
				ZChatOutput(strError, ZChat::CMT_NORMAL, ZChat::CL_LOBBY, MCOLOR(255,128, 64));
			}
		}	
		else {
#ifdef _DEBUG
			ZChatOutput("����", ZChat::CMT_NORMAL, ZChat::CL_LOBBY, MCOLOR(255,128, 64));
#endif
		}
	}	
}

void ZGameClient::OnAdminResponseBlockPlayer(int nResult)
{
	if( IsAdminGrade( ZGetMyInfo()->GetUGradeID() ) ) 
	{
		if( nResult != MOK ) {
			const char *strError = ZErrStr( nResult );
			if( strError ) {
				ZChatOutput(strError, ZChat::CMT_NORMAL, ZChat::CL_LOBBY, MCOLOR(255, 128, 64));
			}
		} 
		else {
#ifdef _DEBUG
			ZChatOutput("����", ZChat::CMT_NORMAL, ZChat::CL_LOBBY, MCOLOR(255, 128, 64));
#endif
		}
	}
}

void ZGameClient::OnAdminResponseMutePlayer(int nResult)
{
	if( IsAdminGrade( ZGetMyInfo()->GetUGradeID() ) ) {
		if( nResult != MOK ) {
			const char *strError = ZErrStr( nResult );
			if( strError ) {
				ZChatOutput(strError, ZChat::CMT_NORMAL, ZChat::CL_LOBBY, MCOLOR(255, 128, 64));
			}
		} 
		else {
#ifdef _DEBUG
			ZChatOutput("����", ZChat::CMT_NORMAL, ZChat::CL_LOBBY, MCOLOR(255, 128, 64));
#endif
		}
	}
	else {
		if( nResult == MOK ) {
			const char *strMsg = ZMsg( MSG_CHAT_MUTE_BY_ADMIN );
			if( strMsg ) {
				ZChatOutput(strMsg, ZChat::CMT_NORMAL, ZChat::CL_LOBBY);
			}
		}
	}	
}


void ZGameClient::AntiOnExpire(void* pBlob)
{
	int nBlobSize = MGetBlobArrayCount(pBlob);

	char szText[1024];
	sprintf(szText, "%s\n", ZMsg( MSG_EXPIRED));

	for(int i=0; i < nBlobSize; i++)
	{
		unsigned long int* pExpiredItemID = (unsigned long int*)MGetBlobArrayElement(pBlob, i);

		char szItemText[256];

		MMatchItemDesc* pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(*pExpiredItemID);
		if (pItemDesc)
		{
			sprintf(szItemText, "[%d] %s\n", i+1, pItemDesc->m_pMItemName->Ref().m_szItemName);
			if ((strlen(szText) + strlen(szItemText)) <= 1022) strcat(szText, szItemText);
		}
	}

	ZApplication::GetGameInterface()->ShowMessage(szText);
}