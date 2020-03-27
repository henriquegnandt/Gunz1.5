#pragma once

#include "MMatchWorldObjectDesc.h"
#include "RTypes.h"
#include "RMesh.h"
#include "RVisualMeshMgr.h"
#include "list"
#include "map"


using namespace std;
using namespace RealSpace2;

enum ZWORLD_OBJECT_SPAWN_FLAG
{
	WORLD_OBJECT_TIME_ONCE			= 0x1,
	WORLD_OBJECT_TIME_REGULAR			= 0x2,
	WORLD_OBJECT_STAND_ALINE			= 0x4,
};

enum ZWORLD_OBJECT_STATE
{
	State0 = 0,
	State1,
	State2,
	State3,
	State4,
	State5,
	State6,
	State7,
};

enum ZWORLD_OBJECT_EFFECT
{
	WORLD_OBJECT_EFFECT_CREATE = 0,
	WORLD_OBJECT_EFFECT_IDLE,
	WORLD_OBJECT_EFFECT_NUM,
	WORLD_OBJECT_EFFECT_REMOVE,
};

#define MAX_NAME_LENGTH 256
class ZCharacter;

//////////////////////////////////////////////////////////////////////////
class ZWorldObject
{
protected:
	short					m_nID;
	short					m_nObjectID;
	char					m_Name[MAX_NAME_LENGTH];
	char					m_modelName[MAX_NAME_LENGTH];
	MMATCH_WORLD_OBJECT_TYPE	m_Type;	
	ZWORLD_OBJECT_STATE		m_State;
	rvector					m_Position;
	rvector					m_Dir;
	rvector					m_Up;
	unsigned int			m_nSpawnTypeFlags;
	float					m_fAmount;											
public:
	RVisualMesh*			m_pVMesh;
	unsigned int			m_dwStartTime;
	unsigned int			m_dwToggleBackupTime;
	bool					m_bToggle;
	bool					m_bisDraw;
public:
	void Initialize( int nID, short nObjectID, ZWORLD_OBJECT_STATE state, unsigned int nSpawnTypeFlags,	rvector& position, float fAmount);
	virtual bool WorldObjectAction(ZCharacter* pCharacte );
	void CreateVisualMesh();
public:
	void SetPostion( const rvector& p )						{ m_Position = p; };
	void SetDir( const rvector& p )							{ m_Dir = p; };
	void SetUp( const rvector& p )							{ m_Up = p; };
	void SetState( ZWORLD_OBJECT_STATE state )				{ m_State	= state; };
	void SetType( MMATCH_WORLD_OBJECT_TYPE type )				{ m_Type = type; };
	void SetName( char* szName )							{ strcpy(m_Name, szName );	};
	void SetModelName( char* szName )						{ strcpy(m_modelName, szName );	};
	rvector GetPosition() const								{ return m_Position; };
	rvector GetDir() const									{ return m_Dir; };
	rvector GetUp() const									{ return m_Up; };
	MMATCH_WORLD_OBJECT_TYPE GetType() const	                { return m_Type; };
	ZWORLD_OBJECT_STATE GetState() const				    { return m_State; };
	int GetID() const										{ return m_nID;	};	
	short GetItemID() const								    { return m_nObjectID; }
	const char* GetName() const								{ return m_Name; };
	const char* GetModelName() const						{ return m_modelName; };
	unsigned int GetSpawnTypeFlags() const					{ return m_nSpawnTypeFlags; };
	
public:
	ZWorldObject();
	~ZWorldObject();
};

//////////////////////////////////////////////////////////////////////////
typedef list<ZWorldObject* > WorldWaitingList;

typedef map< int, ZWorldObject* >	WorldObjectList;
typedef WorldObjectList::iterator		WorldWIL_Iterator;

typedef map<string, RealSpace2::RVisualMesh* >  WorldObjectVMeshMap;
typedef WorldObjectVMeshMap::iterator	WIVMM_iterator;

//////////////////////////////////////////////////////////////////////////
class ZWorldObjectDrawer
{
protected:
	WorldObjectVMeshMap mVMeshList;

protected:
	RVisualMesh* AddMesh( const char* pName );

public:
	void Clear();

	void DrawWorldObject( ZWorldObject* pWorldObject, bool Rotate = false );
	void DrawEffect( ZWORLD_OBJECT_EFFECT effect,  rvector& pos );

public:
	~ZWorldObjectDrawer();	
};

//////////////////////////////////////////////////////////////////////////
class ZWorldObjectManager
{
protected:
	WorldObjectList			   mObjectList;
	ZWorldObjectDrawer		   mDrawer;
	static ZWorldObjectManager msInstance;
	bool					   m_bEnableDraw;
	int						   m_nStandAloneIDGen;
	int GenStandAlondID();
protected:
	bool WorldObjectAction( WorldWIL_Iterator& iter, ZCharacter* pCharacter );
	void DeleteWorldObject( WorldWIL_Iterator& iter, bool bDrawRemoveEffect );
	bool SpawnWorldObject( WorldWIL_Iterator& iter );
public:
	void update();
	ZWorldObject *AddWorldObject( int nID, short nObjectID, rvector& pos );
	bool DeleteWorldObject( int nID, bool bDrawRemoveEffect=false );
	void Clear();
	void Reset(bool bDrawRemoveEffect=false);
	int GetLinkedWorldObjectID(MMatchWorldObjectSpawnInfo* pObjectDesc);
	bool WorldObjectAction( int nID, ZCharacter* pCharacter );	
	void EnableDraw(bool b) { m_bEnableDraw = b; }
	static ZWorldObjectManager*	GetInstance()	{ return &msInstance; }
public:
	void Draw();
	void Draw(int mode,float height,bool bWaterMap);

public:
	ZWorldObjectManager();
	~ZWorldObjectManager() {};
};

ZWorldObjectManager* ZGetWorldObjectManager();