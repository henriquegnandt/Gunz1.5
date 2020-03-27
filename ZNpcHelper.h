#ifndef _ZNPCHELPER
#define _ZNPCHELPER
#include "ZBaseQuest.h"

class ZNpcHelper : public ZBaseQuest
{
private:
	ZNPCInfoFromServerManager m_NPCInfoFromServerMgr;
	//MQuestNPCCatalogue			m_NPCCatalogue;		
	bool	m_bCreatedOnce;
	bool	m_bLoaded;
	float	m_fLastWeightTime;
	/*SHIT*/
	MQuestCombatState	m_QuestCombatState;
	float	m_fElapsedHealthUpdateTime;
	int				m_nRewardXP;				// Äù½ºÆ®¿¡¼­ È¹µæÇÑ °æÇèÄ¡.
	int				m_nRewardBP;				// Äù½ºÆ®¿¡¼­ È¹µæÇÑ ¹Ù¿îÆ¼.
	bool	m_bIsQuestComplete;
	bool	m_bIsRoundClear;
	DWORD	m_tRemainedTime;
	ZQuestGameInfo		m_GameInfo;
public:
	MQuestNPCCatalogue GetCatalogue() { return m_NPCCatalogue; }
	virtual ZNPCInfoFromServerManager& GetNPCInfoFromServerMgr() { return m_NPCInfoFromServerMgr; }
	virtual bool OnGameCommand(MCommand* pCommand);
	virtual void LoadNPCMeshes();
	virtual void LoadNPCSounds();
	virtual void UpdateNavMeshWeight(float fDelta);
	virtual void OnGameCreate();
	virtual void OnGameDestroy();
	virtual void OnGameUpdate(float fElapsed);
	ZNpcHelper();
	virtual ~ZNpcHelper();
	virtual bool Load();
	protected:
	virtual bool OnCreate();
	virtual void OnDestroy();
	virtual bool OnCreateOnce();
	virtual void OnDestroyOnce();
	virtual void Reload();



	/*SHIT*/
	virtual bool OnNPCSpawn(MCommand* pCommand) { return true; }
	virtual bool OnNPCDead(MCommand* pCommand) { return true; }
	virtual bool OnPeerNPCDead(MCommand* pCommand) { return true; }
	virtual bool OnEntrustNPCControl(MCommand* pCommand) { return true; }
	virtual bool OnPeerNPCBasicInfo(MCommand* pCommand) { return true; }
	virtual bool OnPeerNPCHPInfo(MCommand* pCommand) { return true; }
	virtual bool OnPeerNPCAttackMelee(MCommand* pCommand) { return true; }
	virtual bool OnPeerNPCAttackRange(MCommand* pCommand) { return true; }
	virtual bool OnPeerNPCSkillStart(MCommand* pCommand) { return true; }
	virtual bool OnPeerNPCSkillExecute(MCommand* pCommand) { return true; }
	virtual bool OnPeerNPCBossHpAp(MCommand* pCommand) { return true; }
	virtual bool OnRefreshPlayerStatus(MCommand* pCommand) { return true; }
	virtual bool OnClearAllNPC(MCommand* pCommand) { return true; }
	virtual bool OnQuestRoundStart(MCommand* pCommand) { return true; }
	virtual bool OnQuestPlayerDead(MCommand* pCommand) { return true; }
	virtual bool OnQuestGameInfo(MCommand* pCommand) { return true; }
	virtual bool OnQuestCombatState(MCommand* pCommand) { return true; }
	virtual bool OnMovetoPortal(MCommand* pCommand) { return true; }
	virtual bool OnReadyToNewSector(MCommand* pCommand) { return true; }
	virtual bool OnSectorStart(MCommand* pCommand) { return true; }
	virtual bool OnObtainQuestItem(MCommand* pCommand) { return true; }
	virtual bool OnObtainZItem(MCommand* pCommand) { return true; }
	virtual bool OnSectorBonus(MCommand* pCommand) { return true; }
	virtual bool OnQuestCompleted(MCommand* pCommand) { return true; }
	virtual bool OnQuestFailed(MCommand* pCommand) { return true; }
	virtual bool OnQuestPing(MCommand* pCommand) { return true; }
	virtual int GetRewardXP( void)							{ return m_nRewardXP; }
	virtual int GetRewardBP( void)							{ return m_nRewardBP; }
	virtual bool IsQuestComplete( void)						{ return m_bIsQuestComplete; }
	virtual bool IsRoundClear( void)						{ return m_bIsRoundClear; }
	virtual DWORD GetRemainedTime( void)					{ return m_tRemainedTime; }
	virtual bool OnRewardQuest( MCommand* pCmd ) { return true; }
	virtual bool OnNewMonsterInfo( MCommand* pCmd ) { return true; }
	virtual bool OnNewRound(MCommand* pCommand) { return true; }
	virtual MQuestCombatState GetQuestState()				{ return m_QuestCombatState; }
	virtual void MoveToNextSector() {}
	virtual bool OnCommand(MCommand* pCommand) { return true; }
	virtual void SetCheet(ZQuestCheetType nCheetType, bool bValue) {}
	virtual bool GetCheet(ZQuestCheetType nCheetType) { return true; }
	virtual bool OnSetMonsterBibleInfo( MCommand* pCmd ) { return true; }
	virtual bool OnPrePeerNPCAttackMelee(MCommand* pCommand) { return true; }
	virtual ZQuestGameInfo* GetGameInfo()		{ return &m_GameInfo; }
	virtual void GetMyObtainQuestItemList( int nRewardXP, int nRewardBP, void* pMyObtainQuestItemListBlob, void* pMyObtainZItemListBlob ) {}
};
#endif