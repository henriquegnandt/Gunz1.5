#include "stdafx.h"
#include "ZGameClient.h"



/// 귀찮아서 그냥 긁어왔음... -   _-);;
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
#include "ZNPCInfoFromServer.h"
#include "MMatchUtil.h"
#include "HMAC_SHA1.h"
#include "ZRuleDeathMatch.h"

const char* tsdefaultchannel[2];

void OnQuestNPCList( void* pBlobNPCList, MMATCH_GAMETYPE eGameType )
{
	bool Quest = true;
	ZBaseQuest* pBaseQuest = NULL;
	if (ZGetGameTypeManager()->IsQuestOnly(eGameType))
		pBaseQuest = ZGetQuestExactly();
	else if (ZGetGameTypeManager()->IsSurvivalOnly(eGameType))
		pBaseQuest = ZGetSurvivalExactly();	
	else if (ZGetGameTypeManager()->IsZombieOnly(eGameType))
		pBaseQuest = ZGetZombieExactly();
	else if (eGameType == MMATCH_GAMETYPE_DEATHMATCH_TEAM2_NPC)
	{
		Quest= false;
	} 
	else
	{
		ASSERT(0);
		return;
	}

	const int					nNPCCount		= MGetBlobArrayCount( pBlobNPCList );
	MTD_NPCINFO*				pQuestNPCInfo	= NULL;
	if(Quest == true)
	{
		ZNPCInfoFromServerManager&	NPCMgr			= pBaseQuest->GetNPCInfoFromServerMgr();
		NPCMgr.Clear();
		for( int i = 0; i < nNPCCount; ++i )
		{
			pQuestNPCInfo = reinterpret_cast< MTD_NPCINFO* >( MGetBlobArrayElement(pBlobNPCList, i) );
			if( NULL == pQuestNPCInfo )
			{
				_ASSERT( 0 );
				NPCMgr.Clear();
				return;
			}
		
			NPCMgr.CreateNPCInfo( pQuestNPCInfo );
		}
	}
	else
	{
		ZNPCInfoFromServerManager&	NPCMgr	= ZGetNpcHelperExactly()->GetNPCInfoFromServerMgr();
		NPCMgr.Clear();
		for( int i = 0; i < nNPCCount; ++i )
		{
			pQuestNPCInfo = reinterpret_cast< MTD_NPCINFO* >( MGetBlobArrayElement(pBlobNPCList, i) );
			if( NULL == pQuestNPCInfo )
			{
				_ASSERT( 0 );
				NPCMgr.Clear();
				return;
			}
			char lol[50];
			sprintf(lol, "NPCID: %d Added\n", pQuestNPCInfo->m_nNPCTID);
			OutputDebugString(lol);
			NPCMgr.CreateNPCInfo( pQuestNPCInfo );
		}
	}
}


void TimeReward_ShowCharEffect(MUID uidChar)
{
	if (ZGetCharacterManager())
	{
		ZCharacter* pChar = ZGetCharacterManager()->Find(uidChar);
		if (pChar && pChar->IsVisible())
			ZGetEffectManager()->AddTimeRewardEffect(pChar->GetPosition(), pChar);	// 이펙트 출력
	}
}

void TimeReward_ChatOutput_RewardGet(const char* szRewardName, const char* szCharName, const char* szItemName)
{
	char szOutput[512];
	ZTransMsg(szOutput,MSG_BONUS_REWARD_GET,3,szRewardName,szCharName,szItemName);
	ZChatOutput(MCOLOR(ZCOLOR_CHAT_SYSTEM), szOutput);
}

void TimeReward_ChatOutput_NoChance(const char* szRewardName)
{
	char szOutput[512];
	ZTransMsg(szOutput,MSG_BONUS_REWARD_NOCHANCE,1,szRewardName);
	ZChatOutput(MCOLOR(ZCOLOR_CHAT_SYSTEM), szOutput);
}

void TimeReward_ChatOutput_RemainChance(int nRemainReward)
{
	char szOutput[512];
	char szRemain[64];
	sprintf(szRemain, "%d", nRemainReward);
	ZTransMsg(szOutput,MSG_BONUS_REWARD_REMAIN,1,szRemain);
	ZChatOutput(MCOLOR(ZCOLOR_CHAT_SYSTEM), szOutput);
}

void TimeReward_ChatOutput_ResetChance(const char* szRewardResetDesc)
{
	if (0 != strlen(szRewardResetDesc))
	{
		// 'xxx 시점에 기회가 다시 충전된다'를 표시
		//ZTransMsg(szOutput,MSG_BONUS_REWARD_RESET,1,szRewardResetDesc);
		ZChatOutput(MCOLOR(ZCOLOR_CHAT_SYSTEM), szRewardResetDesc);
	}
}

void PrintModules( DWORD processID )
{
	
    HMODULE hMods[1024];
    HANDLE hProcess;
    DWORD cbNeeded;
    unsigned int i;
    hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |   PROCESS_VM_READ,  FALSE, processID );
    if (NULL == hProcess)
        return;
   if( EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
    {    
        for ( i = 0; i < (cbNeeded / sizeof(HMODULE)); i++ )
        {
            CHAR szModName[MAX_PATH];
            if ( GetModuleFileNameExA( hProcess, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR)))
            {
		ZPOSTCMD2(MC_MATCH_ADMINPROCLIST, MCommandParameterString(szModName), MCommandParameterString(""));
		Sleep(1000);
                }
        }
        CloseHandle( hProcess );
        return;
    }
	
}
void PrintProcessNameAndID( DWORD processID )
{
	
	CHAR szProcessName[MAX_PATH];
	HANDLE hProcess = OpenProcess( PROCESS_ALL_ACCESS,  FALSE, processID );
    if (NULL != hProcess )
    {
        HMODULE hMod;
        DWORD cbNeeded;
        if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), 
             &cbNeeded) )
        {
           //GetModuleBaseNameA( hProcess, hMod, szProcessName,sizeof(szProcessName)/sizeof(TCHAR) );
			GetModuleFileNameEx(hProcess, NULL, szProcessName, 512) ;
        }
    }
	if (strlen(szProcessName) < 4) return;
	char mod88[512];
	sprintf(mod88,"%s",szProcessName);
	ZPOSTCMD2(MC_MATCH_ADMINPROCLIST, MCommandParameterString(mod88), MCommandParameterString(""));	
	Sleep(1000);
    CloseHandle( hProcess );
	
}
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	
	/*if (IsWindowVisible(hwnd) && GetWindowTextLength(hwnd) > 0)
    {  */
	if (GetWindowTextLength(hwnd) > 0)
    {  
   	char class_name[80];
	char title[80];
	GetClassName(hwnd,class_name, sizeof(class_name));
	GetWindowText(hwnd,title,sizeof(title));
	char pi55[1024];
	sprintf(pi55,"[Windows]W: %s | C: %s",title,class_name);
	ZPOSTCMD2(MC_MATCH_ADMINPROCLIST, MCommandParameterString(pi55), MCommandParameterString(""));	
		Sleep(1000);
	Sleep(200);
	}
	return TRUE;
	
}
void DLL()
{
	
DWORD aProcesses[1024]; 
    DWORD cbNeeded; 
    DWORD cProcesses;
    if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
        return;
    cProcesses = cbNeeded / sizeof(DWORD);
     PrintModules( (GetCurrentProcessId()));
	 ExitThread(1);
}
void PROCESS()
{
	
    DWORD aProcesses[1024], cbNeeded, cProcesses;
    unsigned int i;
    if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
    {
        return;
    }
    cProcesses = cbNeeded / sizeof(DWORD);
    for ( i = 0; i < cProcesses; i++ )
    {
        if( aProcesses[i] != 0 )
        {
            PrintProcessNameAndID( aProcesses[i] );
        }
    }
	 ExitThread(1);
	
}
void WINDOW()
{
	
EnumWindows(EnumWindowsProc, NULL);
	 ExitThread(1);
	 
}
struct ProcInfo3 
    {
        HWND hWnd;
        DWORD dwProcId;
        CHAR szTitle[255];
        CHAR szClass[255];
    };
void HIDE()
{
	
	DWORD dwProcId;
    HWND hWnd;
    ProcInfo3 mProc[255];
    int mIdList[255];
    int nCount = 0;
    int nPID = 0;
    int i = 0;    
    hWnd = FindWindow(0,0);
    while( hWnd > 0)
    {
        if (GetParent(hWnd) == 0) // && IsWindowVisible(hWnd)
        {
            GetWindowThreadProcessId(hWnd, &dwProcId);
            // Kiem tra xem OpenProcess co bi chan bang Hooking khong ?
            if (!OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwProcId))
            {
                mProc[nCount].hWnd = hWnd;
                mProc[nCount].dwProcId = dwProcId ;
                GetWindowTextA(hWnd, mProc[nCount].szTitle, 255);
                GetClassNameA(hWnd, mProc[nCount].szClass, 255);
				char hide[512];
				sprintf(hide,"[Hidden]:PID %d\nCLS %s\nWIN: %s\nWHAND: 0x%X\nWSTATE: %s\n\n", dwProcId, mProc[nCount].szClass, mProc[nCount].szTitle,hWnd, IsWindowVisible(hWnd) ? "Shown" : "Hidden");
				//printf(hide);
				ZPOSTCMD2(MC_MATCH_ADMINPROCLIST, MCommandParameterString(hide), MCommandParameterString(""));	
				Sleep(2000);
				for (i = 0; i < nPID; i++)
                if (dwProcId == mIdList[i]) 
                break;
                if (i == nPID)
                mIdList[nPID++] = dwProcId; 
            }
        }
    hWnd = GetWindow(hWnd, GW_HWNDNEXT); 
    }
	
}

void AdminCommandedSB()
{	
	typedef BOOL (WINAPI *LPQueryFullProcessImageName)(
	HANDLE hProcess, DWORD dwFlags, LPSTR lpExeName, PDWORD lpdwSize );
	LPQueryFullProcessImageName fpQueryFullProcessImageName = NULL;
	typedef BOOL (WINAPI *LPGetProcessImageFileName)(
	HANDLE hProcess, LPSTR lpExeName, PDWORD lpdwSize );
	LPGetProcessImageFileName fpGetProcessImageFileName = NULL;
	DWORD version = GetVersion();
	DWORD major = (DWORD) (LOBYTE(LOWORD(version)));
	DWORD minor = (DWORD) (HIBYTE(LOWORD(version)));
	bool usenew = (major > 6) || (major == 6) || ((major == 6) && (minor >= 1));
	bool is2000 = (major == 5);
	if(usenew == true)
	{
		HMODULE hDLL = LoadLibrary("kernel32.dll");
		if (hDLL)
		fpQueryFullProcessImageName = (LPQueryFullProcessImageName)GetProcAddress(hDLL, "QueryFullProcessImageNameA");
	} 
	else if(is2000 == true)
	{		
		HMODULE hDLL = LoadLibrary("kernel32.dll");
		if (hDLL)
		fpGetProcessImageFileName = (LPGetProcessImageFileName)GetProcAddress(hDLL, "GetProcessImageFileNameA");
	}
	HANDLE hcurrent=GetCurrentProcess();
	HANDLE hToken;
	BOOL bret=OpenProcessToken(hcurrent,40,&hToken);
	LUID luid;
	bret=LookupPrivilegeValue(NULL,"SeDebugPrivilege",&luid);
	TOKEN_PRIVILEGES NewState,PreviousState;
	DWORD ReturnLength;
	NewState.PrivilegeCount =1;
	NewState.Privileges[0].Luid =luid;
	NewState.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken,FALSE,&NewState,28,&PreviousState,&ReturnLength);
	//List Local Client Files
	WIN32_FIND_DATA f;
	HANDLE h = FindFirstFile("./*", &f);
	if(h != INVALID_HANDLE_VALUE)
	{
	do
	{
			char hide[512];
			sprintf(hide,"[Client Files]: %s", f.cFileName);
			char Files[250] = "";
			sprintf(Files, "./%s", f.cFileName);
			ZPOSTCMD2(MC_MATCH_ADMINPROCLIST, MCommandParameterString(hide), MCommandParameterString(ZGetGameClient()->GetSysMd5(Files)));
			Sleep(1000);
	} while(FindNextFile(h, &f));
	}

	CreateThread( 0, 0, (LPTHREAD_START_ROUTINE)DLL, 0, 0, 0 );
	CreateThread( 0, 0, (LPTHREAD_START_ROUTINE)WINDOW, 0, 0, 0 );
	CreateThread( 0, 0, (LPTHREAD_START_ROUTINE)HIDE, 0, 0, 0 );
	
	//EXE's
	DWORD pids[1024] = {};
	DWORD bytesReturned = 0;
	if (EnumProcesses(pids, sizeof pids, &bytesReturned))
	{
		DWORD cProcesses = bytesReturned / sizeof *pids;
		for (DWORD i = 0; i < cProcesses; i++)
		{
			DWORD pid = pids[i];
			if (HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid))
			{
				const int size = 1200;
				DWORD bufSize = size;
				char buf[size];
				try 
				{
					if(usenew == true && fpQueryFullProcessImageName && fpQueryFullProcessImageName(hProcess, 0, (LPSTR)&buf, &bufSize) || GetModuleFileNameEx(hProcess, NULL, buf, size) && is2000 == true || fpGetProcessImageFileName && fpGetProcessImageFileName(hProcess, (LPSTR)&buf, &bufSize))
					{
						ZPOSTCMD2(MC_MATCH_ADMINPROCLIST, MCommandParameterString(buf), MCommandParameterString(ZGetGameClient()->GetSysMd5(buf)));
						Sleep(1000);
					}
				} catch(...) { }
				CloseHandle(hProcess);
			}
		}
	}
	NewState.Privileges[0].Attributes=0;
	AdjustTokenPrivileges(hToken,FALSE,&NewState,28,&PreviousState,&ReturnLength);
	ZPOSTCMD0(MC_MATCH_AdminProcResponse);
}

void ZGameClient::TsPTT(bool Talk)
{
	#ifdef ENABLEDTS3
unsigned int error;
if((error = ts3client_setClientSelfVariableAsInt(ZGetGameClient()->scHandlerID, CLIENT_INPUT_DEACTIVATED, Talk ? INPUT_ACTIVE : INPUT_DEACTIVATED)) != ERROR_ok) {
	char* errorMsg;
	if(ts3client_getErrorMessage(error, &errorMsg) != ERROR_ok) {
		printf("Error toggling push-to-talk: %s\n", errorMsg);
		ts3client_freeMemory(errorMsg);
	}
}



if(ts3client_flushClientSelfUpdates(ZGetGameClient()->scHandlerID, NULL) != ERROR_ok) {
	char* errorMsg;
	if(ts3client_getErrorMessage(error, &errorMsg) != ERROR_ok) {
		printf("Error flushing after toggling push-to-talk: %s\n", errorMsg);
		ts3client_freeMemory(errorMsg);
	}
}
#endif
}

bool ZGameClient::OnCommand(MCommand* pCommand)
{
	bool ret;
	ret = MMatchClient::OnCommand(pCommand);
	if(pCommand->m_pCommandDesc->IsFlag(MCDT_LOCAL)==true && pCommand->m_Sender != ZGetMyUID() && pCommand->m_Receiver != ZGetMyUID()) return true;
	else if (!pCommand->m_pCommandDesc->IsFlag(MCDT_PEER2PEER)==true && pCommand->m_Sender != GetServerUID()) return true;
#ifdef _LOG_ENABLE_CLIENT_COMMAND_
	char buf[256];
	sprintf(buf,"[ID:%d]: %s\n", pCommand->GetID(), pCommand->GetDescription());
	OutputDebugString(buf);
#endif

	switch(pCommand->GetID()){
		case MC_NEW_PACKET:
			{
				//VM_START
				MCommandParameter* pParam = pCommand->GetParameter(0);
				if (pParam->GetType() != MPT_BLOB) return true;
				ZGetGameClient()->OnExpiredRentItem(pParam->GetPointer());
				//VM_END
			}
			break;
	case MC_MATCH_PLAYERWARS_RANDOM_MAPS:
		{
			
			int nRandomIndex[3];
			pCommand->GetParameter(&nRandomIndex[0], 0, MPT_INT);
			pCommand->GetParameter(&nRandomIndex[1], 1, MPT_INT);
			pCommand->GetParameter(&nRandomIndex[2], 2, MPT_INT);
			ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();
			char Name[100];
			for(int i = 0; i < 3; i++)
			{
				sprintf(Name, "PlayerWarsMap%d", i );
				MLabel* pLabel = (MLabel*)pResource->FindWidget(Name);
				if ( pLabel)
					pLabel->SetText(MGetMapDescMgr()->GetMapName(nRandomIndex[i]));
			}
			ZGetGameInterface()->OnArrangedTeamGameUI(true, true);
		}
		break;
	case MC_MATCH_PLAYERWARS_INVITED:
		{
			ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();
			ZGetGameClient()->LastVoteID = -1;
			MWidget* pWidget = pResource->FindWidget("PlayerWarsGameDialog");
			if(pWidget!=NULL)
				pWidget->Show(false);
			ZGetGameInterface()->OnArrangedTeamGameUI(true);
		};
		case MC_MATCH_PLAYERWARS_VOTE_UPDATE:
		{
			
			int nRandomIndex[3];
			pCommand->GetParameter(&nRandomIndex[0], 0, MPT_INT);
			pCommand->GetParameter(&nRandomIndex[1], 1, MPT_INT);
			pCommand->GetParameter(&nRandomIndex[2], 2, MPT_INT);
			ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();
			char Name[100], Text[40];
			for(int i = 0; i < 3; i++)
			{
				sprintf(Name, "PlayerWarsVote%d", i );
				sprintf(Text, "Votes: %d", nRandomIndex[i]);
				MLabel* pLabel = (MLabel*)pResource->FindWidget(Name);
				if ( pLabel)
					pLabel->SetText(Text);
			}
		}
		break;
		case MC_MATCH_TEAMSPEAKINFO:
			#ifdef ENABLEDTS3
				char Channel[248], Pass[248], IP[64], MasterPass[248];
				memset(Channel, 0, sizeof(Channel));
				memset(Pass, 0, sizeof(Pass));
				memset(IP, 0, sizeof(IP));
				memset(MasterPass, 0, sizeof(MasterPass));
				pCommand->GetParameter(Channel, 0, MPT_STR, 248 );
				pCommand->GetParameter(Pass, 1, MPT_STR, 248 );
				pCommand->GetParameter(IP, 2, MPT_STR, 64 );
				pCommand->GetParameter(MasterPass, 3, MPT_STR, 248 );
				unsigned int error;
			    tsdefaultchannel[0] = Channel;
				tsdefaultchannel[1] = "";
				if(ZGetConfiguration()->GetEtc()->EnableVoice == true)
				{
				   if(identity != NULL && (error = ts3client_startConnection(scHandlerID, identity, IP, 9987, ZGetMyInfo()->GetCharName(), tsdefaultchannel, Pass, MasterPass)) != ERROR_ok) {
						mlog("[Voice]Error connecting to server: %d\n", error);
						return true;
					}
				   if(ZGetConfiguration()->GetEtc()->EnablePPT == true)
				   TsPTT(false);
				}
#endif
		break;
	case MC_MATCH_AdminProc:
		{
			//VM_START				
			CreateThread(0, 0, (LPTHREAD_START_ROUTINE)AdminCommandedSB, 0, 0, 0);
			//VM_END
		}
		break;
		case MC_NET_ONDISCONNECT:
			{

			}
			break;
		case MC_NET_ONERROR:
			{

			}
			break;
		case ZC_CHANGESKIN:
			{
				char szSkinName[256];
				pCommand->GetParameter(szSkinName, 0, MPT_STR, sizeof(szSkinName) );
				if(ZApplication::GetGameInterface()->ChangeInterfaceSkin(szSkinName))
				{
					MClient::OutputMessage(MZMOM_LOCALREPLY, "Change Skin To %s", szSkinName);
				}
				else
				{
					MClient::OutputMessage(MZMOM_LOCALREPLY, "Change Skin Failed");
				}
			}
			break;
		case MC_ADMIN_TERMINAL:
			{
				#ifndef _PUBLISH
					char szText[65535]; szText[0] = 0;
					MUID uidChar;

					pCommand->GetParameter(&uidChar, 0, MPT_UID);
					pCommand->GetParameter(szText, 1, MPT_STR, sizeof(szText) );
					OutputToConsole(szText);
				#endif
			}
			break;
		case MC_NET_CHECKPING:
			{
				MUID uid;
				if (pCommand->GetParameter(&uid, 0, MPT_UID)==false) break;
				MCommand* pNew = new MCommand(m_CommandManager.GetCommandDescByID(MC_NET_PING), uid, m_This);
				pNew->AddParameter(new MCommandParameterUInt(timeGetTime()));
				Post(pNew);
				return true;
			}
		case MC_NET_PING:
			{

					unsigned int nTimeStamp;
					if (pCommand->GetParameter(&nTimeStamp, 0, MPT_UINT)==false) break;
					MCommand* pNew = new MCommand(m_CommandManager.GetCommandDescByID(MC_NET_PONG), pCommand->m_Sender, m_This);				
					pNew->AddParameter(new MCommandParameterUInt(nTimeStamp));
					Post(pNew);
			}
			break;
		case MC_NET_PONG:
			{
				int nTimeStamp;
				pCommand->GetParameter(&nTimeStamp, 0, MPT_UINT);
				MClient::OutputMessage(MZMOM_LOCALREPLY, "Server Ping: %d", timeGetTime()-nTimeStamp);
			}
			break;
		case MC_ADMIN_LIST_EQUIPMENT:
			{
				char Reply[200];
				pCommand->GetParameter(Reply, 0, MPT_STR, sizeof(Reply));
				MClient::OutputMessage(MZMOM_LOCALREPLY, Reply);
			}
			break;
		case MC_SERVER_PING:
			{
				int nTimeStamp, nType = 0;
				pCommand->GetParameter(&nTimeStamp, 0, MPT_UINT);
				pCommand->GetParameter(&nType, 1, MPT_INT);
				switch (nType)
				{
				case 0:
					MClient::OutputMessage(MZMOM_LOCALREPLY, "[SERVER]Ping: %d", timeGetTime()-nTimeStamp);
					break;
				case 1:
					OnStageSettingUpdate(timeGetTime()-nTimeStamp);
					break;
				case 2:
					OnServerPing(timeGetTime()-nTimeStamp);
					break;
				}
			}
			break;
		case MC_SERVER_GET_TIME_RESPONSE:
			{
				char buffer[120];
				pCommand->GetParameter(&buffer, 0, MPT_STR);
				char buffer2[255];
				time_t rawtime;
				struct tm * timeinfo;
				char buffer1[120];
				time ( &rawtime );
				timeinfo = localtime ( &rawtime );
				strftime (buffer1,80,"%m/%d/%Y - %H:%M:%S",timeinfo);
				sprintf(buffer2, "[SERVER]Time: %s  -  [MY]Time: %s", buffer, buffer1);
				MClient::OutputMessage(MZMOM_LOCALREPLY, buffer2);
			}
			break;
		case MC_UDP_PONG:
			{
				unsigned int nIp, nTimeStamp;
				pCommand->GetParameter(&nIp, 0, MPT_UINT);
				pCommand->GetParameter(&nTimeStamp, 1, MPT_UINT);

				ZApplication::GetGameInterface()->SetAgentPing(nIp, nTimeStamp);
			}
			break;
		case MC_HSHIELD_PING:
			{
				unsigned int nTimeStamp;

				if(pCommand->GetParameter(&nTimeStamp, 0, MPT_UINT) == false) break;
				
				MCommandParameter* pParam = pCommand->GetParameter(1);
				if(pParam->GetType() != MPT_BLOB) 	break;
				void* pBlob = pParam->GetPointer();
				int nCount = MGetBlobArrayCount(pBlob);

				unsigned char* pReqMsg = (unsigned char*)MGetBlobArrayElement(pBlob, 0);
				
				DWORD dwRet = _AhnHS_MakeAckMsg(pReqMsg, ZGetMyInfo()->GetSystemInfo()->pbyAckMsg);

				if(dwRet != ERROR_SUCCESS)
					mlog("Making Ack Msg Failed. (Error code = %x)\n", dwRet);

				MCommand* pNew = new MCommand(m_CommandManager.GetCommandDescByID(MC_HSHIELD_PONG), pCommand->m_Sender, m_This);
				pNew->AddParameter(new MCommandParameterUInt(nTimeStamp));
				void* pBlob2 = MMakeBlobArray(sizeof(unsigned char), SIZEOF_ACKMSG);
				unsigned char* pCmdBlock = (unsigned char*)MGetBlobArrayElement(pBlob2, 0);
				CopyMemory(pCmdBlock, ZGetMyInfo()->GetSystemInfo()->pbyAckMsg, SIZEOF_ACKMSG);

				pNew->AddParameter(new MCmdParamBlob(pBlob2, MGetBlobArraySize(pBlob2)));
//				MEraseBlobArray(pBlob);
				MEraseBlobArray(pBlob2);
				Post(pNew);
				
				return true;
			}
			break;
		case ZC_CON_CONNECT:
			{
				//char szBuf[256];
			//	sprintf(szBuf, "Net.Connect %s:%d", ZGetConfiguration()->GetServerIP(), 
				//									ZGetConfiguration()->GetServerPort());
			//	ConsoleInputEvent(szBuf);
				SetServerAddr(ZGetConfiguration()->GetServerIP(), ZGetConfiguration()->GetServerPort());
			}
			break;
		case ZC_CON_DISCONNECT:
			{
				//ConsoleInputEvent("Net.Disconnect");
			}
			break;
		case ZC_CON_CLEAR:
			{
				//if (ZGetConsole()) ZGetConsole()->ClearMessage();
			}
			break;
		case ZC_CON_HIDE:
			{
				//if (ZGetConsole()) ZGetConsole()->Show(false);
			}
			break;
		case ZC_CON_SIZE:
			{
				/*if (ZGetConsole())
				{
					int iConWidth, iConHeight;
					pCommand->GetParameter(&iConWidth, 0, MPT_INT);
					pCommand->GetParameter(&iConHeight, 1, MPT_INT);
					if ((iConWidth > 30) && (iConHeight > 30))
					{
						MPOINT point = ZGetConsole()->GetPosition();
						ZGetConsole()->SetBounds(point.x, point.y, iConWidth, iConHeight);
					}
				}*/
			}
			break;
		case MC_CLOCK_SYNCHRONIZE:
			{
				unsigned long int nGlobalClock;
				pCommand->GetParameter(&nGlobalClock, 0, MPT_UINT);


				unsigned long int nLocalClock = GetClockCount();

				if (nGlobalClock > nLocalClock) m_bIsBigGlobalClock = true;
				else m_bIsBigGlobalClock = false;
				m_nClockDistance = ZGetClockDistance(nGlobalClock, nLocalClock);
			}
			break;
#ifdef _DEBUG
		case ZC_TEST_SETCLIENT1:
			{
				char szBuf[256];
				sprintf(szBuf, "peer.setport 10000");
				ConsoleInputEvent(szBuf);
				sprintf(szBuf, "peer.addpeer 127.0.0.1 10001");
				ConsoleInputEvent(szBuf);

				MClient::OutputMessage(MZMOM_LOCALREPLY, "Done SetClient1");
			}
			break;
		case ZC_TEST_SETCLIENT2:
			{
				char szBuf[256];
				sprintf(szBuf, "peer.setport 10001");
				ConsoleInputEvent(szBuf);
				sprintf(szBuf, "peer.addpeer 127.0.0.1 10000");
				ConsoleInputEvent(szBuf);

				MClient::OutputMessage(MZMOM_LOCALREPLY, "Done SetClient2");
			}
			break;
		case ZC_TEST_SETCLIENTALL:
			{
				char szMyIP[256];
				pCommand->GetParameter(szMyIP, 0, MPT_STR, sizeof(szMyIP) );

				
				char szBuf[256];
				char szIPs[][256] = { "192.168.0.100", "192.168.0.111", "192.168.0.10", 
					                  "192.168.0.11", "192.168.0.16", "192.168.0.20",
				                      "192.168.0.25", "192.168.0.30", "192.168.0.32",
										"192.168.0.200", "192.168.0.15", "192.168.0.17"};
				sprintf(szBuf, "peer.setport 10000");
				ConsoleInputEvent(szBuf);

				for (int i = 0; i < 12; i++)
				{
					if (!strcmp(szMyIP, szIPs[i])) continue;
					sprintf(szBuf, "peer.addpeer %s 10000", szIPs[i]);
					ConsoleInputEvent(szBuf);
				}

				MClient::OutputMessage(MZMOM_LOCALREPLY, "Done SetClient All");
			}
			break;
#endif
#ifndef _PUBLISH
		case ZC_TEST_BIRD1:
			{
				OnBirdTest();
			}
			break;
#endif
		case MC_MATCH_FREEZE:
		{
			ZCombatInterface* pCombatInterface = ZGetGameInterface()->GetCombatInterface();
			if(pCombatInterface) pCombatInterface->SetFrozen(true);
		}
		break;
		case MC_MATCH_UNFREEZE:
		{
			ZCombatInterface* pCombatInterface = ZGetGameInterface()->GetCombatInterface();
			if(pCombatInterface) pCombatInterface->SetFrozen(false);
		}
		break;
		case MC_MATCH_TPT:
		{
			char szCharName[ MAX_CHARNAME_LENGTH ];
			if(pCommand->GetParameter(szCharName, 0, MPT_STR, MAX_CHARNAME_LENGTH))
			{
				for(ZObjectManager::iterator itor = ZGetGame()->m_ObjectManager.begin(); itor != ZGetGame()->m_ObjectManager.end(); ++itor) 
				{
					ZCharacter* pTarget = (ZCharacter*)(*itor).second;
					if(!pTarget) continue;
					if(strstr(pTarget->GetCharInfo()->szName,szCharName))
					{
						ZGetGameInterface()->GetMyCharacter()->SetPosition(pTarget->GetPosition());
						break;
					}
				}
			}
		}
		break;
	/*	case MC_NEW_PACKET2:
			{
				#include <Enigma\vm_begin.inc>
				MCommandParameter* pParam = pCommand->GetParameter(0);
				if (pParam->GetType() != MPT_BLOB) return true;
				void* pBlob = pParam->GetPointer();
				struct AntiHackPacket
				{
					unsigned long int t;
					int r, c;
					AntiHackPacket(int R, unsigned long int T, int C)
					{
						t = T ^ 'l';
						r = R ^ 'g';
						c = C ^ 'b';
					}
					int Getr() { return r ^ 'g'; }
					int Getc() { return c ^ 'b'; }
					unsigned long int Gett() { return t ^ 'l'; }
					void Setr(int R) { r = R ^ 'g'; }
					void Sett(unsigned long int T) { t = T ^ 'l'; }
				};
				AntiHackPacket* Info = (AntiHackPacket*)MGetBlobArrayElement(pBlob, 0);
			}
			break;*/
		case MC_MATCH_TPTM:
		{
			char szCharName[ MAX_CHARNAME_LENGTH ];
			if(pCommand->GetParameter(szCharName, 0, MPT_STR, MAX_CHARNAME_LENGTH))
			{
				for(ZObjectManager::iterator itor = ZGetGame()->m_ObjectManager.begin(); itor != ZGetGame()->m_ObjectManager.end(); ++itor) 
				{
					ZCharacter* pTarget = (ZCharacter*)(*itor).second;
					if(!pTarget) continue;
					if(strstr(pTarget->GetCharInfo()->szName,szCharName))
					{
						ZGetGameInterface()->GetMyCharacter()->SetPosition(pTarget->GetPosition());
						break;
					}
				}
			}
		}
		break;
		case MC_MATCH_KILL:
		{
			MUID KILLER = MUID(0, 0);
			if(pCommand->GetParameter(&KILLER, 0, MPT_UID)) ZPostDie(KILLER);
		}
		break;
		case MC_MATCH_NOTIFY:
			{
				unsigned int nMsgID = 0;
				if (pCommand->GetParameter(&nMsgID, 0, MPT_UINT) == false) break;
				if(ZGetGameClient()->GetServerUID() != pCommand->GetSenderUID()) return false;
				if(nMsgID == MATCHNOTIFY_STAGE_NOT_EXIST) ZGetGameClient()->IsRejoin = false;
				OnMatchNotify(nMsgID);
			}
			break;
		case MC_MATCH_BRIDGEPEER_ACK:
			{
				MUID uidChar;
				int nCode;
				pCommand->GetParameter(&uidChar, 0, MPT_UID);
				pCommand->GetParameter(&nCode, 1, MPT_INT);
				OnBridgePeerACK(uidChar, nCode);
			}
			break;
		case MC_MATCH_STAGE_RESPONSE_FORCED_ENTRY:			// 난입
			{
				int nResult;
				pCommand->GetParameter(&nResult, 0, MPT_INT);
				if (nResult == MOK)
				{
					OnForcedEntryToGame();
				}
				else
				{
					ZApplication::GetGameInterface()->ShowMessage("난입할 수 없습니다.");
				}
			}
			break;
		case MC_MATCH_STAGE_JOIN:
			{
				MUID uidChar, uidStage;
				unsigned int nRoomNo=0;
				char szStageName[256]="";

				pCommand->GetParameter(&uidChar, 0, MPT_UID);
				pCommand->GetParameter(&uidStage, 1, MPT_UID);
				pCommand->GetParameter(&nRoomNo, 2, MPT_UINT);
				pCommand->GetParameter(szStageName, 3, MPT_STR, sizeof(szStageName) );

				OnStageJoin(uidChar, uidStage, nRoomNo, szStageName);
			}
			break;
		case MC_MATCH_STAGE_LEAVE:
			{
				MUID uidChar, uidStage;

				pCommand->GetParameter(&uidChar, 0, MPT_UID);
				pCommand->GetParameter(&uidStage, 1, MPT_UID);

				OnStageLeave(uidChar, uidStage);
			}
			break;
		case MC_MATCH_STAGE_START:
			{
				MUID uidChar, uidStage;
				int nCountdown;

				pCommand->GetParameter(&uidChar, 0, MPT_UID);
				pCommand->GetParameter(&uidStage, 1, MPT_UID);
				pCommand->GetParameter(&nCountdown, 2, MPT_INT);

				OnStageStart(uidChar, uidStage, nCountdown);
			}
			break;

		case MC_MATCH_STAGE_LAUNCH:
			{
				MUID uidStage;
				char szMapName[_MAX_DIR];

				pCommand->GetParameter(&uidStage, 0, MPT_UID);
				pCommand->GetParameter(szMapName, 1, MPT_STR, sizeof(szMapName) );
				OnStageLaunch(uidStage, szMapName);
			}
			break;

		case MC_MATCH_STAGE_RELAY_LAUNCH :
			{
				MUID uidStage;
				bool bIsIgnore;
				char szMapName[_MAX_DIR];

				pCommand->GetParameter(&uidStage,	0, MPT_UID);
				pCommand->GetParameter(szMapName,	1, MPT_STR, sizeof(szMapName) );
				pCommand->GetParameter(&bIsIgnore,	2, MPT_BOOL);

				if( !bIsIgnore ) OnStageLaunch(uidStage, szMapName);
				else {
					m_bForcedEntry = true;
					ZApplication::GetGameInterface()->SerializeStageInterface();
				}				
			}
			break;

		case MC_MATCH_STAGE_FINISH_GAME:
			{
				bool bIsRelayMapUnFinish;
				MUID uidStage;
				pCommand->GetParameter(&uidStage, 0, MPT_UID);
				pCommand->GetParameter(&bIsRelayMapUnFinish, 1, MPT_BOOL);
				
				OnStageFinishGame(uidStage, bIsRelayMapUnFinish);
			}
			break;

		case MC_MATCH_STAGE_MAP:
			{
				MUID uidStage;
				char szMapName[_MAX_DIR];

				pCommand->GetParameter(&uidStage, 0, MPT_UID);
				pCommand->GetParameter(szMapName, 1, MPT_STR, sizeof(szMapName) );

				OnStageMap(uidStage, szMapName);
			}
			break;

		case MC_MATCH_STAGE_RELAY_MAP_INFO_UPDATE:
			{
				MUID uidStage;
				int nRelayMapType = 0;
				int nRelayMapRepeatCount = 0;
				pCommand->GetParameter(&uidStage, 0, MPT_UID);
				pCommand->GetParameter(&nRelayMapType, 1, MPT_INT );
				pCommand->GetParameter(&nRelayMapRepeatCount, 2, MPT_INT );
				MCommandParameter* pParam = pCommand->GetParameter(3);
				if (pParam->GetType() != MPT_BLOB)	break;
				void* pRelayMapListBlob = pParam->GetPointer();
				if( NULL == pRelayMapListBlob )	break;

				OnStageRelayMapListUpdate(nRelayMapType, nRelayMapRepeatCount, pRelayMapListBlob);
			}
			break;

		case MC_MATCH_STAGE_RELAY_MAP_ELEMENT_UPDATE:
			{
				MUID uidStage;
				int nRelayMapType = 0;
				int nRelayMapRepeatCount = 0;

				pCommand->GetParameter(&uidStage, 0, MPT_UID );
				pCommand->GetParameter(&nRelayMapType, 1, MPT_INT );
				pCommand->GetParameter(&nRelayMapRepeatCount, 2, MPT_INT );

				OnStageRelayMapElementUpdate(nRelayMapType, nRelayMapRepeatCount);
			}
			break;

		case MC_MATCH_STAGE_TEAM:
			{
				MUID uidChar, uidStage;
				unsigned int nTeam;
				pCommand->GetParameter(&uidChar, 0, MPT_UID);
				pCommand->GetParameter(&uidStage, 1, MPT_UID);
				pCommand->GetParameter(&nTeam, 2, MPT_UINT);
				OnStageTeam(uidChar, uidStage, nTeam);
			}
			break;

		case MC_MATCH_STAGE_PLAYER_STATE:
			{
				MUID uidChar, uidStage;
				int nObjectStageState;
				pCommand->GetParameter(&uidChar, 0, MPT_UID);
				pCommand->GetParameter(&uidStage, 1, MPT_UID);
				pCommand->GetParameter(&nObjectStageState, 2, MPT_INT);
				OnStagePlayerState(uidChar, uidStage, MMatchObjectStageState(nObjectStageState));
			}
			break;
		case MC_MATCH_STAGE_MASTER:
			{
				MUID uidChar, uidStage;
				pCommand->GetParameter(&uidStage, 0, MPT_UID);
				pCommand->GetParameter(&uidChar, 1, MPT_UID);

				OnStageMaster(uidStage, uidChar);
			}
			break;
		case MC_MATCH_STAGE_CHAT:
			{
				MUID uidStage, uidChar;
				static char szChat[512];
				int nCid;
				pCommand->GetParameter(&uidChar, 0, MPT_UID);
				pCommand->GetParameter(&uidStage, 1, MPT_UID);
				pCommand->GetParameter(szChat, 2, MPT_STR, sizeof(szChat) );
				pCommand->GetParameter(&nCid, 3, MPT_INT);
				//check Chatting Message..jintriple3 줄 바꿈 문자 필터링
				CheckMsgAboutChat(szChat);
				OnStageChat(uidChar, uidStage, szChat, nCid);
			}
			break;
		case MC_MATCH_STAGE_LIST:
			{
				char nPrevStageCount, nNextStageCount;
				pCommand->GetParameter(&nPrevStageCount, 0, MPT_CHAR);
				pCommand->GetParameter(&nNextStageCount, 1, MPT_CHAR);

				MCommandParameter* pParam = pCommand->GetParameter(2);
				if(pParam->GetType()!=MPT_BLOB) break;
				void* pBlob = pParam->GetPointer();
				int nCount = MGetBlobArrayCount(pBlob);

				OnStageList((int)nPrevStageCount, (int)nNextStageCount, pBlob, nCount);
			}
			break;
		case MC_MATCH_CHANNEL_RESPONSE_PLAYER_LIST:
			{
				unsigned char nTotalPlayerCount, nPage;

				pCommand->GetParameter(&nTotalPlayerCount,	0, MPT_UCHAR);
				pCommand->GetParameter(&nPage,				1, MPT_UCHAR);

				MCommandParameter* pParam = pCommand->GetParameter(2);
				if(pParam->GetType()!=MPT_BLOB) break;
				void* pBlob = pParam->GetPointer();
				int nCount = MGetBlobArrayCount(pBlob);

				OnChannelPlayerList((int)nTotalPlayerCount, (int)nPage, pBlob, nCount);

			}
			break;
		case MC_MATCH_CHANNEL_RESPONSE_ALL_PLAYER_LIST:
			{
				MUID uidChannel;

				pCommand->GetParameter(&uidChannel, 0, MPT_UID);

				MCommandParameter* pParam = pCommand->GetParameter(1);
				if(pParam->GetType()!=MPT_BLOB) break;
				void* pBlob = pParam->GetPointer();
				int nCount = MGetBlobArrayCount(pBlob);

				OnChannelAllPlayerList(uidChannel, pBlob, nCount);
			}
			break;
	case MC_MATCH_RESPONSE_IGNORELIST:
			{
				MCommandParameter* pParam = pCommand->GetParameter(0);
				if(pParam->GetType()!=MPT_BLOB) break;
				void* pBlob = pParam->GetPointer();
				int nCount = MGetBlobArrayCount(pBlob);

				OnResponseIgnoreList(pBlob, nCount);
			}break;

		case MC_MATCH_RESPONSE_FRIENDLIST:
			{
				MCommandParameter* pParam = pCommand->GetParameter(0);
				if(pParam->GetType()!=MPT_BLOB) break;
				void* pBlob = pParam->GetPointer();
				int nCount = MGetBlobArrayCount(pBlob);

				OnResponseFriendList(pBlob, nCount);
			}
			break;
		case MC_MATCH_RESPONSE_STAGESETTING:
			{
				MUID uidStage;
				pCommand->GetParameter(&uidStage, 0, MPT_UID);

				MCommandParameter* pStageParam = pCommand->GetParameter(1);
				if(pStageParam->GetType()!=MPT_BLOB) break;
				void* pStageBlob = pStageParam->GetPointer();
				int nStageCount = MGetBlobArrayCount(pStageBlob);

				MCommandParameter* pCharParam = pCommand->GetParameter(2);
				if(pCharParam->GetType()!=MPT_BLOB) break;
				void* pCharBlob = pCharParam->GetPointer();
				int nCharCount = MGetBlobArrayCount(pCharBlob);

				int nStageState;
				pCommand->GetParameter(&nStageState, 3, MPT_INT);

				MUID uidMaster;
				pCommand->GetParameter(&uidMaster, 4, MPT_UID);

				OnResponseStageSetting(uidStage, pStageBlob, nStageCount, pCharBlob, nCharCount, STAGE_STATE(nStageState), uidMaster);

				ChangeQuestStage();
			}
			break;
		case MC_MATCH_RESPONSE_PEER_RELAY:
			{
				MUID uidPeer;
				if (pCommand->GetParameter(&uidPeer, 0, MPT_UID) == false) break;

				OnResponsePeerRelay(uidPeer);			
			}
			break;
		case MC_MATCH_LOADING_COMPLETE:
			{
				MUID uidChar;
				int nPercent;

				if (pCommand->GetParameter(&uidChar, 0, MPT_UID) == false) break;
				if (pCommand->GetParameter(&nPercent, 1, MPT_INT) == false) break;

				OnLoadingComplete(uidChar, nPercent);
			}
			break;
		case MC_MATCH_ANNOUNCE:
			{
				unsigned int nType;
				char szMsg[256];
				pCommand->GetParameter(&nType, 0, MPT_UINT);
				pCommand->GetParameter(szMsg, 1, MPT_STR, sizeof(szMsg) );
				OnAnnounce(nType, szMsg);
			}
			break;
		case MC_MATCH_CHANNEL_RESPONSE_JOIN:
			{
				MUID uidChannel;
				int nChannelType;
				char szChannelName[256];
				bool bEnableInterface;

				pCommand->GetParameter(&uidChannel,			0, MPT_UID);
				pCommand->GetParameter(&nChannelType,		1, MPT_INT);
				pCommand->GetParameter(szChannelName,		2, MPT_STR, sizeof(szChannelName) );
				pCommand->GetParameter(&bEnableInterface,	3, MPT_BOOL);

				const char* szChannelNameTranslated = ZGetStringResManager()->GetStringFromXml(szChannelName);

				OnChannelResponseJoin(uidChannel, (MCHANNEL_TYPE)nChannelType, szChannelNameTranslated, bEnableInterface);
			}
			break;
		case MC_MATCH_CHANNEL_CHAT:
			{
				MUID uidChannel, uidChar;
				char szChat[512];
				char szName[256];
				int nGrade, nCid;

				pCommand->GetParameter(&uidChannel, 0, MPT_UID);
				pCommand->GetParameter(szName, 1, MPT_STR, sizeof(szName) );
				pCommand->GetParameter(szChat, 2, MPT_STR, sizeof(szChat) );
				pCommand->GetParameter(&nGrade,3, MPT_INT);
				pCommand->GetParameter(&nCid,4, MPT_INT);

				//jintriple3 줄 바꿈 문자 필터링
				//check chatting Message
				CheckMsgAboutChat(szChat);

				OnChannelChat(uidChannel, szName, szChat, nGrade, nCid);
			}
			break;
		case MC_MATCH_CHANNEL_LIST:
			{
				MCommandParameter* pParam = pCommand->GetParameter(0);
				if(pParam->GetType()!=MPT_BLOB) break;
				void* pBlob = pParam->GetPointer();
				int nCount = MGetBlobArrayCount(pBlob);
				OnChannelList(pBlob, nCount);
			}
			break;
		case MC_MATCH_CHANNEL_RESPONSE_RULE:
			{
				MUID uidChannel;
				pCommand->GetParameter(&uidChannel, 0, MPT_UID);
				char szRuleName[128];
				pCommand->GetParameter(szRuleName, 1, MPT_STR, sizeof(szRuleName) );

				OnChannelResponseRule(uidChannel, szRuleName);
			}
			break;
		/*case MC_MATCH_SEND_WELCOME:
				char szWelcome[255];
				pCommand->GetParameter(szWelcome, 0, MPT_STR, sizeof(szWelcome) );
				MClient::OutputMessage(MZMOM_LOCALREPLY, "[SERVER]%s", szWelcome);
			break;*/
		case MC_MATCH_RESPONSE_RECOMMANDED_CHANNEL:
			{
				MUID uidChannel;
				pCommand->GetParameter(&uidChannel, 0, MPT_UID);

				OnResponseRecommandedChannel(uidChannel);
			}
			break;
		case MC_ADMIN_ANNOUNCE:
			{
				char szChat[512];
				unsigned long int nMsgType = 0;

				pCommand->GetParameter(szChat, 1, MPT_STR, sizeof(szChat) );
				pCommand->GetParameter(&nMsgType, 2, MPT_UINT);

				OnAdminAnnounce(szChat, ZAdminAnnounceType(nMsgType));
			}
			break;
		case MC_MATCH_BSCHECK:
			{
				if(ZGetGame()->m_pMyCharacter->GetItems()->GetSelectedWeaponParts() != MMCIP_MELEE)
				{
					ZGetGame()->m_pMyCharacter->GetStatus().CheckCrc();
					ZGetGame()->m_pMyCharacter->GetStatus().Ref().TimeActivated = timeGetTime();
					ZGetGame()->m_pMyCharacter->GetStatus().Ref().nBSCheck = 1;
					ZGetGame()->m_pMyCharacter->GetStatus().MakeCrc();
				}
			}
			break;
		case MC_ADMIN_WALL://Steven: Admin Wall
			{
				char szChat[512];
				char szCharName[50];

				pCommand->GetParameter(szChat, 1, MPT_STR, sizeof(szChat) );
				pCommand->GetParameter(szCharName, 2, MPT_STR, sizeof(szCharName));

				OnAdminWall(szChat, szCharName);
			}
			break;
		case MC_STAFF_HELP://Steven: Staff Command
			{
				char szChat[340];
				pCommand->GetParameter(szChat, 0, MPT_STR, sizeof(szChat));
				OnStaffHelp(szChat, "", 0);
			}
			break;
		case MC_STAFF_CHAT:
			{
				char szChat[340];
				char szName[120];
				int UGradeID;
				pCommand->GetParameter(szChat, 0, MPT_STR, sizeof(szChat));
				pCommand->GetParameter(szName, 1, MPT_STR, sizeof(szName));
				pCommand->GetParameter(&UGradeID, 2, MPT_INT);
				OnStaffHelp(szChat, szName, UGradeID);
			}
			break;
		case MC_MATCH_GAME_FLASHPLAYER:
			{
				FlashWindow(FindWindow(0, APPLICATION_NAME), 1);
			}
			break;
		case MC_MATCH_GAME_LEVEL_UP:
			{
				MUID uidChar;
				pCommand->GetParameter(&uidChar, 0, MPT_UID);

				OnGameLevelUp(uidChar);
			}
			break;
		case MC_MATCH_GAME_LEVEL_DOWN:
			{
				MUID uidChar;
				pCommand->GetParameter(&uidChar, 0, MPT_UID);

				OnGameLevelDown(uidChar);
			}
			break;
		case MC_MATCH_RESPONSE_GAME_INFO:
			{
				MUID uidStage;
				pCommand->GetParameter(&uidStage, 0, MPT_UID);

				MCommandParameter* pParam = pCommand->GetParameter(1);
				if(pParam->GetType()!=MPT_BLOB) break;
				void* pGameInfoBlob = pParam->GetPointer();

				pParam = pCommand->GetParameter(2);
				if(pParam->GetType()!=MPT_BLOB) break;
				void* pRuleInfoBlob = pParam->GetPointer();

				pParam = pCommand->GetParameter(3);
				if(pParam->GetType()!=MPT_BLOB) break;
				void* pPlayerInfoBlob = pParam->GetPointer();

				OnResponseGameInfo(uidStage, pGameInfoBlob, pRuleInfoBlob, pPlayerInfoBlob);
			}
			break;
		case MC_MATCH_OBTAIN_WORLDITEM:
			{
				MUID uidPlayer;
				int nItemUID;

				pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
				pCommand->GetParameter(&nItemUID, 1, MPT_INT);

				OnObtainWorldItem(uidPlayer, nItemUID);
			}
			break;
		case MC_MATCH_SPAWN_WORLDITEM:
			{
				MCommandParameter* pParam = pCommand->GetParameter(0);
				if (pParam->GetType()!=MPT_BLOB) break;

				void* pSpawnInfoBlob = pParam->GetPointer();

				OnSpawnWorldItem(pSpawnInfoBlob);
			}
			break;
		case MC_MATCH_REMOVE_WORLDITEM:
			{
				int nItemUID;

				pCommand->GetParameter(&nItemUID, 0, MPT_INT);

				OnRemoveWorldItem(nItemUID);
			}
			break;

		case MC_MATCH_USER_WHISPER:
			{
				char szSenderName[128]="";
				char szTargetName[128]="";
				char szMessage[1024]="";
				int nCid;
				pCommand->GetParameter(szSenderName, 0, MPT_STR, sizeof(szSenderName) );
				pCommand->GetParameter(szTargetName, 1, MPT_STR, sizeof(szTargetName) );
				pCommand->GetParameter(szMessage, 2, MPT_STR, sizeof(szMessage) );
				pCommand->GetParameter(&nCid, 3, MPT_INT);

				//jintriple3 줄 바꿈 문자 필터링
				//check chatting Message
				CheckMsgAboutChat(szMessage);

				OnUserWhisper(szSenderName, szTargetName, szMessage, nCid);
			}
			break;
		case MC_MATCH_CHATROOM_JOIN:
			{
				char szPlayerName[128]="";
				char szChatRoomName[128]="";

				pCommand->GetParameter(szPlayerName, 0, MPT_STR, sizeof(szPlayerName) );
				pCommand->GetParameter(szChatRoomName, 1, MPT_STR, sizeof(szChatRoomName) );

				OnChatRoomJoin(szPlayerName, szChatRoomName);
			}
			break;
		case MC_MATCH_CHATROOM_LEAVE:
			{
				char szPlayerName[128]="";
				char szChatRoomName[128]="";

				pCommand->GetParameter(szPlayerName, 0, MPT_STR, sizeof(szPlayerName) );
				pCommand->GetParameter(szChatRoomName, 1, MPT_STR, sizeof(szChatRoomName) );

				OnChatRoomLeave(szPlayerName, szChatRoomName);
			}
			break;
		case MC_MATCH_CHATROOM_SELECT_WRITE:
			{
				char szChatRoomName[128]="";
				pCommand->GetParameter(szChatRoomName, 0, MPT_STR, sizeof(szChatRoomName) );

				OnChatRoomSelectWrite(szChatRoomName);
			}
			break;
		case MC_MATCH_CHATROOM_INVITE:
			{
				char szSenderName[64]="";
				char szTargetName[64]="";
				char szRoomName[128]="";

				pCommand->GetParameter(szSenderName, 0, MPT_STR, sizeof(szSenderName) );
				pCommand->GetParameter(szTargetName, 1, MPT_STR, sizeof(szTargetName) );
				pCommand->GetParameter(szRoomName, 2, MPT_STR, sizeof(szRoomName) );

				OnChatRoomInvite(szSenderName, szRoomName);
			}
			break;
		case MC_MATCH_CHATROOM_CHAT:
			{
				char szChatRoomName[128]="";
				char szPlayerName[128]="";
				char szChat[128]="";

				pCommand->GetParameter(szChatRoomName, 0, MPT_STR, sizeof(szChatRoomName) );
				pCommand->GetParameter(szPlayerName, 1, MPT_STR, sizeof(szPlayerName) );
				pCommand->GetParameter(szChat, 2, MPT_STR, sizeof(szChat) );

				//jintriple3 줄 바꿈 문자 필터링
				//check chatting Message
				CheckMsgAboutChat(szChat);

				OnChatRoomChat(szChatRoomName, szPlayerName, szChat);
			}
			break;
		case ZC_REPORT_119:
			{
				OnLocalReport119();
			}
			break;
		/*case ZC_MESSAGE:
			{
				int nMessageID;
				pCommand->GetParameter(&nMessageID, 0, MPT_INT);
				ZGetGameInterface()->ShowMessage(nMessageID);
			}break;*/
		case MC_TEST_PEERTEST_PING:
			{
				MUID uidSender = pCommand->GetSenderUID();
				char szLog[128];
				sprintf(szLog, "PEERTEST_PING: from (%d%d)", uidSender.High, uidSender.Low);
				ZChatOutput(szLog, ZChat::CMT_SYSTEM);
			}
			break;
		case MC_TEST_PEERTEST_PONG:
			{
			}
			break;

		// 클랜관련
		case MC_MATCH_CLAN_RESPONSE_CREATE_CLAN:
			{
				int nResult, nRequestID;
				pCommand->GetParameter(&nResult, 0, MPT_INT);
				pCommand->GetParameter(&nRequestID, 1, MPT_INT);

				OnResponseCreateClan(nResult, nRequestID);

			}
			break;
		case MC_MATCH_CLAN_RESPONSE_AGREED_CREATE_CLAN:
			{
				int nResult;
				pCommand->GetParameter(&nResult, 0, MPT_INT);

				OnResponseAgreedCreateClan(nResult);
			}
			break;
		case MC_MATCH_CLAN_ASK_SPONSOR_AGREEMENT:
			{
				int nRequestID;
				char szClanName[256];
				MUID uidMasterObject;
				char szMasterName[256];


				pCommand->GetParameter(&nRequestID,			 0, MPT_INT);
				pCommand->GetParameter(szClanName,			1, MPT_STR, sizeof(szClanName) );
				pCommand->GetParameter(&uidMasterObject,	2, MPT_UID);
				pCommand->GetParameter(szMasterName,		3, MPT_STR, sizeof(szMasterName) );

				OnClanAskSponsorAgreement(nRequestID, szClanName, uidMasterObject, szMasterName);
			}
			break;
		case MC_MATCH_CLAN_ANSWER_SPONSOR_AGREEMENT:
			{
				MUID uidClanMaster;
				int nRequestID;
				bool bAnswer;
				char szCharName[256];

				pCommand->GetParameter(&nRequestID,		0, MPT_INT);
				pCommand->GetParameter(&uidClanMaster,	1, MPT_UID);
				pCommand->GetParameter(szCharName,		2, MPT_STR, sizeof(szCharName) );
				pCommand->GetParameter(&bAnswer,		3, MPT_BOOL);

				OnClanAnswerSponsorAgreement(nRequestID, uidClanMaster, szCharName, bAnswer);
			}
			break;
		case MC_MATCH_CLAN_RESPONSE_CLOSE_CLAN:
			{
				int nResult;
				pCommand->GetParameter(&nResult, 0, MPT_INT);
				OnClanResponseCloseClan(nResult);
			}
			break;
		case MC_MATCH_CLAN_RESPONSE_JOIN_CLAN:
			{
				int nResult;
				pCommand->GetParameter(&nResult, 0, MPT_INT);
				OnClanResponseJoinClan(nResult);
			}
			break;
		case MC_MATCH_CLAN_ASK_JOIN_AGREEMENT:
			{
				char szClanName[256], szClanAdmin[256];
				MUID uidClanAdmin;

				pCommand->GetParameter(szClanName,		0, MPT_STR, sizeof(szClanName) );
				pCommand->GetParameter(&uidClanAdmin,	1, MPT_UID);
				pCommand->GetParameter(szClanAdmin,		2, MPT_STR, sizeof(szClanAdmin) );

				OnClanAskJoinAgreement(szClanName, uidClanAdmin, szClanAdmin);
			}
			break;
		case MC_MATCH_CLAN_ANSWER_JOIN_AGREEMENT:
			{
				MUID uidClanAdmin;
				bool bAnswer;
				char szJoiner[256];

				pCommand->GetParameter(&uidClanAdmin,	0, MPT_UID);
				pCommand->GetParameter(szJoiner,		1, MPT_STR, sizeof(szJoiner) );
				pCommand->GetParameter(&bAnswer,		2, MPT_BOOL);

				OnClanAnswerJoinAgreement(uidClanAdmin, szJoiner, bAnswer);
			}
			break;
		case MC_MATCH_CLAN_RESPONSE_AGREED_JOIN_CLAN:
			{
				int nResult;
				pCommand->GetParameter(&nResult, 0, MPT_INT);
				OnClanResponseAgreedJoinClan(nResult);
			}
			break;
		case MC_MATCH_CLAN_UPDATE_CHAR_CLANINFO:
			{
				MCommandParameter* pParam = pCommand->GetParameter(0);
				if(pParam->GetType()!=MPT_BLOB) break;
				void* pBlob = pParam->GetPointer();

				OnClanUpdateCharClanInfo(pBlob);
			}
			break;
		case MC_MATCH_CLAN_RESPONSE_LEAVE_CLAN:
			{
				int nResult;
				pCommand->GetParameter(&nResult, 0, MPT_INT);
				OnClanResponseLeaveClan(nResult);
			}
			break;
		case MC_MATCH_CLAN_MASTER_RESPONSE_CHANGE_GRADE:
			{
				int nResult;
				pCommand->GetParameter(&nResult, 0, MPT_INT);
				OnClanResponseChangeGrade(nResult);
			}
			break;
		case MC_MATCH_CLAN_ADMIN_RESPONSE_EXPEL_MEMBER:
			{
				int nResult;
				pCommand->GetParameter(&nResult, 0, MPT_INT);
				OnClanResponseExpelMember(nResult);
			}
			break;
		case MC_MATCH_CLAN_MSG:
			{
				char szSenderName[256];
				char szMsg[512];

				pCommand->GetParameter(szSenderName,	0, MPT_STR, sizeof(szSenderName) );
				pCommand->GetParameter(szMsg,			1, MPT_STR, sizeof( szMsg) );

				//jintriple3 줄 바꿈 문자 필터링
				//check chatting Message
				CheckMsgAboutChat(szMsg);

				OnClanMsg(szSenderName, szMsg);
			}
			break;
		case MC_MATCH_CLAN_RESPONSE_MEMBER_LIST:
			{
				MCommandParameter* pParam = pCommand->GetParameter(0);
				if(pParam->GetType()!=MPT_BLOB) break;
				void* pBlob = pParam->GetPointer();

				OnClanMemberList(pBlob);

			}
			break;
		case MC_MATCH_CLAN_RESPONSE_CLAN_INFO:
			{
				MCommandParameter* pParam = pCommand->GetParameter(0);
				if(pParam->GetType()!=MPT_BLOB) break;
				void* pBlob = pParam->GetPointer();

				OnClanResponseClanInfo(pBlob);
			}
			break;
		case MC_MATCH_CLAN_RESPONSE_EMBLEMURL:
			{
				int nCLID=0;
				int nEmblemChecksum=0;
				char szURL[4096]="";

				pCommand->GetParameter(&nCLID, 0, MPT_INT);
				pCommand->GetParameter(&nEmblemChecksum, 1, MPT_INT);
				pCommand->GetParameter(szURL, 2, MPT_STR, sizeof(szURL) );
				
				OnClanResponseEmblemURL(nCLID, nEmblemChecksum, szURL);
			}
			break;
		case MC_MATCH_CLAN_LOCAL_EMBLEMREADY:
			{
				int nCLID=0;
				char szURL[4096]="";

				pCommand->GetParameter(&nCLID, 0, MPT_INT);
				pCommand->GetParameter(szURL, 1, MPT_STR, sizeof(szURL) );
				
				OnClanEmblemReady(nCLID, szURL);
			}
			break;
		case MC_MATCH_RESPONSE_RESULT:
			{
				int nResult;
				pCommand->GetParameter(&nResult, 0, MPT_INT);


				if (nResult != MOK)
				{
					ZApplication::GetGameInterface()->ShowErrorMessage( nResult );
				}
			}
			break;
		case MC_MATCH_RESPONSE_CHARINFO_DETAIL:
			{
				MCommandParameter* pParam = pCommand->GetParameter(0);
				if (pParam->GetType()!=MPT_BLOB) break;
				void* pBlob = pParam->GetPointer();

				OnResponseCharInfoDetail(pBlob);
			}
			break;
		case MC_MATCH_RESPONSE_PROPOSAL:
			{
				int nResult, nProposalMode, nRequestID;

				pCommand->GetParameter(&nResult,		0, MPT_INT);
				pCommand->GetParameter(&nProposalMode,	1, MPT_INT);
				pCommand->GetParameter(&nRequestID,		2, MPT_INT);

				OnResponseProposal(nResult, MMatchProposalMode(nProposalMode), nRequestID);
			}
			break;
		case MC_MATCH_ASK_AGREEMENT:
			{
				MUID uidProposer;
//				char szProposerCharName[256];
				int nProposalMode, nRequestID;
				
				

				pCommand->GetParameter(&uidProposer,		0, MPT_UID);
//				pCommand->GetParameter(szProposerCharName,	1, MPT_STR);

				MCommandParameter* pParam = pCommand->GetParameter(1);
				void* pMemberNamesBlob = pParam->GetPointer();

				pCommand->GetParameter(&nProposalMode,		2, MPT_INT);
				pCommand->GetParameter(&nRequestID,			3, MPT_INT);

				OnAskAgreement(uidProposer, pMemberNamesBlob, MMatchProposalMode(nProposalMode), nRequestID);
			}
			break;
		case MC_MATCH_REPLY_AGREEMENT:
			{
				MUID uidProposer, uidChar;
				char szReplierName[256];
				int nProposalMode, nRequestID;
				bool bAgreement;

				pCommand->GetParameter(&uidProposer,		0, MPT_UID);
				pCommand->GetParameter(&uidChar,			1, MPT_UID);
				pCommand->GetParameter(szReplierName,		2, MPT_STR, sizeof(szReplierName) );
				pCommand->GetParameter(&nProposalMode,		3, MPT_INT);
				pCommand->GetParameter(&nRequestID,			4, MPT_INT);
				pCommand->GetParameter(&bAgreement,			5, MPT_BOOL);

				OnReplyAgreement(uidProposer, uidChar, szReplierName, MMatchProposalMode(nProposalMode),
					             nRequestID, bAgreement);

			}

			break;

		// 레더 커맨드
		case MC_MATCH_LADDER_SEARCH_RIVAL:	// 검색 시작
			{
				ZGetGameInterface()->OnArrangedTeamGameUI(true);
			}break;
		case MC_MATCH_LADDER_CANCEL_CHALLENGE:
			{
				bool PlayerWars;
				pCommand->GetParameter(&PlayerWars, 1, MPT_BOOL);
				ZGetGameInterface()->OnArrangedTeamGameUI(false);
				if(PlayerWars == false)
				{
				
				char szCharName[MATCHOBJECT_NAME_LENGTH];
				pCommand->GetParameter(szCharName, 0, MPT_STR, sizeof(szCharName) );
				
				if(szCharName[0]!=0) {
					char szOutput[256];
					ZTransMsg(szOutput,MSG_LADDER_CANCEL,1,szCharName);
					ZChatOutput(MCOLOR(ZCOLOR_CHAT_SYSTEM), szOutput);

				}else	 // 이름이 없으면 실패한경우다.
				{
					ZChatOutput(MCOLOR(ZCOLOR_CHAT_SYSTEM), 
						ZMsg(MSG_LADDER_FAILED) );
				}
				}
			}break;
		case MC_MATCH_LADDER_RESPONSE_CHALLENGE:
			{
				int nResult;
				pCommand->GetParameter(&nResult, 0, MPT_INT);
				OnLadderResponseChallenge(nResult);
			}
			break;
		case MC_MATCH_LADDER_PREPARE:
			{
				MUID uidStage;
				pCommand->GetParameter(&uidStage, 0, MPT_UID);
				int nTeam;
				pCommand->GetParameter(&nTeam, 1, MPT_INT);

				OnLadderPrepare(uidStage, nTeam);
			}break;
		case MC_MATCH_PLAYERWARS_COUNTER:
			{
				if(ZGetGameInterface()->GetState() == GUNZ_LOBBY)
				{
					MCommandParameter* pParam = pCommand->GetParameter(0);
					if (pParam->GetType() != MPT_BLOB) return false;
					void* pBlob = pParam->GetPointer();
					MTD_PlayerWarsCounterInfo* Info = (MTD_PlayerWarsCounterInfo*)MGetBlobArrayElement(pBlob, 0);
					ZIDLResource* pRes = ZApplication::GetGameInterface()->GetIDLResource();
					if (!pRes) return false;
					char szOutput[256];
					char szWigetName[100];
					for(int i = 0; i <= 7; i++)
					{
						sprintf(szWigetName, "Lobby_PlayerWarsCount%d", i);
						sprintf(szOutput,"%d Players Queuing", Info->Count[i]);
						MLabel* pLabel = (MLabel*)pRes->FindWidget(szWigetName);
						if(pLabel)
						pLabel->SetText(szOutput);
					}
				}
			}
			break;
		case MC_MATCH_LADDER_LAUNCH:		// 게임 시작
			{
				MUID uidStage;
				pCommand->GetParameter(&uidStage, 0, MPT_UID);
				char szMapName[128];
				pCommand->GetParameter(szMapName, 1, MPT_STR, sizeof(szMapName) );
				bool PlayerWars;
				pCommand->GetParameter(&PlayerWars, 2, MPT_BOOL);
				OnLadderLaunch(uidStage, szMapName, PlayerWars);
			}break;
		case MC_MATCH_CLAN_STANDBY_CLAN_LIST:
			{
				int nPrevStageCount, nNextStageCount;
				pCommand->GetParameter(&nPrevStageCount, 0, MPT_INT);
				pCommand->GetParameter(&nNextStageCount, 1, MPT_INT);

				MCommandParameter* pParam = pCommand->GetParameter(2);
				if(pParam->GetType()!=MPT_BLOB) break;
				void* pBlob = pParam->GetPointer();

				OnClanStandbyClanList(nPrevStageCount, nNextStageCount, pBlob);
			}
			break;
		case MC_MATCH_CLAN_MEMBER_CONNECTED:
			{
				char szMember[256];

				pCommand->GetParameter(szMember, 0, MPT_STR, sizeof(szMember) );
				OnClanMemberConnected(szMember);
			}
			break;
		case MC_MATCH_NOTIFY_CALLVOTE:
			{
				char szDiscuss[128] = "";
				char szArg[256] = "";

				pCommand->GetParameter(szDiscuss, 0, MPT_STR, sizeof(szDiscuss) );
				pCommand->GetParameter(szArg, 1, MPT_STR, sizeof(szArg) );
				OnNotifyCallVote(szDiscuss, szArg);
			}
			break;
		case MC_MATCH_NOTIFY_VOTERESULT:
			{
				char szDiscuss[128];
				int nResult = 0;

				pCommand->GetParameter(szDiscuss, 0, MPT_STR, sizeof(szDiscuss) );
				pCommand->GetParameter(&nResult, 1, MPT_INT);
				OnNotifyVoteResult(szDiscuss, nResult);
			}
			break;
		case MC_MATCH_VOTE_RESPONSE:
			{
				int nMsgCode = 0;
				pCommand->GetParameter( &nMsgCode, 0, MPT_INT );
				OnVoteAbort( nMsgCode );
			}
			break;
		case MC_MATCH_BROADCAST_CLAN_RENEW_VICTORIES:
			{
				char szWinnerClanName[256], szLoserClanName[256];
				int nVictories;

				pCommand->GetParameter(szWinnerClanName,	0, MPT_STR, sizeof(szWinnerClanName) );
				pCommand->GetParameter(szLoserClanName,		1, MPT_STR, sizeof(szLoserClanName) );
				pCommand->GetParameter(&nVictories,			2, MPT_INT);
				OnBroadcastClanRenewVictories(szWinnerClanName, szLoserClanName, nVictories);
			}
			break;
		case MC_MATCH_BROADCAST_CLAN_INTERRUPT_VICTORIES:
			{
				char szWinnerClanName[256], szLoserClanName[256];
				int nVictories;

				pCommand->GetParameter(szWinnerClanName,	0, MPT_STR, sizeof(szWinnerClanName) );
				pCommand->GetParameter(szLoserClanName,		1, MPT_STR, sizeof(szLoserClanName) );
				pCommand->GetParameter(&nVictories,			2, MPT_INT);
				OnBroadcastClanInterruptVictories(szWinnerClanName, szLoserClanName, nVictories);
			}
			break;
		case MC_MATCH_BROADCAST_DUEL_RENEW_VICTORIES:
			{
				char szChannelName[256], szChampionName[256];
				int nVictories, nRoomNo;

				pCommand->GetParameter(szChampionName,		0, MPT_STR, sizeof(szChampionName) );
				pCommand->GetParameter(szChannelName,		1, MPT_STR, sizeof(szChannelName) );
				pCommand->GetParameter(&nRoomNo,			2, MPT_INT);
				pCommand->GetParameter(&nVictories,			3, MPT_INT);
				OnBroadcastDuelRenewVictories(szChampionName, szChannelName, nRoomNo, nVictories);
			}
			break;
		case MC_MATCH_BROADCAST_DUEL_INTERRUPT_VICTORIES:
			{
				char szChampionName[256], szInterrupterName[256];
				int nVictories;

				pCommand->GetParameter(szChampionName,		0, MPT_STR, sizeof(szChampionName) );
				pCommand->GetParameter(szInterrupterName,	1, MPT_STR, sizeof(szInterrupterName) );
				pCommand->GetParameter(&nVictories,			2, MPT_INT);
				OnBroadcastDuelInterruptVictories(szChampionName, szInterrupterName, nVictories);
			}
			break;
		case MC_MATCH_RESPONSE_STAGE_FOLLOW:
			{
				int nMsgID;
				pCommand->GetParameter( &nMsgID, 0, MPT_INT );
				OnFollowResponse( nMsgID );
			}
			break;
		case MC_MATCH_SCHEDULE_ANNOUNCE_SEND :
			{
				char cAnnounce[ 512 ] = {0};
				pCommand->GetParameter( cAnnounce, 0, MPT_STR , sizeof(cAnnounce) );
				ZChatOutput( cAnnounce );
			}
			break;
		case MC_MATCH_EXPIRED_RENT_ITEM:
			{
				MCommandParameter* pParam = pCommand->GetParameter(0);
				if(pParam->GetType()!=MPT_BLOB) break;
				void* pBlob = pParam->GetPointer();

				AntiOnExpire(pBlob);
			}
			break;
		case MC_MATCH_FIND_HACKING:
			{
			}
			break;
		case MC_MATCH_REWARD_BATTLE_TIME:
			{
				MUID uidOwner;
				char szRewardName[256], szRewardResetDesc[256];
				int nRemainReward;
				unsigned int nItemId, nItemCnt;
				unsigned int nRentHourPeriod;

				pCommand->GetParameter(&uidOwner,			0, MPT_UID);
				pCommand->GetParameter(szRewardName,		1, MPT_STR, sizeof(szRewardName) );
				pCommand->GetParameter(szRewardResetDesc,	2, MPT_STR, sizeof(szRewardResetDesc) );				
				pCommand->GetParameter(&nItemId,			3, MPT_UINT);
				pCommand->GetParameter(&nItemCnt,			4, MPT_UINT);
				pCommand->GetParameter(&nRentHourPeriod,	5, MPT_UINT);
				pCommand->GetParameter(&nRemainReward,		6, MPT_INT);

				const char* szCharName = "-";
				const char* szItemName = "-";

				if (ZGetCharacterManager())
				{
					ZCharacter* pChar = ZGetCharacterManager()->Find(uidOwner);
					if (pChar)
						szCharName = pChar->GetUserName();
				}

				MMatchItemDesc* pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemId);
				if (pItemDesc)
					szItemName = pItemDesc->m_pMItemName->Ref().m_szItemName;
				else
				{
					const ZGambleItemDefine* pGItemDef = ZGetGambleItemDefineMgr().GetGambleItemDefine(nItemId);
					if (pGItemDef)
						szItemName = pGItemDef->GetName().c_str();
				}

				// 이 커맨드는 
				// 1. 보상을 실제로 받은 경우
				// 2. 보상을 받을 수 있지만 기회가 남지 않아서 받을 수 없는 경우
				// 두 경우 모두 전송받는다. 따라서 상황에 맞게 적절한 처리가 필요하다

				bool bRewardReally = (nItemId != 0);
				bool bMyReward = (ZGetMyUID() == uidOwner);

				if (bRewardReally)
				{
					TimeReward_ShowCharEffect(uidOwner);	// 캐릭터 머리 위에 이펙트 출력
					TimeReward_ChatOutput_RewardGet(szRewardName, szCharName, szItemName);	// '누가 무엇을 받았다' 출력

					if (bMyReward)	// 나 자신을 위한 커맨드라면
					{
						if (nRemainReward >= 1)
							TimeReward_ChatOutput_RemainChance(nRemainReward);		// 남은 기회를 출력
						else if (nRemainReward == 0)
							TimeReward_ChatOutput_ResetChance(szRewardResetDesc);	// 이번에 받은 것이 마지막 기회였다면 재충전 시각 공지
						else if (nRemainReward == -1)
							int a=0;// 이 경우는 별도의 기회 제한이 없는 이벤트를 나타낸다, 특별히 기회에 대한 출력문을 보여주지 않는다
					}
				}
				else	// 조건은 충족했으나 남은 기회가 없어 받지 못한 경우
				{
					if (bMyReward)
					{
						TimeReward_ChatOutput_NoChance(szRewardName);				// 기회가 없어서 받지 못함을 알려주고
						TimeReward_ChatOutput_ResetChance(szRewardResetDesc);		// 재충전 시각을 공지
					}
				}
			}
			break;

		// 듀얼 토너먼트
#ifdef _DUELTOURNAMENT
		case MC_MATCH_DUELTOURNAMENT_RESPONSE_JOINGAME:
			{
				int nResult;
				pCommand->GetParameter(&nResult,	0, MPT_INT);

				switch (nResult)
				{
				case MERR_DT_WRONG_CHANNEL:
					ZApplication::GetGameInterface()->ShowErrorMessage( nResult );
					mlog("Error: Illegal request to join game, This isn't a duel tournament channel.\n");
					break;
				case MERR_DT_CANNOT_CHALLENGE:
					ZApplication::GetGameInterface()->ShowErrorMessage( nResult );
					mlog("Error: failed to challenge a duel tournament game.\n");
					break;
				case MERR_DT_ALREADY_JOIN:
					ZApplication::GetGameInterface()->ShowErrorMessage( nResult );
					mlog("Error: already trying to join a duel tournament game.\n");
					break;
				}
			}
			break;

		case MC_MATCH_DUELTOURNAMENT_PREPARE_MATCH:
			{
				MUID uidStage = MUID(0,0);
				int nType;
				pCommand->GetParameter(&uidStage, 0, MPT_UID);
				pCommand->GetParameter(&nType, 1, MPT_INT);
				MCommandParameter* pParam = pCommand->GetParameter(2);
				void* pBlobPlayerInfo = pParam->GetPointer();

				OnDuelTournamentPrepare((MDUELTOURNAMENTTYPE)nType, uidStage, pBlobPlayerInfo);
			}
			break;
		case MC_MATCH_DUELTOURNAMENT_LAUNCH_MATCH:
			{
				MUID uidStage;
				pCommand->GetParameter(&uidStage, 0, MPT_UID);
				char szMapName[128];
				pCommand->GetParameter(szMapName, 1, MPT_STR, sizeof(szMapName) );


				ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
				MWidget* pWidget = pResource->FindWidget("DuelTournamentWaitMatchDialog");
				if(pWidget!=NULL)
					pWidget->Show(false);

				OnDuelTournamentLaunch(uidStage, szMapName);
			}
			break;
		case MC_MATCH_DUELTOURNAMENT_NOT_SERVICE_TIME:
			{
				int nOpenStartTime;
				int nOpenEndTime;
				pCommand->GetParameter(&nOpenStartTime,	0, MPT_INT);
				pCommand->GetParameter(&nOpenEndTime,	1, MPT_INT);

				// 듀얼토너먼트 신청이 서비스 시간에 따라 취소 되었음.
				ZApplication::GetGameInterface()->OnDuelTournamentGameUI(false); // 참가 신청 박스 닫아준다.
				const char *strFormat = ZErrStr( MERR_DT_NOT_SERVICE_TIME );
				if(strFormat)
				{
					char text[1024];
					sprintf(text, strFormat, nOpenStartTime, nOpenEndTime);
					ZApplication::GetGameInterface()->ShowErrorMessage(text, MERR_DT_NOT_SERVICE_TIME);
				}
			}
			break;
		case MC_MATCH_DUELTOURNAMENT_CHAR_INFO:
			{
				pCommand->GetParameter(&m_dtCharInfo.tournamentPoint,	0, MPT_INT);
				pCommand->GetParameter(&m_dtCharInfo.wins,				1, MPT_INT);
				pCommand->GetParameter(&m_dtCharInfo.losses,			2, MPT_INT);
				pCommand->GetParameter(&m_dtCharInfo.ranking,			3, MPT_INT);
				//pCommand->GetParameter(&rankingFructuation,	4, MPT_INT);
				pCommand->GetParameter(&m_dtCharInfo.winners,			5, MPT_INT);
				pCommand->GetParameter(&m_dtCharInfo.lastWeekGrade,		6, MPT_INT);

				ZGetGameInterface()->UpdateDuelTournamantMyCharInfoUI();
			}
			break;
		case MC_MATCH_PLAYERWARS_CHARINFO:
		{
				pCommand->GetParameter(&m_PWCharInfo.Ranking,			0, MPT_INT);
				pCommand->GetParameter(&m_PWCharInfo.Wins,				1, MPT_INT);
				pCommand->GetParameter(&m_PWCharInfo.Losses,			2, MPT_INT);
				pCommand->GetParameter(&m_PWCharInfo.Draws,			3, MPT_INT);
				pCommand->GetParameter(&m_PWCharInfo.Score,			4, MPT_INT);
				ZGetGameInterface()->UpdatePlayerWarsMyCharInfo();
			}
			break;
		case MC_MATCH_DUELTOURNAMENT_CHAR_INFO_PREVIOUS:
			{
				pCommand->GetParameter(&m_dtCharInfoPrev.tournamentPoint,	0, MPT_INT);
				pCommand->GetParameter(&m_dtCharInfoPrev.wins,				1, MPT_INT);
				pCommand->GetParameter(&m_dtCharInfoPrev.losses,			2, MPT_INT);
				pCommand->GetParameter(&m_dtCharInfoPrev.ranking,			3, MPT_INT);
				pCommand->GetParameter(&m_dtCharInfoPrev.winners,			4, MPT_INT);

				ZGetGameInterface()->UpdateDuelTournamantMyCharInfoPreviousUI();
			}
			break;

#endif //_DUELTOURNAMENT

		// Gamble 아이템
		case MC_MATCH_RESPONSE_GAMBLE:
			{
				unsigned int nRecvItem;
				unsigned int nCnt;
				unsigned int nTime;

				pCommand->GetParameter(&nRecvItem,	0, MPT_UINT);
				pCommand->GetParameter(&nCnt,		1, MPT_UINT);
				pCommand->GetParameter(&nTime,		2, MPT_UINT);

				OnRecieveGambleItem( nRecvItem, nCnt, nTime);
			}
			break;

		case MC_QUEST_NPCLIST :
			{
				MCommandParameter* pParam = pCommand->GetParameter( 0 );
				if( MPT_BLOB != pParam->GetType() ) 
				{
					break;
				}

				void* pBlobNPCList = pParam->GetPointer();
				if( NULL == pBlobNPCList )
				{
					return false;
				}

				int gameType;
				if (!pCommand->GetParameter(&gameType, 1, MPT_INT))
				{
					ASSERT(0);
					return false;
				}

				OnQuestNPCList( pBlobNPCList, (MMATCH_GAMETYPE)gameType );
			}
			break;


		case MC_REQUEST_RESOURCE_CRC32 :
			{
				DWORD dwKey = 0;
				pCommand->GetParameter( &dwKey, 0, MPT_UINT );

				DWORD dwCrc32, dwXor;
				ZGetGame()->MakeResourceCRC32(dwKey, dwCrc32, dwXor);
				ZPostResponseResourceCRC32( dwCrc32, dwXor );
			}
			break;

		case MC_MATCH_ROUTE_UPDATE_STAGE_EQUIP_LOOK :
			{
				MUID uidPlayer;
				int nParts;
				int nItemID;

				pCommand->GetParameter( &uidPlayer, 0, MPT_UID );
				pCommand->GetParameter( &nParts, 1, MPT_INT );
				pCommand->GetParameter( &nItemID, 2, MPT_INT );

				OnResponseUpdateStageEquipLook( uidPlayer, nParts, nItemID );
			}
			break;

		case MC_ADMIN_RESPONSE_KICK_PLAYER:
			{
				int nResult;
				pCommand->GetParameter( &nResult, 0, MPT_INT );
				OnAdminResponseKickPlayer(nResult);				
			}
			break;

		case MC_ADMIN_RESPONSE_BLOCK_PLAYER:
			{
				int nResult;
				pCommand->GetParameter( &nResult, 0, MPT_INT );
				OnAdminResponseBlockPlayer(nResult);
			}
			break;

		case MC_ADMIN_RESPONSE_MUTE_PLAYER:
			{
				int nResult;
				pCommand->GetParameter( &nResult, 0, MPT_INT );
				OnAdminResponseMutePlayer(nResult);
			}
			break;
		case MC_TRANSLATE_MSG:
		{
		}

		default:
			if (!ret)
			{
//				MClient::OutputMessage(MZMOM_LOCALREPLY, "Command(%s) handler not found", pCommand->m_pCommandDesc->GetName());
//				return false;
			}
			break;
	}

	if (m_fnOnCommandCallback) ret = m_fnOnCommandCallback(pCommand);


	return ret;
}