#ifndef ZINTERFACEBACKGROUND_H
#define ZINTERFACEBACKGROUND_H

#include "RBspObject.h"
using namespace RealSpace2;

#define LOGIN_SCENE_FIXEDSKY	0			// �ϴÿ� ī�޶� ����
#define LOGIN_SCENE_FALLDOWN	1			// ī�޶� �������鼭 �ΰ� ����
#define LOGIN_SCENE_FIXEDCHAR	2			// ĳ�������� ī�޶� ����
#define LOGIN_SCENE_SELECTCHAR	3			// ĳ���� ���� ��

class ZInterfaceBackgroundHelper
{
private:
	vector<rvector>	Pos;
	vector<rvector>	Dir;
	int Counter;
	unsigned long int lasttime;
public:
	ZInterfaceBackgroundHelper() { Counter = 0; lasttime = 0; }
	void Add(rvector pos, rvector dir) { Pos.push_back(pos); Dir.push_back(dir); }
	rvector GetStartDir() { return Dir[Counter]; }
	rvector GetEndDir() { return Dir[Counter+1]; }
	rvector GetStartPos() { return Pos[Counter]+rvector(0, 0, 100.f); }
	int GetCount() { return Counter; }
	rvector GetEndPos(rvector pos, rvector dir)
	{
		if((int)Pos.size() >= (Counter+1))
		{
			float fDist = Magnitude(Pos[Counter+1]+rvector(0, 0, 150.f) - pos);
			
			/*if (fDist < 200.0f && timeGetTime() - lasttime >= 1000) 
			{
				char text[50];
				sprintf(text, "Last Count: %d\nDist: %0.2f\n", Counter, fDist);
				OutputDebugString(text);
				lasttime = timeGetTime();
				Counter++;
				return Pos[Counter+1]+rvector(0, 0, 150.f);
			}
			else*/
				return Pos[Counter+1]+rvector(0, 0, 150.f);
		}
		int LastCount = Counter;
		Counter = 0;
		return Pos[LastCount]+rvector(0, 0, 150.f);
	}
};

class ZInterfaceBackground{
private:
	RBspObject*	m_pLogin;					// ���
	ZMapDesc* m_pMapDesc;
	ZInterfaceBackgroundHelper* Helper;
	rmatrix		m_matWorld;					// ���� ��

	int			m_nSceneNumber;				// ��� ��ȣ
	
	rvector		m_vCamPosSt;				// ī�޶� ���� ��ġ
	rvector		m_vCamPosEd;				// ī�޶� ���� ��ġ
	rvector		m_vCamDirSt;				// ī�޶� ���� ����
	rvector		m_vCamDirEd;				// ī�޶� ���� ��ġ
	rvector		m_vCharPos;					// ĳ���� ��ġ
	rvector		m_vCharDir;					// ĳ���� ����

	DWORD		m_dwClock;

//bool		m_bShowMaietLogo;			// Maiet logo ����


protected:
	void SetFogState( float fStart, float fEnd, unsigned long int color);


public:
	ZInterfaceBackground( void);
	virtual ~ZInterfaceBackground( void);

	RBspObject*	GetChurchEnd() { return m_pLogin; }
	int GetScene() { return m_nSceneNumber; }
	void SetScene(int nSceneNumber);

	void LoadMesh( void);
	void Free(void);
	void Draw(void);

	void OnUpdate(float fElapsed);
	void OnInvalidate();
	void OnRestore();

	rvector& GetCharPos() { return m_vCharPos; }
	rvector& GetCharDir() { return m_vCharDir; }
};


#endif
