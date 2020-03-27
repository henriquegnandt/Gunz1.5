#ifndef _ZFILEPATH_H
#define _ZFILEPATH_H


#define FILENAME_TIPS					"tips.xml"
#define FILENAME_ZITEM_DESC				"System/zitem.xml"
#define FILENAME_ZITEM_DESC_LOCALE		"System/zitem_locale.xml"
#define FILENAME_BUFF_DESC				"System/zBuff.xml"
//#define FILENAME_ZITEMEFFECT_DESC		"System/zeffect.xml"
#define FILENAME_INTERFACE_MAIN			"main.xml"
#define FILENAME_ZNPC_DESC				"System/npc.xml"
#define FILENAME_ZSKILL_DESC			"System/zskill.xml"
#define FILENAME_SURVIVALMAP			"System/survivalmap.xml"
#define FILENAME_QUESTMAP				"System/questmap.xml"
#define FILENAME_QUESTITEM_DESC			"System/zquestitem.xml"
#define FILENAME_DROPTABLE				"System/droptable.xml"
#define FILENAME_CHATCMDS				"System/chatcmds.xml"
#define FILENAME_WORLDITEM				"System/worlditem.xml"
#define FILENAME_FILELIST				"System/filelist.xml"
#define FILENAME_ABUSE					"System/abuse.txt"

#define MPATH_EMBLEMFOLDER				"/Emblem"
#define MPATH_EMBLEMFILE				"/emblem.xml"

// path는 마지막에 '/' 를 꼭 붙여줘야 한다.

#define	PATH_INTERFACE					"interface/default/"
#define PATH_USA						"system/USA/"
#define PATH_SPA						"system/SPA/"
#define PATH_POR						"system/POR/"
#define PATH_CUSTOM_CROSSHAIR			"custom/crosshair/"
#define PATH_GAME_MAPS					"maps/"
#define PATH_QUEST						"quest/"
#define PATH_QUEST_MAPS					"quest/maps/"

// 마지막에 /를 붙이지 않음. ZLanguageConf클래스에서 커리해줌.
const char CLI_LANG_DIR[]					= "lang";
const char CLI_MESSAGE_TYPE[]				= "message";
const char CLI_ERRMESSAGE_TYPE[]			= "cserror";
const char CLI_MESSAGE_FILE_NAME[]			= "messages.xml";
const char CLI_ERROR_MESSAGE_FILE_NAME[]	= "cserror.xml";
/////////////////////////////////////////////////////////////







#endif