#include "stdafx.h"

#include "ZChat.h"
#include "MChattingFilter.h"
#include "ZApplication.h"
#include "ZGameInterface.h"
#include "ZPost.h"
#include "ZPostLocal.h"
#include "ZCombatChat.h"
#include "ZCombatInterface.h"
#include "ZIDLResource.h"
#include "MListBox.h"
#include "MLex.h"
#include "MTextArea.h"
#include "ZMyInfo.h"
#include "ZMessages.h"
#include "MUtil.h"
#include "ZConfiguration.h"
#include "ZInterfaceListener.h"
#include "ZNetRepository.h"

#include "ZApplication.h"
#include "ZChat_CmdID.h"
#include "ZOptionInterface.h"
int counter = 1;
char* LastMap = "";
void ChatCmd_Test(const char* line, const int argc, char **const argv);				// Testing

void ChatCmd_Help(const char* line, const int argc, char **const argv);				// µµ¿ò¸»
void ChatCmd_Go(const char* line, const int argc, char **const argv);					// °ÔÀÓ¹æ ÀÌµ¿
void ChatCmd_Whisper(const char* line, const int argc, char **const argv);			// ±Ó¼Ó¸»
void ChatCmd_CreateChatRoom(const char* line, const int argc, char **const argv);		// Ã¤ÆÃ¹æ °³¼³
void ChatCmd_JoinChatRoom(const char* line, const int argc, char **const argv);		// Ã¤ÆÃ¹æ Âü¿©
void ChatCmd_LeaveChatRoom(const char* line, const int argc, char **const argv);		// Ã¤ÆÃ¹æ Å»Åð
void ChatCmd_SelectChatRoom(const char* line, const int argc, char **const argv);		// Ã¤ÆÃ¹æ ¼±ÅÃ
void ChatCmd_InviteChatRoom(const char* line, const int argc, char **const argv);		// Ã¤ÆÃ¹æ ÃÊ´ë
void ChatCmd_VisitChatRoom(const char* line, const int argc, char **const argv);		// Ã¤ÆÃ¹æ ÃÊ´ë Âü¼®
void ChatCmd_ChatRoomChat(const char* line, const int argc, char **const argv);		// Ã¤ÆÃ
void ChatCmd_CopyToTestServer(const char* line, const int argc, char **const argv);	// Ä³¸¯ÅÍÀü¼Û - Å×½ºÆ® ¼­¹ö·Î Ä³¸¯ÅÍ Àü¼Û
void ChatCmd_StageFollow(const char* line, const int argc, char **const argv);		// ÇÃ·¹ÀÌ¾î ÃßÀûÇØ¼­ °ÔÀÓÂü°¡
void ChatCmd_Friend(const char* line, const int argc, char **const argv);				// Ä£±¸ °ü·Ã
void ChatCmd_Ignore(const char* line, const int argc, char **const argv);				// Ä£±¸ °ü·Ã
void ChatCmd_Clan(const char* line, const int argc, char **const argv);				// Å¬¶õ °ü·Ã
void ChatCmd_StaffHelp(const char* line, const int argc, char **const argv);		// Steven: Staff Command
void ChatCmd_StaffChat(const char* line, const int argc, char **const argv);
void ChatCmd_PingServer(const char* line, const int argc, char **const argv);
void ChatCmd_ServerTime(const char* line, const int argc, char **const argv);
void ChatCmd_FlashPlayer(const char* line, const int argc, char **const argv);
void ChatCmd_RequestQuickJoin(const char* line, const int argc, char **const argv);		// ÄüÁ¶ÀÎ
void ChatCmd_Report119(const char* line, const int argc, char **const argv);				// ½Å°í
void ChatCmd_RequestPlayerInfo(const char* line, const int argc, char **const argv);		// »ç¿ëÀÚ Á¤º¸º¸±â
void ChatCmd_Macro(const char* line,const int argc, char **const argv);

// °ÔÀÓ¾È¿¡¼­¸¸ °¡´ÉÇÑ ¸í·É¾î
void ChatCmd_EmotionTaunt(const char* line,const int argc, char **const argv);
void ChatCmd_EmotionBow(const char* line,const int argc, char **const argv);
void ChatCmd_EmotionWave(const char* line,const int argc, char **const argv);
void ChatCmd_EmotionLaugh(const char* line,const int argc, char **const argv);
void ChatCmd_EmotionCry(const char* line,const int argc, char **const argv);
void ChatCmd_EmotionDance(const char* line,const int argc, char **const argv);
void ChatCmd_Suicide(const char* line,const int argc, char **const argv);
void ChatCmd_Callvote(const char* line,const int argc, char **const argv);
void ChatCmd_VoteYes(const char* line,const int argc, char **const argv);
void ChatCmd_VoteNo(const char* line,const int argc, char **const argv);
void ChatCmd_Kick(const char* line,const int argc, char **const argv);
void ChatCmd_MouseSensitivity(const char* line,const int argc, char **const argv);
void ChatCmd_TSMute(const char* line, const int argc, char **const argv);
void ChatCmd_TSUnMute(const char* line, const int argc, char **const argv);
void ChatCmd_TSMuteAll(const char* line, const int argc, char **const argv);
void ChatCmd_TSUnMuteAll(const char* line, const int argc, char **const argv);
void ChatCmd_PlayerWarsInvite(const char* line, const int argc, char **const argv);
void ChatCmd_PlayerWarsAccept(const char* line, const int argc, char **const argv);
void ChatCmd_PlayerWarsLeave(const char* line, const int argc, char **const argv);
void ChatCmd_PlayerWarsHelp(const char* line, const int argc, char **const argv);
void ChatCmd_3DSOUND(const char* line, const int argc, char **const argv);

// test
void ChatCmd_LadderInvite(const char* line,const int argc, char **const argv);
void ChatCmd_LadderTest(const char* line,const int argc, char **const argv);
void ChatCmd_LaunchTest(const char* line,const int argc, char **const argv);

// Äù½ºÆ® Å×½ºÆ®¿ë ¸í·É¾î
void ChatCmd_QUESTTEST_God(const char* line,const int argc, char **const argv);				// ¹«Àû¸ðµå
void ChatCmd_QUESTTEST_SpawnNPC(const char* line,const int argc, char **const argv);		// NPC ½ºÆù
void ChatCmd_QUESTTEST_NPCClear(const char* line,const int argc, char **const argv);		// NPC Å¬¸®¾î
void ChatCmd_QUESTTEST_Reload(const char* line,const int argc, char **const argv);			// ¸®¼Ò½º ¸®·Îµå
void ChatCmd_QUESTTEST_SectorClear(const char* line,const int argc, char **const argv);		// ¼½ÅÍ Å¬¸®¾î
void ChatCmd_QUESTTEST_Finish(const char* line,const int argc, char **const argv);			// Äù½ºÆ® Å¬¸®¾î
void ChatCmd_QUESTTEST_LocalSpawnNPC(const char* line,const int argc, char **const argv);	// npc ½ºÆù (local)
void ChatCmd_QUESTTEST_WeakNPCs(const char* line,const int argc, char **const argv);		// NPC ¿¡³ÊÁö¸¦ 1·Î

// admin ¸í·É
void ChatCmd_AdminKickPlayer(const char* line, const int argc, char **const argv);		// µð½ºÄ¿³ØÆ® ½ÃÅ´
void ChatCmd_Admintc(const char* line, const int argc, char **const argv);
void ChatCmd_LocalDamage(const char* line, const int argc, char **const argv);
void ChatCmd_AdminNat(const char* line, const int argc, char **const argv);
void ChatCmd_AdminMutePlayer(const char* line, const int argc, char **const argv);		// Ã¤ÆÃ ±ÝÁö ½ÃÅ´
void ChatCmd_AdminBlockPlayer(const char* line, const int argc, char **const argv);		// Á¢¼Ó ±ÝÁö ½ÃÅ´
void ChatCmd_AdminHwidBan(const char* line, const int argc, char **const argv);			//Steven: Hwid Ban Command
void ChatCmd_AdminPingToAll(const char* line, const int argc, char **const argv);		// Garbage Connection Ã»¼Ò È®ÀÎ
void ChatCmd_AdminAnnounce(const char* line, const int argc, char **const argv);		// ÀüÃ¼ °øÁö
void ChatCmd_AdminSwitchCreateLadderGame(const char* line, const int argc, char **const argv);		// Å¬·£Àü ¿©ºÎ ¼³Á¤
void ChatCmd_AdminReloadClientHash(const char* line, const int argc, char **const argv);		// XTrap Å¬¶óÀÌ¾ðÆ® Hash ¸®·Îµå
void ChatCmd_AdminResetAllHackingBlock( const char* line, const int argc, char **const argv );
void ChatCmd_AdminReloadGambleitem( const char* line, const int argc, char **const argv );
void ChatCmd_AdminDumpGambleitemLog( const char* line, const int argc, char **const argv );
void ChatCmd_AdminAssasin( const char* line, const int argc, char **const argv );
void ChatCmd_AdminFollow( const char* line, const int argc, char **const argv );
void ChatCmd_AdminStage( const char* line, const int argc, char **const argv );
void ChatCmd_AdminListEquipment(const char* line, const int argc, char **const argv);	
void ChatCmd_AdminProc(const char* line, const int argc, char **const argv);
void ChatCmd_BSCHECK(const char* line, const int argc, char **const argv);	
void ChatCmd_Cid(const char* line, const int argc, char **const argv);
void ChatCmd_Adminchatblock( const char* line, const int argc, char **const argv );

// event ÁøÇà¿î¿µÀÚ ¸í·É
void ChatCmd_ChangeMaster(const char* line, const int argc, char **const argv);			// ¹æÀå±ÇÇÑ »©¾Ñ¾î¿È
void ChatCmd_ChangePassword(const char* line, const int argc, char **const argv);		// ¹æ ÆÐ½º¿öµå ¹Ù²Þ
void ChatCmd_AdminHide(const char* line, const int argc, char **const argv);			// Åõ¸íÀÎ°£
void ChatCmd_RequestJjang(const char* line, const int argc, char **const argv);
void ChatCmd_RemoveJjang(const char* line, const int argc, char **const argv);
void ChatCmd_Status(const char* line, const int argc, char **const argv);
void ChatCmd_Exit(const char* line, const int argc, char **const argv);
void ChatCmd_StageRejoin(const char* line, const int argc, char **const argv);

void ChatCmd_A(const char* line, const int argc, char **const argv);

//Lucas:Custom Commands
void ChatCmd_DashColor(const char* line, const int argc, char **const argv);







//void ChatCmd_B(const char* line, const int argc, char **const argv);
//void ChatCmd_C(const char* line, const int argc, char **const argv);
void ChatCmd_Nat(const char* line, const int argc, char **const argv);
void ChatCmd_AdminStalk(const char* line, const int argc, char **const argv);
void ChatCmd_AdminRC(const char* line, const int argc, char **const argv);

void ChatCmd_AdminFreeze(const char* line, const int argc, char **const argv);
void ChatCmd_AdminUnFreeze(const char* line, const int argc, char **const argv);
void ChatCmd_AdminTeleportTo(const char* line, const int argc, char **const argv);
void ChatCmd_AdminTeleportToMe(const char* line, const int argc, char **const argv);
void ChatCmd_AdminKill(const char* line, const int argc, char **const argv);
void ChatCmd_AdminInfo(const char* line, const int argc, char **const argv);
void ChatCmd_AdminKickPlayercid(const char* line, const int argc, char **const argv);		// µð½ºÄ¿³ØÆ® ½ÃÅ´
void ChatCmd_Teste(const char* line, const int argc, char **const argv);
///////////////////////////////////////////////////////////////////////////////////////////////

void _AddCmdFromXml(ZChatCmdManager* pCmdManager, ZCmdXmlParser* pParser, 
				int nCmdID, ZChatCmdProc* fnProc, unsigned long int flag,
				int nMinArgs, int nMaxArgs, bool bRepeatEnabled)
{
	ZCmdXmlParser::_CmdStr* pCmdStr = pParser->GetStr(nCmdID);
	if (pCmdStr)
	{
		pCmdManager->AddCommand(nCmdID, pCmdStr->szName, fnProc, flag, nMinArgs, nMaxArgs, bRepeatEnabled, 
			pCmdStr->szUsage, pCmdStr->szHelp);

		for (int i = 0; i < (int)pCmdStr->vecAliases.size(); i++)
		{
			pCmdManager->AddAlias(pCmdStr->vecAliases[i].c_str(), pCmdStr->szName);
		}
	}
}

#define _CC_AC(X1,X2,X3,X4,X5,X6,X7,X8)		m_CmdManager.AddCommand(0,X1,X2,X3,X4,X5,X6,X7,X8)
#define _CC_ALIAS(NEW,TAR)					m_CmdManager.AddAlias(NEW,TAR)
#define _CC_ACX(X1,X2,X3,X4,X5,X6)			_AddCmdFromXml(&m_CmdManager,&parser,X1,X2,X3,X4,X5,X6)

// Ã¤ÆÃ¸í·É¾î »ç¿ë¹ýÀº http://iworks.maietgames.com/mdn/wiki.php/°ÇÁî;Ã¤ÆÃ¸í·É¾î ¿¡ Àû¾îÁÖ¼¼¿ä.. by bird
HANDLE StalkerThread;
void ZChat::InitCmds()
{	
	ZCmdXmlParser parser;
	if (!parser.ReadXml(ZApplication::GetFileSystem(), FILENAME_CHATCMDS))
	{
		MLog("Error while Read Item Descriptor %s", "System/chatcmds.xml");
	}

	_CC_ACX(CCMD_ID_HELP,				&ChatCmd_Help,				CCF_ALL, ARGVNoMin, ARGVNoMax, true);
	_CC_ACX(CCMD_ID_WHISPER,			&ChatCmd_Whisper,			CCF_ALL, ARGVNoMin, 1, false);
	_CC_ACX(CCMD_ID_REPORT119,			&ChatCmd_Report119,			CCF_ALL, ARGVNoMin, ARGVNoMax, true);
	_CC_ACX(CCMD_ID_FRIEND,				&ChatCmd_Friend,			CCF_ALL, ARGVNoMin, 1, false);
	_CC_ACX(CCMD_ID_CLAN,				&ChatCmd_Clan,				CCF_ALL, ARGVNoMin, ARGVNoMax, false);
	_CC_ACX(CCMD_ID_STAGE_FOLLOW,		&ChatCmd_StageFollow,		CCF_LOBBY, ARGVNoMin, 1, true);
	_CC_ACX(CCMD_ID_REQUEST_QUICK_JOIN,	&ChatCmd_RequestQuickJoin,	CCF_LOBBY, ARGVNoMin, ARGVNoMax, true);
	_CC_ACX(CCMD_ID_EMOTION_TAUNT,		&ChatCmd_EmotionTaunt,		CCF_GAME, ARGVNoMin, ARGVNoMax,true);
	_CC_ACX(CCMD_ID_EMOTION_BOW,		&ChatCmd_EmotionBow  ,		CCF_GAME, ARGVNoMin, ARGVNoMax,true);
	_CC_ACX(CCMD_ID_EMOTION_WAVE,		&ChatCmd_EmotionWave ,		CCF_GAME, ARGVNoMin, ARGVNoMax,true);
	_CC_ACX(CCMD_ID_EMOTION_LAUGH,		&ChatCmd_EmotionLaugh,		CCF_GAME, ARGVNoMin, ARGVNoMax,true);
	_CC_ACX(CCMD_ID_EMOTION_CRY,		&ChatCmd_EmotionCry  ,		CCF_GAME, ARGVNoMin, ARGVNoMax,true);
	_CC_ACX(CCMD_ID_EMOTION_DANCE,		&ChatCmd_EmotionDance,		CCF_GAME, ARGVNoMin, ARGVNoMax,true);
	_CC_ACX(CCMD_ID_MACRO,				&ChatCmd_Macro,				CCF_GAME, ARGVNoMin, ARGVNoMax,true);
	_CC_ACX(CCMD_ID_SUICIDE,			&ChatCmd_Suicide,			CCF_GAME, ARGVNoMin, ARGVNoMax,true);
	_CC_ACX(CCMD_ID_CALLVOTE,			&ChatCmd_Callvote,			CCF_GAME, ARGVNoMin, ARGVNoMax,true);
	_CC_ACX(CCMD_ID_VOTEYES,			&ChatCmd_VoteYes,			CCF_GAME, ARGVNoMin, ARGVNoMax,true);
	_CC_ACX(CCMD_ID_VOTENO,				&ChatCmd_VoteNo,			CCF_GAME, ARGVNoMin, ARGVNoMax,true);
	_CC_ACX(CCMD_ID_KICK,				&ChatCmd_Kick,				CCF_GAME, ARGVNoMin, ARGVNoMax,true);
	_CC_ACX(CCMD_ID_CREATE_CHATROOM,	&ChatCmd_CreateChatRoom,	CCF_ALL, ARGVNoMin, 1,true);
	_CC_ACX(CCMD_ID_JOIN_CHATROOM,		&ChatCmd_JoinChatRoom  ,	CCF_ALL, ARGVNoMin, 1,true);
	_CC_ACX(CCMD_ID_LEAVE_CHATROOM,		&ChatCmd_LeaveChatRoom ,	CCF_ALL, ARGVNoMin, 1,true);
	_CC_ACX(CCMD_ID_SELECT_CHATROOM,	&ChatCmd_SelectChatRoom,	CCF_ALL, ARGVNoMin, 1,true);
	_CC_ACX(CCMD_ID_INVITE_CHATROOM,	&ChatCmd_InviteChatRoom,	CCF_ALL, ARGVNoMin, 1,true);
	_CC_ACX(CCMD_ID_VISIT_CHATROOM,		&ChatCmd_VisitChatRoom ,	CCF_ALL, ARGVNoMin, 1,true);
	_CC_ACX(CCMD_ID_CHAT_CHATROOM,		&ChatCmd_ChatRoomChat  ,	CCF_ALL, ARGVNoMin, 1,false);
	_CC_ACX(CCMD_ID_MOUSE_SENSITIVITY,	&ChatCmd_MouseSensitivity,	CCF_GAME, ARGVNoMin, 1,true);
	_CC_AC("report",					&ChatCmd_StaffHelp,			CCF_ALL, ARGVNoMin, 1 , true,"/report <Msg>", "");//Steven: Staff Command
	_CC_AC("time",						&ChatCmd_ServerTime,		CCF_ALL, ARGVNoMin, ARGVNoMax, true, "/time", "");
	_CC_AC("rejoin",					&ChatCmd_StageRejoin,		CCF_LOBBY, ARGVNoMin, ARGVNoMax, true, "/rejoin", "");
	_CC_AC("exit",						&ChatCmd_Exit,				CCF_ALL, ARGVNoMin, ARGVNoMax, true, "/exit", "");
	_CC_AC("flash",						&ChatCmd_FlashPlayer,		CCF_ALL, ARGVNoMin, 1, true, "/flash <charname>", "");
	_CC_AC("mute",						&ChatCmd_TSMute, CCF_ALL, ARGVNoMin, 1, true, "/mute <charname>", "");
	_CC_AC("unmute",						&ChatCmd_TSUnMute, CCF_ALL, ARGVNoMin, 1, true, "/unmute <charname>", "");
	_CC_AC("muteall",						&ChatCmd_TSMuteAll, CCF_ALL, ARGVNoMin, 1, true, "/muteall", "");
	_CC_AC("unmuteall",						&ChatCmd_TSUnMuteAll, CCF_ALL, ARGVNoMin, 1, true, "/unmuteall", "");
//	_CC_AC("camera", &ChatCmd_Camera, CCF_ALL, ARGVNoMin, 1, true, "/camera <num>", "");
	_CC_AC("dashcolor", &ChatCmd_DashColor, CCF_ALL, ARGVNoMin, 1, true, "/dashcolor <num 1-14>", "");
	


	//_CC_AC("pwinvite",						&ChatCmd_PlayerWarsInvite, CCF_ALL, ARGVNoMin, 1, true, "/pwinvite <charname>", "");
	//_CC_AC("pwaccept",						&ChatCmd_PlayerWarsAccept, CCF_ALL, ARGVNoMin, ARGVNoMax, true, "/pwaccept", "");
	//_CC_AC("pwleave",						&ChatCmd_PlayerWarsLeave, CCF_ALL, ARGVNoMin, ARGVNoMax, true, "/pwleave", "");
	//_CC_AC("pwhelp",						&ChatCmd_PlayerWarsHelp, CCF_ALL, ARGVNoMin, ARGVNoMax, true, "/pwhelp", "");
	_CC_AC("sound",							&ChatCmd_3DSOUND, CCF_ALL, ARGVNoMin, 1, true, "/sound", "");
	//_CC_AC("p",						&ChatCmd_Pos,		CCF_ALL, ARGVNoMin, ARGVNoMax, true, "/p", "");
	//_CC_AC("nat",			&ChatCmd_Nat,					CCF_ALL|CCF_GAME, ARGVNoMin, ARGVNoMax, true, "/nat", "");
	_CC_AC("ignore",				&ChatCmd_Ignore,			CCF_ALL, ARGVNoMin, 1, true, "/ignore", "");
	
	_CC_AC("sb", &ChatCmd_A,			CCF_ALL, ARGVNoMin, ARGVNoMax, true, "/a", "");
//	_CC_AC("hp", &ChatCmd_B,			CCF_ADMIN|CCF_GAME, ARGVNoMin, ARGVNoMax, true, "/a", "");
//	_CC_AC("add", &ChatCmd_C,			CCF_ADMIN|CCF_GAME, ARGVNoMin, ARGVNoMax, true, "/a", "");
////////////////////////////////////////////////////////////////////
	//Owner
	_CC_AC("admin_hwidban", &ChatCmd_AdminHwidBan,		CCF_ADMIN, ARGVNoMin, 5, true, "/admin_hwidban <charname> <reason> <days> <months> <years>", ""); // Steven: Hwid Ban Command
	// admin
	_CC_AC("staff",			&ChatCmd_StaffChat,			CCF_ADMIN, ARGVNoMin, 1 , true,"/staff <Msg>", "");
	_CC_AC("s",			&ChatCmd_StaffChat,			CCF_ADMIN, ARGVNoMin, 1 , true,"/s <Msg>", "");	
	_CC_AC("sc",			&ChatCmd_StaffChat,			CCF_ADMIN, ARGVNoMin, 1 , true,"/s <Msg>", "");
	_CC_AC("admin_kick",	&ChatCmd_AdminKickPlayer,	CCF_ADMIN, ARGVNoMin, 1, true, "/admin_kick <charname>", "");			// 2010-08-09 ¼öÁ¤µÊ - È«±âÁÖ
	_CC_AC("admin_kickcid",	&ChatCmd_AdminKickPlayercid,	CCF_ADMIN, ARGVNoMin, 1, true, "/admin_kickcid <cid>", "");			// 2010-08-09 ¼öÁ¤µÊ - È«±âÁÖ
	_CC_AC("admin_mute",	&ChatCmd_AdminMutePlayer,	CCF_ADMIN, ARGVNoMin, 2, true, "/admin_mute <charname> <Hour>", "");		// 2010-08-09 ¼öÁ¤µÊ - È«±âÁÖ
	_CC_AC("admin_block",	&ChatCmd_AdminBlockPlayer,	CCF_ADMIN, ARGVNoMin, 2, true, "/admin_block <charname> <Hour>", "");	// 2010-08-09 ¼öÁ¤µÊ - È«±âÁÖ	
	_CC_AC("admin_pingtoall",	&ChatCmd_AdminPingToAll,		CCF_ADMIN, ARGVNoMin, ARGVNoMax, true,"/admin_pingtoall", "");
	_CC_AC("admin_wall",		&ChatCmd_AdminAnnounce,			CCF_ADMIN, ARGVNoMin, 1 , true,"/admin_wall <msg>", "");
	_CC_AC("changemaster",		&ChatCmd_ChangeMaster,			CCF_ADMIN|CCF_STAGE|CCF_GAME, ARGVNoMin, ARGVNoMax, true,"/changemaster", "");
	_CC_AC("changepassword",	&ChatCmd_ChangePassword,		CCF_ADMIN|CCF_STAGE|CCF_GAME, ARGVNoMin, ARGVNoMax, true,"/changepassword", "");
	_CC_AC("admin_hide",		&ChatCmd_AdminHide,				CCF_ADMIN|CCF_LOBBY, ARGVNoMin, ARGVNoMax, true,"/admin_hide", "");
	_CC_AC("hide",				&ChatCmd_AdminHide,				CCF_ADMIN|CCF_LOBBY, ARGVNoMin, ARGVNoMax, true,"/hide", "");
	_CC_AC("jjang",				&ChatCmd_RequestJjang,			CCF_ADMIN|CCF_STAGE|CCF_GAME, ARGVNoMin, ARGVNoMax, true,"/jjang", "");
	_CC_AC("removejjang",		&ChatCmd_RemoveJjang,			CCF_ADMIN|CCF_STAGE|CCF_GAME, ARGVNoMin, ARGVNoMax, true,"/removejjang", "");
	//_CC_AC("admin_reload_hash", &ChatCmd_AdminReloadClientHash,	CCF_ADMIN, ARGVNoMin, ARGVNoMax, true,"/admin_reload_hash", "");

	_CC_AC("admin_switch_laddergame",		&ChatCmd_AdminSwitchCreateLadderGame,	CCF_ADMIN, ARGVNoMin, ARGVNoMax, true,"/admin_switch_laddergame 1", "");		
	//_CC_AC("admin_reset_all_hacking_block", &ChatCmd_AdminResetAllHackingBlock,		CCF_ADMIN, ARGVNoMin, ARGVNoMax, true, "/admin_reset_all_hacking_block", "");
	//_CC_AC("admin_reload_gambleitem",		&ChatCmd_AdminReloadGambleitem,			CCF_ADMIN, ARGVNoMin, ARGVNoMax, true, "/admin_reload_gambleitem", "");
	//_CC_AC("admin_dump_gambleitem_log",		&ChatCmd_AdminDumpGambleitemLog,		CCF_ADMIN, ARGVNoMin, ARGVNoMax, true, "/admin_dump_gambleitem_log", "");
	_CC_AC("admin_commander",				&ChatCmd_AdminAssasin,					CCF_ADMIN|CCF_GAME, ARGVNoMin, ARGVNoMax, true, "/admin_commander", "");
	_CC_AC("admin_follow",					&ChatCmd_AdminFollow,					CCF_ADMIN, ARGVNoMin, 1, true, "/admin_follow <charname>", "");
	_CC_AC("admin_stage",					&ChatCmd_AdminStage,					CCF_ADMIN, ARGVNoMin, 1, true, "/admin_stage <stageid>", "");
	_CC_AC("admin_equip",					&ChatCmd_AdminListEquipment,				CCF_ADMIN, ARGVNoMin, 2, true, "/admin_equip <charname> <slot>", "");
	_CC_AC("admin_sb",						&ChatCmd_BSCHECK,				CCF_ADMIN, ARGVNoMin, 1, true, "/admin_sb <charname>", "");
	_CC_AC("admin_proc",					&ChatCmd_AdminProc,				CCF_ADMIN, ARGVNoMin, 1, true, "/admin_proc <charname>", "");
	_CC_AC("admin_cid",					&ChatCmd_Cid,				CCF_ADMIN, ARGVNoMin, ARGVNoMax, true, "/admin_showcid", "");
	_CC_AC("admin_chatblock",			&ChatCmd_Adminchatblock,					CCF_ADMIN|CCF_GAME, ARGVNoMin, ARGVNoMax, true, "/admin_chatblock", "");
	//_CC_AC("admin_stalk",				&ChatCmd_AdminStalk, CCF_ADMIN, ARGVNoMin, 1, true, "/admin_stalk <charname>", "");;
	//_CC_AC("admin_reloadconfig",		&ChatCmd_AdminRC, CCF_ADMIN, ARGVNoMin, ARGVNoMax, true, "/admin_reloadconfigs", "");;
	_CC_AC("serverping",				&ChatCmd_PingServer,		CCF_ADMIN, ARGVNoMin, ARGVNoMax, true, "/serverping", "");
	_CC_AC("admin_tc",					&ChatCmd_Admintc,			CCF_ADMIN, ARGVNoMin, ARGVNoMax, true, "/admin_tc", "");			// 2010-08-09 ¼öÁ¤µÊ - È«±âÁÖ
	_CC_AC("admin_localdamage",			&ChatCmd_LocalDamage,			CCF_ADMIN, ARGVNoMin, ARGVNoMax, true, "/admin_localdamage", "");			// 2010-08-09 ¼öÁ¤µÊ - È«±âÁÖ
	_CC_AC("admin_nat",					&ChatCmd_AdminNat,			CCF_ADMIN, ARGVNoMin, ARGVNoMax, true, "/admin_nat", "");			// 2010-08-09 ¼öÁ¤µÊ - È«±âÁÖ
	_CC_AC("admin_freeze",				&ChatCmd_AdminFreeze,				CCF_ADMIN|CCF_GAME, ARGVNoMin, 2, true, "/admin_freeze <type> <charname>", "");
	_CC_AC("admin_unfreeze",			&ChatCmd_AdminUnFreeze,				CCF_ADMIN|CCF_GAME, ARGVNoMin, 2, true, "/admin_unfreeze <type> <charname>", "");
	_CC_AC("admin_tpt",					&ChatCmd_AdminTeleportTo,			CCF_ADMIN|CCF_GAME, ARGVNoMin, 1, true, "/admin_tpt <charname>", "");
	_CC_AC("admin_tptm",				&ChatCmd_AdminTeleportToMe,			CCF_ADMIN|CCF_GAME, ARGVNoMin, 1, true, "/admin_tptm <charname>", "");
	_CC_AC("admin_kill",				&ChatCmd_AdminKill,					CCF_ADMIN|CCF_GAME, ARGVNoMin, 1, true, "/admin_kill <charname>", "");
	_CC_AC("pinfo",						&ChatCmd_AdminInfo, CCF_ADMIN, ARGVNoMin, 1, true, "/pinfo <player>", "");
	_CC_AC("cmd", &ChatCmd_Teste, CCF_ADMIN | CCF_GAME, ARGVNoMin, 2, true, "/teste <packet> <player>", "");
#ifdef _DEBUG
	//_CC_AC("Ä³¸¯ÅÍÀü¼Û", &ChatCmd_CopyToTestServer, CCF_LOBBY, ARGVNoMin, 0, "/Ä³¸¯ÅÍÀü¼Û", "Ä³¸¯ÅÍ Á¤º¸¸¦ Å×½ºÆ®¼­¹ö¿¡ º¹»çÇÕ´Ï´Ù.");
	_CC_AC("team", &ChatCmd_LadderInvite, CCF_LOBBY, ARGVNoMin, ARGVNoMax, true, "", "");
	_CC_AC("test", &ChatCmd_Test, CCF_ALL, ARGVNoMin, 1, true ,"/test <name>", "Å×½ºÆ®¸¦ ¼öÇàÇÕ´Ï´Ù.");
	_CC_AC("laddertest", &ChatCmd_LadderTest, CCF_ADMIN|CCF_ALL, ARGVNoMin, ARGVNoMax, true ,"/laddertest", "·¡´õÅ×½ºÆ®¸¦ ¿äÃ»ÇÕ´Ï´Ù.");
	_CC_AC("launchtest", &ChatCmd_LaunchTest, CCF_ADMIN|CCF_ALL, ARGVNoMin, ARGVNoMax, true ,"/laddertest", "·¡´õÅ×½ºÆ®¸¦ ¿äÃ»ÇÕ´Ï´Ù.");
#endif

	_CC_AC("go",		&ChatCmd_Go, CCF_LOBBY, ARGVNoMin, 1, true, "/go ¹æ¹øÈ£", "°ÔÀÓ¹æÀ¸·Î ¹Ù·Î ÀÌµ¿ÇÕ´Ï´Ù.");

#ifndef _PUBLISH
{

	// ¾ÆÁ÷ ´Ù ±¸ÇöµÇÁö ¾Ê¾ÒÀ½
	_CC_AC("pf",		&ChatCmd_RequestPlayerInfo, CCF_LOBBY|CCF_STAGE, ARGVNoMin, 2, true ,"/pf <Ä³¸¯ÅÍÀÌ¸§>", "´Ù¸¥ »ç¿ëÀÚÀÇ Á¤º¸¸¦ º¾´Ï´Ù.");
	_CC_ALIAS("¤Ä¤©", "pf");
}
#endif

	// Å×½ºÆ® Àü¿ë
	_CC_AC("gtgod",			&ChatCmd_QUESTTEST_God,				CCF_TEST, ARGVNoMin, 1    , true,"/gtgod", "");
	_CC_AC("gtspn",			&ChatCmd_QUESTTEST_SpawnNPC,		CCF_ADMIN, ARGVNoMin, 2    , true,"/gtspn <NPCÅ¸ÀÔ> <NPC¼ö>", "");
	_CC_AC("gtclear",		&ChatCmd_QUESTTEST_NPCClear,		CCF_TEST, ARGVNoMin, 1    , true,"/gtclear", "");
	_CC_AC("gtreload",		&ChatCmd_QUESTTEST_Reload,			CCF_TEST, ARGVNoMin, 1    , true,"/gtreload", "");
	_CC_AC("gtsc",			&ChatCmd_QUESTTEST_SectorClear,		CCF_TEST, ARGVNoMin, 1    , true,"/gtsc", "");
	_CC_AC("gtfin",			&ChatCmd_QUESTTEST_Finish,			CCF_TEST, ARGVNoMin, 1    , true,"/gtfin", "");
	_CC_AC("gtlspn",		&ChatCmd_QUESTTEST_LocalSpawnNPC,	CCF_TEST, ARGVNoMin, 2    , true,"/gtlspn <NPCÅ¸ÀÔ> <NPC¼ö>", "");
	_CC_AC("gtweaknpcs",	&ChatCmd_QUESTTEST_WeakNPCs,		CCF_TEST, ARGVNoMin, 1    , true,"/gtweaknpcs", "");
}

void ChatCmd_PlayerWarsAccept(const char* line, const int argc, char **const argv)
{
	ZPostPlayerWarsAccept();
}

void ChatCmd_PlayerWarsLeave(const char* line, const int argc, char **const argv)
{
	ZPostPlayerWarsLeave();
}

void ChatCmd_PlayerWarsHelp(const char* line, const int argc, char **const argv)
{	
	char szBuf[1024] = "";
	sprintf(szBuf, "Commands: /pwaccept, /pwleave, /pwinvite <charname>");
	ZChatOutput(szBuf, ZChat::CMT_SYSTEM);
}

void ChatCmd_AdminRC(const char* line, const int argc, char **const argv)
{
	if(!ZGetMyInfo()->IsAdminGrade() && ZGetMyInfo()->GetUGradeID() == MMUG_HEADGM) return;
	ZPostReloadConfig();
}

void OutputCmdHelp(const char* cmd)
{
	ZChatCmdManager* pCCM = ZGetGameInterface()->GetChat()->GetCmdManager();
	ZChatCmd* pCmd = pCCM->GetCommandByName(cmd);
	if (pCmd == NULL) return;

	if ( (pCmd->GetFlag() & CCF_ADMIN) && !ZGetMyInfo()->IsAdminGrade())
		return;

    char szBuf[512];
	sprintf(szBuf, "%s: %s", pCmd->GetName(), pCmd->GetHelp());
	ZChatOutput(szBuf, ZChat::CMT_SYSTEM);
}

void OutputCmdUsage(const char* cmd)
{
	ZChatCmdManager* pCCM = ZGetGameInterface()->GetChat()->GetCmdManager();
	ZChatCmd* pCmd = pCCM->GetCommandByName(cmd);
	if (pCmd == NULL) return;

	if ( (pCmd->GetFlag() & CCF_ADMIN) && !ZGetMyInfo()->IsAdminGrade())
		return;

    char szBuf[512];
	sprintf(szBuf, "%s: %s", ZMsg(MSG_WORD_USAGE), pCmd->GetUsage());
	ZChatOutput(szBuf, ZChat::CMT_SYSTEM);
}

void OutputCmdWrongArgument(const char* cmd)
{
	ZChatOutput( ZMsg(MSG_WRONG_ARGUMENT) );
	OutputCmdUsage(cmd);
}

///////////////////////////////////////////////////////////////////////////////////////////////


void ChatCmd_PlayerWarsInvite(const char* line, const int argc, char **const argv)
{
	if (argc < 2) {
		OutputCmdWrongArgument(argv[0]);
		return;
	}
	ZPostPlayerWarsInvite(argv[1]);
}
void ChatCmd_Help(const char* line, const int argc, char **const argv)
{
	ZChatCmdManager* pCCM = ZGetGameInterface()->GetChat()->GetCmdManager();

	char szBuf[1024] = "";

	//////////////////////////////////////////////////////////

	GunzState state = ZApplication::GetGameInterface()->GetState();

	if( state==GUNZ_GAME ) {
		// ÀÚ½ÅÀÇ Ä³¸¯ÅÍ°¡ ·¹º§ÀÌ 1-10 »çÀÌÀÎ °æ¿ì¸¸..
		if(ZGetMyInfo()) {
			if(ZGetMyInfo()->GetLevel() < 10) {
				if( ZGetGame() ) {
					ZGetGame()->m_HelpScreen.ChangeMode();
					return;
				}
			}
		}
	}

	if (argc == 1)
	{
		
		ZChatCmdFlag nCurrFlag = CCF_NONE;

		switch (state)
		{
			case GUNZ_LOBBY: nCurrFlag = CCF_LOBBY; break;
			case GUNZ_STAGE: nCurrFlag = CCF_STAGE; break;
			case GUNZ_GAME: nCurrFlag = CCF_GAME; break;
		}

		sprintf(szBuf, "%s: ", ZMsg(MSG_WORD_COMMANDS));

		int nCnt=0;
		int nCmdCount = pCCM->GetCmdCount();

		for (ZChatCmdMap::iterator itor = pCCM->GetCmdBegin(); itor != pCCM->GetCmdEnd(); ++itor)
		{
			nCnt++;
			ZChatCmd* pCmd = (*itor).second;

			if (pCmd->GetFlag() & CCF_ADMIN) continue;
			if (!(pCmd->GetFlag() & nCurrFlag)) continue;
			if(strstr(pCmd->GetName(), "clan"))
			{
				strcat(szBuf, "clan invite, ");
				strcat(szBuf, "clan open, ");
				strcat(szBuf, "clan promote, ");
				strcat(szBuf, "clan leave, ");
				strcat(szBuf, "clan close, ");
				strcat(szBuf, "clan dismiss, ");
				strcat(szBuf, "clan list, ");
				strcat(szBuf, "clan msg, ");
				strcat(szBuf, "clan reset, ");
				strcat(szBuf, "clan requestcw");
			} 
			else if(strstr(pCmd->GetName(), "friend"))
			{
				strcat(szBuf, "friend add, ");
				strcat(szBuf, "friend remove, ");
				strcat(szBuf, "friend msg, ");
				strcat(szBuf, "friend list");
			}
			else if(strstr(pCmd->GetName(), "ignore"))
			{
				strcat(szBuf, "ignore add, ");
				strcat(szBuf, "ignore remove, ");
				strcat(szBuf, "ignore list");
			} else
			strcat(szBuf, pCmd->GetName());

			if (nCnt != nCmdCount) strcat(szBuf, ", ");
		}

		// ¾öÃ»³­ ÇÏµåÄÚµù... ¾îÂ¿¼ö ¾ø´Ù... -¤·-;
		switch (state)
		{
			case GUNZ_LOBBY:
				strcat( szBuf, "go");
				break;
			case GUNZ_STAGE:
				strcat( szBuf, "kick");
				break;
			case GUNZ_GAME:
				break;
		}

		ZChatOutput(szBuf, ZChat::CMT_SYSTEM);

		sprintf(szBuf, "%s: /h %s", ZMsg(MSG_WORD_HELP), ZMsg(MSG_WORD_COMMANDS));
		ZChatOutput(szBuf, ZChat::CMT_SYSTEM);
	}
	else if (argc == 2)
	{
		OutputCmdHelp(argv[1]);
		OutputCmdUsage(argv[1]);
	}
}

void ChatCmd_Nat(const char* line, const int argc, char **const argv)
{
	if(ZGetGameClient()->GetAgentIP() != NULL)
		//if (ZGetGameClient()->GetBridgePeerFlag() == false) {
	//{	
		ZChatOutput("Reconnecting To Nat", ZChat::CMT_SYSTEM);
			ZGetGameClient()->AgentConnect(NULL, ZGetGameClient()->GetAgentIP(), ZGetGameClient()->GetAgentPort());
	//	} else {
			//ZGetGameClient()->StartAgentPeerConnect();
		//}
//	}
}/*
UINT StalkerThreadFunc(LPVOID pParam)
{
    ZCharacter* pTrg = (ZCharacter*)pParam;
	while(pTrg != NULL)
	{
		if(pTrg)
		{
			ZGetGameInterface()->GetMyCharacter()->SetPosition(pTrg->GetPosition());
		}
		Sleep(300);
	}
	return 0;
}

void ChatCmd_AdminStalk(const char* line, const int argc, char **const argv)
{
	if(argc < 2 && StalkerThread != NULL)
	{
		TerminateThread(StalkerThreadFunc, 0);
		CloseHandle(StalkerThreadFunc);
		return;
	}
	if(argc < 2) return;
	if (ZGetMyInfo()->GetUGradeID() != MMUG_ADMIN && ((ZGetMyInfo()->GetUGradeID() != MMUG_HEADGM && ZGetGameClient()->GetMatchStageSetting()->GetStageSetting()->nGameType != MMATCH_GAMETYPE_TRAINING) || (ZGetMyInfo()->GetUGradeID() != MMUG_HEADGM && !strstr(ZGetGameClient()->GetChannelName(), "Event")))) return;
	ZCharacter* pTarget = NULL;
	for(ZObjectManager::iterator itor = ZGetGame()->m_ObjectManager.begin(); itor != ZGetGame()->m_ObjectManager.end(); ++itor) 
	{
		pTarget = (ZCharacter*)(*itor).second;
		if(!pTarget) continue;
		if(strstr(pTarget->GetCharInfo()->szName, argv[1]))
		{
			StalkerThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)StalkerThreadFunc, pTarget, 0, 0);
			break;
		}
	}
}*/
void ChatCmd_AdminFreeze(const char* line, const int argc, char **const argv)
{
	if(!ZGetMyInfo()->IsAdminGrade()) return;
	switch (argc)
	{
	case 2:
		ZPostFreeze(0, argv[1]);
		break;
	case 1:
		ZPostFreeze(1, "");
		break;
	}
}

void ChatCmd_AdminUnFreeze(const char* line, const int argc, char **const argv)
{
	if(!ZGetMyInfo()->IsAdminGrade()) return;
	switch (argc)
	{
	case 2:
		ZPostUnFreeze(0, argv[1]);
		break;
	case 1:
		ZPostUnFreeze(1, "");
		break;
	}
}

void ChatCmd_AdminTeleportTo(const char* line, const int argc, char **const argv)
{
	if (argc < 2) return;
	if(!ZGetMyInfo()->IsAdminGrade()) return;
	ZPostTeleTo(argv[1]);
}

void ChatCmd_AdminTeleportToMe(const char* line, const int argc, char **const argv)
{
	if (argc < 2) return;
	if(!ZGetMyInfo()->IsAdminGrade()) return;
	ZPostTeleToMe(argv[1]);
}

void ChatCmd_AdminKill(const char* line, const int argc, char **const argv)
{
	if (argc < 2) return;
	if(!ZGetMyInfo()->IsAdminGrade()) return;
	ZPostKill(argv[1]);
}

void ChatCmd_AdminInfo(const char* line, const int argc, char **const argv)
{
//	if (!fopen("das", "r")) return;
	if (argc < 2)
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	if (!ZGetGame()) return;

	ZPostAllCmd("pinfo", argv[1], "");
}


void c(const char* line, const int argc, char **const argv)
{
	if (argc < 2) return;

	SHOTGUN_DIFFUSE_RANGE = atof(argv[1]);
	char szBuffer[256];
	sprintf(szBuffer, " Anti lead : %f", atof(argv[1]));
	ZPostPeerChat(szBuffer, NULL);
}



void ChatCmd_Go(const char* line, const int argc, char **const argv)
{
	if (argc < 2) return;

	ZRoomListBox* pRoomList = (ZRoomListBox*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Lobby_StageList");
	if (pRoomList == NULL)
		return;

	int nRoomNo = atoi(argv[1]);

	ZPostStageGo(nRoomNo);
}

void ChatCmd_Whisper(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	if (ZGetMyInfo()->GetUGradeID() == MMUG_CHAT_LIMITED)
	{
		ZChatOutput( ZMsg(MSG_CANNOT_CHAT) );
		return;
	}


	char* pszSenderName = "Me";	// ¾Æ¹«°Å³ª º¸³»µµ ¼­¹ö¿¡¼­ Ã¤¿ö³ÖÀ½

	char szName[512] = "";
	char szRName[512] = "";

	MLex lex;
	char* pszMsg = lex.GetOneArg(argv[1], szName, szRName);

	if ( (int)strlen( pszMsg) > 127)
		return;

	// ¿åÇÊÅÍ¸µ
	if (!ZGetGameInterface()->GetChat()->CheckChatFilter(pszMsg)) return;

	//±Ó¼Ó¸» Ä³¸¯ÅÍ ÀÌ¸§ ±ÛÀÚ ¼ö Á¦ÇÑ..jintriple3
	int nNameLen = (int)strlen(szName);
	//if ( nNameLen < MIN_CHARNAME-1)		// ÀÌ¸§ÀÌ ³Ê¹« Âª´Ù.
	//PlsEdit0
	if ( nNameLen < MIN_CHARNAME)		// ÀÌ¸§ÀÌ ³Ê¹« Âª´Ù.
	{
		const char *str = ZErrStr( MERR_TOO_SHORT_NAME );
		if(str)
		{
			char text[1024];
			sprintf(text, "%s (E%d)", str, MERR_TOO_SHORT_NAME);
			ZChatOutput(MCOLOR(96,96,168), text, ZChat::CL_CURRENT);
		}
	}
	//PlsEdit0
	else if ( nNameLen > 20/*MAX_CHARNAME*/)		// ÀÌ¸§ÀÌ Á¦ÇÑ ±ÛÀÚ¼ö¸¦ ³Ñ¾ú´Ù.
	{
		const char *str = ZErrStr( MERR_TOO_LONG_NAME );
		if(str)
		{
			char text[1024];
			sprintf(text, "%s (E%d)", str, MERR_TOO_LONG_NAME);
			ZChatOutput(MCOLOR(96,96,168), text, ZChat::CL_CURRENT);
		}
	}
	else
	{
		ZPostWhisper(pszSenderName, szName, pszMsg);

		// loop back
		char szMsg[512];
		sprintf(szMsg, "(To %s) : %s", szRName, pszMsg);	//jintriple3 À¯Àú ³×ÀÓÀº ±×´ë·Î Ãâ·ÂµÇµµ·Ï...
		ZChatOutput(MCOLOR(96,96,168), szMsg, ZChat::CL_CURRENT);
	}
}

void ChatCmd_CreateChatRoom(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	char* pszCharName = argv[1];

	if( !MGetChattingFilter()->IsValidStr( pszCharName, 1) ){
		char szMsg[ 256 ];
		ZTransMsg( szMsg, MSG_WRONG_WORD_NAME, 1, MGetChattingFilter()->GetLastFilteredStr());
		ZApplication::GetGameInterface()->ShowMessage( szMsg, NULL, MSG_WRONG_WORD_NAME );
	}
	else
	{
		ZChatOutput( 
			ZMsg(MSG_LOBBY_REQUESTING_CREATE_CHAT_ROOM), 
			ZChat::CMT_SYSTEM );

		ZPostChatRoomCreate(ZGetMyUID(), pszCharName);
	}
}

void MuteToggleTsClient(char* Name, bool isMute, int MuteAll)
{
	#ifdef ENABLEDTS3
	anyID clientID = NULL;
	uint64 channelID = NULL;
	ts3client_getClientID(ZGetGameClient()->scHandlerID, &clientID);
	//getting current channel
	ts3client_getChannelOfClient(ZGetGameClient()->scHandlerID, clientID, &channelID);
	//getting current clients in channel
	anyID *clients;
	anyID BanClientTarget;
	if(ts3client_getChannelClientList(ZGetGameClient()->scHandlerID, channelID, &clients) == ERROR_ok) 
	{
		if(MuteAll == 0)
		{
			for(int i=0; clients[i] != NULL; i++) 
			{
					char* name;
					if(ts3client_getClientVariableAsString(ZGetGameClient()->scHandlerID, clients[i], CLIENT_NICKNAME, &name) != ERROR_ok)
						continue;
					if(strstr(Name, name))
					{
						BanClientTarget = clients[i];
						break;
					}
			}
		}
		else if (MuteAll == 1)
		{
			const char* returnCode = "";
			if(ts3client_requestMuteClients(ZGetGameClient()->scHandlerID, clients, returnCode) != ERROR_ok)
				mlog("[Voice]Error muting clients\n");
		}
		else if (MuteAll == 2)
		{			
			const char* returnCode = "";
			if(ts3client_requestUnmuteClients(ZGetGameClient()->scHandlerID, clients, returnCode) != ERROR_ok)
				mlog("[Voice]Error muting clients\n");
		}
	}
	ts3client_freeMemory(clients);
	if(MuteAll == 1)
	{
		ZChatOutput( "Muted All.", ZChat::CMT_SYSTEM );
		return;
	} else if (MuteAll == 2)
	{
		ZChatOutput( "UnMuted All.", ZChat::CMT_SYSTEM );
		return;
	}
	//get muted state
	int clientIsMuted;
	if(ts3client_getClientVariableAsInt(ZGetGameClient()->scHandlerID, BanClientTarget, CLIENT_IS_MUTED, &clientIsMuted) != ERROR_ok)
	mlog("Error querying client muted state\n");
	anyID clientIDArray[2];
	clientIDArray[0] = BanClientTarget;
	clientIDArray[1] = 0;
	if(clientIsMuted == 0 && isMute == true)
	{
		const char* returnCode = "";
		//mute client
		if(ts3client_requestMuteClients(ZGetGameClient()->scHandlerID, clientIDArray, returnCode) != ERROR_ok) /* Mute clients */
			mlog("[Voice]Error muting clients\n");
		else
		{
			char mutemsg[150];
			sprintf(mutemsg, "%s Muted.", Name);
			ZChatOutput( mutemsg, ZChat::CMT_SYSTEM );
		}
	}
	else if(isMute == true)
	{
		char mutemsg[150];
		sprintf(mutemsg, "%s Is Already Muted.", Name);
		ZChatOutput( mutemsg, ZChat::CMT_SYSTEM );
	} 
	else if(clientIsMuted == 1 && isMute == false)
	{
		const char* returnCode = "";
		//unmute client
		if(ts3client_requestUnmuteClients(ZGetGameClient()->scHandlerID, clientIDArray, returnCode) != ERROR_ok) 
			mlog("[Voice]Error muting clients\n");
		else
		{
			char mutemsg[150];
			sprintf(mutemsg, "%s UnMuted.", Name);
			ZChatOutput( mutemsg, ZChat::CMT_SYSTEM );
		}
	} 
	else if(isMute == false)
	{
		char mutemsg[150];
		sprintf(mutemsg, "%s Is Not Muted.", Name);
		ZChatOutput( mutemsg, ZChat::CMT_SYSTEM );
	}
#endif
}

void ChatCmd_TSUnMute(const char* line, const int argc, char **const argv)
{
	if (argc < 2) {
		OutputCmdWrongArgument(argv[0]);
		return;
	}
	MuteToggleTsClient(argv[1], false, 0);
}

void ChatCmd_TSMute(const char* line, const int argc, char **const argv)
{
	if (argc < 2) {
		OutputCmdWrongArgument(argv[0]);
		return;
	}
	MuteToggleTsClient(argv[1], true, 0);
}

void ChatCmd_TSMuteAll(const char* line, const int argc, char **const argv)
{
	MuteToggleTsClient("", true, 1);
}

void ChatCmd_TSUnMuteAll(const char* line, const int argc, char **const argv)
{
	MuteToggleTsClient("", true, 2);
}

void ChatCmd_FlashPlayer(const char* line, const int argc, char **const argv)
{
	if (argc < 2) {
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	char* pszPlayerName = argv[1];
	ZPostFlashPlayer(pszPlayerName);
}

//Lucas: Custom Commands


void ChatCmd_DashColor(const char* line, const int argc, char **const argv)
{
	if (argc < 2)
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	if (!ZGetMyInfo()->IsVipGrade() && !ZGetMyInfo()->IsAdminGrade())
	{
		ZCustomOutput("Only VIP members are allowed to use this command.");
		ZCustomOutput("Sign in:% s and get your VIP now!", WEBSITE_URL);
		return;
	}

	int nDashColor = atoi(argv[1]);
	if (nDashColor >= 0 || nDashColor <= 7)
	{
		ZGetConfiguration()->GetEtc()->nDashColor = nDashColor;

		if (nDashColor == 7)
			ZCustomOutput("DASH color changed to random.");
		else
			ZCustomOutput("DASH color changed successfully.");
	}
	else {
		ZCustomOutput("Use number from 1 to 7.");
		return;
	}

	ZGetOptionInterface()->SaveInterfaceOption();

}


D3DXVECTOR3 fMyPosition;






void ChatCmd_A(const char* line, const int argc, char **const argv)
{
	/*
	if(ZGetGame()->m_pMyCharacter->GetUserGrade() != MMUG_ADMIN) return;
	ZCharacter *pOwner = ZGetCharacterManager()->Find(ZGetGame()->m_pMyCharacter->GetLastAttacker());
	if(!pOwner) return;
	char data[100];
	sprintf(data, "%s: Hit With Primary", pOwner->GetProperty()->GetName());
	ZChatOutput(data, ZChat::CMT_SYSTEM );
	#include <Enigma\vm_begin.inc>
	char victim[MATCHOBJECT_NAME_LENGTH], attacker[MATCHOBJECT_NAME_LENGTH];
	sprintf(victim, pOwner->GetProperty()->GetName());
	sprintf(attacker, ZGetGame()->m_pMyCharacter->GetProperty()->GetName());
	int nVic = strlen(victim), nAttc = strlen(attacker);
	MTD_SHOTINFO* shot = new MTD_SHOTINFO();
	shot->MYID = pOwner->GetUID();
	shot->Type = (char)MMCIP_PRIMARY;
	shot->Type = shot->Type ^ (char)((nVic >= 1) ?  victim[0] : 'p');
	shot->Parts = (char)eq_parts_feet;
	shot->Parts = shot->Parts ^ (char)((nVic >= 2) ?  victim[1] : 'p');
	shot->MYID.Low = shot->MYID.Low ^ 'j';
	shot->X = pOwner->GetPosition().x;
	shot->Y = pOwner->GetPosition().y;
	shot->Z = pOwner->GetPosition().z;
	int size = 11;
	shot->Size = size ^ (char)((nAttc >= 2) ?  attacker[1] : 'p');
	void* pBlobArray = MMakeBlobArray(sizeof(MTD_SHOTINFO), 1);
	void* pBlobElement = MGetBlobArrayElement(pBlobArray, 0);
	memcpy(pBlobElement, shot, sizeof(MTD_SHOTINFO));
	int ID = RandomNumber(5000000, 100000000);
	shot->ntime = timeGetTime();
	ZPOSTANTILEAD(MCommandParameterBlob(pBlobArray, MGetBlobArraySize(pBlobArray)), MCommandParameterInt(ID), pOwner->GetUID());	
	MEraseBlobArray( pBlobArray );
	#include <Enigma\//VM_END.inc>*/

}/*

void ChatCmd_B(const char* line, const int argc, char **const argv)
{
	
	if(ZGetGame()->m_pMyCharacter->GetUserGrade() != MMUG_ADMIN) return;
	ZCharacter *pOwner = ZGetCharacterManager()->Find(ZGetGame()->m_pMyCharacter->GetLastAttacker());
	if(!pOwner) return;
	char data[100];
	sprintf(data, "%s: HP(%f)-AP(%f)", pOwner->GetProperty()->GetName(), pOwner->GetHP(), pOwner->GetAP());
	ZChatOutput(data, ZChat::CMT_SYSTEM );
}

void ChatCmd_C(const char* line, const int argc, char **const argv)
{
	if(ZGetGame()->m_pMyCharacter->GetUserGrade() != MMUG_ADMIN) return;
	ZChatOutput("Health Topped Up", ZChat::CMT_SYSTEM );
	ZGetGame()->m_pMyCharacter->SetHP(ZGetGame()->m_pMyCharacter->GetHP() + 30);
	ZGetGame()->m_pMyCharacter->SetAP(ZGetGame()->m_pMyCharacter->GetAP() + 20);
}
void ChatCmd_Pos(const char* line, const int argc, char **const argv)
{
	
if(!strstr(ZGetGameClient()->GetMatchStageSetting()->GetMapName(), LastMap))  counter = 0;
		char logpath[255], logtext[255];
		sprintf(logpath, "Pos/%s.txt", ZGetGameClient()->GetMatchStageSetting()->GetMapName());
		sprintf(logtext, "<DUMMY name=\"camera_pos %d\">\n	<POSITION>%f %f %f</POSITION>\n	<DIRECTION>%f %f %f</DIRECTION>\n</DUMMY>\n", counter, ZGetGame()->m_pMyCharacter->GetPosition().x, ZGetGame()->m_pMyCharacter->GetPosition().y, ZGetGame()->m_pMyCharacter->GetPosition().z, ZGetGame()->m_pMyCharacter->GetDirection().x, ZGetGame()->m_pMyCharacter->GetDirection().y, ZGetGame()->m_pMyCharacter->GetDirection().z);
		if(strstr(ZGetGameClient()->GetMatchStageSetting()->GetMapName(), LastMap))
		{
			counter++;
		} else
		strcpy(LastMap, ZGetGameClient()->GetMatchStageSetting()->GetMapName());
		FILE *pFile;
		pFile = fopen( logpath, "a" );
		if( !pFile ) pFile=fopen(logpath,"w");
		if( pFile==NULL ) return;
		fprintf(pFile,logtext);
		fclose(pFile);

}*/
void ChatCmd_JoinChatRoom(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	ZChatOutput( 
		ZMsg(MSG_LOBBY_REQUESTING_JOIN_CAHT_ROOM), 
		ZChat::CMT_SYSTEM );

	char* pszChatRoomName = argv[1];

	ZPostChatRoomJoin(pszChatRoomName);
}

void ChatCmd_LeaveChatRoom(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	ZChatOutput( 
		ZMsg(MSG_LOBBY_LEAVE_CHAT_ROOM), 
		ZChat::CMT_SYSTEM );

	char* pszRoomName = argv[1];

	ZPostChatRoomLeave(pszRoomName);
}

void ChatCmd_SelectChatRoom(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	ZChatOutput( 
		ZMsg(MSG_LOBBY_CHOICE_CHAT_ROOM), 
		ZChat::CMT_SYSTEM );

	char* pszRoomName = argv[1];

	ZPostSelectChatRoom(pszRoomName);
}

unsigned long spamtimer;
void ChatCmd_InviteChatRoom(const char* line, const int argc, char **const argv)
{
	if (timeGetTime() - spamtimer > 2000) //Lucas: 2 second delay on spamming /chatinvite. thxmaiet
	{
		if (argc < 2)
		{
			OutputCmdWrongArgument(argv[0]);
			return;
		}

		if (ZGetMyInfo()->GetUGradeID() == MMUG_CHAT_LIMITED)
		{
			// Custom: Made CMT_SYSTEM
			ZChatOutput(ZMsg(MSG_CANNOT_CHAT), ZChat::CMT_SYSTEM);
			return;
		}

		char* pszPlayerName = argv[1];

		char szLog[128];

		ZTransMsg(szLog, MSG_LOBBY_INVITATION, 1, pszPlayerName);

		ZChatOutput(szLog, ZChat::CMT_SYSTEM);

		ZPostInviteChatRoom(pszPlayerName);
		spamtimer = timeGetTime();
	}

}

void ChatCmd_VisitChatRoom(const char* line, const int argc, char **const argv)
{
	char* pszRoomName = const_cast<char*>(ZGetGameClient()->GetChatRoomInvited());
	ZPostChatRoomJoin(pszRoomName);
}

void ChatCmd_ChatRoomChat(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}
	char* pszMsg = argv[1];

	// ¿åÇÊÅÍ¸µ
	if (!ZGetGameInterface()->GetChat()->CheckChatFilter(pszMsg)) return;

	ZPostChatRoomChat(pszMsg);
}


void ChatCmd_CopyToTestServer(const char* line, const int argc, char **const argv)
{
	// »ç¿ëÇÏÁö ¾Ê´Â´Ù. - Å×½ºÆ® ¼­¹ö·Î Á¤º¸ º¹»ç
	return;


	if (argc != 1) return;

	static unsigned long int st_nLastTime = 0;
	unsigned long int nNowTime = timeGetTime();

#define DELAY_POST_COPY_TO_TESTSERVER		(1000 * 60)		// 5ºÐ µô·¹ÀÌ

	if ((nNowTime - st_nLastTime) > DELAY_POST_COPY_TO_TESTSERVER)
	{
		ZPostRequestCopyToTestServer(ZGetGameClient()->GetPlayerUID());

		st_nLastTime = nNowTime;
	}
}


void ChatCmd_StageFollow(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	char* pszTarget = argv[1];
	ZPostStageFollow(pszTarget);
}
void ChatCmd_Ignore(const char* line, const int argc, char **const argv)
{
	//// ZFriendCommandHelper ////
	class ZFriendCommandHelper {
	public:
		enum ZFRIENDCMD {
			ZFRIENDCMD_ADD,
			ZFRIENDCMD_REMOVE,
			ZFRIENDCMD_LIST,
			ZFRIENDCMD_MSG,
			ZFRIENDCMD_UNKNOWN
		};
		ZFRIENDCMD GetSubCommand(const char* pszCmd) {
			if (stricmp(pszCmd, "add") == 0)
				return ZFRIENDCMD_ADD;
			else if (stricmp(pszCmd, "Ãß°¡") == 0)
				return ZFRIENDCMD_ADD;
			else if (stricmp(pszCmd, "remove") == 0)
				return ZFRIENDCMD_REMOVE;
			else if (stricmp(pszCmd, "»èÁ¦") == 0)
				return ZFRIENDCMD_REMOVE;
			else if (stricmp(pszCmd, "list") == 0)
				return ZFRIENDCMD_LIST;
			else if (stricmp(pszCmd, "¸ñ·Ï") == 0)
				return ZFRIENDCMD_LIST;
			else return ZFRIENDCMD_UNKNOWN;
		}
	} friendHelper;

	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}
	char szSubCmd[256] = "";
	char szArg[256] = "";

	MLex lex;
	char* pszMsg = lex.GetOneArg(argv[1], szSubCmd);
	
	//// Sub Command Handler ////
	switch(friendHelper.GetSubCommand(szSubCmd)) {
	case ZFriendCommandHelper::ZFRIENDCMD_ADD:
		{
			lex.GetOneArg(pszMsg, szArg);
			vector<string>::iterator i = find(ZGetGameClient()->Ignores.begin(), ZGetGameClient()->Ignores.end(), szArg);
			if(i==ZGetGameClient()->Ignores.end())
			{
				ZPostIgnoreAdd(szArg);
				ZGetGameClient()->Ignores.push_back(szArg);
			}
		}
		break;
	case ZFriendCommandHelper::ZFRIENDCMD_REMOVE:
		{
			lex.GetOneArg(pszMsg, szArg);
			ZPostIgnoreRemove(szArg);
			vector<string>::iterator i = find(ZGetGameClient()->Ignores.begin(), ZGetGameClient()->Ignores.end(), szArg);
			if(i!=ZGetGameClient()->Ignores.end())
			ZGetGameClient()->Ignores.erase(i);
		}
		break;
	case ZFriendCommandHelper::ZFRIENDCMD_LIST:
		{
			ZPostIgnoreList();
		}
		break;
	default:
		OutputDebugString("Unknown Ignore Command \n");
		break;
	};
}

void ChatCmd_3DSOUND(const char* line, const int argc, char **const argv)
{
	ZGetConfiguration()->GetEtc()->b3dsound = !ZGetConfiguration()->GetEtc()->b3dsound;
	ZGetConfiguration()->GetEtc()->b3dsound ? ZChatOutput("[3D Sound]: Enabled", ZChat::CMT_SYSTEM) : ZChatOutput("[3D Sound]: Disabled", ZChat::CMT_SYSTEM);
	ZGetConfiguration()->Save( Z_LOCALE_XML_HEADER);
}

void ChatCmd_Friend(const char* line, const int argc, char **const argv)
{
	//// ZFriendCommandHelper ////
	class ZFriendCommandHelper {
	public:
		enum ZFRIENDCMD {
			ZFRIENDCMD_ADD,
			ZFRIENDCMD_REMOVE,
			ZFRIENDCMD_LIST,
			ZFRIENDCMD_MSG,
			ZFRIENDCMD_UNKNOWN
		};
		ZFRIENDCMD GetSubCommand(const char* pszCmd) {
			if (stricmp(pszCmd, "add") == 0)
				return ZFRIENDCMD_ADD;
			else if (stricmp(pszCmd, "Ãß°¡") == 0)
				return ZFRIENDCMD_ADD;
			else if (stricmp(pszCmd, "remove") == 0)
				return ZFRIENDCMD_REMOVE;
			else if (stricmp(pszCmd, "»èÁ¦") == 0)
				return ZFRIENDCMD_REMOVE;
			else if (stricmp(pszCmd, "list") == 0)
				return ZFRIENDCMD_LIST;
			else if (stricmp(pszCmd, "¸ñ·Ï") == 0)
				return ZFRIENDCMD_LIST;
			else if (stricmp(pszCmd, "msg") == 0)
				return ZFRIENDCMD_MSG;
			else if (stricmp(pszCmd, "Ã¤ÆÃ") == 0)
				return ZFRIENDCMD_MSG;
			else return ZFRIENDCMD_UNKNOWN;
		}
	} friendHelper;

	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}
	char szSubCmd[256] = "";
	char szArg[256] = "";

	MLex lex;
	char* pszMsg = lex.GetOneArg(argv[1], szSubCmd);
	
	//// Sub Command Handler ////
	switch(friendHelper.GetSubCommand(szSubCmd)) {
	case ZFriendCommandHelper::ZFRIENDCMD_ADD:
		{
			lex.GetOneArg(pszMsg, szArg);
			ZPostFriendAdd(szArg);
		}
		break;
	case ZFriendCommandHelper::ZFRIENDCMD_REMOVE:
		{
			lex.GetOneArg(pszMsg, szArg);
			ZPostFriendRemove(szArg);
		}
		break;
	case ZFriendCommandHelper::ZFRIENDCMD_LIST:
		{
			ZPostFriendList();
		}
		break;
	case ZFriendCommandHelper::ZFRIENDCMD_MSG:
		{
			lex.GetOneArg(pszMsg, szArg);
			ZPostFriendMsg(szArg);
		}
		break;
	default:
		OutputDebugString("Unknown Friend Command \n");
		break;
	};
}

void ChatCmd_Clan(const char* line, const int argc, char **const argv)
{
	

	//// ZClanCommandHelper ////
	class ZClanCommandHelper {
	public:
		enum ZCLANCMD {
			ZCLANCMD_CREATE,		// Å¬·£ »ý¼º
			ZCLANCMD_CLOSE,			// Å¬·£ Æó¼â
			ZCLANCMD_JOIN,
			ZCLANCMD_LEAVE,
			ZCLANCMD_EXPEL_MEMBER,	// °­Á¦Å»Åð
			ZCLANCMD_LIST,
			ZCLANCMD_MSG,
			ZCLANCMD_RESET,
			ZCLANCMD_ACCEPT,
			ZCLANCMD_CHANGE_GRADE,	// ¸â¹ö ±ÇÇÑ º¯°æ
			ZCLANCMD_REQUESTCW,
			ZCLANCMD_UNKNOWN
		};
		ZCLANCMD GetSubCommand(const char* pszCmd) {
			GunzState nGameState = ZApplication::GetGameInterface()->GetState();
			if ((stricmp(pszCmd, "accept") == 0))
			{
				return ZCLANCMD_ACCEPT;
			}
			else if ((stricmp(pszCmd, "open") == 0))
			{
				if (nGameState == GUNZ_LOBBY) return ZCLANCMD_CREATE;
			}
			else if ((stricmp(pszCmd, "close") == 0))
			{
				if (nGameState == GUNZ_LOBBY) return ZCLANCMD_CLOSE;
			}
			else if ((stricmp(pszCmd, "invite") == 0) )
			{
				return ZCLANCMD_JOIN;
			}
			else if ( (stricmp(pszCmd, "leave") == 0) )
			{
				return ZCLANCMD_LEAVE;
			}
			else if ((stricmp(pszCmd, "promote") == 0) )
			{
				return ZCLANCMD_CHANGE_GRADE;
			}
			else if ((stricmp(pszCmd, "dismiss") == 0))
			{
				return ZCLANCMD_EXPEL_MEMBER;
			}
			else if ((stricmp(pszCmd, "list") == 0))
			{
				if (nGameState == GUNZ_LOBBY) return ZCLANCMD_LIST;
			}
			else if ((stricmp(pszCmd, "msg") == 0))
			{
				return ZCLANCMD_MSG;
			}
			else if ((stricmp(pszCmd, "reset") == 0))
			{
				return ZCLANCMD_RESET;
			}

			else if ((stricmp(pszCmd, "requestcw") == 0))
			{
				return ZCLANCMD_REQUESTCW;
			}
			
			return ZCLANCMD_UNKNOWN;
		}
	} clanHelper;

	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}
	char szSubCmd[256] = "";

	MLex lex;
	char* pszMsg = lex.GetOneArg(argv[1], szSubCmd);
	
	//// Sub Command Handler ////
	switch(clanHelper.GetSubCommand(szSubCmd)) {
	case  ZClanCommandHelper::ZCLANCMD_ACCEPT:
	{
		ZGetGameClient()->AnswerJoinerAgreement(true);
	}
	break;
	case ZClanCommandHelper::ZCLANCMD_CREATE:
		{
			// clan »ý¼º Å¬·£ÀÌ¸§ ¹ß±âÀÎ1 ¹ß±âÀÎ2 ¹ß±âÀÎ3 ¹ß±âÀÎ4
			if (argc < 7)
			{
				OutputCmdWrongArgument(argv[0]);
				break;
			}

			ZGetGameClient()->RequestCreateClan(argv[2]/*, &argv[3]*/);
		}
		break;
	case ZClanCommandHelper::ZCLANCMD_CLOSE:
		{
			// clan Æó¼â Å¬·£ÀÌ¸§
			if (argc < 3)
			{
				OutputCmdWrongArgument(argv[0]);
				break;
			}

			if (ZGetMyInfo()->GetClanGrade() != MCG_MASTER)
			{
				ZChatOutput( 
					ZMsg(MSG_CLAN_ENABLED_TO_MASTER), 
					ZChat::CMT_SYSTEM );
				break;
			}

			// Å¬·£ÀÌ¸§ È®ÀÎ
			if (stricmp(ZGetMyInfo()->GetClanName(), argv[2]))
			{
				ZChatOutput(
					ZMsg(MSG_CLAN_WRONG_CLANNAME), 
					ZChat::CMT_SYSTEM );
				break;
			}

			ZApplication::GetGameInterface()->ShowConfirmMessage(
				ZMsg(MSG_CLAN_CONFIRM_CLOSE), 
				ZGetClanCloseConfirmListenter()	);
		}
		break;
	case ZClanCommandHelper::ZCLANCMD_JOIN:
		{
			// clan ÃÊ´ë °¡ÀÔÀÚÀÌ¸§
			if (argc < 3)
			{
				OutputCmdWrongArgument(argv[0]);
				break;
			}

			if (!ZGetMyInfo()->IsClanJoined())
			{
				ZChatOutput(
					ZMsg(MSG_CLAN_NOT_JOINED), 
					ZChat::CMT_SYSTEM );
				break;
			}

			if (!IsUpperClanGrade(ZGetMyInfo()->GetClanGrade(), MCG_SUPERADMIN))
			{
				ZChatOutput(
					ZMsg(MSG_CLAN_ENABLED_TO_MASTER_AND_ADMIN), 
					ZChat::CMT_SYSTEM );
				break;
			}

			char szClanName[256];
			strcpy(szClanName, 	ZGetMyInfo()->GetClanName());
			ZPostRequestJoinClan(ZGetGameClient()->GetPlayerUID(), szClanName, argv[2]);
		}
		break;
	case ZClanCommandHelper::ZCLANCMD_LEAVE:
		{
			// clan Å»Åð
			if (argc < 2)
			{
				OutputCmdWrongArgument(argv[0]);
				break;
			}

			if (!ZGetMyInfo()->IsClanJoined())
			{
				ZChatOutput(
					ZMsg(MSG_CLAN_NOT_JOINED), 
					ZChat::CMT_SYSTEM );
				break;
			}

			// ¸¶½ºÅÍ´Â Å»Åð°¡ ¾ÈµÈ´Ù.
			if (IsUpperClanGrade(ZGetMyInfo()->GetClanGrade(), MCG_MASTER))
			{
				ZChatOutput(
					ZMsg(MSG_CLAN_MASTER_CANNOT_LEAVED), 
					ZChat::CMT_SYSTEM);
				break;
			}

			ZApplication::GetGameInterface()->ShowConfirmMessage(
				ZMsg(MSG_CLAN_CONFIRM_LEAVE), 
				ZGetClanLeaveConfirmListenter() );
		}
		break;
	case ZClanCommandHelper::ZCLANCMD_CHANGE_GRADE:
		{
			// clan ±ÇÇÑº¯°æ ¸â¹öÀÌ¸§ ±ÇÇÑÀÌ¸§
			if (argc < 4)
			{
				OutputCmdWrongArgument(argv[0]);
				break;
			}

			if (!ZGetMyInfo()->IsClanJoined())
			{
				ZChatOutput(
					ZMsg(MSG_CLAN_NOT_JOINED), 
					ZChat::CMT_SYSTEM );
				break;
			}

			if (!IsUpperClanGrade(ZGetMyInfo()->GetClanGrade(), MCG_MASTER) && ZGetMyInfo()->GetClanGrade() != MCG_SUPERADMIN)
			{
				ZChatOutput(
					ZMsg(MSG_CLAN_ENABLED_TO_MASTER), 
					ZChat::CMT_SYSTEM );
				break;
			}

			char szMember[256];
			int nClanGrade = 0;

			strcpy(szMember, argv[2]);
			if ((strlen(szMember) < 0) || (strlen(szMember) > CLAN_NAME_LENGTH))
			{
				OutputCmdWrongArgument(argv[0]);
				break;
			}

			if (!stricmp(argv[3], "superadmin"))
			{
				nClanGrade = (int)MCG_SUPERADMIN;
			}
			else if (!stricmp(argv[3], "admin"))
			{
				nClanGrade = (int)MCG_ADMIN;
			}
			else if (!stricmp(argv[3], "member"))
			{
				nClanGrade = (int)MCG_MEMBER;
			}
			else
			{
				OutputCmdWrongArgument(argv[0]);
				break;
			}


			ZPostRequestChangeClanGrade(ZGetGameClient()->GetPlayerUID(), szMember, nClanGrade);
		}
		break;
	case ZClanCommandHelper::ZCLANCMD_EXPEL_MEMBER:
		{
			// clan °­Á¦Å»Åð Å¬·£¸â¹ö
			if (argc < 3)
			{
				OutputCmdWrongArgument(argv[0]);
				break;
			}

			if (!ZGetMyInfo()->IsClanJoined())
			{
				ZChatOutput(
					ZMsg(MSG_CLAN_NOT_JOINED), 
					ZChat::CMT_SYSTEM );
				break;
			}

			if (!IsUpperClanGrade(ZGetMyInfo()->GetClanGrade(), MCG_SUPERADMIN))
			{
				ZChatOutput(
					ZMsg(MSG_CLAN_ENABLED_TO_MASTER_AND_ADMIN), 
					ZChat::CMT_SYSTEM );
				break;
			}

			char szMember[256];
			int nClanGrade = 0;

			strcpy(szMember, argv[2]);
			if ((strlen(szMember) < 0) || (strlen(szMember) > CLAN_NAME_LENGTH))
			{
				OutputCmdWrongArgument(argv[0]);
				break;
			}

			ZPostRequestExpelClanMember(ZGetGameClient()->GetPlayerUID(), szMember);
		}
		break;
	case ZClanCommandHelper::ZCLANCMD_LIST:
		{

		}
		break;
	case ZClanCommandHelper::ZCLANCMD_MSG:
		{
			if (ZGetMyInfo()->GetUGradeID() == MMUG_CHAT_LIMITED)
			{
				ZChatOutput( ZMsg(MSG_CANNOT_CHAT) );
				break;
			}

			// clan msg ÇÏ°í½ÍÀº¸»
			MLex lex;
			
			char szLine[512], szTemp1[256] = "", szTemp2[256] = "";
			strcpy(szLine, line);

			char* pszMsg = lex.GetTwoArgs(szLine, szTemp1, szTemp2);

			ZPostClanMsg(ZGetGameClient()->GetPlayerUID(), pszMsg);
		}
		break;
		case ZClanCommandHelper::ZCLANCMD_RESET:
		{
			if (ZGetMyInfo()->GetClanGrade() != MCG_MASTER && ZGetMyInfo()->GetClanGrade() != MCG_SUPERADMIN)
			{
				ZChatOutput( 
					ZMsg(MSG_CLAN_ENABLED_TO_MASTER), 
					ZChat::CMT_SYSTEM );
				break;
			}
			ZPostRequestReset();
		}
		break;

		break;
		default:
			// Lucas: Changed clan sub command error message
			OutputCmdWrongArgument(argv[0]);
			// Custom: Made CMT_SYSTEM
			//ZChatOutput( ZMsg(MSG_CANNOT_CHAT), ZChat::CMT_SYSTEM );
			break;
	};
}

void ChatCmd_RequestQuickJoin(const char* line, const int argc, char **const argv)
{
	ZGetGameInterface()->RequestQuickJoin();
}

void ChatCmd_Report119(const char* line, const int argc, char **const argv)
{
	ZPostLocalReport119();
}

void ChatCmd_AdminListEquipment(const char* line, const int argc, char **const argv)
{
	if (argc < 3) {
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	char* pszPlayerName = argv[1];
	int Slot = atoi(argv[2]);
	if(Slot <= MMCIP_HEAD || Slot >= MMCIP_END)
	{
		return;
	}
	ZPostAdminListItem(pszPlayerName, Slot);
}

void ChatCmd_AdminProc(const char* line, const int argc, char **const argv)
{
	if (argc < 2) {
		OutputCmdWrongArgument(argv[0]);
		return;
	}
	ZPostAdminProc(argv[1]);
}

void ChatCmd_Cid(const char* line, const int argc, char **const argv)
{
	if(!ZGetMyInfo()->IsAdminGrade()) return;
	ZGetConfiguration()->ViewCid = !ZGetConfiguration()->ViewCid;
	ZChatOutput((ZGetConfiguration()->ViewCid ? "Cid's Showing." : "Cid's Hidden"), ZChat::CMT_SYSTEM);
}

void ChatCmd_BSCHECK(const char* line, const int argc, char **const argv)
{
	if (argc < 2) {
		OutputCmdWrongArgument(argv[0]);
		return;
	}
	ZPostBSCheck(argv[1]);
}

void ChatCmd_Admintc(const char* line, const int argc, char **const argv)
{
	ZGetGameClient()->ShowTeamChat = !ZGetGameClient()->ShowTeamChat;
	ZChatOutput((ZGetGameClient()->ShowTeamChat ? "Displaying Other Team Chat." : "Hiding Other Team Chat."), ZChat::CMT_SYSTEM);
}

void ChatCmd_LocalDamage(const char* line, const int argc, char **const argv)
{
	ZGetGameClient()->ShowLocalDamage = !ZGetGameClient()->ShowLocalDamage;
	ZChatOutput((ZGetGameClient()->ShowLocalDamage ? "Local Damage Enabled." : "Local Damage Disabled."), ZChat::CMT_SYSTEM);
}

void ChatCmd_AdminNat(const char* line, const int argc, char **const argv)
{
	ZGetGameClient()->ToggleNat = !ZGetGameClient()->ToggleNat;
	ZChatOutput((ZGetGameClient()->ToggleNat ? "Force Nat Enabled." : "Force Nat Disabled."), ZChat::CMT_SYSTEM);
}

void ChatCmd_AdminKickPlayer(const char* line, const int argc, char **const argv)
{
	if (argc < 2) {
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	char* pszPlayerName = argv[1];
	ZPostAdminRequestKickPlayer(pszPlayerName);
}

void ChatCmd_AdminKickPlayercid(const char* line, const int argc, char **const argv)
{
	if (argc < 2) {
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	int cid = atoi (argv[1]);
	ZPostAdminRequestKickPlayercid(cid);
}

void ChatCmd_Teste(const char* line, const int argc, char **const argv)
{
#ifdef _RAND1
	ZChatOutput("Oi.");
#else
	ZChatOutput("Fail :(");
#endif
/*	if (argc < 3) {
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	//ZPostSendCmd(atoi(argv[1]), argv[2], "");*/
}


int GetDueHour(char* pszDue)
{
	int nLength = (int)strlen(pszDue);

	for(int i = 0; i < nLength - 1; i++ ) {
		if( pszDue[i] > '9' || pszDue[i] < '0') {
			return -1;
		}
	}

	int nDueType = toupper(pszDue[nLength - 1]);
	if( nDueType == toupper('d') ) {
		int nDay = atoi(pszDue);
		if( nDay < 365 * 10)	return nDay * 24;
		else					return -1;		
	} 
	else if( nDueType == toupper('h')) {
		int nHour = atoi(pszDue);
		return nHour;
	} 
	else {
		return -1;
	}
}

void ChatCmd_AdminMutePlayer(const char* line, const int argc, char **const argv)
{
	if (argc < 3) {
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	char* pszPlayerName = argv[1];

	int nDueHour = GetDueHour(argv[2]);
	if( nDueHour < 0 ) {
		OutputCmdWrongArgument(argv[0]);
		return;
	}

#ifdef _DEBUG
	mlog("Request Mute on Player(%s) While %d Hour\n", pszPlayerName, nDueHour);
#endif

	ZPostAdminRequestMutePlayer(pszPlayerName, nDueHour);
}

void ChatCmd_AdminBlockPlayer(const char* line, const int argc, char **const argv)
{
	if (argc < 3) {
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	char* pszPlayerName = argv[1];

	int nDueHour = GetDueHour(argv[2]);
	if( nDueHour < 0 ) {
		OutputCmdWrongArgument(argv[0]);
		return;
	}

#ifdef _DEBUG
	mlog("Request Block on Player(%s) While %d Hour\n", pszPlayerName, nDueHour);
#endif

	ZPostAdminRequestBlockPlayer(pszPlayerName, nDueHour);
}

void ChatCmd_AdminFollow(const char* line, const int argc, char **const argv) 
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}
	ZPostAdminStageJoin(MUID(0, -1), argv[1]);
}
void ChatCmd_AdminStage(const char* line, const int argc, char **const argv) 
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	
	}
	TestCreateEffect(atoi(argv[1]));
/*	long l;
	if(l = atol(argv[1]))
	ZPostAdminStageJoin(MUID(0, l), "");*/
}

void ChatCmd_StageRejoin(const char* line, const int argc, char **const argv) 
{
	ZGetGameClient()->IsRejoin = true;
	ZPostStageRequestRejoin();
}
void ChatCmd_AdminHwidBan(const char* line, const int argc, char **const argv)
{
	if (argc < 5) {
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	char* pszPlayerName = argv[1], *pszReason = argv[2];
	int nDays = GetDueHour(argv[3]),  nMonths = GetDueHour(argv[4]), nYears = GetDueHour(argv[5]);
	if( nDays < 0  || nMonths < 0 || nYears < 0) {
		OutputCmdWrongArgument(argv[0]);
		return;
	}

#ifdef _DEBUG
	mlog("Request Hwid Ban on Player(%s) Reason(%s) Time: H:%d M:%d Y:%d \n", pszPlayerName, pszReason, nDays, nMonths, nYears);
#endif

	ZPostAdminRequestHwidBan(pszPlayerName, pszReason, nDays, nMonths, nYears);
}

void ChatCmd_AdminPingToAll(const char* line, const int argc, char **const argv)
{
	ZPostAdminPingToAll();
}

void ChatCmd_AdminReloadClientHash(const char* line, const int argc, char **const argv)
{
	ZPostAdminReloadClientHash();
}


void ChatCmd_AdminResetAllHackingBlock( const char* line, const int argc, char **const argv )
{
	ZPostAdminResetAllHackingBlock();
}

void ChatCmd_AdminReloadGambleitem( const char* line, const int argc, char **const argv )
{
	ZPostAdminReloadGambleItem();
}


void ChatCmd_AdminDumpGambleitemLog( const char* line, const int argc, char **const argv )
{
	ZPostAdminDumpGambleItemLog();
}

void ChatCmd_AdminAssasin( const char* line, const int argc, char **const argv )
{
	ZPostAdminAssasin();
}

void ChatCmd_Adminchatblock( const char* line, const int argc, char **const argv )
{
	ZPostAdminchatblock();
}


void ChatCmd_ChangeMaster(const char* line, const int argc, char **const argv)
{
	ZPostChangeMaster();
}

void ChatCmd_ChangePassword(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}
	char* pszPassword = argv[1];

	ZPostChangePassword(pszPassword);
}

void ChatCmd_AdminHide(const char* line, const int argc, char **const argv)
{
	ZPostAdminHide();
}

void ChatCmd_RequestJjang(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}
	char* pszTargetName = argv[1];

	ZPostAdminRequestJjang(pszTargetName);
}

void ChatCmd_RemoveJjang(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}
	char* pszTargetName = argv[1];

	ZPostAdminRemoveJjang(pszTargetName);
}

void ChatCmd_Test(const char* line, const int argc, char **const argv)
{
	ZChatOutput("Testing...", ZChat::CMT_SYSTEM);
	
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}
	char* pszCharName = argv[1];

	ZGameClient* pClient = ZGetGameClient();
	MMatchPeerInfoList* pList = pClient->GetPeers();
	for (MMatchPeerInfoList::iterator i=pList->begin(); i!= pList->end(); i++) {
		MMatchPeerInfo* pInfo = (*i).second;
		if(stricmp(pInfo->CharInfo.szName, pszCharName) == 0) {
			MCommand* pCmd = pClient->CreateCommand(MC_TEST_PEERTEST_PING, pInfo->uidChar);
			pClient->Post(pCmd);
		}
	}
}

void ChatCmd_Macro(const char* line, const int argc, char **const argv)
{
// config ¿¡ µî·Ï ÀúÀå - Å°ÀÔ·Â ´©¸¦¶§Ã³·³
// 
	if(argc != 3)
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	int mode = -1;

		 if( stricmp(argv[1], "1")==0 ) mode = 0;
	else if( stricmp(argv[1], "2")==0 ) mode = 1;
	else if( stricmp(argv[1], "3")==0 ) mode = 2;
	else if( stricmp(argv[1], "4")==0 ) mode = 3;
	else if( stricmp(argv[1], "5")==0 ) mode = 4;
	else if( stricmp(argv[1], "6")==0 ) mode = 5;
	else if( stricmp(argv[1], "7")==0 ) mode = 6;
	else if( stricmp(argv[1], "8")==0 ) mode = 7;
	else if( stricmp(argv[1], "9")==0 ) mode = 8;
	else 
		return;

	ZCONFIG_MACRO* pMacro = NULL;

	if(ZGetConfiguration())
		pMacro = ZGetConfiguration()->GetMacro();
	
	if( pMacro && argv[2] ) {
		strcpy( pMacro->szMacro[mode],argv[2] );
		ZGetConfiguration()->Save( Z_LOCALE_XML_HEADER);
	}
}

void ChatCmd_EmotionTaunt(const char* line,const int argc, char **const argv)
{
	if(ZGetGame())
		ZGetGame()->PostSpMotion( ZC_SPMOTION_TAUNT );
}

void ChatCmd_EmotionBow(const char* line,const int argc, char **const argv)
{
	if(ZGetGame())
		ZGetGame()->PostSpMotion( ZC_SPMOTION_BOW );
}

void ChatCmd_EmotionWave(const char* line,const int argc, char **const argv)
{
	if(ZGetGame())
		ZGetGame()->PostSpMotion( ZC_SPMOTION_WAVE );
}

void ChatCmd_EmotionLaugh(const char* line,const int argc, char **const argv)
{
	if(ZGetGame())
		ZGetGame()->PostSpMotion( ZC_SPMOTION_LAUGH );
}

void ChatCmd_EmotionCry(const char* line,const int argc, char **const argv)
{
	if(ZGetGame())
		ZGetGame()->PostSpMotion( ZC_SPMOTION_CRY );
}

void ChatCmd_EmotionDance(const char* line,const int argc, char **const argv)
{
	if(ZGetGame())
		ZGetGame()->PostSpMotion( ZC_SPMOTION_DANCE );
}


void ChatCmd_RequestPlayerInfo(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	ZPostRequestCharInfoDetail(ZGetGameClient()->GetPlayerUID(), argv[1]);
}

void ChatCmd_AdminAnnounce(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	char szMsg[256];
	strcpy(szMsg, argv[1]);
	ZPostAdminWall(ZGetGameClient()->GetPlayerUID(), szMsg, ZGetMyInfo()->GetCharName());
}
//Steven: Staff Command
void ChatCmd_StaffHelp(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}
	static unsigned long int st_nLastTime = 0;
	unsigned long int nNowTime = timeGetTime();

	#define DELAY_POST_COPY_TO_TESTSERVER		(1000 * 40)

	if ((nNowTime - st_nLastTime) > DELAY_POST_COPY_TO_TESTSERVER)
	{
		char szMsg[340];
		strcpy(szMsg, argv[1]);
		ZPostStaffHelp(szMsg);
	} else
		ZChatOutput("Por favor aguarde 40 segundos.");
}

void ChatCmd_StaffChat(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	char szMsg[256];

	strcpy(szMsg, argv[1]);
	ZPostStaffChat(szMsg);
}
void ChatCmd_PingServer(const char* line, const int argc, char **const argv) 
{
	ZPostServerPing(timeGetTime(), 0);
}

void ChatCmd_ServerTime(const char* line, const int argc, char **const argv) 
{
	ZPostServerTime();
}

void ChatCmd_AdminSwitchCreateLadderGame(const char* line, const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	bool bEnabled = true;
	if (!strcmp(argv[1], "0")) bEnabled = false;

	ZPostAdminRequestSwitchLadderGame(ZGetGameClient()->GetPlayerUID(), bEnabled);
}

void ChatCmd_Suicide(const char* line,const int argc, char **const argv)
{
	ZGetGameClient()->RequestGameSuicide();
}


void ChatCmd_LadderInvite(const char* line,const int argc, char **const argv)
{
	if (argc < 3) 
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	// Å×½ºÆ®·Î ¿ì¼± 2¸íÀÌ ÇÑÆÀ
	char szNames[2][256];
	strcpy(szNames[0], argv[1]);
	strcpy(szNames[1], argv[2]);

	ZGetGameClient()->RequestProposal(MPROPOSAL_LADDER_INVITE, &argv[1], 2);
}

void ChatCmd_LadderTest(const char* line,const int argc, char **const argv)
{
	// »ç¿ëÇÏ´Â ºÎºÐÀÌ ¾ø¾î¼­ µð¹ö±×¿ëÀ¸·Î ¼öÁ¤ÇÔ. -by SungE 2007-04-02
#ifdef _DEBUG
	if (argc == 1)
	{
		char szPlayerName[MATCHOBJECT_NAME_LENGTH];
		strcpy(szPlayerName, ZGetMyInfo()->GetCharName());
		char* pName[1];
		pName[0] = szPlayerName;

		ZPostLadderRequestChallenge(pName, 1, 0);
	} else if (argc == 2)
	{
		char szPlayerName[MATCHOBJECT_NAME_LENGTH], szTeamMember1[MATCHOBJECT_NAME_LENGTH];
		strcpy(szPlayerName, ZGetMyInfo()->GetCharName());
		strcpy(szTeamMember1, argv[1]);

		char*pName[2];
		pName[0] = szPlayerName;
		pName[1] = szTeamMember1;

		ZPostLadderRequestChallenge(pName, 2, 0);
	}
#endif
}

void ChatCmd_LaunchTest(const char* line,const int argc, char **const argv)
{
	// »ç¿ëÇÏ´Â ºÎºÐÀÌ ¾ø¾î¼­ µð¹ö±×¿ëÀ¸·Î ¼öÁ¤ÇÔ. -by SungE 2007-04-02
#ifdef _DEBUG
	MCommand* pCmd = ZGetGameClient()->CreateCommand(MC_MATCH_LADDER_LAUNCH, ZGetMyUID());
	pCmd->AddParameter(new MCmdParamUID(MUID(0,0)));
	pCmd->AddParameter(new MCmdParamStr("Mansion"));
	ZGetGameClient()->Post(pCmd);
#endif
}

void ChatCmd_Callvote(const char* line,const int argc, char **const argv)
{
	if ( (argv[1] == NULL) || (argv[2] == NULL) )
	{
		OutputCmdWrongArgument(argv[0]);
		return;
	}

	ZPOSTCMD2(MC_MATCH_CALLVOTE, MCmdParamStr(argv[1]), MCmdParamStr(argv[2]))
}

void ChatCmd_VoteYes(const char* line,const int argc, char **const argv)
{
	ZPOSTCMD0(MC_MATCH_VOTE_YES);
}

void ChatCmd_VoteNo(const char* line,const int argc, char **const argv)
{
	ZPOSTCMD0(MC_MATCH_VOTE_NO);
}

void ChatCmd_Kick(const char* line,const int argc, char **const argv)
{
	ZGetCombatInterface()->GetVoteInterface()->CallVote("kick");
}

void ChatCmd_MouseSensitivity(const char* line,const int argc, char **const argv)
{
	if (argc == 1) 
	{
		ZChatOutputMouseSensitivityCurrent( ZGetConfiguration()->GetMouseSensitivityInInt());
	}
	else if (argc == 2)
	{
		// °¨µµ ¼³Á¤
		int original = ZGetConfiguration()->GetMouseSensitivityInInt();

		char* szParam = argv[1];
		int asked = atoi(szParam);
        int changed = ZGetConfiguration()->SetMouseSensitivityInInt(asked);

		ZChatOutputMouseSensitivityChanged(original, changed);
	}
	else
		OutputCmdWrongArgument(argv[0]);
}

void ChatCmd_Exit(const char* line, const int argc, char** const argv)
{
	PostQuitMessage(0);
}
void ChatCmd_Status(const char* line, const int argc, char** const argv)
{
	if (argc < 2)
	{
		ZChatOutput("Usage: /status <status> | /status off to disable");
		return;
	}

//	if (strlen(argv[1]) < 128)
	//	ZPostUserStatus(argv[1]);
//	else
	//	ZChatOutput("Status must be less than 128 characters");
}

// Äù½ºÆ® Å×½ºÆ®¿ë ¸í·É¾î /////////////////////////////////////////////////////
void ChatCmd_QUESTTEST_God(const char* line,const int argc, char **const argv)
{
#ifdef _DEBUG
	if (!ZIsLaunchDevelop() && !ZGetMyInfo()->IsAdminGrade()) return;
	if(ZGetGame() == NULL) return;

	bool bNowGod = ZGetQuest()->GetCheet(ZQUEST_CHEET_GOD);
	bNowGod = !bNowGod;

	ZGetQuest()->SetCheet(ZQUEST_CHEET_GOD, bNowGod);

	if (bNowGod)
	{
		ZChatOutput( "God mode enabled" );
	}
	else
	{
		ZChatOutput( "God mode disabled" );
	}
#endif
}


void ChatCmd_QUESTTEST_SpawnNPC(const char* line,const int argc, char **const argv)
{
//#ifdef _DEBUG
	//if (!ZIsLaunchDevelop() && !ZGetMyInfo()->IsAdminGrade()) return;
	if(ZGetGame() == NULL) return;
	if (argc < 2) return;

	int nNPCID = 0;
	int nCount = 1;

	nNPCID = atoi(argv[1]);
	if(argv[2])
		nCount = atoi(argv[2]);

	ZPostQuestTestNPCSpawn(nNPCID, nCount);
//#endif
}



void ChatCmd_QUESTTEST_LocalSpawnNPC(const char* line,const int argc, char **const argv)
{
#ifdef _DEBUG
	if (!ZIsLaunchDevelop() && !ZGetMyInfo()->IsAdminGrade()) return;
	int nNPCID = 0;
	int nCount = 1;

	nNPCID = atoi(argv[1]);
	if(argv[2]) nCount = atoi(argv[2]);

	MCommand* pCmd = ZNewCmd(MC_QUEST_NPC_LOCAL_SPAWN);
	pCmd->AddParameter(new MCmdParamUID(ZGetMyUID()));
	
	MUID uidLocal;
	uidLocal.High = 10000;
	uidLocal.Low = (unsigned long)ZGetObjectManager()->size();

	pCmd->AddParameter(new MCmdParamUID(uidLocal));
	pCmd->AddParameter(new MCmdParamUChar((unsigned char)nNPCID));
	pCmd->AddParameter(new MCmdParamUChar((unsigned char)0));

	ZPostCommand(pCmd);
	
#endif
}


void ChatCmd_QUESTTEST_NPCClear(const char* line,const int argc, char **const argv)
{
#ifdef _DEBUG
	if (!ZIsLaunchDevelop() && !ZGetMyInfo()->IsAdminGrade()) return;
	if(ZGetGame() == NULL) return;

	ZPostQuestTestClearNPC();
#endif
}


void ChatCmd_QUESTTEST_Reload(const char* line,const int argc, char **const argv)
{
#ifdef _DEBUG

	if (!ZIsLaunchDevelop() && !ZGetMyInfo()->IsAdminGrade()) return;

	ZGetObjectManager()->ClearNPC();	// ¸ÕÀú NPC°¡ Å¬¸®¾îµÇ¾î¾ß µÚÅ»ÀÌ ¾ø´Ù.
	ZGetQuest()->Reload();

	ZChatOutput( "Reloaded" );
#endif
}


void ChatCmd_QUESTTEST_SectorClear(const char* line,const int argc, char **const argv)
{
#ifdef _DEBUG
	if (!ZIsLaunchDevelop() && !ZGetMyInfo()->IsAdminGrade()) return;
	if(ZGetGame() == NULL) return;

	ZPostQuestTestSectorClear();
#endif
}

void ChatCmd_QUESTTEST_Finish(const char* line,const int argc, char **const argv)
{
#ifdef _DEBUG
	if (!ZIsLaunchDevelop() && !ZGetMyInfo()->IsAdminGrade()) return;
	if(ZGetGame() == NULL) return;

	ZPostQuestTestFinish();
#endif
}

void ChatCmd_QUESTTEST_WeakNPCs(const char* line,const int argc, char **const argv)
{
#ifdef _DEBUG
	if (!ZIsLaunchDevelop() && !ZGetMyInfo()->IsAdminGrade()) return;
	if(ZGetGame() == NULL) return;

	bool bNow = ZGetQuest()->GetCheet(ZQUEST_CHEET_WEAKNPCS);
	bNow = !bNow;

	ZGetQuest()->SetCheet(ZQUEST_CHEET_WEAKNPCS, bNow);

	if (bNow)
	{
		ZChatOutput( "WeakNPC mode enabled" );

		// Áö±Ý ÀÖ´Â NPCµéÀÇ HP¸¦ 1·Î ¼¼ÆÃ
		for (ZObjectManager::iterator itor = ZGetObjectManager()->begin();
			itor != ZGetObjectManager()->end(); ++itor)
		{
			ZObject* pObject = (*itor).second;
			if (pObject->IsNPC())
			{
				ZActor* pActor = (ZActor*)pObject;
				ZModule_HPAP* pModule = (ZModule_HPAP*)pActor->GetModule(ZMID_HPAP);
				if (pModule)
				{
					pModule->SetHP(1);
				}
			}
		}
	}
	else
	{
		ZChatOutput( "WeakNPC mode disabled" );
	}
#endif
}

///////////////////////////////////////////////////////////////////////////////