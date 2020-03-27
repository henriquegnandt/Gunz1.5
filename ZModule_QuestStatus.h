#ifndef _ZMODULE_QUESTSTATUS_H
#define _ZMODULE_QUESTSTATUS_H


#include "ZModule.h"
#include "ZModuleID.h"


// ����Ʈ���� ����ϴ� �÷��̾� ����
class ZModule_QuestStatus : public ZModule {
private:
	int				m_nKills;		// NPC ų��
	int				m_nCoins;
public:
	DECLARE_ID(ZMID_QUESTSTATUS)
	ZModule_QuestStatus();
	virtual ~ZModule_QuestStatus();

	inline void AddKills();
	inline int GetCoins();
	inline int GetKills();
};


// inline functions /////////////////////////////////////////////////////////////
inline void ZModule_QuestStatus::AddKills()
{
	m_nKills++;
	m_nCoins+=50;
}

inline int ZModule_QuestStatus::GetKills()
{
	return m_nKills;
}

inline int ZModule_QuestStatus::GetCoins()
{
	return m_nCoins;
}
#endif