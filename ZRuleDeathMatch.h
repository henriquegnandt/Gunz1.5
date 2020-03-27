#ifndef _ZRULE_DEATH_MATCH_H
#define _ZRULE_DEATH_MATCH_H

#include "ZRule.h"


class ZRuleSoloDeathMatch : public ZRule
{
public:
	ZRuleSoloDeathMatch(ZMatch* pMatch);
	virtual ~ZRuleSoloDeathMatch();
};

class ZRuleSoloGunGame : public ZRule
{
private:
	MUID TopPlayer;
public:
	ZRuleSoloGunGame(ZMatch* pMatch);
	virtual ~ZRuleSoloGunGame();
	void DrawArrow();
	virtual bool OnCommand(MCommand* pCommand);
};

/*class ZRuleTrippleTeamDeathMatch : public ZRule
{
public:
	ZRuleTrippleTeamDeathMatch(ZMatch* pMatch);
	virtual ~ZRuleTrippleTeamDeathMatch();
};*/

class ZRuleTeamDeathMatch : public ZRule
{
public:
	ZRuleTeamDeathMatch(ZMatch* pMatch);
	virtual ~ZRuleTeamDeathMatch();
};

class ZRuleTeamDeathMatch3 : public ZRule
{
public:
	ZRuleTeamDeathMatch3(ZMatch* pMatch);
	virtual ~ZRuleTeamDeathMatch3();
};

class ZRuleTeamDeathMatch2 : public ZRule
{
public:
	ZRuleTeamDeathMatch2(ZMatch* pMatch);
	virtual ~ZRuleTeamDeathMatch2();

	virtual bool OnCommand(MCommand* pCommand);
};

class ZRuleTeamCTF : public ZRule
{
private:
	void AssignFlagEffect(MUID& uidOwner, int nTeam);
public:
	ZRuleTeamCTF(ZMatch* pMatch);
	virtual ~ZRuleTeamCTF();
	MUID m_uidRedFlagHolder;
	MUID m_uidBlueFlagHolder;
	rvector m_BlueFlagPos;
	rvector m_RedFlagPos;
	int m_bRedFlagTaken;
	int m_bBlueFlagTaken;
	inline void SetBlueFlagState(int nState) { m_bBlueFlagTaken = nState; }
	inline void SetRedFlagState(int nState) { m_bRedFlagTaken = nState; }
	inline void SetBlueFlagPos(rvector newvector) { m_BlueFlagPos = newvector; }
	inline void SetRedFlagPos(rvector newvector) { m_RedFlagPos = newvector; }
	inline int GetBlueFlagState() { return m_bBlueFlagTaken; }
	inline int GetRedFlagState() { return m_bRedFlagTaken; }
	inline rvector GetBlueFlagPos() { return m_BlueFlagPos; }
	inline rvector GetRedFlagPos() { return m_RedFlagPos; }
	virtual bool OnCommand(MCommand* pCommand);
	inline void SetBlueCarrier(MUID state) { m_uidBlueFlagHolder = state; }
	inline void SetRedCarrier(MUID state) { m_uidRedFlagHolder = state; }
	inline MUID GetBlueCarrier() { return m_uidBlueFlagHolder; }
	inline MUID GetRedCarrier() { return m_uidRedFlagHolder; }
	void DrawArrow();
};

class ZRuleTeamDeathMatch2Npc : public ZRule
{
private:
public:
	ZRuleTeamDeathMatch2Npc(ZMatch* pMatch);
	virtual ~ZRuleTeamDeathMatch2Npc();
};
// 3 Team DM.
class ZRuleTeamDeathMatchTriple : public ZRule
{
public:
	ZRuleTeamDeathMatchTriple(ZMatch* pMatch);
	virtual ~ZRuleTeamDeathMatchTriple();
};


#endif