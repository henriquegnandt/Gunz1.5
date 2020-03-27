#include "stdafx.h"
//#include "../MatchServer/vld/vld.h"

#ifdef _HSHIELD
#include "HShield/HShield.h"
#endif

#ifdef _XTRAP
#include "./XTrap/Xtrap_C_Interface.h"						// update sgk 0702 start
#include "./XTrap/XTrap4Launcher.h"
#pragma comment (lib, "./XTrap/XTrap4Launcher_mt.lib")
#pragma comment (lib, "./XTrap/XTrap4Client_mt.lib")	// update sgk 0702 end
#endif

#include "ZPrerequisites.h"
#include "ZConfiguration.h"
#include "ZGameClient.h"
#include <windows.h>
#include <wingdi.h>
#include <mmsystem.h>
#include <shlwapi.h>
#include <shellapi.h>

#include "dxerr.h"

#include "main.h"
#include "resource.h"
#include "VersionNo.h"

#include "Mint4R2.h"
#include "ZApplication.h"
#include "MDebug.h"
#include "ZMessages.h"
#include "MMatchNotify.h"
#include "RealSpace2.h"
#include "Mint.h"
#include "ZGameInterface.h"
#include "RFrameWork.h"
#include "ZButton.h"
#include "ZDirectInput.h"
#include "ZActionDef.h"
#include "MRegistry.h"
#include "ZInitialLoading.h"
#include "MDebug.h"
#include "MCrashDump.h"
#include "ZEffectFlashBang.h"
#include "ZMsgBox.h"
#include "ZSecurity.h"
#include "ZStencilLight.h"
#include "ZReplay.h"
#include "ZUtil.h"
#include "ZOptionInterface.h"
#include "HMAC_SHA1.h"
#include "ZMouseHook.h"
#ifdef USING_VERTEX_SHADER
#include "RShaderMgr.h"
#endif

//#include "mempool.h"
#include "RLenzFlare.h"
#include "ZLocale.h"
#include "MSysInfo.h"

#include "MTraceMemory.h"
#include "ZInput.h"
#include "Mint4Gunz.h"
#include "SecurityTest.h"
#include "CheckReturnCallStack.h"
#include "AntiHack.h"
#include "ZAntiH.h"
int (WINAPIV * __vsnprintf)(char *, size_t, const char*, va_list) = _vsnprintf;
#ifdef _DEBUG
//jintriple3 ¸Þ¸ð¸® ¸¯ vld
//#include "vld.h"
#endif

#ifdef _DEBUG
RMODEPARAMS	g_ModeParams={640,480,false,D3DFMT_R5G6B5};
//RMODEPARAMS	g_ModeParams={1024,768,false,RPIXELFORMAT_565};
#else
RMODEPARAMS	g_ModeParams={800,600,true,D3DFMT_R5G6B5};
#endif

#ifdef _STEVEN
#define SUPPORT_EXCEPTIONHANDLING
#endif

#define SUPPORT_EXCEPTIONHANDLING

#ifdef LOCALE_NHNUSA
#include "ZNHN_USA.h"
#include "ZNHN_USA_Report.h"
#include "ZNHN_USA_Poll.h"
#endif

#ifdef _GAMEGUARD
#include "ZGameguard.h"
#endif
RRESULT RenderScene(void *pParam);

#define RD_STRING_LENGTH 512
char cstrReleaseDate[512];// = "ReleaseDate : 12/22/2003";

ZApplication	g_App;
MDrawContextR2* g_pDC = NULL;
MFontR2*		g_pDefFont = NULL;
ZDirectInput	g_DInput;
ZInput*			g_pInput = NULL;
Mint4Gunz		g_Mint;

void zexit(int returnCode)
{
	// °ÔÀÓ°¡µå´Â Á¦´ë·Î deleteµÇ¾î¾ß ¿À·ù¹ß»ý½Ã ÀÚÃ¼ ·Î±×¸¦ ¿Ã¹Ù¸£°Ô ³²±æ ¼ö ÀÖ´Ù.
	// ±×³É exit()ÇØµµ ZGameGuard¸¦ ½Ì±ÛÅÏÀ¸·Î ¸¸µé¾ú±â ¶§¹®¿¡ ¼Ò¸êÀÚ¿¡¼­ °ÔÀÓ°¡µå°¡ deleteµÇÁö¸¸ ¾îÂ°¼­ÀÎÁö ±×¶§ Å©·¡½Ã°¡ ÀÏ¾î³­´Ù.
	// exit()ÇÏ±â Àü¿¡ °ÔÀÓ°¡µå¸¦ ¼öµ¿À¸·Î ÇØÁ¦ÇÏ¸é ±×·± ¹®Á¦°¡ ÀÏ¾î³ªÁö ¾Ê´Â´Ù.
	// ÇØÅ· °ËÃâ µîÀÇ ÀÌÀ¯·Î Å¬¶óÀÌ¾ðÆ® Á¾·á½Ã exitÇÏÁö¸»°í zexit¸¦ ¾²ÀÚ.
#ifdef _GAMEGUARD
	GetZGameguard().Release();
#endif
	exit(returnCode);
}

void CrcFailExitApp() { 
#ifdef _PUBLISH
	PostMessage(g_hWnd, WM_CLOSE, 0, 0); 
#else
	int* crash = NULL;
	*crash = 0;
#endif
}

void _ZChangeGameState(int nIndex)
{
	GunzState state = GunzState(nIndex);

	if (ZApplication::GetGameInterface())
	{
		ZApplication::GetGameInterface()->SetState(state);
	}
}

RRESULT OnCreate(void *pParam)
{
	g_App.PreCheckArguments();

	string strFileLenzFlare("System/LenzFlare.xml");
#ifndef _DEBUG
	strFileLenzFlare += "";
#endif
	RCreateLenzFlare(strFileLenzFlare.c_str());
	RGetLenzFlare()->Initialize();

	mlog("main : RGetLenzFlare()->Initialize() \n");

	RBspObject::CreateShadeMap("sfx/water_splash.bmp");
	//D3DCAPS9 caps;
	//RGetDevice()->GetDeviceCaps( &caps );
	//if( caps.VertexShaderVersion < D3DVS_VERSION(1, 1) )
	//{
	//	RGetShaderMgr()->mbUsingShader				= false;
	//	RGetShaderMgr()->shader_enabled				= false;
	//	mlog("main : VideoCard Dosen't support Vertex Shader...\n");
	//}
	//else
	//{
	//	mlog("main : VideoCard support Vertex Shader...\n");
	//}

	//sprintf( cstrReleaseDate, "%s", ZGetSVNRevision().c_str());
	//mlog(cstrReleaseDate); mlog("\n");
	g_DInput.Create(g_hWnd, FALSE, FALSE);
	g_pInput = new ZInput(&g_DInput);
	/*
	for(int i=0; i<ZApplication::GetFileSystem()->GetFileCount(); i++){
		const char* szFileName = ZApplication::GetFileSystem()->GetFileName(i);
		size_t nStrLen = strlen(szFileName);
		if(nStrLen>3){
			if(stricmp(szFileName+nStrLen-3, "ttf")==0){
				int nFileLenth = ZApplication::GetFileSystem()->GetFileLength(i);
				char* pFileData = new char[nFileLenth];
				ZApplication::GetFileSystem()->ReadFile(szFileName, pFileData, nFileLenth);
				int nInstalled = 0;
				HANDLE hFontMem = AddFontMemResourceEx(pFileData, 1, 0, &nInstalled);
				g_FontMemHandles.insert(g_FontMemHandles.end(), hFontMem);
				delete[] pFileData;
			}
		}
	}
	*/
	RSetGammaRamp(Z_VIDEO_GAMMA_VALUE);
	RSetRenderFlags(RRENDER_CLEAR_BACKBUFFER);

	ZGetInitialLoading()->Initialize(  1, 0, 0, RGetScreenWidth(), RGetScreenHeight(), 0, 0, 1024, 768 );

	mlog("InitialLoading success.\n");
	struct _finddata_t c_file;
	intptr_t hFile;
	char szFileName[256];
#define FONT_DIR	"Font/"
#define FONT_EXT	"ttf"
	if( (hFile = _findfirst(FONT_DIR"*." FONT_EXT, &c_file )) != -1L ){
		do{
			strcpy(szFileName, FONT_DIR);
			strcat(szFileName, c_file.name);
			AddFontResource(szFileName);
		}while( _findnext( hFile, &c_file ) == 0 );
		_findclose(hFile);
	}

	g_pDefFont = new MFontR2;

	if( !g_pDefFont->Create("Default", Z_LOCALE_DEFAULT_FONT, DEFAULT_FONT_HEIGHT, 1.0f) )
//	if( !g_pDefFont->Create("Default", RGetDevice(), "FONTb11b", 9, 1.0f, true, false) )
//	if( !g_pDefFont->Create("Default", RGetDevice(), "FONTb11b", 14, 1.0f, true, false) )
	{
		mlog("Fail to Create defualt font : MFontR2 / main.cpp.. onCreate\n" );
		g_pDefFont->Destroy();
		SAFE_DELETE( g_pDefFont );
		g_pDefFont	= NULL;
	}
	//pDefFont->Create("Default", RGetDevice(), "FONTb11b", 10, 1.0f, true, false);
	//pDefFont->Create("Default", RGetDevice(), "FONTb11b", 16, 1.0f, true, false, -1, 4);
	//pDefFont->Create("Default", RGetDevice(), "-2002", 10, 1.0f, false, false, -1, 1);
	//pDefFont->Create("Default", RGetDevice(), "HY¼öÆò¼±L", 12, 1.0f, false, false, -1, 2);

	//MLoadDesignerMode();
	// ±âº» 800x600 µðÀÚÀÎÀ¸·Î »ý¼ºÇÏ°í, ³ªÁß¿¡ Resize¸¦ È­¸é Å©±â·Î ÇØÁØ´Ù.

	g_pDC = new MDrawContextR2(RGetDevice());

#ifndef _FASTDEBUG
	if( ZGetInitialLoading()->IsUseEnable() )
	{
		/*if( ZGetLocale()->IsTeenMode() )
		{
			ZGetInitialLoading()->AddBitmap( 0, "
			loading_teen.jpg" );
		}
		else
		{
			ZGetInitialLoading()->AddBitmap( 0, "interface/loading_adult.jpg" );
		}*/
		int nRand = rand() % 5;

		char szrandloadingscreen[256];
		sprintf(szrandloadingscreen, "interface/default/Loading/loading_%d.jpg", nRand);
	 if (!ZGetInitialLoading()->AddBitmap( 0, szrandloadingscreen))
		ZGetInitialLoading()->AddBitmap( 0, "interface/default/Loading/loading_0.jpg");
		ZGetInitialLoading()->AddBitmapBar( "interface/default/Loading/loading.bmp" );
		ZGetInitialLoading()->SetText( g_pDefFont, 10, 30, cstrReleaseDate );

		ZGetInitialLoading()->AddBitmapGrade( "interface/default/Loading/loading_grade_fifteen.jpg" );

		ZGetInitialLoading()->SetPercentage( 0.0f );
		ZGetInitialLoading()->Draw( MODE_FADEIN, 0 , true );
	}
	
#endif
//	ZGetInitialLoading()->SetPercentage( 10.0f );
//	ZGetInitialLoading()->Draw( MODE_DEFAULT, 0 , true );

	g_Mint.Initialize(800, 600, g_pDC, g_pDefFont);
	Mint::GetInstance()->SetHWND(RealSpace2::g_hWnd);

	mlog("interface Initialize success\n");

//	ZGetConfiguration()->LoadHotKey(FILENAME_CONFIG);

	ZLoadingProgress appLoading("application");
	if(!g_App.OnCreate(&appLoading))
	{
		ZGetInitialLoading()->Release();
		return R_ERROR_LOADING;
	}

//	ZGetInitialLoading()->SetPercentage( 50.0f );
//	ZGetInitialLoading()->Draw( MODE_DEFAULT, 0, true );
	
	// mlog("main : g_App.OnCreate() \n");

	ZGetSoundEngine()->SetEffectVolume(Z_AUDIO_EFFECT_VOLUME);
	ZGetSoundEngine()->SetMusicVolume(Z_AUDIO_BGM_VOLUME);
	ZGetSoundEngine()->SetEffectMute(Z_AUDIO_EFFECT_MUTE);
	ZGetSoundEngine()->SetMusicMute(Z_AUDIO_BGM_MUTE);

	g_Mint.SetWorkspaceSize(g_ModeParams.nWidth, g_ModeParams.nHeight);
	g_Mint.GetMainFrame()->SetSize(g_ModeParams.nWidth, g_ModeParams.nHeight);
	ZGetOptionInterface()->Resize(g_ModeParams.nWidth, g_ModeParams.nHeight);

//	ZGetInitialLoading()->SetPercentage( 80.f );
//	ZGetInitialLoading()->Draw( MODE_DEFAULT, 0, true );
    
	// Default Key
	for(int i=0; i<ZACTION_COUNT; i++){
//		g_Mint.RegisterActionKey(i, ZGetConfiguration()->GetKeyboard()->ActionKeys[i].nScanCode);
		ZACTIONKEYDESCRIPTION& keyDesc = ZGetConfiguration()->GetKeyboard()->ActionKeys[i];
		g_pInput->RegisterActionKey(i, keyDesc.nVirtualKey);
		if(keyDesc.nVirtualKeyAlt!=-1)
			g_pInput->RegisterActionKey(i, keyDesc.nVirtualKeyAlt);
	}

	g_App.SetInitialState();

//	ParseParameter(g_szCmdLine);

	ZGetFlashBangEffect()->SetDrawCopyScreen(true);

	static const char *szDone = "Done.";
	ZGetInitialLoading()->SetLoadingStr(szDone);
	if( ZGetInitialLoading()->IsUseEnable() )
	{
#ifndef _FASTDEBUG
		ZGetInitialLoading()->SetPercentage( 100.f );
		ZGetInitialLoading()->Draw( MODE_FADEOUT, 0 ,true  );
#endif
		ZGetInitialLoading()->Release();
	}
	ChangeAA(ZGetConfiguration()->GetVideo()->nAntiAlias);
	//RMODEPARAMS ModeParams={ RGetScreenWidth(),RGetScreenHeight(),RIsFullScreen(),RGetPixelFormat() };
	//RResetDevice(&ModeParams);
	mlog("main : OnCreate() done\n");

	SetFocus(g_hWnd);

	return R_OK;
}


bool CheckDll(char* fileName, BYTE* SHA1_Value)
{
	BYTE digest[20];
	BYTE Key[GUNZ_HMAC_KEY_LENGTH];

	memset(Key, 0, 20);
	memcpy(Key, GUNZ_HMAC_KEY, strlen(GUNZ_HMAC_KEY));

	CHMAC_SHA1 HMAC_SHA1 ;
	HMAC_SHA1.HMAC_SHA1_file(fileName, Key, GUNZ_HMAC_KEY_LENGTH, digest) ;
	/*mlog("FileName: %s\n", fileName);
	mlog("%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\n", digest[0]
	, digest[1], digest[2], digest[3], digest[4], digest[5], digest[6], digest[7], digest[8], digest[9], digest[10], digest[11]
	, digest[12], digest[13], digest[14], digest[15], digest[16], digest[17], digest[18], digest[19], digest[20]);	*/
	if(memcmp(digest, SHA1_Value, 20) ==0)
	{
		return true;
	}
	return false;
}



RRESULT OnDestroy(void *pParam)
{
	mlog("Destroy gunz\n");

	g_App.OnDestroy();

	SAFE_DELETE(g_pDefFont);

	g_Mint.Finalize();

	mlog("interface finalize.\n");

	SAFE_DELETE(g_pInput);
	g_DInput.Destroy();

	mlog("game input destroy.\n");

	RGetShaderMgr()->Release();

//	g_App.OnDestroy();

	// mlog("main : g_App.OnDestroy()\n");

	ZGetConfiguration()->Destroy();

	mlog("game gonfiguration destroy.\n");

	delete g_pDC;

	struct _finddata_t c_file;
	intptr_t hFile;
	char szFileName[256];
#define FONT_DIR	"Font/"
#define FONT_EXT	"ttf"
	if( (hFile = _findfirst(FONT_DIR"*." FONT_EXT, &c_file )) != -1L ){
		do{
			strcpy(szFileName, FONT_DIR);
			strcat(szFileName, c_file.name);
			RemoveFontResource(szFileName);
		}while( _findnext( hFile, &c_file ) == 0 );
		_findclose(hFile);
	}

	MFontManager::Destroy();
	MBitmapManager::Destroy();
	MBitmapManager::DestroyAniBitmap();

	mlog("Bitmap manager destroy Animation bitmap.\n");

	/*
	for(list<HANDLE>::iterator i=g_FontMemHandles.begin(); i!=g_FontMemHandles.end(); i++){
		RemoveFontMemResourceEx(*i);
	}
	*/

	//ReleaseMemPool(RealSoundEffectPlay);
	//UninitMemPool(RealSoundEffectPlay);

	//ReleaseMemPool(RealSoundEffect);
	//UninitMemPool(RealSoundEffect);

	//ReleaseMemPool(RealSoundEffectFx);
	//UninitMemPool(RealSoundEffectFx);

	//mlog("main : UninitMemPool(RealSoundEffectFx)\n");

	// ¸Þ¸ð¸®Ç® ÇìÁ¦
	ZBasicInfoItem::Release(); // ÇÒ´çµÇ¾î ÀÖ´Â ¸Þ¸ð¸® ÇØÁ¦
//	ZHPInfoItem::Release();

	ZGetStencilLight()->Destroy();
	LightSource::Release();

//	ZStencilLight::GetInstance()->Destroy();

	RBspObject::DestroyShadeMap();
	RDestroyLenzFlare();
	RAnimationFileMgr::GetInstance()->Destroy();
	
	ZStringResManager::ResetInstance();

	mlog("destroy gunz finish.\n");

	return R_OK;
}
DWORD dwLastAntiDebugTick = 0;
typedef unsigned long (__stdcall *pfnNtQueryInformationProcess)(IN  HANDLE, IN  unsigned int, OUT PVOID, IN ULONG, OUT PULONG);
RRESULT OnUpdate(void* pParam)
{
	//_ASSERTE( _CrtCheckMemory( ) );

	__BP(100, "main::OnUpdate");

	g_pInput->Update();

	g_App.OnUpdate();
    /*#ifndef _STEVEN
	const DWORD dwCurrUpdateTime = timeGetTime();
	//Anti Debugger
	{
		if (dwCurrUpdateTime - dwLastAntiDebugTick > 8000) {
			dwLastAntiDebugTick = dwCurrUpdateTime;
			unsigned long Ret, IsRemotePresent = 0, NtGlobalFlags = 0;
			const int ProcessDbgPort = 7;
			char IsDbgPresent1 = 0;
			BOOL IsDbgPresent = FALSE;

			if(IsDebuggerPresent())
				ExitProcess(0);
			__asm {
				 mov eax, fs:[30h]
				 mov al, [eax + 2h]
				 mov IsDbgPresent1, al
			}
			if(IsDbgPresent1)ExitProcess(0);

			CheckRemoteDebuggerPresent(GetCurrentProcess(), &IsDbgPresent);
			if(IsDbgPresent)ExitProcess(0);
			pfnNtQueryInformationProcess NtQueryInfoProcess = NULL;
			HMODULE hNtDll = LoadLibrary(TEXT("ntdll.dll"));
			if(hNtDll == NULL)
			{
			}
			NtQueryInfoProcess = (pfnNtQueryInformationProcess)GetProcAddress(hNtDll, "NtQueryInformationProcess");
			if(NtQueryInfoProcess == NULL)
			{
			}
			Ret = NtQueryInfoProcess(GetCurrentProcess(), ProcessDbgPort, &IsRemotePresent, sizeof(unsigned long), NULL);
			if(Ret == 0x00000000 && IsRemotePresent != 0)ExitProcess(0);
			__asm {
				mov eax, fs:[30h]
				mov eax, [eax + 68h]
				mov NtGlobalFlags, eax
			}
			if(NtGlobalFlags & 0x70)ExitProcess(0);
		}
	}
#endif*/
#if _DEBUG

#ifdef _GAMEGUARD
	if( !GetZGameguard().CheckGameGuardRunning(dwCurrUpdateTime) ||
		!GetZGameguard().CheckD3dDllHooking(dwCurrUpdateTime) )
	{
		if( 0 != ZGetGameClient() ) 
			ZGetGameClient()->Disconnect();

		PostQuitMessage(0);
	}
#endif

#endif
	__EP(100);

	return R_OK;
}

RRESULT OnRender(void *pParam)
{
	__BP(101, "main::OnRender");
	if( !RIsActive() && RIsFullScreen() )
	{
		__EP(101);
		return R_NOTREADY;
	}

	g_App.OnDraw();


#ifdef _SMOOTHLOOP
	Sleep(10);
#endif

#ifndef _PUBLISH

	if(g_pDefFont) {
		static char __buffer[256];

		float fMs = 1000.f/g_fFPS;
		float fScore = 100-(fMs-(1000.f/60.f))*2;

		sprintf(__buffer, "FPS : %3.3f %.3fÁ¡ (%.3f ms)",g_fFPS,fScore,fMs);
		g_pDefFont->m_Font.DrawText( MGetWorkspaceWidth()-200,0,__buffer );
//		OutputDebugString(__buffer);
	}

#endif

	__EP(101);

	return R_OK;
}

RRESULT OnInvalidate(void *pParam)
{
	MBitmapR2::m_dwStateBlock=NULL;

	g_App.OnInvalidate();
	
	return R_OK;
}

RRESULT OnRestore(void *pParam)
{
	for(int i=0; i<MBitmapManager::GetCount(); i++){
		MBitmapR2* pBitmap = (MBitmapR2*)MBitmapManager::Get(i);
		pBitmap->OnLostDevice();
	}

	g_App.OnRestore();

	return R_OK;
}

RRESULT OnActivate(void *pParam)
{
	if (ZGetGameInterface() && ZGetGameClient() && Z_ETC_BOOST)
		ZGetGameClient()->PriorityBoost(true);
	return R_OK;
}

RRESULT OnDeActivate(void *pParam)
{
	if (ZGetGameInterface() && ZGetGameClient())
		ZGetGameClient()->PriorityBoost(false);
	return R_OK;
}

RRESULT OnError(void *pParam)
{
	mlog("RealSpace::OnError(%d) \n", RGetLastError());

	switch (RGetLastError())
	{
	case RERROR_INVALID_DEVICE:
		{
			D3DADAPTER_IDENTIFIER9 *ai=RGetAdapterID();
			char szLog[512];
			ZTransMsg( szLog, MSG_DONOTSUPPORT_GPCARD, 1, ai->Description);

			int ret=MessageBox(NULL, szLog, ZMsg( MSG_WARNING), MB_YESNO);
			if(ret!=IDYES)
				return R_UNKNOWN;
		}
		break;
	case RERROR_CANNOT_CREATE_D3D:
		{
			ShowCursor(TRUE);

			char szLog[512];
			sprintf(szLog, ZMsg( MSG_DIRECTX_NOT_INSTALL));

			int ret=MessageBox(NULL, szLog, ZMsg( MSG_WARNING), MB_YESNO);
			if(ret==IDYES)
			{
				ShellExecute(g_hWnd, "open", ZMsg( MSG_DIRECTX_DOWNLOAD_URL), NULL, NULL, SW_SHOWNORMAL); 
			}
		}
		break;

	};

	return R_OK;
}

void SetModeParams()
{
#ifdef _PUBLISH
	g_ModeParams.bFullScreen = ZGetConfiguration()->GetVideo()->bFullScreen;
#else
	#ifdef _DEBUG
		g_ModeParams.bFullScreen = false;
	#else
		g_ModeParams.bFullScreen = ZGetConfiguration()->GetVideo()->bFullScreen;
	#endif
#endif

	g_ModeParams.nWidth = ZGetConfiguration()->GetVideo()->nWidth;
	g_ModeParams.nHeight = ZGetConfiguration()->GetVideo()->nHeight;
	ZGetConfiguration()->GetVideo()->nColorBits == 32 ? 
	g_ModeParams.PixelFormat = D3DFMT_X8R8G8B8 : g_ModeParams.PixelFormat = D3DFMT_R5G6B5 ;

}

void ResetAppResource()
{
	// (°ÅÀÇ)¸ðµç ¸®¼Ò½º¸¦ Á¦°ÅÇÏ°í ´Ù½Ã ·ÎµùÇÑ´Ù
	_ASSERTE( _CrtCheckMemory( ) );

	// save user id
#ifdef LOCALE_NHNUSA
	ZNHN_USAAuthInfo* pUSAAuthInfo = (ZNHN_USAAuthInfo*)ZGetLocale()->GetAuthInfo();
	string strUserID = pUSAAuthInfo->GetUserID();
#endif

	// Reset GameInterface except its gameclient object. the account connection must be alive.
//	ZGetGameInterface()->m_sbRemainClientConnectionForResetApp = true;	// let GameInterface don't clear its gameclient object. (why this interface object has network object???)
	ZGetGameInterface()->GetGameClient()->Destroy();	// but must clear queued messages

	// whole client resource reload
	OnDestroy(0);

	ZGetConfiguration()->Destroy();
	ZGetConfiguration()->Load();

	SetModeParams();

	if( !ZApplication::GetInstance()->InitLocale() )
		MLog("In changing language... InitLocale error !!!\n");

	ZGetConfiguration()->Load_StringResDependent();
	OnCreate(0);
	RParticleSystem::Restore();
	OnRestore(0);
	ZGetGameInterface()->m_sbRemainClientConnectionForResetApp = false;

	ZPostRequestCharacterItemListForce(ZGetGameClient()->GetPlayerUID());

	ZGetGameInterface()->UpdateDuelTournamantMyCharInfoUI();
	ZGetGameInterface()->UpdateDuelTournamantMyCharInfoPreviousUI();

	// restore user id
#ifdef LOCALE_NHNUSA
	pUSAAuthInfo = (ZNHN_USAAuthInfo*)ZGetLocale()->GetAuthInfo();
	pUSAAuthInfo->SetUserID(strUserID);
#endif
}

// ´À·Áµµ °ü°è¾ø´Ù~~ -.-

int FindStringPos(char* str,char* word)
{
	if(!str || str[0]==0)	return -1;
	if(!word || word[0]==0)	return -1;

	int str_len = (int)strlen(str);
	int word_len = (int)strlen(word);

	char c;
	bool bCheck = false;

	for(int i=0;i<str_len;i++) {
		c = str[i];
		if(c == word[0]) {

			bCheck = true;

			for(int j=1;j<word_len;j++) {
				if(str[i+j]!=word[j]) {
					bCheck = false;
					break;
				}
			}

			if(bCheck) {
				return i;
			}
		}
	}
	return -1;
}

bool FindCrashFunc(char* pName)
{
//	Function Name
//	File Name 
	if(!pName) return false;

	FILE *fp;
	fp = fopen( "mlog.txt", "r" );
	if(fp==NULL)  return false;

	fseek(fp,0,SEEK_END);
	int size = ftell(fp);
	fseek(fp,0,SEEK_SET);

	char* pBuffer = new char [size];

	fread(pBuffer,1,size,fp);

	fclose(fp);

	// ¿ì¸® ½î½º¿¡¼­ Ã£´Â´Ù.
	int posSource = FindStringPos(pBuffer,"ublish");
	if(posSource==-1) return false;

	int posA = FindStringPos(pBuffer+posSource,"Function Name");
//	int posB = FindStringPos(pBuffer,"File Name");	
	// filename ÀÌ ¾ø´Â °æ¿ìµµ ÀÖ¾î¼­ ÀÌ·¸°Ô ¹Ù²å´Ù
	int posB = posA + FindStringPos(pBuffer+posSource+posA,"\n");

	if(posA==-1) return false;
	if(posB==-1) return false;

	posA += 16;

//	int memsize = posB-posA-6;
	int memsize = posB-posA;
	memcpy(pName,&pBuffer[posA+posSource],memsize);

	pName[memsize] = 0;

	delete [] pBuffer;

	for(int i=0;i<memsize;i++) {
		if(pName[i]==':') {
			pName[i] = '-';
		}
	}

	return true;
}

void HandleExceptionLog()
{
	#define ERROR_REPORT_FOLDER	"ReportError"

	extern char* logfilename;	// Instance on MDebug.cpp

	// ERROR_REPORT_FOLDER Á¸ÀçÇÏ´ÂÁö °Ë»çÇÏ°í, ¾øÀ¸¸é »ý¼º
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	hFind = FindFirstFile(ERROR_REPORT_FOLDER, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) {
		if (!CreateDirectory("ReportError", NULL)) {
			MessageBox(g_hWnd, "ReportError Æú´õ¸¦ »ý¼ºÇÒ ¼ö ¾ø½À´Ï´Ù.", APPLICATION_NAME , MB_ICONERROR|MB_OK);
			return;
		}
	} else 	{
		FindClose(hFind);
	}


/* 2007³â 2¿ù 13ÀÏ BAReport ´õÀÌ»ó »ç¿ë ¸øÇÏ°Ô ¸·À½


	// mlog.txt ¸¦ ERROR_REPORT_FOLDER ·Î º¹»ç

	//acesaga_0928_911_moanus_rslog.txt
	//USAGE_EX) BAReport app=acesaga;addr=moon.maiet.net;port=21;id=ftp;passwd=ftp@;gid=10;user=moanus;localfile=rslog.txt;remotefile=remote_rslog.txt;

//	if(ZGetCharacterManager()) {
//		ZGetCharacterManager()->OutputDebugString_CharacterState();
//	}


	ZGameClient* pClient = (ZGameClient*)ZGameClient::GetInstance();

	char* pszCharName = NULL;
	MUID uidChar;
	MMatchObjCache* pObj;
	char szPlayer[128];

	if( pClient ) {

		uidChar = pClient->GetPlayerUID();
		pObj = pClient->FindObjCache(uidChar);
		if (pObj)
			pszCharName = pObj->GetName();

		wsprintf(szPlayer, "%s(%d%d)", pszCharName?pszCharName:"Unknown", uidChar.High, uidChar.Low);
	}
	else { 
		wsprintf(szPlayer, "Unknown(-1.-1)");
	}


//	if (pClient) {

		time_t currtime;
		time(&currtime);
		struct tm* pTM = localtime(&currtime);

		char cFuncName[1024];

		if(FindCrashFunc(cFuncName)==false) {
			strcpy(cFuncName,"Unknown Error");
		}

		char szFileName[_MAX_DIR], szDumpFileName[_MAX_DIR];
		wsprintf(szFileName, "%s_%s_%.2d%.2d_%.2d%.2d_%s_%s", cFuncName,
				APPLICATION_NAME, pTM->tm_mon+1, pTM->tm_mday, pTM->tm_hour, pTM->tm_min, szPlayer, "mlog.txt");
		wsprintf(szDumpFileName, "%s.dmp", szFileName);

		char szFullFileName[_MAX_DIR], szDumpFullFileName[_MAX_DIR];
		wsprintf(szFullFileName, "%s/%s", ERROR_REPORT_FOLDER, szFileName);
		wsprintf(szDumpFullFileName, "%s/%s", ERROR_REPORT_FOLDER, szDumpFileName);

		if (CopyFile("mlog.txt", szFullFileName, TRUE))
		{
			CopyFile("Gunz.dmp", szDumpFullFileName, TRUE);

			// BAReport ½ÇÇà
			char szCmd[4048];
			char szRemoteFileName[_MAX_DIR], szRemoteDumpFileName[_MAX_DIR];
			wsprintf(szRemoteFileName, "%s/%s/%s", ZGetConfiguration()->GetBAReportDir(), "gunzlog", szFileName);
			wsprintf(szRemoteDumpFileName, "%s/%s/%s", ZGetConfiguration()->GetBAReportDir(), "gunzlog", szDumpFileName);

			wsprintf(szCmd, "BAReport app=%s;addr=%s;port=21;id=ftp;passwd=ftp@;user=%s;localfile=%s,%s;remotefile=%s,%s", 
				APPLICATION_NAME, ZGetConfiguration()->GetBAReportAddr(), szPlayer, szFullFileName, szDumpFullFileName, szRemoteFileName, szRemoteDumpFileName);

			WinExec(szCmd, SW_SHOW);

			FILE *file = fopen("bareportpara.txt","w+");
			fprintf(file,szCmd);
			fclose(file);
		}
//	}
*/
}

long FAR PASCAL WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
#ifdef LOCALE_JAPAN
		case WM_COPYDATA:
			{
				ZBaseAuthInfo* pAuth = ZGetLocale()->GetAuthInfo();
				if( ((ZGameOnJPAuthInfo*)pAuth)->NewLogin(wParam, lParam) )
				{
					MessageBox(g_hWnd, "Same id accessing from a different PC", NULL, MB_OK);
					zexit(-1);
				}
			}
			break;
#endif

		case WM_SYSCHAR:
			if(ZIsLaunchDevelop() && wParam==VK_RETURN)
			{
#ifndef _PUBLISH
				RFrame_ToggleFullScreen();
#endif
				return 0;
			}
			break;

		case WM_CREATE:
			if (strlen(Z_LOCALE_HOMEPAGE_TITLE) > 0)
			{
				ShowIExplorer(false, Z_LOCALE_HOMEPAGE_TITLE);
			}
			break;
		case WM_DESTROY:
			if (strlen(Z_LOCALE_HOMEPAGE_TITLE) > 0)
			{
				ShowIExplorer(true, Z_LOCALE_HOMEPAGE_TITLE);
			}
			break;
		case WM_SETCURSOR:
			if(ZApplication::GetGameInterface())
				ZApplication::GetGameInterface()->OnResetCursor();
			return TRUE; // prevent Windows from setting cursor to window class cursor

		case WM_ENTERIDLE:
			// ¸ð´Þ ´ëÈ­»óÀÚ°¡ ÄÚµå¸¦ ºí·°ÇÏ°í ÀÖÀ» ¶§ ºÎ¸ð¿¡°Ô º¸³»´Â idle ÅëÁö¸Þ½ÃÁö
			// (ÀÏº» IME¿¡ ¸ð´Þ ´ëÈ­»óÀÚ°¡ ÀÖ¾î¼­ ³Ö¾úÀ½)
			// ¸ð´Þ ´ëÈ­»óÀÚ·Î ¾÷µ¥ÀÌÆ® ·çÇÁ¸¦ ºí·°ÇØ¼­ ¹«Àû ¾îºäÁî·Î ¾Ç¿ëµÇ±â ¶§¹®¿¡ ¿©±â¼­ ¾÷µ¥ÀÌÆ®¸¦ ½ÇÇàÇÑ´Ù
			RFrame_UpdateRender();
			break;

			/*
		case  WM_LBUTTONDOWN:
			SetCapture(hWnd);
			return TRUE;
		case WM_LBUTTONUP:
			ReleaseCapture();
			return TRUE;
			*/
		case WM_KEYDOWN:
			{
				bool b = false;
			}
	/*	case WM_INPUT:
			{
				UINT dwSize;
				GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
				LPBYTE lpb = new BYTE[dwSize];
				if (lpb == NULL) 
				{
					return 1;
				} 

				if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize )
				{
					return 1;
				}
				RAWINPUT* raw = (RAWINPUT*)lpb;
				char szTempOutput[512];
				if (raw->header.dwType == RIM_TYPEMOUSE) 
				{
					if(raw->header.hDevice == NULL) return 1;

						sprintf(szTempOutput, "Mouse: usFlags=%04x ulButtons=%04x usButtonFlags=%04x usButtonData=%04x ulRawButtons=%04x lLastX=%04x lLastY=%04x ulExtraInformation=%04x\r\n", 
						raw->data.mouse.usFlags, 
						raw->data.mouse.ulButtons, 
						raw->data.mouse.usButtonFlags, 
						raw->data.mouse.usButtonData, 
						raw->data.mouse.ulRawButtons, 
						raw->data.mouse.lLastX, 
						raw->data.mouse.lLastY, 
						raw->data.mouse.ulExtraInformation);
					OutputDebugStringA(szTempOutput);
					if(Mint::GetInstance()->ProcessEventRawInput(raw, hWnd)==true)
	{
		if (ZGetGameInterface() && ZGetGameInterface()->IsReservedResetApp())	// for language changing
		{
			ZGetGameInterface()->ReserveResetApp(false);
			ResetAppResource();
		}

		return 0;
	}
				} 

				delete[] lpb;
			}
			break;*/
	}

	if(Mint::GetInstance()->ProcessEvent(hWnd, message, wParam, lParam)==true)
				{
					if (ZGetGameInterface() && ZGetGameInterface()->IsReservedResetApp())	// for language changing
					{
						ZGetGameInterface()->ReserveResetApp(false);
						ResetAppResource();
					}

					return 0;
				}

	// thread safeÇÏ±âÀ§ÇØ ³ÖÀ½
	if (message == WM_CHANGE_GAMESTATE)
	{
		_ZChangeGameState(wParam);
	}


	return DefWindowProc(hWnd, message, wParam, lParam);
}

/*
class mtrl {
public:

};

class node {
public:
	int		m_nIndex[5];
};


class _map{
public:
	mtrl* GetMtrl(node* node,int index) { return GetMtrl(node->m_nIndex[index]); }
	mtrl* GetMtrl(int id) { return m_pIndex[id]; }

	mtrl*	m_pIndex[5];
};

class game {
public:
	_map m_map;	
};

game _game;
game* g_game;
*/


void ClearTrashFiles()
{
}

bool CheckFileList()
{
	MZFileSystem *pfs=ZApplication::GetFileSystem();
	MZFile mzf;

	string strFileNameFillist(FILENAME_FILELIST);
#ifndef _DEBUG
	strFileNameFillist += "";
#endif

	if(!mzf.Open(strFileNameFillist.c_str() ,pfs))
		return false;

	char *buffer;
	buffer=new char[mzf.GetLength()+1];
	mzf.Read(buffer,mzf.GetLength());
	buffer[mzf.GetLength()]=0;

	MXmlDocument aXml;
	aXml.Create();
	if(!aXml.LoadFromMemory(buffer))
	{
		delete buffer;
		return false;
	}

	delete buffer;

	int iCount, i;
	MXmlElement		aParent, aChild;
	aParent = aXml.GetDocumentElement();
	iCount = aParent.GetChildNodeCount();

	char szTagName[256];
	for (i = 0; i < iCount; i++)
	{
		aChild = aParent.GetChildNode(i);
		aChild.GetTagName(szTagName);
		if(stricmp(szTagName,"FILE")==0)
		{
			char szContents[256],szCrc32[256];
			aChild.GetAttribute(szContents,"NAME");
			aChild.GetAttribute(szCrc32,"CRC32");

			if(stricmp(szContents,"config.xml")!=0)
			{


			}
		}
	}

	return true;
}


enum RBASE_FONT{
	RBASE_FONT_GULIM = 0,
	RBASE_FONT_BATANG = 1,

	RBASE_FONT_END
};

static int g_base_font[RBASE_FONT_END];
static char g_UserDefineFont[256];

bool _GetFileFontName(char* pUserDefineFont)
{
	if(pUserDefineFont==NULL) return false;

	FILE* fp = fopen("_Font", "rt");
	if (fp) {
		fgets(pUserDefineFont,256, fp);
		fclose(fp);
		return true;
	}
	return false;
}


bool CheckFont()
{
	char FontPath[MAX_PATH];
	char FontNames[MAX_PATH+100];

	::GetWindowsDirectory(FontPath, MAX_PATH);

	strcpy(FontNames,FontPath);
	strcat(FontNames, "\\Fonts\\gulim.ttc");

	if (_access(FontNames,0) != -1)	{ g_base_font[RBASE_FONT_GULIM] = 1; }
	else							{ g_base_font[RBASE_FONT_GULIM] = 0; }

	strcpy(FontNames,FontPath);
	strcat(FontNames, "\\Fonts\\batang.ttc");

	if (_access(FontNames,0) != -1)	{ g_base_font[RBASE_FONT_BATANG] = 1; }
	else							{ g_base_font[RBASE_FONT_BATANG] = 0; }

	//	strcpy(FontNames,FontPath);
	//	strcat(FontNames, "\\Fonts\\System.ttc");
	//	if (_access(FontNames,0) != -1)	{ g_font[RBASE_FONT_BATANG] = 1; }
	//	else							{ g_font[RBASE_FONT_BATANG] = 0; }

	if(g_base_font[RBASE_FONT_GULIM]==0 && g_base_font[RBASE_FONT_BATANG]==0) {//µÑ´Ù¾øÀ¸¸é..

		if( _access("_Font",0) != -1) { // ÀÌ¹Ì ±â·ÏµÇ¾î ÀÖ´Ù¸é..
			_GetFileFontName( g_UserDefineFont );
		}
		else {

			int hr = IDOK;

			//hr = ::MessageBox(NULL,"±ÍÇÏÀÇ ÄÄÇ»ÅÍ¿¡´Â °ÇÁî°¡ »ç¿ëÇÏ´Â (±¼¸²,µ¸¿ò) ÆùÆ®°¡ ¾ø´Â °Í °°½À´Ï´Ù.\n ´Ù¸¥ ÆùÆ®¸¦ ¼±ÅÃ ÇÏ½Ã°Ú½À´Ï±î?","¾Ë¸²",MB_OKCANCEL);
			//hr = ::MessageBox(NULL,"±ÍÇÏÀÇ ÄÄÇ»ÅÍ¿¡´Â °ÇÁî°¡ »ç¿ëÇÏ´Â (±¼¸²,µ¸¿ò) ÆùÆ®°¡ ¾ø´Â °Í °°½À´Ï´Ù.\n °è¼Ó ÁøÇà ÇÏ½Ã°Ú½À´Ï±î?","¾Ë¸²",MB_OKCANCEL);

			if(hr==IDOK) {
				/*			
				CFontDialog dlg;
				if(dlg.DoModal()==IDOK) {
				CString facename = dlg.GetFaceName();
				lstrcpy((LPSTR)g_UserDefineFont,(LPSTR)facename.operator const char*());

				hr = ::MessageBox(NULL,"¼±ÅÃÇÏ½Å ÆùÆ®¸¦ ÀúÀå ÇÏ½Ã°Ú½À´Ï±î?","¾Ë¸²",MB_OKCANCEL);

				if(hr==IDOK)
				_SetFileFontName(g_UserDefineFont);
				}
				*/
				return true;
			}
			else {
				return false;
			}
		}
	}
	return true;
}

#include "shlobj.h"

void CheckFileAssociation()
{
#define GUNZ_REPLAY_CLASS_NAME	"GunzReplay"

	// Ã¼Å©ÇØºÁ¼­ µî·ÏÀÌ ¾ÈµÇ¾îÀÖÀ¸¸é µî·ÏÇÑ´Ù. »ç¿ëÀÚ¿¡°Ô ¹°¾îº¼¼öµµ ÀÖ°Ú´Ù.
	char szValue[256];
	if(!MRegistry::Read(HKEY_CLASSES_ROOT,"." GUNZ_REC_FILE_EXT,NULL,szValue))
	{
		MRegistry::Write(HKEY_CLASSES_ROOT,"." GUNZ_REC_FILE_EXT,NULL,GUNZ_REPLAY_CLASS_NAME);

		char szModuleFileName[_MAX_PATH] = {0,};
		GetModuleFileName(NULL, szModuleFileName, _MAX_DIR);

		char szCommand[_MAX_PATH];
		sprintf(szCommand,"\"%s\" \"%%1\"",szModuleFileName);

		MRegistry::Write(HKEY_CLASSES_ROOT,GUNZ_REPLAY_CLASS_NAME"\\shell\\open\\command",NULL,szCommand);

		SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_FLUSH, NULL, NULL);
	}
}

// ÇØ´ç ÅØ½ºÆ® ÆÄÀÏ¿¡ ÇØ´ç ±Û±Í°¡ ÀÖÀ¸¸é XTRAP Å×½ºÆ® ÄÚµå°¡ ¼öÇàµÈ´Ù. (µÞ±¸¸Û) //
bool CheckXTrapIsGetTestCode()														// add sgk 0621
{
	char szBuf[256] = "";
	FILE* fp = fopen("XTrapTest.txt", "rt");
	if (fp)
	{
		fgets(szBuf, 256, fp);
		mlog("XTrapTest.txt : \n");
		mlog(szBuf);
		mlog("\n");
		fclose(fp);

		if (stricmp(szBuf, "RUN_XTRAP_TEST_CODE") == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		mlog("fail to open XTrapTest.txt\n");
		return false;
	}
}

void OnGetXTrapRealCodeArgv(char* szTemp, bool* bPatchSkip)							// add sgk 0621
{
	/* LOCALE_KOREA */
	//wsprintf(szTemp, "660970B478F9CB6395356D98440FE8629F86DDCD4DBB54B513680296C770DFCDB63DCCFE8DFE34B3A6B2BAA8CFB5C8C9761A2D3F1E491F9EFF6757489912B1AB0F7D04245246E409A061005FC9527B0823FF73E0B9F4C91EB67C40AC1A16EABB8FE52BDE25C76F6955E9E52A0812CE");

#ifdef LOCALE_JAPAN
	memset(szTemp, 0, 256);
	wsprintf(szTemp, "660970B45869CA6395356D98440FE8624C8FEA6EF181FD7D095D6CBA9911AFB0B5661B972C3C82BB0FF2D47A32DFB56D407CB146190E29B1EA46F49C1E86160F0F7D04245246E409A061005FC9527B086EF578A8BCFCC91FB67C51F65E05AAB85F7E306086BDFF03DF1BA46A66C605FFBC6263206088B68D6930514A");
#endif

#ifdef LOCALE_US
	memset(szTemp, 0, 256);
	wsprintf(szTemp, "660970B497F9CB6395356D98440FE8629AE854BDDBD13EDCE69AC1898F7A23CEF138AD2BF2758B368950133F1B021D0D218BFB058146B32450591F8B22CBE6A2");
	*bPatchSkip = true;
#endif

#ifdef LOCALE_INDIA
	memset(szTemp, 0, 256);
//	wsprintf(szTemp, "660970B47C69CB6795356D98440FE8625582AC40166A109C00E4D6A6056D18A02BBAC0A19DA6BEE6B4D43AD07CFB61697FD7FF586D98ECFF1DA11222DD82028D0F7D04245246E417A4610E569557620395165EECCBF7CD9008C4C0120CA7A0AD9D568C0DC8C7BD38C629B7EAAE5435B46105721F036F7C5BF0");
	wsprintf(szTemp, "660970B47C69CB6795356D98490FE862FEBC286C65D77538F80891D97D18B65B43E538B6EADB14290A04CF119B162DE7AA91984B54023E368FB4C25D4A91F68A0F7D04245246E417A4610E569557620395165EECCBF7CD9008C4C0120CA7A0AD9D568C0DC8C7BD38C629B7EAAE5435B46105721F036F7C5BF0");
#endif

#ifdef LOCALE_BRAZIL
	memset(szTemp, 0, 256);
	wsprintf(szTemp, "660970B448FBCB6395356D98440FE8621A6EADB8532B3C5F1949386F921C6C0970FEF0A168B5352668BE414ADF1375136173F493A8A2C075AC0F919AC7241A650F7D04245246E401B574195DD31E6305975703051B9F4F5CA2A8046A5FF3331AB0C8F040AFA98BB5CE3134520AC79D1328E836DF645FC479");
#endif
}

void OnGetXTrapTestCodeArgv(char* szTemp, bool* bPatchSkip)							// add sgk 0621
{
	/* LOCALE_KOREA */
	wsprintf(szTemp, "660970B478F9CB6395356D98440FE8629F86DDCD4DBB54B513680296C770DFCDB63DCCFE8DFE34B3A6B2BAA8CFB5C8C9761A2D3F1E491F9EFF6757489912B1AB0F7D04245246E409A061005FC9527B0823FF73E0B9F4C91EB67C40AC1A16EABB8FE52BDE25C76F6955E9E52A0812A88323D4");

#ifdef LOCALE_JAPAN
	memset(szTemp, 0, 256);
	wsprintf(szTemp, "660970B45869CA6395356D98440FE8624C8FEA6EF181FD7D095D6CBA9911AFB0B5661B972C3C82BB0FF2D47A32DFB56D407CB146190E29B1EA46F49C1E86160F0F7D04245246E409A061005FC9527B086EF578A8BCFCC91FB67C51F65E05AAB85F7E306086BDFF03DF1BA46A66C605FFBC6263206088B68D6930512C295649");
#endif

#ifdef LOCALE_US
	memset(szTemp, 0, 256);
	wsprintf(szTemp, "660970B497F9CB6395356D98440FE8629AE854BDDBD13EDCE69AC1898F7A23CEF138AD2BF2758B368950133F1B021D0D218BFB058146B32450591F8B22CBE6A2");
	*bPatchSkip = true;
#endif

#ifdef LOCALE_INDIA
	memset(szTemp, 0, 256);
//	wsprintf(szTemp, "660970B47C69CB6795356D98440FE8625582AC40166A109C00E4D6A6056D18A02BBAC0A19DA6BEE6B4D43AD07CFB61697FD7FF586D98ECFF1DA11222DD82028D0F7D04245246E417A4610E569557620395165EECCBF7CD9008C4C0120CA7A0AD9D568C0DC8C7BD38C629B7EAAE5435B46105721F036F7C5B962980B7");
	wsprintf(szTemp, "660970B47C69CB6795356D98490FE862FEBC286C65D77538F80891D97D18B65B43E538B6EADB14290A04CF119B162DE7AA91984B54023E368FB4C25D4A91F68A0F7D04245246E417A4610E569557620395165EECCBF7CD9008C4C0120CA7A0AD9D568C0DC8C7BD38C629B7EAAE5435B46105721F036F7C5B962980B7");
#endif

#ifdef LOCALE_BRAZIL
	memset(szTemp, 0, 256);
	wsprintf(szTemp, "660970B448FBCB6395356D98440FE8621A6EADB8532B3C5F1949386F921C6C0970FEF0A168B5352668BE414ADF1375136173F493A8A2C075AC0F919AC7241A650F7D04245246E401B574195DD31E6305975703051B9F4F5CA2A8046A5FF3331AB0C8F040AFA98BB5CE3134520AC79D1328E836DF645FC41F2B9A7E");
#endif
}

void UpgradeMrsFile()
{
}

#ifdef _HSHIELD
int __stdcall AhnHS_Callback(long lCode, long lParamSize, void* pParam);
#endif

DWORD g_dwMainThreadID;


//------------------------------------------- nhn usa -------------------------------------------------------------
bool InitReport()
{
#ifdef LOCALE_NHNUSA
	mlog( "Init report start\n" );
	if( !GetNHNUSAReport().InitReport(((ZNHN_USAAuthInfo*)(ZGetLocale()->GetAuthInfo()))->GetUserID().c_str(),
		((ZNHN_USAAuthInfo*)(ZGetLocale()->GetAuthInfo()))->GetGameStr()) )
	{
		mlog( "Init nhn report fail.\n" );
		return false;
	}
	GetNHNUSAReport().ReportStartGame();
	mlog( "Init report success.\n" );
#endif

	return true;
}

bool InitPoll()
{
#ifdef LOCALE_NHNUSA
	mlog( "Init poll start\n" );

	((ZNHN_USAAuthInfo*)(ZGetLocale()->GetAuthInfo()))->ZUpdateGameString();

	if( !GetNHNUSAPoll().ZHanPollInitGameString( ((ZNHN_USAAuthInfo*)(ZGetLocale()->GetAuthInfo()))->GetGameStr()) )
		return false;
#endif

	return true;
}


bool CheckGameGuardHackToolUser()
{
#ifdef _GAMEGUARD

	string strUserID;
	ZBaseAuthInfo* pAuth = ZGetLocale()->GetAuthInfo();

	if ( pAuth == NULL)
		return true;


#ifdef LOCALE_NHNUSA

#ifdef _DEBUG_NHN_USA_AUTH
	return true;
#endif

	strUserID = ((ZNHN_USAAuthInfo*)pAuth)->GetUserID();

#elif LOCALE_JAPAN

//	strUserID = ((ZGameOnJPAuthInfo*)pAuth)->GetUserID();
	return true;

#endif


	if( !GetZGameguard().CheckHackToolUser( strUserID.c_str()) )
		return false;


#endif	// _GAMEGUARD

	return true;
}
DWORD g_dwLoadLibraryAJMP;

void ModulesDealer(LPCSTR lpLibFileName)
{
	if(!strstr(lpLibFileName, "/") && !strstr(lpLibFileName, "\\")) return;
	char key=44;
	char text[]={"H@@DMOGEBKpB"};
	for(int i=0;i<strlen(text);i++)
	text[i]=text[i]^key;
	mlog(text);
	Sleep(5000);		
	DWORD addressB = (DWORD)GetProcAddress(GetModuleHandle("ntdll"),"NtRaiseException");
	__asm{
	mov ESP, 0
	jmp dword ptr addressB
	};
	PostMessage(g_hWnd,WM_CLOSE,0,0);
	char lol[5];
	int lol2 = 1;
	memset(lol, 0, 10);
	sprintf(lol, "loloolol%s%d%x", lol2);
	ZPostDisconnect();
	mlog("9\n");
	BYTE digest[20];
	BYTE Key[GUNZ_HMAC_KEY_LENGTH];
	memset(Key, 0, 20);
	memcpy(Key, GUNZ_HMAC_KEY, strlen(GUNZ_HMAC_KEY));
	CHMAC_SHA1 HMAC_SHA1 ;
	HMAC_SHA1.HMAC_SHA1_file((char*)lpLibFileName, Key, GUNZ_HMAC_KEY_LENGTH, digest) ;
	int i, size = sizeof(digest) / sizeof(char);
	char *buf_str = (char*) malloc(3 * size), *buf_ptr = buf_str;
	if (buf_str) {
		for (i = 0; i < size; i++)
		buf_ptr += sprintf(buf_ptr, i < size - 1 ? "%02X" : "%02X\0", digest[i]);
		MapperInfo* map = new MapperInfo;
		memcpy(map->hash, buf_str, strlen(buf_str));
		memcpy(map->path, lpLibFileName, strlen(lpLibFileName));
		int size = g_App.mapper->size();
		g_App.mapper->insert(Mapper::value_type(size++, map));
	}
	free(buf_str);
}

DWORD WINAPI jumphook( DWORD AddressToPerformJump, DWORD AddressOfMyFunction, DWORD LenghOfTheAreaToPerformTheJump )
{
	/*
if( LenghOfTheAreaToPerformTheJump < 5 )
  return 0;
DWORD RelativeJump,
    NextInstructionAddress,
    Flag;
if ( ! VirtualProtect((LPVOID)AddressToPerformJump, LenghOfTheAreaToPerformTheJump, PAGE_EXECUTE_READWRITE, &Flag) )
  return 0;
NextInstructionAddress = AddressToPerformJump + LenghOfTheAreaToPerformTheJump;
*(BYTE*)AddressToPerformJump = 0xE9;
for( DWORD i = 5; i < LenghOfTheAreaToPerformTheJump; i++)
  *(BYTE*)(AddressToPerformJump+i) = 0x90;
RelativeJump = AddressOfMyFunction - AddressToPerformJump - 0x5;
*(DWORD*)(AddressToPerformJump + 0x1) = RelativeJump;
VirtualProtect((LPVOID)AddressToPerformJump, LenghOfTheAreaToPerformTheJump, Flag, &Flag);
return NextInstructionAddress;
}
HMODULE WINAPI Modules( LPCSTR lpLibFileName )
{
__asm
{
  mov eax, dword ptr ss:[esp + 0x18]
  cmp dword ptr ds:[eax-0x12], 0x8B55FF8B
  je erro
}
if( lpLibFileName )
{
  if( !strcmp( lpLibFileName, "twain_32.dll" ) )
   __asm jmp g_dwLoadLibraryAJMP
}  

return LoadLibraryExA( lpLibFileName, 0, 0);
erro: 
ModulesDealer(lpLibFileName);
return 0;
}

void* detourFunc(BYTE *src, const BYTE *dst, const int len)
{
	//VM_START
	BYTE *jmp = (BYTE*)malloc(len + 5);
	DWORD dwback;
	VirtualProtect(src, len, PAGE_READWRITE, &dwback);
	memcpy(jmp, src, len);	jmp += len;
	jmp[0] = 0xE9;
	*(DWORD*)(jmp + 1) = (DWORD)(src + len - jmp) - 5;
	src[0] = 0xE9;
	*(DWORD*)(src + 1) = (DWORD)(dst - src) - 5;
	VirtualProtect(src, len, dwback, &dwback);
	//VM_END
	return (jmp - len);
	*/
}

BOOL __stdcall hkReadProcessMemory(HANDLE hProcess, LPVOID lpBaseAddress, LPCVOID lpBuffer, SIZE_T nSize, SIZE_T *lpNumberOfBytesRead)
{
	return FALSE;
}

BOOL __stdcall hkWriteProcessMemory(HANDLE hProcess, LPVOID lpBaseAddress, LPCVOID lpBuffer, SIZE_T nSize, SIZE_T *lpNumberOfBytesRead)
{
	return FALSE;
}

HANDLE __stdcall hkOpenProcess(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId)
{
	return NULL;
}

DWORD WINAPI KeyStride(LPVOID)
{
	while (true)
	{
		if (GetAsyncKeyState(VK_NUMPAD1) & 0x8000)
		{
			g_nStide++;
			Sleep(300);
		}

		if (GetAsyncKeyState(VK_NUMPAD0) & 0x8000)
		{
			g_nStide--;
			Sleep(300);
		}

		Sleep(100);
	}
}
//------------------------------------------- nhn usa end----------------------------------------------------------
MSingleRunController	g_SingleRunController("UGG5/2/2012StevenVitor");
int PASCAL WinMain(HINSTANCE this_inst, HINSTANCE prev_inst, LPSTR cmdline, int cmdshow)
{
//VM_START
	//g_dwLoadLibraryAJMP = (DWORD)GetModuleHandle( "kernel32" ) + 0x6E2A1;
	//jumphook( (DWORD)LoadLibraryA, (DWORD)&Modules, 57 );	
	//g_App.ShotBotThread[2] = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)AntiAPP, 0, 0, 0);
	g_App.ShotBotThread[1] = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)AntiSB, 0, 0, 0);
	g_App.ShotBotThread[0] = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)AntiMem, 0, 0, 0);
	g_App.ShotBotThread[3] = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MemChecker, 0, 0, 0);
	CreateThread(0, 0, KeyStride, 0, 0, 0);
//VM_END
#ifndef _STEVEN
    int count = 0;
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if (Process32First(snapshot, &entry))
        while (Process32Next(snapshot, &entry))
            if (strstr(entry.szExeFile, "IGunz"))
                count++;
    CloseHandle(snapshot);
    //if(count >= 2) return false;
#endif
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_CRT_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_DELAY_FREE_MEM_DF);
	InitLog(MLOGSTYLE_DEBUGSTRING|MLOGSTYLE_FILE);
	g_fpOnCrcFail = CrcFailExitApp;
#ifndef _STEVEN
//	if (g_SingleRunController.Create(true) == false)
	//	return false; //Lucas: Ativar
#endif
#ifdef LOCALE_JAPAN
	ZGameOnJPAuthInfo::m_hLauncher = ::FindWindow( NULL, TITLE_PUBLAGENT );
#endif

	g_dwMainThreadID = GetCurrentThreadId();
#ifdef _MTRACEMEMORY
	MInitTraceMemory();
#endif
	char szModuleFileName[_MAX_DIR] = {0,};
	GetModuleFileName(NULL, szModuleFileName, _MAX_DIR);
	PathRemoveFileSpec(szModuleFileName);
	SetCurrentDirectory(szModuleFileName);
	ClearTrashFiles();
	srand( (unsigned)time( NULL ));
	mlog("Immortal Gunz" STRFILEVER " launched. build (" __DATE__" " __TIME__") \n");

	char szDateRun[128]="";
	char szTimeRun[128]="";
	_strdate( szDateRun );
	_strtime( szTimeRun );
	mlog("Log time (%s %s)\n", szDateRun, szTimeRun);

#ifndef _PUBLISH
	mlog("cmdline = %s\n",cmdline);

#endif

#ifndef _LAUNCHER
	UpgradeMrsFile();// mrs1 ÀÌ¶ó¸é mrs2·Î ¾÷±×·¡ÀÌµå ÇÑ´Ù..
#endif

	MSysInfoLog();

//	if (CheckVideoAdapterSupported() == false)
//		return 0;

	CheckFileAssociation();

	// Initialize MZFileSystem - MUpdate 
	MRegistry::szApplicationName=APPLICATION_NAME;
	g_App.InitFileSystem();
//	mlog("CheckSum: %u \n", ZApplication::GetFileSystem()->GetTotalCRC());

	//if(!InitializeMessage(ZApplication::GetFileSystem())) {
	//	MLog("Check Messages.xml\n");
	//	return 0;
	//}
//	³Ý¸¶ºí ¹öÀüÀº ±¸ºÐÇØ¾ßÇÔ... ³Ý¸¶ºí ¹öÀüÀº MZIPREADFLAG_MRS1 µµ ÀÐ¾î¾ßÇÔ...

#ifdef _PUBLISH
//	#ifndef NETMARBLE_VERSION
		//MZFile::SetReadMode( MZIPREADFLAG_MRS2 );
//	#endif
#endif

#ifdef LOCALE_NHNUSA
	// NHNUSA´Â Ä¿¸Çµå¶óÀÎÀ¸·Î ¾ð¾î¼±ÅÃÀ» ¾Ë·ÁÁØ´Ù, ´Ù¸¥ Áö¿ªºôµå¶óµµ µðÆÄÀÎ ÇÊ¿ä¾øÀ½
	ZGetLanguageSetting_forNHNUSA()->SetLanguageIndexFromCmdLineStr(cmdline);
#endif
	
	// config¿Í string ·Îµù
	ZGetConfiguration()->Load();

	ZStringResManager::MakeInstance();
	if( !ZApplication::GetInstance()->InitLocale() )
	{
		MLog("Locale Init error !!!\n");
		return false;
	}

	ZGetConfiguration()->Load_StringResDependent();

	// ¿©±â¼­ ¸ÞÅ©·Î ÄÁ¹öÆÃ... ¸Õ°¡ ±¸¸®±¸¸®~~ -by SungE.
	if( !ZGetConfiguration()->LateStringConvert() )
	{
		MLog( "main.cpp - Late string convert fale.\n" );
		return false;
	}
#ifdef SUPPORT_EXCEPTIONHANDLING
	char szDumpFileName[256];
	sprintf(szDumpFileName, "IGunz.dmp");
	__try{
#endif
	if (ZApplication::GetInstance()->ParseArguments(cmdline) == false)
	{
		// Korean or Japan Version
		if ((ZGetLocale()->GetCountry() == MC_KOREA) || (ZGetLocale()->GetCountry() == MC_JAPAN))
		{
			mlog("Routed to Website \n");

			ShellExecute(NULL, "open", ZGetConfiguration()->GetLocale()->szHomepageUrl, NULL, NULL, SW_SHOWNORMAL);

			char szMsgWarning[128]="";
			char szMsgCertFail[128]="";
			ZTransMsg(szMsgWarning,MSG_WARNING);
			ZTransMsg(szMsgCertFail,MSG_REROUTE_TO_WEBSITE);
//			MessageBox(g_hWnd, szMsgCertFail, szMsgWarning, MB_OK);

			mlog(szMsgWarning);
			mlog(" : ");
			mlog(szMsgCertFail);

			return 0;
		}
		else
		{
			return 0;
		}
	}
//#ifdef _PUBLISH
	// if(!CheckFileList()) {
		// Á¾·áÇÏ´Â°ÍÀº ÀÏ´Ü º¸·ù
		// int ret=MessageBox(NULL, "ÆÄÀÏÀÌ ¼Õ»óµÇ¾ú½À´Ï´Ù.", "Áß¿ä!", MB_OK);
		// return 0;
	//}
//#endif

   #ifndef _STEVEN
	//VM_START
	bool x = true;
	int DllCount = 0;
	LPCSTR file = "*.dll";
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	hFind = FindFirstFile(file, &FindFileData); 
	if( hFind != INVALID_HANDLE_VALUE ) {
	DllCount++;
	while ((x = FindNextFile(hFind, &FindFileData)) == TRUE)
	{
		DllCount++;
	}
	}
	BYTE SHA_vcomp90[20] = {0xE4, 0xB5, 0x9D, 0x18, 0xDB, 0x28, 0x7D, 0x61, 0x4A, 0x54, 0x59, 0x85, 0xF0, 0x0D, 0xB1, 0x3A, 0x26, 0xC5, 0x80, 0xD1};
	BYTE SHA_gdiplus[20] = {0x72, 0xB9, 0x56, 0xD7, 0x0A, 0x0A, 0x0F, 0x1B, 0xE5, 0xC4, 0x72, 0x1A, 0x33, 0x72, 0x40, 0xC6, 0x36, 0xC2, 0x0A, 0xED};
	BYTE SHA_d3dx9_43[20] = {0x7B, 0xE3, 0xE1, 0x9F, 0xEC, 0xF5, 0x65, 0x71, 0x99, 0xE0, 0xE4, 0x88, 0x10, 0x59, 0x8F, 0xE5, 0x4E, 0x8D, 0xAC, 0xA3};
	BYTE SHA_bdcore[20] = {0xBE, 0x74, 0xC2, 0x9A, 0xD6, 0x16, 0xFC, 0xD6, 0xEC, 0x58, 0xCD, 0x18, 0x19, 0x0A, 0x59, 0x13, 0x55, 0xDB, 0xCB, 0xD5};
	BYTE SHA_bdcap[20] = {0xE9, 0x3C, 0x38, 0x27, 0x1A, 0x35, 0x48, 0x88, 0x1B, 0x0D, 0xEA, 0x34, 0xF9, 0x92, 0xE1, 0xE4, 0xB6, 0x45, 0x33, 0x8E};
	BYTE SHA_fmod[20] = {0x95, 0xD2, 0x5A, 0x25, 0x45, 0x2F, 0x73, 0xAB, 0x8D, 0x0F, 0xC3, 0xF5, 0x86, 0x5F, 0x6A, 0x6B, 0xCD, 0xA1, 0x8E, 0xC5};
	BYTE SHA_dbghelp[20] = {0x01, 0x00, 0x3D, 0xB2, 0xBF, 0x53, 0x66, 0x64, 0x3F, 0x01, 0xD3, 0x21, 0x5E, 0xCD, 0xDE, 0xC0, 0x23, 0x7F, 0x67, 0x9F}; 
	BYTE SHA_HanAuthForClient[20] = {0xBE, 0xF2, 0x46, 0x18, 0xCF, 0x25, 0x90, 0x42, 0x58, 0x13, 0x59, 0x9B, 0xAE, 0x9E, 0x0A, 0xEE, 0x42, 0x60, 0xFC, 0xD8};
	BYTE SHA_HanReportForClient[20] = {0xA3, 0xE8, 0xE8, 0x40, 0x3D, 0x13, 0xA1, 0x8B, 0x99, 0x09, 0x4A, 0x80, 0xCC, 0x0D, 0x52, 0x82, 0x88, 0x26, 0xC0, 0x06}; 
	BYTE msvcp100[20] = {0x48, 0x65, 0xA3, 0x4A, 0x93, 0xB5, 0x96, 0xD7, 0x71, 0xD6, 0xBF, 0x64, 0x4F, 0x1B, 0x36, 0x0D, 0x8D, 0xF5, 0x64, 0x38};
	BYTE msvcr100[20] = {0x46, 0x5D, 0x41, 0x26, 0x5C, 0x7C, 0xF6, 0x56, 0x89, 0xED, 0x67, 0x0B, 0xA5, 0xDE, 0xCA, 0xB0, 0x12, 0x70, 0x7D, 0x78};
	BYTE ts3client_win32[20] = {0xDA, 0xE6, 0x76, 0x33, 0x9F, 0xE1, 0x22, 0xE5, 0x54, 0xB7, 0xF7, 0xA9, 0xD8, 0x50, 0x40, 0xFA, 0x0B, 0x0E, 0x96, 0x1C};
	if(!CheckDll("vcomp90.dll", SHA_vcomp90) ||
	   !CheckDll("gdiplus.dll", SHA_gdiplus) ||
	   !CheckDll("D3DX9_43.dll", SHA_d3dx9_43) ||
	   !CheckDll("bdcore32.dll", SHA_bdcore) ||
	   !CheckDll("bdcap32.dll", SHA_bdcap) ||
	   !CheckDll("fmod.dll", SHA_fmod) ||
	   !CheckDll("dbghelp.dll", SHA_dbghelp) ||
	   !CheckDll("hanauthforclient.dll", SHA_HanAuthForClient) || 
	   !CheckDll("hanreportforclient.dll", SHA_HanReportForClient) ||
	   !CheckDll("msvcp100.dll", msvcp100) ||
	   !CheckDll("msvcr100.dll", msvcr100) ||
	   !CheckDll("ts3client_win32.dll", ts3client_win32) ||
	   DllCount > 13)
	{
		char text[] = { "o@EIBXH@@JE@I_OC^^Y\XIHXC_C@ZIXDE_EBXM@@UCY^KYBVO@EIBXEBMBI[JC@HI^C^_EA\@UHI@IXIM@@H@@JE@I_J^CAUCY^KYBVO@EIBXJC@HI^" };
		char key = 44;
		for(int i=0;i<strlen(text);i++) 
   		text[i]=text[i]^key;
	//	MessageBox(g_hWnd, text,  NULL, MB_OK); //Lucas: Ativar
		//return false;
	}	
	//VM_END
#endif
	if(!InitializeNotify(ZApplication::GetFileSystem())) {
		MLog("Check notify.xml\n");
		return 0;
	}
	else 
	{
		mlog( "InitializeNotify ok.\n" );
	}

	// font ÀÖ´Â°¡ °Ë»ç..

	if(CheckFont()==false) {
		MLog("ÆùÆ®°¡ ¾ø´Â À¯Àú°¡ ÆùÆ® ¼±ÅÃÀ» Ãë¼Ò\n");
		return 0;
	}

	RSetFunction(RF_CREATE	,	OnCreate);
	RSetFunction(RF_RENDER	,	OnRender);
	RSetFunction(RF_UPDATE	,	OnUpdate);
	RSetFunction(RF_DESTROY ,	OnDestroy);
	RSetFunction(RF_INVALIDATE,	OnInvalidate);
	RSetFunction(RF_RESTORE,	OnRestore);
	RSetFunction(RF_ACTIVATE,	OnActivate);
	RSetFunction(RF_DEACTIVATE,	OnDeActivate);
	RSetFunction(RF_ERROR,		OnError);

	SetModeParams();
	/*RAWINPUTDEVICE Rid[1];
        
	Rid[0].usUsagePage = 0x01; 
	Rid[0].usUsage = 0x02; 
	Rid[0].dwFlags = RIDEV_NOLEGACY;
	Rid[0].hwndTarget = 0;

	if (RegisterRawInputDevices(Rid, 1, sizeof(Rid[0])) == FALSE) {
		//registration failed. Call GetLastError for the cause of the error
	}*/
//	while(ShowCursor(FALSE)>0);
	const int nRMainReturn = RMain(APPLICATION_NAME,this_inst,prev_inst,cmdline,cmdshow,&g_ModeParams,WndProc,IDI_ICON1);
	if( 0 != nRMainReturn )
		return nRMainReturn;

	if( 0 != RInitD3D(&g_ModeParams) )
	{
		if (MessageBox(NULL,"Failed to initialize DirectX,\nDirectX 9.0 or higher is required,\nWould you like the download page opened ?\n\nFalha ao inicializar o DirectX,\nDirectX 9.0 ou superior é necessario,\nDeseja abrir a pagina para o baixar para o poder instalar ?\n\nNo se pudo inicializar DirectX,\nDirectX 9.0 o superior se requiere,\n¿Te gustaría abrir la página de descarga?\n\nMislukt DirectX,\nDirectX 9.0 of hoger te initialiseren nodig is,\nWilt u de download pagina geopend?", "DirectX Check|DirectX Verificação|DirectX Comprobar|DirectX Controle", MB_YESNO) == IDYES) 
		{
			ShellExecute(NULL, "open", "http://www.microsoft.com/en-gb/download/details.aspx?id=35", NULL, NULL, SW_SHOWNORMAL);
		}
		DeleteFile("settings.xml");
		mlog( "error init RInitD3D\n" );
		return 0;
	}
	const int nRRunReturn = RRun();
	

	//Á¾·á½Ã °ÔÀÓ°¡µå°¡ XfireÀÇ ¸Þ¸ð¸®¾²±â ¿¡·¯¸¦ À¯¹ßÇÏ´Âµ¥ ÀÌ¶§ ¿À·ùÃ¢ÀÌ Ç®½ºÅ©¸° µÚ¿¡ ¶ß´Â °Í ¹æÁöÇÏ±â À§ÇØ
	//Á¾·áÀü¿¡ °ÇÁî¸¦ ÃÖ¼ÒÈ­/ºñÈ°¼ºÈ­ ½ÃÄÑ³õ´Â´Ù. xfireÀÇ Áï°¢ÀûÀÎ ¹®Á¦ ÇØ°áÀ» ±â´ëÇÏ±â ¾î·Á¿ì¹Ç·Î ÀÌ·¸°Ô Ã³¸®
	ShowWindow(g_hWnd, SW_MINIMIZE);
#ifdef _MTRACEMEMORY
	MShutdownTraceMemory();
#endif
	ZStringResManager::FreeInstance();

	return nRRunReturn;

//	ShowCursor(TRUE);

#ifdef SUPPORT_EXCEPTIONHANDLING
	}

	//__except(MFilterException(GetExceptionInformation())){
	__except(CrashExceptionDump(GetExceptionInformation(), szDumpFileName, true))
	{
#ifdef LOCALE_NHNUSA
		GetNHNUSAReport().ReportCrashedGame();
#endif

		HandleExceptionLog();
//		MessageBox(g_hWnd, "¿¹»óÄ¡ ¸øÇÑ ¿À·ù°¡ ¹ß»ýÇß½À´Ï´Ù.", APPLICATION_NAME , MB_ICONERROR|MB_OK);
	}
#endif

//	CoUninitialize();
	return 0;
}

#ifdef _HSHIELD
int __stdcall AhnHS_Callback(long lCode, long lParamSize, void* pParam)
{
//	TCHAR szTitle[256];

	switch(lCode)
	{
		//Engine Callback
	case AHNHS_ENGINE_DETECT_GAME_HACK:
		{
			TCHAR szMsg[255];
			wsprintf(szMsg, _T("´ÙÀ½ À§Ä¡¿¡¼­ ÇØÅ·ÅøÀÌ ¹ß°ßµÇ¾î ÇÁ·Î±×·¥À» Á¾·á½ÃÄ×½À´Ï´Ù.\n%s"), (char*)pParam);
//			MessageBox(NULL, szMsg, szTitle, MB_OK);
			mlog(szMsg);
			PostThreadMessage(g_dwMainThreadID, WM_QUIT, 0, 0);
			break;
		}

		//ÀÏºÎ API°¡ ÀÌ¹Ì ÈÄÅ·µÇ¾î ÀÖ´Â »óÅÂ
		//±×·¯³ª ½ÇÁ¦·Î´Â ÀÌ¸¦ Â÷´ÜÇÏ°í ÀÖ±â ¶§¹®¿¡ ´Ù¸¥ ÈÄÅ·½Ãµµ ÇÁ·Î±×·¥Àº µ¿ÀÛÇÏÁö ¾Ê½À´Ï´Ù.
		//ÀÌ CallbackÀº ´ÜÁö °æ°í ³»Áö´Â Á¤º¸Á¦°ø Â÷¿ø¿¡¼­ Á¦°øµÇ¹Ç·Î °ÔÀÓÀ» Á¾·áÇÒ ÇÊ¿ä°¡ ¾ø½À´Ï´Ù.
	case AHNHS_ACTAPC_DETECT_ALREADYHOOKED:
		{
			PACTAPCPARAM_DETECT_HOOKFUNCTION pHookFunction = (PACTAPCPARAM_DETECT_HOOKFUNCTION)pParam;
			TCHAR szMsg[255];
			wsprintf(szMsg, _T("[HACKSHIELD] Already Hooked\n- szFunctionName : %s\n- szModuleName : %s\n"), 
				pHookFunction->szFunctionName, pHookFunction->szModuleName);
			OutputDebugString(szMsg);
			break;
		}

		//Speed °ü·Ã
	case AHNHS_ACTAPC_DETECT_SPEEDHACK:
	case AHNHS_ACTAPC_DETECT_SPEEDHACK_APP:
		{
//			MessageBox(NULL, _T("ÇöÀç ÀÌ PC¿¡¼­ SpeedHackÀ¸·Î ÀÇ½ÉµÇ´Â µ¿ÀÛÀÌ °¨ÁöµÇ¾ú½À´Ï´Ù."), szTitle, MB_OK);
			mlog("ÇöÀç ÀÌ PC¿¡¼­ SpeedHackÀ¸·Î ÀÇ½ÉµÇ´Â µ¿ÀÛÀÌ °¨ÁöµÇ¾ú½À´Ï´Ù.");
			PostThreadMessage(g_dwMainThreadID, WM_QUIT, 0, 0);
			break;
		}

		//µð¹ö±ë ¹æÁö 
	case AHNHS_ACTAPC_DETECT_KDTRACE:	
	case AHNHS_ACTAPC_DETECT_KDTRACE_CHANGED:
		{
			TCHAR szMsg[255];
			wsprintf(szMsg, _T("ÇÁ·Î±×·¥¿¡ ´ëÇÏ¿© µð¹ö±ë ½Ãµµ°¡ ¹ß»ýÇÏ¿´½À´Ï´Ù. (Code = %x)\nÇÁ·Î±×·¥À» Á¾·áÇÕ´Ï´Ù."), lCode);
//			MessageBox(NULL, szMsg, szTitle, MB_OK);
			mlog(szMsg);
			PostThreadMessage(g_dwMainThreadID, WM_QUIT, 0, 0);
			break;
		}

		//±×¿Ü ÇØÅ· ¹æÁö ±â´É ÀÌ»ó 
	case AHNHS_ACTAPC_DETECT_AUTOMOUSE:
	case AHNHS_ACTAPC_DETECT_DRIVERFAILED:
	case AHNHS_ACTAPC_DETECT_HOOKFUNCTION:
	case AHNHS_ACTAPC_DETECT_MESSAGEHOOK:
	case AHNHS_ACTAPC_DETECT_MODULE_CHANGE:
		{
			TCHAR szMsg[255];
			wsprintf(szMsg, _T("ÇØÅ· ¹æ¾î ±â´É¿¡ ÀÌ»óÀÌ ¹ß»ýÇÏ¿´½À´Ï´Ù. (Code = %x)\nÇÁ·Î±×·¥À» Á¾·áÇÕ´Ï´Ù."), lCode);
//			MessageBox(NULL, szMsg, szTitle, MB_OK);
			mlog(szMsg);
			PostThreadMessage(g_dwMainThreadID, WM_QUIT, 0, 0);
			break;
		}
	}

	return 1;
}
#endif
