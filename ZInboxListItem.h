#ifndef _ZINBOXLISTITEM_H
#define _ZINBOXLISTITEM_H

#include "MListBox.h"
class ZInboxListItem : public MListItem {
	int nUID;
	char SenderName[MATCHOBJECT_NAME_LENGTH];
	char Title[INBOX_TITLE_LEN];
	int nInboxState;
	int nInboxType;
	char			m_szItemString[320];
	SYSTEMTIME sysTime;
public:
	ZInboxListItem(int uid, int Type, const char senderName[MATCHOBJECT_NAME_LENGTH], const char title[INBOX_TITLE_LEN], SYSTEMTIME SysTime) 
	{ 
		nUID = uid; 
		nInboxType = Type;  
		strcpy(SenderName, senderName); 
		strcpy(Title, title);
		sysTime = SysTime;
	}
	virtual ~ZInboxListItem()			{}
	virtual const char* GetString(void)	{ 
		sprintf(m_szItemString,"%s - %s - (%d/%d/%d - %d:%d)", Title, SenderName, sysTime.wDay, sysTime.wMonth, sysTime.wYear, sysTime.wHour, sysTime.wMinute);
		return m_szItemString; 
	}
	int GetUID()						{ return nUID; }
	char* GetName()						{ return Title; }
};




#endif