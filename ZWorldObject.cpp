#include "stdafx.h"

#include "ZApplication.h"
#include "MXML.h"
#include "MZFileSystem.h"
#include "RealSpace2.h"
#include "ZWorldObject.h"
#include "ZCharacter.h"
#include "RDummyList.h"
#include "MDebug.h"
#include "ZPost.h"
#include "ZPostLocal.h"
#include "MDataChecker.h"
#include "MUtil.h"

using namespace RealSpace2;

#define BASE_EFFECT_MODEL "baseEffect"
#define USER_WORLDOBJECT_FIRST		100			// 100 번부터는 유저가 스폰시키는 아이템 - 의료킷 등


ZWorldObjectManager ZWorldObjectManager::msInstance;

//////////////////////////////////////////////////////////////////////////
//		ZWorldObject
//////////////////////////////////////////////////////////////////////////
void ZWorldObject::Initialize( int nID, short nObjectID, ZWORLD_OBJECT_STATE state, unsigned int nSpawnTypeFlags,  rvector& position, float fAmount )
{
	m_nID				= nID;
	m_nObjectID			= nObjectID;
	m_State				= state;
	m_nSpawnTypeFlags	= nSpawnTypeFlags;
	m_Position			= position;
	m_fAmount			= fAmount;
	m_Dir				= rvector(0,1,0);
	m_Up				= rvector(0,0,1);
}

bool ZWorldObject::WorldObjectAction( ZCharacter* pCharacter )
{
	if( pCharacter == NULL || m_State != State0)
		return false;	

	ZItem* pSeletedWeapon = 0;
	switch( m_Type ) 
	{
	case WIT_WINDOW:
		//fix window
		if( pCharacter == ZGetGame()->m_pMyCharacter ) {
		ZGetSoundEngine()->PlaySound("fx_windowfix");
	}
		break;
	default:
		mlog("Zombies: Unknown Action");
		return false;
	}
	return true;

}

ZWorldObject::ZWorldObject()
{
	m_nID					= 0;
	m_Type				= WIT_WINDOW;
	m_State				= State0;
	m_Position			= rvector( 0.f, 0.f, 0.f );
	m_nSpawnTypeFlags	= WORLD_OBJECT_TIME_ONCE;
	m_pVMesh			= NULL;

	m_dwStartTime		 = 0.f;
	m_dwToggleBackupTime = 0.f;

	m_bToggle			= true;
	m_bisDraw			= true;
}

ZWorldObject::~ZWorldObject() 
{
	if(m_pVMesh) {
		delete m_pVMesh;
		m_pVMesh = NULL;
	}
}

void ZWorldObject::CreateVisualMesh()
{
	RMesh* pMesh	= ZGetMeshMgr()->Get( m_modelName );
	m_pVMesh = new RVisualMesh;
	m_pVMesh->Create( pMesh );
	m_pVMesh->SetAnimation("play");
	m_pVMesh->SetCheckViewFrustum(true);
}

//////////////////////////////////////////////////////////////////////////
//		ZWorldObjectMananger
//////////////////////////////////////////////////////////////////////////
ZWorldObjectManager::ZWorldObjectManager()
{
	m_nStandAloneIDGen = 10000000;		// WorldObject이 천만개 이상은 나오지 않는다.

	m_bEnableDraw = true;
}

int ZWorldObjectManager::GenStandAlondID()
{
	return (++m_nStandAloneIDGen);
}

bool ZWorldObjectManager::WorldObjectAction( int nID, ZCharacter* pCharacter )
{
	WorldWIL_Iterator iter = mObjectList.find( nID );
	if( iter == mObjectList.end() )	return false;

	return WorldObjectAction( iter, pCharacter );
}

bool ZWorldObjectManager::WorldObjectAction( WorldWIL_Iterator& iter, ZCharacter* pCharacter )
{
	ZWorldObject* pWorldObject = iter->second;
	if( !pWorldObject->WorldObjectAction( pCharacter ) )	
	{
		mlog("ApplyItem Function" );
		return false;	
	}

	// 퀘스트에서 다른 팀을 기다리는 대기 상태인 경우..

	if( ZGetQuest() && ZGetQuest()->GetQuestState() == MQUEST_COMBAT_PREPARE )
		if( ZGetGame()->m_pMyCharacter->IsObserverTarget() )// 다음스테이지로 넘어가서 기다리는 경우
			return true;

	switch(pWorldObject->GetType())
	{
		case WIT_WINDOW:
			//repair window effectZGetEffectManager()->AddExpanseAmmoEffect(pCharacter->GetPosition(), pCharacter );
			break;
	}

	if( CheckBitSet(pWorldObject->GetSpawnTypeFlags(), WORLD_OBJECT_TIME_ONCE) ) {
		DeleteWorldObject( iter, false );
	} else {
		pWorldObject->SetState( State7 );
	}

	return true;
}

ZWorldObject *ZWorldObjectManager::AddWorldObject( int nID, short nItemID,rvector& pos )
{
	ZWorldObject* pWorldObject = NULL;

	WorldWIL_Iterator iterItem = mObjectList.find( nID );
	if( iterItem == mObjectList.end() )
	{
		map<short, MMatchWorldObjectDesc*>::iterator iter = MGetMatchWorldObjectDescMgr()->find( nItemID );
		if( iter == MGetMatchWorldObjectDescMgr()->end() ) 
		{
			// mlog("추가하려는 월드아이템이 정의 되지 않은 이름입니다\n" );
			return NULL;
		}
		MMatchWorldObjectDesc* pDesc = iter->second;

		unsigned long int nSpawnTypeFlags = WORLD_OBJECT_TIME_ONCE;

		pWorldObject = new ZWorldObject();
		pWorldObject->Initialize( nID, nItemID,State0, nSpawnTypeFlags, pos, pDesc->m_fAmount );
		pWorldObject->SetName( pDesc->m_szDescName );
		pWorldObject->SetModelName( pDesc->m_szModelName );
		pWorldObject->SetType(pDesc->m_nObjectType);
		pWorldObject->m_dwStartTime = timeGetTime();
		pWorldObject->CreateVisualMesh();
		mObjectList.insert( WorldObjectList::value_type( nID, pWorldObject ));
		iterItem = mObjectList.find( nID );
	}
	SpawnWorldObject( iterItem );
	
	return pWorldObject;
}

// TODO 디버깅 중이다
#pragma optimize( "", off )

#define  WORLD_OBJECT_RADIUS		100.f
void ZWorldObjectManager::update()
{
	ZCharacter* pCharacter = ZGetGame()->m_pMyCharacter;
	
	if( pCharacter==NULL||pCharacter->IsDie() ) return; 
	
	for( WorldWIL_Iterator iter = mObjectList.begin(); iter != mObjectList.end();)
	{
		ZWorldObject* pItem = iter->second;
		if( pItem->GetState() == State0 )
		{
			char szName[64];
			strcpy(szName,pItem->GetName());
			rvector charPos = pCharacter->GetPosition();
			rvector itemPos = pItem->GetPosition();

			if( D3DXVec3Length( &(charPos - itemPos)) <= WORLD_OBJECT_RADIUS )
			{
				//OnOptainWorldObject(pItem);
			}			
		}
		++iter;
	}
}
#pragma optimize( "", on )

void ZWorldObjectManager::Clear()
{
	for( WorldWIL_Iterator iter = mObjectList.begin(); iter != mObjectList.end(); )
	{
		SAFE_DELETE( iter->second );
		iter = mObjectList.erase( iter );
	}
	mDrawer.Clear();
}

bool ZWorldObjectManager::DeleteWorldObject( int nID , bool bDrawRemoveEffect)
{
	WorldWIL_Iterator iter = mObjectList.find( nID );
	if( iter == mObjectList.end() )	return false;

	DeleteWorldObject( iter, bDrawRemoveEffect);
	return true;
}

void ZWorldObjectManager::DeleteWorldObject( WorldWIL_Iterator& iter , bool bDrawRemoveEffect)
{
	if (bDrawRemoveEffect)
	{
		ZWorldObject* pItem = iter->second;
		mDrawer.DrawEffect(WORLD_OBJECT_EFFECT_REMOVE, pItem->GetPosition());
	}

	SAFE_DELETE( iter->second );
	mObjectList.erase( iter );
}

void ZWorldObjectManager::Draw(int mode,float height,bool bWaterMap)//임시..
{
	if (!m_bEnableDraw) return;

	ZWorldObject* pWorldObject	= 0;

	RSetWBuffer( TRUE );
	RGetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	float _h = 0.f;

	for( WorldWIL_Iterator iter = mObjectList.begin(); iter != mObjectList.end(); ++iter )
	{
		pWorldObject	= iter->second;
		if( pWorldObject->GetState() == State0 )
		{
			_h = pWorldObject->GetPosition().z;

			bool bDraw = false;

			if(mode==0) {			// 먼저그리기

				if( bWaterMap ) {
					if( _h <= height)	// 물속
						bDraw = true;
				}
				else {
					bDraw = false;
				}
					bDraw = true;
				
			} else if(mode==1) {	// 나중그리기

				if( bWaterMap )  {
					if(bWaterMap && _h > height)	// 물밖
						bDraw = true;
				}
				else {
					bDraw = true;
				}
			}

			if(bDraw)
				mDrawer.DrawWorldObject( pWorldObject );
		}
	}
}

void ZWorldObjectManager::Draw()
{
	if (!m_bEnableDraw) return;

	ZWorldObject* pWorldObject	= 0;

	RSetWBuffer( TRUE );
	RGetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	for( WorldWIL_Iterator iter = mObjectList.begin(); iter != mObjectList.end(); ++iter )
	{
		pWorldObject	= iter->second;
		if( pWorldObject->GetState() == State0 )
			mDrawer.DrawWorldObject( pWorldObject );
	}
}

bool ZWorldObjectManager::SpawnWorldObject( WorldWIL_Iterator& iter )
{
	ZWorldObject* pItem = iter->second;
	pItem->SetState( State0 );

	if (pItem->GetItemID() < USER_WORLDOBJECT_FIRST)
		mDrawer.DrawEffect(WORLD_OBJECT_EFFECT_CREATE, pItem->GetPosition());

	return true;
}

void ZWorldObjectManager::Reset(bool bDrawRemoveEffect)
{
	for( WorldWIL_Iterator iter = mObjectList.begin(); iter != mObjectList.end(); ++iter)
	{
		ZWorldObject* pItem = iter->second;

		if (bDrawRemoveEffect)
		{
			if ((pItem->GetType() != WIT_WINDOW) && (pItem->GetState() == State0))
				mDrawer.DrawEffect(WORLD_OBJECT_EFFECT_REMOVE, pItem->GetPosition());
		}

		SAFE_DELETE( pItem );
	}

	mObjectList.clear();
}

//////////////////////////////////////////////////////////////////////////
//		ZWorldObjectDrawer
//////////////////////////////////////////////////////////////////////////
RVisualMesh* ZWorldObjectDrawer::AddMesh( const char* pName )
{
	_ASSERT( pName );
	RMesh* pMesh	= ZGetMeshMgr()->Get( (char*)pName );
	RVisualMesh* pVMesh = new RVisualMesh;
	pVMesh->Create( pMesh );
 	pVMesh->SetAnimation("play");
	pVMesh->SetCheckViewFrustum(true);
	mVMeshList.insert( WorldObjectVMeshMap::value_type( string(pName), pVMesh ));
	return pVMesh;
}

void ZWorldObjectDrawer::DrawWorldObject( ZWorldObject* pWorldObject, bool Rotate )
{
	_ASSERT( pWorldObject != 0 );

	if( pWorldObject->m_bisDraw==false )
		return; //weapon 에서 그려주는 모델의 경우..

	WIVMM_iterator iter = mVMeshList.find( string( pWorldObject->GetModelName()) );
	RVisualMesh* pVMesh = 0;

	if(pWorldObject->m_pVMesh) {// 자신만의 visual mesh 를 사용한다면~
		pVMesh = pWorldObject->m_pVMesh;

		/*if(pWorldObject->GetType()==WIT_QUEST)
			if( pVMesh->isOncePlayDone() )
				pVMesh->SetAnimation("playloop");*/
	}
	else {

		if( iter == mVMeshList.end() ) 
			pVMesh = AddMesh( pWorldObject->GetModelName() );
		else
			pVMesh = iter->second;
	}
	
	rmatrix world;
	rvector pos = pWorldObject->GetPosition();
	rvector dir = pWorldObject->GetDir();
	rvector up  = pWorldObject->GetUp();

//	MakeWorldMatrix( &world, pos, rvector(0,1,0), rvector(0,0,1) );
	MakeWorldMatrix( &world, pos, dir , up );

	float fVis = 1.f;

	DWORD thistime = timeGetTime();
	pVMesh->SetVisibility(fVis);

	pVMesh->SetWorldMatrix( world );
	pVMesh->Frame();
	pVMesh->Render();

	if(pVMesh->m_bIsRender==false)
		return;

	/*if (pWorldObject->GetItemID() < USER_WorldObject_FIRST) 
		if(pWorldObject->GetType()!=WIT_QUEST)
			DrawEffect( WORLD_OBJECT_EFFECT_IDLE, pos );*/
}

void ZWorldObjectDrawer::DrawEffect( ZWORLD_OBJECT_EFFECT effect, rvector& pos )
{
	RVisualMesh* pVMesh = 0;
	switch( effect ) 
	{
	case WORLD_OBJECT_EFFECT_CREATE:
		//ZGetEffectManager()->AddWorldObjectEatenEffect( pos );
		break;
	case WORLD_OBJECT_EFFECT_REMOVE:
		//ZGetEffectManager()->AddWorldObjectEatenEffect( pos );
		break;
	case WORLD_OBJECT_EFFECT_IDLE:
		{
		WIVMM_iterator iter = mVMeshList.find( string(BASE_EFFECT_MODEL) );
		if( iter == mVMeshList.end() )
			pVMesh = AddMesh( BASE_EFFECT_MODEL );
		else
			pVMesh = iter->second;
		if( pVMesh != 0 )
		{
			rmatrix world;
			MakeWorldMatrix( &world, pos, rvector(0,1,0), rvector(0,0,1) );

			pVMesh->SetWorldMatrix( world );
			pVMesh->Frame();
			pVMesh->Render();
		}			
		}
		break;

	}
}

ZWorldObjectDrawer::~ZWorldObjectDrawer()
{
	Clear();
}

void ZWorldObjectDrawer::Clear()
{
	for( WIVMM_iterator iter = mVMeshList.begin(); iter != mVMeshList.end(); )
	{
		RVisualMesh* pVMesh = iter->second;
		SAFE_DELETE( pVMesh );
		iter	= mVMeshList.erase( iter );
	}
}



//////////////////////////////////////////////////////////////////////////
//		Global Method
//////////////////////////////////////////////////////////////////////////
ZWorldObjectManager* ZGetWorldObjectManager()
{
	return ZWorldObjectManager::GetInstance();
}